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
			startTimerHz(UI_TIMER_FREQ);

			//Updates the slider position
			updateSliderPos();
			
		}
		
		//Calls the setValueNotifyingHost method when value is changed
		//Allows the plugin to send data to the host, changing the param value in the host window to that of the plugin window
		void valueChanged() override
		{	
			//If the mouse is being held then the user is trying to set the value from the UI and this should be used
			if (isMouseButtonDown()) {
				param.setValueNotifyingHost((float)Slider::getValue());
			}
			//Else let the host decide the parameter (ie allow automation of the parameter from the host)
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
			if (newValue != (float)Slider::getValue())
			{
				//Sets the slider value to this value
				Slider::setValue(newValue);

				//If the user is actively holding the slider in a position then set the parameter to this held value
				if (isMouseButtonDown()) {
					//Notifies the host of this new value being held to cancel any automation that the host may be doing
					param.setValueNotifyingHost((float)Slider::getValue());
				}
			}

		}
		

		AudioProcessorParameter& param; //declare a param object for referencing inside the class
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterSlider)
};

//==============================================================================
FuzzFaceJuceAudioProcessorEditor::FuzzFaceJuceAudioProcessorEditor(FuzzFaceJuceAudioProcessor& p)
	: AudioProcessorEditor(&p), processor(p),
	gainLabel(String::empty, "Input Gain: "),
	volLabel(String::empty, "Vol: "),
	fuzzLabel(String::empty, "Fuzz: "),
	inputSignalLabel(String::empty, "Input Signal: ")

		
{
	//Add the sliders to the editor
	//Adds the input gain slider
	addAndMakeVisible(gainSlider = new ParameterSlider(*p.gainParam));
	//Set the slider as a rotary
	gainSlider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
	//Sets the textbox Below, editable, width and height
	gainSlider->setTextBoxStyle(Slider::TextBoxBelow, true, TEXT_BOX_WIDTH, TEXT_BOX_HEIGHT);	
	//Sets the sensitivity of the slider
	gainSlider->setMouseDragSensitivity(SLIDER_SENS);
	//Sets the suffix to appear to allow the user to know what knob is being controlled
	gainSlider->setTextValueSuffix(" dB");


	//Adds the Vol Slider
	addAndMakeVisible(volSlider = new ParameterSlider(*p.volParam));
	//Set the slider as a rotary
	volSlider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
	//Sets the textbox below, editable, width and height
	volSlider->setTextBoxStyle(Slider::NoTextBox, true, TEXT_BOX_WIDTH, TEXT_BOX_HEIGHT);
	//Sets the sensitivity of the slider
	volSlider->setMouseDragSensitivity(SLIDER_SENS);
	//Set the popup box to display when in use
	volSlider->setPopupDisplayEnabled(true, this);
	//Sets the suffix to appear to allow the user to know what knob is being controlled
	volSlider->setTextValueSuffix(" Vol");

	//Adds the Fuzz Slider
	addAndMakeVisible(fuzzSlider = new ParameterSlider(*p.fuzzParam));
	//Set the slider as a rotary
	fuzzSlider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
	//Sets the textbox below, editable, width and height
	fuzzSlider->setTextBoxStyle(Slider::NoTextBox, true, TEXT_BOX_WIDTH, TEXT_BOX_HEIGHT);
	//Sets the sensitivity of the slider
	fuzzSlider->setMouseDragSensitivity(SLIDER_SENS);
	//Set the popup box to display when in use
	fuzzSlider->setPopupDisplayEnabled(true, this);
	//Sets the suffix to appear to allow the user to know what knob is being controlled
	fuzzSlider->setTextValueSuffix(" Fuzz");


	//Attach the labels to the components
	gainLabel.attachToComponent(gainSlider, false);
	gainLabel.setFont(Font(16.0));
	gainLabel.setColour(Label::textColourId, Colour(Colours::white));


	volLabel.attachToComponent(volSlider, false);
	volLabel.setFont(Font(16.0));
	volLabel.setColour(Label::textColourId, Colour(Colours::white));

	fuzzLabel.attachToComponent(fuzzSlider, false);
	fuzzLabel.setFont(Font(16.0));
	fuzzLabel.setColour(Label::textColourId, Colour(Colours::white));

	//Add the input signal tracer into the UI
	addAndMakeVisible(inputSignalLabel);
	inputSignalLabel.setFont(Font(16.0));
	

	//Set the sliders to the custom UI class
	gainSlider->setLookAndFeel(myLookAndFeel);
	volSlider->setLookAndFeel(myLookAndFeel);
	fuzzSlider->setLookAndFeel(myLookAndFeel);




	//The window should not be resizable
	setResizable(false, false);

	//sets the size to width, height
	setSize(WIN_WIDTH, WIN_HEIGHT);
	startTimerHz(P_TIMER_FREQ);
}

FuzzFaceJuceAudioProcessorEditor::~FuzzFaceJuceAudioProcessorEditor()
{
}

void FuzzFaceJuceAudioProcessorEditor::timerCallback()
{
	inputSignalLabel.setText("Input Signal: " + std::to_string(processor.currentInput), dontSendNotification);
}

//==============================================================================
void FuzzFaceJuceAudioProcessorEditor::paint (Graphics& g)
{
	//Set the colour for the backgroun
	Colour colour(149,00,00);
	//Fill the backgroun
    g.fillAll (colour);


	//Set the text colour
    g.setColour (Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("FuzzFace", getLocalBounds(), Justification::centredTop, 1);
}

void FuzzFaceJuceAudioProcessorEditor::resized()
{
    //Layout and size of the sliders 
	gainSlider->setBounds(60, 50, KNOB_WIDTH, KNOB_HEIGHT);
	volSlider->setBounds(260, 50, KNOB_WIDTH, KNOB_HEIGHT);
	fuzzSlider->setBounds(420, 50, KNOB_WIDTH, KNOB_HEIGHT);	
	inputSignalLabel.setBounds(50, 160, 200, 20);
}

//===========================================================

