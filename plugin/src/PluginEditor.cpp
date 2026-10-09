#include "PluginEditor.h"

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "PluginProcessor.h"
#include "params_gen.hpp"

namespace {

// Geometry (pixels).
constexpr int kHeaderHeight = 28;
constexpr int kOuterMargin = 6;
constexpr int kBoxGap = 4;
constexpr int kScrollBarThickness = 10;
constexpr int kMaxDefaultWidth = 1500;
constexpr int kMaxDefaultHeight = 800;

constexpr int kGroupPadding = 8;
constexpr int kTitleBand = 22;
constexpr int kGroupTopInset = kTitleBand + 4;
constexpr int kCellWidth = 64;
constexpr int kLabelHeight = 16;
constexpr int kKnobCellHeight = 96;
constexpr int kSwitchHeight = 24;
constexpr int kSwitchCellHeight = kLabelHeight + 2 + kSwitchHeight;
constexpr int kChoiceBoxHeight = 26;
constexpr int kChoiceCellHeight = kLabelHeight + 2 + kChoiceBoxHeight;
constexpr int kRootSwitchMaxWidth = 76;

constexpr int kStackWidth = 88;
constexpr int kStackGap = 6;
constexpr int kStackHeaderHeight = 20;

constexpr int kMeterColumnWidth = 44;
constexpr int kMeterGap = 6;
constexpr int kLimiterLabelHeight = 16;

constexpr int kMatrixSourceWidth = 92;
constexpr int kMatrixScaleWidth = 112;
constexpr int kMatrixDestWidth = 124;
constexpr int kMatrixColumnGap = 6;
constexpr int kMatrixHeaderHeight = 18;
constexpr int kMatrixRowHeight = 26;
constexpr int kMatrixControlHeight = 24;

// Hand-placed grid, as in the Python app (midi_synth/gui/main_window.py).
struct CellDef {
    const char* title;
    int row;
    int col;
    int colSpan;
    int stretch;  // relative width when several boxes share the cell
};

const CellDef kCells[] = {
    {"Oscillator 1", 0, 0, 1, 1}, {"Oscillator 2", 0, 1, 1, 1}, {"Modulation", 0, 2, 1, 1},
    {"Master", 0, 3, 1, 1},       {"Tempo", 0, 4, 1, 2},        {"Noise", 0, 4, 1, 3},
    {"Filter", 1, 0, 1, 1},       {"Filter Env", 1, 1, 1, 1},   {"Amp Envelope", 1, 2, 1, 1},
    {"LFO", 1, 3, 1, 1},          {"Mod Matrix", 1, 4, 1, 1},   {"Effects", 2, 0, 3, 1},
    {"Unison", 2, 3, 1, 1},       {"Glide", 2, 4, 1, 1},
};

// Groups whose controls wrap onto a new row after this many columns.
struct WrapDef {
    const char* title;
    int columns;
};
const WrapDef kWrapColumns[] = {{"Oscillator 1", 3}, {"Filter", 3}};

// Registry groups shown inside another group's box: registry group -> (box title, stack index).
struct MergeDef {
    const char* group;
    const char* title;
    int stack;
};
const MergeDef kMergedGroups[] = {{"LFO 1", "LFO", 0}, {"LFO 2", "LFO", 1}};

const char* const kBlockGroup = "Effects";
constexpr int kBlockMaxColumns = 6;
const char* const kMatrixGroup = "Mod Matrix";
const char* const kMatrixHeaders[] = {"Source", "Scale", "Destination"};
const char* const kMatrixSuffixes[] = {"src", "amt", "dst"};
const char* const kMatrixTooltip =
    "Scale is relative: the destination's current value x (1 + scale x source). "
    "A destination whose current value is 0 stays 0.";


// Palette (matches the Python app).
const juce::Colour kBackground(0xff1b1e25);
const juce::Colour kPanel(0xff20242d);
const juce::Colour kOutline(0xff343a4a);
const juce::Colour kText(0xffdfe3ec);
const juce::Colour kTextDim(0xff9aa3b5);
const juce::Colour kAccent(0xff4fc3f7);
const juce::Colour kTrack(0xff3a3f4b);
const juce::Colour kThumb(0xffe8eaf0);
const juce::Colour kControlFill(0xff2c3242);

juce::StringArray splitChoices(const char* choices) {
    return juce::StringArray::fromTokens(juce::String(choices == nullptr ? "" : choices), "|", "");
}

int decimalPlacesForRange(double range) {
    if (range >= 100.0) return 0;
    if (range >= 10.0) return 1;
    if (range >= 2.0) return 2;
    return 3;
}


const snakeoil::ParamSpec* findSpec(const snakeoil::ParamSpec* specs, int count, const char* id) {
    for (int i = 0; i < count; ++i) {
        if (std::strcmp(specs[i].id, id) == 0) {
            return &specs[i];
        }
    }
    return nullptr;
}

/** Follow the `under` chain to the toggle that heads the block. */
const snakeoil::ParamSpec* rootOf(const snakeoil::ParamSpec* specs, int count, const snakeoil::ParamSpec& spec) {
    const snakeoil::ParamSpec* current = &spec;
    for (int guard = 0; guard < count && current->under != nullptr && *current->under != 0; ++guard) {
        const auto* parent = findSpec(specs, count, current->under);
        if (parent == nullptr) {
            break;
        }
        current = parent;
    }
    return current;
}

int cellHeightFor(const snakeoil::ParamSpec& spec) {
    switch (spec.kind) {
        case snakeoil::ParamKind::Toggle: return kSwitchCellHeight;
        case snakeoil::ParamKind::Choice: return kChoiceCellHeight;
        default: return kKnobCellHeight;
    }
}

/** A group frame that can carry a tooltip. */
class TipGroup : public juce::GroupComponent, public juce::SettableTooltipClient {
public:
    using juce::GroupComponent::GroupComponent;
};

/** A toggle drawn as the Python app's Off/On push button. */
class SwitchButton : public juce::TextButton {
public:
    SwitchButton() { setClickingTogglesState(true); }

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override {
        const bool on = getToggleState();
        auto fill = on ? kAccent.darker(0.55f) : kControlFill;
        if (shouldDrawButtonAsDown) {
            fill = fill.brighter(0.15f);
        } else if (shouldDrawButtonAsHighlighted) {
            fill = fill.brighter(0.08f);
        }
        const auto area = getLocalBounds().toFloat().reduced(0.5f);
        g.setColour(fill);
        g.fillRoundedRectangle(area, 4.0f);
        g.setColour(on ? kAccent : kOutline);
        g.drawRoundedRectangle(area, 4.0f, 1.0f);
        g.setColour(on ? kText : kTextDim);
        g.setFont(juce::Font(juce::FontOptions(12.5f)));
        g.drawText(on ? "On" : "Off", getLocalBounds(), juce::Justification::centred, false);
    }
};

class SnakeOilLookAndFeel : public juce::LookAndFeel_V4 {
public:
    SnakeOilLookAndFeel() {
        setColour(juce::ResizableWindow::backgroundColourId, kBackground);

        setColour(juce::Slider::rotarySliderFillColourId, kAccent);
        setColour(juce::Slider::rotarySliderOutlineColourId, kTrack);
        setColour(juce::Slider::thumbColourId, kThumb);
        setColour(juce::Slider::trackColourId, kAccent);
        setColour(juce::Slider::backgroundColourId, kTrack);
        setColour(juce::Slider::textBoxTextColourId, kText);
        setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
        setColour(juce::Slider::textBoxOutlineColourId, kOutline);
        setColour(juce::Slider::textBoxHighlightColourId, kAccent.withAlpha(0.4f));

        setColour(juce::Label::textColourId, kText);
        setColour(juce::Label::textWhenEditingColourId, kText);
        setColour(juce::Label::backgroundWhenEditingColourId, kControlFill);
        setColour(juce::Label::outlineWhenEditingColourId, kAccent);
        setColour(juce::TextEditor::backgroundColourId, kControlFill);
        setColour(juce::TextEditor::textColourId, kText);
        setColour(juce::TextEditor::outlineColourId, kOutline);
        setColour(juce::TextEditor::focusedOutlineColourId, kAccent);
        setColour(juce::TextEditor::highlightColourId, kAccent.withAlpha(0.4f));

        setColour(juce::ComboBox::backgroundColourId, kControlFill);
        setColour(juce::ComboBox::textColourId, kText);
        setColour(juce::ComboBox::outlineColourId, kOutline);
        setColour(juce::ComboBox::buttonColourId, kControlFill);
        setColour(juce::ComboBox::arrowColourId, kTextDim);
        setColour(juce::ComboBox::focusedOutlineColourId, kAccent);
        setColour(juce::PopupMenu::backgroundColourId, kControlFill);
        setColour(juce::PopupMenu::textColourId, kText);
        setColour(juce::PopupMenu::highlightedBackgroundColourId, kAccent.withAlpha(0.35f));
        setColour(juce::PopupMenu::highlightedTextColourId, kText);

        setColour(juce::GroupComponent::outlineColourId, kOutline);
        setColour(juce::GroupComponent::textColourId, kText);

        setColour(juce::ToggleButton::textColourId, kText);
        setColour(juce::ToggleButton::tickColourId, kAccent);
        setColour(juce::ToggleButton::tickDisabledColourId, kTextDim);

        setColour(juce::ScrollBar::thumbColourId, kOutline.brighter(0.4f));
        setColour(juce::ScrollBar::trackColourId, juce::Colours::transparentBlack);
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                          float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override {
        const auto bounds = juce::Rectangle<float>(static_cast<float>(x), static_cast<float>(y),
                                                   static_cast<float>(width), static_cast<float>(height))
                                .reduced(3.0f);
        const float diameter = std::min(bounds.getWidth(), bounds.getHeight());
        const auto centre = bounds.getCentre();
        const float arcWidth = 4.0f;
        const float arcRadius = diameter * 0.5f - arcWidth * 0.5f;
        const float toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        float fromAngle = rotaryStartAngle;
        if (slider.getMinimum() < 0.0 && slider.getMaximum() > 0.0) {
            fromAngle = rotaryStartAngle +
                        static_cast<float>(slider.valueToProportionOfLength(0.0)) * (rotaryEndAngle - rotaryStartAngle);
        }

        const juce::PathStrokeType stroke(arcWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
        juce::Path track;
        track.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(slider.findColour(juce::Slider::rotarySliderOutlineColourId));
        g.strokePath(track, stroke);

        if (std::abs(toAngle - fromAngle) > 0.001f) {
            juce::Path value;
            value.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.0f, std::min(fromAngle, toAngle),
                                std::max(fromAngle, toAngle), true);
            g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
            g.strokePath(value, stroke);
        }

        const float bodyRadius = arcRadius - arcWidth - 2.0f;
        g.setColour(kControlFill);
        g.fillEllipse(centre.x - bodyRadius, centre.y - bodyRadius, bodyRadius * 2.0f, bodyRadius * 2.0f);
        g.setColour(kOutline);
        g.drawEllipse(centre.x - bodyRadius, centre.y - bodyRadius, bodyRadius * 2.0f, bodyRadius * 2.0f, 1.0f);

        const float inner = bodyRadius * 0.35f;
        const juce::Point<float> from(centre.x + inner * std::sin(toAngle), centre.y - inner * std::cos(toAngle));
        const juce::Point<float> to(centre.x + (bodyRadius - 2.0f) * std::sin(toAngle),
                                    centre.y - (bodyRadius - 2.0f) * std::cos(toAngle));
        g.setColour(slider.findColour(juce::Slider::thumbColourId));
        g.drawLine(juce::Line<float>(from, to), 2.5f);
    }

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos,
                          float maxSliderPos, juce::Slider::SliderStyle style, juce::Slider& slider) override {
        if (style != juce::Slider::LinearHorizontal) {
            LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style,
                                             slider);
            return;
        }
        const float trackHeight = 4.0f;
        const float cy = static_cast<float>(y) + static_cast<float>(height) * 0.5f;
        const float left = static_cast<float>(x) + 6.0f;
        const float right = static_cast<float>(x + width) - 6.0f;
        const juce::Rectangle<float> track(left, cy - trackHeight * 0.5f, right - left, trackHeight);
        g.setColour(kTrack);
        g.fillRoundedRectangle(track, 2.0f);

