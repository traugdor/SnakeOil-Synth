// Offscreen layout check and PNG snapshots for the plugin editor.
//
//   EditorSnapshot --check            verify the layout invariants, exit non-zero on failure
//   EditorSnapshot --png <dir>        write editor_default.png and editor_full.png
//
// The tool only inspects the public component tree: groups and controls are the
// children of the editor (or of the Viewport's content component), in creation order,
// each GroupComponent followed by the controls that belong to it.

#include <juce_audio_utils/juce_audio_utils.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "params_gen.hpp"

namespace {

constexpr int kHeaderHeight = 28;
constexpr int kMinKnobArea = 56;

struct Group {
    juce::GroupComponent* box = nullptr;
    std::vector<juce::Component*> controls;
};

bool isControl(const juce::Component& c) {
    return dynamic_cast<const juce::Slider*>(&c) != nullptr ||
           dynamic_cast<const juce::Button*>(&c) != nullptr ||
           dynamic_cast<const juce::ComboBox*>(&c) != nullptr;
}

std::string describe(const juce::Component& c) {
    std::string kind = "component";
    if (dynamic_cast<const juce::Slider*>(&c) != nullptr) kind = "slider";
    else if (dynamic_cast<const juce::Button*>(&c) != nullptr) kind = "toggle";
    else if (dynamic_cast<const juce::ComboBox*>(&c) != nullptr) kind = "combo";
    else if (dynamic_cast<const juce::Label*>(&c) != nullptr) kind = "label";
    const auto b = c.getBounds();
    return kind + " '" + c.getName().toStdString() + "' [" + std::to_string(b.getX()) + "," +
           std::to_string(b.getY()) + " " + std::to_string(b.getWidth()) + "x" +
           std::to_string(b.getHeight()) + "]";
}

struct Inspector {
    juce::AudioProcessorEditor& editor;
    juce::Viewport* viewport = nullptr;
    juce::Component* root = nullptr;
    std::vector<Group> groups;

