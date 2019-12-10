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
	lfo2ModTarget = 55,
	osc1Waveform = 0,
	osc1BankIndex = 1,
	osc2Waveform = 10,
	osc3Waveform = 20,
	osc4Waveform = 30,
	osc2BankIndex = 11,
	osc3BankIndex = 21,
	osc4BankIndex = 31,
	osc1Detune_cents = 3,
	fc1_hertz = 5,
	q1Control = 6,
	lfo1Amplitude = 170,
	osc1FoModIn = 161,
	enableKeyTrack = 7,
	keyTrackRatio = 8,
	lfo1_to_osc1Fo = 171,
	eg2DelayTime_mSec = 71,
	eg2AttackTime_mSec = 72,
	eg2HoldTime_mSec = 73,
	eg2DecayTime_mSec = 74,
	eg2SustainLevel = 75,
	eg2ReleaseTime_mSec = 76,
	eg2AutoTrigger = 77,
	eg2ManualTrigger = 78,
	eg1DelayTime_mSec = 61,
	eg1AttackTime_mSec = 62,
	eg1HoldTime_mSec = 63,
	eg1DecayTime_mSec = 64,
	eg1SustainLevel = 65,
	eg1ReleaseTime_mSec = 66,
	eg1AutoRetrigger = 67,
	eg1ManualTrigger = 68,
	eg1Offset = 69,
	eg2Offset = 79,
	eg1Mode = 60,
	eg2Mode = 70,
	mode = 2,
	unisonDetune_Cents = 4,
	eg1ModOut = 190,
	eg1_to_osc1Fo = 191,
	lfo2Amplitude = 180,
	eg2ModOut = 200,
	joystickACModOut = 210,
	joystickBDModOut = 220,
	lfo1_to_osc2Fo = 173,
	osc2FoModIn = 163,
	osc1ShapeModIn = 162,
	osc2ShapeModIn = 164,
	filter1FoModIn = 165,
	filter1QModIn = 166,
	filter2FoModIn = 167,
	filter2QModIn = 168,
	lfo1_to_osc1Shape = 172,
	lfo1_to_osc2Shape = 174,
	lfo1_to_filter1Fo = 175,
	lfo1_to_filter1Q = 176,
	lfo1_to_filter2Fo = 177,
	lfo1_to_filter2Q = 178,
	lfo2_to_osc1Fo = 181,
	lfo2_to_osc1Shape = 182,
	lfo2_to_osc2Fo = 183,
	lfo2_to_osc2Shape = 184,
	lfo2_to_filter1Fo = 185,
	lfo2_to_filter1Q = 186,
	lfo2_to_filter2Fo = 187,
	lfo2_to_filter2Q = 188,
	eg1_to_osc1Shape = 192,
	eg1_to_osc2Fo = 193,
	eg1_to_osc2Shape = 194,
	eg1_to_filter1Fo = 195,
	eg1_to_filter1Q = 196,
	eg1_to_filter2Fo = 197,
	eg1_to_filter2Q = 198,
	eg2_to_osc1Fo = 201,
	eg2_to_osc1Shape = 202,
	eg2_to_osc2Fo = 203,
	eg2_to_osc2Shape = 204,
	eg2_to_filter1Fo = 205,
	eg2_to_filter1Q = 206,
	eg2_to_filter2Fo = 207,
	eg2_to_filter2Q = 208,
	joystickAC_to_osc1Fo = 211,
	joystickAC_to_osc1Shape = 212,
	joystickAC_to_osc2Fo = 213,
	joystickAC_to_osc2Shape = 214,
	joystickAC_to_filter1Fo = 215,
	joystickAC_to_filter1Q = 216,
	joystickAC_to_filter2Fo = 217,
	joystickAC_to_filter2Q = 218,
	joystickBD_to_osc1Fo = 221,
	joystickBD_to_osc1Shape = 222,
	joystickBD_to_osc2Fo = 223,
	joystickBD_to_osc2Shape = 224,
	joystickBD_to_filter1Fo = 225,
	joystickBD_to_filter1Q = 226,
	joystickBD_to_filter2Fo = 227,
	joystickBD_to_filter2Q = 228,
	auxUnipolar1ModOut = 230,
	auxUnipolar2ModOut = 240,
	auxUnipolar3ModOut = 250,
	auxUnipolar4ModOut = 260,
	auxUnipolar5ModOut = 270,
	auxUnipolar6ModOut = 280,
	auxBipolar1ModOut = 290,
	auxBipolar2ModOut = 300,
	auxBipolar3ModOut = 310,
	auxBipolar4ModOut = 320,
	DCASampleHoldModIn = 169,
	lfo1_to_DCASampleHoldModIn = 179,
	lfo2_to_DCASampleHoldModIn = 189,
	joystickAC_to_DCASampleHoldModIn = 219,
	joystickBD_to_DCASampleHoldModIn = 229,
	auxUnipolar1Mod_to_osc1Fo = 231,
	auxUnipolar1Mod_to_osc1Shape = 232,
	auxUnipolar1Mod_to_osc2Fo = 233,
	auxUnipolar1Mod_to_osc2Shape = 234,
	auxUnipolar1Mod_to_filter1Fo = 235,
	auxUnipolar1Mod_to_filter1Q = 236,
	auxUnipolar1Mod_to_filter2Fo = 237,
	auxUnipolar1Mod_to_filter2Q = 238,
	auxUnipolar1Mod_to_DCASampleHoldModIn = 239,
	auxUnipolar2Mod_to_osc1Fo = 241,
	auxUnipolar2Mod_to_osc1Shape = 242,
	auxUnipolar2Mod_to_osc2Fo = 243,
	auxUnipolar2Mod_to_osc2Shape = 244,
	auxUnipolar2Mod_to_filter1Fo = 245,
	auxUnipolar2Mod_to_filter1Q = 246,
	auxUnipolar2Mod_to_filter2Fo = 247,
	auxUnipolar2Mod_to_filter2Q = 248,
	auxUnipolar2Mod_to_DCASampleHoldModIn = 249,
	auxUnipolar3Mod_to_osc1Fo = 251,
	auxUnipolar3Mod_to_osc1Shape = 252,
	auxUnipolar3Mod_to_osc2Fo = 253,
	auxUnipolar3Mod_to_osc2Shape = 254,
	auxUnipolar3Mod_to_filter1Fo = 255,
	auxUnipolar3Mod_to_filter1Q = 256,
	auxUnipolar3Mod_to_filter2Fo = 257,
	auxUnipolar3Mod_to_filter2Q = 258,
	auxUnipolar3Mod_to_DCASampleHoldModIn = 259,
	auxUnipolar4Mod_to_osc1Fo = 261,
	auxUnipolar4Mod_to_osc1Shape = 262,
	auxUnipolar4Mod_to_osc2Fo = 263,
	auxUnipolar4Mod_to_osc2Shape = 264,
	auxUnipolar4Mod_to_filter1Fo = 265,
	auxUnipolar4Mod_to_filter1Q = 266,
	auxUnipolar4Mod_to_filter2Fo = 267,
	auxUnipolar4Mod_to_filter2Q = 268,
	auxUnipolar4Mod_to_DCASampleHoldModIn = 269,
	auxUnipolar5Mod_to_osc1Fo = 271,
	auxUnipolar5Mod_to_osc1Shape = 272,
	auxUnipolar5Mod_to_osc2Fo = 273,
	auxUnipolar5Mod_to_osc2Shape = 274,
	auxUnipolar5Mod_to_filter1Fo = 275,
	auxUnipolar5Mod_to_filter1Q = 276,
	auxUnipolar5Mod_to_filter2Fo = 277,
	auxUnipolar5Mod_to_filter2Q = 278,
	auxUnipolar5Mod_to_DCASampleHoldModIn = 279,
	auxUnipolar6Mod_to_osc1Fo = 281,
	auxUnipolar6Mod_to_osc1Shape = 282,
	auxUnipolar6Mod_to_osc2Fo = 283,
	auxUnipolar6Mod_to_osc2Shape = 284,
	auxUnipolar6Mod_to_filter1Fo = 285,
	auxUnipolar6Mod_to_filter1Q = 286,
	auxUnipolar6Mod_to_filter2Fo = 287,
	auxUnipolar6Mod_to_filter2Q = 288,
	auxUnipolar6Mod_to_DCASampleHoldModIn = 289,
	auxBipolar1Mod_to_osc1Fo = 291,
	auxBipolar1Mod_to_osc1Shape = 292,
	auxBipolar1Mod_to_osc2Fo = 293,
	auxBipolar1Mod_to_osc2Shape = 294,
	auxBipolar1Mod_to_filter1Fo = 295,
	auxBipolar1Mod_to_filter1Q = 296,
	auxBipolar1Mod_to_filter2Fo = 297,
	auxBipolar1Mod_to_filter2Q = 298,
	auxBipolar1Mod_to_DCASampleHoldModIn = 299,
	auxBipolar2Mod_to_osc1Fo = 301,
	auxBipolar2Mod_to_osc1Shape = 302,
	auxBipolar2Mod_to_osc2Fo = 303,
	auxBipolar2Mod_to_osc2Shape = 304,
	auxBipolar2Mod_to_filter1Fo = 305,
	auxBipolar2Mod_to_filter1Q = 306,
	auxBipolar2Mod_to_filter2Fo = 307,
	auxBipolar2Mod_to_filter2Q = 308,
	auxBipolar2Mod_to_DCASampleHoldModIn = 309,
	auxBipolar3Mod_to_osc1Fo = 311,
	auxBipolar3Mod_to_osc1Shape = 312,
	auxBipolar3Mod_to_osc2Fo = 313,
	auxBipolar3Mod_to_osc2Shape = 314,
	auxBipolar3Mod_to_filter1Fo = 315,
	auxBipolar3Mod_to_filter1Q = 316,
	auxBipolar3Mod_to_filter2Fo = 317,
	auxBipolar3Mod_to_filter2Q = 318,
	auxBipolar3Mod_to_DCASampleHoldModIn = 319,
	auxBipolar4Mod_to_osc1Fo = 321,
	auxBipolar4Mod_to_osc1Shape = 322,
	auxBipolar4Mod_to_osc2Fo = 323,
	auxBipolar4Mod_to_osc2Shape = 324,
	auxBipolar4Mod_to_filter1Fo = 325,
	auxBipolar4Mod_to_filter1Q = 326,
	auxBipolar4Mod_to_filter2Fo = 327,
	auxBipolar4Mod_to_filter2Q = 328,
	auxBipolar4Mod_to_DCASampleHoldModIn = 329,
	eg1_to_DCASampleHoldModIn = 199,
	eg2_to_DCASampleHoldModIn = 209,
	lfo1ShapeModIn = 341,
	lfo1ShapeSplitModIn = 342,
	lfo2ShapeModIn = 343,
	lfo2ShapeSplitModIn = 344,
	eg1AttackModIn = 345,
	eg1HoldModIn = 346,
	eg1DecayModIn = 347,
	eg1SustainModIn = 348,
	eg1ReleaseModIn = 349,
	unisonDetuneModIn = 340,
	lfo1_to_lfo1Shape = 351,
	lfo2_to_lfo1Shape = 361,
	eg1_to_lfo1Shape = 371,
	eg2_to_lfo1Shape = 381,
	joystickAC_to_lfo1Shape = 391,
	joystickBD_to_lfo1Shape = 401,
	auxUnipolar1Mod_to_lfo1Shape = 411,
	auxUnipolar2Mod_to_lfo1Shape = 421,
	auxUnipolar3Mod_to_lfo1Shape = 431,
	auxUnipolar4Mod_to_lfo1Shape = 441,
	auxUnipolar5Mod_to_lfo1Shape = 451,
	auxUnipolar6Mod_to_lfo1Shape = 461,
	auxBipolar1Mod_to_lfo1Shape = 471,
	auxBipolar2Mod_to_lfo1Shape = 481,
	auxBipolar3Mod_to_lfo1Shape = 491,
	auxBipolar4Mod_to_lfo1Shape = 501,
	lfo1_to_unisonDetune = 350,
	lfo2_to_unisonDetune = 360,
	eg1_to_unisonDetune = 370,
	eg2_to_unisonDetune = 380,
	joystickAC_to_unisonDetune = 390,
	joystickBD_to_unisonDetune = 400,
	lfo1_to_lfo1ShapeSplit = 352,
	lfo2_to_lfo1ShapeSplit = 362,
	eg1_to_lfo1ShapeSplit = 372,
	eg2_to_lfo1ShapeSplit = 382,
	joystickAC_to_lfo1ShapeSplit = 392,
	joystickBD_to_lfo1ShapeSplit = 402,
	lfo1_to_lfo2Shape = 353,
	lfo2_to_lfo2Shape = 363,
	eg1_to_lfo2Shape = 373,
	eg2_to_lfo2Shape = 383,
	joystickAC_to_lfo2Shape = 393,
	joystickBD_to_lfo2Shape = 403,
	lfo1_to_lfo2ShapeSplit = 354,
	lfo2_to_lfo2ShapeSplit = 364,
	eg1_to_lfo2ShapeSplit = 374,
	eg2_to_lfo2ShapeSplit = 384,
	joystickAC_to_lfo2ShapeSplit = 394,
	joystickBD_to_lfo2ShapeSplit = 404,
	lfo1_to_eg1Attack = 355,
	lfo2_to_eg1Attack = 365,
	joystickAC_to_eg1Attack = 395,
	joystickBD_to_eg1Attack = 405,
	eg2_to_eg1Attack = 385,
	lfo1_to_eg1Hold = 356,
	lfo2_to_eg1Hold = 366,
	eg2_to_eg1Hold = 386,
	joystickAC_to_eg1Hold = 396,
	joystickBD_to_eg1Hold = 406,
	lfo1_to_eg1Decay = 357,
	lfo2_to_eg1Decay = 367,
	eg2_to_eg1Decay = 387,
	joystickAC_to_eg1Decay = 397,
	joystickBD_to_eg1Decay = 407,
	lfo1_to_eg1Sustain = 358,
	lfo2_to_eg1Sustain = 368,
	eg2_to_eg1Sustain = 388,
	joystickAC_to_eg1Sustain = 398,
	joystickBD_to_eg1Sustain = 408,
	lfo1_to_eg1Release = 359,
	lfo2_to_eg1Release = 369,
	eg2_to_eg1Release = 389,
	joystickAC_to_eg1Release = 399,
	joystickBD_to_eg1Release = 409,
	temperamentKeySelection = 39,
	osc1ExciterMode = 80,
	exciter1Attack_msec = 81,
	exciter1Hold_msec = 82,
	exciter1Release_msec = 83,
	resonator1Decay = 84
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
	double bpm;

	ICustomView* bankAndWaveGroup_0 = nullptr;

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
	double osc1Detune_cents = 0.0;
	double fc1_hertz = 0.0;
	double q1Control = 0.0;
	double lfo1Amplitude = 0.0;
	double osc1FoModIn = 0.0;
	double keyTrackRatio = 0.0;
	double eg2DelayTime_mSec = 0.0;
	double eg2AttackTime_mSec = 0.0;
	double eg2HoldTime_mSec = 0.0;
	double eg2DecayTime_mSec = 0.0;
	double eg2SustainLevel = 0.0;
	double eg2ReleaseTime_mSec = 0.0;
	double eg1DelayTime_mSec = 0.0;
	double eg1AttackTime_mSec = 0.0;
	double eg1HoldTime_mSec = 0.0;
	double eg1DecayTime_mSec = 0.0;
	double eg1SustainLevel = 0.0;
	double eg1ReleaseTime_mSec = 0.0;
	double eg1Offset = 0.0;
	double eg2Offset = 0.0;
	double unisonDetune_Cents = 0.0;
	double eg1ModOut = 0.0;
	double lfo2Amplitude = 0.0;
	double eg2ModOut = 0.0;
	double joystickACModOut = 0.0;
	double joystickBDModOut = 0.0;
	double osc2FoModIn = 0.0;
	double osc1ShapeModIn = 0.0;
	double osc2ShapeModIn = 0.0;
	double filter1FoModIn = 0.0;
	double filter1QModIn = 0.0;
	double filter2FoModIn = 0.0;
	double filter2QModIn = 0.0;
	double auxUnipolar1ModOut = 0.0;
	double auxUnipolar2ModOut = 0.0;
	double auxUnipolar3ModOut = 0.0;
	double auxUnipolar4ModOut = 0.0;
	double auxUnipolar5ModOut = 0.0;
	double auxUnipolar6ModOut = 0.0;
	double auxBipolar1ModOut = 0.0;
	double auxBipolar2ModOut = 0.0;
	double auxBipolar3ModOut = 0.0;
	double auxBipolar4ModOut = 0.0;
	double DCASampleHoldModIn = 0.0;
	double lfo1ShapeModIn = 0.0;
	double lfo1ShapeSplitModIn = 0.0;
	double lfo2ShapeModIn = 0.0;
	double lfo2ShapeSplitModIn = 0.0;
	double eg1AttackModIn = 0.0;
	double eg1HoldModIn = 0.0;
	double eg1DecayModIn = 0.0;
	double eg1SustainModIn = 0.0;
	double eg1ReleaseModIn = 0.0;
	double unisonDetuneModIn = 0.0;
	int temperamentKeySelection = 0;
	double exciter1Attack_msec = 0.0;
	double exciter1Hold_msec = 0.0;
	double exciter1Release_msec = 0.0;
	double resonator1Decay = 0.0;

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
	enum class lfo2ModTargetEnum { None,LFO1_Fo,LFO1_Shape,Both,Rhythmic_Breaks };	// to compare: if(compareEnumToInt(lfo2ModTargetEnum::None, lfo2ModTarget)) etc... 

	int osc1Waveform = 0;
	enum class osc1WaveformEnum { wave0,wave1,wave2,wave3,wave4,wave5,wave6,wave7,wave8,wave9,wave10,wave11,wave12,wave13,wave14,wave15,wave16,wave17,wave18,wave19,wave20,wave21,wave22,wave23,wave24,wave25,wave26,wave27,wave28,wave29,wave30,wave31 };	// to compare: if(compareEnumToInt(osc1WaveformEnum::wave0, osc1Waveform)) etc... 

	int osc1BankIndex = 0;
	enum class osc1BankIndexEnum { Bank_0,Bank_1,Bank_2,Bank_3 };	// to compare: if(compareEnumToInt(osc1BankIndexEnum::Bank_0, osc1BankIndex)) etc... 

	int osc2Waveform = 0;
	enum class osc2WaveformEnum { wave0,wave1,wave2,wave3,wave4,wave5,wave6,wave7,wave8,wave9,wave10,wave11,wave12,wave13,wave14,wave15,wave16,wave17,wave18,wave19,wave20,wave21,wave22,wave23,wave24,wave25,wave26,wave27,wave28,wave29,wave30,wave31 };	// to compare: if(compareEnumToInt(osc2WaveformEnum::wave0, osc2Waveform)) etc... 

	int osc3Waveform = 0;
	enum class osc3WaveformEnum { wave0,wave1,wave2,wave3,wave4,wave5,wave6,wave7,wave8,wave9,wave10,wave11,wave12,wave13,wave14,wave15,wave16,wave17,wave18,wave19,wave20,wave21,wave22,wave23,wave24,wave25,wave26,wave27,wave28,wave29,wave30,wave31 };	// to compare: if(compareEnumToInt(osc3WaveformEnum::wave0, osc3Waveform)) etc... 

	int osc4Waveform = 0;
	enum class osc4WaveformEnum { wave0,wave1,wave2,wave3,wave4,wave5,wave6,wave7,wave8,wave9,wave10,wave11,wave12,wave13,wave14,wave15,wave16,wave17,wave18,wave19,wave20,wave21,wave22,wave23,wave24,wave25,wave26,wave27,wave28,wave29,wave30,wave31 };	// to compare: if(compareEnumToInt(osc4WaveformEnum::wave0, osc4Waveform)) etc... 

	int osc2BankIndex = 0;
	enum class osc2BankIndexEnum { Bank_0,Bank_1,Bank_2,Bank_3 };	// to compare: if(compareEnumToInt(osc2BankIndexEnum::Bank_0, osc2BankIndex)) etc... 

	int osc3BankIndex = 0;
	enum class osc3BankIndexEnum { Bank_0,Bank_1,Bank_2,Bank_3 };	// to compare: if(compareEnumToInt(osc3BankIndexEnum::Bank_0, osc3BankIndex)) etc... 

	int osc4BankIndex = 0;
	enum class osc4BankIndexEnum { Bank_0,Bank_1,Bank_2,Bank_3 };	// to compare: if(compareEnumToInt(osc4BankIndexEnum::Bank_0, osc4BankIndex)) etc... 

	int enableKeyTrack = 0;
	enum class enableKeyTrackEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(enableKeyTrackEnum::SWITCH_OFF, enableKeyTrack)) etc... 

	int lfo1_to_osc1Fo = 0;
	enum class lfo1_to_osc1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo1_to_osc1FoEnum::SWITCH_OFF, lfo1_to_osc1Fo)) etc... 

	int eg2AutoTrigger = 0;
	enum class eg2AutoTriggerEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg2AutoTriggerEnum::SWITCH_OFF, eg2AutoTrigger)) etc... 

	int eg2ManualTrigger = 0;
	enum class eg2ManualTriggerEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg2ManualTriggerEnum::SWITCH_OFF, eg2ManualTrigger)) etc... 

	int eg1AutoRetrigger = 0;
	enum class eg1AutoRetriggerEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg1AutoRetriggerEnum::SWITCH_OFF, eg1AutoRetrigger)) etc... 

	int eg1ManualTrigger = 0;
	enum class eg1ManualTriggerEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg1ManualTriggerEnum::SWITCH_OFF, eg1ManualTrigger)) etc... 

	int eg1Mode = 0;
	enum class eg1ModeEnum { ADSR,AHDSR,AHR };	// to compare: if(compareEnumToInt(eg1ModeEnum::ADSR, eg1Mode)) etc... 

	int eg2Mode = 0;
	enum class eg2ModeEnum { ADSR,AHDSR,AHR };	// to compare: if(compareEnumToInt(eg2ModeEnum::ADSR, eg2Mode)) etc... 

	int mode = 0;
	enum class modeEnum { Poly,Mono,Unison };	// to compare: if(compareEnumToInt(modeEnum::Poly, mode)) etc... 

	int eg1_to_osc1Fo = 0;
	enum class eg1_to_osc1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg1_to_osc1FoEnum::SWITCH_OFF, eg1_to_osc1Fo)) etc... 

	int lfo1_to_osc2Fo = 0;
	enum class lfo1_to_osc2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo1_to_osc2FoEnum::SWITCH_OFF, lfo1_to_osc2Fo)) etc... 

	int lfo1_to_osc1Shape = 0;
	enum class lfo1_to_osc1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo1_to_osc1ShapeEnum::SWITCH_OFF, lfo1_to_osc1Shape)) etc... 

	int lfo1_to_osc2Shape = 0;
	enum class lfo1_to_osc2ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo1_to_osc2ShapeEnum::SWITCH_OFF, lfo1_to_osc2Shape)) etc... 

	int lfo1_to_filter1Fo = 0;
	enum class lfo1_to_filter1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo1_to_filter1FoEnum::SWITCH_OFF, lfo1_to_filter1Fo)) etc... 

	int lfo1_to_filter1Q = 0;
	enum class lfo1_to_filter1QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo1_to_filter1QEnum::SWITCH_OFF, lfo1_to_filter1Q)) etc... 

	int lfo1_to_filter2Fo = 0;
	enum class lfo1_to_filter2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo1_to_filter2FoEnum::SWITCH_OFF, lfo1_to_filter2Fo)) etc... 

	int lfo1_to_filter2Q = 0;
	enum class lfo1_to_filter2QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo1_to_filter2QEnum::SWITCH_OFF, lfo1_to_filter2Q)) etc... 

	int lfo2_to_osc1Fo = 0;
	enum class lfo2_to_osc1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo2_to_osc1FoEnum::SWITCH_OFF, lfo2_to_osc1Fo)) etc... 

	int lfo2_to_osc1Shape = 0;
	enum class lfo2_to_osc1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo2_to_osc1ShapeEnum::SWITCH_OFF, lfo2_to_osc1Shape)) etc... 

	int lfo2_to_osc2Fo = 0;
	enum class lfo2_to_osc2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo2_to_osc2FoEnum::SWITCH_OFF, lfo2_to_osc2Fo)) etc... 

	int lfo2_to_osc2Shape = 0;
	enum class lfo2_to_osc2ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo2_to_osc2ShapeEnum::SWITCH_OFF, lfo2_to_osc2Shape)) etc... 

	int lfo2_to_filter1Fo = 0;
	enum class lfo2_to_filter1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo2_to_filter1FoEnum::SWITCH_OFF, lfo2_to_filter1Fo)) etc... 

	int lfo2_to_filter1Q = 0;
	enum class lfo2_to_filter1QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo2_to_filter1QEnum::SWITCH_OFF, lfo2_to_filter1Q)) etc... 

	int lfo2_to_filter2Fo = 0;
	enum class lfo2_to_filter2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo2_to_filter2FoEnum::SWITCH_OFF, lfo2_to_filter2Fo)) etc... 

	int lfo2_to_filter2Q = 0;
	enum class lfo2_to_filter2QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo2_to_filter2QEnum::SWITCH_OFF, lfo2_to_filter2Q)) etc... 

	int eg1_to_osc1Shape = 0;
	enum class eg1_to_osc1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg1_to_osc1ShapeEnum::SWITCH_OFF, eg1_to_osc1Shape)) etc... 

	int eg1_to_osc2Fo = 0;
	enum class eg1_to_osc2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg1_to_osc2FoEnum::SWITCH_OFF, eg1_to_osc2Fo)) etc... 

	int eg1_to_osc2Shape = 0;
	enum class eg1_to_osc2ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg1_to_osc2ShapeEnum::SWITCH_OFF, eg1_to_osc2Shape)) etc... 

	int eg1_to_filter1Fo = 0;
	enum class eg1_to_filter1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg1_to_filter1FoEnum::SWITCH_OFF, eg1_to_filter1Fo)) etc... 

	int eg1_to_filter1Q = 0;
	enum class eg1_to_filter1QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg1_to_filter1QEnum::SWITCH_OFF, eg1_to_filter1Q)) etc... 

	int eg1_to_filter2Fo = 0;
	enum class eg1_to_filter2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg1_to_filter2FoEnum::SWITCH_OFF, eg1_to_filter2Fo)) etc... 

	int eg1_to_filter2Q = 0;
	enum class eg1_to_filter2QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg1_to_filter2QEnum::SWITCH_OFF, eg1_to_filter2Q)) etc... 

	int eg2_to_osc1Fo = 0;
	enum class eg2_to_osc1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg2_to_osc1FoEnum::SWITCH_OFF, eg2_to_osc1Fo)) etc... 

	int eg2_to_osc1Shape = 0;
	enum class eg2_to_osc1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg2_to_osc1ShapeEnum::SWITCH_OFF, eg2_to_osc1Shape)) etc... 

	int eg2_to_osc2Fo = 0;
	enum class eg2_to_osc2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg2_to_osc2FoEnum::SWITCH_OFF, eg2_to_osc2Fo)) etc... 

	int eg2_to_osc2Shape = 0;
	enum class eg2_to_osc2ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg2_to_osc2ShapeEnum::SWITCH_OFF, eg2_to_osc2Shape)) etc... 

	int eg2_to_filter1Fo = 0;
	enum class eg2_to_filter1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg2_to_filter1FoEnum::SWITCH_OFF, eg2_to_filter1Fo)) etc... 

	int eg2_to_filter1Q = 0;
	enum class eg2_to_filter1QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg2_to_filter1QEnum::SWITCH_OFF, eg2_to_filter1Q)) etc... 

	int eg2_to_filter2Fo = 0;
	enum class eg2_to_filter2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg2_to_filter2FoEnum::SWITCH_OFF, eg2_to_filter2Fo)) etc... 

	int eg2_to_filter2Q = 0;
	enum class eg2_to_filter2QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg2_to_filter2QEnum::SWITCH_OFF, eg2_to_filter2Q)) etc... 

	int joystickAC_to_osc1Fo = 0;
	enum class joystickAC_to_osc1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickAC_to_osc1FoEnum::SWITCH_OFF, joystickAC_to_osc1Fo)) etc... 

	int joystickAC_to_osc1Shape = 0;
	enum class joystickAC_to_osc1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickAC_to_osc1ShapeEnum::SWITCH_OFF, joystickAC_to_osc1Shape)) etc... 

	int joystickAC_to_osc2Fo = 0;
	enum class joystickAC_to_osc2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickAC_to_osc2FoEnum::SWITCH_OFF, joystickAC_to_osc2Fo)) etc... 

	int joystickAC_to_osc2Shape = 0;
	enum class joystickAC_to_osc2ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickAC_to_osc2ShapeEnum::SWITCH_OFF, joystickAC_to_osc2Shape)) etc... 

	int joystickAC_to_filter1Fo = 0;
	enum class joystickAC_to_filter1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickAC_to_filter1FoEnum::SWITCH_OFF, joystickAC_to_filter1Fo)) etc... 

	int joystickAC_to_filter1Q = 0;
	enum class joystickAC_to_filter1QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickAC_to_filter1QEnum::SWITCH_OFF, joystickAC_to_filter1Q)) etc... 

	int joystickAC_to_filter2Fo = 0;
	enum class joystickAC_to_filter2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickAC_to_filter2FoEnum::SWITCH_OFF, joystickAC_to_filter2Fo)) etc... 

	int joystickAC_to_filter2Q = 0;
	enum class joystickAC_to_filter2QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickAC_to_filter2QEnum::SWITCH_OFF, joystickAC_to_filter2Q)) etc... 

	int joystickBD_to_osc1Fo = 0;
	enum class joystickBD_to_osc1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickBD_to_osc1FoEnum::SWITCH_OFF, joystickBD_to_osc1Fo)) etc... 

	int joystickBD_to_osc1Shape = 0;
	enum class joystickBD_to_osc1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickBD_to_osc1ShapeEnum::SWITCH_OFF, joystickBD_to_osc1Shape)) etc... 

	int joystickBD_to_osc2Fo = 0;
	enum class joystickBD_to_osc2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickBD_to_osc2FoEnum::SWITCH_OFF, joystickBD_to_osc2Fo)) etc... 

	int joystickBD_to_osc2Shape = 0;
	enum class joystickBD_to_osc2ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickBD_to_osc2ShapeEnum::SWITCH_OFF, joystickBD_to_osc2Shape)) etc... 

	int joystickBD_to_filter1Fo = 0;
	enum class joystickBD_to_filter1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickBD_to_filter1FoEnum::SWITCH_OFF, joystickBD_to_filter1Fo)) etc... 

	int joystickBD_to_filter1Q = 0;
	enum class joystickBD_to_filter1QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickBD_to_filter1QEnum::SWITCH_OFF, joystickBD_to_filter1Q)) etc... 

	int joystickBD_to_filter2Fo = 0;
	enum class joystickBD_to_filter2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickBD_to_filter2FoEnum::SWITCH_OFF, joystickBD_to_filter2Fo)) etc... 

	int joystickBD_to_filter2Q = 0;
	enum class joystickBD_to_filter2QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickBD_to_filter2QEnum::SWITCH_OFF, joystickBD_to_filter2Q)) etc... 

	int lfo1_to_DCASampleHoldModIn = 0;
	enum class lfo1_to_DCASampleHoldModInEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo1_to_DCASampleHoldModInEnum::SWITCH_OFF, lfo1_to_DCASampleHoldModIn)) etc... 

	int lfo2_to_DCASampleHoldModIn = 0;
	enum class lfo2_to_DCASampleHoldModInEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo2_to_DCASampleHoldModInEnum::SWITCH_OFF, lfo2_to_DCASampleHoldModIn)) etc... 

	int joystickAC_to_DCASampleHoldModIn = 0;
	enum class joystickAC_to_DCASampleHoldModInEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickAC_to_DCASampleHoldModInEnum::SWITCH_OFF, joystickAC_to_DCASampleHoldModIn)) etc... 

	int joystickBD_to_DCASampleHoldModIn = 0;
	enum class joystickBD_to_DCASampleHoldModInEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickBD_to_DCASampleHoldModInEnum::SWITCH_OFF, joystickBD_to_DCASampleHoldModIn)) etc... 

	int auxUnipolar1Mod_to_osc1Fo = 0;
	enum class auxUnipolar1Mod_to_osc1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar1Mod_to_osc1FoEnum::SWITCH_OFF, auxUnipolar1Mod_to_osc1Fo)) etc... 

	int auxUnipolar1Mod_to_osc1Shape = 0;
	enum class auxUnipolar1Mod_to_osc1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar1Mod_to_osc1ShapeEnum::SWITCH_OFF, auxUnipolar1Mod_to_osc1Shape)) etc... 

	int auxUnipolar1Mod_to_osc2Fo = 0;
	enum class auxUnipolar1Mod_to_osc2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar1Mod_to_osc2FoEnum::SWITCH_OFF, auxUnipolar1Mod_to_osc2Fo)) etc... 

	int auxUnipolar1Mod_to_osc2Shape = 0;
	enum class auxUnipolar1Mod_to_osc2ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar1Mod_to_osc2ShapeEnum::SWITCH_OFF, auxUnipolar1Mod_to_osc2Shape)) etc... 

	int auxUnipolar1Mod_to_filter1Fo = 0;
	enum class auxUnipolar1Mod_to_filter1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar1Mod_to_filter1FoEnum::SWITCH_OFF, auxUnipolar1Mod_to_filter1Fo)) etc... 

	int auxUnipolar1Mod_to_filter1Q = 0;
	enum class auxUnipolar1Mod_to_filter1QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar1Mod_to_filter1QEnum::SWITCH_OFF, auxUnipolar1Mod_to_filter1Q)) etc... 

	int auxUnipolar1Mod_to_filter2Fo = 0;
	enum class auxUnipolar1Mod_to_filter2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar1Mod_to_filter2FoEnum::SWITCH_OFF, auxUnipolar1Mod_to_filter2Fo)) etc... 

	int auxUnipolar1Mod_to_filter2Q = 0;
	enum class auxUnipolar1Mod_to_filter2QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar1Mod_to_filter2QEnum::SWITCH_OFF, auxUnipolar1Mod_to_filter2Q)) etc... 

	int auxUnipolar1Mod_to_DCASampleHoldModIn = 0;
	enum class auxUnipolar1Mod_to_DCASampleHoldModInEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar1Mod_to_DCASampleHoldModInEnum::SWITCH_OFF, auxUnipolar1Mod_to_DCASampleHoldModIn)) etc... 

	int auxUnipolar2Mod_to_osc1Fo = 0;
	enum class auxUnipolar2Mod_to_osc1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar2Mod_to_osc1FoEnum::SWITCH_OFF, auxUnipolar2Mod_to_osc1Fo)) etc... 

	int auxUnipolar2Mod_to_osc1Shape = 0;
	enum class auxUnipolar2Mod_to_osc1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar2Mod_to_osc1ShapeEnum::SWITCH_OFF, auxUnipolar2Mod_to_osc1Shape)) etc... 

	int auxUnipolar2Mod_to_osc2Fo = 0;
	enum class auxUnipolar2Mod_to_osc2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar2Mod_to_osc2FoEnum::SWITCH_OFF, auxUnipolar2Mod_to_osc2Fo)) etc... 

	int auxUnipolar2Mod_to_osc2Shape = 0;
	enum class auxUnipolar2Mod_to_osc2ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar2Mod_to_osc2ShapeEnum::SWITCH_OFF, auxUnipolar2Mod_to_osc2Shape)) etc... 

	int auxUnipolar2Mod_to_filter1Fo = 0;
	enum class auxUnipolar2Mod_to_filter1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar2Mod_to_filter1FoEnum::SWITCH_OFF, auxUnipolar2Mod_to_filter1Fo)) etc... 

	int auxUnipolar2Mod_to_filter1Q = 0;
	enum class auxUnipolar2Mod_to_filter1QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar2Mod_to_filter1QEnum::SWITCH_OFF, auxUnipolar2Mod_to_filter1Q)) etc... 

	int auxUnipolar2Mod_to_filter2Fo = 0;
	enum class auxUnipolar2Mod_to_filter2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar2Mod_to_filter2FoEnum::SWITCH_OFF, auxUnipolar2Mod_to_filter2Fo)) etc... 

	int auxUnipolar2Mod_to_filter2Q = 0;
	enum class auxUnipolar2Mod_to_filter2QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar2Mod_to_filter2QEnum::SWITCH_OFF, auxUnipolar2Mod_to_filter2Q)) etc... 

	int auxUnipolar2Mod_to_DCASampleHoldModIn = 0;
	enum class auxUnipolar2Mod_to_DCASampleHoldModInEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar2Mod_to_DCASampleHoldModInEnum::SWITCH_OFF, auxUnipolar2Mod_to_DCASampleHoldModIn)) etc... 

	int auxUnipolar3Mod_to_osc1Fo = 0;
	enum class auxUnipolar3Mod_to_osc1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar3Mod_to_osc1FoEnum::SWITCH_OFF, auxUnipolar3Mod_to_osc1Fo)) etc... 

	int auxUnipolar3Mod_to_osc1Shape = 0;
	enum class auxUnipolar3Mod_to_osc1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar3Mod_to_osc1ShapeEnum::SWITCH_OFF, auxUnipolar3Mod_to_osc1Shape)) etc... 

	int auxUnipolar3Mod_to_osc2Fo = 0;
	enum class auxUnipolar3Mod_to_osc2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar3Mod_to_osc2FoEnum::SWITCH_OFF, auxUnipolar3Mod_to_osc2Fo)) etc... 

	int auxUnipolar3Mod_to_osc2Shape = 0;
	enum class auxUnipolar3Mod_to_osc2ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar3Mod_to_osc2ShapeEnum::SWITCH_OFF, auxUnipolar3Mod_to_osc2Shape)) etc... 

	int auxUnipolar3Mod_to_filter1Fo = 0;
	enum class auxUnipolar3Mod_to_filter1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar3Mod_to_filter1FoEnum::SWITCH_OFF, auxUnipolar3Mod_to_filter1Fo)) etc... 

	int auxUnipolar3Mod_to_filter1Q = 0;
	enum class auxUnipolar3Mod_to_filter1QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar3Mod_to_filter1QEnum::SWITCH_OFF, auxUnipolar3Mod_to_filter1Q)) etc... 

	int auxUnipolar3Mod_to_filter2Fo = 0;
	enum class auxUnipolar3Mod_to_filter2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar3Mod_to_filter2FoEnum::SWITCH_OFF, auxUnipolar3Mod_to_filter2Fo)) etc... 

	int auxUnipolar3Mod_to_filter2Q = 0;
	enum class auxUnipolar3Mod_to_filter2QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar3Mod_to_filter2QEnum::SWITCH_OFF, auxUnipolar3Mod_to_filter2Q)) etc... 

	int auxUnipolar3Mod_to_DCASampleHoldModIn = 0;
	enum class auxUnipolar3Mod_to_DCASampleHoldModInEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar3Mod_to_DCASampleHoldModInEnum::SWITCH_OFF, auxUnipolar3Mod_to_DCASampleHoldModIn)) etc... 

	int auxUnipolar4Mod_to_osc1Fo = 0;
	enum class auxUnipolar4Mod_to_osc1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar4Mod_to_osc1FoEnum::SWITCH_OFF, auxUnipolar4Mod_to_osc1Fo)) etc... 

	int auxUnipolar4Mod_to_osc1Shape = 0;
	enum class auxUnipolar4Mod_to_osc1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar4Mod_to_osc1ShapeEnum::SWITCH_OFF, auxUnipolar4Mod_to_osc1Shape)) etc... 

	int auxUnipolar4Mod_to_osc2Fo = 0;
	enum class auxUnipolar4Mod_to_osc2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar4Mod_to_osc2FoEnum::SWITCH_OFF, auxUnipolar4Mod_to_osc2Fo)) etc... 

	int auxUnipolar4Mod_to_osc2Shape = 0;
	enum class auxUnipolar4Mod_to_osc2ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar4Mod_to_osc2ShapeEnum::SWITCH_OFF, auxUnipolar4Mod_to_osc2Shape)) etc... 

	int auxUnipolar4Mod_to_filter1Fo = 0;
	enum class auxUnipolar4Mod_to_filter1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar4Mod_to_filter1FoEnum::SWITCH_OFF, auxUnipolar4Mod_to_filter1Fo)) etc... 

	int auxUnipolar4Mod_to_filter1Q = 0;
	enum class auxUnipolar4Mod_to_filter1QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar4Mod_to_filter1QEnum::SWITCH_OFF, auxUnipolar4Mod_to_filter1Q)) etc... 

	int auxUnipolar4Mod_to_filter2Fo = 0;
	enum class auxUnipolar4Mod_to_filter2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar4Mod_to_filter2FoEnum::SWITCH_OFF, auxUnipolar4Mod_to_filter2Fo)) etc... 

	int auxUnipolar4Mod_to_filter2Q = 0;
	enum class auxUnipolar4Mod_to_filter2QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar4Mod_to_filter2QEnum::SWITCH_OFF, auxUnipolar4Mod_to_filter2Q)) etc... 

	int auxUnipolar4Mod_to_DCASampleHoldModIn = 0;
	enum class auxUnipolar4Mod_to_DCASampleHoldModInEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar4Mod_to_DCASampleHoldModInEnum::SWITCH_OFF, auxUnipolar4Mod_to_DCASampleHoldModIn)) etc... 

	int auxUnipolar5Mod_to_osc1Fo = 0;
	enum class auxUnipolar5Mod_to_osc1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar5Mod_to_osc1FoEnum::SWITCH_OFF, auxUnipolar5Mod_to_osc1Fo)) etc... 

	int auxUnipolar5Mod_to_osc1Shape = 0;
	enum class auxUnipolar5Mod_to_osc1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar5Mod_to_osc1ShapeEnum::SWITCH_OFF, auxUnipolar5Mod_to_osc1Shape)) etc... 

	int auxUnipolar5Mod_to_osc2Fo = 0;
	enum class auxUnipolar5Mod_to_osc2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar5Mod_to_osc2FoEnum::SWITCH_OFF, auxUnipolar5Mod_to_osc2Fo)) etc... 

	int auxUnipolar5Mod_to_osc2Shape = 0;
	enum class auxUnipolar5Mod_to_osc2ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar5Mod_to_osc2ShapeEnum::SWITCH_OFF, auxUnipolar5Mod_to_osc2Shape)) etc... 

	int auxUnipolar5Mod_to_filter1Fo = 0;
	enum class auxUnipolar5Mod_to_filter1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar5Mod_to_filter1FoEnum::SWITCH_OFF, auxUnipolar5Mod_to_filter1Fo)) etc... 

	int auxUnipolar5Mod_to_filter1Q = 0;
	enum class auxUnipolar5Mod_to_filter1QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar5Mod_to_filter1QEnum::SWITCH_OFF, auxUnipolar5Mod_to_filter1Q)) etc... 

	int auxUnipolar5Mod_to_filter2Fo = 0;
	enum class auxUnipolar5Mod_to_filter2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar5Mod_to_filter2FoEnum::SWITCH_OFF, auxUnipolar5Mod_to_filter2Fo)) etc... 

	int auxUnipolar5Mod_to_filter2Q = 0;
	enum class auxUnipolar5Mod_to_filter2QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar5Mod_to_filter2QEnum::SWITCH_OFF, auxUnipolar5Mod_to_filter2Q)) etc... 

	int auxUnipolar5Mod_to_DCASampleHoldModIn = 0;
	enum class auxUnipolar5Mod_to_DCASampleHoldModInEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar5Mod_to_DCASampleHoldModInEnum::SWITCH_OFF, auxUnipolar5Mod_to_DCASampleHoldModIn)) etc... 

	int auxUnipolar6Mod_to_osc1Fo = 0;
	enum class auxUnipolar6Mod_to_osc1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar6Mod_to_osc1FoEnum::SWITCH_OFF, auxUnipolar6Mod_to_osc1Fo)) etc... 

	int auxUnipolar6Mod_to_osc1Shape = 0;
	enum class auxUnipolar6Mod_to_osc1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar6Mod_to_osc1ShapeEnum::SWITCH_OFF, auxUnipolar6Mod_to_osc1Shape)) etc... 

	int auxUnipolar6Mod_to_osc2Fo = 0;
	enum class auxUnipolar6Mod_to_osc2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar6Mod_to_osc2FoEnum::SWITCH_OFF, auxUnipolar6Mod_to_osc2Fo)) etc... 

	int auxUnipolar6Mod_to_osc2Shape = 0;
	enum class auxUnipolar6Mod_to_osc2ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar6Mod_to_osc2ShapeEnum::SWITCH_OFF, auxUnipolar6Mod_to_osc2Shape)) etc... 

	int auxUnipolar6Mod_to_filter1Fo = 0;
	enum class auxUnipolar6Mod_to_filter1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar6Mod_to_filter1FoEnum::SWITCH_OFF, auxUnipolar6Mod_to_filter1Fo)) etc... 

	int auxUnipolar6Mod_to_filter1Q = 0;
	enum class auxUnipolar6Mod_to_filter1QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar6Mod_to_filter1QEnum::SWITCH_OFF, auxUnipolar6Mod_to_filter1Q)) etc... 

	int auxUnipolar6Mod_to_filter2Fo = 0;
	enum class auxUnipolar6Mod_to_filter2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar6Mod_to_filter2FoEnum::SWITCH_OFF, auxUnipolar6Mod_to_filter2Fo)) etc... 

	int auxUnipolar6Mod_to_filter2Q = 0;
	enum class auxUnipolar6Mod_to_filter2QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar6Mod_to_filter2QEnum::SWITCH_OFF, auxUnipolar6Mod_to_filter2Q)) etc... 

	int auxUnipolar6Mod_to_DCASampleHoldModIn = 0;
	enum class auxUnipolar6Mod_to_DCASampleHoldModInEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar6Mod_to_DCASampleHoldModInEnum::SWITCH_OFF, auxUnipolar6Mod_to_DCASampleHoldModIn)) etc... 

	int auxBipolar1Mod_to_osc1Fo = 0;
	enum class auxBipolar1Mod_to_osc1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar1Mod_to_osc1FoEnum::SWITCH_OFF, auxBipolar1Mod_to_osc1Fo)) etc... 

	int auxBipolar1Mod_to_osc1Shape = 0;
	enum class auxBipolar1Mod_to_osc1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar1Mod_to_osc1ShapeEnum::SWITCH_OFF, auxBipolar1Mod_to_osc1Shape)) etc... 

	int auxBipolar1Mod_to_osc2Fo = 0;
	enum class auxBipolar1Mod_to_osc2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar1Mod_to_osc2FoEnum::SWITCH_OFF, auxBipolar1Mod_to_osc2Fo)) etc... 

	int auxBipolar1Mod_to_osc2Shape = 0;
	enum class auxBipolar1Mod_to_osc2ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar1Mod_to_osc2ShapeEnum::SWITCH_OFF, auxBipolar1Mod_to_osc2Shape)) etc... 

	int auxBipolar1Mod_to_filter1Fo = 0;
	enum class auxBipolar1Mod_to_filter1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar1Mod_to_filter1FoEnum::SWITCH_OFF, auxBipolar1Mod_to_filter1Fo)) etc... 

	int auxBipolar1Mod_to_filter1Q = 0;
	enum class auxBipolar1Mod_to_filter1QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar1Mod_to_filter1QEnum::SWITCH_OFF, auxBipolar1Mod_to_filter1Q)) etc... 

	int auxBipolar1Mod_to_filter2Fo = 0;
	enum class auxBipolar1Mod_to_filter2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar1Mod_to_filter2FoEnum::SWITCH_OFF, auxBipolar1Mod_to_filter2Fo)) etc... 

	int auxBipolar1Mod_to_filter2Q = 0;
	enum class auxBipolar1Mod_to_filter2QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar1Mod_to_filter2QEnum::SWITCH_OFF, auxBipolar1Mod_to_filter2Q)) etc... 

	int auxBipolar1Mod_to_DCASampleHoldModIn = 0;
	enum class auxBipolar1Mod_to_DCASampleHoldModInEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar1Mod_to_DCASampleHoldModInEnum::SWITCH_OFF, auxBipolar1Mod_to_DCASampleHoldModIn)) etc... 

	int auxBipolar2Mod_to_osc1Fo = 0;
	enum class auxBipolar2Mod_to_osc1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar2Mod_to_osc1FoEnum::SWITCH_OFF, auxBipolar2Mod_to_osc1Fo)) etc... 

	int auxBipolar2Mod_to_osc1Shape = 0;
	enum class auxBipolar2Mod_to_osc1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar2Mod_to_osc1ShapeEnum::SWITCH_OFF, auxBipolar2Mod_to_osc1Shape)) etc... 

	int auxBipolar2Mod_to_osc2Fo = 0;
	enum class auxBipolar2Mod_to_osc2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar2Mod_to_osc2FoEnum::SWITCH_OFF, auxBipolar2Mod_to_osc2Fo)) etc... 

	int auxBipolar2Mod_to_osc2Shape = 0;
	enum class auxBipolar2Mod_to_osc2ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar2Mod_to_osc2ShapeEnum::SWITCH_OFF, auxBipolar2Mod_to_osc2Shape)) etc... 

	int auxBipolar2Mod_to_filter1Fo = 0;
	enum class auxBipolar2Mod_to_filter1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar2Mod_to_filter1FoEnum::SWITCH_OFF, auxBipolar2Mod_to_filter1Fo)) etc... 

	int auxBipolar2Mod_to_filter1Q = 0;
	enum class auxBipolar2Mod_to_filter1QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar2Mod_to_filter1QEnum::SWITCH_OFF, auxBipolar2Mod_to_filter1Q)) etc... 

	int auxBipolar2Mod_to_filter2Fo = 0;
	enum class auxBipolar2Mod_to_filter2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar2Mod_to_filter2FoEnum::SWITCH_OFF, auxBipolar2Mod_to_filter2Fo)) etc... 

	int auxBipolar2Mod_to_filter2Q = 0;
	enum class auxBipolar2Mod_to_filter2QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar2Mod_to_filter2QEnum::SWITCH_OFF, auxBipolar2Mod_to_filter2Q)) etc... 

	int auxBipolar2Mod_to_DCASampleHoldModIn = 0;
	enum class auxBipolar2Mod_to_DCASampleHoldModInEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar2Mod_to_DCASampleHoldModInEnum::SWITCH_OFF, auxBipolar2Mod_to_DCASampleHoldModIn)) etc... 

	int auxBipolar3Mod_to_osc1Fo = 0;
	enum class auxBipolar3Mod_to_osc1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar3Mod_to_osc1FoEnum::SWITCH_OFF, auxBipolar3Mod_to_osc1Fo)) etc... 

	int auxBipolar3Mod_to_osc1Shape = 0;
	enum class auxBipolar3Mod_to_osc1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar3Mod_to_osc1ShapeEnum::SWITCH_OFF, auxBipolar3Mod_to_osc1Shape)) etc... 

	int auxBipolar3Mod_to_osc2Fo = 0;
	enum class auxBipolar3Mod_to_osc2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar3Mod_to_osc2FoEnum::SWITCH_OFF, auxBipolar3Mod_to_osc2Fo)) etc... 

	int auxBipolar3Mod_to_osc2Shape = 0;
	enum class auxBipolar3Mod_to_osc2ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar3Mod_to_osc2ShapeEnum::SWITCH_OFF, auxBipolar3Mod_to_osc2Shape)) etc... 

	int auxBipolar3Mod_to_filter1Fo = 0;
	enum class auxBipolar3Mod_to_filter1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar3Mod_to_filter1FoEnum::SWITCH_OFF, auxBipolar3Mod_to_filter1Fo)) etc... 

	int auxBipolar3Mod_to_filter1Q = 0;
	enum class auxBipolar3Mod_to_filter1QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar3Mod_to_filter1QEnum::SWITCH_OFF, auxBipolar3Mod_to_filter1Q)) etc... 

	int auxBipolar3Mod_to_filter2Fo = 0;
	enum class auxBipolar3Mod_to_filter2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar3Mod_to_filter2FoEnum::SWITCH_OFF, auxBipolar3Mod_to_filter2Fo)) etc... 

	int auxBipolar3Mod_to_filter2Q = 0;
	enum class auxBipolar3Mod_to_filter2QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar3Mod_to_filter2QEnum::SWITCH_OFF, auxBipolar3Mod_to_filter2Q)) etc... 

	int auxBipolar3Mod_to_DCASampleHoldModIn = 0;
	enum class auxBipolar3Mod_to_DCASampleHoldModInEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar3Mod_to_DCASampleHoldModInEnum::SWITCH_OFF, auxBipolar3Mod_to_DCASampleHoldModIn)) etc... 

	int auxBipolar4Mod_to_osc1Fo = 0;
	enum class auxBipolar4Mod_to_osc1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar4Mod_to_osc1FoEnum::SWITCH_OFF, auxBipolar4Mod_to_osc1Fo)) etc... 

	int auxBipolar4Mod_to_osc1Shape = 0;
	enum class auxBipolar4Mod_to_osc1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar4Mod_to_osc1ShapeEnum::SWITCH_OFF, auxBipolar4Mod_to_osc1Shape)) etc... 

	int auxBipolar4Mod_to_osc2Fo = 0;
	enum class auxBipolar4Mod_to_osc2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar4Mod_to_osc2FoEnum::SWITCH_OFF, auxBipolar4Mod_to_osc2Fo)) etc... 

	int auxBipolar4Mod_to_osc2Shape = 0;
	enum class auxBipolar4Mod_to_osc2ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar4Mod_to_osc2ShapeEnum::SWITCH_OFF, auxBipolar4Mod_to_osc2Shape)) etc... 

	int auxBipolar4Mod_to_filter1Fo = 0;
	enum class auxBipolar4Mod_to_filter1FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar4Mod_to_filter1FoEnum::SWITCH_OFF, auxBipolar4Mod_to_filter1Fo)) etc... 

	int auxBipolar4Mod_to_filter1Q = 0;
	enum class auxBipolar4Mod_to_filter1QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar4Mod_to_filter1QEnum::SWITCH_OFF, auxBipolar4Mod_to_filter1Q)) etc... 

	int auxBipolar4Mod_to_filter2Fo = 0;
	enum class auxBipolar4Mod_to_filter2FoEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar4Mod_to_filter2FoEnum::SWITCH_OFF, auxBipolar4Mod_to_filter2Fo)) etc... 

	int auxBipolar4Mod_to_filter2Q = 0;
	enum class auxBipolar4Mod_to_filter2QEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar4Mod_to_filter2QEnum::SWITCH_OFF, auxBipolar4Mod_to_filter2Q)) etc... 

	int auxBipolar4Mod_to_DCASampleHoldModIn = 0;
	enum class auxBipolar4Mod_to_DCASampleHoldModInEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar4Mod_to_DCASampleHoldModInEnum::SWITCH_OFF, auxBipolar4Mod_to_DCASampleHoldModIn)) etc... 

	int eg1_to_DCASampleHoldModIn = 0;
	enum class eg1_to_DCASampleHoldModInEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg1_to_DCASampleHoldModInEnum::SWITCH_OFF, eg1_to_DCASampleHoldModIn)) etc... 

	int eg2_to_DCASampleHoldModIn = 0;
	enum class eg2_to_DCASampleHoldModInEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg2_to_DCASampleHoldModInEnum::SWITCH_OFF, eg2_to_DCASampleHoldModIn)) etc... 

	int lfo1_to_lfo1Shape = 0;
	enum class lfo1_to_lfo1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo1_to_lfo1ShapeEnum::SWITCH_OFF, lfo1_to_lfo1Shape)) etc... 

	int lfo2_to_lfo1Shape = 0;
	enum class lfo2_to_lfo1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo2_to_lfo1ShapeEnum::SWITCH_OFF, lfo2_to_lfo1Shape)) etc... 

	int eg1_to_lfo1Shape = 0;
	enum class eg1_to_lfo1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg1_to_lfo1ShapeEnum::SWITCH_OFF, eg1_to_lfo1Shape)) etc... 

	int eg2_to_lfo1Shape = 0;
	enum class eg2_to_lfo1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg2_to_lfo1ShapeEnum::SWITCH_OFF, eg2_to_lfo1Shape)) etc... 

	int joystickAC_to_lfo1Shape = 0;
	enum class joystickAC_to_lfo1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickAC_to_lfo1ShapeEnum::SWITCH_OFF, joystickAC_to_lfo1Shape)) etc... 

	int joystickBD_to_lfo1Shape = 0;
	enum class joystickBD_to_lfo1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickBD_to_lfo1ShapeEnum::SWITCH_OFF, joystickBD_to_lfo1Shape)) etc... 

	int auxUnipolar1Mod_to_lfo1Shape = 0;
	enum class auxUnipolar1Mod_to_lfo1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar1Mod_to_lfo1ShapeEnum::SWITCH_OFF, auxUnipolar1Mod_to_lfo1Shape)) etc... 

	int auxUnipolar2Mod_to_lfo1Shape = 0;
	enum class auxUnipolar2Mod_to_lfo1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar2Mod_to_lfo1ShapeEnum::SWITCH_OFF, auxUnipolar2Mod_to_lfo1Shape)) etc... 

	int auxUnipolar3Mod_to_lfo1Shape = 0;
	enum class auxUnipolar3Mod_to_lfo1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar3Mod_to_lfo1ShapeEnum::SWITCH_OFF, auxUnipolar3Mod_to_lfo1Shape)) etc... 

	int auxUnipolar4Mod_to_lfo1Shape = 0;
	enum class auxUnipolar4Mod_to_lfo1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar4Mod_to_lfo1ShapeEnum::SWITCH_OFF, auxUnipolar4Mod_to_lfo1Shape)) etc... 

	int auxUnipolar5Mod_to_lfo1Shape = 0;
	enum class auxUnipolar5Mod_to_lfo1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar5Mod_to_lfo1ShapeEnum::SWITCH_OFF, auxUnipolar5Mod_to_lfo1Shape)) etc... 

	int auxUnipolar6Mod_to_lfo1Shape = 0;
	enum class auxUnipolar6Mod_to_lfo1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxUnipolar6Mod_to_lfo1ShapeEnum::SWITCH_OFF, auxUnipolar6Mod_to_lfo1Shape)) etc... 

	int auxBipolar1Mod_to_lfo1Shape = 0;
	enum class auxBipolar1Mod_to_lfo1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar1Mod_to_lfo1ShapeEnum::SWITCH_OFF, auxBipolar1Mod_to_lfo1Shape)) etc... 

	int auxBipolar2Mod_to_lfo1Shape = 0;
	enum class auxBipolar2Mod_to_lfo1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar2Mod_to_lfo1ShapeEnum::SWITCH_OFF, auxBipolar2Mod_to_lfo1Shape)) etc... 

	int auxBipolar3Mod_to_lfo1Shape = 0;
	enum class auxBipolar3Mod_to_lfo1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar3Mod_to_lfo1ShapeEnum::SWITCH_OFF, auxBipolar3Mod_to_lfo1Shape)) etc... 

	int auxBipolar4Mod_to_lfo1Shape = 0;
	enum class auxBipolar4Mod_to_lfo1ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(auxBipolar4Mod_to_lfo1ShapeEnum::SWITCH_OFF, auxBipolar4Mod_to_lfo1Shape)) etc... 

	int lfo1_to_unisonDetune = 0;
	enum class lfo1_to_unisonDetuneEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo1_to_unisonDetuneEnum::SWITCH_OFF, lfo1_to_unisonDetune)) etc... 

	int lfo2_to_unisonDetune = 0;
	enum class lfo2_to_unisonDetuneEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo2_to_unisonDetuneEnum::SWITCH_OFF, lfo2_to_unisonDetune)) etc... 

	int eg1_to_unisonDetune = 0;
	enum class eg1_to_unisonDetuneEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg1_to_unisonDetuneEnum::SWITCH_OFF, eg1_to_unisonDetune)) etc... 

	int eg2_to_unisonDetune = 0;
	enum class eg2_to_unisonDetuneEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg2_to_unisonDetuneEnum::SWITCH_OFF, eg2_to_unisonDetune)) etc... 

	int joystickAC_to_unisonDetune = 0;
	enum class joystickAC_to_unisonDetuneEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickAC_to_unisonDetuneEnum::SWITCH_OFF, joystickAC_to_unisonDetune)) etc... 

	int joystickBD_to_unisonDetune = 0;
	enum class joystickBD_to_unisonDetuneEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickBD_to_unisonDetuneEnum::SWITCH_OFF, joystickBD_to_unisonDetune)) etc... 

	int lfo1_to_lfo1ShapeSplit = 0;
	enum class lfo1_to_lfo1ShapeSplitEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo1_to_lfo1ShapeSplitEnum::SWITCH_OFF, lfo1_to_lfo1ShapeSplit)) etc... 

	int lfo2_to_lfo1ShapeSplit = 0;
	enum class lfo2_to_lfo1ShapeSplitEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo2_to_lfo1ShapeSplitEnum::SWITCH_OFF, lfo2_to_lfo1ShapeSplit)) etc... 

	int eg1_to_lfo1ShapeSplit = 0;
	enum class eg1_to_lfo1ShapeSplitEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg1_to_lfo1ShapeSplitEnum::SWITCH_OFF, eg1_to_lfo1ShapeSplit)) etc... 

	int eg2_to_lfo1ShapeSplit = 0;
	enum class eg2_to_lfo1ShapeSplitEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg2_to_lfo1ShapeSplitEnum::SWITCH_OFF, eg2_to_lfo1ShapeSplit)) etc... 

	int joystickAC_to_lfo1ShapeSplit = 0;
	enum class joystickAC_to_lfo1ShapeSplitEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickAC_to_lfo1ShapeSplitEnum::SWITCH_OFF, joystickAC_to_lfo1ShapeSplit)) etc... 

	int joystickBD_to_lfo1ShapeSplit = 0;
	enum class joystickBD_to_lfo1ShapeSplitEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickBD_to_lfo1ShapeSplitEnum::SWITCH_OFF, joystickBD_to_lfo1ShapeSplit)) etc... 

	int lfo1_to_lfo2Shape = 0;
	enum class lfo1_to_lfo2ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo1_to_lfo2ShapeEnum::SWITCH_OFF, lfo1_to_lfo2Shape)) etc... 

	int lfo2_to_lfo2Shape = 0;
	enum class lfo2_to_lfo2ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo2_to_lfo2ShapeEnum::SWITCH_OFF, lfo2_to_lfo2Shape)) etc... 

	int eg1_to_lfo2Shape = 0;
	enum class eg1_to_lfo2ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg1_to_lfo2ShapeEnum::SWITCH_OFF, eg1_to_lfo2Shape)) etc... 

	int eg2_to_lfo2Shape = 0;
	enum class eg2_to_lfo2ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg2_to_lfo2ShapeEnum::SWITCH_OFF, eg2_to_lfo2Shape)) etc... 

	int joystickAC_to_lfo2Shape = 0;
	enum class joystickAC_to_lfo2ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickAC_to_lfo2ShapeEnum::SWITCH_OFF, joystickAC_to_lfo2Shape)) etc... 

	int joystickBD_to_lfo2Shape = 0;
	enum class joystickBD_to_lfo2ShapeEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickBD_to_lfo2ShapeEnum::SWITCH_OFF, joystickBD_to_lfo2Shape)) etc... 

	int lfo1_to_lfo2ShapeSplit = 0;
	enum class lfo1_to_lfo2ShapeSplitEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo1_to_lfo2ShapeSplitEnum::SWITCH_OFF, lfo1_to_lfo2ShapeSplit)) etc... 

	int lfo2_to_lfo2ShapeSplit = 0;
	enum class lfo2_to_lfo2ShapeSplitEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo2_to_lfo2ShapeSplitEnum::SWITCH_OFF, lfo2_to_lfo2ShapeSplit)) etc... 

	int eg1_to_lfo2ShapeSplit = 0;
	enum class eg1_to_lfo2ShapeSplitEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg1_to_lfo2ShapeSplitEnum::SWITCH_OFF, eg1_to_lfo2ShapeSplit)) etc... 

	int eg2_to_lfo2ShapeSplit = 0;
	enum class eg2_to_lfo2ShapeSplitEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg2_to_lfo2ShapeSplitEnum::SWITCH_OFF, eg2_to_lfo2ShapeSplit)) etc... 

	int joystickAC_to_lfo2ShapeSplit = 0;
	enum class joystickAC_to_lfo2ShapeSplitEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickAC_to_lfo2ShapeSplitEnum::SWITCH_OFF, joystickAC_to_lfo2ShapeSplit)) etc... 

	int joystickBD_to_lfo2ShapeSplit = 0;
	enum class joystickBD_to_lfo2ShapeSplitEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickBD_to_lfo2ShapeSplitEnum::SWITCH_OFF, joystickBD_to_lfo2ShapeSplit)) etc... 

	int lfo1_to_eg1Attack = 0;
	enum class lfo1_to_eg1AttackEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo1_to_eg1AttackEnum::SWITCH_OFF, lfo1_to_eg1Attack)) etc... 

	int lfo2_to_eg1Attack = 0;
	enum class lfo2_to_eg1AttackEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo2_to_eg1AttackEnum::SWITCH_OFF, lfo2_to_eg1Attack)) etc... 

	int joystickAC_to_eg1Attack = 0;
	enum class joystickAC_to_eg1AttackEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickAC_to_eg1AttackEnum::SWITCH_OFF, joystickAC_to_eg1Attack)) etc... 

	int joystickBD_to_eg1Attack = 0;
	enum class joystickBD_to_eg1AttackEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickBD_to_eg1AttackEnum::SWITCH_OFF, joystickBD_to_eg1Attack)) etc... 

	int eg2_to_eg1Attack = 0;
	enum class eg2_to_eg1AttackEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg2_to_eg1AttackEnum::SWITCH_OFF, eg2_to_eg1Attack)) etc... 

	int lfo1_to_eg1Hold = 0;
	enum class lfo1_to_eg1HoldEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo1_to_eg1HoldEnum::SWITCH_OFF, lfo1_to_eg1Hold)) etc... 

	int lfo2_to_eg1Hold = 0;
	enum class lfo2_to_eg1HoldEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo2_to_eg1HoldEnum::SWITCH_OFF, lfo2_to_eg1Hold)) etc... 

	int eg2_to_eg1Hold = 0;
	enum class eg2_to_eg1HoldEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg2_to_eg1HoldEnum::SWITCH_OFF, eg2_to_eg1Hold)) etc... 

	int joystickAC_to_eg1Hold = 0;
	enum class joystickAC_to_eg1HoldEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickAC_to_eg1HoldEnum::SWITCH_OFF, joystickAC_to_eg1Hold)) etc... 

	int joystickBD_to_eg1Hold = 0;
	enum class joystickBD_to_eg1HoldEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickBD_to_eg1HoldEnum::SWITCH_OFF, joystickBD_to_eg1Hold)) etc... 

	int lfo1_to_eg1Decay = 0;
	enum class lfo1_to_eg1DecayEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo1_to_eg1DecayEnum::SWITCH_OFF, lfo1_to_eg1Decay)) etc... 

	int lfo2_to_eg1Decay = 0;
	enum class lfo2_to_eg1DecayEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo2_to_eg1DecayEnum::SWITCH_OFF, lfo2_to_eg1Decay)) etc... 

	int eg2_to_eg1Decay = 0;
	enum class eg2_to_eg1DecayEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg2_to_eg1DecayEnum::SWITCH_OFF, eg2_to_eg1Decay)) etc... 

	int joystickAC_to_eg1Decay = 0;
	enum class joystickAC_to_eg1DecayEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickAC_to_eg1DecayEnum::SWITCH_OFF, joystickAC_to_eg1Decay)) etc... 

	int joystickBD_to_eg1Decay = 0;
	enum class joystickBD_to_eg1DecayEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickBD_to_eg1DecayEnum::SWITCH_OFF, joystickBD_to_eg1Decay)) etc... 

	int lfo1_to_eg1Sustain = 0;
	enum class lfo1_to_eg1SustainEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo1_to_eg1SustainEnum::SWITCH_OFF, lfo1_to_eg1Sustain)) etc... 

	int lfo2_to_eg1Sustain = 0;
	enum class lfo2_to_eg1SustainEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo2_to_eg1SustainEnum::SWITCH_OFF, lfo2_to_eg1Sustain)) etc... 

	int eg2_to_eg1Sustain = 0;
	enum class eg2_to_eg1SustainEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg2_to_eg1SustainEnum::SWITCH_OFF, eg2_to_eg1Sustain)) etc... 

	int joystickAC_to_eg1Sustain = 0;
	enum class joystickAC_to_eg1SustainEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickAC_to_eg1SustainEnum::SWITCH_OFF, joystickAC_to_eg1Sustain)) etc... 

	int joystickBD_to_eg1Sustain = 0;
	enum class joystickBD_to_eg1SustainEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickBD_to_eg1SustainEnum::SWITCH_OFF, joystickBD_to_eg1Sustain)) etc... 

	int lfo1_to_eg1Release = 0;
	enum class lfo1_to_eg1ReleaseEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo1_to_eg1ReleaseEnum::SWITCH_OFF, lfo1_to_eg1Release)) etc... 

	int lfo2_to_eg1Release = 0;
	enum class lfo2_to_eg1ReleaseEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(lfo2_to_eg1ReleaseEnum::SWITCH_OFF, lfo2_to_eg1Release)) etc... 

	int eg2_to_eg1Release = 0;
	enum class eg2_to_eg1ReleaseEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(eg2_to_eg1ReleaseEnum::SWITCH_OFF, eg2_to_eg1Release)) etc... 

	int joystickAC_to_eg1Release = 0;
	enum class joystickAC_to_eg1ReleaseEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickAC_to_eg1ReleaseEnum::SWITCH_OFF, joystickAC_to_eg1Release)) etc... 

	int joystickBD_to_eg1Release = 0;
	enum class joystickBD_to_eg1ReleaseEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(joystickBD_to_eg1ReleaseEnum::SWITCH_OFF, joystickBD_to_eg1Release)) etc... 

	int osc1ExciterMode = 0;
	enum class osc1ExciterModeEnum { None,Noise,Waveform };	// to compare: if(compareEnumToInt(osc1ExciterModeEnum::None, osc1ExciterMode)) etc... 

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
