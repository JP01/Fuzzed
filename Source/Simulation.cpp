#include "Simulation.h"


//Default Constructor with default values for samplerate and vcc
Simulation::Simulation() : Simulation(DEFAULT_SR, DEFAULT_VCC) {};

//Constructor with Args, calls the superclass constructor with the same sampleRate
Simulation::Simulation(double _sampleRate, double _vcc) : Circuit(_sampleRate)
{
	//set sampleRate to _sampleRate
	sampleRate = _sampleRate;

	//set vcc to _vcc
	vcc = _vcc;

	//Sets up the statespace matrices used in the simulation
	refreshAll();

	//Initialise the matrices used in simulation
	initialiseSimulationParameters();

	//Get the system to a steady State
	getSteadyState();

}


void Simulation::refreshAll()
{
	//Calls the Circuit method to refresh Circuit Vals
	refreshFullCircuit();

	//Sets up the state space matrices
	simStateSpaceA = getStateSpaceMatrix("A");
	simStateSpaceB = getStateSpaceMatrix("B");
	simStateSpaceC = getStateSpaceMatrix("C");
	simStateSpaceD = getStateSpaceMatrix("D");
	simStateSpaceE = getStateSpaceMatrix("E");
	simStateSpaceF = getStateSpaceMatrix("F");
	simStateSpaceG = getStateSpaceMatrix("G");
	simStateSpaceH = getStateSpaceMatrix("H");

	simPSI = getNonlinearFunctionMatrix("psi");
	simAlteredStateSpaceK = getNonlinearFunctionMatrix("alteredStateSpaceK");
	simNonLinEquationMatrix = getNonlinearFunctionMatrix("nonLinEquationMatrix");

	simSaturationCurrent = getSaturationCurrent();
	simThermalVoltage = getThermalVoltage();

}

//Initialise the matrices used in the simulation
void Simulation::initialiseSimulationParameters() {

	//Set up a 3x1 vector stateVector and set all vals to 0 - xz
	stateSpaceVectorMem.setZero();

	//Set up a 4x1 vector vdVector and set all vals to 0 - vdz
	nonLinVoltageVectorMem.setZero();

	//Set up a 2x1 vector inputVector and set all vals to 0 - u
	inputVector.setZero();
}

//Get the system to steady state ready for processing
void Simulation::getSteadyState() {

	//setup the zero input 
	float zero = ZERO_INPUT;
	zeroInput = &zero;

	//MM value is used to get to steady state
	MM = ceil(durfade * sampleRate);

	//TM = 1./MM	
	//Declare the variable TM used in the time vector
	double T = 1 / sampleRate;

	//Resize the vccv vector to match MM
	vccv.resize(MM);
	//Resize the win vector to match 2*MM
	win.resize(2 * MM);
	//Resize the powerUpTimeVector to MM
	powerUpTimeVector.resize(MM);


	/*VCCV Hanning multiplier to achieve steady state*/
	//Hanning win multiplier
	for (int i = 0; i < 2 * MM; i++) {
		//calculate the hanning value at angle "i" 
		double multiplier = 0.5*(1 - cos((2 * PI * i) / (2 * MM - 1)));
		//set the value at index win(i) equal to the multiplier
		win(i) = multiplier;
	}

	//Population loop, populates the vcc powerUpTimeVector and dummyData
	for (int i = 0; i < MM; i++) {
		//Multiply vcc by the ramp up section to get ramp up voltage
		//Multiply the hanning value by max voltage vcc at index "i" and input into vccv
		vccv(i) = win(i) * vcc;

		//t = (0:MM)*T 
		//Populate the time vector
		powerUpTimeVector(i) = i*T;
	}


	//process until steady state is reached
	for (int i = 0; i < MM*steadyStatePeriodFactor; i++) {
		//Process the full MM window then pad the rest of the vccv values with vcc = 9;
		if (i < MM) {
			processSample(zeroInput, vccv(i));
		}
		else {
			processSample(zeroInput, vcc);
		}
	}

}

