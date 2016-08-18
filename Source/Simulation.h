#pragma once
#include "MyMatrixTypes.h"
#include "Circuit.h"
#include <math.h>
#include <vector>
#include <ctime>

/* PI */
#define PI 3.14159265358979323846 //- extra digits of precision

#define DEFAULT_VCC 9.  // default value for the voltage power supply
#define ZERO_INPUT 0.  // zero input used for getting the system to steady state
#define DURFADE 0.1    //duration of the faded power supply used for steady state
#define STEADY_STATE_FACTOR 2.  //Factor which controls the size of the window window used to reach steady state (where window size in samples = MM*steadyStateFactor)
#define MAX_ITERATIONS 100  //Maximum number of iterations for the newton raphson solver
#define MAX_SUB_ITERATIONS 10  //maximum number of subiterations for the damped newton raphson solver
#define TOL 1e-10  //error tolerance of the system


class Simulation : public Circuit
{
public:
	//Default Constructor
	Simulation();

	//Constructor with samplerate and vcc as arguement
	Simulation(double _sampleRate, double _vcc);

	//Default Destructor
	~Simulation();

	//Refresh all the circuit values and setup the statespace matrices used in the simulation
	void refreshAll();

	//process the buffer of data where vcc is set to the system VCC (default 9v)
	Eigen::VectorXd processBuffer(Eigen::VectorXd inputBuffer);

	//Buffer to store the output vector of the simulation to send to the audio channel
	Eigen::VectorXd outputBuffer;

	//Set the sampleRate
	void setSimSampleRate(double _sampleRate);
	
	//IPlug parameter smoothing
	class CParamSmooth
	{
	public:
		
		CParamSmooth() { a = 0.99; b = 1. - a; z = 0.; };
		~CParamSmooth() {};
		double Process(double in) { z = (in * b) + (z * a); return z; }
	private: 
		double a, b, z;
	};



private:

	//Buffer size in samples, defaulted to...??
	int bufferSize;

	//Gets the system to a steady state ready for processing
	void getSteadyState();

	//zero input used as signal for warmup phase / getSteadyState
	const double zeroInput = ZERO_INPUT;

	/* Input */
	//VCC voltage
	double vcc = DEFAULT_VCC; //steady state voltage
	const double durfade = DURFADE; //duration of the faded power up
	double MM; //integer rounded value used during the powerup phase
	const double steadyStatePeriodFactor = STEADY_STATE_FACTOR; //Factor which controls the size of the window window used to reach steady state (where window size in samples = MM*steadyStateFactor)

	Eigen::VectorXd win; //hanning window
	Eigen::VectorXd vccv; //power up voltage used in initial setup
	Eigen::VectorXd powerUpTimeVector; //time vector used in the powerup phase

	const double maxIterations = MAX_ITERATIONS;
	const double maxSubIterations = MAX_SUB_ITERATIONS;

	//Specified tolerance in nonlinear voltage vd
	const double tol = TOL;
	//tol^2, used for end conditions of the NR solver
	const double tols = tol*tol;


	StateSpaceA simStateSpaceA;
	StateSpaceB simStateSpaceB;
	StateSpaceC simStateSpaceC;
	StateSpaceD simStateSpaceD;
	StateSpaceE simStateSpaceE;
	StateSpaceF simStateSpaceF;
	StateSpaceG simStateSpaceG;
	StateSpaceH simStateSpaceH;
	NonlinearFunctionMatrix simPSI, simAlteredStateSpaceK, simNonLinEquationMatrix;

	double simSaturationCurrent, simThermalVoltage;


	/*Simulation Preparations*/

	double nrms;	//newton rhapson solver value

	int iteration; 	//number of iterations per sample
	int subIterationTotal; //total number of subiterations occuring in a sample
	int subIterCounter;  //used in the damped newton iterations to determine sub iterations per iteration

	Eigen::Vector3d stateSpaceVector; //(MATLAB - x)
	Eigen::Vector3d stateSpaceVectorMem; //memorised state vector, 3x1 vector (MATLAB - xz)

	Eigen::Vector4d nonLinVoltageVector; //discrete nonlinear voltage vector. 4x1 vector (MATLAB - vd)
	Eigen::Vector4d nonLinVoltageVectorMem; //memorised nonlinear voltage vector, 4x1 vector (MATLAB - vdz)
	Eigen::Vector4d nonLinVoltageVectorPrev; //previous value --- MATLAB = vd0
	Eigen::Vector4d nonLinVoltageVectorNew; //new vd vector -- MATLAB = vdnew

	Eigen::Vector2d inputVector; //input vector, 2x1 vector (MATLAB - u)
	Eigen::Vector4d nonLinSolverInput; //input vector used in calculation of the nonlinear equation (MATLAB - pd)

	Eigen::Vector4d nonLinTransistorFunction; //MATLAB - f
	Eigen::Matrix4d nonLinTransistorFunctionAltered; //Matlab - fd
	Eigen::Vector4d nodalDKNonlinearG; //the nonlinear function from the Nodal DK method (i.e. g = p + K*f - v) (MATLAB - g)
	Eigen::Vector4d nodalDKNonlinearGNew; //Matlab - gnew
	Eigen::Vector4d nodalDKNonlinearGtemp; //Matlab (used as temp value for gnew = M*vdnew + IS*(exp(vdnew/VT) - 1) - pd;)
	Eigen::Matrix4d nodalDKNonlinearGAltered; //MATLAB - gd
	Eigen::Vector4d nonLinearCurrent; //the nonlinear currents through the transistor (i.e. i = f(v)) .... f and i basically have the same purpose so there is redundancy. (MATLAB - i)

	Eigen::Vector4d newtonStep; //the newston Step, used in simulation, MATLAB - STEP
	Eigen::Vector4d newtonStepTemp; //temp term used in simulation, adjusted step which works to keep max subiterations within the limit MATLAB - STP

	Eigen::Vector4d calcTemp; //temporary term used in simulation calculation (MATLAB - TERM)

	//initialise and zero all the simulation paramaters and vectors
	void initialiseSimulationParameters();

	//Takes the current sample as an arguement along with vcc and processes it, returning the new data.
	//channelData is used as the name to be implemented with JUCE API channelData variable name
	double processSample(double channelData, double _vcc);

	//Eigen::Vector2d outputVector;
	double output; //output of the model (MATLAB - y)


};
