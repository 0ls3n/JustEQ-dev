/*
  ==============================================================================

    BrandingHeader.h
    Created: 24 Jul 2025 10:30:52pm
    Author:  rasmu

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ApplicationColours.h"

//==============================================================================
/*
*/
class BrandingHeader  : public juce::Component
{
public:
    BrandingHeader(juce::String pluginName, juce::String brandName);
    ~BrandingHeader() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:

    juce::Label pluginNameLabel;
    juce::Label brandNameLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BrandingHeader)
};
