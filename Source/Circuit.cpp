#include "Circuit.h"

/*Default Constructor*/
//Circuit::Circuit() { std::cout << "Circuit Created!" << std::endl; };

/*Calls the default constructor with default 44.1k Hz sample rate */


/*Constructor which takes sampleRate as an arguement and initialises the sampling period to 1./sampleRate */
Circuit::Circuit(double _sampleRate) :samplePeriod(1. / _sampleRate) {

	//Set the sampleRate of the circuit to input _sampleRate
	sampleRate = _sampleRate;

	std::cout << "Circuit Created with SR: " << sampleRate << std::endl;

	//Initialise Default circuit values
	initialiseValues();

	//Initialise controllable paramaters
	setParams(FUZZ_DEFAULT, VOL_DEFAULT);

	//Perform initial setup of the circuit
	setupCircuit();

	//initialise the zipper value smoothing coefficient
	kCoeff = exp(-2.0 * PI * (ZIP_SMOOTH / sampleRate));
}


void Circuit::initialiseValues() {
	//BJT Values
	forwardGain = 70;
	forwardGain2 = 110;
	reverseGain = 2;
	thermalVoltage = 25.8e-3;
	saturationCurrent = 1e-14;

	//Potentiometer base resistances
	volPotRes = 500e3;
	fuzzPotRes = 1e3;

	//Resistors Values
	r1 = 33e3;
	r2 = 8.2e3;
	r3 = 470;
	r4 = 2 * volPotRes;
	r5 = 2 * volPotRes;
	r6 = 100e3;
	r7 = 2 * fuzzPotRes;
	r8 = 2 * fuzzPotRes;

	//Capacitors Values
	c1 = 2.2e-6;
	c2 = 20e-6;
	c3 = 10e-9;

	//Initialise the psi values
	psi << 1 / forwardGain2, 1 / reverseGain, 0, 0,
		1, -(reverseGain + 1) / reverseGain, 0, 0,
		0, 0, 1 / forwardGain, 1 / reverseGain,
		0, 0, 1, -(reverseGain + 1) / reverseGain;

	//Initialise the phi values
	phi << 1, 0, 0, 0,
		1, -1, 0, 0,
		0, 0, 1, 0,
		0, 0, 1, -1;
}

/* Initial setup of default circuit values and parameters*/
void Circuit::setupCircuit() {
	/* Setup */
	//Populate circuit matrices
	populateComponentMatrices();
	//Initialise the incident matrices
	initialiseIncidentMatrices();
	//Populate the constant system matrix
	populateConstantSystemMatrix();
	//Populate the constant statespace terms
	populateConstantStateSpaceTerms();
	//Initialise the system matrices
	refreshFullCircuit();

	//initialise the zipper values
	aStore22 = stateSpaceA(1, 1);

    cStore13 = stateSpaceC(0, 2);
	cStore23 = stateSpaceC(1, 2);

	kStore33 = stateSpaceK(2, 2);
	kStore34 = stateSpaceK(2, 3);
	kStore44 = stateSpaceK(3, 3);

}

