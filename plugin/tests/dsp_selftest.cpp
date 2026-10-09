// No-JUCE DSP self-tests: block-size invariance and basic safety. Mirrors the
// Python tests/test_block_invariance.py tolerances.
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <vector>

#include "snakeoil/effects.hpp"
#include "snakeoil/engine.hpp"

namespace {

struct XorShift {
    std::uint64_t s = 0x9e3779b97f4a7c15ULL;
    double uniform() {
        s ^= s << 13;
        s ^= s >> 7;
        s ^= s << 17;
        return static_cast<double>(s >> 11) / static_cast<double>(1ULL << 53) - 0.5;
    }
};

double maxAbsDiff(const std::vector<float>& a, const std::vector<float>& b) {
    double worst = 0.0;
    for (std::size_t i = 0; i < a.size(); ++i) {
        worst = std::max(worst, std::fabs(static_cast<double>(a[i]) - static_cast<double>(b[i])));
    }
    return worst;
}

std::vector<float> renderEngine(int total, int block) {
    // An Engine is ~0.6 MB; keep it off the stack (MSVC's default stack is 1 MB).
    const auto enginePtr = std::make_unique<snakeoil::Engine>(44100.0, block, 4);
    snakeoil::Engine& engine = *enginePtr;
    engine.setParamById("osc1_level", 1.0);
    engine.setParamById("osc2_level", 0.5);
    for (const char* fx : {"fx_chorus", "fx_delay", "fx_reverb", "fx_bitcrush"}) {
        engine.setParamById(fx, 1.0);
    }
    engine.setParamById("fx_chorus_depth", 1.0);
    engine.setParamById("fx_delay_time", 200.0);
    engine.setParamById("fx_delay_pingpong", 1.0);
    engine.noteOn(48, 100);
    engine.noteOn(55, 100);
    engine.noteOn(64, 100);
    std::vector<float> out;
    int done = 0;
    std::vector<float> buffer(static_cast<std::size_t>(block) * 2);
    while (done < total) {
        const int n = std::min(block, total - done);
        engine.render(buffer.data(), n);
        out.insert(out.end(), buffer.begin(), buffer.begin() + static_cast<std::size_t>(n) * 2);
        done += n;
    }
    return out;
}

std::vector<float> renderChain(int total, int block) {
    snakeoil::EffectChain chain(44100.0);
    chain.chorus().setEnabled(true);
    chain.chorus().setDepth(1.0);
    chain.delay().setEnabled(true);
    chain.delay().setTimeMs(200.0);
    chain.delay().setPingpong(true);
    chain.reverb().setEnabled(true);
    chain.bitcrush().setEnabled(true);
    XorShift rng;
    std::vector<float> out;
    std::vector<double> l(static_cast<std::size_t>(block));
    std::vector<double> r(static_cast<std::size_t>(block));
    int done = 0;
    while (done < total) {
        const int n = std::min(block, total - done);
        for (int i = 0; i < n; ++i) {
            l[static_cast<std::size_t>(i)] = rng.uniform();
            r[static_cast<std::size_t>(i)] = rng.uniform();
        }
        chain.process(l.data(), r.data(), n);
        for (int i = 0; i < n; ++i) {
            out.push_back(static_cast<float>(l[static_cast<std::size_t>(i)]));
            out.push_back(static_cast<float>(r[static_cast<std::size_t>(i)]));
        }
        done += n;
    }
    return out;
}

bool finite(const std::vector<float>& x) {
    for (float v : x) {
        if (!std::isfinite(v)) {
            return false;
        }
    }
    return true;
}

// Mirrors tests/test_tail_slots.py: with tail slots the oldest held note is force-released
// into a short tail instead of being hard-stolen, and the pool never exceeds its capacity.
int checkHybridAllocation() {
    int failures = 0;
    const auto fail = [&](const char* what) {
        std::printf("FAIL hybrid allocation: %s\n", what);
        ++failures;
    };
    const int block = 256;
    std::vector<float> buffer(static_cast<std::size_t>(block) * 2);
    const auto make = [&](int slots) {
        auto e = std::make_unique<snakeoil::Engine>(44100.0, block, snakeoil::kMaxVoices, slots, 12);
        e->setParamById("amp_release", 3.0);
        return e;
    };

    auto ptr = make(6);
    snakeoil::Engine& e = *ptr;
    if (e.tailSlots() != 6 || e.tailCapacity() != 12 || e.tailCount() != 0) {
        fail("initial slots/capacity/count");
    }
    for (int i = 0; i < 12; ++i) {
        e.noteOn(48 + i, 100);
    }
    e.render(buffer.data(), block);
    e.noteOff(48);
    e.noteOff(49);
    e.render(buffer.data(), block);
    if (e.tailCount() != 2) {
        fail("released notes are not ringing as tails");
    }
    for (int i = 0; i < 6; ++i) {
        e.noteOn(70 + i, 100);
    }
    e.render(buffer.data(), block);
    // 12 playable + 6 tail slots: the two released notes keep ringing, nothing is cut short.
    if (e.tailCount() < 2) {
        fail("first-released notes were stolen instead of ringing on");
    }
    if (e.activeVoices() > snakeoil::kMaxVoices + e.tailSlots()) {
        fail("active voices exceed playable + tail slots");
    }
    for (int i = 0; i < 40; ++i) {
        e.noteOn(30 + i, 100);
        if (i % 2 == 0) {
            e.noteOff(30 + i);
        }
        e.render(buffer.data(), block);
        if (e.activeVoices() > snakeoil::kMaxVoices + e.tailSlots()) {
            fail("active voices exceed playable + tail slots under load");
            break;
        }
        for (float v : buffer) {
            if (!std::isfinite(v) || std::fabs(v) > 4.0f) {
                fail("non-finite or runaway output under load");
                i = 40;
                break;
            }
        }
    }

    // Slot count: clamped to the capacity, never cuts sounding voices.
    if (e.setTailSlots(99) != 12 || e.tailSlots() != 12 || e.setTailSlots(-3) != 0 || e.setTailSlots(6) != 6) {
        fail("setTailSlots clamping");
    }
    auto classicPtr = make(0);
    if (classicPtr->setTailSlots(6) != 6) {
        fail("capacity 12 should allow enabling tails on a classic start");
    }
    classicPtr->setTailSlots(0);
    const int before = e.activeVoices();
    e.setTailSlots(0);
    if (e.activeVoices() != before) {
        fail("shrinking the slots cut sounding voices");
    }
    return failures;
}

}  // namespace

