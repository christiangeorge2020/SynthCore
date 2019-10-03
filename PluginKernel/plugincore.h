// -----------------------------------------------------------------------------
//    ASPiK Plugin Kernel File:  plugincore.h
//
/**
    \file   plugincore.h
    \author Will Pirkle
    \date   17-September-2018
    \brief  base class interface file for ASPiK plugincore object
    		- http://www.aspikplugins.com
    		- http://www.willpirkle.com
*/
// -----------------------------------------------------------------------------
#ifndef __pluginCore_h__
#define __pluginCore_h__

#include "pluginbase.h"
#include "synthcore.h"

// **--0x7F1F--**

// --- Plugin Variables controlID Enumeration 

enum controlID {
	lfo1Waveform = 40,
	lfo1Mode = 41,
	lfo1Frequency_Hz = 42,
	masterPitchBend = 9,
	masterTune = 19,
	masterVolume_dB = 29,
	lfo1DelayTime_mSec = 43,
	lfo1RampTime_mSec = 44,
	lfo2Mode = 51,
	lfo2Waveform = 50,
	lfo2Frequency_Hz = 52,
	lfo1Shape = 45,
	lfo1ShapeSplit = 46,
	scaleMode = 47,
	enableRhythmicBreaks = 48,
	lfo2Shape = 53,
	lfo2ShapeSplit = 54,
	lfo2ModTarget = 55
};

	// **--0x0F1F--**

/**
\class PluginCore
\ingroup ASPiK-Core
\brief
The PluginCore object is the default PluginBase derived object for ASPiK projects.
Note that you are fre to change the name of this object (as long as you change it in the compiler settings, etc...)


PluginCore Operations:
- overrides the main processing functions from the base class
- performs reset operation on sub-modules
- processes audio
- processes messages for custom views
- performs pre and post processing functions on parameters and audio (if needed)

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class PluginCore : public PluginBase
{
public:
    PluginCore();

	/** Destructor: empty in default version */
    virtual ~PluginCore(){}

	// --- PluginBase Overrides ---
	//
	/** this is the creation function for all plugin parameters */
	bool initPluginParameters();

	/** called when plugin is loaded, a new audio file is playing or sample rate changes */
	virtual bool reset(ResetInfo& resetInfo);

	/** one-time post creation init function; pluginInfo contains path to this plugin */
	virtual bool initialize(PluginInfo& _pluginInfo);

	// --- preProcess: sync GUI parameters here; override if you don't want to use automatic variable-binding
	virtual bool preProcessAudioBuffers(ProcessBufferInfo& processInfo);

	/** process frames of data */
	virtual bool processAudioFrame(ProcessFrameInfo& processFrameInfo);

	// --- uncomment and override this for buffer processing; see base class implementation for
	//     help on breaking up buffers and getting info from processBufferInfo
	//virtual bool processAudioBuffers(ProcessBufferInfo& processBufferInfo);

	/** preProcess: do any post-buffer processing required; default operation is to send metering data to GUI  */
	virtual bool postProcessAudioBuffers(ProcessBufferInfo& processInfo);

	/** called by host plugin at top of buffer proccess; this alters parameters prior to variable binding operation  */
	virtual bool updatePluginParameter(int32_t controlID, double controlValue, ParameterUpdateInfo& paramInfo);

	/** called by host plugin at top of buffer proccess; this alters parameters prior to variable binding operation  */
	virtual bool updatePluginParameterNormalized(int32_t controlID, double normalizedValue, ParameterUpdateInfo& paramInfo);

	/** this can be called: 1) after bound variable has been updated or 2) after smoothing occurs  */
	virtual bool postUpdatePluginParameter(int32_t controlID, double controlValue, ParameterUpdateInfo& paramInfo);

	/** this is ony called when the user makes a GUI control change */
	virtual bool guiParameterChanged(int32_t controlID, double actualValue);

	/** processMessage: messaging system; currently used for custom/special GUI operations */
	virtual bool processMessage(MessageInfo& messageInfo);

	/** processMIDIEvent: MIDI event processing */
	virtual bool processMIDIEvent(midiEvent& event);

	/** specialized joystick servicing (currently not used) */
	virtual bool setVectorJoystickParameters(const VectorJoystickData& vectorJoysickData);

	/** create the presets */
	bool initPluginPresets();

	// --- BEGIN USER VARIABLES AND FUNCTIONS -------------------------------------- //
	//	   Add your variables and methods here

	/*
	Projects 1
	 1) 2 LFOs
	 2) Cross modulation
		i) fo modulation
		ii) shape modulation
	 3) Shape Control; phase distortion
	 4) QRSH
		i) intervals; generates musical pitches quantized to the nearest pitch within the major scale; note the user plays is the root of the scale
		ii) timing; generates musical timing (breaks) according to the tempo; 

	Rewrite the voice render so the lfo output is on the oscilloscope

	*/
	SynthEngine synthEngine; ///< 
	void updateParameters();


	// --- END USER VARIABLES AND FUNCTIONS -------------------------------------- //

