/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//Define the ParameterSlider class and methods
//The param is the value the host has, the slider::getValue is the value the plugin has
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
	gainLabel(String::empty, "Input Gain"),
	volLabel(String::empty, "Vol"),
	fuzzLabel(String::empty, "Fuzz")
	
{
	//Add the sliders to the editor
	//Adds the input gain slider
	addAndMakeVisible(gainSlider = new ParameterSlider(*p.gainParam));
	//Set the slider as a rotary
	gainSlider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
	//Sets the textbox Below, editable, width and height
	gainSlider->setTextBoxStyle(Slider::NoTextBox, true, TEXT_BOX_WIDTH, TEXT_BOX_HEIGHT);	
	//Sets the sensitivity of the slider
	gainSlider->setMouseDragSensitivity(SLIDER_SENS);
	//Allows for the use of double click to return slider to default
	gainSlider->setDoubleClickReturnValue(true, 0.5);

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
	//Allows for the use of double click to return slider to default
	volSlider->setDoubleClickReturnValue(true, VOL_DEFAULT);
	

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
	//Allows for the use of double click to return slider to default
	fuzzSlider->setDoubleClickReturnValue(true, FUZZ_DEFAULT);


	//Attach the labels to the components
	gainLabel.attachToComponent(gainSlider, false);
	gainLabel.setFont(Font(FONT_SIZE));
	gainLabel.setColour(Label::textColourId, Colour(Colours::white));
	gainLabel.setJustificationType(Justification::centred);

	volLabel.attachToComponent(volSlider, false);
	volLabel.setFont(Font(FONT_SIZE));
	volLabel.setColour(Label::textColourId, Colour(Colours::white));
	volLabel.setJustificationType(Justification::centred);


	fuzzLabel.attachToComponent(fuzzSlider, false);
	fuzzLabel.setFont(Font(FONT_SIZE));
	fuzzLabel.setColour(Label::textColourId, Colour(Colours::white));
	fuzzLabel.setJustificationType(Justification::centred);

	//Set the sliders to the custom UI class
	gainSlider->setLookAndFeel(gainLookAndFeel);
	volSlider->setLookAndFeel(paramLookAndFeel);
	fuzzSlider->setLookAndFeel(paramLookAndFeel);


	//Meter slider
	addAndMakeVisible(meterSlider);  //adds to the view
	meterSlider->setSliderStyle(Slider::Rotary);  //sliderstyle rotary
	meterSlider->setRange(METER_MIN, METER_MAX, METER_UPDATE_RATE); //slider range
	meterSlider->setEnabled(false);  //disables the slider so it cannot be clicked
	meterSlider->setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
	meterSlider->setPopupDisplayEnabled(false, this);
	meterSlider->setSkewFactor(METER_MAX/(abs(METER_MIN)+METER_MAX));
	meterSlider->setLookAndFeel(meterLookAndFeel); //set up the look and feel of the slider
	meterSlider->setValue(METER_MIN);

	//The window should not be resizable
	setResizable(false, false);

	//sets the size to width, height
	setSize(WIN_WIDTH, WIN_HEIGHT);
	startTimerHz(P_TIMER_FREQ);	
}

FuzzFaceJuceAudioProcessorEditor::~FuzzFaceJuceAudioProcessorEditor()
{
}

//Timer callback used for updating the VU meter, checks if current input is a peak and sets the meter to this peak.
void FuzzFaceJuceAudioProcessorEditor::timerCallback()
{
	double currentReading = meterSlider->getValue();
	double processorSignal = Decibels::gainToDecibels(processor.currentInput, METER_MIN);
	
	
	//inputSignalLabel.setText("Input Signal: " + std::to_string(processorSignal) + " and the Current Reading: " + std::to_string(currentReading), dontSendNotification);
	

    if (processorSignal > currentReading) {
		//increment the meter value until it reaches the processor signal
		meterSlider->setValue(currentReading + METER_UPDATE_RATE);
	}
	else if (processorSignal < currentReading) {
		//increment the meter value until it reaches the processor signal
		meterSlider->setValue(currentReading - METER_UPDATE_RATE);
	}
	
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
    g.setFont (FONT_SIZE);
    g.drawFittedText (PLUGIN_NAME, getLocalBounds(), Justification::centredTop, 1);
}

void FuzzFaceJuceAudioProcessorEditor::resized()
{
    //Layout and size of the sliders 
	gainSlider->setBounds(WIN_WIDTH/12, (WIN_HEIGHT/3) - FONT_SIZE, KNOB_WIDTH, KNOB_HEIGHT);
	volSlider->setBounds((5* WIN_WIDTH)/12, (WIN_HEIGHT /3) - FONT_SIZE, KNOB_WIDTH, KNOB_HEIGHT);
	fuzzSlider->setBounds((9* WIN_WIDTH)/12, (WIN_HEIGHT /3) - FONT_SIZE, KNOB_WIDTH, KNOB_HEIGHT);
	meterSlider->setBounds(195, 119, 35, 35);
}

//===========================================================

