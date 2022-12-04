#pragma once

#include "ParameterIds.h"

struct ParameterReferences
{
	using Parameter = juce::AudioProcessorValueTreeState::Parameter;
	using Attributes = juce::AudioProcessorValueTreeStateParameterAttributes;

	template <typename Param>
	static void add(juce::AudioProcessorParameterGroup& group, std::unique_ptr<Param> param)
	{
		group.addChild(std::move(param));
	}

	template <typename Param>
	static void add(juce::AudioProcessorValueTreeState::ParameterLayout& group, std::unique_ptr<Param> param)
	{
		group.add(std::move(param));
	}

	template <typename Param, typename Group, typename... Ts>
	static Param& addToLayout(Group& layout, Ts&&... ts)
	{
		auto param = new Param(std::forward<Ts>(ts)...);
		auto& ref = *param;
		add(layout, juce::rawToUniquePtr(param));
		return ref;
	}

	static juce::String valueToTextFunction(float x, int)
	{
		return juce::String(x, 2);
	}

	static float textToValueFunction(const juce::String& str)
	{
		return str.getFloatValue();
	}

	static auto getBasicAttributes()
	{
		return Attributes()
			.withStringFromValueFunction(valueToTextFunction)
			.withValueFromStringFunction(textToValueFunction);
	}

	static auto getDbAttributes()
	{
		return getBasicAttributes().withLabel("dB");
	}

	struct MainGroup
	{
		MainGroup(juce::AudioProcessorParameterGroup& layout)
			: inputGain(addToLayout<Parameter>(
				layout,
				juce::ParameterID { ID::inputGain, 1 },
				"Input",
				juce::NormalisableRange<float>(-60.0f, 60.0f),
				0.0f,
				getDbAttributes())),
			  distInputGain(addToLayout<Parameter>(
			  	layout,
			  	juce::ParameterID { ID::distInputGain, 1 },
			  	"Dist. Input Gain",
			  	juce::NormalisableRange<float>(-60.0f, 60.0f),
			  	0.0f,
			  	getDbAttributes())),
			  distCompGain(addToLayout<Parameter>(
			  	layout,
			  	juce::ParameterID { ID::distCompGain, 1 },
			  	"Dist. Comp Gain",
			  	juce::NormalisableRange<float>(-60.0f, 60.0f),
			  	0.0f,
			  	getDbAttributes())),
			  outputGain(addToLayout<Parameter>(
			  	layout,
			  	juce::ParameterID { ID::outputGain, 1 },
			  	"Output",
			  	juce::NormalisableRange<float>(-60.0f, 0.0f),
			  	0.0f,
			  	getDbAttributes()))
		{}

		Parameter& inputGain;
		Parameter& distInputGain;
		Parameter& distCompGain;
		Parameter& outputGain;

	};

	ParameterReferences(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
		: main(addToLayout<juce::AudioProcessorParameterGroup>(layout, "main", "Main", "|"))
	{}

	MainGroup main;
};