/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JustEQAudioProcessorEditor::JustEQAudioProcessorEditor (JustEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1000, 600);
    setResizable(false, false);

    addAndMakeVisible(brandingHeader);
}

JustEQAudioProcessorEditor::~JustEQAudioProcessorEditor()
{
}

//==============================================================================
void JustEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(ApplicationColours::background);
}

void JustEQAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    brandingHeader.setBounds(area.removeFromTop(60));
}