int main() {
    int failures = checkHybridAllocation();

    // Effects must be block-size invariant (Python tolerance 1e-9).
    const auto chainRef = renderChain(8192, 8192);
    for (int block : {64, 256, 1024, 4096}) {
        const double diff = maxAbsDiff(chainRef, renderChain(8192, block));
        if (diff > 1e-9) {
            std::printf("FAIL effect block invariance at %d (%.3e)\n", block, diff);
            ++failures;
        }
    }

    // Engine must be block-size invariant (Python tolerance 1e-5).
    const auto engineRef = renderEngine(8192, 8192);
    if (!finite(engineRef)) {
        std::printf("FAIL engine produced non-finite output\n");
        ++failures;
    }
    for (int block : {64, 256, 1024, 4096}) {
        const double diff = maxAbsDiff(engineRef, renderEngine(8192, block));
        if (diff > 1e-5) {
            std::printf("FAIL engine block invariance at %d (%.3e)\n", block, diff);
            ++failures;
        }
    }

    // Silence in must stay silent out.
    const auto quietPtr = std::make_unique<snakeoil::Engine>(44100.0, 256);
    snakeoil::Engine& quiet = *quietPtr;
    std::vector<float> buffer(512);
    quiet.render(buffer.data(), 256);
    for (float v : buffer) {
        if (v != 0.0f) {
            std::printf("FAIL idle engine is not silent\n");
            ++failures;
            break;
        }
    }

    if (failures == 0) {
        std::printf("PASS dsp_selftest\n");
        return 0;
    }
    return 1;
}
