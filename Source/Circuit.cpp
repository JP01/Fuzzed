#include "Circuit.h"

/*Default Constructor*/
//Circuit::Circuit() { std::cout << "Circuit Created!" << std::endl; };

/*Calls the default constructor with default 44.1k Hz sample rate */
Circuit::Circuit() : Circuit(DEFAULT_SR) {};

/*Constructor which takes sampleRate as an arguement and initialises the sampling period to 1./sampleRate */
Circuit::Circuit(double _sampleRate) :samplePeriod(1. / _sampleRate) {

	//Set the sampleRate of the circuit to input _sampleRate
	sampleRate = _sampleRate;

	std::cout << "Circuit Created with SR: " << sampleRate << std::endl;

	//Perform initial setup of the circuit
	setupCircuit();

}

/* Initial setup of default circuit values and parmaters*/
void Circuit::setupCircuit() {
	/* Setup */
	//Initialise controllable paramaters
	setVol(defaultVol);
	setFuzz(defaultFuzz);
	//Populate circuit matrices
	populateCircuitMatrices();
	//Initialise the incident matrices
	initialiseIncidentMatrices();
	//Initialise the system matrices
	refreshFullCircuit();
}

/* Initial setup of circuit matrices */
void Circuit::populateCircuitMatrices() {
	//Update the resistor values
	r4 = (1 - vol)*500e3;
	r5 = vol*500e3;
	r7 = (1 - fuzz)*1e3;
	r8 = fuzz*1e3;

	//prep the resistor values for input into the diagonal matrix
	resMatrix << 1 / r1, 1 / r2, 1 / r3, 1 / r4, 1 / r5, 1 / r6, 1 / r7, 1 / r8;

	//prep the capacitor values for input into diagonal matrix
	capMatrix << c1, c2, c3;
	capMatrix = (2 * capMatrix) / samplePeriod;

	//Convert the matrices to diagonal matrices
	diagResMatrix = resMatrix.asDiagonal();
	diagCapMatrix = capMatrix.asDiagonal();

}

/*One time setup of incident matrices, this is performed in the constructor and sets up the incident matrices*/
void Circuit::initialiseIncidentMatrices() {

	//The incident matrix for the resistors
	incidentResistors <<
		0, 0, -1, 1, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, -1, 1, 0, 0, 0, 0,
		0, 0, 0, 1, 0, -1, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 1, -1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		0, 1, 0, 0, 0, 0, -1, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 1, -1, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 1, 0, 0;

	//The incident matrix for the Capacitors
	incidentCapacitors <<
		1, -1, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 1, 0, 0, -1, 0;

	//The incident matrix for Voltage
	incidentVoltage <<
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, -1, 0, 0, 0, 0, 0, 0;

	//The incident matrix for the nonlinearities
	incidentNonlinearities <<
		0, -1, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, -1, 0, 0, 0, 0, 0, 0, 0,
		0, 0, -1, 0, 0, 0, 1, 0, 0, 0,
		0, 0, 0, 0, -1, 0, 1, 0, 0, 0;

	//The incident matrix for the output
	incidentOutput <<
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1;
}

/* Refresh all matrices in the system when fuzz or vol changes*/
void Circuit::refreshFullCircuit() {
	refreshCircuitMatrices();
	refreshSystemMatrix();
	refreshNonLinStateSpace();
	refreshNonlinearFunctions();
}

/*Function to populate the circuit matrices*/
void Circuit::refreshCircuitMatrices() {

	//Update the resistor values
	r4 = (1 - vol)*500e3;
	r5 = vol*500e3;
	r7 = (1 - fuzz)*1e3;
	r8 = fuzz*1e3;

	//update the variable resistor values
	resMatrix(0, 3) = 1 / r4;
	resMatrix(0, 4) = 1 / r5;
	resMatrix(0, 6) = 1 / r7;
	resMatrix(0, 7) = 1 / r8;

	//Convert the matrices to diagonal matrices
	diagResMatrix = resMatrix.asDiagonal();

}

