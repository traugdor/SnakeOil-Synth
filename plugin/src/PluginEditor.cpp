#include "PluginEditor.h"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "PluginProcessor.h"
#include "params_gen.hpp"

namespace {

// Geometry (pixels).
constexpr int kHeaderHeight = 36;
constexpr int kContentWidth = 1180;
constexpr int kMinContentWidth = 600;
constexpr int kBodyMargin = 10;
constexpr int kGroupGap = 10;
constexpr int kScrollBarThickness = 10;

constexpr int kGroupPadding = 10;
constexpr int kTitleBand = 24;
constexpr int kGroupTopInset = kTitleBand + 6;
constexpr int kCellWidth = 84;
constexpr int kLabelHeight = 16;
constexpr int kKnobCellHeight = 104;
constexpr int kToggleHeight = 30;
constexpr int kChoiceCellHeight = 50;
constexpr int kChoiceBoxHeight = 28;

constexpr int kMatrixSourceWidth = 120;
constexpr int kMatrixScaleWidth = 150;
constexpr int kMatrixDestWidth = 190;
constexpr int kMatrixColumnGap = 8;
constexpr int kMatrixHeaderHeight = 18;
constexpr int kMatrixRowHeight = 28;
constexpr int kMatrixControlHeight = 24;

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

int columnsForGroup(const std::string& name) {
    if (name == "Effects") return 6;
    if (name == "Filter") return 4;
    return 4;
}

bool isMatrixGroup(const std::string& name) { return name == "Mod Matrix"; }

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
        g.setColour(slider.findColour(juce::Slider::backgroundColourId));
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
            g.setColour(slider.findColour(juce::Slider::trackColourId));
            g.fillRoundedRectangle(a, cy - trackHeight * 0.5f, b - a, trackHeight, 2.0f);
        }
        g.setColour(slider.findColour(juce::Slider::thumbColourId));
        g.fillEllipse(sliderPos - 6.0f, cy - 6.0f, 12.0f, 12.0f);
    }

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
    explicit Content(juce::AudioProcessorValueTreeState& state) {
        int count = 0;
        const snakeoil::ParamSpec* specs = snakeoil::paramSpecs(count);

        // Group -> parameter indices, built once, in registry order.
        std::vector<std::pair<std::string, std::vector<int>>> byGroup;
        for (int i = 0; i < count; ++i) {
            const std::string name = specs[i].group;
            auto it = std::find_if(byGroup.begin(), byGroup.end(), [&](const auto& entry) { return entry.first == name; });
            if (it == byGroup.end()) {
                byGroup.push_back({name, {}});
                it = byGroup.end() - 1;
            }
            it->second.push_back(i);
        }

        controls_.reserve(static_cast<std::size_t>(count));
        for (const auto& [name, indices] : byGroup) {
            groups_.emplace_back();
            auto& group = groups_.back();
            group.box = std::make_unique<juce::GroupComponent>(name, name);
            group.box->setTextLabelPosition(juce::Justification::centredLeft);
            group.box->setInterceptsMouseClicks(false, false);
            addAndMakeVisible(*group.box);
            if (isMatrixGroup(name)) {
                buildMatrixGroup(group, indices, specs, state);
            } else {
                buildGridGroup(group, columnsForGroup(name), indices, specs, state);
            }
        }
    }

    /** Pack the groups left to right for the given width, wrapping rows. Returns the content height. */
    int layoutForWidth(int width) {
        if (width == getWidth() && getHeight() > 0) {
            return getHeight();
        }
        int x = kBodyMargin;
        int y = kBodyMargin;
        int rowHeight = 0;
        for (auto& group : groups_) {
            if (x > kBodyMargin && x + group.width > width - kBodyMargin) {
                x = kBodyMargin;
                y += rowHeight + kGroupGap;
                rowHeight = 0;
            }
            group.box->setBounds(x, y, group.width, group.height);
            for (const auto& item : group.items) {
                item.component->setBounds(item.rel.translated(x, y));
            }
            x += group.width + kGroupGap;
            rowHeight = std::max(rowHeight, group.height);
        }
        const int height = y + rowHeight + kBodyMargin;
        setSize(width, height);
        return height;
    }