        float zeroX = left;
        if (slider.getMinimum() < 0.0 && slider.getMaximum() > 0.0) {
            zeroX = left + static_cast<float>(slider.valueToProportionOfLength(0.0)) * (right - left);
            g.setColour(kTextDim);
            g.fillRect(zeroX - 0.5f, cy - 5.0f, 1.0f, 10.0f);
        }
        const float a = std::min(zeroX, sliderPos);
        const float b = std::max(zeroX, sliderPos);
        if (b > a) {
            g.setColour(kAccent);
            g.fillRoundedRectangle(a, cy - trackHeight * 0.5f, b - a, trackHeight, 2.0f);
        }
        g.setColour(slider.findColour(juce::Slider::thumbColourId));
        g.fillEllipse(sliderPos - 6.0f, cy - 6.0f, 12.0f, 12.0f);
    }

    juce::Slider::SliderLayout getSliderLayout(juce::Slider& slider) override {
        auto layout = LookAndFeel_V4::getSliderLayout(slider);
        if (slider.getSliderStyle() == juce::Slider::LinearHorizontal &&
            slider.getTextBoxPosition() == juce::Slider::TextBoxRight) {
            // use the whole width left of the text box (V4 trims it for the thumb)
            layout.sliderBounds = slider.getLocalBounds().withTrimmedRight(slider.getTextBoxWidth());
        }
        return layout;
    }

