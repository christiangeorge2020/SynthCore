#pragma once

#include "synthdefs.h"

// --- this detector can receive signals and transmit detection values that are both > 0dBFS
class TruLogDetector
{
public:
	TruLogDetector(void) {}
	~TruLogDetector(void) {}

public:
	// Call the Init Function to initialize and setup all at once; this can be called as many times
	// as you want
	inline void initialize(double attack_in_ms, double release_in_ms, bool bAnalogTC, unsigned int uDetect)
	{
		envelope = 0.0;
		analogTC = bAnalogTC;
		attackTime_mSec = attack_in_ms;
		releaseTime_mSec = release_in_ms;
		detectMode = uDetect;
		setAttackTime(attack_in_ms);
		setReleaseTime(release_in_ms);
	}

	// these functions allow you to change modes and attack/release one at a time during
	// realtime operation
	inline void setTCModeAnalog(bool _analogTC)
	{
		analogTC = _analogTC;
		setAttackTime(attackTime_mSec);
		setReleaseTime(releaseTime_mSec);
	}

	// THEN do these after init
	inline void setAttackTime(double attack_in_ms)
	{
		attackTime_mSec = attack_in_ms;

		if (analogTC)
			attackTime = exp(ENVELOPE_ANALOG_TC / (attack_in_ms * detectorSampleRate * 0.001f));
		else
			attackTime = exp(ENVELOPE_DIGITAL_TC / (attack_in_ms * detectorSampleRate * 0.001f));
	}

	inline void setReleaseTime(double release_in_ms)
	{
		releaseTime_mSec = release_in_ms;

		if (analogTC)
			releaseTime = exp(ENVELOPE_ANALOG_TC / (release_in_ms * detectorSampleRate * 0.001f));
		else
			releaseTime = exp(ENVELOPE_DIGITAL_TC / (release_in_ms * detectorSampleRate * 0.001f));
	}

	inline void update(double _attackTime_mSec, double _releaseTime_mSec, unsigned int _detectMode)
	{
		if (attackTime_mSec != _attackTime_mSec)
			setAttackTime(_attackTime_mSec);
		if (releaseTime_mSec != _releaseTime_mSec)
			setAttackTime(_releaseTime_mSec);
		detectMode = _detectMode;
	}

	// const unsigned int ENVELOPE_DETECT_MODE_PEAK = 0;
	// const unsigned int ENVELOPE_DETECT_MODE_MS = 1;
	// const unsigned int ENVELOPE_DETECT_MODE_RMS = 2;
	// const unsigned int ENVELOPE_DETECT_MODE_NONE = 3;
	inline void setDetectMode(unsigned int _detectMode) { detectMode = _detectMode; }

	inline void setSampleRate(double d)
	{
		detectorSampleRate = d;
		setAttackTime(attackTime_mSec);
		setReleaseTime(releaseTime_mSec);
	}
	
	// call this from your prepareForPlay() function each time to reset the detector
	inline void reset(double _samplerate)
	{
		envelope = 0.0;
		peakEnvelope = -1.0;
		detectorSampleRate = _samplerate;
	}

	// call this to detect; it returns the peak ms or rms value at that instant
	inline double detect(double input)
	{
		switch (detectMode)
		{
			case ENVELOPE_DETECT_MODE_PEAK:
				input = fabs(input);
				break;
			case ENVELOPE_DETECT_MODE_MS:
			case ENVELOPE_DETECT_MODE_RMS: // --- both MS and RMS require squaring the input
				input = fabs(input) * fabs(input);
				break;
			default:
				input = fabs(input);
				break;
		}

		double currEnvelope = 0.0;
		if (input> envelope)
			currEnvelope = attackTime * (envelope - input) + input;
		else
			currEnvelope = releaseTime * (envelope - input) + input;

		if (currEnvelope > 0.0 && currEnvelope <  1.175494351e-38) currEnvelope = 0;
		if (currEnvelope < 0.0 && currEnvelope > -1.175494351e-38) currEnvelope = 0;

		// --- bound them; can happen when using pre-detector gains of more than 1.0
		if (clampToUnityMax)
			currEnvelope = fmin(currEnvelope, 1.0);

		currEnvelope = fmax(currEnvelope, 0.0);

		// --- store envelope prior to sqrt for RMS version; this also sets peak-hold value
		setEnvelope(currEnvelope);

		// --- if RMS, do the SQRT
		if (detectMode == ENVELOPE_DETECT_MODE_RMS)
			currEnvelope = pow(currEnvelope, 0.5);

		// --- setup for log( )
		if (currEnvelope <= 0)
			return 0;
		
		// --- true log, can go above 0dBFS!
		return 20.0*log10(currEnvelope);
	}

	// --- for peak-hold
	inline void resetPeakHold() { peakEnvelope = -1.0; }
	inline void setPeakHold(bool b) { peakHold = b; }
	inline void setClampToUnityMax(bool b) { clampToUnityMax = b; }

protected:
	double attackTime_mSec = 0.0;
	double releaseTime_mSec = 0.0;
	double attackTime = 0.0;
	double releaseTime = 0.0;
	double detectorSampleRate = 44100.0;
	double envelope = 0.0;
	unsigned int  detectMode = 0;
	double peakEnvelope = -1.0;
	bool peakHold = false;
	bool analogTC = false;
	bool clampToUnityMax = true;

	void setEnvelope(double value)
	{
		if (!peakHold)
		{
			envelope = value;
			return;
		}

		// --- holding peak
		if (value > peakEnvelope)
		{
			peakEnvelope = value;
			envelope = value;
		}
	}
};






// --- custom limiter designed especially for self oscillating filters whose outputs are > 0dBFS
class Limiter
{
public:
	Limiter() {}
	~Limiter() {}

	void reset(double _sampleRate)
	{
		// --- reset detector (always first)
		detector.reset(_sampleRate);

		// --- init; 
		detector.initialize(0.1,	/* attack time mSec*/
							25.0,	/* release time mSec*/
							true,	/* true = analog time-constant */
							ENVELOPE_DETECT_MODE_PEAK); /* peak detector*/

		// --- allow inputs > 0dBFS, which is what oscillating filters output
		detector.setClampToUnityMax(false);
	}

	void setAttackTime(double attack_in_ms) { detector.setAttackTime(attack_in_ms); }
	void setReleaseTime(double release_in_ms) { detector.setReleaseTime(release_in_ms); }
	void setThreshold_dB(double _threshold_dB) { threshold_dB = _threshold_dB; }
	
	float calcLimiterGain(float fDetectorValue, float fThreshold)
	{
		// --- slope variable // limiting is infinite ratio thus CS->1.0
		//                       you can play with this for compression CS < 1.0
		float CS = 1.0;
		
		// --- compute gain; threshold and detection values are in dB	
		float yG = CS*(fThreshold - fDetectorValue);  // [Eq. 13.1]

		// --- clamp; this allows ratios of 1:1 to still operate
		yG = fmin(0, yG);

		// --- convert back to linear
		return pow(10.0, yG / 20.0);
	}

	// --- do the limiter
	double processAudio(double input)
	{
		double detectedValue_dB = detector.detect(input);
		double gain = calcLimiterGain(detectedValue_dB, threshold_dB);
		return input*gain;
	}

protected:
	TruLogDetector detector;
	double threshold_dB = -0.5; // (dB)
};

