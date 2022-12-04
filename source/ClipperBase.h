#pragma once

class ClipperBase
{
public:
	ClipperBase() {}
	~ClipperBase() {}

	void reset (float Fs)
	{
		Ts = 1.f / Fs;
		updateCoefficients();
	}

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

    float getCapResistance(float C)
    {
    	return Ts / (2.f * C);
    }

    static float symmetricDiodes(float Vin, bool isDenom = false, float n = 1.f)
	{
		const float eta = 1.f;
		const float Is = (float) 1e-15;
		const float Vt = (float) 26e-3;

		float Vd = 0.f;

		if (isDenom)
    	{
    		Vd = 2.f * Is / (n * eta * Vt) * cosh(Vin / (n * eta * Vt));
    	}
    	else
    	{
    		Vd = 2.f * Is * sinh(Vin / (n * eta * Vt));
    	}

    	return Vd;
	}

	static float positiveDiode(float Vin, bool isDenom = false, float n = 1.f)
    {
    	const float eta = 1.2f;
    	const float Is = (float) 10e-12;
    	const float Vt = (float) 26e-3;

    	float Vd = 0.f;

    	if (isDenom)
    	{
    		Vd = (Is / (n * eta * Vt)) * exp(Vin / (n * eta * Vt));
    	}
    	else
    	{
    		Vd = Is * (exp(Vin / (n * eta * Vt)) - 1);
    	}

    	return Vd;
    }

    static float negativeDiode(float Vin, bool isDenom = false, float n = 1.f)
    {
    	const float eta = 1.2f;
    	const float Is = (float) 10e-12;
    	const float Vt = (float) 26e-3;

    	float Vd = 0.f;

    	if (isDenom)
    	{
    		Vd = (Is / (n * eta * Vt)) * exp(-Vin / (n * eta * Vt));
    	}
    	else
    	{
    		Vd = -Is * (exp(-Vin / (n * eta * Vt)) - 1);
    	}

    	return Vd;
    }

private:
	float Ts = 1.f / 44100.0f;

	virtual float processSingleSample(float Vin)
	{
		float Vout = Vin;

		return Vout;
	}

	virtual void updateCoefficients() {}
};