#pragma once
//Contains all the settings used throughout the project
namespace constants {
	//Plugin name
	const std::string PLUGIN_NAME = "Fuzzed";

	/* PI */
	const double PI = 3.14159265358979323846; //- extra digits of precision
	//Define the default sample rates as 44.1kHz
	const int DEFAULT_SR = 44100;

	//Declare enum types for the different knob styles
	enum KnobType { PARAM_KNOB, GAIN_KNOB, METER };

	//Controls the min and max param values
	const double CTRL_INCREMENT = 0.01; //The increment of the control knobs
	const double CTRL_MIN = 0; //The minimum ctrl parameter value
	const double CTRL_MAX = 1; //The maximum ctrl parameter
	const double SLIDER_SENS = 400; //Controls the sensitivty of the sliders, Higher number = lower sensitivity

	//Constrols the min and max input gain vals
	const double GAIN_INCREMENT = 0.01;
	const double GAIN_MIN = -6; //in db
	const double GAIN_MAX = 6; //in db

	//Input Signal/ Clipping control
	const double INPUT_SCALAR = 3; //scales the input signal to the processSample method by a factor of MAX_INPUT_SIG * (2^-INPUT_SCALAR)
	const double MAX_INPUT_SIG = 0.1; //The maximum value that can be put into the system and maintain stability, 
	const double CLIPPING_POINT = 0.1; //if the input signal excedes this then clip to ensure system cannot crash
	const double OUTPUT_SCALAR = 2; //Scale the output of the system back up to useable levels

	const double CLIPPING_FILTER_CUTOFF = 140; //The cutoff in hz of the clipping filter for filtering the clipped signal
	
	//timer freq for the UI updater in hz
	const int UI_TIMER_FREQ = 300;
	//timer freq for the param updater in hz
	const int P_TIMER_FREQ = 1000;
	//the time in s between parameter updates
	const double SMOOTHING_TIME_S = 0.01;
	//controls the number of samples between parameter updates
	const int UPDATE_PARAM_SAMPLE_INTERVAL = 16;

	//controls the update speed of the VU meter, smaller is faster
	const double METER_UPDATE_RATE = 0.15;
	const double METER_MAX = 12.0;  //db max value for meter
	const double METER_MIN = -48.0; //dB min value for meter

	//Defaults for the parameters
	const double GAIN_DEFAULT = 0;
	const double FUZZ_DEFAULT = 0.01;
	const double VOL_DEFAULT = 0.4;

	//Window size of plugin
	const int WIN_WIDTH = 600;
	const int WIN_HEIGHT = 270;

	const int KNOB_WIDTH = 120;
	const int KNOB_HEIGHT = 120;

	const int TEXT_BOX_WIDTH = 65;
	const int TEXT_BOX_HEIGHT = 16;

	const float FONT_SIZE = 16.0f;

	//Defaults for the Simulation cpp
	const double DEFAULT_VCC = 8.1;  // default value for the voltage power supply, 90% of 9v = 8.1v for more realism
	const float ZERO_INPUT = 0.;  // zero input used for getting the system to steady state
	const double DURFADE = 0.1;   //duration of the faded power supply used for steady state
	const double STEADY_STATE_FACTOR = 4.5;  //Factor which controls the size of the window window used to reach steady state (where window size in samples = hanWin*steadyStateFactor)
	const int MAX_ITERATIONS = 90;  //Maximum number of iterations for the newton raphson solver
	const int MAX_SUB_ITERATIONS = 10;  //maximum number of subiterations for the damped newton raphson solver
	const double TOL = 1e-10;  //error tolerance of the system

}