private:
    struct Control {
        std::unique_ptr<juce::Label> label;
        std::unique_ptr<juce::Slider> slider;
        std::unique_ptr<juce::ToggleButton> toggle;
        std::unique_ptr<juce::ComboBox> combo;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> comboAttachment;
    };

    struct Item {
        juce::Component* component;
        juce::Rectangle<int> rel;  // relative to the group's top-left corner
    };

    struct Group {
        std::unique_ptr<juce::GroupComponent> box;
        std::vector<Item> items;
        int width = 0;
        int height = 0;
    };

    struct Cell {
        std::size_t control;
        int col;
        int span;
        int height;
        int row;
    };

    juce::Label* makeLabel(const juce::String& text, juce::Justification justification) {
        auto label = std::make_unique<juce::Label>(juce::String(), text);
        label->setJustificationType(justification);
        label->setFont(juce::Font(juce::FontOptions(12.5f)));
        label->setInterceptsMouseClicks(false, false);
        label->setBorderSize(juce::BorderSize<int>(0, 2, 0, 2));
        auto* raw = label.get();
        addAndMakeVisible(*raw);
        extraLabels_.push_back(std::move(label));
        return raw;
    }

    void styleRotary(Control& control, const snakeoil::ParamSpec& spec, juce::AudioProcessorValueTreeState& state) {
        control.slider = std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag,
                                                        juce::Slider::TextBoxBelow);
        control.slider->setName(spec.id);
        control.slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 76, 18);
        control.slider->setScrollWheelEnabled(false);
        control.sliderAttachment =
            std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, spec.id, *control.slider);
        applyDecimals(*control.slider, spec);
    }

    static void applyDecimals(juce::Slider& slider, const snakeoil::ParamSpec& spec) {
        const int decimals = decimalPlacesForRange(static_cast<double>(spec.maximum) - static_cast<double>(spec.minimum));
        slider.setNumDecimalPlacesToDisplay(decimals);
        slider.textFromValueFunction = [decimals](double value) { return juce::String(value, decimals); };
        slider.updateText();
    }

    static void fillCombo(juce::ComboBox& combo, const snakeoil::ParamSpec& spec) {
        combo.setName(spec.id);
        combo.addItemList(splitChoices(spec.choices), 1);
        combo.setJustificationType(juce::Justification::centredLeft);
    }

    void buildGridGroup(Group& group, int maxColumns, const std::vector<int>& indices,
                        const snakeoil::ParamSpec* specs, juce::AudioProcessorValueTreeState& state) {
        // Spans and heights per control.
        std::vector<Cell> cells;
        int totalSpan = 0;
        int widestSpan = 1;
        std::vector<std::size_t> created;
        for (int index : indices) {
            const auto& spec = specs[index];
            controls_.emplace_back();
            auto& control = controls_.back();
            created.push_back(controls_.size() - 1);
            int span = 1;
            int height = kKnobCellHeight;
            if (spec.kind == snakeoil::ParamKind::Toggle) {
                control.toggle = std::make_unique<juce::ToggleButton>(spec.label);
                control.toggle->setName(spec.id);
                control.buttonAttachment =
                    std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(state, spec.id,
                                                                                           *control.toggle);
                addAndMakeVisible(*control.toggle);
                span = 2;
                height = kToggleHeight;
            } else if (spec.kind == snakeoil::ParamKind::Choice) {
                control.label = std::make_unique<juce::Label>(juce::String(), spec.label);
                control.combo = std::make_unique<juce::ComboBox>();
                fillCombo(*control.combo, spec);
                control.comboAttachment =
                    std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(state, spec.id,
                                                                                             *control.combo);
                addAndMakeVisible(*control.label);
                addAndMakeVisible(*control.combo);
                span = 2;
                height = kChoiceCellHeight;
            } else {
                control.label = std::make_unique<juce::Label>(juce::String(), spec.label);
                styleRotary(control, spec, state);
                addAndMakeVisible(*control.label);
                addAndMakeVisible(*control.slider);
            }
            if (control.label != nullptr) {
                control.label->setJustificationType(span == 1 ? juce::Justification::centred
                                                              : juce::Justification::centredLeft);
                control.label->setFont(juce::Font(juce::FontOptions(12.5f)));
                control.label->setInterceptsMouseClicks(false, false);
            }
            cells.push_back({controls_.size() - 1, 0, span, height, 0});
            totalSpan += span;
            widestSpan = std::max(widestSpan, span);
        }

        // Narrow groups only take the columns they need.
        const int columns = std::max(widestSpan, std::min(maxColumns, totalSpan));

        // Flow the cells left to right, wrapping at the column count.
        int col = 0;
        int row = 0;
        std::vector<int> rowHeights;
        for (auto& cell : cells) {
            if (col + cell.span > columns) {
                col = 0;
                ++row;
            }
            cell.col = col;
            cell.row = row;
            col += cell.span;
            if (static_cast<int>(rowHeights.size()) <= row) {
                rowHeights.resize(static_cast<std::size_t>(row) + 1, 0);
            }
            rowHeights[static_cast<std::size_t>(row)] = std::max(rowHeights[static_cast<std::size_t>(row)], cell.height);
        }
        std::vector<int> rowTops(rowHeights.size(), kGroupTopInset);
        for (std::size_t r = 1; r < rowHeights.size(); ++r) {
            rowTops[r] = rowTops[r - 1] + rowHeights[r - 1];
        }

        for (const auto& cell : cells) {
            auto& control = controls_[cell.control];
            const int x = kGroupPadding + cell.col * kCellWidth;
            const int rowHeight = rowHeights[static_cast<std::size_t>(cell.row)];
            const int y = rowTops[static_cast<std::size_t>(cell.row)] + (rowHeight - cell.height) / 2;
            const int w = cell.span * kCellWidth - 4;
            if (control.toggle != nullptr) {
                group.items.push_back({control.toggle.get(), {x + 2, y, w, kToggleHeight}});
            } else if (control.combo != nullptr) {
                group.items.push_back({control.label.get(), {x + 2, y, w, kLabelHeight}});
                group.items.push_back({control.combo.get(), {x + 2, y + kLabelHeight + 2, w, kChoiceBoxHeight}});
            } else {
                group.items.push_back({control.label.get(), {x + 2, y, w, kLabelHeight}});
                group.items.push_back(
                    {control.slider.get(), {x + 2, y + kLabelHeight, w, kKnobCellHeight - kLabelHeight}});
            }
        }
        group.width = 2 * kGroupPadding + columns * kCellWidth;
        group.height = kGroupTopInset + (rowTops.empty() ? 0 : rowTops.back() - kGroupTopInset + rowHeights.back()) +
                       kGroupPadding;
    }

    void buildMatrixGroup(Group& group, const std::vector<int>& indices, const snakeoil::ParamSpec* specs,
                          juce::AudioProcessorValueTreeState& state) {
        const int xSource = kGroupPadding;
        const int xScale = xSource + kMatrixSourceWidth + kMatrixColumnGap;
        const int xDest = xScale + kMatrixScaleWidth + kMatrixColumnGap;

        // Header row: Source / Scale / Destination.
        const struct {
            const char* text;
            int x;
            int w;
        } headers[] = {{"Source", xSource, kMatrixSourceWidth},
                       {"Scale", xScale, kMatrixScaleWidth},
                       {"Destination", xDest, kMatrixDestWidth}};
        for (const auto& header : headers) {
            auto* label = makeLabel(header.text, juce::Justification::centredLeft);
            label->setColour(juce::Label::textColourId, kTextDim);
            group.items.push_back({label, {header.x, kGroupTopInset, header.w, kMatrixHeaderHeight}});
        }

        const int rowsTop = kGroupTopInset + kMatrixHeaderHeight + 4;
        int row = 0;
        int column = 0;
        for (int index : indices) {
            const auto& spec = specs[index];
            controls_.emplace_back();
            auto& control = controls_.back();
            const int y = rowsTop + row * kMatrixRowHeight + (kMatrixRowHeight - kMatrixControlHeight) / 2;
            if (spec.kind == snakeoil::ParamKind::Choice) {
                control.combo = std::make_unique<juce::ComboBox>();
                fillCombo(*control.combo, spec);
                control.comboAttachment =
                    std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(state, spec.id,
                                                                                             *control.combo);
                addAndMakeVisible(*control.combo);
                const bool isSource = column == 0;
                group.items.push_back({control.combo.get(),
                                       {isSource ? xSource : xDest, y, isSource ? kMatrixSourceWidth : kMatrixDestWidth,
                                        kMatrixControlHeight}});
            } else {
                control.slider = std::make_unique<juce::Slider>(juce::Slider::LinearHorizontal,
                                                                juce::Slider::TextBoxRight);
                control.slider->setName(spec.id);
                control.slider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
                control.slider->setScrollWheelEnabled(false);
                control.sliderAttachment =
                    std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, spec.id,
                                                                                           *control.slider);
                applyDecimals(*control.slider, spec);
                addAndMakeVisible(*control.slider);
                group.items.push_back({control.slider.get(), {xScale, y, kMatrixScaleWidth, kMatrixControlHeight}});
            }
            if (++column == 3) {
                column = 0;
                ++row;
            }
        }
        const int rows = (static_cast<int>(indices.size()) + 2) / 3;
        group.width = 2 * kGroupPadding + kMatrixSourceWidth + kMatrixScaleWidth + kMatrixDestWidth +
                      2 * kMatrixColumnGap;
        group.height = rowsTop + rows * kMatrixRowHeight + kGroupPadding;
    }

    std::vector<Control> controls_;
    std::vector<std::unique_ptr<juce::Label>> extraLabels_;
    std::vector<Group> groups_;
};

