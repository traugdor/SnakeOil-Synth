#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include <memory>

#include "LevelMeter.h"

class SnakeOilProcessor;

/**
 * Plugin editor: a header strip (title and level meter) above a scrollable body.
 * The body is a juce::Viewport showing a Content component that holds one group box
 * per parameter group, packed left to right and wrapped to the available width.
 */
class SnakeOilEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
    explicit SnakeOilEditor(SnakeOilProcessor& processor);
    ~SnakeOilEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    /** The header meter; exposed so tools can drive it without a running audio engine. */
    LevelMeter& levelMeter() { return meter_; }

private:
    class Content;

    void timerCallback() override;

    SnakeOilProcessor& processor_;
    std::unique_ptr<juce::LookAndFeel_V4> laf_;
    std::unique_ptr<Content> content_;
    juce::Viewport viewport_;
    LevelMeter meter_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SnakeOilEditor)
};
