#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <algorithm>
#include <array>
#include <cmath>

/**
 * Stereo peak meter: two vertical bars on a -60..0 dBFS scale (linear in dB),
 * green below -12 dB, yellow up to -3 dB, red above. The bar attacks instantly and
 * falls at kFallDbPerSec; a thin tick holds the recent peak for kPeakHoldMs and then
 * falls at kPeakFallDbPerSec. A CLIP box latches for kClipHoldMs after the processor
 * reports clipping, and a click on it clears it.
 *
 * All time-dependent state advances only through update(), which takes an explicit
 * timestamp, so the ballistics can be driven deterministically from a test.
 */
class LevelMeter : public juce::Component {
public:
    static constexpr float kFloorDb = -60.0f;
    static constexpr float kGreenTopDb = -12.0f;
    static constexpr float kYellowTopDb = -3.0f;
    static constexpr float kFallDbPerSec = 24.0f;
    static constexpr float kPeakFallDbPerSec = 12.0f;
    static constexpr double kPeakHoldMs = 1000.0;
    static constexpr double kClipHoldMs = 2000.0;

    /** Linear peak (0..1) to dBFS, floored at kFloorDb. */
    static float toDb(double linear) {
        if (!(linear > 0.0)) {
            return kFloorDb;
        }
        return std::max(kFloorDb, static_cast<float>(20.0 * std::log10(linear)));
    }

    /** Feed peaks (linear) measured since the previous call. nowMs is a monotonic clock. */
    void update(double leftPeak, double rightPeak, bool clipped, double nowMs) {
        const double dt = lastMs_ < 0.0 ? 0.0 : std::max(0.0, (nowMs - lastMs_) / 1000.0);
        lastMs_ = nowMs;
        const std::array<float, 2> input{toDb(leftPeak), toDb(rightPeak)};
        for (std::size_t ch = 0; ch < 2; ++ch) {
            float& level = levelDb_[ch];
            float& peak = peakDb_[ch];
            if (input[ch] >= level) {
                level = input[ch];
            } else {
                level = std::max(input[ch], level - kFallDbPerSec * static_cast<float>(dt));
            }
            if (level >= peak) {
                peak = level;
                peakHoldUntilMs_[ch] = nowMs + kPeakHoldMs;
            } else if (nowMs >= peakHoldUntilMs_[ch]) {
                peak = std::max(level, peak - kPeakFallDbPerSec * static_cast<float>(dt));
            }
        }
        if (clipped) {
            clip_ = true;
            clipUntilMs_ = nowMs + kClipHoldMs;
        } else if (clip_ && nowMs >= clipUntilMs_) {
            clip_ = false;
        }
        repaint();
    }

    float levelDb(int channel) const { return levelDb_[static_cast<std::size_t>(channel & 1)]; }
    float peakDb(int channel) const { return peakDb_[static_cast<std::size_t>(channel & 1)]; }
    bool clipLatched() const { return clip_; }
    void clearClip() {
        clip_ = false;
        repaint();
    }

    void mouseDown(const juce::MouseEvent& e) override {
        if (clipBox().contains(e.getPosition())) {
            clearClip();
        }
    }

    void paint(juce::Graphics& g) override {
        const auto track = juce::Colour(0xff12141a);
        const auto marker = juce::Colour(0xffe8ecf5);

        const auto box = clipBox();
        g.setColour(clip_ ? juce::Colour(0xffe53935) : juce::Colour(0xff3a2224));
        g.fillRect(box);
        g.setColour(clip_ ? marker : juce::Colour(0xff6a5658));
        g.setFont(juce::Font(juce::FontOptions(9.0f, juce::Font::bold)));
        g.drawText("CLIP", box, juce::Justification::centred, false);

        for (int ch = 0; ch < 2; ++ch) {
            const auto bar = barBounds(ch);
            g.setColour(track);
            g.fillRect(bar);
            const float level = levelDb(ch);
            drawSegment(g, bar, kFloorDb, std::min(level, kGreenTopDb), juce::Colour(0xff43a047));
            if (level > kGreenTopDb) {
                drawSegment(g, bar, kGreenTopDb, std::min(level, kYellowTopDb), juce::Colour(0xfffdd835));
            }
            if (level > kYellowTopDb) {
                drawSegment(g, bar, kYellowTopDb, level, juce::Colour(0xffe53935));
            }
            if (peakDb(ch) > kFloorDb) {
                const float py = dbToY(peakDb(ch), bar);
                g.setColour(marker);
                g.fillRect(static_cast<float>(bar.getX()), std::max(py - 1.0f, static_cast<float>(bar.getY())),
                           static_cast<float>(bar.getWidth()), 2.0f);
            }
        }
    }

private:
    static constexpr int kMargin = 3;
    static constexpr int kBarWidth = 11;
    static constexpr int kBarHeight = 80;
    static constexpr int kBarGap = 4;
    static constexpr int kClipHeight = 12;

public:
    /** Preferred component size: a CLIP box above two vertical bars. */
    static constexpr int kPreferredWidth = 2 * kBarWidth + kBarGap + 2 * kMargin;
    static constexpr int kPreferredHeight = kClipHeight + kBarHeight + 3 * kMargin;

private:
    juce::Rectangle<int> barBounds(int channel) const {
        return {kMargin + channel * (kBarWidth + kBarGap), 2 * kMargin + kClipHeight, kBarWidth, kBarHeight};
    }

    juce::Rectangle<int> clipBox() const { return {kMargin, kMargin, getWidth() - 2 * kMargin, kClipHeight}; }

    static float dbToY(float db, juce::Rectangle<int> bar) {
        const float t = (std::clamp(db, kFloorDb, 0.0f) - kFloorDb) / (0.0f - kFloorDb);
        return static_cast<float>(bar.getBottom()) - t * static_cast<float>(bar.getHeight());
    }

    static void drawSegment(juce::Graphics& g, juce::Rectangle<int> bar, float fromDb, float toDb, juce::Colour colour) {
        const float y1 = dbToY(fromDb, bar);
        const float y0 = dbToY(toDb, bar);
        if (y1 > y0) {
            g.setColour(colour);
            g.fillRect(static_cast<float>(bar.getX()), y0, static_cast<float>(bar.getWidth()), y1 - y0);
        }
    }

    std::array<float, 2> levelDb_{kFloorDb, kFloorDb};
    std::array<float, 2> peakDb_{kFloorDb, kFloorDb};
    std::array<double, 2> peakHoldUntilMs_{0.0, 0.0};
    double lastMs_ = -1.0;
    double clipUntilMs_ = 0.0;
    bool clip_ = false;
};
