#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "ProjectSettings.h"
using namespace constants;

//Custom look and feel class that provides a new implementation for the drawRotarySlider method from LookAndFeel_V3, allowing for the use of "film strip png" for the knob states
class MyLookAndFeel : public LookAndFeel_V3
{
public:
	MyLookAndFeel();
	MyLookAndFeel(KnobType knobType);
	virtual ~MyLookAndFeel();

	virtual void drawRotarySlider(
		Graphics & 	g,
		int 	x,
		int 	y,
		int 	width,
		int 	height,
		float 	sliderPosProportional,
		float 	rotaryStartAngle,
		float 	rotaryEndAngle,
		Slider & 	slider) override;

	//Declare the image used for this knob
	Image knobStrip;

	
};