/* Initial setup of circuit matrices.
Set up the constant elements then refresh the variable elements*/
void Circuit::populateComponentMatrices() {
	//prep the resistor values for input into the diagonal matrix
	resMatrix << 1 / r1, 1 / r2, 1 / r3, 1 / r4, 1 / r5, 1 / r6, 1 / r7, 1 / r8;

	//prep the capacitor values for input into diagonal matrix
	capMatrix << c1, c2, c3;
	capMatrix = (2 * capMatrix) / samplePeriod;

	//Convert the matrices to diagonal matrices
	diagResMatrix = resMatrix.asDiagonal();
	diagCapMatrix = capMatrix.asDiagonal();

	//Refresh the potentiometerMatrices
	refreshPotentiometerMatrices();
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

	//The incident matrix for the potentiometers
	incidentPot <<
		0, 0, 0, 0, 0, 0, 0, 0, 1, -1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
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

/* Refresh all variable matrices in the system when fuzz or vol changes*/
void Circuit::refreshFullCircuit() {
	refreshPotentiometerMatrices();
	refreshStateSpace();
	refreshNonlinearFunctions();
}

/*Function to populate the circuit matrices*/
void Circuit::refreshPotentiometerMatrices() {
	//Update the resistor values
	volPotVar1 = (2 * (1 - vol)*volPotRes) / (2 - (1 - vol));
	volPotVar2 = (2 * vol*volPotRes) / (2 - vol);
	fuzzPotVar1 = (2 * (1 - fuzz)*fuzzPotRes) / (2 - (1 - fuzz));
	fuzzPotVar2 = (2 * fuzz*fuzzPotRes) / (2 - fuzz);

	//populate the potentiometer matrix
	potMatrix << volPotVar1, volPotVar2, fuzzPotVar1, fuzzPotVar2;

	//Diagonalise the matrix
	diagPotMatrix = potMatrix.asDiagonal();
}

/* Function used to populate the constant elements of system matrix, call when fuzz or vol is changed, called by refresh() */
void Circuit::populateConstantSystemMatrix() {
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

//Populate Constant StateSpace
//Function used to populate the constant elements of the state space matrices
void Circuit::populateConstantStateSpaceTerms() {
	/* Padded matrices used in state space term calculations*/
	//Padded Potentiometer Matrix 
	padPot.block(0, 0, numPot, numNodes) = incidentPot;
	padPot.block(0, numNodes, numPot, numInputs).setZero();

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

	/*Populate the constant terms*/
	//Q = [N_V zeros(nv,nu)]*(S0\([N_V zeros(nv,nu)].'));
	stateSpaceQ = padPot * systemMatrix.partialPivLu().solve(padPot.transpose());
	//Ux = [N_X zeros(nx,nu)]*(S0\([N_V zeros(nv,nu)].'));
	stateSpaceUx = padC * systemMatrix.partialPivLu().solve(padPot.transpose());
	//Uo = [N_O zeros(1,nu)]*(S0\([N_V zeros(nv,nu)].'));
	stateSpaceUo = padO * systemMatrix.partialPivLu().solve(padPot.transpose());
	//Un = [N_N zeros(nn,nu)]*(S0\([N_V zeros(nv,nu)].'));
	stateSpaceUn = padNL * systemMatrix.partialPivLu().solve(padPot.transpose());
	//Uu = [zeros(nu,ns) eye(nu)]*(S0\([N_V zeros(nv,nu)].'));
	stateSpaceUu = padI * systemMatrix.partialPivLu().solve(padPot.transpose());
	//A0 = 2*G_X*[N_X zeros(nx,nu)]*(S0\[N_X zeros(nx, nu)].')- eye(nx);
	stateSpaceA0 = 2 * diagCapMatrix*padC*systemMatrix.partialPivLu().solve(padC.transpose()) - Eigen::MatrixXd::Identity(numCap, numCap);  //populate
    //B0 = 2*G_X*[N_X zeros(nx,nu)]*(S0\[zeros(nu,ns) eye(nu)].');
	stateSpaceB0 = 2 * diagCapMatrix*padC*systemMatrix.partialPivLu().solve(padI.transpose()); //populate
	//C0 = 2*G_X*[N_X zeros(nx,nu)]*(S0\[N_N zeros(nn,nu)].');
	stateSpaceC0 = 2 * diagCapMatrix*padC*systemMatrix.partialPivLu().solve(padNL.transpose()); //populate
	//D0 = [N_O zeros(1,nu)]*(S0\[N_X zeros(nx,nu)].');
	stateSpaceD0 = padO*systemMatrix.partialPivLu().solve(padC.transpose()); //populate
	//E0 = [N_O zeros(1,nu)]*(S0\[zeros(2,10) eye(2)].');
	stateSpaceE0 = padO*systemMatrix.partialPivLu().solve(padI.transpose()); //populate
	//F0 = [N_O zeros(1,nu)]*(S0\[N_N zeros(nn,nu)].');
	stateSpaceF0 = padO*systemMatrix.partialPivLu().solve(padNL.transpose()); //populate
	//G0 = [N_N zeros(nn,nu)]*(S0\[N_X zeros(nx,nu)].');
	stateSpaceG0 = padNL*systemMatrix.partialPivLu().solve(padC.transpose()); //populate
	//H0 = [N_N zeros(nn,nu)]*(S0\[zeros(nu,ns) eye(nu)].');
	stateSpaceH0 = padNL*systemMatrix.partialPivLu().solve(padI.transpose()); //populate
	//K0 = [N_N zeros(nn,nu)]*(S0\[N_N zeros(nn,nu)].');
	stateSpaceK0 = padNL*systemMatrix.partialPivLu().solve(padNL.transpose()); //populate

}

/* Function used to refrseh the final state space terms, combines the variable elements with the constant terms, called by refresh()*/
void Circuit::refreshStateSpace() {
	/*Refresh the final statespace terms*/
	//A = A0 - 2*G_X*Ux*((R_V+Q)\(Ux.'));
	stateSpaceA = stateSpaceA0 - (2 * diagCapMatrix*stateSpaceUx*((diagPotMatrix + stateSpaceQ).partialPivLu().solve(stateSpaceUx.transpose())));
	//B = B0 - 2*G_X*Ux*((R_V+Q)\(Uu.'));
	stateSpaceB = stateSpaceB0 - (2 * diagCapMatrix*stateSpaceUx*((diagPotMatrix + stateSpaceQ).partialPivLu().solve(stateSpaceUu.transpose())));
	//C = -(C0 - 2 * G_X*Ux*((R_V + Q)\(Un.')));
	stateSpaceC = -(stateSpaceC0 - (2 * diagCapMatrix*stateSpaceUx*((diagPotMatrix + stateSpaceQ).partialPivLu().solve(stateSpaceUn.transpose()))));
	//D = D0 - Uo*((R_V + Q)\(Ux.'));
	stateSpaceD = stateSpaceD0 - stateSpaceUo*((diagPotMatrix + stateSpaceQ).partialPivLu().solve(stateSpaceUx.transpose()));
	//E = E0 - Uo*((R_V+Q)\(Uu.'));
	stateSpaceE = stateSpaceE0 - stateSpaceUo*((diagPotMatrix + stateSpaceQ).partialPivLu().solve(stateSpaceUu.transpose()));
	//F = -(F0 - Uo*((R_V+Q)\(Un.')));
	stateSpaceF = -(stateSpaceF0 - stateSpaceUo*((diagPotMatrix + stateSpaceQ).partialPivLu().solve(stateSpaceUn.transpose())));
	//G = G0 - Un*((R_V+Q)\(Ux.'));
	stateSpaceG = stateSpaceG0 - stateSpaceUn*((diagPotMatrix + stateSpaceQ).partialPivLu().solve(stateSpaceUx.transpose()));
	//H = H0 - Un*((R_V+Q)\(Uu.'));
	stateSpaceH = stateSpaceH0 - stateSpaceUn*((diagPotMatrix + stateSpaceQ).partialPivLu().solve(stateSpaceUu.transpose()));
	//K = -(K0 - Un*((R_V+Q)\(Un.')));   
	stateSpaceK = -(stateSpaceK0 - stateSpaceUn*((diagPotMatrix + stateSpaceQ).partialPivLu().solve(stateSpaceUn.transpose())));

	updateZipperTargets();
}

//Update the target values for the zipper matrices
void Circuit::updateZipperTargets() {
	//Set the target values for the zipper causing elements
	aTarget22 = stateSpaceA(1, 1);

	cTarget13 = stateSpaceC(0, 2);
	cTarget23 = stateSpaceC(1, 2);

	kTarget33 = stateSpaceK(2, 2);
	kTarget34 = stateSpaceK(2, 3);
	kTarget44 = stateSpaceK(3, 3);
}

//Update the values of the zipper matrices with smoothed values every sample
void Circuit::updateZipperMatrices() {

	aStore22 = aTarget22*(1. - kCoeff) + aStore22*kCoeff;
	stateSpaceA(1, 1) = aStore22;

	cStore13 = cTarget13*(1. - kCoeff) + cStore13*kCoeff;
	stateSpaceC(0, 2) = cStore13;
	cStore23 = cTarget23*(1. - kCoeff) + cStore23*kCoeff;
	stateSpaceC(1, 2) = cStore23;

	kStore33 = kTarget33*(1. - kCoeff) + kStore33*kCoeff;
	stateSpaceK(2, 2) = kStore33;
	kStore34 = kTarget34*(1. - kCoeff) + kStore34*kCoeff;
	stateSpaceK(2, 3) = kStore34;
	stateSpaceK(3, 2) = kStore34;
	kStore44 = kTarget44*(1. - kCoeff) + kStore44*kCoeff;
	stateSpaceK(3, 3) = kStore44;

	//update the nonlinear functions with new K values
	refreshNonlinearFunctions();
}


/* Function used to refresh the nonlinear function matrices, called by refresh() */
void Circuit::refreshNonlinearFunctions() {
	//Input the Kd values
	alteredStateSpaceK = -stateSpaceK*psi;

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
	refreshPotentiometerMatrices();
	refreshStateSpace();
	//refreshNonlinearFunctions();
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
	samplePeriod = 1. / sampleRate;
	setupCircuit();
}

double Circuit::getCircuitSampleRate() {
	return sampleRate;
}

/*Default Destructor */
Circuit::~Circuit() {

	//Cleanup
	std::cout << "Circuit Destroyed" << std::endl;
}

