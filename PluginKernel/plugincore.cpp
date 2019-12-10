// -----------------------------------------------------------------------------
//    ASPiK Plugin Kernel File:  plugincore.cpp
//
/**
    \file   plugincore.cpp
    \author Will Pirkle
    \date   17-September-2018
    \brief  Implementation file for PluginCore object
    		- http://www.aspikplugins.com
    		- http://www.willpirkle.com
*/
// -----------------------------------------------------------------------------
#include "plugincore.h"
#include "plugindescription.h"
#include "bankwaveviews.h"

/**
\brief PluginCore constructor is launching pad for object initialization

Operations:
- initialize the plugin description (strings, codes, numbers, see initPluginDescriptors())
- setup the plugin's audio I/O channel support
- create the PluginParameter objects that represent the plugin parameters (see FX book if needed)
- create the presets
*/
PluginCore::PluginCore()
{
    // --- describe the plugin; call the helper to init the static parts you setup in plugindescription.h
    initPluginDescriptors();

    // --- default I/O combinations
	// --- for FX plugins
	if (getPluginType() == kFXPlugin)
	{
		addSupportedIOCombination({ kCFMono, kCFMono });
		addSupportedIOCombination({ kCFMono, kCFStereo });
		addSupportedIOCombination({ kCFStereo, kCFStereo });
	}
	else // --- synth plugins have no input, only output
	{
		addSupportedIOCombination({ kCFNone, kCFMono });
		addSupportedIOCombination({ kCFNone, kCFStereo });
	}

	// --- for sidechaining, we support mono and stereo inputs; auxOutputs reserved for future use
	addSupportedAuxIOCombination({ kCFMono, kCFNone });
	addSupportedAuxIOCombination({ kCFStereo, kCFNone });

	// --- create the parameters
    initPluginParameters();

    // --- create the presets
    initPluginPresets();
}