/* Function used to refresh the system matrix, call when fuzz or vol is changed, called by refresh() */
void Circuit::refreshSystemMatrix() {

	//Update the matrix systemRes with the resistor element of the system Matrix
	systemRes = (incidentResistors.transpose())*diagResMatrix*incidentResistors;

	//Update the matrix systemCap with the capacitor element of the system Matrix
	systemCap = (incidentCapacitors.transpose())*diagCapMatrix*incidentCapacitors;

	//Construct system matrix
	systemMatrix.block(0, 0, numNodes, numNodes) = systemRes + systemCap;  //sets the first 10x10 top left matrix to be the sum of systemRes and systemCap matrices
	systemMatrix.block(0, numNodes, numNodes, numInputs) = incidentVoltage.transpose();  //sets the 10x2 matrix starting at (0,10) equal to the transpose of incidentVoltage matrix
	systemMatrix.block(numNodes, 0, numInputs, numNodes) = incidentVoltage;  //sets the 2x10 matrix starting at (10,0) equal to the incidentVoltage matrix
	systemMatrix.block(numNodes, numNodes, numInputs, numInputs).setZero();  //sets the last 2x2 matrix in the bottom right corner to 0

}

/* Function used to refrseh the nonlinear state space terms, called by refresh()*/
void Circuit::refreshNonLinStateSpace() {
	/* Padded matrices used in state space term calculations*/
	//Padded Capacitor Matrix
	padC.block(0, 0, numCap, numNodes) = incidentCapacitors;
	padC.block(0, numNodes, numCap, numInputs).setZero();

	//Padded NonLinearity Matrix
	padNL.block(0, 0, numNonLin, numNodes) = incidentNonlinearities;
	padNL.block(0, numNodes, numNonLin, numInputs).setZero();

	//Padded output Matrix
	padO.block(0, 0, numOutputs, numNodes) = incidentOutput;
	padO.block(0, numNodes, numOutputs, numInputs).setZero();

	//Padded identity matrix
	padI.block(0, 0, numInputs, numNodes).setZero();
	padI.block(0, numNodes, numInputs, numInputs).setIdentity();

	//Calculate State Space Matrices
	stateSpaceA = 2 * diagCapMatrix*padC*systemMatrix.partialPivLu().solve(padC.transpose()) - Eigen::MatrixXd::Identity(3, 3);  //populate
	stateSpaceB = 2 * diagCapMatrix*padC*systemMatrix.partialPivLu().solve(padI.transpose()); //populate
	stateSpaceC = -2 * diagCapMatrix*padC*systemMatrix.partialPivLu().solve(padNL.transpose()); //populate
	stateSpaceD = padO*systemMatrix.partialPivLu().solve(padC.transpose()); //populate
	stateSpaceE = padO*systemMatrix.partialPivLu().solve(padI.transpose()); //populate
	stateSpaceF = -padO*systemMatrix.partialPivLu().solve(padNL.transpose()); //populate
	stateSpaceG = padNL*systemMatrix.partialPivLu().solve(padC.transpose()); //populate
	stateSpaceH = padNL*systemMatrix.partialPivLu().solve(padI.transpose()); //populate
	stateSpaceK = -padNL*systemMatrix.partialPivLu().solve(padNL.transpose()); //populate
}

/* Function used to refresh the nonlinear function matrices, called by refresh() */
void Circuit::refreshNonlinearFunctions() {
	//Input the psi values
	psi << 1 / forwardGain, 1 / reverseGain, 0, 0,
		1, -(reverseGain + 1) / reverseGain, 0, 0,
		0, 0, 1 / forwardGain, 1 / reverseGain,
		0, 0, 1, -(reverseGain + 1) / reverseGain;

	//Input the phi values
	phi << 1, 0, 0, 0,
		1, -1, 0, 0,
		0, 0, 1, 0,
		0, 0, 1, -1;

	//Input the Kd values
	alteredStateSpaceK << -stateSpaceK*psi;

	//Input the M values
	nonLinEquationMatrix = alteredStateSpaceK.inverse()*phi.inverse();
}