// --- Editor -------------------------------------------------------------------------------------------

SnakeOilEditor::SnakeOilEditor(SnakeOilProcessor& processor)
    : AudioProcessorEditor(&processor), processor_(processor) {
    laf_ = std::make_unique<SnakeOilLookAndFeel>();
    setLookAndFeel(laf_.get());

    content_ = std::make_unique<Content>(processor_.state());
    viewport_.setViewedComponent(content_.get(), false);
    viewport_.setScrollBarsShown(true, false);
    viewport_.setScrollBarThickness(kScrollBarThickness);
    addAndMakeVisible(viewport_);
    addAndMakeVisible(meter_);
    sendLookAndFeelChange();  // children built before parenting must pick up the themed colours

    const int contentHeight = content_->layoutForWidth(kContentWidth);
    const int fullWidth = kContentWidth + kScrollBarThickness;
    const int fullHeight = contentHeight + kHeaderHeight;

    setResizable(true, true);
    setResizeLimits(640, 420, fullWidth, fullHeight);
    setSize(std::min(fullWidth, 1220), std::min(fullHeight, 780));
    startTimerHz(15);
}

SnakeOilEditor::~SnakeOilEditor() {
    stopTimer();
    setLookAndFeel(nullptr);
}

void SnakeOilEditor::paint(juce::Graphics& g) {
    g.fillAll(kBackground);
    g.setColour(kOutline);
    g.fillRect(0, kHeaderHeight - 1, getWidth(), 1);
    g.setColour(kText);
    g.setFont(juce::Font(juce::FontOptions(18.0f, juce::Font::bold)));
    g.drawText("SnakeOil Synth", 14, 0, 300, kHeaderHeight - 1, juce::Justification::centredLeft, false);
}

void SnakeOilEditor::resized() {
    auto bounds = getLocalBounds();
    auto header = bounds.removeFromTop(kHeaderHeight);
    header.removeFromRight(14);
    meter_.setBounds(header.removeFromRight(LevelMeter::kPreferredWidth)
                         .withSizeKeepingCentre(LevelMeter::kPreferredWidth, LevelMeter::kPreferredHeight));
    viewport_.setBounds(bounds);
    const int available = std::max(kMinContentWidth, bounds.getWidth() - viewport_.getScrollBarThickness());
    content_->layoutForWidth(std::min(kContentWidth, available));
}

void SnakeOilEditor::timerCallback() {
    double left = 0.0;
    double right = 0.0;
    bool clipped = false;
    processor_.engineMeter(left, right, clipped);
    meter_.update(left, right, clipped, juce::Time::getMillisecondCounterHiRes());
}
