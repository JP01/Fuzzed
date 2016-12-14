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

	//Declare variables for the individual elements of the matrices which cause zipper noise, where cTarget13 = C(1,3) etc.
	double aTarget22, cTarget13, cTarget23, kTarget33, kTarget34, kTarget44;
	//Update the values causing zipper noise with smoothed values each sample
	void updateZipperMatrices();
	void updateZipperTargets();
	//the smoothed values
	double aStore22, cStore13, cStore23, kStore33, kStore34, kStore44;
	//the coefficient which governs the smoothing of zipper matrix values
	double kCoeff;
			
	double getFuzz();                //returns the fuzz parameter
	double getVol();                 //returns the volume parameter


	//Declare the statespace terms and their constant elements
	StateSpaceA stateSpaceA;
	StateSpaceB stateSpaceB;
	StateSpaceC stateSpaceC;
	StateSpaceD stateSpaceD;
	StateSpaceE stateSpaceE;
	StateSpaceF stateSpaceF;
	StateSpaceG stateSpaceG;
	StateSpaceH stateSpaceH;
	StateSpaceK stateSpaceK;
	
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
	StateSpaceA stateSpaceA0;
	StateSpaceB stateSpaceB0;
	StateSpaceC stateSpaceC0;
	StateSpaceD stateSpaceD0;
	StateSpaceE stateSpaceE0;
	StateSpaceF stateSpaceF0;
	StateSpaceG stateSpaceG0;
	StateSpaceH stateSpaceH0;
	StateSpaceK stateSpaceK0;

	//Declare the constant state space terms used in Holters method
	StateSpaceQ stateSpaceQ;
	StateSpaceUx stateSpaceUx;
	StateSpaceUo stateSpaceUo;
	StateSpaceUn stateSpaceUn;
	StateSpaceUu stateSpaceUu;

	//Sample period
	double samplePeriod;

	//PNP Bipolar Junction Transistor Values
	static double forwardGain; //200 or 110
	static double forwardGain2;
	static double reverseGain;
	static double thermalVoltage;
	static double saturationCurrent;

	/*
	*   Nonlinear function matrices, set as 4x4 matrices
	*/
	NonlinearFunctionMatrix psi; //Ebers-moller term
	NonlinearFunctionMatrix phi; //Ebers-moller term
	NonlinearFunctionMatrix nonLinEquationMatrix;   //Nonlinear equation matrix (MATLAB - M)
	NonlinearFunctionMatrix alteredStateSpaceK;  //Kd is an altered form of the K statespace matrix (MATLAB - kd)


private: //access control

	//Initialise all the circuit values r1,r2 etc etc
	void initialiseValues();


	double fuzz;  //value for the fuzz parameter
	double vol;   //value for the vol parameter


	 //potentiometer resistances
	static double volPotRes;
	static double fuzzPotRes;
	double volPotVar1, volPotVar2, fuzzPotVar1, fuzzPotVar2; //Variables for the potentiometer

	//Resistors Values
	static double r1;
	static double r2 ;
	static double r3;
	static double r4;
	static double r5;
	static double r6;
	static double r7;
	static double r8;

	//Capacitors Values
	static double c1;
	static double c2;
	static double c3;

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