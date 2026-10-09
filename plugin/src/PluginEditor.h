#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include <memory>

#include "LevelMeter.h"

class SnakeOilProcessor;

/**
 * Plugin editor: a slim title strip above a scrollable body. The body is a juce::Viewport
 * showing a Content component that lays the group boxes out on the same hand-placed
 * five-column grid as the Python app (see the cell table in PluginEditor.cpp).
 */
class SnakeOilEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
    explicit SnakeOilEditor(SnakeOilProcessor& processor);
    ~SnakeOilEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    /** The output meter in the Master box; exposed so tools can drive it without a running audio engine. */
    LevelMeter& levelMeter();

private:
    class Content;

    void timerCallback() override;

    SnakeOilProcessor& processor_;
    std::unique_ptr<juce::LookAndFeel_V4> laf_;
    std::unique_ptr<Content> content_;
    juce::Viewport viewport_;
    juce::TooltipWindow tooltips_{this, 700};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SnakeOilEditor)
};
