#pragma once

#include <JuceHeader.h>

namespace EQProcessing
{
	class BandProcessor
	{
	public:
		BandProcessor(juce::AudioProcessorValueTreeState& shared_apvts, int shared_index);
		~BandProcessor();

		BandProcessor(const BandProcessor&) = delete;
		BandProcessor& operator=(const BandProcessor&) = delete;

		BandProcessor(BandProcessor&&) = default;
		BandProcessor& operator=(BandProcessor&&) = default;

		void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);

		void process(juce::AudioBuffer<float>& buffer, double sampleRate);

	private:
		using Filter = juce::dsp::IIR::Filter<float>;

		Filter leftBand, rightBand;

		juce::AudioProcessorValueTreeState& apvts;

		int index;
	};
}