//Process the incoming sample
void Simulation::processSample(float* _channelData, double _vcc) {

	inputVector(0) = *_channelData; //sets the input vector equal to the input channelData
	inputVector(1) = _vcc;

	//Prepare the nonlinear solver
	//pd = Kdinv*(G*xz + H*u);
	nonLinSolverInput = (simAlteredStateSpaceK.inverse())*(simStateSpaceG*stateSpaceVectorMem + simStateSpaceH*inputVector); //define input to the Nonlinear equation --- MATLAB pd

	nonLinVoltageVector = nonLinVoltageVectorMem; //sets the nonlinVoltageVector as the memorised vector vd = vdz

	nrms = 1.; //sets the nrms high to begin with 
	iteration = 0;
	subIterationTotal = 0;

	while (nrms > tols && iteration < maxIterations) {

		//TERM = IS*exp(vd/VT);
		calcTemp = simSaturationCurrent * (nonLinVoltageVector / simThermalVoltage).array().exp();
		//f = TERM - IS;
		nonLinTransistorFunction = calcTemp.array() - simSaturationCurrent;
		//fd = diag(TERM/VT);
		nonLinTransistorFunctionAltered = (calcTemp / simThermalVoltage).asDiagonal();
		//g = M*vd + f - pd;
		nodalDKNonlinearG = (simNonLinEquationMatrix * nonLinVoltageVector)  //M*vd
			+ nonLinTransistorFunction                           //+ f
			- nonLinSolverInput;                                 //- pd              

																 //gd = M + fd;
		nodalDKNonlinearGAltered = simNonLinEquationMatrix + nonLinTransistorFunctionAltered;
		//STEP = gd\g;
		newtonStep = nodalDKNonlinearGAltered.inverse() * nodalDKNonlinearG;

		//vdnew = vd - STEP;
		nonLinVoltageVectorNew = nonLinVoltageVector - newtonStep;

		//gnew = M*vdnew + IS*(exp(vdnew/VT) - 1) - pd;
		nodalDKNonlinearGNew = (simNonLinEquationMatrix * nonLinVoltageVectorNew) +                            //M*vdnew + 
			(simSaturationCurrent* ((nonLinVoltageVectorNew / simThermalVoltage).array().exp() - 1).matrix())    //IS*(exp(vdnew/VT) - 1)
			- nonLinSolverInput;                                                                               //-pd

																											   //m = 0
		subIterCounter = 0.0;

		//STP = STEP;
		newtonStepTemp = newtonStep;

		while ((nodalDKNonlinearGNew.squaredNorm() > (nodalDKNonlinearG.squaredNorm())) && subIterCounter < maxSubIterations)
		{
			//m = m+1
			subIterCounter++;
			//STP = (2^(-m))*STEP;    % adjusted step 
			newtonStepTemp.array() /= 2.; //half the step with each iteration	
										  //vdnew = vd - STP
			nonLinVoltageVectorNew = nonLinVoltageVector - newtonStepTemp;

			//gnew = M*vdnew + IS*(exp(vdnew/VT) - 1) - pd;
			nodalDKNonlinearGNew = (simNonLinEquationMatrix * nonLinVoltageVectorNew) +                             //M*vdnew + 
				(simSaturationCurrent* ((nonLinVoltageVectorNew / simThermalVoltage).array().exp() - 1).matrix())   //IS*(exp(vdnew/VT) - 1)
				- nonLinSolverInput;                                                                                //-pd																							  
		}
		//nrms = STP'*STP;
		nrms = newtonStepTemp.squaredNorm();  //squared norm used to limit iterations
											  //vd = vdnew;
		nonLinVoltageVector = nonLinVoltageVectorNew;  //set the nonLinVoltageVector to the new value after calculation
													   //subiter = subiter + m;
		subIterationTotal += subIterCounter;  //keep track of the subiterations
											  //iter = iter + 1;
		iteration++; //keep track of the iterations

	}

	//Update nonlinear currents, state and output
	nonLinearCurrent = simPSI * (simSaturationCurrent*((nonLinVoltageVector / simThermalVoltage).array().exp() - 1).matrix());     //  i = PSI*(IS*(exp(vd/VT) - 1));
	stateSpaceVector = (simStateSpaceA * stateSpaceVectorMem) + (simStateSpaceB*inputVector) + (simStateSpaceC*nonLinearCurrent);  //  x = A*xz + B*u + C*i;
	output = ((simStateSpaceD*stateSpaceVectorMem) + (simStateSpaceE*inputVector) + (simStateSpaceF*nonLinearCurrent))(0);         //  y = D*xz + E*u + F*i 
																																   //when declaring output  = RHS, the RHS is technically an Eigen Vector of size 1x1, so you must use the (0) to select the value at index 0

	stateSpaceVectorMem = stateSpaceVector;  //xz = x;   Memorise the stateSpaceVector
	nonLinVoltageVectorMem = nonLinVoltageVector; //vdz = vd;   Memorise the nonLinVoltageVector

	*_channelData = output;
	//return output; //returns the processed sample
}



//Method to set the fuzz and vol params to the arguement vals and then refresh the system.
void Simulation::setParams(double _fuzzVal, double _volVal) {
	Circuit::setParams(_fuzzVal, _volVal);
	//Refresh the system with updated values
	refreshAll();
}



//Set the sampleRate and return the system to steady state
void Simulation::setSimSampleRate(double _sampleRate)
{
	setCircuitSampleRate(_sampleRate);
	sampleRate = _sampleRate;

	//Sets up the statespace matrices used in the simulation
	refreshAll();

	//Initialise the matrices used in simulation
	initialiseSimulationParameters();

	//Get the system to a steady State
	getSteadyState();

}

//Default Destructor
Simulation::~Simulation()
{
}
