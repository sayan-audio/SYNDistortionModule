/*
 * TODOs:
 * 1. [x] I'll digitise the Non-Inverting Op Amp Clipper Circuit
 * 2. [x] Mark variables as constants
 * 2. [x] Fix the warning for explicit conversion to float
 * 2. [x] Refactor Diodes into a function
 * 3. [x] Refactor the reset method
 * 4. [x] Refactor base class for processor
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

#include "ClipperBase.h"

class NonInvertingOpAmpClipper : public ClipperBase
{
public:
	NonInvertingOpAmpClipper() {}
	~NonInvertingOpAmpClipper() {}

private:
	// Components
	float C1 = (float) 47e-9;
	float R1 = getCapResistance(C1);
	const float R4 = 4700.f;

	float C2 = (float) 51e-12;
	float R2 = getCapResistance(C1);
	const float R3 = 51000.f + 500e3;

	// Combined Resistances
	float G1 = (1.f + R4 / R1);
	float G4 = (1.f + R1 / R4);

	// States
	float X1 = 0.f;
	float X2 = 0.f;
	float Vd = 0.f;

	const float thr = 0.00000000001f;

	float processSingleSample(float Vin)
	{
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
		R1 = getCapResistance(C1);
		R2 = getCapResistance(C2);

		G1 = (1.f + R4 / R1);
		G4 = (1.f + R1 / R4);
	}

	//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NonInvertingOpAmpClipper)
};