    void drawComboBox(juce::Graphics& g, int width, int height, bool, int, int, int, int,
                      juce::ComboBox& box) override {
        const auto area = juce::Rectangle<float>(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height))
                              .reduced(0.5f);
        g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
        g.fillRoundedRectangle(area, 4.0f);
        g.setColour(box.findColour(juce::ComboBox::outlineColourId));
        g.drawRoundedRectangle(area, 4.0f, 1.0f);
        const float cx = static_cast<float>(width) - 11.0f;
        const float cy = static_cast<float>(height) * 0.5f;
        juce::Path arrow;
        arrow.startNewSubPath(cx - 3.5f, cy - 1.5f);
        arrow.lineTo(cx, cy + 2.0f);
        arrow.lineTo(cx + 3.5f, cy - 1.5f);
        g.setColour(box.findColour(juce::ComboBox::arrowColourId));
        g.strokePath(arrow, juce::PathStrokeType(1.5f));
    }

    void positionComboBoxText(juce::ComboBox& box, juce::Label& label) override {
        label.setBounds(1, 1, box.getWidth() - 20, box.getHeight() - 2);
        label.setFont(getComboBoxFont(box));
    }

    juce::Font getComboBoxFont(juce::ComboBox&) override { return juce::Font(juce::FontOptions(12.5f)); }

    void drawGroupComponentOutline(juce::Graphics& g, int width, int height, const juce::String& text,
                                   const juce::Justification&, juce::GroupComponent& group) override {
        const auto area = juce::Rectangle<float>(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
        g.setColour(kPanel);
        g.fillRoundedRectangle(area.reduced(0.5f), 5.0f);
        g.setColour(group.findColour(juce::GroupComponent::outlineColourId));
        g.drawRoundedRectangle(area.reduced(0.5f), 5.0f, 1.0f);
        g.setColour(group.findColour(juce::GroupComponent::textColourId));
        g.setFont(juce::Font(juce::FontOptions(13.0f, juce::Font::bold)));
        g.drawText(text, kGroupPadding + 2, 2, width - 2 * kGroupPadding, kTitleBand - 4,
                   juce::Justification::centredLeft, true);
    }
};


}  // namespace

// --- Content: the scrolled body ---------------------------------------------------------------------

