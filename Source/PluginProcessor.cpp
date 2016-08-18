/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
FuzzFaceJuceAudioProcessor::FuzzFaceJuceAudioProcessor()
{
	//Initisalise the vals for smoothing later
	fuzzVal = smoothedFuzz = FUZZ_DEFAULT;
	volVal = smoothedVol = VOL_DEFAULT;
	smoothingCoeff = exp(-2.0 * M_PI * smoothingCutoff / currentSampleRate);
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
	//If the sampleRate has changes then update the sim samplerate
	if (currentSampleRate != getSampleRate()) {
		currentSampleRate = getSampleRate();
		sim.setSimSampleRate(currentSampleRate);
		smoothingCoeff = exp(-2.0 * M_PI * smoothingCutoff / currentSampleRate);
	}

	const int totalNumInputChannels = getTotalNumInputChannels();
	const int totalNumOutputChannels = getTotalNumOutputChannels();
	
	smoothedFuzz = fuzzVal + smoothingCoeff*(smoothedFuzz - fuzzVal);
	sim.setFuzz(smoothedFuzz);
        
	sim.setVol(volVal);

	// In case we have more outputs than inputs, this code clears any output
	// channels that didn't contain input data, (because these aren't
	// guaranteed to be empty - they may contain garbage).
	// This is here to avoid people getting screaming feedback
	// when they first compile a plugin, but obviously you don't need to keep
	// this code if your algorithm always overwrites all the output channels.
	for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());


	// This is the place where you'd normally do the guts of your plugin's
   // audio processing...


   //Set channel to 0 as the plugin is MONO
	int channel = 0;

	//Resize the eigenBuffers to store all the samples
	eigenInputBuffer.resize(buffer.getNumSamples());
	eigenOutputBuffer.resize(buffer.getNumSamples());

	//Fill the input buffer
	for (int index = 0; index < buffer.getNumSamples(); ++index)
	{
		//Fill the input buffer with sample at corresponding channel and index
		eigenInputBuffer(index) = buffer.getSample(channel, index);
	}

	/*Process*/
	//Scale the buffer for input to system
	eigenInputBuffer *= 0.1;

	//process the scaled data
	eigenOutputBuffer = sim.processBuffer(eigenInputBuffer);

	//scale the buffer for output to system
	eigenOutputBuffer *= 2;

	//For each output channel write the output data
	for (channel = 0; channel < totalNumOutputChannels; ++channel) {
		//write the data
		for (int index = 0; index < buffer.getNumSamples(); ++index)
		{
			//get the pointer to the channel data at the index
			float* channelData = buffer.getWritePointer(channel, index);
			*channelData = eigenOutputBuffer(index);
		}
	}

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
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FuzzFaceJuceAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
	
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FuzzFaceJuceAudioProcessor();
}