    explicit Inspector(juce::AudioProcessorEditor& e) : editor(e) {
        for (auto* child : editor.getChildren()) {
            if (auto* vp = dynamic_cast<juce::Viewport*>(child)) {
                viewport = vp;
            }
        }
        root = viewport != nullptr ? viewport->getViewedComponent() : &editor;
        for (auto* child : root->getChildren()) {
            if (auto* box = dynamic_cast<juce::GroupComponent*>(child)) {
                groups.push_back({box, {}});
            } else if (!groups.empty()) {
                if (auto* label = dynamic_cast<juce::Label*>(child); label != nullptr && label->getText().isEmpty()) {
                    continue;  // not part of any group (the old editor's meter label)
                }
                groups.back().controls.push_back(child);
            }
        }
    }
};

// Expected placement, as in the Python app's GROUP_POSITIONS: title, grid row, first column, columns spanned.
struct Placement {
    const char* title;
    int row;
    int col;
    int span;
};
const Placement kPlacements[] = {
    {"Oscillator 1", 0, 0, 1}, {"Oscillator 2", 0, 1, 1}, {"Modulation", 0, 2, 1}, {"Master", 0, 3, 1},
    {"Tempo", 0, 4, 1},        {"Noise", 0, 4, 1},        {"Filter", 1, 0, 1},     {"Filter Env", 1, 1, 1},
    {"Amp Envelope", 1, 2, 1}, {"LFO", 1, 3, 1},          {"Mod Matrix", 1, 4, 1}, {"Effects", 2, 0, 3},
    {"Unison", 2, 3, 1},       {"Glide", 2, 4, 1},
};
constexpr int kExpectedGroups = 14;
constexpr int kMaxBlockCellWidth = 70;
constexpr int kBlockMaxColumns = 6;

int centreX(const juce::Component& c) { return c.getX() + c.getWidth() / 2; }

const snakeoil::ParamSpec* rootOf(const snakeoil::ParamSpec* specs, int count, const snakeoil::ParamSpec& spec) {
    const snakeoil::ParamSpec* current = &spec;
    for (int guard = 0; guard < count && current->under[0] != '\0'; ++guard) {
        for (int k = 0; k < count; ++k) {
            if (std::strcmp(specs[k].id, current->under) == 0) {
                current = &specs[k];
                break;
            }
        }
    }
    return current;
}

int checkLayout(juce::AudioProcessorEditor& editor, const std::string& tag, bool expectFits) {
    Inspector in(editor);
    int failures = 0;
    const auto fail = [&](const std::string& message) {
        std::printf("FAIL [%s] %s\n", tag.c_str(), message.c_str());
        ++failures;
    };

    int specCount = 0;
    const auto* specs = snakeoil::paramSpecs(specCount);

    const auto content = in.root->getLocalBounds();
    int controlCount = 0;
    int knobCount = 0;
    int smallKnobs = 0;
    int minKnob = 1 << 30;
    int escaped = 0;
    int overlaps = 0;
    int groupsOutside = 0;
    int maxEscape = 0;
    std::map<std::string, juce::Component*> byId;
    std::map<std::string, const Group*> byTitle;

    if (static_cast<int>(in.groups.size()) != kExpectedGroups) {
        fail("group count " + std::to_string(in.groups.size()) + " != " + std::to_string(kExpectedGroups));
    }

    for (const auto& g : in.groups) {
        const auto gb = g.box->getBounds();
        const std::string gname = g.box->getText().toStdString();
        byTitle[gname] = &g;
        if (!content.contains(gb)) {
            ++groupsOutside;
            fail("group '" + gname + "' " + std::to_string(gb.getX()) + "," + std::to_string(gb.getY()) + " " +
                 std::to_string(gb.getWidth()) + "x" + std::to_string(gb.getHeight()) + " not inside content " +
                 std::to_string(content.getWidth()) + "x" + std::to_string(content.getHeight()));
        }
        for (std::size_t i = 0; i < g.controls.size(); ++i) {
            auto* c = g.controls[i];
            const auto b = c->getBounds();
            if (isControl(*c)) {
                ++controlCount;
                if (byId.count(c->getName().toStdString()) != 0) {
                    fail("duplicate control '" + c->getName().toStdString() + "'");
                }
                byId[c->getName().toStdString()] = c;
            }
            if (auto* s = dynamic_cast<juce::Slider*>(c); s != nullptr && s->isRotary()) {
                ++knobCount;
                const auto area = s->getLookAndFeel().getSliderLayout(*s).sliderBounds;
                minKnob = std::min(minKnob, std::min(area.getWidth(), area.getHeight()));
                if (area.getWidth() < kMinKnobArea || area.getHeight() < kMinKnobArea) {
                    ++smallKnobs;
                    fail("knob area " + std::to_string(area.getWidth()) + "x" + std::to_string(area.getHeight()) +
                         " < 56 in group '" + gname + "' " + describe(*c));
                }
            }
            if (!(b.getX() > gb.getX() && b.getY() > gb.getY() && b.getRight() < gb.getRight() &&
                  b.getBottom() < gb.getBottom())) {
                ++escaped;
                const int out = std::max({gb.getX() - b.getX(), gb.getY() - b.getY(), b.getRight() - gb.getRight(),
                                          b.getBottom() - gb.getBottom()});
                maxEscape = std::max(maxEscape, out);
                fail("control not strictly inside group '" + gname + "' (group " + std::to_string(gb.getX()) + "," +
                     std::to_string(gb.getY()) + " " + std::to_string(gb.getWidth()) + "x" +
                     std::to_string(gb.getHeight()) + "): " + describe(*c));
            }
            for (std::size_t j = i + 1; j < g.controls.size(); ++j) {
                if (b.intersects(g.controls[j]->getBounds())) {
                    ++overlaps;
                    fail("overlap in group '" + gname + "': " + describe(*c) + " vs " + describe(*g.controls[j]));
                }
            }
        }
    }

    // No two group boxes overlap.
    for (std::size_t i = 0; i < in.groups.size(); ++i) {
        for (std::size_t j = i + 1; j < in.groups.size(); ++j) {
            if (in.groups[i].box->getBounds().intersects(in.groups[j].box->getBounds())) {
                fail("group boxes overlap: '" + in.groups[i].box->getText().toStdString() + "' and '" +
                     in.groups[j].box->getText().toStdString() + "'");
            }
        }
    }

    // Every parameter has exactly one control.
    if (controlCount != specCount) {
        fail("control count " + std::to_string(controlCount) + " != " + std::to_string(specCount));
    }
    for (int i = 0; i < specCount; ++i) {
        if (byId.count(specs[i].id) == 0) {
            fail(std::string("no control for parameter ") + specs[i].id);
        }
    }

    // Grid placement: rows by y, columns by x, boxes in a row equally tall.
    std::map<int, juce::Rectangle<int>> rowRef;
    std::map<int, int> colX;
    bool allPresent = true;
    for (const auto& p : kPlacements) {
        const auto it = byTitle.find(p.title);
        if (it == byTitle.end()) {
            fail(std::string("missing group '") + p.title + "'");
            allPresent = false;
            continue;
        }
        const auto b = it->second->box->getBounds();
        const auto ref = rowRef.find(p.row);
        if (ref == rowRef.end()) {
            rowRef[p.row] = b;
        } else if (b.getY() != ref->second.getY() || b.getHeight() != ref->second.getHeight()) {
            fail(std::string("'") + p.title + "' is not on the same row/height as the other boxes of row " +
                 std::to_string(p.row));
        }
        if (std::strcmp(p.title, "Noise") != 0) {  // Noise shares the column with Tempo
            const auto col = colX.find(p.col);
            if (col == colX.end()) {
                colX[p.col] = b.getX();
            } else if (col->second != b.getX()) {
                fail(std::string("'") + p.title + "' is not aligned to grid column " + std::to_string(p.col));
            }
        }
    }
    int previousBottom = -1;
    for (const auto& [row, rect] : rowRef) {
        if (rect.getY() <= previousBottom) {
            fail("grid row " + std::to_string(row) + " is not below the previous row");
        }
        previousBottom = rect.getBottom();
    }
    int previousX = -1;
    for (const auto& [col, x] : colX) {
        if (x <= previousX) {
            fail("grid column " + std::to_string(col) + " is not right of the previous column");
        }
        previousX = x;
    }
    if (allPresent) {
        const auto box = [&](const char* title) { return byTitle[title]->box->getBounds(); };
        const auto effects = box("Effects");
        if (effects.getX() != box("Filter").getX() || effects.getRight() != box("Amp Envelope").getRight()) {
            fail("Effects does not span the first three columns");
        }
        if (box("Oscillator 1").getRight() > box("Oscillator 2").getX() ||
            box("Oscillator 2").getRight() > box("Modulation").getX() ||
            box("Modulation").getRight() > box("Master").getX() || box("Master").getRight() > box("Tempo").getX() ||
            box("Tempo").getRight() > box("Noise").getX()) {
            fail("row 0 is not ordered Oscillator 1, Oscillator 2, Modulation, Master, Tempo, Noise");
        }
        if (box("Mod Matrix").getX() < box("LFO").getRight() || box("Mod Matrix").getRight() != box("Noise").getRight() ||
            box("Glide").getX() != box("Mod Matrix").getX() || box("Unison").getX() != box("LFO").getX()) {
            fail("Mod Matrix / Unison / Glide are not in columns 3-4");
        }
    }

    // LFO: two vertical stacks under centred headers, inside one box.
    if (const auto it = byTitle.find("LFO"); it != byTitle.end()) {
        const char* const stack1[] = {"lfo_rate", "lfo_depth", "lfo_wave", "lfo_dest"};
        const char* const stack2[] = {"lfo2_rate", "lfo2_depth", "lfo2_wave", "lfo2_dest"};
        const char* const* stacks[] = {stack1, stack2};
        const char* const headers[] = {"LFO 1", "LFO 2"};
        int stackRight[2] = {0, 0};
        int stackLeft[2] = {1 << 30, 1 << 30};
        for (int s = 0; s < 2; ++s) {
            juce::Component* previous = nullptr;
            for (int k = 0; k < 4; ++k) {
                auto* c = byId.count(stacks[s][k]) != 0 ? byId[stacks[s][k]] : nullptr;
                if (c == nullptr) {
                    fail(std::string("LFO stack control missing: ") + stacks[s][k]);
                    continue;
                }
                if (std::find(it->second->controls.begin(), it->second->controls.end(), c) == it->second->controls.end()) {
                    fail(std::string(stacks[s][k]) + " is not inside the LFO box");
                }
                if (previous != nullptr &&
                    (c->getY() < previous->getBottom() || std::abs(centreX(*c) - centreX(*previous)) > 1)) {
                    fail(std::string("LFO stack not vertical at ") + stacks[s][k]);
                }
                stackLeft[s] = std::min(stackLeft[s], c->getX());
                stackRight[s] = std::max(stackRight[s], c->getRight());
                previous = c;
            }
            juce::Label* header = nullptr;
            for (auto* c : it->second->controls) {
                if (auto* l = dynamic_cast<juce::Label*>(c); l != nullptr && l->getText() == headers[s]) {
                    header = l;
                }
            }
            if (header == nullptr) {
                fail(std::string("LFO header missing: ") + headers[s]);
            } else if (std::abs(centreX(*header) - (stackLeft[s] + stackRight[s]) / 2) > 2 ||
                       (byId.count(stacks[s][0]) != 0 && header->getBottom() > byId[stacks[s][0]]->getY() + 1)) {
                fail(std::string("LFO header not centred above its stack: ") + headers[s]);
            }
        }
        if (stackRight[0] >= stackLeft[1]) {
            fail("LFO 1 controls are not left of LFO 2 controls");
        }
    }

    // Effects: dependents sit below their root toggle, inside the block's columns, blocks left to right.
    {
        int previousRootX = -1;
        for (int i = 0; i < specCount; ++i) {
            if (std::strcmp(specs[i].group, "Effects") != 0 || specs[i].under[0] != '\0') {
                continue;
            }
            auto* root = byId.count(specs[i].id) != 0 ? byId[specs[i].id] : nullptr;
            if (root == nullptr) {
                continue;
            }
            if (centreX(*root) <= previousRootX) {
                fail(std::string("effect block ") + specs[i].id + " is not right of the previous block");
            }
            previousRootX = centreX(*root);
            std::vector<juce::Component*> deps;
            for (int j = 0; j < specCount; ++j) {
                if (j != i && rootOf(specs, specCount, specs[j]) == &specs[i] && byId.count(specs[j].id) != 0) {
                    deps.push_back(byId[specs[j].id]);
                }
            }
            const int width = std::min(std::max(static_cast<int>(deps.size()), 1), kBlockMaxColumns);
            for (std::size_t k = 0; k < deps.size(); ++k) {
                if (deps[k]->getY() <= root->getBottom()) {
                    fail("dependent " + describe(*deps[k]) + " is not below its root toggle " + describe(*root));
                }
                if (std::abs(centreX(*deps[k]) - centreX(*root)) > width * kMaxBlockCellWidth / 2) {
                    fail("dependent " + describe(*deps[k]) + " is outside the block of " + describe(*root));
                }
                if (k > 0) {
                    const bool sameRow = std::abs(deps[k]->getY() - deps[k - 1]->getY()) < 40;
                    if (sameRow ? centreX(*deps[k]) <= centreX(*deps[k - 1]) : deps[k]->getY() < deps[k - 1]->getY()) {
                        fail("dependents of " + describe(*root) + " are not in registry order");
                    }
                }
            }
        }
    }

    // Mod Matrix: header row, then eight aligned rows of source / scale / destination.
    if (const auto it = byTitle.find("Mod Matrix"); it != byTitle.end()) {
        std::vector<juce::Label*> headers;
        for (auto* c : it->second->controls) {
            if (auto* l = dynamic_cast<juce::Label*>(c)) {
                headers.push_back(l);
            }
        }
        const char* const names[] = {"Source", "Scale", "Destination"};
        const char* const suffix[] = {"src", "amt", "dst"};
        if (headers.size() != 3) {
            fail("mod matrix has " + std::to_string(headers.size()) + " header labels, expected 3");
        }
        int rowBottom = -1;
        for (int row = 1; row <= 8; ++row) {
            juce::Component* cells[3] = {nullptr, nullptr, nullptr};
            for (int c = 0; c < 3; ++c) {
                const std::string id = "mod" + std::to_string(row) + "_" + suffix[c];
                cells[c] = byId.count(id) != 0 ? byId[id] : nullptr;
            }
            if (cells[0] == nullptr || cells[1] == nullptr || cells[2] == nullptr) {
                fail("mod matrix row " + std::to_string(row) + " is incomplete");
                continue;
            }
            for (int c = 1; c < 3; ++c) {
                if (cells[c]->getY() != cells[0]->getY() || cells[c]->getHeight() != cells[0]->getHeight()) {
                    fail("mod matrix row " + std::to_string(row) + " not on one line: " + describe(*cells[0]) +
                         " vs " + describe(*cells[c]));
                }
                if (cells[c]->getX() <= cells[c - 1]->getRight()) {
                    fail("mod matrix row " + std::to_string(row) + " controls not left to right");
                }
            }
            if (cells[0]->getY() < rowBottom) {
                fail("mod matrix row " + std::to_string(row) + " not below the previous row");
            }
            rowBottom = cells[0]->getBottom();
            for (std::size_t h = 0; h < headers.size() && h < 3; ++h) {
                if (headers[h]->getText() != names[h] || headers[h]->getBottom() > cells[0]->getY() ||
                    std::abs(centreX(*headers[h]) - centreX(*cells[h])) > 2) {
                    fail(std::string("mod matrix header '") + names[h] + "' is not centred over its column");
                }
            }
        }
    }

    // Master hosts the level meter to the right of its controls, top-aligned.
    if (const auto it = byTitle.find("Master"); it != byTitle.end()) {
        LevelMeter* meter = nullptr;
        for (auto* c : it->second->controls) {
            if (auto* m = dynamic_cast<LevelMeter*>(c)) {
                meter = m;
            }
        }
        if (meter == nullptr) {
            fail("Master does not contain the level meter");
        } else {
            for (auto* c : it->second->controls) {
                if (isControl(*c) && c->getRight() > meter->getX()) {
                    fail("level meter is not right of " + describe(*c));
                }
            }
            if (byId.count("master_gain") != 0 && meter->getY() > byId["master_gain"]->getY()) {
                fail("level meter is not top-aligned with the Master controls");
            }
        }
    }

    // Unison hosts the clickable tail-slot label below its controls (not a parameter control).
    if (const auto it = byTitle.find("Unison"); it != byTitle.end()) {
        juce::Label* tails = nullptr;
        for (auto* c : it->second->controls) {
            if (auto* l = dynamic_cast<juce::Label*>(c); l != nullptr && l->getText().startsWith("Tails ")) {
                tails = l;
            }
        }
        if (tails == nullptr) {
            fail("Unison does not contain the 'Tails n/slots' label");
        } else {
            for (auto* c : it->second->controls) {
                if (isControl(*c) && c->getBottom() > tails->getY()) {
                    fail("tail label is not below " + describe(*c));
                }
            }
            if (tails->getTooltip().isEmpty()) {
                fail("tail label has no tooltip");
            }
            if (tails->getText() != "Tails 0/6") {
                fail("tail label reads '" + tails->getText().toStdString() + "', expected 'Tails 0/6'");
            }
        }
    }

    if (expectFits && in.viewport != nullptr) {
        const int visible = in.viewport->getMaximumVisibleWidth();
        if (in.root->getWidth() > visible) {
            fail("horizontal overflow: content " + std::to_string(in.root->getWidth()) + " > visible " +
                 std::to_string(visible));
        }
    }

    std::printf("[%s] editor %dx%d content %dx%d groups %zu controls %d knobs %d (min knob area %d) "
                "escaped %d (max %d px) overlaps %d groups-outside %d small-knobs %d => %s\n",
                tag.c_str(), editor.getWidth(), editor.getHeight(), content.getWidth(), content.getHeight(),
                in.groups.size(), controlCount, knobCount, knobCount > 0 ? minKnob : 0, escaped, maxEscape,
                overlaps, groupsOutside, smallKnobs, failures == 0 ? "ok" : "FAILED");
    return failures;
}

bool writePng(const juce::Image& image, const juce::File& file) {
    file.deleteFile();
    juce::FileOutputStream stream(file);
    if (!stream.openedOk()) {
        return false;
    }
    juce::PNGImageFormat png;
    return png.writeImageToStream(image, stream);
}

}  // namespace