class SnakeOilEditor::Content : public juce::Component {
public:
    explicit Content(juce::AudioProcessorValueTreeState& state) : state_(state) {
        specs_ = snakeoil::paramSpecs(count_);

        // One box per displayed title, in order of first appearance in the registry.
        std::vector<std::string> titles;
        for (int i = 0; i < count_; ++i) {
            const std::string title = titleFor(specs_[i]);
            if (std::find(titles.begin(), titles.end(), title) == titles.end()) {
                titles.push_back(title);
            }
        }
        for (const auto& title : titles) {
            std::vector<int> indices;
            for (int i = 0; i < count_; ++i) {
                if (titleFor(specs_[i]) == title) {
                    indices.push_back(i);
                }
            }
            boxes_.push_back(std::make_unique<Box>());
            buildBox(*boxes_.back(), title, indices);
        }
        assignSlots();
    }

    LevelMeter& meter() { return meter_; }

    /** Width the grid needs to show every box at its natural size. */
    int naturalWidth() const {
        const auto widths = columnWidths();
        int total = 2 * kOuterMargin + (static_cast<int>(widths.size()) - 1) * kBoxGap;
        for (int w : widths) {
            total += w;
        }
        return total;
    }

    /** Place everything for the given width (never narrower than the natural width). Returns the height. */
    int layoutForWidth(int width) {
        auto widths = columnWidths();
        const int natural = naturalWidth();
        const int contentWidth = std::max(width, natural);
        const int extra = contentWidth - natural;
        const int columns = static_cast<int>(widths.size());
        for (int c = 0; c < columns; ++c) {
            widths[static_cast<std::size_t>(c)] += extra / columns + (c == columns - 1 ? extra % columns : 0);
        }
        const auto colX = [&](int col) {
            int x = kOuterMargin;
            for (int c = 0; c < col; ++c) {
                x += widths[static_cast<std::size_t>(c)] + kBoxGap;
            }
            return x;
        };

        int rows = 0;
        for (const auto& slot : slots_) {
            rows = std::max(rows, slot.row + 1);
        }
        std::vector<int> rowHeights(static_cast<std::size_t>(rows), 0);
        for (const auto& slot : slots_) {
            auto& h = rowHeights[static_cast<std::size_t>(slot.row)];
            h = std::max(h, slot.naturalHeight());
        }

        int y = kOuterMargin;
        std::vector<int> rowY(static_cast<std::size_t>(rows), 0);
        for (int r = 0; r < rows; ++r) {
            rowY[static_cast<std::size_t>(r)] = y;
            y += rowHeights[static_cast<std::size_t>(r)] + kBoxGap;
        }
        const int contentHeight = y - kBoxGap + kOuterMargin;

        for (const auto& slot : slots_) {
            int w = (slot.colSpan - 1) * kBoxGap;
            for (int c = slot.col; c < slot.col + slot.colSpan; ++c) {
                w += widths[static_cast<std::size_t>(c)];
            }
            const int h = rowHeights[static_cast<std::size_t>(slot.row)];
            int x = colX(slot.col);
            const int top = rowY[static_cast<std::size_t>(slot.row)];

            const int n = static_cast<int>(slot.boxes.size());
            int sumNatural = (n - 1) * kBoxGap;
            int sumStretch = 0;
            for (int i = 0; i < n; ++i) {
                sumNatural += slot.boxes[static_cast<std::size_t>(i)]->naturalWidth();
                sumStretch += slot.stretches[static_cast<std::size_t>(i)];
            }
            const int spare = w - sumNatural;
            int used = 0;
            for (int i = 0; i < n; ++i) {
                auto* box = slot.boxes[static_cast<std::size_t>(i)];
                int boxWidth = box->naturalWidth();
                if (n == 1) {
                    boxWidth = w;
                } else if (i == n - 1) {
                    boxWidth += spare - used;
                } else {
                    const int share = spare * slot.stretches[static_cast<std::size_t>(i)] / sumStretch;
                    boxWidth += share;
                    used += share;
                }
                box->place({x, top, boxWidth, h});
                x += boxWidth + kBoxGap;
            }
        }
        setSize(contentWidth, contentHeight);
        return contentHeight;
    }

private:
    struct Control {
        std::unique_ptr<juce::Label> label;
        std::unique_ptr<juce::Slider> slider;
        std::unique_ptr<SwitchButton> button;
        std::unique_ptr<juce::ComboBox> combo;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> comboAttachment;
    };

    enum class Anchor { None, GrowWidth, ShiftRight };

    struct Item {
        juce::Component* component;
        juce::Rectangle<int> rel;  // relative to the interior (inside the padding, below the title)
        Anchor anchor = Anchor::None;
    };

    struct Box {
        std::string title;
        std::unique_ptr<TipGroup> frame;
        std::vector<Item> items;
        int contentWidth = 0;
        int contentHeight = 0;
        bool leftAligned = false;  // the Mod Matrix keeps its columns left and stretches the middle one

        int naturalWidth() const { return contentWidth + 2 * kGroupPadding; }
        int naturalHeight() const { return kGroupTopInset + contentHeight + kGroupPadding; }

