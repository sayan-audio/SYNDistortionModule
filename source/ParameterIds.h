#pragma once

namespace ID
{
	#define PARAMETER_ID(str) constexpr const char* str { #str };

	PARAMETER_ID(inputGain)
	PARAMETER_ID(outputGain)

	#undef PARAMETER_ID
}