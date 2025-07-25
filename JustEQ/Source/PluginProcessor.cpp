/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JustEQAudioProcessor::JustEQAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

JustEQAudioProcessor::~JustEQAudioProcessor()
{
}

//==============================================================================
const juce::String JustEQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JustEQAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool JustEQAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool JustEQAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double JustEQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JustEQAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int JustEQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void JustEQAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String JustEQAudioProcessor::getProgramName (int index)
{
    return {};
}

void JustEQAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void JustEQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec{ sampleRate, static_cast<juce::uint32>(samplesPerBlock), static_cast<juce::uint32>(getTotalNumOutputChannels())};

    leftFilters.clear();
    leftFilters.resize(16);

    rightFilters.clear();
    rightFilters.resize(16);

    for (auto &filter : leftFilters)
    {
        filter.prepare(spec);
    }

    for (auto &filter : rightFilters)
    {
        filter.prepare(spec);
    }
}

void JustEQAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JustEQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void JustEQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::dsp::AudioBlock<float> block(buffer);

    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    for (int i = 0; i < 16; i++)
    {
        auto prefix = "Band" + juce::String(i + 1);

        bool active = *apvts.getRawParameterValue(prefix + "_Active");
        if (!active)
            continue;

        float freq = *apvts.getRawParameterValue(prefix + "_Freq");
        float gain = *apvts.getRawParameterValue(prefix + "_Gain");
        float Q = *apvts.getRawParameterValue(prefix + "_Q");
        int type = *apvts.getRawParameterValue(prefix + "_Type");

        auto& leftFilter = leftFilters[i];
        auto& rightFilter = rightFilters[i];

        switch (type)
        {
        case 0: // Bell
            *leftFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                getSampleRate(), freq, Q, juce::Decibels::decibelsToGain(gain));

            *rightFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                getSampleRate(), freq, Q, juce::Decibels::decibelsToGain(gain));
            break;

        case 1: // Low Shelf
            *leftFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowPass(
                getSampleRate(), freq, Q);
            *rightFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowPass(
                getSampleRate(), freq, Q);
            break;

        case 2: // High Shelf
            *leftFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeHighPass(
                getSampleRate(), freq, Q);
            *rightFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeHighPass(
                getSampleRate(), freq, Q);
            break;

        case 3: // Notch
            *leftFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeNotch(
                getSampleRate(), freq, Q);
            *rightFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeNotch(
                getSampleRate(), freq, Q);
            break;
        }

        leftFilter.process(juce::dsp::ProcessContextReplacing<float>(leftBlock));
        rightFilter.process(juce::dsp::ProcessContextReplacing<float>(rightBlock));
    }
}

//==============================================================================
bool JustEQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* JustEQAudioProcessor::createEditor()
{
    //return new JustEQAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void JustEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void JustEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout JustEQAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    for (int i = 0; i < 16; ++i)
    {
        layout.add(std::make_unique<juce::AudioParameterChoice>(
            "Band" + juce::String(i + 1) + "_Type", "Band" + juce::String(i + 1) + " Type",
            juce::StringArray{ "Bell", "Lowpass", "Highpass", "Notch" }, 0));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            "Band" + juce::String(i + 1) + "_Freq", "Band" + juce::String(i + 1) + " Frequency",
            juce::NormalisableRange<float>(20.0f, 20000.0f, 0.01f, 0.5f), 1000.0f));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            "Band" + juce::String(i + 1) + "_Gain", "Band" + juce::String(i + 1) + " Gain",
            juce::NormalisableRange<float>(-24.0f, 24.0f, 0.01f), 0.0f));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            "Band" + juce::String(i + 1) + "_Q", "Band" + juce::String(i + 1) + " Q",
            juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f), 1.0f));

        layout.add(std::make_unique<juce::AudioParameterBool>(
            "Band" + juce::String(i + 1) + "_Active", "Band" + juce::String(i + 1) + " Active",
            false));

        layout.add(std::make_unique<juce::AudioParameterBool>(
            "Band" + juce::String(i + 1) + "_Visible", "Band" + juce::String(i + 1) + " Visible",
            false));
    }

    return layout; 
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JustEQAudioProcessor();
}
