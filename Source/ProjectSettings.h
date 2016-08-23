#pragma once
//Contains all the settings used throughout the project
namespace constants {

	//Controls the min and max param values
	const double CTRL_INCREMENT = 0.001; //The increment of the control knobs
	const double CTRL_MIN = 0;
	const double CTRL_MAX = 1;
	const double SMOOTH_COEFF = 0.9995;

	//Constrols the min and max input gain vals
	const double GAIN_INCREMENT = 0.01;
	const double GAIN_MIN = -12; //in db
	const double GAIN_MAX = 6; //in db


	//timer freq for the UI updater in hz
	const int UI_TIMER_FREQ = 30;
	//timer freq for the param updater in hz
	const int P_TIMER_FREQ = 1000;

	//Defaults for the parameters
	const double GAIN_DEFAULT = 0;
	const double FUZZ_DEFAULT = 0.6;
	const double VOL_DEFAULT = 0.4;

	//Window size of plugin
	const int WIN_WIDTH = 600;
	const int WIN_HEIGHT = 200;

	const int KNOB_WIDTH = 90;
	const int KNOB_HEIGHT = 90;


	/* PI */
	const double PI = 3.14159265358979323846; //- extra digits of precision

	//Defaults for the Simulation cpp
	const double DEFAULT_VCC = 8.1;  // default value for the voltage power supply, 90% of 9v = 8.1v for more realism
	const float ZERO_INPUT = 0;  // zero input used for getting the system to steady state
	const double DURFADE = 0.1;   //duration of the faded power supply used for steady state
	const double STEADY_STATE_FACTOR = 4.5;  //Factor which controls the size of the window window used to reach steady state (where window size in samples = MM*steadyStateFactor)
	const int MAX_ITERATIONS = 100;  //Maximum number of iterations for the newton raphson solver
	const int MAX_SUB_ITERATIONS = 10;  //maximum number of subiterations for the damped newton raphson solver
	const double TOL = 1e-10;  //error tolerance of the system

							   //Define the default sample rates as 44.1kHz
	const int DEFAULT_SR = 44100;
}