#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "ParameterReferences.h"
#include "NonInvertingOpAmpClipper.h"

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
    
    struct DistortionProcessor
    {
        DistortionProcessor() {}
        ~DistortionProcessor() {}

        void prepare (const juce::dsp::ProcessSpec& spec) {
            oversampler.initProcessing(spec.maximumBlockSize);

            distortion.reset(oversampler.getOversamplingFactor() * (float) spec.sampleRate);
        }

        void reset() {
            oversampler.reset();
        }

        template <typename Context>
        void process (Context& context)
        {
            if (context.isBypassed)
                return;

            const auto& inputBlock = context.getInputBlock();

            distInputGain.process(context);

            auto ovBlock = oversampler.processSamplesUp(inputBlock);
            juce::dsp::ProcessContextReplacing<float> distortionContext (ovBlock);

            distortion.process(context);

            auto& outputBlock = context.getOutputBlock();
            oversampler.processSamplesDown(outputBlock);

            distCompGain.process(context);
        }

        juce::dsp::Gain<float> distInputGain, distCompGain;
        NonInvertingOpAmpClipper distortion;
        juce::dsp::Oversampling<float> oversampler { 2, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, false };
    };

    ParameterReferences parameters;
    juce::AudioProcessorValueTreeState apvts;

    enum ProcessorIndices
    {
        inputGainIndex,
        distortionProcessorIndex,
        outputGainIndex
    };

    using Chain = juce::dsp::ProcessorChain<juce::dsp::Gain<float>, DistortionProcessor, juce::dsp::Gain<float>>;
    Chain chain;

    std::atomic<bool> requiresUpdate { true };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
