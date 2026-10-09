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
#include <memory>
#include <string>
#include <vector>

#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "params_gen.hpp"

namespace {

constexpr int kHeaderHeight = 36;
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

int checkLayout(juce::AudioProcessorEditor& editor, const std::string& tag, int expectedControls,
                int expectedGroups) {
    Inspector in(editor);
    int failures = 0;
    const auto fail = [&](const std::string& message) {
        std::printf("FAIL [%s] %s\n", tag.c_str(), message.c_str());
        ++failures;
    };

    const auto content = in.root->getLocalBounds();
    int controlCount = 0;
    int knobCount = 0;
    int smallKnobs = 0;
    int minKnob = 1 << 30;
    int escaped = 0;
    int overlaps = 0;
    int groupsOutside = 0;
    int maxEscape = 0;

    if (static_cast<int>(in.groups.size()) != expectedGroups) {
        fail("group count " + std::to_string(in.groups.size()) + " != " + std::to_string(expectedGroups));
    }

    for (const auto& g : in.groups) {
        const auto gb = g.box->getBounds();
        const std::string gname = g.box->getText().toStdString();
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

        if (gname == "Mod Matrix") {
            std::vector<juce::Component*> row;
            for (auto* c : g.controls) {
                if (isControl(*c)) {
                    row.push_back(c);
                }
            }
            if (row.empty() || row.size() % 3 != 0) {
                fail("mod matrix has " + std::to_string(row.size()) + " controls, expected a multiple of 3");
            }
            int prevY = -1;
            for (std::size_t r = 0; r + 2 < row.size(); r += 3) {
                const int y = row[r]->getY();
                const int h = row[r]->getHeight();
                for (std::size_t k = 1; k < 3; ++k) {
                    if (row[r + k]->getY() != y || row[r + k]->getHeight() != h) {
                        fail("mod matrix row " + std::to_string(r / 3) + " not on one line: " + describe(*row[r]) +
                             " vs " + describe(*row[r + k]));
                    }
                }
                if (row[r + 1]->getX() <= row[r]->getRight() || row[r + 2]->getX() <= row[r + 1]->getRight()) {
                    fail("mod matrix row " + std::to_string(r / 3) + " controls not left to right");
                }
                if (y <= prevY) {
                    fail("mod matrix row " + std::to_string(r / 3) + " not below the previous row");
                }
                prevY = y;
            }
        }
    }

    if (in.viewport != nullptr) {
        const int visible = in.viewport->getMaximumVisibleWidth();
        if (in.root->getWidth() > visible) {
            fail("horizontal overflow: content " + std::to_string(in.root->getWidth()) + " > visible " +
                 std::to_string(visible));
        }
        if (in.viewport->isHorizontalScrollBarShown()) {
            fail("horizontal scrollbar is shown");
        }
    }
    if (controlCount != expectedControls) {
        fail("control count " + std::to_string(controlCount) + " != " + std::to_string(expectedControls));
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

    int specCount = 0;
    const auto* specs = snakeoil::paramSpecs(specCount);
    std::vector<std::string> groupNames;
    for (int i = 0; i < specCount; ++i) {
        if (std::find(groupNames.begin(), groupNames.end(), specs[i].group) == groupNames.end()) {
            groupNames.emplace_back(specs[i].group);
        }
    }
    const int groupCount = static_cast<int>(groupNames.size());

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
        failures += checkLayout(*editor, "default", specCount, groupCount);
    }
    snapshot("editor_default.png");

    {
        Inspector in(*editor);
        const int w = in.root->getWidth() + (in.viewport != nullptr ? in.viewport->getScrollBarThickness() : 0);
        const int h = in.root->getHeight() + (in.viewport != nullptr ? kHeaderHeight : 0);
        editor->setSize(w, h);
    }
    if (check) {
        failures += checkLayout(*editor, "full", specCount, groupCount);
    }
    snapshot("editor_full.png");

    if (check) {
        editor->setSize(640, 420);
        failures += checkLayout(*editor, "min", specCount, groupCount);
    }

    editor.reset();
    if (failures != 0) {
        std::printf("%d layout violation(s)\n", failures);
        return 1;
    }
    std::printf("layout ok\n");
    return 0;
}
