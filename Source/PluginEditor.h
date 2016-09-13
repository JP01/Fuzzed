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
#include "MyLookAndFeel.h"

//==============================================================================
/**
*/
class FuzzFaceJuceAudioProcessorEditor  : public AudioProcessorEditor, private Timer
{
public:
    FuzzFaceJuceAudioProcessorEditor (FuzzFaceJuceAudioProcessor&);
    ~FuzzFaceJuceAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
	void timerCallback() override;
private:
	class ParameterSlider; //class for controlling parameters

	Label volLabel, fuzzLabel, gainLabel; //Labels for the fuzz and vol params + gainLabel


	//Create a scoped pointer to the custom look and feel class used for custom UI
	ScopedPointer<MyLookAndFeel> paramLookAndFeel = new MyLookAndFeel(PARAM_KNOB);
	ScopedPointer<MyLookAndFeel> gainLookAndFeel = new MyLookAndFeel(GAIN_KNOB);

	//Creates pointers for volSlider and fuzzSlider which are automatically deleted once out of scope + gainSlider
	ScopedPointer<ParameterSlider> volSlider, fuzzSlider, gainSlider; 
	
	//Set up the requirements for the metering
	ScopedPointer<MyLookAndFeel> meterLookAndFeel = new MyLookAndFeel(METER);
	ScopedPointer<Slider> meterSlider = new Slider();


	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	FuzzFaceJuceAudioProcessor& processor;
	

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FuzzFaceJuceAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
