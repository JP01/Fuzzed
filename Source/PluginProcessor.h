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
class FuzzFaceJuceAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    FuzzFaceJuceAudioProcessor();
    ~FuzzFaceJuceAudioProcessor();

	//==============================================================================
	//Instance of the simulation class
	Simulation sim;

	//Value for fuzz
	double fuzzVal;
	//smoothed fuzz value
	double smoothedFuzz;

	//Value for Vol
	double volVal;
	//smoothed volume value
	double smoothedVol; 

	
	//value for currentSampleRate
	double currentSampleRate = DEFAULT_SR;

	//smoothing time
	double smoothingCoeff = 0.9;
	double smoothingCutoff = 40;

	//Eigen Vector storing current buffer
	Eigen::VectorXd eigenInputBuffer;
	Eigen::VectorXd eigenOutputBuffer;


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
	

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FuzzFaceJuceAudioProcessor)


};


#endif  // PLUGINPROCESSOR_H_INCLUDED