        void place(juce::Rectangle<int> bounds) {
            frame->setBounds(bounds);
            const int spare = std::max(0, bounds.getWidth() - naturalWidth());
            const int originX = bounds.getX() + (leftAligned ? kGroupPadding : kGroupPadding + spare / 2);
            const int originY = bounds.getY() + kGroupTopInset;
            for (const auto& item : items) {
                auto r = item.rel;
                if (leftAligned && item.anchor == Anchor::GrowWidth) {
                    r.setWidth(r.getWidth() + spare);
                } else if (leftAligned && item.anchor == Anchor::ShiftRight) {
                    r.translate(spare, 0);
                }
                item.component->setBounds(r.translated(originX, originY));
            }
        }
    };

    struct Slot {
        int row = 0;
        int col = 0;
        int colSpan = 1;
        std::vector<Box*> boxes;
        std::vector<int> stretches;

        int naturalWidth() const {
            int w = (static_cast<int>(boxes.size()) - 1) * kBoxGap;
            for (const auto* box : boxes) {
                w += box->naturalWidth();
            }
            return w;
        }
        int naturalHeight() const {
            int h = 0;
            for (const auto* box : boxes) {
                h = std::max(h, box->naturalHeight());
            }
            return h;
        }
    };

    static std::string titleFor(const snakeoil::ParamSpec& spec) {
        for (const auto& merge : kMergedGroups) {
            if (std::strcmp(merge.group, spec.group) == 0) {
                return merge.title;
            }
        }
        return spec.group;
    }

    static int stackFor(const snakeoil::ParamSpec& spec) {
        for (const auto& merge : kMergedGroups) {
            if (std::strcmp(merge.group, spec.group) == 0) {
                return merge.stack;
            }
        }
        return -1;
    }

    void assignSlots() {
        int fallbackCol = 0;
        for (auto& box : boxes_) {
            const CellDef* def = nullptr;
            for (const auto& cell : kCells) {
                if (box->title == cell.title) {
                    def = &cell;
                }
            }
            int row = 3;
            int col = fallbackCol;
            int span = 1;
            int stretch = 1;
            if (def != nullptr) {
                row = def->row;
                col = def->col;
                span = def->colSpan;
                stretch = def->stretch;
            } else {
                ++fallbackCol;  // unknown groups form an extra row at the bottom
            }
            auto it = std::find_if(slots_.begin(), slots_.end(),
                                   [&](const Slot& s) { return s.row == row && s.col == col; });
            if (it == slots_.end()) {
                slots_.push_back({});
                it = slots_.end() - 1;
                it->row = row;
                it->col = col;
                it->colSpan = span;
            }
            it->boxes.push_back(box.get());
            it->stretches.push_back(stretch);
        }
        // Keep shared cells in table order (Tempo before Noise).
        for (auto& slot : slots_) {
            if (slot.boxes.size() < 2) {
                continue;
            }
            std::vector<std::pair<int, std::size_t>> order;
            for (std::size_t i = 0; i < slot.boxes.size(); ++i) {
                int index = 0;
                for (const auto& cell : kCells) {
                    if (slot.boxes[i]->title == cell.title) {
                        break;
                    }
                    ++index;
                }
                order.push_back({index, i});
            }
            std::sort(order.begin(), order.end());
            std::vector<Box*> boxes;
            std::vector<int> stretches;
            for (const auto& [index, i] : order) {
                boxes.push_back(slot.boxes[i]);
                stretches.push_back(slot.stretches[i]);
            }
            slot.boxes = boxes;
            slot.stretches = stretches;
        }
    }

    std::vector<int> columnWidths() const {
        int columns = 0;
        for (const auto& slot : slots_) {
            columns = std::max(columns, slot.col + slot.colSpan);
        }
        std::vector<int> widths(static_cast<std::size_t>(columns), 0);
        for (const auto& slot : slots_) {
            if (slot.colSpan == 1) {
                auto& w = widths[static_cast<std::size_t>(slot.col)];
                w = std::max(w, slot.naturalWidth());
            }
        }
        for (const auto& slot : slots_) {
            if (slot.colSpan > 1) {
                int have = (slot.colSpan - 1) * kBoxGap;
                for (int c = slot.col; c < slot.col + slot.colSpan; ++c) {
                    have += widths[static_cast<std::size_t>(c)];
                }
                const int deficit = slot.naturalWidth() - have;
                for (int c = 0; deficit > 0 && c < slot.colSpan; ++c) {
                    widths[static_cast<std::size_t>(slot.col + c)] +=
                        deficit / slot.colSpan + (c == slot.colSpan - 1 ? deficit % slot.colSpan : 0);
                }
            }
        }
        return widths;
    }

    // --- control construction ---

    juce::Label* makeLabel(const juce::String& text, juce::Justification justification, float fontSize = 12.0f) {
        auto label = std::make_unique<juce::Label>(juce::String(), text);
        label->setJustificationType(justification);
        label->setFont(juce::Font(juce::FontOptions(fontSize)));
        label->setBorderSize(juce::BorderSize<int>(0, 1, 0, 1));
        label->setInterceptsMouseClicks(false, false);
        auto* raw = label.get();
        addAndMakeVisible(*raw);
        extraLabels_.push_back(std::move(label));
        return raw;
    }

    static void applyDecimals(juce::Slider& slider, const snakeoil::ParamSpec& spec) {
        const int decimals = decimalPlacesForRange(static_cast<double>(spec.maximum) - static_cast<double>(spec.minimum));
        slider.setNumDecimalPlacesToDisplay(decimals);
        slider.textFromValueFunction = [decimals](double value) { return juce::String(value, decimals); };
        slider.updateText();
    }

