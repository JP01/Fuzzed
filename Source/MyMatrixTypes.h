#pragma once
#include "Eigen/Dense"
#include "ProjectSettings.h"

//Circuit parameters
const int numNodes = 10;  //10 Nodes
const int numRes = 8;  //Number of Resistors
const int numCap = 3;  //Number of Capacitors
const int numInputs = 2;  //Number of inputs
const int numNonLin = 4; //Number of nonlinearities
const int numOutputs = 1; //Number of outputs

/*Define the matrix types for use throughout the Circuit class, predefined to reduce use of Eigen::Dynamic */
//Circuit and System matrices
typedef Eigen::Matrix<double, numOutputs, numRes> ResMatrix; //used for resistor
typedef Eigen::Matrix<double, numRes, numRes> DiagRes;       // used for diagonal resistor matrix
typedef Eigen::Matrix<double, numOutputs, numCap> CapMatrix; //used for capacitor
typedef Eigen::Matrix<double, numCap, numCap> DiagCap; //used for diagonal capacitor
typedef Eigen::Matrix<double, numNodes, numNodes> IntermediateSystemMatrix; //intermediate  matrix used for calculating system matrix
typedef Eigen::Matrix<double, numNodes + numInputs, numNodes + numInputs> SystemMatrix; //used for the system matrix

																						//Incident Matrices
typedef Eigen::Matrix<double, numRes, numNodes> IncidentRes; //used for the incident resistor matrix
typedef Eigen::Matrix<double, numCap, numNodes> IncidentCap; //used for the incident capacitor matrix 
typedef Eigen::Matrix<double, numInputs, numNodes> IncidentVoltage; //used for the incident voltage matrix
typedef Eigen::Matrix<double, numNonLin, numNodes> IncidentNonLin; //used for the incident nonlinearity matrix
typedef Eigen::Matrix<double, numOutputs, numNodes> IncidentOutput; //used for the incident output matrix

																	//Padded matrices for calculation of state space matrices
typedef Eigen::Matrix<double, numCap, numNodes + numInputs> PaddedCap; //used for padded cap matrix in state space calcultions
typedef Eigen::Matrix<double, numNonLin, numNodes + numInputs> PaddedNonLin; //used for padded NonLin matrix in state space calcultions
typedef Eigen::Matrix<double, numOutputs, numNodes + numInputs> PaddedOutput; //used for padded output matrix in state space calcultions
typedef Eigen::Matrix<double, numInputs, numNodes + numInputs> PaddedInput; //used for padded input matrix in state space calcultions

																			//State Space Function Matrices
typedef Eigen::Matrix<double, numCap, numCap> StateSpaceA;
typedef Eigen::Matrix<double, numCap, numInputs> StateSpaceB;
typedef Eigen::Matrix<double, numCap, numNonLin> StateSpaceC;
typedef Eigen::Matrix<double, numOutputs, numCap> StateSpaceD;
typedef Eigen::Matrix<double, numOutputs, numInputs> StateSpaceE;
typedef Eigen::Matrix<double, numOutputs, numNonLin> StateSpaceF;
typedef Eigen::Matrix<double, numNonLin, numCap> StateSpaceG;
typedef Eigen::Matrix<double, numNonLin, numInputs> StateSpaceH;
typedef Eigen::Matrix<double, numNonLin, numNonLin> StateSpaceK;

//Nonlinear function matrices
typedef Eigen::Matrix<double, numNonLin, numNonLin> NonlinearFunctionMatrix;

