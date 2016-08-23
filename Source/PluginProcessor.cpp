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
	addParameter(fuzzParam = new AudioParameterFloat("fuzz", "Fuzz", CTRL_MIN, CTRL_MAX, FUZZ_DEFAULT));
	
	//Sets the volVal and fuzzVal to defaults
	gainVal = pow(10, (GAIN_DEFAULT/10)); //in dB
	volVal = VOL_DEFAULT;
	fuzzVal = FUZZ_DEFAULT;
	//Sets the frequency for the timerCallback method to 1ms
	startTimerHz(P_TIMER_FREQ);
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
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
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
	//Declare the consts for number of channels
	const int totalNumInputChannels = getTotalNumInputChannels();
	const int totalNumOutputChannels = getTotalNumOutputChannels();

	//Set the channel to 0 as the plugin is mono
	const int channel = 0;

	// In case we have more outputs than inputs, this code clears any output
	// channels that didn't contain input data, (because these aren't
	// guaranteed to be empty - they may contain garbage).
	// This is here to avoid people getting screaming feedback
	// when they first compile a plugin, but obviously you don't need to keep
	// this code if your algorithm always overwrites all the output channels.
	for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i) 
		buffer.clear(i, 0, buffer.getNumSamples());


	//If the sampleRate has changes then update the sim samplerate
	if (currentSampleRate != getSampleRate()) {
		currentSampleRate = getSampleRate();
		sim.setSimSampleRate(currentSampleRate);
	}

	//Check if the gainVal matches the parameter and update
	if (gainVal = pow(10, (*gainParam / 10))) {
		//Apply the input gain
		buffer.applyGain(gainVal);
	}


	//Process		
	for (int index = 0; index < buffer.getNumSamples(); ++index)
	{	
		//get the pointer to the channel data at the index
		float* channelData = buffer.getWritePointer(channel, index);
		//Scale the data for processing
		*channelData *= 0.05;
		//process the data
		sim.processSample(channelData, DEFAULT_VCC);
	}
	

}

//Call this method at P_TIMER_FREQ hz to update the circuit param vals
void FuzzFaceJuceAudioProcessor::timerCallback() 
{
	//Set the gainVal to the value from the slider in dB
	gainVal = pow(10,(*gainParam/10));

	//Check if fuzz or vol params have changed
	if ((volVal != *volParam) || (fuzzVal != *fuzzParam)) {
		//Set the new target value
		volVal = smooth(*volParam);
		fuzzVal = smooth(*fuzzParam);			
		
		//Refresh the sim
		sim.setParams(fuzzVal, volVal);
	}




}

double FuzzFaceJuceAudioProcessor::smooth(double input) {
	volVal = SMOOTH_COEFF * (volVal - input) + input;
	return volVal;
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
