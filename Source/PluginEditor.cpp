/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//Define the ParameterSlider class and methods
class FuzzFaceJuceAudioProcessorEditor::ParameterSlider : public Slider, private Timer
{
	public:
		ParameterSlider(AudioProcessorParameter& p) : Slider(p.getName(256)), param(p)
		{
			//Sets the range of the slider, min, max, increment
			setRange(CTRL_MIN, CTRL_MAX, CTRL_INCREMENT);

			//Start the timer with an interval of TIMER_FREQ hz, this is used to callBack
			//and update the slider vals when params are changed in the host (eg automation)
			startTimerHz(TIMER_FREQ);

			//Updates the slider position
			updateSliderPos();
		}

		//Calls the setValueNotifyingHost method when value is changed
		//Allows the host/editor to modify the slider values
		void valueChanged() override
		{
			param.setValueNotifyingHost((float)Slider::getValue());
		}

		//This method is called at regular intervals of TIMER_FREQ
		void timerCallback() override { 
			//update the slider position at regular intervals to ensure the UI matches 
			//the param values
			updateSliderPos(); 			
		}

		//Calls the beginChangeGesture method to allow the host to know when a parameter is being held by user
		void startedDragging() override { param.beginChangeGesture();	}
		//Calls the endChangeGesture method to allow the host to know when a parameter has been let go by the user
		void stoppedDragging() override { param.endChangeGesture(); }
		
		//gets the value from text and returns it as a double, used for typing param values
		double getValueFromText(const String& text) override { return param.getValueForText(text); }

		//returns the sliders current vallue as a string
		String getTextFromValue(double value) override { return param.getText((float)value, 1024); }

		//updates the slider position
		void updateSliderPos()
		{
			//sets newValue as the current param value
			const float newValue = param.getValue();

			//If the param value does not match the slider value then update
			if (newValue != (float)Slider::getValue() && !isMouseButtonDown())
			{
				Slider::setValue(newValue);
			}
		}
		
	
		AudioProcessorParameter& param; //declare a param object for referencing inside the class
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterSlider)
};

//==============================================================================
FuzzFaceJuceAudioProcessorEditor::FuzzFaceJuceAudioProcessorEditor(FuzzFaceJuceAudioProcessor& p)
	: AudioProcessorEditor(&p), processor(p),
	volLabel(String::empty, "Vol: "),
	fuzzLabel(String::empty, "Fuzz: "),
	gainLabel(String::empty, "Input Gain: ")
{
	//Add the sliders to the editor
	//Adds the Vol Slider
	addAndMakeVisible(volSlider = new ParameterSlider(*p.volParam));
	//Set the slider as a rotary
	volSlider->setSliderStyle(Slider::Rotary);
	//Sets the textbox below, editable, width and height
	volSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 40, 14);

	//Adds the Fuzz Slider
	addAndMakeVisible(fuzzSlider = new ParameterSlider(*p.fuzzParam));
	//Set the slider as a rotary
	fuzzSlider->setSliderStyle(Slider::Rotary);
	//Sets the textbox below, editable, width and height
	fuzzSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 40, 14);
	
	//Adds the input gain slider
	addAndMakeVisible(gainSlider = new ParameterSlider(*p.gainParam));
	//Set the slider as a rotary
	gainSlider->setSliderStyle(Slider::Rotary);
	//Sets the textbox Below, editable, width and height
	gainSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 40, 14);

	
	//Add labels to the sliders
	volLabel.attachToComponent(volSlider, false);
	volLabel.setFont(Font(16.0));

	fuzzLabel.attachToComponent(fuzzSlider, false);
	fuzzLabel.setFont(Font(16.0));

	gainLabel.attachToComponent(gainSlider, false);
	gainLabel.setFont(Font(16.0));

	//The window should not be resizable
	setResizable(false, false);

	//sets the size to width, height
	setSize(WIN_WIDTH, WIN_HEIGHT);

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
    //Layout and size of the sliders 
	volSlider->setBounds(260, 50, 120, 120);
	fuzzSlider->setBounds(420, 50, 120, 120);
	gainSlider->setBounds(60, 50, 120, 120);

}

//===========================================================