private:
	//  **--0x07FD--**

	// --- Continuous Plugin Variables 
	double lfo1Frequency_Hz = 0.0;
	double masterPitchBend = 0.0;
	double masterTune = 0.0;
	double masterVolume_dB = 0.0;
	double lfo1DelayTime_mSec = 0.0;
	double lfo1RampTime_mSec = 0.0;
	double lfo2Frequency_Hz = 0.0;
	double lfo1Shape = 0.0;
	double lfo1ShapeSplit = 0.0;
	double lfo2Shape = 0.0;
	double lfo2ShapeSplit = 0.0;

	// --- Discrete Plugin Variables 
	int lfo1Waveform = 0;
	enum class lfo1WaveformEnum { Triangle,Sin,Saw,RSH,QRSH,Noise,QRNoise };	// to compare: if(compareEnumToInt(lfo1WaveformEnum::Triangle, lfo1Waveform)) etc... 

	int lfo1Mode = 0;
	enum class lfo1ModeEnum { Sync,One_Shot,Free_Run };	// to compare: if(compareEnumToInt(lfo1ModeEnum::Sync, lfo1Mode)) etc... 

	int lfo2Mode = 0;
	enum class lfo2ModeEnum { Sync,One_Shot,Free_Run };	// to compare: if(compareEnumToInt(lfo2ModeEnum::Sync, lfo2Mode)) etc... 

	int lfo2Waveform = 0;
	enum class lfo2WaveformEnum { Triangle,Sin,Saw,RSH,QRSH,Noise,QRNoise };	// to compare: if(compareEnumToInt(lfo2WaveformEnum::Triangle, lfo2Waveform)) etc... 

	int scaleMode = 0;
	enum class scaleModeEnum { None,Ionian,Dorian,Phrygian,Lydian,Mixolydian,Aeolian,Locrian,Chromatic };	// to compare: if(compareEnumToInt(scaleModeEnum::None, scaleMode)) etc... 

	int enableRhythmicBreaks = 0;
	enum class enableRhythmicBreaksEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(enableRhythmicBreaksEnum::SWITCH_OFF, enableRhythmicBreaks)) etc... 

	int lfo2ModTarget = 0;
	enum class lfo2ModTargetEnum { None,LFO1_Fo,LFO1_Shape,Both };	// to compare: if(compareEnumToInt(lfo2ModTargetEnum::None, lfo2ModTarget)) etc... 

	// **--0x1A7F--**
    // --- end member variables

public:
    /** static description: bundle folder name

	\return bundle folder name as a const char*
	*/
    static const char* getPluginBundleName();

    /** static description: name

	\return name as a const char*
	*/
    static const char* getPluginName();

	/** static description: short name

	\return short name as a const char*
	*/
	static const char* getShortPluginName();

	/** static description: vendor name

	\return vendor name as a const char*
	*/
	static const char* getVendorName();

	/** static description: URL

	\return URL as a const char*
	*/
	static const char* getVendorURL();

	/** static description: email

	\return email address as a const char*
	*/
	static const char* getVendorEmail();

	/** static description: Cocoa View Factory Name

	\return Cocoa View Factory Name as a const char*
	*/
	static const char* getAUCocoaViewFactoryName();

	/** static description: plugin type

	\return type (FX or Synth)
	*/
	static pluginType getPluginType();

	/** static description: VST3 GUID

	\return VST3 GUID as a const char*
	*/
	static const char* getVSTFUID();

	/** static description: 4-char code

	\return 4-char code as int
	*/
	static int32_t getFourCharCode();

	/** initalizer */
	bool initPluginDescriptors();

};




#endif /* defined(__pluginCore_h__) */