    Control& makeControl(const snakeoil::ParamSpec& spec, bool matrixStyle) {
        controls_.push_back(std::make_unique<Control>());
        auto& control = *controls_.back();
        switch (spec.kind) {
            case snakeoil::ParamKind::Toggle:
                control.label = std::make_unique<juce::Label>(juce::String(), spec.label);
                control.button = std::make_unique<SwitchButton>();
                control.button->setName(spec.id);
                control.button->setTooltip(spec.label);
                control.buttonAttachment =
                    std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(state_, spec.id, *control.button);
                addAndMakeVisible(*control.button);
                break;
            case snakeoil::ParamKind::Choice:
                if (!matrixStyle) {
                    control.label = std::make_unique<juce::Label>(juce::String(), spec.label);
                }
                control.combo = std::make_unique<juce::ComboBox>();
                control.combo->setName(spec.id);
                control.combo->setTooltip(spec.label);
                control.combo->addItemList(splitChoices(spec.choices), 1);
                control.combo->setJustificationType(juce::Justification::centredLeft);
                control.comboAttachment =
                    std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(state_, spec.id, *control.combo);
                addAndMakeVisible(*control.combo);
                break;
            default:
                if (matrixStyle) {
                    control.slider = std::make_unique<juce::Slider>(juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight);
                    control.slider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
                } else {
                    control.label = std::make_unique<juce::Label>(juce::String(), spec.label);
                    control.slider = std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag,
                                                                    juce::Slider::TextBoxBelow);
                    control.slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, kCellWidth - 4, 18);
                }
                control.slider->setName(spec.id);
                control.slider->setTooltip(spec.label);
                control.slider->setScrollWheelEnabled(false);
                control.sliderAttachment =
                    std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state_, spec.id, *control.slider);
                applyDecimals(*control.slider, spec);
                addAndMakeVisible(*control.slider);
                break;
        }
        if (control.label != nullptr) {
            control.label->setJustificationType(juce::Justification::centred);
            control.label->setFont(juce::Font(juce::FontOptions(12.0f)));
            control.label->setBorderSize(juce::BorderSize<int>(0, 1, 0, 1));
            control.label->setInterceptsMouseClicks(false, false);
            addAndMakeVisible(*control.label);
        }
        return control;
    }

    /** Create a control and place it in `cell` (relative to the box interior). */
    void addCell(Box& box, const snakeoil::ParamSpec& spec, juce::Rectangle<int> cell, int maxEditorWidth = 0) {
        auto& control = makeControl(spec, false);
        const int innerWidth = cell.getWidth() - 4;
        const int editorWidth = maxEditorWidth > 0 ? std::min(innerWidth, maxEditorWidth) : innerWidth;
        const int editorX = cell.getX() + 2 + (innerWidth - editorWidth) / 2;
        box.items.push_back({control.label.get(), {cell.getX() + 2, cell.getY(), innerWidth, kLabelHeight}});
        if (control.button != nullptr) {
            box.items.push_back({control.button.get(), {editorX, cell.getY() + kLabelHeight + 2, editorWidth, kSwitchHeight}});
        } else if (control.combo != nullptr) {
            box.items.push_back({control.combo.get(), {editorX, cell.getY() + kLabelHeight + 2, editorWidth, kChoiceBoxHeight}});
        } else {
            box.items.push_back(
                {control.slider.get(), {cell.getX() + 2, cell.getY() + kLabelHeight, innerWidth, kKnobCellHeight - kLabelHeight}});
        }
    }

    // --- box builders ---

    void buildBox(Box& box, const std::string& title, const std::vector<int>& indices) {
        box.title = title;
        box.frame = std::make_unique<TipGroup>(title, title);
        box.frame->setTextLabelPosition(juce::Justification::centredLeft);
        addAndMakeVisible(*box.frame);
        if (title == kMatrixGroup) {
            box.frame->setTooltip(kMatrixTooltip);
            buildMatrix(box, indices);
        } else if (stackFor(specs_[indices.front()]) >= 0) {
            box.frame->setInterceptsMouseClicks(false, false);
            buildStacks(box, indices);
        } else if (title == kBlockGroup) {
            box.frame->setInterceptsMouseClicks(false, false);
            buildBlocks(box, indices);
        } else {
            box.frame->setInterceptsMouseClicks(false, false);
            int wrap = 0;
            for (const auto& def : kWrapColumns) {
                if (title == def.title) {
                    wrap = def.columns;
                }
            }
            buildFlow(box, indices, wrap);
            if (title == "Master") {
                addMeter(box);
            }
        }
    }

    /** Controls left to right, wrapping after `wrap` columns (0 = one row). */
    void buildFlow(Box& box, const std::vector<int>& indices, int wrap) {
        const int n = static_cast<int>(indices.size());
        const int columns = wrap > 0 ? std::min(wrap, n) : n;
        const int rows = (n + columns - 1) / columns;
        std::vector<int> rowHeights(static_cast<std::size_t>(rows), 0);
        for (int i = 0; i < n; ++i) {
            auto& h = rowHeights[static_cast<std::size_t>(i / columns)];
            h = std::max(h, cellHeightFor(specs_[indices[static_cast<std::size_t>(i)]]));
        }
        int y = 0;
        std::vector<int> rowY;
        for (int h : rowHeights) {
            rowY.push_back(y);
            y += h;
        }
        for (int i = 0; i < n; ++i) {
            const auto& spec = specs_[indices[static_cast<std::size_t>(i)]];
            addCell(box, spec,
                    {(i % columns) * kCellWidth, rowY[static_cast<std::size_t>(i / columns)], kCellWidth, cellHeightFor(spec)});
        }
        box.contentWidth = columns * kCellWidth;
        box.contentHeight = y;
    }

    /** Registry groups as vertical stacks side by side, each under a centred header. */
    void buildStacks(Box& box, const std::vector<int>& indices) {
        int stacks = 0;
        for (int index : indices) {
            stacks = std::max(stacks, stackFor(specs_[index]) + 1);
        }
        int tallest = 0;
        for (int s = 0; s < stacks; ++s) {
            const int x = s * (kStackWidth + kStackGap);
            int y = kStackHeaderHeight;
            bool header = false;
            for (int index : indices) {
                const auto& spec = specs_[index];
                if (stackFor(spec) != s) {
                    continue;
                }
                if (!header) {
                    auto* label = makeLabel(spec.group, juce::Justification::centred, 12.5f);
                    label->setColour(juce::Label::textColourId, kTextDim);
                    box.items.push_back({label, {x, 0, kStackWidth, kStackHeaderHeight}});
                    header = true;
                }
                addCell(box, spec, {x, y, kStackWidth, cellHeightFor(spec)}, kStackWidth - 8);
                y += cellHeightFor(spec);
            }
            tallest = std::max(tallest, y);
        }
        box.contentWidth = stacks * kStackWidth + (stacks - 1) * kStackGap;
        box.contentHeight = tallest;
    }

    /** Toggle blocks: each root toggle heads a block, its dependents sit in rows beneath it. */
    void buildBlocks(Box& box, const std::vector<int>& indices) {
        struct Placed {
            int index;
            int row;
            int col;
            int span;
        };
        std::vector<int> dependents(static_cast<std::size_t>(count_), 0);
        for (int index : indices) {
            const auto& spec = specs_[index];
            if (spec.under != nullptr && spec.under[0] != '\0') {
                ++dependents[static_cast<std::size_t>(rootOf(specs_, count_, spec) - specs_)];
            }
        }
        const auto widthOf = [&](int rootIndex) {
            return std::min(std::max(dependents[static_cast<std::size_t>(rootIndex)], 1), kBlockMaxColumns);
        };

        std::vector<Placed> placed;
        std::vector<int> seen(static_cast<std::size_t>(count_), 0);
        std::vector<int> rootCol(static_cast<std::size_t>(count_), 0);
        int nextCol = 0;
        for (int index : indices) {
            const auto& spec = specs_[index];
            if (spec.under == nullptr || spec.under[0] == '\0') {
                rootCol[static_cast<std::size_t>(index)] = nextCol;
                placed.push_back({index, 0, nextCol, widthOf(index)});
                nextCol += widthOf(index);
            } else {
                const int root = static_cast<int>(rootOf(specs_, count_, spec) - specs_);
                const int n = ++seen[static_cast<std::size_t>(root)];
                const int width = widthOf(root);
                placed.push_back({index, 1 + (n - 1) / width, rootCol[static_cast<std::size_t>(root)] + (n - 1) % width, 1});
            }
        }
        int rows = 0;
        for (const auto& p : placed) {
            rows = std::max(rows, p.row + 1);
        }
        std::vector<int> rowHeights(static_cast<std::size_t>(rows), 0);
        for (const auto& p : placed) {
            auto& h = rowHeights[static_cast<std::size_t>(p.row)];
            h = std::max(h, cellHeightFor(specs_[p.index]));
        }
        std::vector<int> rowY;
        int y = 0;
        for (int h : rowHeights) {
            rowY.push_back(y);
            y += h;
        }
        for (const auto& p : placed) {
            const auto& spec = specs_[p.index];
            addCell(box, spec, {p.col * kCellWidth, rowY[static_cast<std::size_t>(p.row)], p.span * kCellWidth, cellHeightFor(spec)},
                    p.row == 0 ? kRootSwitchMaxWidth : 0);
        }
        box.contentWidth = nextCol * kCellWidth;
        box.contentHeight = y;
    }

    /** Header row (Source / Scale / Destination) over one row per slot: combo, slider, combo. */
    void buildMatrix(Box& box, const std::vector<int>& indices) {
        box.leftAligned = true;
        const int xs[] = {0, kMatrixSourceWidth + kMatrixColumnGap,
                          kMatrixSourceWidth + kMatrixScaleWidth + 2 * kMatrixColumnGap};
        const int ws[] = {kMatrixSourceWidth, kMatrixScaleWidth, kMatrixDestWidth};
        const Anchor anchors[] = {Anchor::None, Anchor::GrowWidth, Anchor::ShiftRight};
        for (int c = 0; c < 3; ++c) {
            auto* label = makeLabel(kMatrixHeaders[c], juce::Justification::centred);
            label->setColour(juce::Label::textColourId, kTextDim);
            label->setInterceptsMouseClicks(true, false);
            label->setTooltip(kMatrixTooltip);
            box.items.push_back({label, {xs[c], 0, ws[c], kMatrixHeaderHeight}, anchors[c]});
        }
        const int rowsTop = kMatrixHeaderHeight + 4;
        int rows = 0;
        for (int index : indices) {
            const std::string id = specs_[index].id;
            const auto underscore = id.find('_');
            const int row = std::stoi(id.substr(3, underscore - 3)) - 1;
            const std::string suffix = id.substr(underscore + 1);
            int column = 0;
            for (int c = 0; c < 3; ++c) {
                if (suffix == kMatrixSuffixes[c]) {
                    column = c;
                }
            }
            rows = std::max(rows, row + 1);
            auto& control = makeControl(specs_[index], true);
            juce::Component* component = control.combo != nullptr ? static_cast<juce::Component*>(control.combo.get())
                                                                   : static_cast<juce::Component*>(control.slider.get());
            box.items.push_back({component,
                                 {xs[column], rowsTop + row * kMatrixRowHeight + (kMatrixRowHeight - kMatrixControlHeight) / 2,
                                  ws[column], kMatrixControlHeight},
                                 anchors[column]});
        }
        box.contentWidth = xs[2] + ws[2];
        box.contentHeight = rowsTop + rows * kMatrixRowHeight;
    }

    /** The output meter and limiter label, to the right of the Master controls. */
    void addMeter(Box& box) {
        const int x = box.contentWidth + kMeterGap;
        addAndMakeVisible(meter_);
        box.items.push_back({&meter_,
                             {x + (kMeterColumnWidth - LevelMeter::kPreferredWidth) / 2, 0, LevelMeter::kPreferredWidth,
                              LevelMeter::kPreferredHeight}});
        limiterLabel_ = std::make_unique<juce::Label>(juce::String(), "GR off");
        limiterLabel_->setJustificationType(juce::Justification::centred);
        limiterLabel_->setFont(juce::Font(juce::FontOptions(11.0f)));
        limiterLabel_->setColour(juce::Label::textColourId, kTextDim);
        limiterLabel_->setBorderSize(juce::BorderSize<int>(0, 0, 0, 0));
        limiterLabel_->setTooltip("Limiter gain reduction (not reported by the plugin yet)");
        addAndMakeVisible(*limiterLabel_);
        box.items.push_back(
            {limiterLabel_.get(), {x, LevelMeter::kPreferredHeight + 2, kMeterColumnWidth, kLimiterLabelHeight}});
        box.contentWidth = x + kMeterColumnWidth;
        box.contentHeight = std::max(box.contentHeight, LevelMeter::kPreferredHeight + 2 + kLimiterLabelHeight);
    }

    juce::AudioProcessorValueTreeState& state_;
    const snakeoil::ParamSpec* specs_ = nullptr;
    int count_ = 0;
    std::vector<std::unique_ptr<Control>> controls_;
    std::vector<std::unique_ptr<juce::Label>> extraLabels_;
    std::unique_ptr<juce::Label> limiterLabel_;
    LevelMeter meter_;
    std::vector<std::unique_ptr<Box>> boxes_;
    std::vector<Slot> slots_;
};

