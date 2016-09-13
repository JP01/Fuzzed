/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//Create the plugin instance
AudioProcessor* JUCE_CALLTYPE createPluginFilter();

//==============================================================================
FuzzFaceJuceAudioProcessor::FuzzFaceJuceAudioProcessor()
	  : volParam(nullptr),
		fuzzParam(nullptr),
		gainParam(nullptr)
{
	//Create the parameters 
	addParameter(gainParam = new AudioParameterFloat("gain", "Gain", GAIN_MIN, GAIN_MAX, GAIN_DEFAULT));
	addParameter(volParam = new AudioParameterFloat("vol", "Vol", CTRL_MIN, CTRL_MAX, VOL_DEFAULT));
	addParameter(fuzzParam = new AudioParameterFloat("fuzz", "Fuzz", CTRL_MIN, CTRL_MAX, CTRL_MIN));
	
	//Sets the volVal and fuzzVal to defaults
	gainVal = pow(10, (GAIN_DEFAULT/10)); //in dB
	volVal = VOL_DEFAULT;
	fuzzVal = FUZZ_DEFAULT;
	//Sets the frequency for the timerCallback method to 1ms
	startTimerHz(P_TIMER_FREQ);

	//Default the samplerate
	currentSampleRate = DEFAULT_SR;

	//Set the smoothing times
	linFuzzSmoother->reset(currentSampleRate, SMOOTHING_TIME_S);
	linVolSmoother->reset(currentSampleRate, SMOOTHING_TIME_S);
	linGainSmoother->reset(currentSampleRate, SMOOTHING_TIME_S);
}

FuzzFaceJuceAudioProcessor::~FuzzFaceJuceAudioProcessor()
{
}

//==============================================================================
const String FuzzFaceJuceAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FuzzFaceJuceAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FuzzFaceJuceAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double FuzzFaceJuceAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FuzzFaceJuceAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FuzzFaceJuceAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FuzzFaceJuceAudioProcessor::setCurrentProgram (int index)
{
}

const String FuzzFaceJuceAudioProcessor::getProgramName (int index)
{
    return String();
}

void FuzzFaceJuceAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void FuzzFaceJuceAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
}

void FuzzFaceJuceAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FuzzFaceJuceAudioProcessor::setPreferredBusArrangement (bool isInput, int bus, const AudioChannelSet& preferredSet)
{
    // Reject any bus arrangements that are not compatible with your plugin

    const int numChannels = preferredSet.size();

   #if JucePlugin_IsMidiEffect
    if (numChannels != 0)
        return false;
   #elif JucePlugin_IsSynth
    if (isInput || (numChannels != 1 && numChannels != 2))
        return false;
   #else
    if (numChannels != 1 && numChannels != 2)
        return false;

    if (! AudioProcessor::setPreferredBusArrangement (! isInput, bus, preferredSet))
        return false;
   #endif

    return AudioProcessor::setPreferredBusArrangement (isInput, bus, preferredSet);
}
#endif


void FuzzFaceJuceAudioProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{	
	int numberOfSamples = buffer.getNumSamples();

	//Declare the consts for number of channels
	const int totalNumInputChannels = getTotalNumInputChannels();
	const int totalNumOutputChannels = getTotalNumOutputChannels();

	// In case we have more outputs than inputs, this code clears any output
	// channels that didn't contain input data, (because these aren't
	// guaranteed to be empty - they may contain garbage).
	// This is here to avoid people getting screaming feedback
	// when they first compile a plugin, but obviously you don't need to keep
	// this code if your algorithm always overwrites all the output channels.
	for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i) 
		buffer.clear(i, 0, buffer.getNumSamples());

//===========================================================================================

	//If the sampleRate has changes then update the sim samplerate and clipping filter coeffs
	if (currentSampleRate != getSampleRate()) {
		currentSampleRate = getSampleRate();
		//sets the simulation samplerate to current samplerate from DAW
		sim->setSimSampleRate(currentSampleRate);	
		//update the smoothing times
		linFuzzSmoother->reset(currentSampleRate, SMOOTHING_TIME_S);
		linVolSmoother->reset(currentSampleRate, SMOOTHING_TIME_S);
		linGainSmoother->reset(currentSampleRate, SMOOTHING_TIME_S);

	}
		
