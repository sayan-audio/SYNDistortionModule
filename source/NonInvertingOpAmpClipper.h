class NonInvertingOpAmpClipper
{
public:
	NonInvertingOpAmpClipper() {}
	~NonInvertingOpAmpClipper() {}

	void prepare (float newFs)
	{
		if (Fs != newFs)
		{
			Fs = newFs;
			updateCoefficients();
		}
	}

	void reset() {}

	template <typename Context>
    void process (Context& context)
    {
    	auto&& inputBlock  = context.getInputBlock();
    	auto&& outputBlock = context.getOutputBlock();
    	auto numSamples  = inputBlock.getNumSamples();
    	auto numChannels = inputBlock.getNumChannels();

    	for (size_t channel = 0; channel < numChannels; ++channel)
    	{
    		auto* src = inputBlock .getChannelPointer (channel);
    		auto* dst = outputBlock.getChannelPointer (channel);

    		if (context.isBypassed)
    		{
    			for (size_t i = 0; i < numSamples; ++i)
    			{
    				dst[i] = src[i];
    			}
    		}
    		else
    		{
    			for (size_t i = 0; i < numSamples; ++i)
    			{
    				dst[i] = processSingleSample(src[i]);
    			}
    		}
    	}
    }
private:
	float Fs = 44100.f; // Initial sample rate
	float Ts = 1.f / Fs;

	float processSingleSample(float Vin) {
		float Vout = Vin;

		return Vout;
	}

	void updateCoefficients()
	{
		Ts = 1.f / Fs;
	}

	//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NonInvertingOpAmpClipper)
};