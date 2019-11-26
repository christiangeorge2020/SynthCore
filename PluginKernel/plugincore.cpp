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
	piParam = new PluginParameter(controlID::scaleMode, "Scale", "None,Ionian,Dorian,Phrygian,Lydian,Mixolydian,Aeolian,Locrian,Chromatic", "None");
	piParam->setBoundVariable(&scaleMode, boundVariableType::kInt);
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

	// --- continuous control: JS X Rate
	piParam = new PluginParameter(controlID::joystickX_rate, "JS X Rate", "hertz", controlVariableType::kDouble, 0.000000, 20.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&joystickX_rate, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: JS X Amp
	piParam = new PluginParameter(controlID::joystickX_amp, "JS X Amp", "", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&joystickX_amp, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: JS Y Rate
	piParam = new PluginParameter(controlID::joystickY_rate, "JS Y Rate", "hertz", controlVariableType::kDouble, 0.000000, 20.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&joystickY_rate, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: JS Y Amp
	piParam = new PluginParameter(controlID::joystickY_amp, "JS Y Amp", "", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&joystickY_amp, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: EG1 Attack
	piParam = new PluginParameter(controlID::eg1AttackTime_mSec, "EG1 Attack", "mSec", controlVariableType::kDouble, 0.000000, 2000.000000, 5.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg1AttackTime_mSec, boundVariableType::kDouble);
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

	// --- continuous control: EG1 Hold
	piParam = new PluginParameter(controlID::eg1HoldTime_mSec, "EG1 Hold", "msec", controlVariableType::kDouble, 0.000000, 5000.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg1HoldTime_mSec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: EG1 Delay
	piParam = new PluginParameter(controlID::eg1DelayTime_mSec, "EG1 Delay", "msec", controlVariableType::kDouble, 0.000000, 100.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg1DelayTime_mSec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: EG1 Mode
	piParam = new PluginParameter(controlID::eg1Mode, "EG1 Mode", "ADSR,AHDSR,AHR", "ADSR");
	piParam->setBoundVariable(&eg1Mode, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG1 AutoTrigg
	piParam = new PluginParameter(controlID::eg1AutoRetrigger, "EG1 AutoTrigg", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg1AutoRetrigger, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG1 Trigger
	piParam = new PluginParameter(controlID::eg1ManualTrigger, "EG1 Trigger", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg1ManualTrigger, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: FC 1
	piParam = new PluginParameter(controlID::fc1_hertz, "FC 1", "Hertz", controlVariableType::kDouble, 20.000000, 18000.000000, 880.000000, taper::kVoltOctaveTaper);
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

	// --- continuous control: EG1 Out
	piParam = new PluginParameter(controlID::eg1ModOut, "EG1 Out", "", controlVariableType::kDouble, -1.000000, 1.000000, 1.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg1ModOut, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Osc1 Fo In
	piParam = new PluginParameter(controlID::osc1FoModIn, "Osc1 Fo In", "", controlVariableType::kDouble, -1.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&osc1FoModIn, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Osc2 Fo In
	piParam = new PluginParameter(controlID::osc2FoModIn, "Osc2 Fo In", "", controlVariableType::kDouble, -1.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&osc2FoModIn, boundVariableType::kDouble);
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

	// --- discrete control: LFO1
	piParam = new PluginParameter(controlID::lfo1_to_osc2Fo, "LFO1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&lfo1_to_osc2Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: EG1
	piParam = new PluginParameter(controlID::eg1_to_osc1Fo, "EG1", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&eg1_to_osc1Fo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: EG1 Offset
	piParam = new PluginParameter(controlID::eg1Offset, "EG1 Offset", "", controlVariableType::kDouble, -1.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(false);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&eg1Offset, boundVariableType::kDouble);
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

	// --- controlID::scaleMode
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::scaleMode, auxAttribute);

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

	// --- controlID::joystickX_rate
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::joystickX_rate, auxAttribute);

	// --- controlID::joystickX_amp
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::joystickX_amp, auxAttribute);

	// --- controlID::joystickY_rate
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::joystickY_rate, auxAttribute);

	// --- controlID::joystickY_amp
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::joystickY_amp, auxAttribute);

	// --- controlID::eg1AttackTime_mSec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg1AttackTime_mSec, auxAttribute);

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

	// --- controlID::eg1HoldTime_mSec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg1HoldTime_mSec, auxAttribute);

	// --- controlID::eg1DelayTime_mSec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg1DelayTime_mSec, auxAttribute);

	// --- controlID::eg1Mode
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::eg1Mode, auxAttribute);

	// --- controlID::eg1AutoRetrigger
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741824);
	setParamAuxAttribute(controlID::eg1AutoRetrigger, auxAttribute);

	// --- controlID::eg1ManualTrigger
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1610612738);
	setParamAuxAttribute(controlID::eg1ManualTrigger, auxAttribute);

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

	// --- controlID::eg1ModOut
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg1ModOut, auxAttribute);

	// --- controlID::osc1FoModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::osc1FoModIn, auxAttribute);

	// --- controlID::osc2FoModIn
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::osc2FoModIn, auxAttribute);

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

	// --- controlID::lfo1_to_osc2Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::lfo1_to_osc2Fo, auxAttribute);

	// --- controlID::eg1_to_osc1Fo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741827);
	setParamAuxAttribute(controlID::eg1_to_osc1Fo, auxAttribute);

	// --- controlID::eg1Offset
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::eg1Offset, auxAttribute);


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
	engineParams.voiceParameters->lfo1Parameters->modRoute = convertIntToEnum(lfo2ModTarget, ModRouting);

	/// LFO 2 Parameters
	engineParams.voiceParameters->lfo2Parameters->frequency_Hz = lfo2Frequency_Hz;
	engineParams.voiceParameters->lfo2Parameters->waveform = convertIntToEnum(lfo2Waveform, LFOWaveform);
	engineParams.voiceParameters->lfo2Parameters->mode = convertIntToEnum(lfo2Mode, LFOMode);
	engineParams.voiceParameters->lfo2Parameters->lfoShape = lfo2Shape;
	engineParams.voiceParameters->lfo2Parameters->shapeSplitpoint = lfo2ShapeSplit;

	//engineParams.voiceParameters->osc1Parameters->scaleSelect = convertIntToEnum(scaleMode, ScaleMode);
	
	engineParams.voiceParameters->dcaParameters->bpm = bpm;

	engineParams.voiceParameters->dcaParameters->modRoute = convertIntToEnum(lfo2ModTarget, ModRouting);

	engineParams.voiceParameters->osc1Parameters->oscillatorWaveformIndex = osc1Waveform;
	engineParams.voiceParameters->osc1Parameters->oscillatorBankIndex = osc1BankIndex;
	engineParams.voiceParameters->osc2Parameters->oscillatorWaveformIndex = osc2Waveform;
	engineParams.voiceParameters->osc2Parameters->oscillatorBankIndex = osc2BankIndex;
	engineParams.voiceParameters->osc3Parameters->oscillatorWaveformIndex = osc3Waveform;
	engineParams.voiceParameters->osc3Parameters->oscillatorBankIndex = osc3BankIndex;
	engineParams.voiceParameters->osc4Parameters->oscillatorWaveformIndex = osc4Waveform;
	engineParams.voiceParameters->osc4Parameters->oscillatorBankIndex = osc4BankIndex;

	engineParams.voiceParameters->osc1Parameters->detuneCents = osc1Detune_cents;

	// EG1 Parameters
	engineParams.voiceParameters->ampEGParameters->delayTime_mSec = eg1DelayTime_mSec;
	engineParams.voiceParameters->ampEGParameters->attackTime_mSec = eg1AttackTime_mSec;
	engineParams.voiceParameters->ampEGParameters->holdTime_mSec = eg1HoldTime_mSec;
	engineParams.voiceParameters->ampEGParameters->decayTime_mSec = eg1DecayTime_mSec;
	engineParams.voiceParameters->ampEGParameters->sustainLevel = eg1SustainLevel;
	engineParams.voiceParameters->ampEGParameters->releaseTime_mSec = eg1ReleaseTime_mSec;
	engineParams.voiceParameters->ampEGParameters->egContourType = convertIntToEnum(eg1Mode, egType);
	engineParams.voiceParameters->ampEGParameters->autoRetrigger = eg1AutoRetrigger;
	engineParams.voiceParameters->ampEGParameters->manualRetrigger = eg1ManualTrigger;

	engineParams.voiceParameters->moogFilterParameters->fc = fc1_hertz;
	engineParams.voiceParameters->moogFilterParameters->Q = q1Control;
	engineParams.voiceParameters->moogFilterParameters->enableKeyTrack = (enableKeyTrack == 1);
	engineParams.voiceParameters->moogFilterParameters->keyTrackRatio = keyTrackRatio;

	engineParams.setMM_SourceMasterIntensity(kLFO1_Normal, lfo1Amplitude);
	engineParams.setMM_SourceMasterIntensity(kEG1_Normal, eg1ModOut);

	engineParams.setMM_ChannelEnable(kLFO1_Normal, kOsc1_fo, lfo1_to_osc1Fo);
	engineParams.setMM_ChannelEnable(kLFO1_Normal, kOsc2_fo, lfo1_to_osc2Fo);
	engineParams.setMM_ChannelEnable(kEG1_Normal, kOsc1_fo, eg1_to_osc1Fo);

	engineParams.setMM_DestMasterIntensity(kOsc1_fo, osc1FoModIn);
	engineParams.setMM_DestMasterIntensity(kOsc2_fo, osc2FoModIn);

	

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
	setPresetParameter(preset->presetParameters, controlID::scaleMode, -0.000000);
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
	setPresetParameter(preset->presetParameters, controlID::joystickX_rate, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickX_amp, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickY_rate, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::joystickY_amp, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1AttackTime_mSec, 5.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1DecayTime_mSec, 100.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1SustainLevel, 0.707000);
	setPresetParameter(preset->presetParameters, controlID::eg1ReleaseTime_mSec, 1000.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1HoldTime_mSec, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1DelayTime_mSec, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1Mode, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1AutoRetrigger, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1ManualTrigger, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::fc1_hertz, 880.000000);
	setPresetParameter(preset->presetParameters, controlID::q1Control, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1Amplitude, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1ModOut, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::osc1FoModIn, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::osc2FoModIn, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::enableKeyTrack, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::keyTrackRatio, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1_to_osc1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::lfo1_to_osc2Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1_to_osc1Fo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::eg1Offset, 0.000000);
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