//===========================================================================================

	//Process Samples		 
	for (int index = 0; index < numberOfSamples; ++index)
	{	
		//each UPDATE_PARAM_SAMPLE_INTERVAL, check the parameters changed and if so update
		if (index % UPDATE_PARAM_SAMPLE_INTERVAL == 0) {
			//Check if fuzz or vol params have changed
			if ((volVal != *volParam) || (fuzzVal != *fuzzParam)) {

				//every interval update the fuzz and vol val to the smoothed fuzz
				fuzzVal = linFuzzSmoother->getNextValue();
				volVal = linVolSmoother->getNextValue();
				//Refresh the sim with new parameter values
				sim->setParams(fuzzVal, volVal);
			}			
		}

		//get the pointer to the channel data at the index
		float* channelData = buffer.getWritePointer(0, index);
		float* channelDataR = buffer.getWritePointer(1, index);

		//Sum the channels to make mono
		*channelData += *channelDataR;

		
		//Get smoothed gain val
		gainVal = linGainSmoother->getNextValue();
		
		//Apply the input gain to the sample
		*channelData *= gainVal;
		
		//Update the variable for displaying the input signal label/meter
		currentInput = abs(*channelData);
	
		//Scale the signal for input to the system and clip the signal if outside allowable range
		inputScaling(channelData);
			
		//process the data
		sim->processSample(channelData, DEFAULT_VCC);

		//Scale the output signal back up to useable level
		*channelData *= OUTPUT_SCALAR;

		//Make both channels produce same output: ie convert back to "stereo"
		*channelDataR = *channelData;
	}
}

//===========================================================================================
//Scale the input, hard clip if the scaled input is still above the CLIPPING POINT, this
//stops the system from crashing when the input signal is too high
void FuzzFaceJuceAudioProcessor::inputScaling(float* _channelData) {
	//Scale the input Signal
	*_channelData *= MAX_INPUT_SIG * pow(2, -INPUT_SCALAR);
	

	//Hard Clipping
	//If signal is greater than max set it to max
	if (*_channelData >= CLIPPING_POINT) {
		*_channelData = CLIPPING_POINT;
	}
	else if (*_channelData <= -CLIPPING_POINT) {
		*_channelData = -CLIPPING_POINT;
	}

}

//Call this method at P_TIMER_FREQ hz to update the circuit param vals
void FuzzFaceJuceAudioProcessor::timerCallback() 
{
	//Every timer callback set the new target parameter val to the current setting on the knob
	linFuzzSmoother->setValue(*fuzzParam);
	linVolSmoother->setValue(*volParam);
	//for decibel conversion
	linGainSmoother->setValue(pow(10, (*gainParam / 10)));
}

//==============================================================================
bool FuzzFaceJuceAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* FuzzFaceJuceAudioProcessor::createEditor()
{
    return new FuzzFaceJuceAudioProcessorEditor (*this);
}

//==============================================================================
void FuzzFaceJuceAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    //Create an XML element to store the parameter values
	XmlElement xml("FUZZFACESETTINGS");

	// Store the values of all our parameters, using their param ID as the XML attribute
	for (int i = 0; i < getNumParameters(); ++i)
		if (AudioProcessorParameterWithID* p = dynamic_cast<AudioProcessorParameterWithID*> (getParameters().getUnchecked(i)))
			xml.setAttribute(p->paramID, p->getValue());

	// then use this helper function to stuff it into the binary blob and return it..
	copyXmlToBinary(xml, destData);
}

void FuzzFaceJuceAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	// This getXmlFromBinary() helper function retrieves our XML from the binary blob..
	ScopedPointer<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState != nullptr)
	{
		// make sure that it's actually our type of XML object..
		if (xmlState->hasTagName("FUZZFACESETTINGS"))
		{
			// reload the parameters
			for (int i = 0; i < getNumParameters(); ++i)
				if (AudioProcessorParameterWithID* p = dynamic_cast<AudioProcessorParameterWithID*> (getParameters().getUnchecked(i)))
					p->setValueNotifyingHost((float)xmlState->getDoubleAttribute(p->paramID, p->getValue()));
		}
	}
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FuzzFaceJuceAudioProcessor();
}