/* Create a setter for the Fuzz parameter, when input is outside the allowable range 0 > fuzzVal > 1 */
void Circuit::setFuzz(double _fuzz) {
	//Checks value is within allowable upper range, if the value is greater than 1-0.001 = 0.999 then default to 0.999
	if (_fuzz > CTRL_MAX - CTRL_INCREMENT) 
	{
		//Defaults value to MAX - INCREMENT and prints a message
		std::cout << "Fuzz greater than range, defaulted to " << CTRL_MAX - CTRL_INCREMENT << std::endl;
		fuzz = CTRL_MAX - CTRL_INCREMENT;
	} 
	//Checks value is within allowable lower range, if the valuse is less than 0.0 + 0.001 = 0.001 then default to 0.001
	else if (_fuzz < CTRL_MIN + CTRL_INCREMENT)
	{
		//Defaults value to Min and prints a message
		std::cout << "Fuzz less than range, defaulted to " << CTRL_MIN + CTRL_INCREMENT << std::endl;
		fuzz = CTRL_MIN + CTRL_INCREMENT;
	} 	
	else
	{
		//0 <_fuzz < 1 set fuzz to the arguement
		fuzz = _fuzz;
	}
}

/* Returns the current value for the fuzz setting */
double Circuit::getFuzz()
{
	return fuzz;
}

/* Create a setter for the Vol parameter, when input is outside the allowable range 0 > volVal > 1, default to 0.4 */
void Circuit::setVol(double _vol) {
	//Checks value is within allowable upper range, if the value is greater than 1-0.001 = 0.999 then default to 0.999
	if (_vol > CTRL_MAX - CTRL_INCREMENT)
	{
		//Defaults value to MAX - INCREMENT and prints a message
		std::cout << "Vol greater than range, defaulted to " << CTRL_MAX - CTRL_INCREMENT << std::endl;
		vol = CTRL_MAX - CTRL_INCREMENT;
	}
	//Checks value is within allowable lower range, if the valuse is less than 0.0 + 0.001 = 0.001 then default to 0.001
	else if (_vol < CTRL_MIN + CTRL_INCREMENT)
	{
		//Defaults value to Min + Increment and prints a message
		std::cout << "Vol less than range, defaulted to " << CTRL_MIN + CTRL_INCREMENT << std::endl;
		vol = CTRL_MIN + CTRL_INCREMENT;
	}
	else
	{
		//0 <_vol < 1 set fuzz to the arguement
		vol = _vol;
	}
}

/* Returns the current value for the vol setting*/
double Circuit::getVol()
{
	return vol;
}

//Method to set the fuzz and vol params to the arguement vals and then refresh the system.
void Circuit::setParams(double _fuzzVal, double _volVal) {
	setFuzz(_fuzzVal);
	setVol(_volVal);
}

/* Returns the circuit saturation current IS */
double Circuit::getSaturationCurrent()
{
	return saturationCurrent;
}

/* Returns the circuit thermal voltage VT*/
double Circuit::getThermalVoltage()
{
	return thermalVoltage;
}

/*Sets samplerate to new samplerate and refreshes the circuit matrices*/
void Circuit::setCircuitSampleRate(double _sampleRate)
{
	sampleRate = _sampleRate;
	populateCircuitMatrices();
	refreshFullCircuit();
}

double Circuit::getCircuitSampleRate() {
	return sampleRate;
}



//Return the specified nonlinear function matrix
Eigen::MatrixXd Circuit::getNonlinearFunctionMatrix(std::string input) {
	if (input == "psi") { return psi; }
	if (input == "phi") { return phi; }
	if (input == "nonLinEquationMatrix") { return nonLinEquationMatrix; }
	if (input == "alteredStateSpaceK") { return alteredStateSpaceK; }
	else {
		std::cout << "Input \"" << input << "\" not recognised, defaulted output is matrix PSI" << std::endl;
		return psi;
	}
}


//Return the specified state space matrix, takes capital letters only A-K
Eigen::MatrixXd Circuit::getStateSpaceMatrix(std::string input) {

	if (input == "A") { return stateSpaceA; }
	if (input == "B") { return stateSpaceB; }
	if (input == "C") { return stateSpaceC; }
	if (input == "D") { return stateSpaceD; }
	if (input == "E") { return stateSpaceE; }
	if (input == "F") { return stateSpaceF; }
	if (input == "G") { return stateSpaceG; }
	if (input == "H") { return stateSpaceH; }
	if (input == "K") { return stateSpaceK; }
	else {
		std::cout << "Input \"" << input << "\" not recognised, defaulted output is matrix A";
		return stateSpaceA;
	}

}



/*Default Destructor */
Circuit::~Circuit() {
	//Cleanup
	std::cout << "Circuit Destroyed" << std::endl;
}

