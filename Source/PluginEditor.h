/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include <sstream>

#define CTRL_INCREMENT 0.01 //The increment of the control knobs
#define CTRL_MIN 0.01
#define CTRL_MAX 0.99

#define FUZZ_DEFAULT 0.6
#define VOL_DEFAULT 0.4

//==============================================================================
/**
*/
class FuzzFaceJuceAudioProcessorEditor  : public AudioProcessorEditor, private Slider::Listener
{
public:
    FuzzFaceJuceAudioProcessorEditor (FuzzFaceJuceAudioProcessor&);
    ~FuzzFaceJuceAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FuzzFaceJuceAudioProcessor& processor;

	Slider fuzzControl; //slider for controlling fuzz
	Slider volControl; //slider for controlling vol

	String actualFuzz; //string showing actual value of fuzz
	String actualVol; //string for showing the actual value of vol

	void sliderValueChanged(Slider* slider) override; //method which changes fuzzVal or volVal when slider changes

	class ParameterSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FuzzFaceJuceAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