/**
\brief create all of your plugin parameters here

\return true if parameters were created, false if they already existed
*/
bool PluginCore::initPluginParameters()
{
	if (pluginParameterMap.size() > 0)
		return false;

    // --- Add your plugin parameter instantiation code bewtween these hex codes
	// **--0xDEA7--**


	// --- Declaration of Plugin Parameter Objects 
	PluginParameter* piParam = nullptr;

	// --- discrete control: LFO1 Wave
	piParam = new PluginParameter(controlID::lfo1Waveform, "LFO1 Wave", "Triangle,Sin,Saw,RSH,QRSH,Noise,QRNoise", "Triangle");
	piParam->setBoundVariable(&lfo1Waveform, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO1 Mode
	piParam = new PluginParameter(controlID::lfo1Mode, "LFO1 Mode", "Sync,One Shot,Free Run", "Sync");
	piParam->setBoundVariable(&lfo1Mode, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: LFO1 Fo
	piParam = new PluginParameter(controlID::lfo1Frequency_Hz, "LFO1 Fo", "Hz", controlVariableType::kDouble, 0.000000, 20.000000, 0.500000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&lfo1Frequency_Hz, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: PB Range
	piParam = new PluginParameter(controlID::masterPitchBend, "PB Range", "semi", controlVariableType::kDouble, 1.000000, 24.000000, 7.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&masterPitchBend, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Master Tune
	piParam = new PluginParameter(controlID::masterTune, "Master Tune", "semi", controlVariableType::kDouble, -12.000000, 12.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&masterTune, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Master Volume
	piParam = new PluginParameter(controlID::masterVolume_dB, "Master Volume", "dB", controlVariableType::kDouble, -60.000000, 12.000000, 3.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&masterVolume_dB, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: LFO1 Delay Time
	piParam = new PluginParameter(controlID::lfo1DelayTime_mSec, "LFO1 Delay Time", "mSec", controlVariableType::kDouble, 0.000000, 2000.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&lfo1DelayTime_mSec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: LFO1 Ramp Time
	piParam = new PluginParameter(controlID::lfo1RampTime_mSec, "LFO1 Ramp Time", "mSec", controlVariableType::kDouble, 0.000000, 2000.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&lfo1RampTime_mSec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: LFO2 Mode
	piParam = new PluginParameter(controlID::lfo2Mode, "LFO2 Mode", "Sync,One Shot,Free Run", "Sync");
	piParam->setBoundVariable(&lfo2Mode, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO2 Wave
	piParam = new PluginParameter(controlID::lfo2Waveform, "LFO2 Wave", "Triangle,Sin,Saw,RSH,QRSH,Noise,QRNoise", "Triangle");
	piParam->setBoundVariable(&lfo2Waveform, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: LFO2 Fo
	piParam = new PluginParameter(controlID::lfo2Frequency_Hz, "LFO2 Fo", "Hz", controlVariableType::kDouble, 0.000000, 20.000000, 0.020000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&lfo2Frequency_Hz, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: LFO1 Shape
	piParam = new PluginParameter(controlID::lfo1Shape, "LFO1 Shape", "", controlVariableType::kDouble, 0.020000, 0.980000, 0.500000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&lfo1Shape, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: LFO1 ShapeSplit
	piParam = new PluginParameter(controlID::lfo1ShapeSplit, "LFO1 ShapeSplit", "", controlVariableType::kDouble, 0.020000, 0.980000, 0.500000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&lfo1ShapeSplit, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: Scale
	piParam = new PluginParameter(controlID::ScaleMode, "Scale", "None,Ionian,Dorian,Phrygian,Lydian,Mixolydian,Aeolian,Locrian,Chromatic", "None");
	piParam->setBoundVariable(&ScaleMode, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Rhythmic Breaks
	piParam = new PluginParameter(controlID::enableRhythmicBreaks, "Rhythmic Breaks", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&enableRhythmicBreaks, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: LFO2 Shape
	piParam = new PluginParameter(controlID::lfo2Shape, "LFO2 Shape", "", controlVariableType::kDouble, 0.020000, 0.980000, 0.500000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&lfo2Shape, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: LFO2 ShapeSplit
	piParam = new PluginParameter(controlID::lfo2ShapeSplit, "LFO2 ShapeSplit", "", controlVariableType::kDouble, 0.020000, 0.980000, 0.500000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&lfo2ShapeSplit, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: Modulation Target
	piParam = new PluginParameter(controlID::lfo2ModTarget, "Modulation Target", "None,LFO1 Fo,LFO1 Shape,Both,Rhythmic Breaks", "None");
	piParam->setBoundVariable(&lfo2ModTarget, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Osc1 Wave
	piParam = new PluginParameter(controlID::osc1Waveform, "Osc1 Wave", "wave0,wave1,wave2,wave3,wave4,wave5,wave6,wave7,wave8,wave9,wave10,wave11,wave12,wave13,wave14,wave15,wave16,wave17,wave18,wave19,wave20,wave21,wave22,wave23,wave24,wave25,wave26,wave27,wave28,wave29,wave30,wave31", "wave0");
	piParam->setBoundVariable(&osc1Waveform, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Osc1 Bank
	piParam = new PluginParameter(controlID::osc1BankIndex, "Osc1 Bank", "Bank 0,Bank 1,Bank 2,Bank 3", "Bank 0");
	piParam->setBoundVariable(&osc1BankIndex, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Osc2 Wave
	piParam = new PluginParameter(controlID::osc2Waveform, "Osc2 Wave", "wave0,wave1,wave2,wave3,wave4,wave5,wave6,wave7,wave8,wave9,wave10,wave11,wave12,wave13,wave14,wave15,wave16,wave17,wave18,wave19,wave20,wave21,wave22,wave23,wave24,wave25,wave26,wave27,wave28,wave29,wave30,wave31", "wave0");
	piParam->setBoundVariable(&osc2Waveform, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Osc3 Wave
	piParam = new PluginParameter(controlID::osc3Waveform, "Osc3 Wave", "wave0,wave1,wave2,wave3,wave4,wave5,wave6,wave7,wave8,wave9,wave10,wave11,wave12,wave13,wave14,wave15,wave16,wave17,wave18,wave19,wave20,wave21,wave22,wave23,wave24,wave25,wave26,wave27,wave28,wave29,wave30,wave31", "wave0");
	piParam->setBoundVariable(&osc3Waveform, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Osc4
	piParam = new PluginParameter(controlID::osc4Waveform, "Osc4", "wave0,wave1,wave2,wave3,wave4,wave5,wave6,wave7,wave8,wave9,wave10,wave11,wave12,wave13,wave14,wave15,wave16,wave17,wave18,wave19,wave20,wave21,wave22,wave23,wave24,wave25,wave26,wave27,wave28,wave29,wave30,wave31", "wave0");
	piParam->setBoundVariable(&osc4Waveform, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Osc2 Bank
	piParam = new PluginParameter(controlID::osc2BankIndex, "Osc2 Bank", "Bank 0,Bank 1,Bank 2,Bank 3", "Bank 0");
	piParam->setBoundVariable(&osc2BankIndex, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Osc3 Bank
	piParam = new PluginParameter(controlID::osc3BankIndex, "Osc3 Bank", "Bank 0,Bank 1,Bank 2,Bank 3", "Bank 0");
	piParam->setBoundVariable(&osc3BankIndex, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Osc4 Bank
	piParam = new PluginParameter(controlID::osc4BankIndex, "Osc4 Bank", "Bank 0,Bank 1,Bank 2,Bank 3", "Bank 0");
	piParam->setBoundVariable(&osc4BankIndex, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: Osc1 Detune
	piParam = new PluginParameter(controlID::osc1Detune_cents, "Osc1 Detune", "Cents", controlVariableType::kDouble, -100.000000, 100.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&osc1Detune_cents, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: FC 1
	piParam = new PluginParameter(controlID::fc1_hertz, "FC 1", "Hertz", controlVariableType::kDouble, 20.000000, 18000.000000, 2000.000000, taper::kVoltOctaveTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&fc1_hertz, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Q 1
	piParam = new PluginParameter(controlID::q1Control, "Q 1", "", controlVariableType::kDouble, 1.000000, 10.000000, 1.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&q1Control, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: LFO1 Out
	piParam = new PluginParameter(controlID::lfo1Amplitude, "LFO1 Out", "", controlVariableType::kDouble, -1.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&lfo1Amplitude, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Osc1 Fo In
	piParam = new PluginParameter(controlID::osc1FoModIn, "Osc1 Fo In", "", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&osc1FoModIn, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: Key Track
	piParam = new PluginParameter(controlID::enableKeyTrack, "Key Track", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&enableKeyTrack, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: Key Track Ratio
	piParam = new PluginParameter(controlID::keyTrackRatio, "Key Track Ratio", "", controlVariableType::kDouble, 0.250000, 4.000000, 1.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&keyTrackRatio, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: LFO1
	piParam = new PluginParameter(controlID::lfo1_to_osc1Fo, "LFO1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo1_to_osc1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: EG2 Delay
	piParam = new PluginParameter(controlID::eg2DelayTime_mSec, "EG2 Delay", "mSec", controlVariableType::kDouble, 0.000000, 100.000000, 10.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg2DelayTime_mSec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: EG2 Attack
	piParam = new PluginParameter(controlID::eg2AttackTime_mSec, "EG2 Attack", "mSec", controlVariableType::kDouble, 0.000000, 2000.000000, 10.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg2AttackTime_mSec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: EG2 Hold
	piParam = new PluginParameter(controlID::eg2HoldTime_mSec, "EG2 Hold", "msec", controlVariableType::kDouble, 0.000000, 2000.000000, 10.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg2HoldTime_mSec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: EG2 Decay
	piParam = new PluginParameter(controlID::eg2DecayTime_mSec, "EG2 Decay", "mSec", controlVariableType::kDouble, 0.000000, 1000.000000, 100.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg2DecayTime_mSec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: EG2 Sustain
	piParam = new PluginParameter(controlID::eg2SustainLevel, "EG2 Sustain", "", controlVariableType::kDouble, 0.000000, 1.000000, 0.707000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg2SustainLevel, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: EG2 Release
	piParam = new PluginParameter(controlID::eg2ReleaseTime_mSec, "EG2 Release", "mSec", controlVariableType::kDouble, 0.000000, 10000.000000, 1000.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg2ReleaseTime_mSec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: EG2 AutoTrig
	piParam = new PluginParameter(controlID::eg2AutoTrigger, "EG2 AutoTrig", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg2AutoTrigger, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG2 Trigger
	piParam = new PluginParameter(controlID::eg2ManualTrigger, "EG2 Trigger", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg2ManualTrigger, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: Eg1 Delay
	piParam = new PluginParameter(controlID::eg1DelayTime_mSec, "Eg1 Delay", "mSec", controlVariableType::kDouble, 0.000000, 100.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg1DelayTime_mSec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: EG1 Attack
	piParam = new PluginParameter(controlID::eg1AttackTime_mSec, "EG1 Attack", "mSec", controlVariableType::kDouble, 0.000000, 2000.000000, 5.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg1AttackTime_mSec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: EG1 Hold
	piParam = new PluginParameter(controlID::eg1HoldTime_mSec, "EG1 Hold", "mSec", controlVariableType::kDouble, 0.000000, 100.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg1HoldTime_mSec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: EG1 Decay
	piParam = new PluginParameter(controlID::eg1DecayTime_mSec, "EG1 Decay", "mSec", controlVariableType::kDouble, 0.000000, 2000.000000, 100.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg1DecayTime_mSec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: EG1 Sustain
	piParam = new PluginParameter(controlID::eg1SustainLevel, "EG1 Sustain", "", controlVariableType::kDouble, 0.000000, 1.000000, 0.707000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg1SustainLevel, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: EG1 Release
	piParam = new PluginParameter(controlID::eg1ReleaseTime_mSec, "EG1 Release", "mSec", controlVariableType::kDouble, 0.000000, 10000.000000, 1000.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg1ReleaseTime_mSec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: EG1 AutoTrig
	piParam = new PluginParameter(controlID::eg1AutoRetrigger, "EG1 AutoTrig", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg1AutoRetrigger, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG1 Trigger
	piParam = new PluginParameter(controlID::eg1ManualTrigger, "EG1 Trigger", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg1ManualTrigger, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: EG1 Offset
	piParam = new PluginParameter(controlID::eg1Offset, "EG1 Offset", "", controlVariableType::kDouble, -1.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg1Offset, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: EG2 Offset
	piParam = new PluginParameter(controlID::eg2Offset, "EG2 Offset", "", controlVariableType::kDouble, -1.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg2Offset, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: EG1 Mode
	piParam = new PluginParameter(controlID::eg1Mode, "EG1 Mode", "ADSR,AHDSR,AHR", "ADSR");
	piParam->setBoundVariable(&eg1Mode, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG2 Mode
	piParam = new PluginParameter(controlID::eg2Mode, "EG2 Mode", "ADSR,AHDSR,AHR", "ADSR");
	piParam->setBoundVariable(&eg2Mode, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Voice Mode
	piParam = new PluginParameter(controlID::mode, "Voice Mode", "Poly,Mono,Unison", "Poly");
	piParam->setBoundVariable(&mode, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: Unison Detune
	piParam = new PluginParameter(controlID::unisonDetune_Cents, "Unison Detune", "Cents", controlVariableType::kDouble, -10.000000, 10.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&unisonDetune_Cents, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: EG1 Out
	piParam = new PluginParameter(controlID::eg1ModOut, "EG1 Out", "Units", controlVariableType::kDouble, 0.000000, 1.000000, 1.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg1ModOut, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: EG1
	piParam = new PluginParameter(controlID::eg1_to_osc1Fo, "EG1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg1_to_osc1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: LFO2 Out
	piParam = new PluginParameter(controlID::lfo2Amplitude, "LFO2 Out", "", controlVariableType::kDouble, -1.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&lfo2Amplitude, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: EG2 Out
	piParam = new PluginParameter(controlID::eg2ModOut, "EG2 Out", "Units", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg2ModOut, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Joystick AC Out
	piParam = new PluginParameter(controlID::joystickACModOut, "Joystick AC Out", "Units", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&joystickACModOut, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Joystick BD Out
	piParam = new PluginParameter(controlID::joystickBDModOut, "Joystick BD Out", "Units", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&joystickBDModOut, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: LFO1
	piParam = new PluginParameter(controlID::lfo1_to_osc2Fo, "LFO1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo1_to_osc2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: Osc2 Fo In
	piParam = new PluginParameter(controlID::osc2FoModIn, "Osc2 Fo In", "", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&osc2FoModIn, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Osc1 Shape In
	piParam = new PluginParameter(controlID::osc1ShapeModIn, "Osc1 Shape In", "", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&osc1ShapeModIn, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Osc2 Shape In
	piParam = new PluginParameter(controlID::osc2ShapeModIn, "Osc2 Shape In", "", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&osc2ShapeModIn, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Filter 1 Fo In
	piParam = new PluginParameter(controlID::filter1FoModIn, "Filter 1 Fo In", "", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&filter1FoModIn, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Filter 1 Q In
	piParam = new PluginParameter(controlID::filter1QModIn, "Filter 1 Q In", "", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&filter1QModIn, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Filter 2 Fo In
	piParam = new PluginParameter(controlID::filter2FoModIn, "Filter 2 Fo In", "", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&filter2FoModIn, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Filter 2 Q In
	piParam = new PluginParameter(controlID::filter2QModIn, "Filter 2 Q In", "", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&filter2QModIn, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: LFO1
	piParam = new PluginParameter(controlID::lfo1_to_osc1Shape, "LFO1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo1_to_osc1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO1
	piParam = new PluginParameter(controlID::lfo1_to_osc2Shape, "LFO1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo1_to_osc2Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO1
	piParam = new PluginParameter(controlID::lfo1_to_filter1Fo, "LFO1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo1_to_filter1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO1
	piParam = new PluginParameter(controlID::lfo1_to_filter1Q, "LFO1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo1_to_filter1Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO1
	piParam = new PluginParameter(controlID::lfo1_to_filter2Fo, "LFO1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo1_to_filter2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO1
	piParam = new PluginParameter(controlID::lfo1_to_filter2Q, "LFO1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo1_to_filter2Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO2
	piParam = new PluginParameter(controlID::lfo2_to_osc1Fo, "LFO2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo2_to_osc1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO2
	piParam = new PluginParameter(controlID::lfo2_to_osc1Shape, "LFO2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo2_to_osc1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO2
	piParam = new PluginParameter(controlID::lfo2_to_osc2Fo, "LFO2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo2_to_osc2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO2
	piParam = new PluginParameter(controlID::lfo2_to_osc2Shape, "LFO2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo2_to_osc2Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO2
	piParam = new PluginParameter(controlID::lfo2_to_filter1Fo, "LFO2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo2_to_filter1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO2
	piParam = new PluginParameter(controlID::lfo2_to_filter1Q, "LFO2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo2_to_filter1Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO2
	piParam = new PluginParameter(controlID::lfo2_to_filter2Fo, "LFO2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo2_to_filter2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO2
	piParam = new PluginParameter(controlID::lfo2_to_filter2Q, "LFO2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo2_to_filter2Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG1
	piParam = new PluginParameter(controlID::eg1_to_osc1Shape, "EG1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg1_to_osc1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG1
	piParam = new PluginParameter(controlID::eg1_to_osc2Fo, "EG1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg1_to_osc2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG1
	piParam = new PluginParameter(controlID::eg1_to_osc2Shape, "EG1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg1_to_osc2Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG1
	piParam = new PluginParameter(controlID::eg1_to_filter1Fo, "EG1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg1_to_filter1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG1
	piParam = new PluginParameter(controlID::eg1_to_filter1Q, "EG1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg1_to_filter1Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG1
	piParam = new PluginParameter(controlID::eg1_to_filter2Fo, "EG1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg1_to_filter2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG1
	piParam = new PluginParameter(controlID::eg1_to_filter2Q, "EG1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg1_to_filter2Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG2
	piParam = new PluginParameter(controlID::eg2_to_osc1Fo, "EG2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg2_to_osc1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG2
	piParam = new PluginParameter(controlID::eg2_to_osc1Shape, "EG2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg2_to_osc1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG2
	piParam = new PluginParameter(controlID::eg2_to_osc2Fo, "EG2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg2_to_osc2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG2
	piParam = new PluginParameter(controlID::eg2_to_osc2Shape, "EG2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg2_to_osc2Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG2
	piParam = new PluginParameter(controlID::eg2_to_filter1Fo, "EG2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg2_to_filter1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG2
	piParam = new PluginParameter(controlID::eg2_to_filter1Q, "EG2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg2_to_filter1Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG2
	piParam = new PluginParameter(controlID::eg2_to_filter2Fo, "EG2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg2_to_filter2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG2
	piParam = new PluginParameter(controlID::eg2_to_filter2Q, "EG2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg2_to_filter2Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. AC
	piParam = new PluginParameter(controlID::joystickAC_to_osc1Fo, "JS. AC", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickAC_to_osc1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. AC
	piParam = new PluginParameter(controlID::joystickAC_to_osc1Shape, "JS. AC", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickAC_to_osc1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. AC
	piParam = new PluginParameter(controlID::joystickAC_to_osc2Fo, "JS. AC", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickAC_to_osc2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. AC
	piParam = new PluginParameter(controlID::joystickAC_to_osc2Shape, "JS. AC", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickAC_to_osc2Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. AC
	piParam = new PluginParameter(controlID::joystickAC_to_filter1Fo, "JS. AC", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickAC_to_filter1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. AC
	piParam = new PluginParameter(controlID::joystickAC_to_filter1Q, "JS. AC", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickAC_to_filter1Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. AC
	piParam = new PluginParameter(controlID::joystickAC_to_filter2Fo, "JS. AC", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickAC_to_filter2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. AC
	piParam = new PluginParameter(controlID::joystickAC_to_filter2Q, "JS. AC", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickAC_to_filter2Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. BD
	piParam = new PluginParameter(controlID::joystickBD_to_osc1Fo, "JS. BD", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickBD_to_osc1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. BD
	piParam = new PluginParameter(controlID::joystickBD_to_osc1Shape, "JS. BD", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickBD_to_osc1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. BD
	piParam = new PluginParameter(controlID::joystickBD_to_osc2Fo, "JS. BD", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickBD_to_osc2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. BD
	piParam = new PluginParameter(controlID::joystickBD_to_osc2Shape, "JS. BD", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickBD_to_osc2Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. BD
	piParam = new PluginParameter(controlID::joystickBD_to_filter1Fo, "JS. BD", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickBD_to_filter1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. BD
	piParam = new PluginParameter(controlID::joystickBD_to_filter1Q, "JS. BD", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickBD_to_filter1Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. BD
	piParam = new PluginParameter(controlID::joystickBD_to_filter2Fo, "JS. BD", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickBD_to_filter2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. BD
	piParam = new PluginParameter(controlID::joystickBD_to_filter2Q, "JS. BD", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickBD_to_filter2Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: AuxUni 1 Out
	piParam = new PluginParameter(controlID::auxUnipolar1ModOut, "AuxUni 1 Out", "Units", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&auxUnipolar1ModOut, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: AuxUni 2 Out
	piParam = new PluginParameter(controlID::auxUnipolar2ModOut, "AuxUni 2 Out", "Units", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&auxUnipolar2ModOut, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: AuxUni 3 Out
	piParam = new PluginParameter(controlID::auxUnipolar3ModOut, "AuxUni 3 Out", "Units", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&auxUnipolar3ModOut, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: AuxUni 4 Out
	piParam = new PluginParameter(controlID::auxUnipolar4ModOut, "AuxUni 4 Out", "Units", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&auxUnipolar4ModOut, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: AuxUni 5 Out
	piParam = new PluginParameter(controlID::auxUnipolar5ModOut, "AuxUni 5 Out", "Units", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&auxUnipolar5ModOut, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: AuxUni 6 Out
	piParam = new PluginParameter(controlID::auxUnipolar6ModOut, "AuxUni 6 Out", "Units", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&auxUnipolar6ModOut, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: AuxBi 1 Out
	piParam = new PluginParameter(controlID::auxBipolar1ModOut, "AuxBi 1 Out", "Units", controlVariableType::kDouble, -1.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&auxBipolar1ModOut, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: AuxBi 2 Out
	piParam = new PluginParameter(controlID::auxBipolar2ModOut, "AuxBi 2 Out", "Units", controlVariableType::kDouble, -1.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&auxBipolar2ModOut, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: AuxBi 3 Out
	piParam = new PluginParameter(controlID::auxBipolar3ModOut, "AuxBi 3 Out", "Units", controlVariableType::kDouble, -1.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&auxBipolar3ModOut, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: AuxBi 4 Out
	piParam = new PluginParameter(controlID::auxBipolar4ModOut, "AuxBi 4 Out", "Units", controlVariableType::kDouble, -1.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&auxBipolar4ModOut, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Samp.Hold Mod In
	piParam = new PluginParameter(controlID::DCASampleHoldModIn, "Samp.Hold Mod In", "Units", controlVariableType::kDouble, -1.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&DCASampleHoldModIn, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: LFO1
	piParam = new PluginParameter(controlID::lfo1_to_DCASampleHoldModIn, "LFO1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo1_to_DCASampleHoldModIn, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO2
	piParam = new PluginParameter(controlID::lfo2_to_DCASampleHoldModIn, "LFO2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo2_to_DCASampleHoldModIn, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. AC
	piParam = new PluginParameter(controlID::joystickAC_to_DCASampleHoldModIn, "JS. AC", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickAC_to_DCASampleHoldModIn, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. BD
	piParam = new PluginParameter(controlID::joystickBD_to_DCASampleHoldModIn, "JS. BD", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickBD_to_DCASampleHoldModIn, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 1
	piParam = new PluginParameter(controlID::auxUnipolar1Mod_to_osc1Fo, "AUX U 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar1Mod_to_osc1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 1
	piParam = new PluginParameter(controlID::auxUnipolar1Mod_to_osc1Shape, "AUX U 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar1Mod_to_osc1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 1
	piParam = new PluginParameter(controlID::auxUnipolar1Mod_to_osc2Fo, "AUX U 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar1Mod_to_osc2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 1
	piParam = new PluginParameter(controlID::auxUnipolar1Mod_to_osc2Shape, "AUX U 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar1Mod_to_osc2Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 1
	piParam = new PluginParameter(controlID::auxUnipolar1Mod_to_filter1Fo, "AUX U 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar1Mod_to_filter1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 1
	piParam = new PluginParameter(controlID::auxUnipolar1Mod_to_filter1Q, "AUX U 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar1Mod_to_filter1Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 1
	piParam = new PluginParameter(controlID::auxUnipolar1Mod_to_filter2Fo, "AUX U 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar1Mod_to_filter2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 1
	piParam = new PluginParameter(controlID::auxUnipolar1Mod_to_filter2Q, "AUX U 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar1Mod_to_filter2Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 1
	piParam = new PluginParameter(controlID::auxUnipolar1Mod_to_DCASampleHoldModIn, "AUX U 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar1Mod_to_DCASampleHoldModIn, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 2
	piParam = new PluginParameter(controlID::auxUnipolar2Mod_to_osc1Fo, "AUX U 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar2Mod_to_osc1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 2
	piParam = new PluginParameter(controlID::auxUnipolar2Mod_to_osc1Shape, "AUX U 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar2Mod_to_osc1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 2
	piParam = new PluginParameter(controlID::auxUnipolar2Mod_to_osc2Fo, "AUX U 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar2Mod_to_osc2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 2
	piParam = new PluginParameter(controlID::auxUnipolar2Mod_to_osc2Shape, "AUX U 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar2Mod_to_osc2Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 2
	piParam = new PluginParameter(controlID::auxUnipolar2Mod_to_filter1Fo, "AUX U 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar2Mod_to_filter1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 2
	piParam = new PluginParameter(controlID::auxUnipolar2Mod_to_filter1Q, "AUX U 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar2Mod_to_filter1Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 2
	piParam = new PluginParameter(controlID::auxUnipolar2Mod_to_filter2Fo, "AUX U 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar2Mod_to_filter2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 2
	piParam = new PluginParameter(controlID::auxUnipolar2Mod_to_filter2Q, "AUX U 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar2Mod_to_filter2Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 2
	piParam = new PluginParameter(controlID::auxUnipolar2Mod_to_DCASampleHoldModIn, "AUX U 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar2Mod_to_DCASampleHoldModIn, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 3
	piParam = new PluginParameter(controlID::auxUnipolar3Mod_to_osc1Fo, "AUX U 3", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar3Mod_to_osc1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 3
	piParam = new PluginParameter(controlID::auxUnipolar3Mod_to_osc1Shape, "AUX U 3", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar3Mod_to_osc1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 3
	piParam = new PluginParameter(controlID::auxUnipolar3Mod_to_osc2Fo, "AUX U 3", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar3Mod_to_osc2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 3
	piParam = new PluginParameter(controlID::auxUnipolar3Mod_to_osc2Shape, "AUX U 3", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar3Mod_to_osc2Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 3
	piParam = new PluginParameter(controlID::auxUnipolar3Mod_to_filter1Fo, "AUX U 3", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar3Mod_to_filter1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 3
	piParam = new PluginParameter(controlID::auxUnipolar3Mod_to_filter1Q, "AUX U 3", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar3Mod_to_filter1Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 3
	piParam = new PluginParameter(controlID::auxUnipolar3Mod_to_filter2Fo, "AUX U 3", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar3Mod_to_filter2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 3
	piParam = new PluginParameter(controlID::auxUnipolar3Mod_to_filter2Q, "AUX U 3", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar3Mod_to_filter2Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 3
	piParam = new PluginParameter(controlID::auxUnipolar3Mod_to_DCASampleHoldModIn, "AUX U 3", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar3Mod_to_DCASampleHoldModIn, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 4
	piParam = new PluginParameter(controlID::auxUnipolar4Mod_to_osc1Fo, "AUX U 4", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar4Mod_to_osc1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 4
	piParam = new PluginParameter(controlID::auxUnipolar4Mod_to_osc1Shape, "AUX U 4", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar4Mod_to_osc1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 4
	piParam = new PluginParameter(controlID::auxUnipolar4Mod_to_osc2Fo, "AUX U 4", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar4Mod_to_osc2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 4
	piParam = new PluginParameter(controlID::auxUnipolar4Mod_to_osc2Shape, "AUX U 4", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar4Mod_to_osc2Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 4
	piParam = new PluginParameter(controlID::auxUnipolar4Mod_to_filter1Fo, "AUX U 4", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar4Mod_to_filter1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 4
	piParam = new PluginParameter(controlID::auxUnipolar4Mod_to_filter1Q, "AUX U 4", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar4Mod_to_filter1Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 4
	piParam = new PluginParameter(controlID::auxUnipolar4Mod_to_filter2Fo, "AUX U 4", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar4Mod_to_filter2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 4
	piParam = new PluginParameter(controlID::auxUnipolar4Mod_to_filter2Q, "AUX U 4", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar4Mod_to_filter2Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 4
	piParam = new PluginParameter(controlID::auxUnipolar4Mod_to_DCASampleHoldModIn, "AUX U 4", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar4Mod_to_DCASampleHoldModIn, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 5
	piParam = new PluginParameter(controlID::auxUnipolar5Mod_to_osc1Fo, "AUX U 5", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar5Mod_to_osc1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 5
	piParam = new PluginParameter(controlID::auxUnipolar5Mod_to_osc1Shape, "AUX U 5", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar5Mod_to_osc1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 5
	piParam = new PluginParameter(controlID::auxUnipolar5Mod_to_osc2Fo, "AUX U 5", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar5Mod_to_osc2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 5
	piParam = new PluginParameter(controlID::auxUnipolar5Mod_to_osc2Shape, "AUX U 5", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar5Mod_to_osc2Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 5
	piParam = new PluginParameter(controlID::auxUnipolar5Mod_to_filter1Fo, "AUX U 5", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar5Mod_to_filter1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 5
	piParam = new PluginParameter(controlID::auxUnipolar5Mod_to_filter1Q, "AUX U 5", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar5Mod_to_filter1Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 5
	piParam = new PluginParameter(controlID::auxUnipolar5Mod_to_filter2Fo, "AUX U 5", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar5Mod_to_filter2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 5
	piParam = new PluginParameter(controlID::auxUnipolar5Mod_to_filter2Q, "AUX U 5", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar5Mod_to_filter2Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 5
	piParam = new PluginParameter(controlID::auxUnipolar5Mod_to_DCASampleHoldModIn, "AUX U 5", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar5Mod_to_DCASampleHoldModIn, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 6
	piParam = new PluginParameter(controlID::auxUnipolar6Mod_to_osc1Fo, "AUX U 6", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar6Mod_to_osc1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 6
	piParam = new PluginParameter(controlID::auxUnipolar6Mod_to_osc1Shape, "AUX U 6", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar6Mod_to_osc1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 6
	piParam = new PluginParameter(controlID::auxUnipolar6Mod_to_osc2Fo, "AUX U 6", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar6Mod_to_osc2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 6
	piParam = new PluginParameter(controlID::auxUnipolar6Mod_to_osc2Shape, "AUX U 6", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar6Mod_to_osc2Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 6
	piParam = new PluginParameter(controlID::auxUnipolar6Mod_to_filter1Fo, "AUX U 6", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar6Mod_to_filter1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 6
	piParam = new PluginParameter(controlID::auxUnipolar6Mod_to_filter1Q, "AUX U 6", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar6Mod_to_filter1Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 6
	piParam = new PluginParameter(controlID::auxUnipolar6Mod_to_filter2Fo, "AUX U 6", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar6Mod_to_filter2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 6
	piParam = new PluginParameter(controlID::auxUnipolar6Mod_to_filter2Q, "AUX U 6", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar6Mod_to_filter2Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 6
	piParam = new PluginParameter(controlID::auxUnipolar6Mod_to_DCASampleHoldModIn, "AUX U 6", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar6Mod_to_DCASampleHoldModIn, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 1
	piParam = new PluginParameter(controlID::auxBipolar1Mod_to_osc1Fo, "AUX B 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar1Mod_to_osc1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 1
	piParam = new PluginParameter(controlID::auxBipolar1Mod_to_osc1Shape, "AUX B 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar1Mod_to_osc1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 1
	piParam = new PluginParameter(controlID::auxBipolar1Mod_to_osc2Fo, "AUX B 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar1Mod_to_osc2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 1
	piParam = new PluginParameter(controlID::auxBipolar1Mod_to_osc2Shape, "AUX B 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar1Mod_to_osc2Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 1
	piParam = new PluginParameter(controlID::auxBipolar1Mod_to_filter1Fo, "AUX B 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar1Mod_to_filter1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 1
	piParam = new PluginParameter(controlID::auxBipolar1Mod_to_filter1Q, "AUX B 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar1Mod_to_filter1Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 1
	piParam = new PluginParameter(controlID::auxBipolar1Mod_to_filter2Fo, "AUX B 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar1Mod_to_filter2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 1
	piParam = new PluginParameter(controlID::auxBipolar1Mod_to_filter2Q, "AUX B 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar1Mod_to_filter2Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 1
	piParam = new PluginParameter(controlID::auxBipolar1Mod_to_DCASampleHoldModIn, "AUX B 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar1Mod_to_DCASampleHoldModIn, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 2
	piParam = new PluginParameter(controlID::auxBipolar2Mod_to_osc1Fo, "AUX B 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar2Mod_to_osc1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 2
	piParam = new PluginParameter(controlID::auxBipolar2Mod_to_osc1Shape, "AUX B 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar2Mod_to_osc1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 2
	piParam = new PluginParameter(controlID::auxBipolar2Mod_to_osc2Fo, "AUX B 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar2Mod_to_osc2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 2
	piParam = new PluginParameter(controlID::auxBipolar2Mod_to_osc2Shape, "AUX B 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar2Mod_to_osc2Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 2
	piParam = new PluginParameter(controlID::auxBipolar2Mod_to_filter1Fo, "AUX B 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar2Mod_to_filter1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 2
	piParam = new PluginParameter(controlID::auxBipolar2Mod_to_filter1Q, "AUX B 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar2Mod_to_filter1Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 2
	piParam = new PluginParameter(controlID::auxBipolar2Mod_to_filter2Fo, "AUX B 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar2Mod_to_filter2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 2
	piParam = new PluginParameter(controlID::auxBipolar2Mod_to_filter2Q, "AUX B 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar2Mod_to_filter2Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 2
	piParam = new PluginParameter(controlID::auxBipolar2Mod_to_DCASampleHoldModIn, "AUX B 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar2Mod_to_DCASampleHoldModIn, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 3
	piParam = new PluginParameter(controlID::auxBipolar3Mod_to_osc1Fo, "AUX B 3", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar3Mod_to_osc1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 3
	piParam = new PluginParameter(controlID::auxBipolar3Mod_to_osc1Shape, "AUX B 3", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar3Mod_to_osc1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 3
	piParam = new PluginParameter(controlID::auxBipolar3Mod_to_osc2Fo, "AUX B 3", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar3Mod_to_osc2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 3
	piParam = new PluginParameter(controlID::auxBipolar3Mod_to_osc2Shape, "AUX B 3", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar3Mod_to_osc2Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 3
	piParam = new PluginParameter(controlID::auxBipolar3Mod_to_filter1Fo, "AUX B 3", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar3Mod_to_filter1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 3
	piParam = new PluginParameter(controlID::auxBipolar3Mod_to_filter1Q, "AUX B 3", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar3Mod_to_filter1Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 3
	piParam = new PluginParameter(controlID::auxBipolar3Mod_to_filter2Fo, "AUX B 3", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar3Mod_to_filter2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 3
	piParam = new PluginParameter(controlID::auxBipolar3Mod_to_filter2Q, "AUX B 3", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar3Mod_to_filter2Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 3
	piParam = new PluginParameter(controlID::auxBipolar3Mod_to_DCASampleHoldModIn, "AUX B 3", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar3Mod_to_DCASampleHoldModIn, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 4
	piParam = new PluginParameter(controlID::auxBipolar4Mod_to_osc1Fo, "AUX B 4", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar4Mod_to_osc1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 4
	piParam = new PluginParameter(controlID::auxBipolar4Mod_to_osc1Shape, "AUX B 4", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar4Mod_to_osc1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 4
	piParam = new PluginParameter(controlID::auxBipolar4Mod_to_osc2Fo, "AUX B 4", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar4Mod_to_osc2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 4
	piParam = new PluginParameter(controlID::auxBipolar4Mod_to_osc2Shape, "AUX B 4", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar4Mod_to_osc2Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 4
	piParam = new PluginParameter(controlID::auxBipolar4Mod_to_filter1Fo, "AUX B 4", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar4Mod_to_filter1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 4
	piParam = new PluginParameter(controlID::auxBipolar4Mod_to_filter1Q, "AUX B 4", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar4Mod_to_filter1Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 4
	piParam = new PluginParameter(controlID::auxBipolar4Mod_to_filter2Fo, "AUX B 4", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar4Mod_to_filter2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 4
	piParam = new PluginParameter(controlID::auxBipolar4Mod_to_filter2Q, "AUX B 4", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar4Mod_to_filter2Q, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 4
	piParam = new PluginParameter(controlID::auxBipolar4Mod_to_DCASampleHoldModIn, "AUX B 4", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar4Mod_to_DCASampleHoldModIn, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG1
	piParam = new PluginParameter(controlID::eg1_to_DCASampleHoldModIn, "EG1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg1_to_DCASampleHoldModIn, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG2
	piParam = new PluginParameter(controlID::eg2_to_DCASampleHoldModIn, "EG2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg2_to_DCASampleHoldModIn, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: LFO 1 Shape In
	piParam = new PluginParameter(controlID::lfo1ShapeModIn, "LFO 1 Shape In", "", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&lfo1ShapeModIn, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: LFO 1 ShapeSplit In
	piParam = new PluginParameter(controlID::lfo1ShapeSplitModIn, "LFO 1 ShapeSplit In", "", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&lfo1ShapeSplitModIn, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: LFO 2 Shape In
	piParam = new PluginParameter(controlID::lfo2ShapeModIn, "LFO 2 Shape In", "", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&lfo2ShapeModIn, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: LFO 2 ShapeSplit In
	piParam = new PluginParameter(controlID::lfo2ShapeSplitModIn, "LFO 2 ShapeSplit In", "", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&lfo2ShapeSplitModIn, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: EG 1 Attack In
	piParam = new PluginParameter(controlID::eg1AttackModIn, "EG 1 Attack In", "", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg1AttackModIn, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: EG 1 Hold In
	piParam = new PluginParameter(controlID::eg1HoldModIn, "EG 1 Hold In", "", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg1HoldModIn, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: EG 1 Decay In
	piParam = new PluginParameter(controlID::eg1DecayModIn, "EG 1 Decay In", "", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg1DecayModIn, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: EG 1 Sus In
	piParam = new PluginParameter(controlID::eg1SustainModIn, "EG 1 Sus In", "", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg1SustainModIn, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: EG 1 Rel In
	piParam = new PluginParameter(controlID::eg1ReleaseModIn, "EG 1 Rel In", "", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg1ReleaseModIn, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Unison Detune In
	piParam = new PluginParameter(controlID::unisonDetuneModIn, "Unison Detune In", "", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&unisonDetuneModIn, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: LFO 1
	piParam = new PluginParameter(controlID::lfo1_to_lfo1Shape, "LFO 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo1_to_lfo1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO 2
	piParam = new PluginParameter(controlID::lfo2_to_lfo1Shape, "LFO 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo2_to_lfo1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG 1
	piParam = new PluginParameter(controlID::eg1_to_lfo1Shape, "EG 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg1_to_lfo1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG 2
	piParam = new PluginParameter(controlID::eg2_to_lfo1Shape, "EG 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg2_to_lfo1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. AC
	piParam = new PluginParameter(controlID::joystickAC_to_lfo1Shape, "JS. AC", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickAC_to_lfo1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. BD
	piParam = new PluginParameter(controlID::joystickBD_to_lfo1Shape, "JS. BD", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickBD_to_lfo1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 1
	piParam = new PluginParameter(controlID::auxUnipolar1Mod_to_lfo1Shape, "AUX U 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar1Mod_to_lfo1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 2
	piParam = new PluginParameter(controlID::auxUnipolar2Mod_to_lfo1Shape, "AUX U 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar2Mod_to_lfo1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 3
	piParam = new PluginParameter(controlID::auxUnipolar3Mod_to_lfo1Shape, "AUX U 3", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar3Mod_to_lfo1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 4
	piParam = new PluginParameter(controlID::auxUnipolar4Mod_to_lfo1Shape, "AUX U 4", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar4Mod_to_lfo1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 5
	piParam = new PluginParameter(controlID::auxUnipolar5Mod_to_lfo1Shape, "AUX U 5", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar5Mod_to_lfo1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX U 6
	piParam = new PluginParameter(controlID::auxUnipolar6Mod_to_lfo1Shape, "AUX U 6", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxUnipolar6Mod_to_lfo1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 1
	piParam = new PluginParameter(controlID::auxBipolar1Mod_to_lfo1Shape, "AUX B 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar1Mod_to_lfo1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 2
	piParam = new PluginParameter(controlID::auxBipolar2Mod_to_lfo1Shape, "AUX B 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar2Mod_to_lfo1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 3
	piParam = new PluginParameter(controlID::auxBipolar3Mod_to_lfo1Shape, "AUX B 3", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar3Mod_to_lfo1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: AUX B 4
	piParam = new PluginParameter(controlID::auxBipolar4Mod_to_lfo1Shape, "AUX B 4", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&auxBipolar4Mod_to_lfo1Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO 1
	piParam = new PluginParameter(controlID::lfo1_to_unisonDetune, "LFO 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo1_to_unisonDetune, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO 2
	piParam = new PluginParameter(controlID::lfo2_to_unisonDetune, "LFO 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo2_to_unisonDetune, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG 1
	piParam = new PluginParameter(controlID::eg1_to_unisonDetune, "EG 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg1_to_unisonDetune, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG 2
	piParam = new PluginParameter(controlID::eg2_to_unisonDetune, "EG 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg2_to_unisonDetune, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. AC
	piParam = new PluginParameter(controlID::joystickAC_to_unisonDetune, "JS. AC", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickAC_to_unisonDetune, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. BD
	piParam = new PluginParameter(controlID::joystickBD_to_unisonDetune, "JS. BD", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickBD_to_unisonDetune, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO 1
	piParam = new PluginParameter(controlID::lfo1_to_lfo1ShapeSplit, "LFO 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo1_to_lfo1ShapeSplit, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO 2
	piParam = new PluginParameter(controlID::lfo2_to_lfo1ShapeSplit, "LFO 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo2_to_lfo1ShapeSplit, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG 1
	piParam = new PluginParameter(controlID::eg1_to_lfo1ShapeSplit, "EG 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg1_to_lfo1ShapeSplit, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG 2
	piParam = new PluginParameter(controlID::eg2_to_lfo1ShapeSplit, "EG 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg2_to_lfo1ShapeSplit, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. AC
	piParam = new PluginParameter(controlID::joystickAC_to_lfo1ShapeSplit, "JS. AC", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickAC_to_lfo1ShapeSplit, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. BD
	piParam = new PluginParameter(controlID::joystickBD_to_lfo1ShapeSplit, "JS. BD", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickBD_to_lfo1ShapeSplit, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO 1
	piParam = new PluginParameter(controlID::lfo1_to_lfo2Shape, "LFO 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo1_to_lfo2Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO 2
	piParam = new PluginParameter(controlID::lfo2_to_lfo2Shape, "LFO 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo2_to_lfo2Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG 1
	piParam = new PluginParameter(controlID::eg1_to_lfo2Shape, "EG 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg1_to_lfo2Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG 2
	piParam = new PluginParameter(controlID::eg2_to_lfo2Shape, "EG 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg2_to_lfo2Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. AC
	piParam = new PluginParameter(controlID::joystickAC_to_lfo2Shape, "JS. AC", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickAC_to_lfo2Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. BD
	piParam = new PluginParameter(controlID::joystickBD_to_lfo2Shape, "JS. BD", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickBD_to_lfo2Shape, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO 1
	piParam = new PluginParameter(controlID::lfo1_to_lfo2ShapeSplit, "LFO 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo1_to_lfo2ShapeSplit, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO 2
	piParam = new PluginParameter(controlID::lfo2_to_lfo2ShapeSplit, "LFO 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo2_to_lfo2ShapeSplit, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG 1
	piParam = new PluginParameter(controlID::eg1_to_lfo2ShapeSplit, "EG 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg1_to_lfo2ShapeSplit, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG 2
	piParam = new PluginParameter(controlID::eg2_to_lfo2ShapeSplit, "EG 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg2_to_lfo2ShapeSplit, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. AC
	piParam = new PluginParameter(controlID::joystickAC_to_lfo2ShapeSplit, "JS. AC", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickAC_to_lfo2ShapeSplit, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. BD
	piParam = new PluginParameter(controlID::joystickBD_to_lfo2ShapeSplit, "JS. BD", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickBD_to_lfo2ShapeSplit, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO 1
	piParam = new PluginParameter(controlID::lfo1_to_eg1Attack, "LFO 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo1_to_eg1Attack, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO 2
	piParam = new PluginParameter(controlID::lfo2_to_eg1Attack, "LFO 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo2_to_eg1Attack, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. AC
	piParam = new PluginParameter(controlID::joystickAC_to_eg1Attack, "JS. AC", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickAC_to_eg1Attack, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. BD
	piParam = new PluginParameter(controlID::joystickBD_to_eg1Attack, "JS. BD", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickBD_to_eg1Attack, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG 2
	piParam = new PluginParameter(controlID::eg2_to_eg1Attack, "EG 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg2_to_eg1Attack, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO 1
	piParam = new PluginParameter(controlID::lfo1_to_eg1Hold, "LFO 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo1_to_eg1Hold, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO 2
	piParam = new PluginParameter(controlID::lfo2_to_eg1Hold, "LFO 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo2_to_eg1Hold, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG 2
	piParam = new PluginParameter(controlID::eg2_to_eg1Hold, "EG 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg2_to_eg1Hold, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. AC
	piParam = new PluginParameter(controlID::joystickAC_to_eg1Hold, "JS. AC", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickAC_to_eg1Hold, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. BD
	piParam = new PluginParameter(controlID::joystickBD_to_eg1Hold, "JS. BD", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickBD_to_eg1Hold, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO 1
	piParam = new PluginParameter(controlID::lfo1_to_eg1Decay, "LFO 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo1_to_eg1Decay, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO 2
	piParam = new PluginParameter(controlID::lfo2_to_eg1Decay, "LFO 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo2_to_eg1Decay, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG 2
	piParam = new PluginParameter(controlID::eg2_to_eg1Decay, "EG 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg2_to_eg1Decay, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. AC
	piParam = new PluginParameter(controlID::joystickAC_to_eg1Decay, "JS. AC", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickAC_to_eg1Decay, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. BD
	piParam = new PluginParameter(controlID::joystickBD_to_eg1Decay, "JS. BD", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickBD_to_eg1Decay, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO 1
	piParam = new PluginParameter(controlID::lfo1_to_eg1Sustain, "LFO 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo1_to_eg1Sustain, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO 2
	piParam = new PluginParameter(controlID::lfo2_to_eg1Sustain, "LFO 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo2_to_eg1Sustain, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG 2
	piParam = new PluginParameter(controlID::eg2_to_eg1Sustain, "EG 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg2_to_eg1Sustain, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. AC
	piParam = new PluginParameter(controlID::joystickAC_to_eg1Sustain, "JS. AC", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickAC_to_eg1Sustain, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. BD
	piParam = new PluginParameter(controlID::joystickBD_to_eg1Sustain, "JS. BD", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickBD_to_eg1Sustain, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO 1
	piParam = new PluginParameter(controlID::lfo1_to_eg1Release, "LFO 1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo1_to_eg1Release, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: LFO 2
	piParam = new PluginParameter(controlID::lfo2_to_eg1Release, "LFO 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo2_to_eg1Release, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG 2
	piParam = new PluginParameter(controlID::eg2_to_eg1Release, "EG 2", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg2_to_eg1Release, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. AC
	piParam = new PluginParameter(controlID::joystickAC_to_eg1Release, "JS. AC", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickAC_to_eg1Release, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: JS. BD
	piParam = new PluginParameter(controlID::joystickBD_to_eg1Release, "JS. BD", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&joystickBD_to_eg1Release, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: Key Selection
	piParam = new PluginParameter(controlID::temperamentKeySelection, "Key Selection", "", controlVariableType::kInt, 0.000000, 11.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&temperamentKeySelection, boundVariableType::kInt);
	addPluginParameter(piParam);

	// --- discrete control: Osc1 Model
	piParam = new PluginParameter(controlID::osc1ExciterMode, "Osc1 Model", "None,Noise,Waveform", "None");
	piParam->setBoundVariable(&osc1ExciterMode, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: Exciter1 Attack
	piParam = new PluginParameter(controlID::exciter1Attack_msec, "Exciter1 Attack", "mSec", controlVariableType::kDouble, 0.000000, 500.000000, 100.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&exciter1Attack_msec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Exciter1 Hold
	piParam = new PluginParameter(controlID::exciter1Hold_msec, "Exciter1 Hold", "msec", controlVariableType::kDouble, 0.000000, 5000.000000, 250.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&exciter1Hold_msec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Exciter 1 Release
	piParam = new PluginParameter(controlID::exciter1Release_msec, "Exciter 1 Release", "mSec", controlVariableType::kDouble, 0.000000, 5000.000000, 1000.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&exciter1Release_msec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Tube 1 Decay
	piParam = new PluginParameter(controlID::resonator1Decay, "Tube 1 Decay", "", controlVariableType::kDouble, 0.750000, 1.000000, 1.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&resonator1Decay, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: Osc2 Model
	piParam = new PluginParameter(controlID::osc2ExciterMode, "Osc2 Model", "None,Noise,Waveform", "None");
	piParam->setBoundVariable(&osc2ExciterMode, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Osc3 Model
	piParam = new PluginParameter(controlID::osc3ExciterMode, "Osc3 Model", "None,Noise,Waveform", "None");
	piParam->setBoundVariable(&osc3ExciterMode, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Osc4 Model
	piParam = new PluginParameter(controlID::osc4ExciterMode, "Osc4 Model", "None,Noise,Waveform", "None");
	piParam->setBoundVariable(&osc4ExciterMode, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: Exciter2 Attack
	piParam = new PluginParameter(controlID::exciter2Attack_msec, "Exciter2 Attack", "mSec", controlVariableType::kDouble, 0.000000, 500.000000, 100.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&exciter2Attack_msec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Exciter2 Hold
	piParam = new PluginParameter(controlID::exciter2Hold_msec, "Exciter2 Hold", "msec", controlVariableType::kDouble, 0.000000, 5000.000000, 250.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&exciter2Hold_msec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Exciter2 Release
	piParam = new PluginParameter(controlID::exciter2Release_msec, "Exciter2 Release", "mSec", controlVariableType::kDouble, 0.000000, 5000.000000, 1000.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&exciter2Release_msec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Tube 2 Decay
	piParam = new PluginParameter(controlID::resonator2Decay, "Tube 2 Decay", "", controlVariableType::kDouble, 0.750000, 1.000000, 1.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&resonator2Decay, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Exciter3 Attack
	piParam = new PluginParameter(controlID::exciter3Attack_msec, "Exciter3 Attack", "mSec", controlVariableType::kDouble, 0.000000, 500.000000, 100.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&exciter3Attack_msec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Exciter3 Hold
	piParam = new PluginParameter(controlID::exciter3Hold_msec, "Exciter3 Hold", "msec", controlVariableType::kDouble, 0.000000, 5000.000000, 250.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&exciter3Hold_msec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Exciter3 Release
	piParam = new PluginParameter(controlID::exciter3Release_msec, "Exciter3 Release", "mSec", controlVariableType::kDouble, 0.000000, 5000.000000, 1000.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&exciter3Release_msec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Tube 3 Decay
	piParam = new PluginParameter(controlID::resonator3Decay, "Tube 3 Decay", "", controlVariableType::kDouble, 0.750000, 1.000000, 1.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&resonator3Decay, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Exciter4 Attack
	piParam = new PluginParameter(controlID::exciter4Attack_msec, "Exciter4 Attack", "mSec", controlVariableType::kDouble, 0.000000, 500.000000, 100.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&exciter4Attack_msec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Exciter4 Hold
	piParam = new PluginParameter(controlID::exciter4Hold_msec, "Exciter4 Hold", "msec", controlVariableType::kDouble, 0.000000, 5000.000000, 250.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&exciter4Hold_msec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Exciter4 Release
	piParam = new PluginParameter(controlID::exciter4Release_msec, "Exciter4 Release", "mSec", controlVariableType::kDouble, 0.000000, 5000.000000, 1000.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&exciter4Release_msec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Tube 4 Decay
	piParam = new PluginParameter(controlID::resonator4Decay, "Tube 4 Decay", "", controlVariableType::kDouble, 0.750000, 1.000000, 1.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&resonator4Decay, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: FC 2
	piParam = new PluginParameter(controlID::fc2_hertz, "FC 2", "Hertz", controlVariableType::kDouble, 20.000000, 18000.000000, 18000.000000, taper::kVoltOctaveTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&fc2_hertz, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Q 2
	piParam = new PluginParameter(controlID::q2Control, "Q 2", "", controlVariableType::kDouble, 1.000000, 10.000000, 1.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&q2Control, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: Filter 1 Algorithm
	piParam = new PluginParameter(controlID::filter1Algorithm, "Filter 1 Algorithm", "LPF2,HPF2,LPF4,HPF4", "LPF2");
	piParam->setBoundVariable(&filter1Algorithm, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Filter 2 Algorithm
	piParam = new PluginParameter(controlID::filter2Algorithm, "Filter 2 Algorithm", "LPF2,HPF2,LPF4,HPF4", "LPF2");
	piParam->setBoundVariable(&filter2Algorithm, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Gain Comp
	piParam = new PluginParameter(controlID::enableGainCompensation, "Gain Comp", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&enableGainCompensation, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- Aux Attributes
	AuxParameterAttribute auxAttribute;

	// --- RAFX GUI attributes
	// --- controlID::lfo1Waveform
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::lfo1Waveform, auxAttribute);

	// --- controlID::lfo1Mode
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::lfo1Mode, auxAttribute);

	// --- controlID::lfo1Frequency_Hz
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::lfo1Frequency_Hz, auxAttribute);

	// --- controlID::masterPitchBend
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::masterPitchBend, auxAttribute);

	// --- controlID::masterTune
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::masterTune, auxAttribute);

	// --- controlID::masterVolume_dB
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::masterVolume_dB, auxAttribute);

	// --- controlID::lfo1DelayTime_mSec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::lfo1DelayTime_mSec, auxAttribute);

	// --- controlID::lfo1RampTime_mSec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::lfo1RampTime_mSec, auxAttribute);

	// --- controlID::lfo2Mode
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::lfo2Mode, auxAttribute);

	// --- controlID::lfo2Waveform
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::lfo2Waveform, auxAttribute);

	// --- controlID::lfo2Frequency_Hz
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::lfo2Frequency_Hz, auxAttribute);

	// --- controlID::lfo1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::lfo1Shape, auxAttribute);

	// --- controlID::lfo1ShapeSplit
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::lfo1ShapeSplit, auxAttribute);

	// --- controlID::ScaleMode
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::ScaleMode, auxAttribute);

	// --- controlID::enableRhythmicBreaks
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741824);
	setParamAuxAttribute(controlID::enableRhythmicBreaks, auxAttribute);

	// --- controlID::lfo2Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::lfo2Shape, auxAttribute);

	// --- controlID::lfo2ShapeSplit
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::lfo2ShapeSplit, auxAttribute);

	// --- controlID::lfo2ModTarget
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::lfo2ModTarget, auxAttribute);

	// --- controlID::osc1Waveform
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::osc1Waveform, auxAttribute);

	// --- controlID::osc1BankIndex
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::osc1BankIndex, auxAttribute);

	// --- controlID::osc2Waveform
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::osc2Waveform, auxAttribute);

	// --- controlID::osc3Waveform
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::osc3Waveform, auxAttribute);

	// --- controlID::osc4Waveform
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::osc4Waveform, auxAttribute);

	// --- controlID::osc2BankIndex
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::osc2BankIndex, auxAttribute);

	// --- controlID::osc3BankIndex
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::osc3BankIndex, auxAttribute);

	// --- controlID::osc4BankIndex
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::osc4BankIndex, auxAttribute);

	// --- controlID::osc1Detune_cents
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::osc1Detune_cents, auxAttribute);

	// --- controlID::fc1_hertz
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::fc1_hertz, auxAttribute);

	// --- controlID::q1Control
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::q1Control, auxAttribute);

	// --- controlID::lfo1Amplitude
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::lfo1Amplitude, auxAttribute);

	// --- controlID::osc1FoModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::osc1FoModIn, auxAttribute);

	// --- controlID::enableKeyTrack
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741824);
	setParamAuxAttribute(controlID::enableKeyTrack, auxAttribute);

	// --- controlID::keyTrackRatio
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::keyTrackRatio, auxAttribute);

	// --- controlID::lfo1_to_osc1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo1_to_osc1Fo, auxAttribute);

	// --- controlID::eg2DelayTime_mSec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg2DelayTime_mSec, auxAttribute);

	// --- controlID::eg2AttackTime_mSec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg2AttackTime_mSec, auxAttribute);

	// --- controlID::eg2HoldTime_mSec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg2HoldTime_mSec, auxAttribute);

	// --- controlID::eg2DecayTime_mSec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg2DecayTime_mSec, auxAttribute);

	// --- controlID::eg2SustainLevel
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg2SustainLevel, auxAttribute);

	// --- controlID::eg2ReleaseTime_mSec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg2ReleaseTime_mSec, auxAttribute);

	// --- controlID::eg2AutoTrigger
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741824);
	setParamAuxAttribute(controlID::eg2AutoTrigger, auxAttribute);

	// --- controlID::eg2ManualTrigger
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1610612736);
	setParamAuxAttribute(controlID::eg2ManualTrigger, auxAttribute);

	// --- controlID::eg1DelayTime_mSec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg1DelayTime_mSec, auxAttribute);

	// --- controlID::eg1AttackTime_mSec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg1AttackTime_mSec, auxAttribute);

	// --- controlID::eg1HoldTime_mSec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg1HoldTime_mSec, auxAttribute);

	// --- controlID::eg1DecayTime_mSec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg1DecayTime_mSec, auxAttribute);

	// --- controlID::eg1SustainLevel
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg1SustainLevel, auxAttribute);

	// --- controlID::eg1ReleaseTime_mSec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg1ReleaseTime_mSec, auxAttribute);

	// --- controlID::eg1AutoRetrigger
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741824);
	setParamAuxAttribute(controlID::eg1AutoRetrigger, auxAttribute);

	// --- controlID::eg1ManualTrigger
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1610612736);
	setParamAuxAttribute(controlID::eg1ManualTrigger, auxAttribute);

	// --- controlID::eg1Offset
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg1Offset, auxAttribute);

	// --- controlID::eg2Offset
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg2Offset, auxAttribute);

	// --- controlID::eg1Mode
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::eg1Mode, auxAttribute);

	// --- controlID::eg2Mode
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::eg2Mode, auxAttribute);

	// --- controlID::mode
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::mode, auxAttribute);

	// --- controlID::unisonDetune_Cents
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::unisonDetune_Cents, auxAttribute);

	// --- controlID::eg1ModOut
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg1ModOut, auxAttribute);

	// --- controlID::eg1_to_osc1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg1_to_osc1Fo, auxAttribute);

	// --- controlID::lfo2Amplitude
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::lfo2Amplitude, auxAttribute);

	// --- controlID::eg2ModOut
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg2ModOut, auxAttribute);

	// --- controlID::joystickACModOut
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::joystickACModOut, auxAttribute);

	// --- controlID::joystickBDModOut
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::joystickBDModOut, auxAttribute);

	// --- controlID::lfo1_to_osc2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo1_to_osc2Fo, auxAttribute);

	// --- controlID::osc2FoModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::osc2FoModIn, auxAttribute);

	// --- controlID::osc1ShapeModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::osc1ShapeModIn, auxAttribute);

	// --- controlID::osc2ShapeModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::osc2ShapeModIn, auxAttribute);

	// --- controlID::filter1FoModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::filter1FoModIn, auxAttribute);

	// --- controlID::filter1QModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::filter1QModIn, auxAttribute);

	// --- controlID::filter2FoModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::filter2FoModIn, auxAttribute);

	// --- controlID::filter2QModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::filter2QModIn, auxAttribute);

	// --- controlID::lfo1_to_osc1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo1_to_osc1Shape, auxAttribute);

	// --- controlID::lfo1_to_osc2Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo1_to_osc2Shape, auxAttribute);

	// --- controlID::lfo1_to_filter1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo1_to_filter1Fo, auxAttribute);

	// --- controlID::lfo1_to_filter1Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo1_to_filter1Q, auxAttribute);

	// --- controlID::lfo1_to_filter2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo1_to_filter2Fo, auxAttribute);

	// --- controlID::lfo1_to_filter2Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo1_to_filter2Q, auxAttribute);

	// --- controlID::lfo2_to_osc1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo2_to_osc1Fo, auxAttribute);

	// --- controlID::lfo2_to_osc1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo2_to_osc1Shape, auxAttribute);

	// --- controlID::lfo2_to_osc2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo2_to_osc2Fo, auxAttribute);

	// --- controlID::lfo2_to_osc2Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo2_to_osc2Shape, auxAttribute);

	// --- controlID::lfo2_to_filter1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo2_to_filter1Fo, auxAttribute);

	// --- controlID::lfo2_to_filter1Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo2_to_filter1Q, auxAttribute);

	// --- controlID::lfo2_to_filter2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo2_to_filter2Fo, auxAttribute);

	// --- controlID::lfo2_to_filter2Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo2_to_filter2Q, auxAttribute);

	// --- controlID::eg1_to_osc1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg1_to_osc1Shape, auxAttribute);

	// --- controlID::eg1_to_osc2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg1_to_osc2Fo, auxAttribute);

	// --- controlID::eg1_to_osc2Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg1_to_osc2Shape, auxAttribute);

	// --- controlID::eg1_to_filter1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg1_to_filter1Fo, auxAttribute);

	// --- controlID::eg1_to_filter1Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg1_to_filter1Q, auxAttribute);

	// --- controlID::eg1_to_filter2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg1_to_filter2Fo, auxAttribute);

	// --- controlID::eg1_to_filter2Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg1_to_filter2Q, auxAttribute);

	// --- controlID::eg2_to_osc1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg2_to_osc1Fo, auxAttribute);

	// --- controlID::eg2_to_osc1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg2_to_osc1Shape, auxAttribute);

	// --- controlID::eg2_to_osc2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg2_to_osc2Fo, auxAttribute);

	// --- controlID::eg2_to_osc2Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg2_to_osc2Shape, auxAttribute);

	// --- controlID::eg2_to_filter1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg2_to_filter1Fo, auxAttribute);

	// --- controlID::eg2_to_filter1Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg2_to_filter1Q, auxAttribute);

	// --- controlID::eg2_to_filter2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg2_to_filter2Fo, auxAttribute);

	// --- controlID::eg2_to_filter2Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg2_to_filter2Q, auxAttribute);

	// --- controlID::joystickAC_to_osc1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickAC_to_osc1Fo, auxAttribute);

	// --- controlID::joystickAC_to_osc1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickAC_to_osc1Shape, auxAttribute);

	// --- controlID::joystickAC_to_osc2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickAC_to_osc2Fo, auxAttribute);

	// --- controlID::joystickAC_to_osc2Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickAC_to_osc2Shape, auxAttribute);

	// --- controlID::joystickAC_to_filter1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickAC_to_filter1Fo, auxAttribute);

	// --- controlID::joystickAC_to_filter1Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickAC_to_filter1Q, auxAttribute);

	// --- controlID::joystickAC_to_filter2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickAC_to_filter2Fo, auxAttribute);

	// --- controlID::joystickAC_to_filter2Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickAC_to_filter2Q, auxAttribute);

	// --- controlID::joystickBD_to_osc1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickBD_to_osc1Fo, auxAttribute);

	// --- controlID::joystickBD_to_osc1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickBD_to_osc1Shape, auxAttribute);

	// --- controlID::joystickBD_to_osc2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickBD_to_osc2Fo, auxAttribute);

	// --- controlID::joystickBD_to_osc2Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickBD_to_osc2Shape, auxAttribute);

	// --- controlID::joystickBD_to_filter1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickBD_to_filter1Fo, auxAttribute);

	// --- controlID::joystickBD_to_filter1Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickBD_to_filter1Q, auxAttribute);

	// --- controlID::joystickBD_to_filter2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickBD_to_filter2Fo, auxAttribute);

	// --- controlID::joystickBD_to_filter2Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickBD_to_filter2Q, auxAttribute);

	// --- controlID::auxUnipolar1ModOut
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::auxUnipolar1ModOut, auxAttribute);

	// --- controlID::auxUnipolar2ModOut
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::auxUnipolar2ModOut, auxAttribute);

	// --- controlID::auxUnipolar3ModOut
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::auxUnipolar3ModOut, auxAttribute);

	// --- controlID::auxUnipolar4ModOut
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::auxUnipolar4ModOut, auxAttribute);

	// --- controlID::auxUnipolar5ModOut
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::auxUnipolar5ModOut, auxAttribute);

	// --- controlID::auxUnipolar6ModOut
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::auxUnipolar6ModOut, auxAttribute);

	// --- controlID::auxBipolar1ModOut
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::auxBipolar1ModOut, auxAttribute);

	// --- controlID::auxBipolar2ModOut
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::auxBipolar2ModOut, auxAttribute);

	// --- controlID::auxBipolar3ModOut
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::auxBipolar3ModOut, auxAttribute);

	// --- controlID::auxBipolar4ModOut
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::auxBipolar4ModOut, auxAttribute);

	// --- controlID::DCASampleHoldModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::DCASampleHoldModIn, auxAttribute);

	// --- controlID::lfo1_to_DCASampleHoldModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo1_to_DCASampleHoldModIn, auxAttribute);

	// --- controlID::lfo2_to_DCASampleHoldModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo2_to_DCASampleHoldModIn, auxAttribute);

	// --- controlID::joystickAC_to_DCASampleHoldModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickAC_to_DCASampleHoldModIn, auxAttribute);

	// --- controlID::joystickBD_to_DCASampleHoldModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickBD_to_DCASampleHoldModIn, auxAttribute);

	// --- controlID::auxUnipolar1Mod_to_osc1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar1Mod_to_osc1Fo, auxAttribute);

	// --- controlID::auxUnipolar1Mod_to_osc1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar1Mod_to_osc1Shape, auxAttribute);

	// --- controlID::auxUnipolar1Mod_to_osc2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar1Mod_to_osc2Fo, auxAttribute);

	// --- controlID::auxUnipolar1Mod_to_osc2Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar1Mod_to_osc2Shape, auxAttribute);

	// --- controlID::auxUnipolar1Mod_to_filter1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar1Mod_to_filter1Fo, auxAttribute);

	// --- controlID::auxUnipolar1Mod_to_filter1Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar1Mod_to_filter1Q, auxAttribute);

	// --- controlID::auxUnipolar1Mod_to_filter2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar1Mod_to_filter2Fo, auxAttribute);

	// --- controlID::auxUnipolar1Mod_to_filter2Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar1Mod_to_filter2Q, auxAttribute);

	// --- controlID::auxUnipolar1Mod_to_DCASampleHoldModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar1Mod_to_DCASampleHoldModIn, auxAttribute);

	// --- controlID::auxUnipolar2Mod_to_osc1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar2Mod_to_osc1Fo, auxAttribute);

	// --- controlID::auxUnipolar2Mod_to_osc1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar2Mod_to_osc1Shape, auxAttribute);

	// --- controlID::auxUnipolar2Mod_to_osc2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar2Mod_to_osc2Fo, auxAttribute);

	// --- controlID::auxUnipolar2Mod_to_osc2Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar2Mod_to_osc2Shape, auxAttribute);

	// --- controlID::auxUnipolar2Mod_to_filter1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar2Mod_to_filter1Fo, auxAttribute);

	// --- controlID::auxUnipolar2Mod_to_filter1Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar2Mod_to_filter1Q, auxAttribute);

	// --- controlID::auxUnipolar2Mod_to_filter2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar2Mod_to_filter2Fo, auxAttribute);

	// --- controlID::auxUnipolar2Mod_to_filter2Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar2Mod_to_filter2Q, auxAttribute);

	// --- controlID::auxUnipolar2Mod_to_DCASampleHoldModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar2Mod_to_DCASampleHoldModIn, auxAttribute);

	// --- controlID::auxUnipolar3Mod_to_osc1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar3Mod_to_osc1Fo, auxAttribute);

	// --- controlID::auxUnipolar3Mod_to_osc1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar3Mod_to_osc1Shape, auxAttribute);

	// --- controlID::auxUnipolar3Mod_to_osc2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar3Mod_to_osc2Fo, auxAttribute);

	// --- controlID::auxUnipolar3Mod_to_osc2Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar3Mod_to_osc2Shape, auxAttribute);

	// --- controlID::auxUnipolar3Mod_to_filter1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar3Mod_to_filter1Fo, auxAttribute);

	// --- controlID::auxUnipolar3Mod_to_filter1Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar3Mod_to_filter1Q, auxAttribute);

	// --- controlID::auxUnipolar3Mod_to_filter2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar3Mod_to_filter2Fo, auxAttribute);

	// --- controlID::auxUnipolar3Mod_to_filter2Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar3Mod_to_filter2Q, auxAttribute);

	// --- controlID::auxUnipolar3Mod_to_DCASampleHoldModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar3Mod_to_DCASampleHoldModIn, auxAttribute);

	// --- controlID::auxUnipolar4Mod_to_osc1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar4Mod_to_osc1Fo, auxAttribute);

	// --- controlID::auxUnipolar4Mod_to_osc1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar4Mod_to_osc1Shape, auxAttribute);

	// --- controlID::auxUnipolar4Mod_to_osc2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar4Mod_to_osc2Fo, auxAttribute);

	// --- controlID::auxUnipolar4Mod_to_osc2Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar4Mod_to_osc2Shape, auxAttribute);

	// --- controlID::auxUnipolar4Mod_to_filter1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar4Mod_to_filter1Fo, auxAttribute);

	// --- controlID::auxUnipolar4Mod_to_filter1Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar4Mod_to_filter1Q, auxAttribute);

	// --- controlID::auxUnipolar4Mod_to_filter2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar4Mod_to_filter2Fo, auxAttribute);

	// --- controlID::auxUnipolar4Mod_to_filter2Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar4Mod_to_filter2Q, auxAttribute);

	// --- controlID::auxUnipolar4Mod_to_DCASampleHoldModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar4Mod_to_DCASampleHoldModIn, auxAttribute);

	// --- controlID::auxUnipolar5Mod_to_osc1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar5Mod_to_osc1Fo, auxAttribute);

	// --- controlID::auxUnipolar5Mod_to_osc1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar5Mod_to_osc1Shape, auxAttribute);

	// --- controlID::auxUnipolar5Mod_to_osc2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar5Mod_to_osc2Fo, auxAttribute);

	// --- controlID::auxUnipolar5Mod_to_osc2Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar5Mod_to_osc2Shape, auxAttribute);

	// --- controlID::auxUnipolar5Mod_to_filter1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar5Mod_to_filter1Fo, auxAttribute);

	// --- controlID::auxUnipolar5Mod_to_filter1Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar5Mod_to_filter1Q, auxAttribute);

	// --- controlID::auxUnipolar5Mod_to_filter2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar5Mod_to_filter2Fo, auxAttribute);

	// --- controlID::auxUnipolar5Mod_to_filter2Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar5Mod_to_filter2Q, auxAttribute);

	// --- controlID::auxUnipolar5Mod_to_DCASampleHoldModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar5Mod_to_DCASampleHoldModIn, auxAttribute);

	// --- controlID::auxUnipolar6Mod_to_osc1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar6Mod_to_osc1Fo, auxAttribute);

	// --- controlID::auxUnipolar6Mod_to_osc1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar6Mod_to_osc1Shape, auxAttribute);

	// --- controlID::auxUnipolar6Mod_to_osc2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar6Mod_to_osc2Fo, auxAttribute);

	// --- controlID::auxUnipolar6Mod_to_osc2Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar6Mod_to_osc2Shape, auxAttribute);

	// --- controlID::auxUnipolar6Mod_to_filter1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar6Mod_to_filter1Fo, auxAttribute);

	// --- controlID::auxUnipolar6Mod_to_filter1Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar6Mod_to_filter1Q, auxAttribute);

	// --- controlID::auxUnipolar6Mod_to_filter2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar6Mod_to_filter2Fo, auxAttribute);

	// --- controlID::auxUnipolar6Mod_to_filter2Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar6Mod_to_filter2Q, auxAttribute);

	// --- controlID::auxUnipolar6Mod_to_DCASampleHoldModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar6Mod_to_DCASampleHoldModIn, auxAttribute);

	// --- controlID::auxBipolar1Mod_to_osc1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar1Mod_to_osc1Fo, auxAttribute);

	// --- controlID::auxBipolar1Mod_to_osc1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar1Mod_to_osc1Shape, auxAttribute);

	// --- controlID::auxBipolar1Mod_to_osc2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar1Mod_to_osc2Fo, auxAttribute);

	// --- controlID::auxBipolar1Mod_to_osc2Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar1Mod_to_osc2Shape, auxAttribute);

	// --- controlID::auxBipolar1Mod_to_filter1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar1Mod_to_filter1Fo, auxAttribute);

	// --- controlID::auxBipolar1Mod_to_filter1Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar1Mod_to_filter1Q, auxAttribute);

	// --- controlID::auxBipolar1Mod_to_filter2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar1Mod_to_filter2Fo, auxAttribute);

	// --- controlID::auxBipolar1Mod_to_filter2Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar1Mod_to_filter2Q, auxAttribute);

	// --- controlID::auxBipolar1Mod_to_DCASampleHoldModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar1Mod_to_DCASampleHoldModIn, auxAttribute);

	// --- controlID::auxBipolar2Mod_to_osc1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar2Mod_to_osc1Fo, auxAttribute);

	// --- controlID::auxBipolar2Mod_to_osc1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar2Mod_to_osc1Shape, auxAttribute);

	// --- controlID::auxBipolar2Mod_to_osc2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar2Mod_to_osc2Fo, auxAttribute);

	// --- controlID::auxBipolar2Mod_to_osc2Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar2Mod_to_osc2Shape, auxAttribute);

	// --- controlID::auxBipolar2Mod_to_filter1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar2Mod_to_filter1Fo, auxAttribute);

	// --- controlID::auxBipolar2Mod_to_filter1Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar2Mod_to_filter1Q, auxAttribute);

	// --- controlID::auxBipolar2Mod_to_filter2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar2Mod_to_filter2Fo, auxAttribute);

	// --- controlID::auxBipolar2Mod_to_filter2Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar2Mod_to_filter2Q, auxAttribute);

	// --- controlID::auxBipolar2Mod_to_DCASampleHoldModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar2Mod_to_DCASampleHoldModIn, auxAttribute);

	// --- controlID::auxBipolar3Mod_to_osc1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar3Mod_to_osc1Fo, auxAttribute);

	// --- controlID::auxBipolar3Mod_to_osc1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar3Mod_to_osc1Shape, auxAttribute);

	// --- controlID::auxBipolar3Mod_to_osc2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar3Mod_to_osc2Fo, auxAttribute);

	// --- controlID::auxBipolar3Mod_to_osc2Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar3Mod_to_osc2Shape, auxAttribute);

	// --- controlID::auxBipolar3Mod_to_filter1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar3Mod_to_filter1Fo, auxAttribute);

	// --- controlID::auxBipolar3Mod_to_filter1Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar3Mod_to_filter1Q, auxAttribute);

	// --- controlID::auxBipolar3Mod_to_filter2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar3Mod_to_filter2Fo, auxAttribute);

	// --- controlID::auxBipolar3Mod_to_filter2Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar3Mod_to_filter2Q, auxAttribute);

	// --- controlID::auxBipolar3Mod_to_DCASampleHoldModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar3Mod_to_DCASampleHoldModIn, auxAttribute);

	// --- controlID::auxBipolar4Mod_to_osc1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar4Mod_to_osc1Fo, auxAttribute);

	// --- controlID::auxBipolar4Mod_to_osc1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar4Mod_to_osc1Shape, auxAttribute);

	// --- controlID::auxBipolar4Mod_to_osc2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar4Mod_to_osc2Fo, auxAttribute);

	// --- controlID::auxBipolar4Mod_to_osc2Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar4Mod_to_osc2Shape, auxAttribute);

	// --- controlID::auxBipolar4Mod_to_filter1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar4Mod_to_filter1Fo, auxAttribute);

	// --- controlID::auxBipolar4Mod_to_filter1Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar4Mod_to_filter1Q, auxAttribute);

	// --- controlID::auxBipolar4Mod_to_filter2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar4Mod_to_filter2Fo, auxAttribute);

	// --- controlID::auxBipolar4Mod_to_filter2Q
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar4Mod_to_filter2Q, auxAttribute);

	// --- controlID::auxBipolar4Mod_to_DCASampleHoldModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar4Mod_to_DCASampleHoldModIn, auxAttribute);

	// --- controlID::eg1_to_DCASampleHoldModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg1_to_DCASampleHoldModIn, auxAttribute);

	// --- controlID::eg2_to_DCASampleHoldModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg2_to_DCASampleHoldModIn, auxAttribute);

	// --- controlID::lfo1ShapeModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::lfo1ShapeModIn, auxAttribute);

	// --- controlID::lfo1ShapeSplitModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::lfo1ShapeSplitModIn, auxAttribute);

	// --- controlID::lfo2ShapeModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::lfo2ShapeModIn, auxAttribute);

	// --- controlID::lfo2ShapeSplitModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::lfo2ShapeSplitModIn, auxAttribute);

	// --- controlID::eg1AttackModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg1AttackModIn, auxAttribute);

	// --- controlID::eg1HoldModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg1HoldModIn, auxAttribute);

	// --- controlID::eg1DecayModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg1DecayModIn, auxAttribute);

	// --- controlID::eg1SustainModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg1SustainModIn, auxAttribute);

	// --- controlID::eg1ReleaseModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg1ReleaseModIn, auxAttribute);

	// --- controlID::unisonDetuneModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::unisonDetuneModIn, auxAttribute);

	// --- controlID::lfo1_to_lfo1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo1_to_lfo1Shape, auxAttribute);

	// --- controlID::lfo2_to_lfo1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo2_to_lfo1Shape, auxAttribute);

	// --- controlID::eg1_to_lfo1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg1_to_lfo1Shape, auxAttribute);

	// --- controlID::eg2_to_lfo1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg2_to_lfo1Shape, auxAttribute);

	// --- controlID::joystickAC_to_lfo1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickAC_to_lfo1Shape, auxAttribute);

	// --- controlID::joystickBD_to_lfo1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickBD_to_lfo1Shape, auxAttribute);

	// --- controlID::auxUnipolar1Mod_to_lfo1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar1Mod_to_lfo1Shape, auxAttribute);

	// --- controlID::auxUnipolar2Mod_to_lfo1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar2Mod_to_lfo1Shape, auxAttribute);

	// --- controlID::auxUnipolar3Mod_to_lfo1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar3Mod_to_lfo1Shape, auxAttribute);

	// --- controlID::auxUnipolar4Mod_to_lfo1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar4Mod_to_lfo1Shape, auxAttribute);

	// --- controlID::auxUnipolar5Mod_to_lfo1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar5Mod_to_lfo1Shape, auxAttribute);

	// --- controlID::auxUnipolar6Mod_to_lfo1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxUnipolar6Mod_to_lfo1Shape, auxAttribute);

	// --- controlID::auxBipolar1Mod_to_lfo1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar1Mod_to_lfo1Shape, auxAttribute);

	// --- controlID::auxBipolar2Mod_to_lfo1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar2Mod_to_lfo1Shape, auxAttribute);

	// --- controlID::auxBipolar3Mod_to_lfo1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar3Mod_to_lfo1Shape, auxAttribute);

	// --- controlID::auxBipolar4Mod_to_lfo1Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::auxBipolar4Mod_to_lfo1Shape, auxAttribute);

	// --- controlID::lfo1_to_unisonDetune
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo1_to_unisonDetune, auxAttribute);

	// --- controlID::lfo2_to_unisonDetune
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo2_to_unisonDetune, auxAttribute);

	// --- controlID::eg1_to_unisonDetune
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg1_to_unisonDetune, auxAttribute);

	// --- controlID::eg2_to_unisonDetune
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg2_to_unisonDetune, auxAttribute);

	// --- controlID::joystickAC_to_unisonDetune
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickAC_to_unisonDetune, auxAttribute);

	// --- controlID::joystickBD_to_unisonDetune
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickBD_to_unisonDetune, auxAttribute);

	// --- controlID::lfo1_to_lfo1ShapeSplit
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo1_to_lfo1ShapeSplit, auxAttribute);

	// --- controlID::lfo2_to_lfo1ShapeSplit
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo2_to_lfo1ShapeSplit, auxAttribute);

	// --- controlID::eg1_to_lfo1ShapeSplit
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg1_to_lfo1ShapeSplit, auxAttribute);

	// --- controlID::eg2_to_lfo1ShapeSplit
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg2_to_lfo1ShapeSplit, auxAttribute);

	// --- controlID::joystickAC_to_lfo1ShapeSplit
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickAC_to_lfo1ShapeSplit, auxAttribute);

	// --- controlID::joystickBD_to_lfo1ShapeSplit
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickBD_to_lfo1ShapeSplit, auxAttribute);

	// --- controlID::lfo1_to_lfo2Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo1_to_lfo2Shape, auxAttribute);

	// --- controlID::lfo2_to_lfo2Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo2_to_lfo2Shape, auxAttribute);

	// --- controlID::eg1_to_lfo2Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg1_to_lfo2Shape, auxAttribute);

	// --- controlID::eg2_to_lfo2Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg2_to_lfo2Shape, auxAttribute);

	// --- controlID::joystickAC_to_lfo2Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickAC_to_lfo2Shape, auxAttribute);

	// --- controlID::joystickBD_to_lfo2Shape
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickBD_to_lfo2Shape, auxAttribute);

	// --- controlID::lfo1_to_lfo2ShapeSplit
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo1_to_lfo2ShapeSplit, auxAttribute);

	// --- controlID::lfo2_to_lfo2ShapeSplit
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo2_to_lfo2ShapeSplit, auxAttribute);

	// --- controlID::eg1_to_lfo2ShapeSplit
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg1_to_lfo2ShapeSplit, auxAttribute);

	// --- controlID::eg2_to_lfo2ShapeSplit
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg2_to_lfo2ShapeSplit, auxAttribute);

	// --- controlID::joystickAC_to_lfo2ShapeSplit
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickAC_to_lfo2ShapeSplit, auxAttribute);

	// --- controlID::joystickBD_to_lfo2ShapeSplit
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickBD_to_lfo2ShapeSplit, auxAttribute);

	// --- controlID::lfo1_to_eg1Attack
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo1_to_eg1Attack, auxAttribute);

	// --- controlID::lfo2_to_eg1Attack
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo2_to_eg1Attack, auxAttribute);

	// --- controlID::joystickAC_to_eg1Attack
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickAC_to_eg1Attack, auxAttribute);

	// --- controlID::joystickBD_to_eg1Attack
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741824);
	setParamAuxAttribute(controlID::joystickBD_to_eg1Attack, auxAttribute);

	// --- controlID::eg2_to_eg1Attack
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg2_to_eg1Attack, auxAttribute);

	// --- controlID::lfo1_to_eg1Hold
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo1_to_eg1Hold, auxAttribute);

	// --- controlID::lfo2_to_eg1Hold
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo2_to_eg1Hold, auxAttribute);

	// --- controlID::eg2_to_eg1Hold
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg2_to_eg1Hold, auxAttribute);

	// --- controlID::joystickAC_to_eg1Hold
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickAC_to_eg1Hold, auxAttribute);

	// --- controlID::joystickBD_to_eg1Hold
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickBD_to_eg1Hold, auxAttribute);

	// --- controlID::lfo1_to_eg1Decay
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo1_to_eg1Decay, auxAttribute);

	// --- controlID::lfo2_to_eg1Decay
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo2_to_eg1Decay, auxAttribute);

	// --- controlID::eg2_to_eg1Decay
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg2_to_eg1Decay, auxAttribute);

	// --- controlID::joystickAC_to_eg1Decay
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickAC_to_eg1Decay, auxAttribute);

	// --- controlID::joystickBD_to_eg1Decay
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickBD_to_eg1Decay, auxAttribute);

	// --- controlID::lfo1_to_eg1Sustain
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo1_to_eg1Sustain, auxAttribute);

	// --- controlID::lfo2_to_eg1Sustain
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo2_to_eg1Sustain, auxAttribute);

	// --- controlID::eg2_to_eg1Sustain
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg2_to_eg1Sustain, auxAttribute);

	// --- controlID::joystickAC_to_eg1Sustain
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickAC_to_eg1Sustain, auxAttribute);

	// --- controlID::joystickBD_to_eg1Sustain
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickBD_to_eg1Sustain, auxAttribute);

	// --- controlID::lfo1_to_eg1Release
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo1_to_eg1Release, auxAttribute);

	// --- controlID::lfo2_to_eg1Release
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo2_to_eg1Release, auxAttribute);

	// --- controlID::eg2_to_eg1Release
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg2_to_eg1Release, auxAttribute);

	// --- controlID::joystickAC_to_eg1Release
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickAC_to_eg1Release, auxAttribute);

	// --- controlID::joystickBD_to_eg1Release
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::joystickBD_to_eg1Release, auxAttribute);

	// --- controlID::temperamentKeySelection
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::temperamentKeySelection, auxAttribute);

	// --- controlID::osc1ExciterMode
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::osc1ExciterMode, auxAttribute);

	// --- controlID::exciter1Attack_msec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::exciter1Attack_msec, auxAttribute);

	// --- controlID::exciter1Hold_msec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::exciter1Hold_msec, auxAttribute);

	// --- controlID::exciter1Release_msec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::exciter1Release_msec, auxAttribute);

	// --- controlID::resonator1Decay
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::resonator1Decay, auxAttribute);

	// --- controlID::osc2ExciterMode
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::osc2ExciterMode, auxAttribute);

	// --- controlID::osc3ExciterMode
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::osc3ExciterMode, auxAttribute);

	// --- controlID::osc4ExciterMode
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::osc4ExciterMode, auxAttribute);

	// --- controlID::exciter2Attack_msec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::exciter2Attack_msec, auxAttribute);

	// --- controlID::exciter2Hold_msec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::exciter2Hold_msec, auxAttribute);

	// --- controlID::exciter2Release_msec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::exciter2Release_msec, auxAttribute);

	// --- controlID::resonator2Decay
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::resonator2Decay, auxAttribute);

	// --- controlID::exciter3Attack_msec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::exciter3Attack_msec, auxAttribute);

	// --- controlID::exciter3Hold_msec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::exciter3Hold_msec, auxAttribute);

	// --- controlID::exciter3Release_msec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::exciter3Release_msec, auxAttribute);

	// --- controlID::resonator3Decay
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::resonator3Decay, auxAttribute);

	// --- controlID::exciter4Attack_msec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::exciter4Attack_msec, auxAttribute);

	// --- controlID::exciter4Hold_msec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::exciter4Hold_msec, auxAttribute);

	// --- controlID::exciter4Release_msec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::exciter4Release_msec, auxAttribute);

	// --- controlID::resonator4Decay
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::resonator4Decay, auxAttribute);

	// --- controlID::fc2_hertz
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::fc2_hertz, auxAttribute);

	// --- controlID::q2Control
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::q2Control, auxAttribute);

	// --- controlID::filter1Algorithm
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::filter1Algorithm, auxAttribute);

	// --- controlID::filter2Algorithm
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::filter2Algorithm, auxAttribute);

	// --- controlID::enableGainCompensation
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741824);
	setParamAuxAttribute(controlID::enableGainCompensation, auxAttribute);


	// **--0xEDA5--**
   
    // --- BONUS Parameter
    // --- SCALE_GUI_SIZE
    PluginParameter* piParamBonus = new PluginParameter(SCALE_GUI_SIZE, "Scale GUI", "tiny,small,medium,normal,large,giant", "normal");
    addPluginParameter(piParamBonus);

	// --- create the super fast access array
	initPluginParameterArray();

    return true;
}

/**
\brief initialize object for a new run of audio; called just before audio streams

Operation:
- store sample rate and bit depth on audioProcDescriptor - this information is globally available to all core functions
- reset your member objects here

\param resetInfo structure of information about current audio format

\return true if operation succeeds, false otherwise
*/
bool PluginCore::reset(ResetInfo& resetInfo)
{
    // --- save for audio processing
    audioProcDescriptor.sampleRate = resetInfo.sampleRate;
    audioProcDescriptor.bitDepth = resetInfo.bitDepth;

	// --- reset engine
	synthEngine.reset(resetInfo.sampleRate);

	

    // --- other reset inits
    return PluginBase::reset(resetInfo);
}

/**
\brief one-time initialize function called after object creation and before the first reset( ) call

Operation:
- saves structure for the plugin to use; you can also load WAV files or state information here
*/
bool PluginCore::initialize(PluginInfo& pluginInfo)
{
	// --- add one-time init stuff here

	return true;
}

/**
\brief do anything needed prior to arrival of audio buffers

Operation:
- syncInBoundVariables when preProcessAudioBuffers is called, it is *guaranteed* that all GUI control change information
  has been applied to plugin parameters; this binds parameter changes to your underlying variables
- NOTE: postUpdatePluginParameter( ) will be called for all bound variables that are acutally updated; if you need to process
  them individually, do so in that function
- use this function to bulk-transfer the bound variable data into your plugin's member object variables

\param processInfo structure of information about *buffer* processing

\return true if operation succeeds, false otherwise
*/
bool PluginCore::preProcessAudioBuffers(ProcessBufferInfo& processInfo)
{
    // --- sync internal variables to GUI parameters; you can also do this manually if you don't
    //     want to use the auto-variable-binding
    syncInBoundVariables();

	// --- update all parameters ONCE per buffer
	updateParameters();

    return true;
}

void PluginCore::updateParameters()
{
	SynthEngineParameters engineParams = synthEngine.getParameters();

	// --- collect GUI control update values
	engineParams.masterPitchBendSensCoarse = (unsigned int)masterPitchBend; // --- this is pitch bend max range in semitones
	engineParams.masterPitchBendSensFine = (unsigned int)(100.0*(masterPitchBend - engineParams.masterPitchBendSensCoarse)); // this is pitch bend max range in semitones

	// --- create two tuning offsets from one master tune value
	engineParams.masterTuningCoarse = (int)masterTune;
	engineParams.masterTuningFine = (int)(100.0*(masterTune - engineParams.masterTuningCoarse)); // --- get fraction and convert to cents (1/100th of a semitone)

	engineParams.masterVolume_dB = masterVolume_dB;

	/// LFO 1 Parameters
	engineParams.voiceParameters->lfo1Parameters->frequency_Hz = lfo1Frequency_Hz;
	engineParams.voiceParameters->lfo1Parameters->waveform = convertIntToEnum(lfo1Waveform, LFOWaveform);
	engineParams.voiceParameters->lfo1Parameters->mode = convertIntToEnum(lfo1Mode, LFOMode);
	engineParams.voiceParameters->lfo1Parameters->lfoDelay_mSec = lfo1DelayTime_mSec;
	engineParams.voiceParameters->lfo1Parameters->lfoRamp_mSec = lfo1RampTime_mSec;
	engineParams.voiceParameters->lfo1Parameters->lfoShape = lfo1Shape;
	engineParams.voiceParameters->lfo1Parameters->shapeSplitpoint = lfo1ShapeSplit;
	//engineParams.voiceParameters->lfo1Parameters->modRoute = convertIntToEnum(lfo2ModTarget, ModRouting);

	/// LFO 2 Parameters
	engineParams.voiceParameters->lfo2Parameters->frequency_Hz = lfo2Frequency_Hz;
	engineParams.voiceParameters->lfo2Parameters->waveform = convertIntToEnum(lfo2Waveform, LFOWaveform);
	engineParams.voiceParameters->lfo2Parameters->mode = convertIntToEnum(lfo2Mode, LFOMode);
	engineParams.voiceParameters->lfo2Parameters->lfoShape = lfo2Shape;
	engineParams.voiceParameters->lfo2Parameters->shapeSplitpoint = lfo2ShapeSplit;

	//engineParams.voiceParameters->osc1Parameters->scaleSelect = convertIntToEnum(scaleMode, ScaleMode);
	
	/// DCA Parameters

	engineParams.voiceParameters->dcaParameters->bpm = bpm;
	//engineParams.voiceParameters->dcaParameters->enableRhythmicBreaks = false;
	//engineParams.voiceParameters->dcaParameters->modRoute = convertIntToEnum(lfo2ModTarget, ModRouting);

	/// OSC Parameters
	engineParams.voiceParameters->osc1Parameters->scaleSelect = convertIntToEnum(ScaleMode, scaleMode);
	engineParams.voiceParameters->osc2Parameters->scaleSelect = convertIntToEnum(ScaleMode, scaleMode);
	engineParams.voiceParameters->osc3Parameters->scaleSelect = convertIntToEnum(ScaleMode, scaleMode);
	engineParams.voiceParameters->osc4Parameters->scaleSelect = convertIntToEnum(ScaleMode, scaleMode);

	//en...rs->oscillator1Shape = 0.5;
	engineParams.voiceParameters->osc1Parameters->oscillatorWaveformIndex = osc1Waveform;
	engineParams.voiceParameters->osc1Parameters->oscillatorBankIndex = osc1BankIndex;
	//engineParams.voiceParameters->osc1Parameters->keyNoteNumber = keySelection;
	//engineParams.voiceParameters->osc1Parameters->temperamentChoice = temperamentSelection;
	engineParams.voiceParameters->osc2Parameters->oscillatorWaveformIndex = osc2Waveform;
	engineParams.voiceParameters->osc2Parameters->oscillatorBankIndex = osc2BankIndex;
	//engineParams.voiceParameters->osc2Parameters->keyNoteNumber = keySelection;
	//engineParams.voiceParameters->osc1Parameters->temperamentChoice = temperamentSelection;
	engineParams.voiceParameters->osc3Parameters->oscillatorWaveformIndex = osc3Waveform;
	engineParams.voiceParameters->osc3Parameters->oscillatorBankIndex = osc3BankIndex;
	//engineParams.voiceParameters->osc3Parameters->keyNoteNumber = keySelection;
	//engineParams.voiceParameters->osc1Parameters->temperamentChoice = temperamentSelection;
	engineParams.voiceParameters->osc4Parameters->oscillatorWaveformIndex = osc4Waveform;
	engineParams.voiceParameters->osc4Parameters->oscillatorBankIndex = osc4BankIndex;
	//engineParams.voiceParameters->osc4Parameters->keyNoteNumber = keySelection;
	//engineParams.voiceParameters->osc1Parameters->temperamentChoice = temperamentSelection;

	engineParams.voiceParameters->osc1Parameters->detuneCents = osc1Detune_cents;

	/// EG1 Parameters

	engineParams.voiceParameters->EG1Parameters->delayTime_mSec = eg1DelayTime_mSec;
	engineParams.voiceParameters->EG1Parameters->attackTime_mSec = eg1AttackTime_mSec;
	engineParams.voiceParameters->EG1Parameters->holdTime_mSec = eg1HoldTime_mSec;
	engineParams.voiceParameters->EG1Parameters->decayTime_mSec = eg1DecayTime_mSec;
	engineParams.voiceParameters->EG1Parameters->sustainLevel = eg1SustainLevel;
	engineParams.voiceParameters->EG1Parameters->releaseTime_mSec = eg1ReleaseTime_mSec;
	engineParams.voiceParameters->EG1Parameters->egContourType = convertIntToEnum(eg1Mode, egType);
	engineParams.voiceParameters->EG1Parameters->autoRetrigger = eg1AutoRetrigger;
	engineParams.voiceParameters->EG1Parameters->manualRetrigger = eg1ManualTrigger;

	/// MOOG FILTER Parameters

	engineParams.voiceParameters->moogFilter1Parameters->fc = fc1_hertz;
	engineParams.voiceParameters->moogFilter1Parameters->Q = q1Control;
	engineParams.voiceParameters->moogFilter1Parameters->enableKeyTrack = (enableKeyTrack == 1);
	engineParams.voiceParameters->moogFilter1Parameters->keyTrackRatio = keyTrackRatio;
	engineParams.voiceParameters->moogFilter1Parameters->filterAlgorithm = convertIntToEnum(filter1Algorithm, moogFilterAlgorithm);
	engineParams.voiceParameters->moogFilter1Parameters->enableGainComp = enableGainCompensation;


	engineParams.voiceParameters->moogFilter2Parameters->fc = fc2_hertz;
	engineParams.voiceParameters->moogFilter2Parameters->Q = q2Control;
	engineParams.voiceParameters->moogFilter2Parameters->enableKeyTrack = (enableKeyTrack == 1);
	engineParams.voiceParameters->moogFilter2Parameters->keyTrackRatio = keyTrackRatio;
	engineParams.voiceParameters->moogFilter2Parameters->filterAlgorithm = convertIntToEnum(filter2Algorithm, moogFilterAlgorithm);
	engineParams.voiceParameters->moogFilter2Parameters->enableGainComp = enableGainCompensation;

	/// MOD MATRIX Routings

	// --- Source Intensities

	engineParams.setMM_SourceMasterIntensity(kLFO1_Normal, lfo1Amplitude);
	engineParams.setMM_SourceMasterIntensity(kLFO2_Normal, lfo2Amplitude);
	engineParams.setMM_SourceMasterIntensity(kEG1_Normal, eg1ModOut);
	engineParams.setMM_SourceMasterIntensity(kEG2_Normal, eg2ModOut);
	engineParams.setMM_SourceMasterIntensity(kJoystickAC, joystickACModOut);
	engineParams.setMM_SourceMasterIntensity(kJoystickBD, joystickBDModOut);
	engineParams.setMM_SourceMasterIntensity(kAuxUnipolarModOut1, auxUnipolar1ModOut);
	engineParams.setMM_SourceMasterIntensity(kAuxUnipolarModOut2, auxUnipolar2ModOut);
	engineParams.setMM_SourceMasterIntensity(kAuxUnipolarModOut3, auxUnipolar3ModOut);
	engineParams.setMM_SourceMasterIntensity(kAuxUnipolarModOut4, auxUnipolar4ModOut);
	engineParams.setMM_SourceMasterIntensity(kAuxUnipolarModOut5, auxUnipolar5ModOut);
	engineParams.setMM_SourceMasterIntensity(kAuxUnipolarModOut6, auxUnipolar6ModOut);
	engineParams.setMM_SourceMasterIntensity(kAuxBipolarModOut1, auxUnipolar1ModOut);
	engineParams.setMM_SourceMasterIntensity(kAuxBipolarModOut2, auxUnipolar2ModOut);
	engineParams.setMM_SourceMasterIntensity(kAuxBipolarModOut3, auxUnipolar3ModOut);
	engineParams.setMM_SourceMasterIntensity(kAuxBipolarModOut4, auxUnipolar4ModOut);


	// --- Destination Intensities

	// Oscillators
	//engineParams.setMM_DestMasterIntensity(kOsc1_fo, osc1FoModIn);
	engineParams.setMM_DestMasterIntensity(kOsc1_Shape, osc1ShapeModIn);
	engineParams.setMM_DestMasterIntensity(kOsc2_fo, osc2FoModIn);
	engineParams.setMM_DestMasterIntensity(kOsc2_Shape, osc2ShapeModIn);

	// LFO's
	engineParams.setMM_DestMasterIntensity(kLFO1_Shape, lfo1ShapeModIn);
	engineParams.setMM_DestMasterIntensity(kLFO1_ShapeSplit, lfo1ShapeSplitModIn);
	engineParams.setMM_DestMasterIntensity(kLFO2_Shape, lfo2ShapeModIn);
	engineParams.setMM_DestMasterIntensity(kLFO2_ShapeSplit, lfo2ShapeSplitModIn);

	// EG's
	engineParams.setMM_DestMasterIntensity(kEG1_Attack, eg1AttackModIn);
	engineParams.setMM_DestMasterIntensity(kEG1_Hold, eg1HoldModIn);
	engineParams.setMM_DestMasterIntensity(kEG1_Decay, eg1DecayModIn);
	engineParams.setMM_DestMasterIntensity(kEG1_Sustain, eg1SustainModIn);
	engineParams.setMM_DestMasterIntensity(kEG1_Release, eg1ReleaseModIn);

	// Filters
	engineParams.setMM_DestMasterIntensity(kFilter1_fc, filter1FoModIn);
	engineParams.setMM_DestMasterIntensity(kFilter1_Q, filter1QModIn);
	engineParams.setMM_DestMasterIntensity(kFilter2_fc, filter2FoModIn);
	engineParams.setMM_DestMasterIntensity(kFilter2_Q, filter2QModIn);

	// DCA
	engineParams.setMM_DestMasterIntensity(kDCA_SampleHoldMod, DCASampleHoldModIn); //param name needs changing (along with all routing's)

	// Globals
	engineParams.setMM_DestMasterIntensity(kUnisonDetune, unisonDetuneModIn);
	




	// --- Routings (On/Off Switches)

	// -- Oscillators
	// OSC 1
	engineParams.setMM_ChannelEnable(kLFO1_Normal, kOsc1_fo, lfo1_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kLFO2_Normal, kOsc1_fo, lfo2_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kEG1_Normal, kOsc1_fo, eg1_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kEG2_Normal, kOsc1_fo, eg2_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kJoystickAC, kOsc1_fo, joystickAC_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kJoystickBD, kOsc1_fo, joystickBD_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut1, kOsc1_fo, auxUnipolar1Mod_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut2, kOsc1_fo, auxUnipolar2Mod_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut3, kOsc1_fo, auxUnipolar3Mod_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut4, kOsc1_fo, auxUnipolar4Mod_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut5, kOsc1_fo, auxUnipolar5Mod_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut6, kOsc1_fo, auxUnipolar6Mod_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut1, kOsc1_fo, auxBipolar1Mod_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut2, kOsc1_fo, auxBipolar2Mod_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut3, kOsc1_fo, auxBipolar3Mod_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut4, kOsc1_fo, auxBipolar4Mod_to_osc1Fo);

	engineParams.setMM_ChannelEnable(kLFO1_Normal, kOsc1_Shape, lfo1_to_osc1Shape);
	engineParams.setMM_ChannelEnable(kLFO2_Normal, kOsc1_Shape, lfo2_to_osc1Shape);
	engineParams.setMM_ChannelEnable(kEG1_Normal, kOsc1_Shape, eg1_to_osc1Shape);
	engineParams.setMM_ChannelEnable(kEG2_Normal, kOsc1_Shape, eg2_to_osc1Shape);
	engineParams.setMM_ChannelEnable(kJoystickAC, kOsc1_Shape, joystickAC_to_osc1Shape);
	engineParams.setMM_ChannelEnable(kJoystickBD, kOsc1_Shape, joystickBD_to_osc1Shape);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut1, kOsc1_Shape, auxUnipolar1Mod_to_osc1Shape);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut2, kOsc1_Shape, auxUnipolar2Mod_to_osc1Shape);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut3, kOsc1_Shape, auxUnipolar3Mod_to_osc1Shape);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut4, kOsc1_Shape, auxUnipolar4Mod_to_osc1Shape);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut5, kOsc1_Shape, auxUnipolar5Mod_to_osc1Shape);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut6, kOsc1_Shape, auxUnipolar6Mod_to_osc1Shape);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut1, kOsc1_Shape, auxBipolar1Mod_to_osc1Shape);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut2, kOsc1_Shape, auxBipolar2Mod_to_osc1Shape);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut3, kOsc1_Shape, auxBipolar3Mod_to_osc1Shape);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut4, kOsc1_Shape, auxBipolar4Mod_to_osc1Shape);

	//OSC 2
	engineParams.setMM_ChannelEnable(kLFO1_Normal, kOsc2_fo, lfo1_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kLFO2_Normal, kOsc2_fo, lfo2_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kEG1_Normal, kOsc2_fo, eg1_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kEG2_Normal, kOsc2_fo, eg2_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kJoystickAC, kOsc2_fo, joystickAC_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kJoystickBD, kOsc2_fo, joystickBD_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut1, kOsc2_fo, auxUnipolar1Mod_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut2, kOsc2_fo, auxUnipolar2Mod_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut3, kOsc2_fo, auxUnipolar3Mod_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut4, kOsc2_fo, auxUnipolar4Mod_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut5, kOsc2_fo, auxUnipolar5Mod_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut6, kOsc2_fo, auxUnipolar6Mod_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut1, kOsc2_fo, auxBipolar1Mod_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut2, kOsc2_fo, auxBipolar2Mod_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut3, kOsc2_fo, auxBipolar3Mod_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut4, kOsc2_fo, auxBipolar4Mod_to_osc2Fo);

	engineParams.setMM_ChannelEnable(kLFO1_Normal, kOsc2_Shape, lfo1_to_osc2Shape);
	engineParams.setMM_ChannelEnable(kLFO2_Normal, kOsc2_Shape, lfo2_to_osc2Shape);
	engineParams.setMM_ChannelEnable(kEG1_Normal, kOsc2_Shape, eg1_to_osc2Shape);
	engineParams.setMM_ChannelEnable(kEG2_Normal, kOsc2_Shape, eg2_to_osc2Shape);
	engineParams.setMM_ChannelEnable(kJoystickAC, kOsc2_Shape, joystickAC_to_osc2Shape);
	engineParams.setMM_ChannelEnable(kJoystickBD, kOsc2_Shape, joystickBD_to_osc2Shape);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut1, kOsc2_Shape, auxUnipolar1Mod_to_osc2Shape);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut2, kOsc2_Shape, auxUnipolar2Mod_to_osc2Shape);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut3, kOsc2_Shape, auxUnipolar3Mod_to_osc2Shape);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut4, kOsc2_Shape, auxUnipolar4Mod_to_osc2Shape);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut5, kOsc2_Shape, auxUnipolar5Mod_to_osc2Shape);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut6, kOsc2_Shape, auxUnipolar6Mod_to_osc2Shape);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut1, kOsc2_Shape, auxBipolar1Mod_to_osc2Shape);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut2, kOsc2_Shape, auxBipolar2Mod_to_osc2Shape);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut3, kOsc2_Shape, auxBipolar3Mod_to_osc2Shape);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut4, kOsc2_Shape, auxBipolar4Mod_to_osc2Shape);

	// -- LFO's
	// LFO 1
	/*engineParams.setMM_ChannelEnable(kLFO1_Normal, kLFO1_fo, lfo1_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kLFO2_Normal, kLFO1_fo, lfo2_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kEG1_Normal, kLFO1_fo, eg1_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kEG2_Normal, kLFO1_fo, eg2_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kJoystickAC, kLFO1_fo, joystickAC_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kJoystickBD, kLFO1_fo, joystickBD_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut1, kLFO1_fo, auxUnipolar1Mod_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut2, kLFO1_fo, auxUnipolar2Mod_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut3, kLFO1_fo, auxUnipolar3Mod_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut4, kLFO1_fo, auxUnipolar4Mod_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut5, kLFO1_fo, auxUnipolar5Mod_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut6, kLFO1_fo, auxUnipolar6Mod_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut1, kLFO1_fo, auxBipolar1Mod_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut2, kLFO1_fo, auxBipolar2Mod_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut3, kLFO1_fo, auxBipolar3Mod_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut4, kLFO1_fo, auxBipolar4Mod_to_osc1Fo);*/

	engineParams.setMM_ChannelEnable(kLFO1_Normal, kLFO1_Shape, lfo1_to_lfo1Shape);
	engineParams.setMM_ChannelEnable(kLFO2_Normal, kLFO1_Shape, lfo2_to_lfo1Shape);
	engineParams.setMM_ChannelEnable(kEG1_Normal, kLFO1_Shape, eg1_to_lfo1Shape);
	engineParams.setMM_ChannelEnable(kEG2_Normal, kLFO1_Shape, eg2_to_lfo1Shape);
	engineParams.setMM_ChannelEnable(kJoystickAC, kLFO1_Shape, joystickAC_to_lfo1Shape);
	engineParams.setMM_ChannelEnable(kJoystickBD, kLFO1_Shape, joystickBD_to_lfo1Shape);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut1, kLFO1_Shape, auxUnipolar1Mod_to_lfo1Shape);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut2, kLFO1_Shape, auxUnipolar2Mod_to_lfo1Shape);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut3, kLFO1_Shape, auxUnipolar3Mod_to_lfo1Shape);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut4, kLFO1_Shape, auxUnipolar4Mod_to_lfo1Shape);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut5, kLFO1_Shape, auxUnipolar5Mod_to_lfo1Shape);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut6, kLFO1_Shape, auxUnipolar6Mod_to_lfo1Shape);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut1, kLFO1_Shape, auxBipolar1Mod_to_lfo1Shape);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut2, kLFO1_Shape, auxBipolar2Mod_to_lfo1Shape);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut3, kLFO1_Shape, auxBipolar3Mod_to_lfo1Shape);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut4, kLFO1_Shape, auxBipolar4Mod_to_lfo1Shape);

	engineParams.setMM_ChannelEnable(kLFO1_Normal, kLFO1_ShapeSplit, lfo1_to_lfo1ShapeSplit);
	engineParams.setMM_ChannelEnable(kLFO2_Normal, kLFO1_ShapeSplit, lfo2_to_lfo1ShapeSplit);
	engineParams.setMM_ChannelEnable(kEG1_Normal, kLFO1_ShapeSplit, eg1_to_lfo1ShapeSplit);
	engineParams.setMM_ChannelEnable(kEG2_Normal, kLFO1_ShapeSplit, eg2_to_lfo1ShapeSplit);
	engineParams.setMM_ChannelEnable(kJoystickAC, kLFO1_ShapeSplit, joystickAC_to_lfo1ShapeSplit);
	engineParams.setMM_ChannelEnable(kJoystickBD, kLFO1_ShapeSplit, joystickBD_to_lfo1ShapeSplit);
	/*engineParams.setMM_ChannelEnable(kAuxUnipolarModOut1, kLFO1_ShapeSplit, auxUnipolar1Mod_to_lfo1ShapeSplit);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut2, kLFO1_ShapeSplit, auxUnipolar2Mod_to_lfo1ShapeSplit);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut3, kLFO1_ShapeSplit, auxUnipolar3Mod_to_lfo1ShapeSplit);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut4, kLFO1_ShapeSplit, auxUnipolar4Mod_to_lfo1ShapeSplit);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut5, kLFO1_ShapeSplit, auxUnipolar5Mod_to_lfo1ShapeSplit);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut6, kLFO1_ShapeSplit, auxUnipolar6Mod_to_lfo1ShapeSplit);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut1, kLFO1_ShapeSplit, auxBipolar1Mod_to_lfo1ShapeSplit);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut2, kLFO1_ShapeSplit, auxBipolar2Mod_to_lfo1ShapeSplit);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut3, kLFO1_ShapeSplit, auxBipolar3Mod_to_lfo1ShapeSplit);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut4, kLFO1_ShapeSplit, auxBipolar4Mod_to_lfo1ShapeSplit);*/

	//LFO 2
	/*engineParams.setMM_ChannelEnable(kLFO1_Normal, kLFO2_fo, lfo1_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kLFO2_Normal, kLFO2_fo, lfo2_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kEG1_Normal, kLFO2_fo, eg1_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kEG2_Normal, kLFO2_fo, eg2_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kJoystickAC, kLFO2_fo, joystickAC_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kJoystickBD, kLFO2_fo, joystickBD_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut1, kLFO2_fo, auxUnipolar1Mod_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut2, kLFO2_fo, auxUnipolar2Mod_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut3, kLFO2_fo, auxUnipolar3Mod_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut4, kLFO2_fo, auxUnipolar4Mod_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut5, kLFO2_fo, auxUnipolar5Mod_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut6, kLFO2_fo, auxUnipolar6Mod_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut1, kLFO2_fo, auxBipolar1Mod_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut2, kLFO2_fo, auxBipolar2Mod_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut3, kLFO2_fo, auxBipolar3Mod_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut4, kLFO2_fo, auxBipolar4Mod_to_osc2Fo);*/


	engineParams.setMM_ChannelEnable(kLFO1_Normal, kLFO2_Shape, lfo1_to_lfo2Shape);
	engineParams.setMM_ChannelEnable(kLFO2_Normal, kLFO2_Shape, lfo2_to_lfo2Shape);
	engineParams.setMM_ChannelEnable(kEG1_Normal, kLFO2_Shape, eg1_to_lfo2Shape);
	engineParams.setMM_ChannelEnable(kEG2_Normal, kLFO2_Shape, eg2_to_lfo2Shape);
	engineParams.setMM_ChannelEnable(kJoystickAC, kLFO2_Shape, joystickAC_to_lfo2Shape);
	engineParams.setMM_ChannelEnable(kJoystickBD, kLFO2_Shape, joystickBD_to_lfo2Shape);
	/*engineParams.setMM_ChannelEnable(kAuxUnipolarModOut1, kLFO2_Shape, auxUnipolar1Mod_to_lfo2Shape);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut2, kLFO2_Shape, auxUnipolar2Mod_to_lfo2Shape);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut3, kLFO2_Shape, auxUnipolar3Mod_to_lfo2Shape);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut4, kLFO2_Shape, auxUnipolar4Mod_to_lfo2Shape);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut5, kLFO2_Shape, auxUnipolar5Mod_to_lfo2Shape);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut6, kLFO2_Shape, auxUnipolar6Mod_to_lfo2Shape);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut1, kLFO2_Shape, auxBipolar1Mod_to_lfo2Shape);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut2, kLFO2_Shape, auxBipolar2Mod_to_lfo2Shape);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut3, kLFO2_Shape, auxBipolar3Mod_to_lfo2Shape);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut4, kLFO2_Shape, auxBipolar4Mod_to_lfo2Shape);*/

	engineParams.setMM_ChannelEnable(kLFO1_Normal, kLFO2_ShapeSplit, lfo1_to_lfo2ShapeSplit);
	engineParams.setMM_ChannelEnable(kLFO2_Normal, kLFO2_ShapeSplit, lfo2_to_lfo2ShapeSplit);
	engineParams.setMM_ChannelEnable(kEG1_Normal, kLFO2_ShapeSplit, eg1_to_lfo2ShapeSplit);
	engineParams.setMM_ChannelEnable(kEG2_Normal, kLFO2_ShapeSplit, eg2_to_lfo2ShapeSplit);
	engineParams.setMM_ChannelEnable(kJoystickAC, kLFO2_ShapeSplit, joystickAC_to_lfo2ShapeSplit);
	engineParams.setMM_ChannelEnable(kJoystickBD, kLFO2_ShapeSplit, joystickBD_to_lfo2ShapeSplit);
	/*engineParams.setMM_ChannelEnable(kAuxUnipolarModOut1, kLFO2_ShapeSplit, auxUnipolar1Mod_to_lfo2ShapeSplit);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut2, kLFO2_ShapeSplit, auxUnipolar2Mod_to_lfo2ShapeSplit);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut3, kLFO2_ShapeSplit, auxUnipolar3Mod_to_lfo2ShapeSplit);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut4, kLFO2_ShapeSplit, auxUnipolar4Mod_to_lfo2ShapeSplit);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut5, kLFO2_ShapeSplit, auxUnipolar5Mod_to_lfo2ShapeSplit);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut6, kLFO2_ShapeSplit, auxUnipolar6Mod_to_lfo2ShapeSplit);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut1, kLFO2_ShapeSplit, auxBipolar1Mod_to_lfo2ShapeSplit);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut2, kLFO2_ShapeSplit, auxBipolar2Mod_to_lfo2ShapeSplit);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut3, kLFO2_ShapeSplit, auxBipolar3Mod_to_lfo2ShapeSplit);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut4, kLFO2_ShapeSplit, auxBipolar4Mod_to_lfo2ShapeSplit);*/


	// -- EG's

	//EG1 Attack
	engineParams.setMM_ChannelEnable(kLFO1_Normal, kEG1_Attack, lfo1_to_eg1Attack);
	engineParams.setMM_ChannelEnable(kLFO2_Normal, kEG1_Attack, lfo2_to_eg1Attack);
	//engineParams.setMM_ChannelEnable(kEG1_Normal, kEG1_Attack, eg1_to_eg1Attack);
	engineParams.setMM_ChannelEnable(kEG2_Normal, kEG1_Attack, eg2_to_eg1Attack);
	engineParams.setMM_ChannelEnable(kJoystickAC, kEG1_Attack, joystickAC_to_eg1Attack);
	engineParams.setMM_ChannelEnable(kJoystickBD, kEG1_Attack, joystickBD_to_eg1Attack);
	/*engineParams.setMM_ChannelEnable(kAuxUnipolarModOut1, kEG1_Attack, auxUnipolar1Mod_to_eg1Attack);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut2, kEG1_Attack, auxUnipolar2Mod_to_eg1Attack);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut3, kEG1_Attack, auxUnipolar3Mod_to_eg1Attack);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut4, kEG1_Attack, auxUnipolar4Mod_to_eg1Attack);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut5, kEG1_Attack, auxUnipolar5Mod_to_eg1Attack);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut6, kEG1_Attack, auxUnipolar6Mod_to_eg1Attack);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut1, kEG1_Attack, auxBipolar1Mod_to_eg1Attack);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut2, kEG1_Attack, auxBipolar2Mod_to_eg1Attack);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut3, kEG1_Attack, auxBipolar3Mod_to_eg1Attack);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut4, kEG1_Attack, auxBipolar4Mod_to_eg1Attack);*/

	//EG1 Hold
	engineParams.setMM_ChannelEnable(kLFO1_Normal, kEG1_Hold, lfo1_to_eg1Hold);
	engineParams.setMM_ChannelEnable(kLFO2_Normal, kEG1_Hold, lfo2_to_eg1Hold);
	//engineParams.setMM_ChannelEnable(kEG1_Normal, kEG1_Hold, eg1_to_eg1Hold);
	engineParams.setMM_ChannelEnable(kEG2_Normal, kEG1_Hold, eg2_to_eg1Hold);
	engineParams.setMM_ChannelEnable(kJoystickAC, kEG1_Hold, joystickAC_to_eg1Hold);
	engineParams.setMM_ChannelEnable(kJoystickBD, kEG1_Hold, joystickBD_to_eg1Hold);
	/*engineParams.setMM_ChannelEnable(kAuxUnipolarModOut1, kEG1_Hold, auxUnipolar1Mod_to_eg1Hold);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut2, kEG1_Hold, auxUnipolar2Mod_to_eg1Hold);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut3, kEG1_Hold, auxUnipolar3Mod_to_eg1Hold);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut4, kEG1_Hold, auxUnipolar4Mod_to_eg1Hold);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut5, kEG1_Hold, auxUnipolar5Mod_to_eg1Hold);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut6, kEG1_Hold, auxUnipolar6Mod_to_eg1Hold);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut1, kEG1_Hold, auxBipolar1Mod_to_eg1Hold);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut2, kEG1_Hold, auxBipolar2Mod_to_eg1Hold);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut3, kEG1_Hold, auxBipolar3Mod_to_eg1Hold);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut4, kEG1_Hold, auxBipolar4Mod_to_eg1Hold);*/

	//EG1 Decay
	engineParams.setMM_ChannelEnable(kLFO1_Normal, kEG1_Decay, lfo1_to_eg1Decay);
	engineParams.setMM_ChannelEnable(kLFO2_Normal, kEG1_Decay, lfo2_to_eg1Decay);
	//engineParams.setMM_ChannelEnable(kEG1_Normal, kEG1_Decay, eg1_to_eg1Decay);
	engineParams.setMM_ChannelEnable(kEG2_Normal, kEG1_Decay, eg2_to_eg1Decay);
	engineParams.setMM_ChannelEnable(kJoystickAC, kEG1_Decay, joystickAC_to_eg1Decay);
	engineParams.setMM_ChannelEnable(kJoystickBD, kEG1_Decay, joystickBD_to_eg1Decay);
	/*engineParams.setMM_ChannelEnable(kAuxUnipolarModOut1, kEG1_Decay, auxUnipolar1Mod_to_eg1Decay);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut2, kEG1_Decay, auxUnipolar2Mod_to_eg1Decay);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut3, kEG1_Decay, auxUnipolar3Mod_to_eg1Decay);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut4, kEG1_Decay, auxUnipolar4Mod_to_eg1Decay);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut5, kEG1_Decay, auxUnipolar5Mod_to_eg1Decay);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut6, kEG1_Decay, auxUnipolar6Mod_to_eg1Decay);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut1, kEG1_Decay, auxBipolar1Mod_to_eg1Decay);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut2, kEG1_Decay, auxBipolar2Mod_to_eg1Decay);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut3, kEG1_Decay, auxBipolar3Mod_to_eg1Decay);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut4, kEG1_Decay, auxBipolar4Mod_to_eg1Decay);*/

	//EG1 Sustain
	engineParams.setMM_ChannelEnable(kLFO1_Normal, kEG1_Sustain, lfo1_to_eg1Sustain);
	engineParams.setMM_ChannelEnable(kLFO2_Normal, kEG1_Sustain, lfo2_to_eg1Sustain);
	//engineParams.setMM_ChannelEnable(kEG1_Normal, kEG1_Sustain, eg1_to_eg1Sustain);
	engineParams.setMM_ChannelEnable(kEG2_Normal, kEG1_Sustain, eg2_to_eg1Sustain);
	engineParams.setMM_ChannelEnable(kJoystickAC, kEG1_Sustain, joystickAC_to_eg1Sustain);
	engineParams.setMM_ChannelEnable(kJoystickBD, kEG1_Sustain, joystickBD_to_eg1Sustain);
	/*engineParams.setMM_ChannelEnable(kAuxUnipolarModOut1, kEG1_Sustain, auxUnipolar1Mod_to_eg1Sustain);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut2, kEG1_Sustain, auxUnipolar2Mod_to_eg1Sustain);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut3, kEG1_Sustain, auxUnipolar3Mod_to_eg1Sustain);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut4, kEG1_Sustain, auxUnipolar4Mod_to_eg1Sustain);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut5, kEG1_Sustain, auxUnipolar5Mod_to_eg1Sustain);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut6, kEG1_Sustain, auxUnipolar6Mod_to_eg1Sustain);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut1, kEG1_Sustain, auxBipolar1Mod_to_eg1Sustain);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut2, kEG1_Sustain, auxBipolar2Mod_to_eg1Sustain);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut3, kEG1_Sustain, auxBipolar3Mod_to_eg1Sustain);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut4, kEG1_Sustain, auxBipolar4Mod_to_eg1Sustain);*/

	//EG1 Release
	engineParams.setMM_ChannelEnable(kLFO1_Normal, kEG1_Release, lfo1_to_eg1Release);
	engineParams.setMM_ChannelEnable(kLFO2_Normal, kEG1_Release, lfo2_to_eg1Release);
	//engineParams.setMM_ChannelEnable(kEG1_Normal, kEG1_Release, eg1_to_eg1Release);
	engineParams.setMM_ChannelEnable(kEG2_Normal, kEG1_Release, eg2_to_eg1Release);
	engineParams.setMM_ChannelEnable(kJoystickAC, kEG1_Release, joystickAC_to_eg1Release);
	engineParams.setMM_ChannelEnable(kJoystickBD, kEG1_Release, joystickBD_to_eg1Release);
	/*engineParams.setMM_ChannelEnable(kAuxUnipolarModOut1, kEG1_Release, auxUnipolar1Mod_to_eg1Release);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut2, kEG1_Release, auxUnipolar2Mod_to_eg1Release);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut3, kEG1_Release, auxUnipolar3Mod_to_eg1Release);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut4, kEG1_Release, auxUnipolar4Mod_to_eg1Release);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut5, kEG1_Release, auxUnipolar5Mod_to_eg1Release);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut6, kEG1_Release, auxUnipolar6Mod_to_eg1Release);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut1, kEG1_Release, auxBipolar1Mod_to_eg1Release);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut2, kEG1_Release, auxBipolar2Mod_to_eg1Release);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut3, kEG1_Release, auxBipolar3Mod_to_eg1Release);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut4, kEG1_Release, auxBipolar4Mod_to_eg1Release);*/


	// -- Filters
	//Filter 1
	engineParams.setMM_ChannelEnable(kLFO1_Normal, kFilter1_fc, lfo1_to_filter1Fo);
	engineParams.setMM_ChannelEnable(kLFO2_Normal, kFilter1_fc, lfo2_to_filter1Fo);
	engineParams.setMM_ChannelEnable(kEG1_Normal, kFilter1_fc, eg1_to_filter1Fo);
	engineParams.setMM_ChannelEnable(kEG2_Normal, kFilter1_fc, eg2_to_filter1Fo);
	engineParams.setMM_ChannelEnable(kJoystickAC, kFilter1_fc, joystickAC_to_filter1Fo);
	engineParams.setMM_ChannelEnable(kJoystickBD, kFilter1_fc, joystickBD_to_filter1Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut1, kFilter1_fc, auxUnipolar1Mod_to_filter1Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut2, kFilter1_fc, auxUnipolar2Mod_to_filter1Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut3, kFilter1_fc, auxUnipolar3Mod_to_filter1Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut4, kFilter1_fc, auxUnipolar4Mod_to_filter1Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut5, kFilter1_fc, auxUnipolar5Mod_to_filter1Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut6, kFilter1_fc, auxUnipolar6Mod_to_filter1Fo);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut1, kFilter1_fc, auxBipolar1Mod_to_filter1Fo);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut2, kFilter1_fc, auxBipolar2Mod_to_filter1Fo);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut3, kFilter1_fc, auxBipolar3Mod_to_filter1Fo);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut4, kFilter1_fc, auxBipolar4Mod_to_filter1Fo);

	engineParams.setMM_ChannelEnable(kLFO1_Normal, kFilter1_Q, lfo1_to_filter1Q);
	engineParams.setMM_ChannelEnable(kLFO2_Normal, kFilter1_Q, lfo2_to_filter1Q);
	engineParams.setMM_ChannelEnable(kEG1_Normal, kFilter1_Q, eg1_to_filter1Q);
	engineParams.setMM_ChannelEnable(kEG2_Normal, kFilter1_Q, eg2_to_filter1Q);
	engineParams.setMM_ChannelEnable(kJoystickAC, kFilter1_Q, joystickAC_to_filter1Q);
	engineParams.setMM_ChannelEnable(kJoystickBD, kFilter1_Q, joystickBD_to_filter1Q);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut1, kFilter1_Q, auxUnipolar1Mod_to_filter1Q);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut2, kFilter1_Q, auxUnipolar2Mod_to_filter1Q);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut3, kFilter1_Q, auxUnipolar3Mod_to_filter1Q);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut4, kFilter1_Q, auxUnipolar4Mod_to_filter1Q);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut5, kFilter1_Q, auxUnipolar5Mod_to_filter1Q);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut6, kFilter1_Q, auxUnipolar6Mod_to_filter1Q);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut1, kFilter1_Q, auxBipolar1Mod_to_filter1Q);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut2, kFilter1_Q, auxBipolar2Mod_to_filter1Q);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut3, kFilter1_Q, auxBipolar3Mod_to_filter1Q);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut4, kFilter1_Q, auxBipolar4Mod_to_filter1Q);

	//Filter 2
	engineParams.setMM_ChannelEnable(kLFO1_Normal, kFilter2_fc, lfo1_to_filter2Fo);
	engineParams.setMM_ChannelEnable(kLFO2_Normal, kFilter2_fc, lfo2_to_filter2Fo);
	engineParams.setMM_ChannelEnable(kEG1_Normal, kFilter2_fc, eg1_to_filter2Fo);
	engineParams.setMM_ChannelEnable(kEG2_Normal, kFilter2_fc, eg2_to_filter2Fo);
	engineParams.setMM_ChannelEnable(kJoystickAC, kFilter2_fc, joystickAC_to_filter2Fo);
	engineParams.setMM_ChannelEnable(kJoystickBD, kFilter2_fc, joystickBD_to_filter2Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut1, kFilter2_fc, auxUnipolar1Mod_to_filter2Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut2, kFilter2_fc, auxUnipolar2Mod_to_filter2Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut3, kFilter2_fc, auxUnipolar3Mod_to_filter2Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut4, kFilter2_fc, auxUnipolar4Mod_to_filter2Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut5, kFilter2_fc, auxUnipolar5Mod_to_filter2Fo);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut6, kFilter2_fc, auxUnipolar6Mod_to_filter2Fo);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut1, kFilter2_fc, auxBipolar1Mod_to_filter2Fo);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut2, kFilter2_fc, auxBipolar2Mod_to_filter2Fo);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut3, kFilter2_fc, auxBipolar3Mod_to_filter2Fo);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut4, kFilter2_fc, auxBipolar4Mod_to_filter2Fo);

	engineParams.setMM_ChannelEnable(kLFO1_Normal, kFilter2_Q, lfo1_to_filter2Q);
	engineParams.setMM_ChannelEnable(kLFO2_Normal, kFilter2_Q, lfo2_to_filter2Q);
	engineParams.setMM_ChannelEnable(kEG1_Normal, kFilter2_Q, eg1_to_filter2Q);
	engineParams.setMM_ChannelEnable(kEG2_Normal, kFilter2_Q, eg2_to_filter2Q);
	engineParams.setMM_ChannelEnable(kJoystickAC, kFilter2_Q, joystickAC_to_filter2Q);
	engineParams.setMM_ChannelEnable(kJoystickBD, kFilter2_Q, joystickBD_to_filter2Q);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut1, kFilter2_Q, auxUnipolar1Mod_to_filter2Q);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut2, kFilter2_Q, auxUnipolar2Mod_to_filter2Q);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut3, kFilter2_Q, auxUnipolar3Mod_to_filter2Q);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut4, kFilter2_Q, auxUnipolar4Mod_to_filter2Q);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut5, kFilter2_Q, auxUnipolar5Mod_to_filter2Q);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut6, kFilter2_Q, auxUnipolar6Mod_to_filter2Q);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut1, kFilter2_Q, auxBipolar1Mod_to_filter2Q);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut2, kFilter2_Q, auxBipolar2Mod_to_filter2Q);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut3, kFilter2_Q, auxBipolar3Mod_to_filter2Q);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut4, kFilter2_Q, auxBipolar4Mod_to_filter2Q);

	
	// -- DCA
	
	//SampleHoldMod
	engineParams.setMM_ChannelEnable(kLFO1_Normal, kDCA_SampleHoldMod, lfo1_to_DCASampleHoldModIn);
	engineParams.setMM_ChannelEnable(kLFO2_Normal, kDCA_SampleHoldMod, lfo2_to_DCASampleHoldModIn);
	engineParams.setMM_ChannelEnable(kEG1_Normal, kDCA_SampleHoldMod, eg1_to_DCASampleHoldModIn);
	engineParams.setMM_ChannelEnable(kEG2_Normal, kDCA_SampleHoldMod, eg2_to_DCASampleHoldModIn);
	engineParams.setMM_ChannelEnable(kJoystickAC, kDCA_SampleHoldMod, joystickAC_to_DCASampleHoldModIn);
	engineParams.setMM_ChannelEnable(kJoystickBD, kDCA_SampleHoldMod, joystickBD_to_DCASampleHoldModIn);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut1, kDCA_SampleHoldMod, auxUnipolar1Mod_to_DCASampleHoldModIn);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut2, kDCA_SampleHoldMod, auxUnipolar2Mod_to_DCASampleHoldModIn);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut3, kDCA_SampleHoldMod, auxUnipolar3Mod_to_DCASampleHoldModIn);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut4, kDCA_SampleHoldMod, auxUnipolar4Mod_to_DCASampleHoldModIn);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut5, kDCA_SampleHoldMod, auxUnipolar5Mod_to_DCASampleHoldModIn);
	engineParams.setMM_ChannelEnable(kAuxUnipolarModOut6, kDCA_SampleHoldMod, auxUnipolar6Mod_to_DCASampleHoldModIn);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut1, kDCA_SampleHoldMod, auxBipolar1Mod_to_DCASampleHoldModIn);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut2, kDCA_SampleHoldMod, auxBipolar2Mod_to_DCASampleHoldModIn);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut3, kDCA_SampleHoldMod, auxBipolar3Mod_to_DCASampleHoldModIn);
	engineParams.setMM_ChannelEnable(kAuxBipolarModOut4, kDCA_SampleHoldMod, auxBipolar4Mod_to_DCASampleHoldModIn);


	// -- Globals
	//Unison Detune
	engineParams.setMM_ChannelEnable(kLFO1_Normal, kDCA_SampleHoldMod, lfo1_to_unisonDetune);
	engineParams.setMM_ChannelEnable(kLFO2_Normal, kDCA_SampleHoldMod, lfo2_to_unisonDetune);
	engineParams.setMM_ChannelEnable(kEG1_Normal, kDCA_SampleHoldMod, eg1_to_unisonDetune);
	engineParams.setMM_ChannelEnable(kEG2_Normal, kDCA_SampleHoldMod, eg2_to_unisonDetune);
	engineParams.setMM_ChannelEnable(kJoystickAC, kDCA_SampleHoldMod, joystickAC_to_unisonDetune);
	engineParams.setMM_ChannelEnable(kJoystickBD, kDCA_SampleHoldMod, joystickBD_to_unisonDetune);
	//engineParams.setMM_ChannelEnable(kAuxUnipolarModOut1, kDCA_SampleHoldMod, auxUnipolar1Mod_to_unisonDetune);
	//engineParams.setMM_ChannelEnable(kAuxUnipolarModOut2, kDCA_SampleHoldMod, auxUnipolar2Mod_to_unisonDetune);
	//engineParams.setMM_ChannelEnable(kAuxUnipolarModOut3, kDCA_SampleHoldMod, auxUnipolar3Mod_to_unisonDetune);
	//engineParams.setMM_ChannelEnable(kAuxUnipolarModOut4, kDCA_SampleHoldMod, auxUnipolar4Mod_to_unisonDetune);
	//engineParams.setMM_ChannelEnable(kAuxUnipolarModOut5, kDCA_SampleHoldMod, auxUnipolar5Mod_to_unisonDetune);
	//engineParams.setMM_ChannelEnable(kAuxUnipolarModOut6, kDCA_SampleHoldMod, auxUnipolar6Mod_to_unisonDetune);
	//engineParams.setMM_ChannelEnable(kAuxBipolarModOut1, kDCA_SampleHoldMod, auxBipolar1Mod_to_unisonDetune);
	//engineParams.setMM_ChannelEnable(kAuxBipolarModOut2, kDCA_SampleHoldMod, auxBipolar2Mod_to_unisonDetune);
	//engineParams.setMM_ChannelEnable(kAuxBipolarModOut3, kDCA_SampleHoldMod, auxBipolar3Mod_to_unisonDetune);
	//engineParams.setMM_ChannelEnable(kAuxBipolarModOut4, kDCA_SampleHoldMod, auxBipolar4Mod_to_unisonDetune);

// Synth Mode; Unison, Mono, Poly
	engineParams.mode = convertIntToEnum(mode, synthMode);
	engineParams.masterUnisonDetune_Cents = unisonDetune_Cents;


	// Physical Modeling
	engineParams.voiceParameters->osc1Parameters->exciterInput = convertIntToEnum(osc1ExciterMode, ExciterMode);
	engineParams.voiceParameters->osc1Parameters->noiseEGAttack = exciter1Attack_msec;
	engineParams.voiceParameters->osc1Parameters->noiseEGHold = exciter1Hold_msec;
	engineParams.voiceParameters->osc1Parameters->noiseEGRelease = exciter1Release_msec;
	engineParams.voiceParameters->osc1Parameters->resonatorDecay = resonator1Decay;

	engineParams.voiceParameters->osc2Parameters->exciterInput = convertIntToEnum(osc2ExciterMode, ExciterMode);
	engineParams.voiceParameters->osc2Parameters->noiseEGAttack = exciter2Attack_msec;
	engineParams.voiceParameters->osc2Parameters->noiseEGHold = exciter2Hold_msec;
	engineParams.voiceParameters->osc2Parameters->noiseEGRelease = exciter2Release_msec;
	engineParams.voiceParameters->osc2Parameters->resonatorDecay = resonator2Decay;

	engineParams.voiceParameters->osc3Parameters->exciterInput = convertIntToEnum(osc3ExciterMode, ExciterMode);
	engineParams.voiceParameters->osc3Parameters->noiseEGAttack = exciter3Attack_msec;
	engineParams.voiceParameters->osc3Parameters->noiseEGHold = exciter3Hold_msec;
	engineParams.voiceParameters->osc3Parameters->noiseEGRelease = exciter3Release_msec;
	engineParams.voiceParameters->osc3Parameters->resonatorDecay = resonator3Decay;

	engineParams.voiceParameters->osc4Parameters->exciterInput = convertIntToEnum(osc4ExciterMode, ExciterMode);
	engineParams.voiceParameters->osc4Parameters->noiseEGAttack = exciter4Attack_msec;
	engineParams.voiceParameters->osc4Parameters->noiseEGHold = exciter4Hold_msec;
	engineParams.voiceParameters->osc4Parameters->noiseEGRelease = exciter4Release_msec;
	engineParams.voiceParameters->osc4Parameters->resonatorDecay = resonator4Decay;

	//engineParams.temperament = temperamentList;
	engineParams.masterUnisonDetune_Cents = unisonDetune_Cents;

	

	// --- THE update - this trickles all param updates
	// via the setParameters( ) of each
	synthEngine.setParameters(engineParams);
}

/**
\brief frame-processing method

Operation:
- decode the plugin type - for synth plugins, fill in the rendering code; for FX plugins, delete the if(synth) portion and add your processing code
- note that MIDI events are fired for each sample interval so that MIDI is tightly sunk with audio
- doSampleAccurateParameterUpdates will perform per-sample interval smoothing

\param processFrameInfo structure of information about *frame* processing

\return true if operation succeeds, false otherwise
*/
bool PluginCore::processAudioFrame(ProcessFrameInfo& processFrameInfo)
{
    // --- fire any MIDI events for this sample interval
    processFrameInfo.midiEventQueue->fireMidiEvents(processFrameInfo.currentFrame);

	bpm = processFrameInfo.hostInfo->dBPM;

	// --- do per-frame updates; VST automation and parameter smoothing
	doSampleAccurateParameterUpdates();

    // --- decode the channelIOConfiguration and process accordingly
    //
	// --- Synth Plugin:
	// --- Synth Plugin --- remove for FX plugins
	if (getPluginType() == kSynthPlugin)
	{
		// --- do the synth render
		const SynthRenderData render = synthEngine.renderAudioOutput();
		// --- write left channel
		processFrameInfo.audioOutputFrame[0] = render.synthOutputs[0];
		// --- write right channel
		if (processFrameInfo.channelIOConfig.outputChannelFormat ==
			kCFStereo)
			processFrameInfo.audioOutputFrame[1] =
			render.synthOutputs[1];

		return true;	/// processed
	}

    return false; /// NOT processed
}


/**
\brief do anything needed prior to arrival of audio buffers

Operation:
- updateOutBoundVariables sends metering data to the GUI meters

\param processInfo structure of information about *buffer* processing

\return true if operation succeeds, false otherwise
*/
bool PluginCore::postProcessAudioBuffers(ProcessBufferInfo& processInfo)
{
	// --- update outbound variables; currently this is meter data only, but could be extended
	//     in the future
	updateOutBoundVariables();

    return true;
}

/**
\brief update the PluginParameter's value based on GUI control, preset, or data smoothing (thread-safe)

Operation:
- update the parameter's value (with smoothing this initiates another smoothing process)
- call postUpdatePluginParameter to do any further processing

\param controlID the control ID value of the parameter being updated
\param controlValue the new control value
\param paramInfo structure of information about why this value is being udpated (e.g as a result of a preset being loaded vs. the top of a buffer process cycle)

\return true if operation succeeds, false otherwise
*/
bool PluginCore::updatePluginParameter(int32_t controlID, double controlValue, ParameterUpdateInfo& paramInfo)
{
    // --- use base class helper
    setPIParamValue(controlID, controlValue);

    // --- do any post-processing
    postUpdatePluginParameter(controlID, controlValue, paramInfo);

    return true; /// handled
}

/**
\brief update the PluginParameter's value based on *normlaized* GUI control, preset, or data smoothing (thread-safe)

Operation:
- update the parameter's value (with smoothing this initiates another smoothing process)
- call postUpdatePluginParameter to do any further processing

\param controlID the control ID value of the parameter being updated
\param normalizedValue the new control value in normalized form
\param paramInfo structure of information about why this value is being udpated (e.g as a result of a preset being loaded vs. the top of a buffer process cycle)

\return true if operation succeeds, false otherwise
*/
bool PluginCore::updatePluginParameterNormalized(int32_t controlID, double normalizedValue, ParameterUpdateInfo& paramInfo)
{
	// --- use base class helper, returns actual value
	double controlValue = setPIParamValueNormalized(controlID, normalizedValue, paramInfo.applyTaper);

	// --- do any post-processing
	postUpdatePluginParameter(controlID, controlValue, paramInfo);

	return true; /// handled
}

/**
\brief perform any operations after the plugin parameter has been updated; this is one paradigm for
	   transferring control information into vital plugin variables or member objects. If you use this
	   method you can decode the control ID and then do any cooking that is needed. NOTE: do not
	   overwrite bound variables here - this is ONLY for any extra cooking that is required to convert
	   the GUI data to meaninful coefficients or other specific modifiers.

\param controlID the control ID value of the parameter being updated
\param controlValue the new control value
\param paramInfo structure of information about why this value is being udpated (e.g as a result of a preset being loaded vs. the top of a buffer process cycle)

\return true if operation succeeds, false otherwise
*/
bool PluginCore::postUpdatePluginParameter(int32_t controlID, double controlValue, ParameterUpdateInfo& paramInfo)
{
    // --- now do any post update cooking; be careful with VST Sample Accurate automation
    //     If enabled, then make sure the cooking functions are short and efficient otherwise disable it
    //     for the Parameter involved
    /*switch(controlID)
    {
        case 0:
        {
            return true;    /// handled
        }

        default:
            return false;   /// not handled
    }*/

    return false;
}

/**
\brief has nothing to do with actual variable or updated variable (binding)

CAUTION:
- DO NOT update underlying variables here - this is only for sending GUI updates or letting you
  know that a parameter was changed; it should not change the state of your plugin.

WARNING:
- THIS IS NOT THE PREFERRED WAY TO LINK OR COMBINE CONTROLS TOGETHER. THE PROPER METHOD IS
  TO USE A CUSTOM SUB-CONTROLLER THAT IS PART OF THE GUI OBJECT AND CODE.
  SEE http://www.willpirkle.com for more information

\param controlID the control ID value of the parameter being updated
\param actualValue the new control value

\return true if operation succeeds, false otherwise
*/
bool PluginCore::guiParameterChanged(int32_t controlID, double actualValue)
{
	/*
	switch (controlID)
	{
		case controlID::<your control here>
		{

			return true; // handled
		}

		default:
			break;
	}*/

	return false; /// not handled
}

/**
\brief For Custom View and Custom Sub-Controller Operations

NOTES:
- this is for advanced users only to implement custom view and custom sub-controllers
- see the SDK for examples of use

\param messageInfo a structure containing information about the incoming message

\return true if operation succeeds, false otherwise
*/
bool PluginCore::processMessage(MessageInfo& messageInfo)
{
	// --- decode message
	switch (messageInfo.message)
	{
		// --- add customization appearance here
	case PLUGINGUI_DIDOPEN:
	{
		if (bankAndWaveGroup_0)
			bankAndWaveGroup_0->updateView();

		return false;
	}

	// --- NULL pointers so that we don't accidentally use them
	case PLUGINGUI_WILLCLOSE:
	{
		return false;
	}

	// --- update view; this will only be called if the GUI is actually open
	case PLUGINGUI_TIMERPING:
	{
		return false;
	}

	// --- register the custom view, grab the ICustomView interface
	case PLUGINGUI_REGISTER_CUSTOMVIEW:
	{

		return false;
	}

	case PLUGINGUI_REGISTER_SUBCONTROLLER:
	{
		// --- decode name string
		if (messageInfo.inMessageString.compare("BankWaveController_0") == 0)
		{
			// --- (1) get the custom view interface via incoming message data*
			//         can use it for communication

			if (bankAndWaveGroup_0 != static_cast<ICustomView*>(messageInfo.inMessageData))
				bankAndWaveGroup_0 = static_cast<ICustomView*>(messageInfo.inMessageData);

			if (!bankAndWaveGroup_0) return false;


			// --- need to tell the subcontroller the bank names
			VSTGUI::BankWaveMessage subcontrollerMessage;
			subcontrollerMessage.message = VSTGUI::UPDATE_BANK_NAMES;

			subcontrollerMessage.bankNames = synthEngine.getBankNames(0, 0); // voice, voice-oscillator 
			bankAndWaveGroup_0->sendMessage(&subcontrollerMessage);

			// --- now add the wave names for each bank
			subcontrollerMessage.message = VSTGUI::ADD_BANK_WAVENAMES;

			// --- bank 0
			subcontrollerMessage.bankIndex = 0;
			subcontrollerMessage.waveformNames = synthEngine.getOscWaveformNames(0, 0, 0); // voice, voice-oscillator, bank 

			bankAndWaveGroup_0->sendMessage(&subcontrollerMessage);

			// --- bank 1
			subcontrollerMessage.bankIndex = 1;
			subcontrollerMessage.waveformNames = synthEngine.getOscWaveformNames(0, 0, 1); // voice,  voice-oscillator, bank 

			bankAndWaveGroup_0->sendMessage(&subcontrollerMessage);

			// --- registered!
			return true;
		}
	}

	case PLUGINGUI_QUERY_HASUSERCUSTOM:
	case PLUGINGUI_USER_CUSTOMOPEN:
	case PLUGINGUI_USER_CUSTOMCLOSE:
	case PLUGINGUI_EXTERNAL_SET_NORMVALUE:
	case PLUGINGUI_EXTERNAL_SET_ACTUALVALUE:
	{

		return false;
	}

	default:
		break;
	}

	return false; /// not handled
}


/**
\brief process a MIDI event

NOTES:
- MIDI events are 100% sample accurate; this function will be called repeatedly for every MIDI message
- see the SDK for examples of use

\param event a structure containing the MIDI event data

\return true if operation succeeds, false otherwise
*/
bool PluginCore::processMIDIEvent(midiEvent& event)
{
	// --- send directly to engine for processing
	synthEngine.processMIDIEvent(event);

	return true;
}

/**
\brief (for future use)

NOTES:
- MIDI events are 100% sample accurate; this function will be called repeatedly for every MIDI message
- see the SDK for examples of use

\param vectorJoysickData a structure containing joystick data

\return true if operation succeeds, false otherwise
*/
bool PluginCore::setVectorJoystickParameters(const VectorJoystickData& vectorJoysickData)
{
	SynthEngineParameters engineParams = synthEngine.getParameters();
	engineParams.voiceParameters->vectorJSData = vectorJoysickData;
	synthEngine.setParameters(engineParams);
	return true;
}

/**
\brief use this method to add new presets to the list

NOTES:
- see the SDK for examples of use
- for non RackAFX users that have large paramter counts, there is a secret GUI control you
  can enable to write C++ code into text files, one per preset. See the SDK or http://www.willpirkle.com for details

\return true if operation succeeds, false otherwise
*/
bool PluginCore::initPluginPresets()
{
	// **--0xFF7A--**

	// --- Plugin Presets 
	int index = 0;
	PresetInfo* preset = nullptr;

	// --- Preset: Factory Preset
	preset = new PresetInfo(index++, "Factory Preset");
	initPresetParameters(preset->presetParameters);
	setPresetParameter(preset->presetParameters, controlID::lfo1Waveform, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1Mode, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1Frequency_Hz, 0.500000);
	setPresetParameter(preset->presetParameters, controlID::masterPitchBend, 7.000000);
	setPresetParameter(preset->presetParameters, controlID::masterTune, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::masterVolume_dB, 3.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1DelayTime_mSec, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1RampTime_mSec, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2Mode, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2Waveform, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2Frequency_Hz, 0.020000);
	setPresetParameter(preset->presetParameters, controlID::lfo1Shape, 0.500000);
	setPresetParameter(preset->presetParameters, controlID::lfo1ShapeSplit, 0.500000);
	setPresetParameter(preset->presetParameters, controlID::ScaleMode, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::enableRhythmicBreaks, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2Shape, 0.500000);
	setPresetParameter(preset->presetParameters, controlID::lfo2ShapeSplit, 0.500000);
	setPresetParameter(preset->presetParameters, controlID::lfo2ModTarget, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::osc1Waveform, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::osc1BankIndex, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::osc2Waveform, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::osc3Waveform, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::osc4Waveform, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::osc2BankIndex, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::osc3BankIndex, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::osc4BankIndex, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::osc1Detune_cents, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::fc1_hertz, 880.000000);
	setPresetParameter(preset->presetParameters, controlID::q1Control, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1Amplitude, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::osc1FoModIn, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::enableKeyTrack, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::keyTrackRatio, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1_to_osc1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2DelayTime_mSec, 10.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2AttackTime_mSec, 10.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2HoldTime_mSec, 10.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2DecayTime_mSec, 100.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2SustainLevel, 0.707000);
	setPresetParameter(preset->presetParameters, controlID::eg2ReleaseTime_mSec, 1000.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2AutoTrigger, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2ManualTrigger, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1DelayTime_mSec, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1AttackTime_mSec, 5.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1HoldTime_mSec, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1DecayTime_mSec, 100.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1SustainLevel, 0.707000);
	setPresetParameter(preset->presetParameters, controlID::eg1ReleaseTime_mSec, 1000.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1AutoRetrigger, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1ManualTrigger, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1Offset, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2Offset, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1Mode, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2Mode, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::mode, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::unisonDetune_Cents, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1ModOut, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1_to_osc1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2Amplitude, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2ModOut, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickACModOut, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickBDModOut, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1_to_osc2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::osc2FoModIn, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::osc1ShapeModIn, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::osc2ShapeModIn, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::filter1FoModIn, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::filter1QModIn, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::filter2FoModIn, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::filter2QModIn, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1_to_osc1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1_to_osc2Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1_to_filter1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1_to_filter1Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1_to_filter2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1_to_filter2Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2_to_osc1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2_to_osc1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2_to_osc2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2_to_osc2Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2_to_filter1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2_to_filter1Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2_to_filter2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2_to_filter2Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1_to_osc1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1_to_osc2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1_to_osc2Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1_to_filter1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1_to_filter1Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1_to_filter2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1_to_filter2Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2_to_osc1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2_to_osc1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2_to_osc2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2_to_osc2Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2_to_filter1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2_to_filter1Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2_to_filter2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2_to_filter2Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickAC_to_osc1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickAC_to_osc1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickAC_to_osc2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickAC_to_osc2Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickAC_to_filter1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickAC_to_filter1Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickAC_to_filter2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickAC_to_filter2Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickBD_to_osc1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickBD_to_osc1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickBD_to_osc2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickBD_to_osc2Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickBD_to_filter1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickBD_to_filter1Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickBD_to_filter2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickBD_to_filter2Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar1ModOut, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar2ModOut, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar3ModOut, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar4ModOut, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar5ModOut, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar6ModOut, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar1ModOut, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar2ModOut, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar3ModOut, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar4ModOut, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::DCASampleHoldModIn, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1_to_DCASampleHoldModIn, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2_to_DCASampleHoldModIn, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickAC_to_DCASampleHoldModIn, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickBD_to_DCASampleHoldModIn, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar1Mod_to_osc1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar1Mod_to_osc1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar1Mod_to_osc2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar1Mod_to_osc2Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar1Mod_to_filter1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar1Mod_to_filter1Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar1Mod_to_filter2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar1Mod_to_filter2Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar1Mod_to_DCASampleHoldModIn, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar2Mod_to_osc1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar2Mod_to_osc1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar2Mod_to_osc2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar2Mod_to_osc2Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar2Mod_to_filter1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar2Mod_to_filter1Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar2Mod_to_filter2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar2Mod_to_filter2Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar2Mod_to_DCASampleHoldModIn, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar3Mod_to_osc1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar3Mod_to_osc1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar3Mod_to_osc2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar3Mod_to_osc2Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar3Mod_to_filter1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar3Mod_to_filter1Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar3Mod_to_filter2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar3Mod_to_filter2Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar3Mod_to_DCASampleHoldModIn, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar4Mod_to_osc1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar4Mod_to_osc1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar4Mod_to_osc2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar4Mod_to_osc2Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar4Mod_to_filter1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar4Mod_to_filter1Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar4Mod_to_filter2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar4Mod_to_filter2Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar4Mod_to_DCASampleHoldModIn, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar5Mod_to_osc1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar5Mod_to_osc1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar5Mod_to_osc2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar5Mod_to_osc2Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar5Mod_to_filter1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar5Mod_to_filter1Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar5Mod_to_filter2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar5Mod_to_filter2Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar5Mod_to_DCASampleHoldModIn, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar6Mod_to_osc1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar6Mod_to_osc1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar6Mod_to_osc2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar6Mod_to_osc2Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar6Mod_to_filter1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar6Mod_to_filter1Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar6Mod_to_filter2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar6Mod_to_filter2Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar6Mod_to_DCASampleHoldModIn, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar1Mod_to_osc1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar1Mod_to_osc1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar1Mod_to_osc2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar1Mod_to_osc2Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar1Mod_to_filter1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar1Mod_to_filter1Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar1Mod_to_filter2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar1Mod_to_filter2Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar1Mod_to_DCASampleHoldModIn, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar2Mod_to_osc1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar2Mod_to_osc1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar2Mod_to_osc2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar2Mod_to_osc2Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar2Mod_to_filter1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar2Mod_to_filter1Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar2Mod_to_filter2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar2Mod_to_filter2Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar2Mod_to_DCASampleHoldModIn, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar3Mod_to_osc1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar3Mod_to_osc1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar3Mod_to_osc2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar3Mod_to_osc2Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar3Mod_to_filter1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar3Mod_to_filter1Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar3Mod_to_filter2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar3Mod_to_filter2Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar3Mod_to_DCASampleHoldModIn, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar4Mod_to_osc1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar4Mod_to_osc1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar4Mod_to_osc2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar4Mod_to_osc2Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar4Mod_to_filter1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar4Mod_to_filter1Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar4Mod_to_filter2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar4Mod_to_filter2Q, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar4Mod_to_DCASampleHoldModIn, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1_to_DCASampleHoldModIn, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2_to_DCASampleHoldModIn, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1ShapeModIn, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1ShapeSplitModIn, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2ShapeModIn, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2ShapeSplitModIn, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1AttackModIn, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1HoldModIn, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1DecayModIn, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1SustainModIn, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1ReleaseModIn, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::unisonDetuneModIn, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1_to_lfo1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2_to_lfo1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1_to_lfo1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2_to_lfo1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickAC_to_lfo1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickBD_to_lfo1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar1Mod_to_lfo1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar2Mod_to_lfo1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar3Mod_to_lfo1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar4Mod_to_lfo1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar5Mod_to_lfo1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxUnipolar6Mod_to_lfo1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar1Mod_to_lfo1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar2Mod_to_lfo1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar3Mod_to_lfo1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::auxBipolar4Mod_to_lfo1Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1_to_unisonDetune, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2_to_unisonDetune, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1_to_unisonDetune, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2_to_unisonDetune, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickAC_to_unisonDetune, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickBD_to_unisonDetune, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1_to_lfo1ShapeSplit, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2_to_lfo1ShapeSplit, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1_to_lfo1ShapeSplit, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2_to_lfo1ShapeSplit, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickAC_to_lfo1ShapeSplit, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickBD_to_lfo1ShapeSplit, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1_to_lfo2Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2_to_lfo2Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1_to_lfo2Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2_to_lfo2Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickAC_to_lfo2Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickBD_to_lfo2Shape, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1_to_lfo2ShapeSplit, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2_to_lfo2ShapeSplit, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1_to_lfo2ShapeSplit, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2_to_lfo2ShapeSplit, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickAC_to_lfo2ShapeSplit, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickBD_to_lfo2ShapeSplit, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1_to_eg1Attack, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2_to_eg1Attack, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickAC_to_eg1Attack, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickBD_to_eg1Attack, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2_to_eg1Attack, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1_to_eg1Hold, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2_to_eg1Hold, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2_to_eg1Hold, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickAC_to_eg1Hold, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickBD_to_eg1Hold, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1_to_eg1Decay, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2_to_eg1Decay, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2_to_eg1Decay, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickAC_to_eg1Decay, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickBD_to_eg1Decay, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1_to_eg1Sustain, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2_to_eg1Sustain, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2_to_eg1Sustain, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickAC_to_eg1Sustain, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickBD_to_eg1Sustain, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1_to_eg1Release, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo2_to_eg1Release, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg2_to_eg1Release, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickAC_to_eg1Release, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickBD_to_eg1Release, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::temperamentKeySelection, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::osc1ExciterMode, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::exciter1Attack_msec, 100.000000);
	setPresetParameter(preset->presetParameters, controlID::exciter1Hold_msec, 250.000000);
	setPresetParameter(preset->presetParameters, controlID::exciter1Release_msec, 1000.000000);
	setPresetParameter(preset->presetParameters, controlID::resonator1Decay, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::osc2ExciterMode, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::osc3ExciterMode, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::osc4ExciterMode, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::exciter2Attack_msec, 100.000000);
	setPresetParameter(preset->presetParameters, controlID::exciter2Hold_msec, 250.000000);
	setPresetParameter(preset->presetParameters, controlID::exciter2Release_msec, 1000.000000);
	setPresetParameter(preset->presetParameters, controlID::resonator2Decay, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::exciter3Attack_msec, 100.000000);
	setPresetParameter(preset->presetParameters, controlID::exciter3Hold_msec, 250.000000);
	setPresetParameter(preset->presetParameters, controlID::exciter3Release_msec, 1000.000000);
	setPresetParameter(preset->presetParameters, controlID::resonator3Decay, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::exciter4Attack_msec, 100.000000);
	setPresetParameter(preset->presetParameters, controlID::exciter4Hold_msec, 250.000000);
	setPresetParameter(preset->presetParameters, controlID::exciter4Release_msec, 1000.000000);
	setPresetParameter(preset->presetParameters, controlID::resonator4Decay, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::fc2_hertz, 880.000000);
	setPresetParameter(preset->presetParameters, controlID::q2Control, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::filter1Algorithm, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::filter2Algorithm, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::enableGainCompensation, -0.000000);
	addPreset(preset);


	// **--0xA7FF--**

    return true;
}

/**
\brief setup the plugin description strings, flags and codes; this is ordinarily done through the ASPiKreator or CMake

\return true if operation succeeds, false otherwise
*/
bool PluginCore::initPluginDescriptors()
{
    pluginDescriptor.pluginName = PluginCore::getPluginName();
    pluginDescriptor.shortPluginName = PluginCore::getShortPluginName();
    pluginDescriptor.vendorName = PluginCore::getVendorName();
    pluginDescriptor.pluginTypeCode = PluginCore::getPluginType();

	// --- describe the plugin attributes; set according to your needs
	pluginDescriptor.hasSidechain = kWantSidechain;
	pluginDescriptor.latencyInSamples = kLatencyInSamples;
	pluginDescriptor.tailTimeInMSec = kTailTimeMsec;
	pluginDescriptor.infiniteTailVST3 = kVSTInfiniteTail;

    // --- AAX
    apiSpecificInfo.aaxManufacturerID = kManufacturerID;
    apiSpecificInfo.aaxProductID = kAAXProductID;
    apiSpecificInfo.aaxBundleID = kAAXBundleID;  /* MacOS only: this MUST match the bundle identifier in your info.plist file */
    apiSpecificInfo.aaxEffectID = "aaxDeveloper.";
    apiSpecificInfo.aaxEffectID.append(PluginCore::getPluginName());
    apiSpecificInfo.aaxPluginCategoryCode = kAAXCategory;

    // --- AU
    apiSpecificInfo.auBundleID = kAUBundleID;   /* MacOS only: this MUST match the bundle identifier in your info.plist file */

    // --- VST3
    apiSpecificInfo.vst3FUID = PluginCore::getVSTFUID(); // OLE string format
    apiSpecificInfo.vst3BundleID = kVST3BundleID;/* MacOS only: this MUST match the bundle identifier in your info.plist file */
	apiSpecificInfo.enableVST3SampleAccurateAutomation = kVSTSAA;
	apiSpecificInfo.vst3SampleAccurateGranularity = kVST3SAAGranularity;

    // --- AU and AAX
    apiSpecificInfo.fourCharCode = PluginCore::getFourCharCode();

    return true;
}

// --- static functions required for VST3/AU only --------------------------------------------- //
const char* PluginCore::getPluginBundleName() { return kAUBundleName; }
const char* PluginCore::getPluginName(){ return kPluginName; }
const char* PluginCore::getShortPluginName(){ return kShortPluginName; }
const char* PluginCore::getVendorName(){ return kVendorName; }
const char* PluginCore::getVendorURL(){ return kVendorURL; }
const char* PluginCore::getVendorEmail(){ return kVendorEmail; }
const char* PluginCore::getAUCocoaViewFactoryName(){ return AU_COCOA_VIEWFACTORY_STRING; }
pluginType PluginCore::getPluginType(){ return kPluginType; }
const char* PluginCore::getVSTFUID(){ return kVSTFUID; }
int32_t PluginCore::getFourCharCode(){ return kFourCharCode; }
