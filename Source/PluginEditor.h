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

#define CTRL_INCREMENT 0.01 //The increment of the control knobs
#define CTRL_MIN 0.01
#define CTRL_MAX 0.99
#define TIMER_FREQ 30

#define FUZZ_DEFAULT 0.6
#define VOL_DEFAULT 0.4

#define WIN_WIDTH 400
#define WIN_HEIGHT 200

//==============================================================================
/**
*/
class FuzzFaceJuceAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    FuzzFaceJuceAudioProcessorEditor (FuzzFaceJuceAudioProcessor&);
    ~FuzzFaceJuceAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
	

private:
	class ParameterSlider; //class for controlling parameters

	Label volLabel, fuzzLabel; //Labels for the fuzz and vol params

	//Creates pointers for volSlider and fuzzSlider which are automatically deleted once out of scope
	ScopedPointer<ParameterSlider> volSlider, fuzzSlider; 
	
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	FuzzFaceJuceAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FuzzFaceJuceAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
