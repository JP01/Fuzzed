/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
FuzzFaceJuceAudioProcessorEditor::FuzzFaceJuceAudioProcessorEditor (FuzzFaceJuceAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    //Sets the default window size of the plugin
    setSize (400, 300);

	//define the parameters of the fuzzControl slider object
	fuzzControl.setSliderStyle(Slider::Rotary);  //creates a rotary object
	fuzzControl.setRange(CTRL_MIN, CTRL_MAX, CTRL_INCREMENT);  //sets the min, max, and increment values for the rotary
	fuzzControl.setTextBoxStyle(Slider::TextBoxBelow, false, 190, 20); //sets the textbox position, makes it editable, horizontal and vertical pixels
	fuzzControl.setPopupDisplayEnabled(false, this);  //removes the popup display of current value
	fuzzControl.setTextValueSuffix(" "); //adds a suffix to the end of the current setting
	fuzzControl.setValue(FUZZ_DEFAULT);  //sets the default setting

	//define the parameters of the volControl slider object
	volControl.setSliderStyle(Slider::Rotary);  //creates a rotary object
	volControl.setRange(CTRL_MIN, CTRL_MAX, CTRL_INCREMENT);  //sets the min, max, and increment values for the rotary
	volControl.setTextBoxStyle(Slider::TextBoxBelow, false, 190, 20); //sets the textbox position, makes it editable, horizontal and vertical pixels
	volControl.setPopupDisplayEnabled(false, this);  //removes the popup display of current value
	volControl.setTextValueSuffix(" Vol"); //adds a suffix to the end of the current setting
	volControl.setValue(VOL_DEFAULT);  //sets the default setting
	

	//Add the controls to the GUI
	addAndMakeVisible(&fuzzControl);
	addAndMakeVisible(&volControl);

	//Add listeners to sliders
	fuzzControl.addListener(this);
	volControl.addListener(this);
	
}

FuzzFaceJuceAudioProcessorEditor::~FuzzFaceJuceAudioProcessorEditor()
{
}

//==============================================================================
void FuzzFaceJuceAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::white);

    g.setColour (Colours::black);
    g.setFont (15.0f);
    g.drawFittedText ("FuzzFace", getLocalBounds(), Justification::centredTop, 1);
}

void FuzzFaceJuceAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
	fuzzControl.setBounds(240, 100, 100, 100);
	volControl.setBounds(60, 100, 100, 100);
}

//===========================================================
void FuzzFaceJuceAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
	//The idea for smoothing is that you Set the value when the slider is moved,
	// but only getNextValue() in the processBlock


	//sets the fuzzVal in the processor class 
	processor.fuzzVal = fuzzControl.getValue();

	//sets the volVal in the processor class 
	processor.volVal = volControl.getValue();

	std::ostringstream convert; 

	convert << processor.smoothedFuzz;
	actualFuzz = convert.str();
	fuzzControl.setTextValueSuffix("   " + actualFuzz);
}

