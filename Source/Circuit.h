#pragma once
// Header file for the Circuit.cpp class which contains
// all the circuit parameters
#include "MyMatrixTypes.h"
#include <iostream>
#include "ProjectSettings.h"
//Use the namespace in projectsettings.h
using namespace constants;

class Circuit
{
public: //access control
	const double minCTRL = CTRL_MIN; //Sets the minimum allowable value for the parameters
	const double maxCTRL = CTRL_MAX; //sets the maximum allowable value for the parameters

	//Default Constructor
	Circuit();

	//Constructor with samplerate as an argument
	Circuit(double _sampleRate);
	//Default Destructor
	~Circuit();

	//The samplerate of the system
	double sampleRate;

	double getSaturationCurrent(); //returns the circuit's saturation current IS
	double getThermalVoltage(); //returns the circuit's thermal voltage VT

	double Circuit::getCircuitSampleRate(); //returns the samplerate

	//Method to set the fuzz and vol params to the arguement vals.
	void setParams(double _fuzzVal, double _volVal);
	double getFuzz();                //returns the fuzz parameter
	double getVol();                 //returns the volume parameter

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW //Aligns all the Eigen Members in the class to avoid memory allocation errors... see https://eigen.tuxfamily.org/dox/group__TopicUnalignedArrayAssert.html & https://eigen.tuxfamily.org/dox/group__TopicStructHavingEigenMembers.html

protected:
	/*Accessors and Mutators*/
	//Fuzz
	void setFuzz(double _fuzz);   //function used to set fuzz paramater

	void setVol(double _vol);     //function used to set volume parameter

	void refreshFullCircuit();	//Refresh All matrices, call when paramater change needs to be implemented

								//sets the samplerate and updates the matrices
	void setCircuitSampleRate(double _sampleRate);

	//Declare the statespace terms and their constant elements
	StateSpaceA stateSpaceA, stateSpaceA0;
	StateSpaceB stateSpaceB, stateSpaceB0;
	StateSpaceC stateSpaceC, stateSpaceC0;
	StateSpaceD stateSpaceD, stateSpaceD0;
	StateSpaceE stateSpaceE, stateSpaceE0;
	StateSpaceF stateSpaceF, stateSpaceF0;
	StateSpaceG stateSpaceG, stateSpaceG0;
	StateSpaceH stateSpaceH, stateSpaceH0;
	StateSpaceK stateSpaceK, stateSpaceK0;

	//Declare the constant state space terms used in Holters method
	StateSpaceQ stateSpaceQ;
	StateSpaceUx stateSpaceUx;
	StateSpaceUo stateSpaceUo;
	StateSpaceUn stateSpaceUn;
	StateSpaceUu stateSpaceUu;

	//Sample period
	double samplePeriod;

	//PNP Bipolar Junction Transistor Values
	const double forwardGain = 200;
	const double reverseGain = 2;
	const double thermalVoltage = 25.8e-3;
	const double saturationCurrent = 1e-14;

	/*
	*   Nonlinear function matrices, set as 4x4 matrices
	*/
	NonlinearFunctionMatrix psi; //Ebers-moller term
	NonlinearFunctionMatrix phi; //Ebers-moller term
	NonlinearFunctionMatrix nonLinEquationMatrix;   //Nonlinear equation matrix (MATLAB - M)
	NonlinearFunctionMatrix alteredStateSpaceK;  //Kd is an altered form of the K statespace matrix (MATLAB - kd)


private: //access control
	double fuzz;  //value for the fuzz parameter
	double vol;   //value for the vol parameter


	 //potentiometer resistances
	const double volPotRes = 500e3;
	const double fuzzPotRes = 1e3;
	double volPotVar1, volPotVar2, fuzzPotVar1, fuzzPotVar2; //Variables for the potentiometer

	//Resistors Values
	const double r1 = 33e3;
	const double r2 = 8.2e3;
	const double r3 = 330;
	const double r4 = 2 * volPotRes;
	const double r5 = 2 * volPotRes;
	const double r6 = 100e3;
	const double r7 = 2 * fuzzPotRes;
	const double r8 = 2 * fuzzPotRes;

	//Capacitors Values
	const double c1 = 2.2e-6;
	const double c2 = 20e-6;
	const double c3 = 10e-9;

	//Initial setup functions
	void setupCircuit();

	/*Circuit matrices*/
	void populateComponentMatrices();	//populate circuit component matrices, performed at setup
	void refreshPotentiometerMatrices();	//update the circuit matrices

	//Potentiometer Matrices
	PotMatrix potMatrix;  //1row 4 col - potentiometer matrix
	DiagPot diagPotMatrix; //diagonal - pot matrix

	//Resistor Matrices
	ResMatrix resMatrix;            //1row 8col - resistor matrix
	DiagRes diagResMatrix;          //diagonal - resistor matrix

	//Capacitor Matrices
	CapMatrix capMatrix;           //1row 3col - capacitor matrix
	DiagCap diagCapMatrix;          //diagonal - capacitor matrix


	/*Incident Matrices*/
	void initialiseIncidentMatrices(); //One time setup of incident matrices, this is performed in the constructor and sets up the incident matrices

	IncidentPot incidentPot; //incident potentiometer matrix
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

	void populateConstantSystemMatrix();  //function to setup and refrseh the system matrix

	//Setup functions for the state space terms
	PaddedCap padC;  //padded capacitor matrix 
	PaddedNonLin padNL; //padded nonlinearity matrix 
	PaddedOutput padO;  //padded output matrix
	PaddedInput padI;   //padded input matrix
	PaddedPot padPot; //padded potentiometer matrix

	//Populate the constant state space terms
	void populateConstantStateSpaceTerms();

	//Refresh the nonlinear state space equations with updated fuzz and vol settings
	void refreshStateSpace();

	//Refresh the nonlinear function matrices
	void refreshNonlinearFunctions();
};