// --- Editor -------------------------------------------------------------------------------------------

SnakeOilEditor::SnakeOilEditor(SnakeOilProcessor& processor)
    : AudioProcessorEditor(&processor), processor_(processor) {
    laf_ = std::make_unique<SnakeOilLookAndFeel>();
    setLookAndFeel(laf_.get());

    content_ = std::make_unique<Content>(processor_.state());
    viewport_.setViewedComponent(content_.get(), false);
    viewport_.setScrollBarsShown(true, true);
    viewport_.setScrollBarThickness(kScrollBarThickness);
    addAndMakeVisible(viewport_);
    sendLookAndFeelChange();  // children built before parenting must pick up the themed colours

    const int naturalWidth = content_->naturalWidth();
    const int contentHeight = content_->layoutForWidth(naturalWidth);
    const int fullWidth = naturalWidth + kScrollBarThickness;
    const int fullHeight = contentHeight + kHeaderHeight;

    setResizable(true, true);
    setResizeLimits(640, 420, std::max(fullWidth, 640) + 600, std::max(fullHeight, 420));
    setSize(std::min(fullWidth, kMaxDefaultWidth), std::min(fullHeight, kMaxDefaultHeight));
    startTimerHz(15);
}

SnakeOilEditor::~SnakeOilEditor() {
    stopTimer();
    setLookAndFeel(nullptr);
}

LevelMeter& SnakeOilEditor::levelMeter() { return content_->meter(); }

void SnakeOilEditor::paint(juce::Graphics& g) {
    g.fillAll(kBackground);
    g.setColour(kOutline);
    g.fillRect(0, kHeaderHeight - 1, getWidth(), 1);
    g.setColour(kText);
    g.setFont(juce::Font(juce::FontOptions(16.0f, juce::Font::bold)));
    g.drawText("SnakeOil Synth", 14, 0, 300, kHeaderHeight - 1, juce::Justification::centredLeft, false);
}

void SnakeOilEditor::resized() {
    auto bounds = getLocalBounds();
    bounds.removeFromTop(kHeaderHeight);
    viewport_.setBounds(bounds);
    content_->layoutForWidth(bounds.getWidth() - viewport_.getScrollBarThickness());
}

void SnakeOilEditor::timerCallback() {
    double left = 0.0;
    double right = 0.0;
    bool clipped = false;
    processor_.engineMeter(left, right, clipped);
    content_->meter().update(left, right, clipped, juce::Time::getMillisecondCounterHiRes());
}
