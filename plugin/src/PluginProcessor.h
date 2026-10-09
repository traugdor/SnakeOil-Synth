#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include <atomic>
#include <memory>
#include <vector>

#include "params_gen.hpp"
#include "snakeoil/engine.hpp"

class SnakeOilProcessor : public juce::AudioProcessor {
public:
    SnakeOilProcessor();
    ~SnakeOilProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState& state() { return apvts_; }
    // Peaks (linear) and the clip flag since the previous call, then resets them.
    void engineMeter(double& left, double& right, bool& clipped);

    // Hybrid voice allocation (mirrors midi_synth/config.py): 12 or 24 tail slots on
    // top of the 12 playable voices. A non-automatable setting kept in the state tree
    // property "tailSlots", not a registry parameter.
    static constexpr int kDefaultTailSlots = 12;
    static constexpr int kTailCapacity = 24;
    int tailSlots() const { return tailSlots_.load(std::memory_order_relaxed); }
    void setTailSlots(int slots);
    // Released notes still ringing, as of the last processed block.
    int tailCount() const { return tailCount_.load(std::memory_order_relaxed); }

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout makeLayout();
    void syncParametersToEngine();
    void handleMidi(const juce::MidiBuffer&);
    void loadNoiseTables();

    juce::AudioProcessorValueTreeState apvts_;
    std::unique_ptr<snakeoil::Engine> engine_;
    std::vector<std::atomic<float>*> rawValues_;
    std::vector<const snakeoil::ParamSpec*> specs_;
    std::vector<float> scratch_;
    std::atomic<int> tailSlots_{kDefaultTailSlots};
    std::atomic<int> tailCount_{0};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SnakeOilProcessor)
};
