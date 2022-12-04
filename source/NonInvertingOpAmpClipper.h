/*
 * TODOs:
 * 1. [x] I'll digitise the Non-Inverting Op Amp Clipper Circuit
 * 2. [x] Mark variables as constants
 * 2. [x] Fix the warning for explicit conversion to float
 * 2. [x] Refactor Diodes into a function
 * 3. [x] Refactor the reset method
 * 4. Refactor base class for processor
 * 5. Ensure that the sample rate is being updated
 *
 * At each step make sure it works
 *
 * Notes:
 * - What if I take another class with my favourite clipper
 *   and add it to process after this block?
 *   In the same DistortionProcessor
 * - It'd be great to do tests by automating params in Cubase
 */

#pragma once

class NonInvertingOpAmpClipper
{
public:
	NonInvertingOpAmpClipper() {}
	~NonInvertingOpAmpClipper() {}

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
private:
	float Ts = 1.f / 44100.0f;

	// Components
	float C1 = (float) 47e-9;
	float R1 = Ts / (2.f * C1);
	const float R4 = 4700.f;

	float C2 = (float) 51e-12;
	float R2 = Ts / (2.f * C2);
	const float R3 = 51000.f + 500e3;

	// Combined Resistances
	float G1 = (1.f + R4 / R1);
	float G4 = (1.f + R1 / R4);

	// States
	float X1 = 0.f;
	float X2 = 0.f;
	float Vd = 0.f;

	const float thr = 0.00000000001f;

	float processSingleSample(float Vin) {
		size_t iter = 1;
		float b = 1.f;

		float p = -Vin / (G4 * R4) + R1 / (G4 * R4) * X1 - X2;

		float fVd = p + Vd / R2 + Vd / R3 + positiveDiode(Vd) + negativeDiode(Vd);

		while (iter < 50 && abs(fVd) > thr)
		{
			float fpVd = positiveDiode(Vd, true) + negativeDiode(Vd, true) + 1.f / R2 + 1.f / R3;
			float Vnew = Vd - b * fVd / fpVd;
			float fn = p + Vnew / R2 + Vnew / R3 + positiveDiode(Vnew) + negativeDiode(Vnew);

			if (abs(fn) < abs(fVd))
			{
				Vd = Vnew;
				b = 1.f;
			}
			else
			{
				b *= 0.5f;
			}

			fVd = p + Vd / R2 + Vd / R3 + positiveDiode(Vd) + negativeDiode(Vd);
			iter++;
		}

		float Vout = Vd + Vin;
		X1 = (2.f / R1) * (Vin / G1 + X1 * R4 / G1) - X1;
    	X2 = (2.f / R2) * (Vd) - X2;

		return Vout;
	}

	void updateCoefficients()
	{
		R1 = Ts / (2.f * C1);
		R2 = Ts / (2.f * C2);

		G1 = (1.f + R4 / R1);
		G4 = (1.f + R1 / R4);
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

	//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NonInvertingOpAmpClipper)
};