/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Simulation.h"


//==============================================================================
/**
*/
class FuzzFaceJuceAudioProcessor  : public AudioProcessor, private Timer
{
public:
    //==============================================================================
    FuzzFaceJuceAudioProcessor();
    ~FuzzFaceJuceAudioProcessor();

	//==============================================================================
	//Instance of the simulation class, using scoped pointer to avoid memory leaking
	ScopedPointer<Simulation> sim;

	//Parameters
	AudioParameterFloat* volParam, *fuzzParam, *gainParam;

	//The actual values for the parameters
	double volVal, fuzzVal, gainVal;

	//Parameter smoothing
	ScopedPointer<LinearSmoothedValue<double>> linFuzzSmoother;
	ScopedPointer<LinearSmoothedValue<double>> linVolSmoother;
	ScopedPointer<LinearSmoothedValue<double>> linGainSmoother;

	//value for currentSampleRate
	double currentSampleRate;
	//parameter update coefficient
	double paramCoeff;
	
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool setPreferredBusArrangement (bool isInput, int bus, const AudioChannelSet& preferredSet) override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;


    //==============================================================================
    AudioProcessorEditor* createEditor() override;
	
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	//==============================================================================
	
	//Monitor the input to the FUZZFACE simulation, the value to be displayed on inputSignal Label
	double currentInput;

private:
	//Timer callback used for parameter updates
	void timerCallback() override;

	//Input scaling, including a clipper  used to limit the max input to signal and avoid crashes
	void inputScaling(float* _channelData);

	//Used to determine if the current sample is within the muted startup period 
	int sampleIndex;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FuzzFaceJuceAudioProcessor)


};


#endif  // PLUGINPROCESSOR_H_INCLUDED
