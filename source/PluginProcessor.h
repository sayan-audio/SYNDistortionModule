#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "ParameterReferences.h"

//==============================================================================
class AudioPluginAudioProcessor  : public juce::AudioProcessor, private juce::ValueTree::Listener
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    AudioPluginAudioProcessor(juce::AudioProcessorValueTreeState::ParameterLayout layout);

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void reset() override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    void update();
    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override;
    
    ParameterReferences parameters;
    juce::AudioProcessorValueTreeState apvts;

    enum ProcessorIndices
    {
        inputGainIndex,
        outputGainIndex
    };

    using Chain = juce::dsp::ProcessorChain<juce::dsp::Gain<float>, juce::dsp::Gain<float>>;
    Chain chain;

    std::atomic<bool> requiresUpdate { true };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
