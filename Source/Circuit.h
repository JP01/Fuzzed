#pragma once
// Header file for the Circuit.cpp class which contains
// all the circuit parameters
#include "MyMatrixTypes.h"
#include <iostream>

//Define the default sample rates as 44.1kHz
#define DEFAULT_SR 44100.

class Circuit
{
public: //access control
	const double defaultFuzz = 0.6; //Default value for fuzz
	const double defaultVol = 0.4;  //Default value for volume

	//Default Constructor
	Circuit();

	//Constructor with samplerate as an argument
	Circuit(double _sampleRate);
	//Default Destructor
	~Circuit();

	//The samplerate of the system
	double sampleRate;

	/*Accessors and Mutators*/
	//Fuzz
	void setFuzz(double _fuzz);   //function used to set fuzz paramater
	double getFuzz();                //returns the fuzz parameter

	//Vol
	void setVol(double _vol);     //function used to set volume parameter
	double getVol();                 //returns the volume parameter

	double getSaturationCurrent(); //returns the circuit's saturation current IS
	double getThermalVoltage(); //returns the circuit's thermal voltage VT

	double Circuit::getCircuitSampleRate(); //returns the samplerate

	//StateSpaceMatrices, returns a dynamically sized matrix due to unknown size at compile time
	//Return the specified state space matrix, takes capital letters only A-K
	Eigen::MatrixXd getStateSpaceMatrix(std::string input);  //Returns the statespace matrix corresponding to the input string, used in testing

	//Nonlinear Function Matrices, returns a dynamically sized matrix due to unknown size at compile time
	//Returns the nonlinear function matrix corresponding to the input string, used in testing
	//Acceptable arguements "psi", "phi", "nonLinEquationMatrix", "alteredStateSpaceK"
	Eigen::MatrixXd getNonlinearFunctionMatrix(std::string input);

	//Refreshes the system matrix with new fuzz and vol values then returns the system matrix
	SystemMatrix getSystemMatrix() { refreshSystemMatrix();  return systemMatrix; }

protected:
	//Refresh All matrices, call when paramater change needs to be implemented
	void refreshFullCircuit();

	//sets the samplerate and updates the matrices
	void setCircuitSampleRate(double _sampleRate);

private: //access control
	double fuzz;  //value for the fuzz parameter
	double vol;   //value for the vol parameter

	 //Sample period
	double T;

	//Resistors Values
	const double r1 = 33e3;
	const double r2 = 8.2e3;
	const double r3 = 330;
	const double r6 = 100e3;
	double r4, r5, r7, r8;        //variable Resistors

	//Capacitors Values
	const double c1 = 2.2e-6;
	const double c2 = 20e-6;
	const double c3 = 10e-9;

	//PNP Bipolar Junction Transistor Values
	const double forwardGain = 200;
	const double reverseGain = 2;
	const double thermalVoltage = 25.8e-3;
	const double saturationCurrent = 1e-14;

	//Initial setup functions
	void setupCircuit();

	/*Circuit matrices*/
	void populateCircuitMatrices();	//populate circuit matrices, performed at setup
	void refreshCircuitMatrices();	//update the circuit matrices

	//Resistor Matrices
	ResMatrix resMatrix;  //1row 8col - resistor matrix
	DiagRes diagResMatrix;          //diagonal - resistor matrix

	//Capacitor Matrices
	CapMatrix capMatrix;           //1row 3col - capacitor matrix
	DiagCap diagCapMatrix;          //diagonal - capacitor matrix


	/*Incident Matrices*/
	void initialiseIncidentMatrices(); //One time setup of incident matrices, this is performed in the constructor and sets up the incident matrices

	IncidentRes incidentResistors;    //incident resistor matrix
	IncidentCap incidentCapacitors;   //incident capacitor matrix
	IncidentVoltage incidentVoltage;   //incident voltage matrix
	IncidentNonLin incidentNonlinearities;  //incident nonlinearity matrix
	IncidentOutput incidentOutput;  //incident output matrix

	/**
	* State Space Matrices
	*/
	//Setup functions for the system matrix
	IntermediateSystemMatrix systemRes;  //Resistor matrix used in calculation of system matrix
	IntermediateSystemMatrix systemCap;  //Capacitor matrix used in calculation of system matrix

	SystemMatrix systemMatrix;  //Create a 12x12 system matrix

	void refreshSystemMatrix();  //function to setup and refrseh the system matrix

    //Setup functions for the state space terms
	PaddedCap padC;  //padded capacitor matrix 
	PaddedNonLin padNL; //padded nonlinearity matrix 
	PaddedOutput padO;  //padded output matrix
	PaddedInput padI;   //padded input matrix

	StateSpaceA stateSpaceA;
	StateSpaceB stateSpaceB;
	StateSpaceC stateSpaceC;
	StateSpaceD stateSpaceD;
	StateSpaceE stateSpaceE;
	StateSpaceF stateSpaceF;
	StateSpaceG stateSpaceG;
	StateSpaceH stateSpaceH;
	StateSpaceK stateSpaceK;

	//Refresh the nonlinear state space equations with updated fuzz and vol settings
	void refreshNonLinStateSpace();

	/*
	*   Nonlinear function matrices, set as 4x4 matrices
	*/
	NonlinearFunctionMatrix psi; //Ebers-moller term
	NonlinearFunctionMatrix phi; //Ebers-moller term
	NonlinearFunctionMatrix nonLinEquationMatrix;   //Nonlinear equation matrix (MATLAB - M)
	NonlinearFunctionMatrix alteredStateSpaceK;  //Kd is an altered form of the K statespace matrix (MATLAB - kd)


	void refreshNonlinearFunctions();
};