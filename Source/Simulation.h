#pragma once
#include "MyMatrixTypes.h"
#include "Circuit.h"
#include <math.h>
#include <vector>
#include <ctime>


class Simulation : public Circuit
{
public:
	//Default Constructor
	Simulation();

	//Constructor with samplerate and vcc as arguement
	Simulation(double _sampleRate, double _vcc);

	//Default Destructor
	~Simulation();

	//process the buffer of data where vcc is set to the system VCC (default 9v)
	//Eigen::VectorXd processBuffer(Eigen::VectorXd inputBuffer);

	//Buffer to store the output vector of the simulation to send to the audio channel
	//Eigen::VectorXd outputBuffer;

	//Set the sampleRate
	void setSimSampleRate(double _sampleRate);

	//Takes the current sample as an arguement along with vcc and processes it, returning the new data.
	//channelData is used as the name to be implemented with JUCE API channelData variable name
	void processSample(float* channelData, double _vcc);


	//Method to set the fuzz and vol params to the arguement vals and then refresh the system.
	void setParams(double _fuzzVal, double _volVal);

	//Gets the system to a steady state ready for processing
	void getSteadyState();

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW //Aligns all the Eigen Members in the class to avoid memory allocation errors... see https://eigen.tuxfamily.org/dox/group__TopicUnalignedArrayAssert.html & https://eigen.tuxfamily.org/dox/group__TopicStructHavingEigenMembers.html

private:
	//Buffer size in samples, defaulted to...??
	int bufferSize;

	/* Input */
	//VCC voltage
	double vcc = DEFAULT_VCC; //steady state voltage
	const double durfade = DURFADE; //duration of the faded power up
	int hanWin; //length in samples of the hanning window for voltage ramping, used in the get steady state phase
	const double steadyStatePeriodFactor = STEADY_STATE_FACTOR; //Factor which controls the size of the window window used to reach steady state (where window size in samples = hanWin*steadyStateFactor)

	Eigen::VectorXd win; //hanning window
	Eigen::VectorXd vccv; //power up voltage used in initial setup

	const double maxIterations = MAX_ITERATIONS;
	const double maxSubIterations = MAX_SUB_ITERATIONS;

	//Specified tolerance in nonlinear voltage vd
	const double tol = TOL;
	//tol^2, used for end conditions of the NR solver
	const double tols = tol*tol;


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

	//Eigen::Vector2d outputVector;
	double output; //output of the model (MATLAB - y)


};