int main(int argc, char** argv) {
    juce::ScopedJuceInitialiser_GUI juceInit;

    bool check = false;
    juce::File pngDir;
    for (int i = 1; i < argc; ++i) {
        const juce::String arg(argv[i]);
        if (arg == "--check") {
            check = true;
        } else if (arg == "--png" && i + 1 < argc) {
            pngDir = juce::File::getCurrentWorkingDirectory().getChildFile(argv[++i]);
        } else {
            std::fprintf(stderr, "usage: EditorSnapshot [--check] [--png <dir>]\n");
            return 2;
        }
    }
    if (!check && pngDir == juce::File()) {
        check = true;
    }

    SnakeOilProcessor processor;
    std::unique_ptr<juce::AudioProcessorEditor> editor(processor.createEditor());
    int failures = 0;

    // Show some signal in the header meter (no audio runs here).
    if (auto* snakeEditor = dynamic_cast<SnakeOilEditor*>(editor.get())) {
        snakeEditor->levelMeter().update(0.5, 0.15, true, 0.0);
    }

    const auto snapshot = [&](const char* name) {
        if (pngDir == juce::File()) {
            return;
        }
        pngDir.createDirectory();
        const auto image = editor->createComponentSnapshot(editor->getLocalBounds(), true, 1.0f);
        const auto file = pngDir.getChildFile(name);
        if (!writePng(image, file)) {
            std::fprintf(stderr, "could not write %s\n", file.getFullPathName().toRawUTF8());
            ++failures;
        } else {
            std::printf("wrote %s (%dx%d)\n", file.getFullPathName().toRawUTF8(), image.getWidth(),
                        image.getHeight());
        }
    };

    if (check) {
        failures += checkLayout(*editor, "default", true);
    }
    snapshot("editor_default.png");

    {
        Inspector in(*editor);
        const int w = in.root->getWidth() + (in.viewport != nullptr ? in.viewport->getScrollBarThickness() : 0);
        const int h = in.root->getHeight() + (in.viewport != nullptr ? kHeaderHeight : 0);
        editor->setSize(w, h);
    }
    if (check) {
        failures += checkLayout(*editor, "full", true);
    }
    snapshot("editor_full.png");

    if (check) {
        editor->setSize(640, 420);
        failures += checkLayout(*editor, "min", false);
    }

    editor.reset();
    if (failures != 0) {
        std::printf("%d layout violation(s)\n", failures);
        return 1;
    }
    std::printf("layout ok\n");
    return 0;
}
