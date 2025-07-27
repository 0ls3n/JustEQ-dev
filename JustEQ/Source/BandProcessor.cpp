/*
  ==============================================================================

    BandProcessor.cpp
    Created: 27 Jul 2025 7:27:07pm
    Author:  rasmu

  ==============================================================================
*/

#include "BandProcessor.h"

EQProcessing::BandProcessor::BandProcessor(juce::AudioProcessorValueTreeState& shared_apvts, int shared_index) : apvts(shared_apvts), index(shared_index)
{

}

EQProcessing::BandProcessor::~BandProcessor() = default;

void EQProcessing::BandProcessor::prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels)
{
    juce::dsp::ProcessSpec spec;

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = numChannels;

    leftBand.prepare(spec);
    rightBand.prepare(spec);
}

void EQProcessing::BandProcessor::process(juce::AudioBuffer<float>& buffer, double sampleRate)
{
    juce::dsp::AudioBlock<float> block(buffer);

    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    auto prefix = "Band" + juce::String(index + 1);

    bool active = *apvts.getRawParameterValue(prefix + "_Active");
    if (!active)
        return;

    float freq = *apvts.getRawParameterValue(prefix + "_Freq");
    float gain = *apvts.getRawParameterValue(prefix + "_Gain");
    float Q = *apvts.getRawParameterValue(prefix + "_Q");
    int type = *apvts.getRawParameterValue(prefix + "_Type");

    switch (type)
    {
    case 0: // Bell
        *leftBand.coefficients = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            sampleRate, freq, Q, juce::Decibels::decibelsToGain(gain));

        *rightBand.coefficients = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            sampleRate, freq, Q, juce::Decibels::decibelsToGain(gain));
        break;

    case 1: // Low Shelf
        *leftBand.coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowPass(
            sampleRate, freq, Q);

        *rightBand.coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowPass(
            sampleRate, freq, Q);
        break;

    case 2: // High Shelf
        *leftBand.coefficients = *juce::dsp::IIR::Coefficients<float>::makeHighPass(
            sampleRate, freq, Q);

        *rightBand.coefficients = *juce::dsp::IIR::Coefficients<float>::makeHighPass(
            sampleRate, freq, Q);
        break;

    case 3: // Notch
        *leftBand.coefficients = *juce::dsp::IIR::Coefficients<float>::makeNotch(
            sampleRate, freq, Q);

        *rightBand.coefficients = *juce::dsp::IIR::Coefficients<float>::makeNotch(
            sampleRate, freq, Q);
        break;
    }

    leftBand.process(juce::dsp::ProcessContextReplacing<float>(leftBlock));
    rightBand.process(juce::dsp::ProcessContextReplacing<float>(rightBlock));

}
