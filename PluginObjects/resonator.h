#ifndef __rezonator_h__
#define __rezonator_h__

// --- includes
#include "synthdefs.h"

class IntegerSampleDelay 
{
public:
	IntegerSampleDelay(void) {}	/* C-TOR */
	~IntegerSampleDelay(void) {}	/* D-TOR */

public:
	void clear()
	{
		delayBuffer.flushBuffer();
	}

	void reset(double _sampleRate)
	{
		// --- 2 seconds max
		delayBuffer.createCircularBuffer(_sampleRate*20.0);
	}

	void setDelayTime(unsigned int _delaySamples)
	{
		delaySamples = _delaySamples;
	}

	void writeDelay(double xn)
	{
		// --- write to delay buffer
		delayBuffer.writeBuffer(xn);
	}

	double readDelay()
	{
		double yn = delayBuffer.readBuffer((int)delaySamples);
		return yn;
	}

private:
	// --- our only variable
	unsigned int delaySamples = 0;

	// --- delay buffer of doubles
	CircularBuffer<double> delayBuffer;
};


class TinyAPF
{
public:
	TinyAPF(void) {}	/* C-TOR */
	~TinyAPF(void) {}	/* D-TOR */

public:
	void reset()
	{
		state[0] = 0.0;
		state[1] = 0.0;
	}

	void setAlpha(double _alpha)
	{
		alpha = _alpha;
	}

	// --- one sample
	double processAudioSample(double xn)
	{
		double yn = xn*alpha + state[0] - alpha*state[1];
		state[0] = xn;
		state[1] = yn;
		return yn;
	}

private:
	// --- our only coefficient
	double alpha = 0.0;
	double state[2] = { 0.0 };
};


class TinyLPF
{
public:
	TinyLPF(void) {}	/* C-TOR */
	~TinyLPF(void) {}	/* D-TOR */

public:
	void reset()
	{
		state[0] = 0.0;
		state[1] = 0.0;
	}

	// --- one sample
	double processAudioSample(double xn)
	{
		double yn = 0.5*xn + 0.5*state[0];
		state[0] = xn;
		return yn;
	}

private:
	double state[2] = { 0.0 };
};


class Resonator : public IAudioSignalProcessor
{
public:
	Resonator(void) {}	/* C-TOR */
	~Resonator(void) {}	/* D-TOR */

public:
	// --- IAudioSignalProcessor
	virtual bool reset(double _sampleRate)
	{
		sampleRate = _sampleRate;
		integerDelay.reset(_sampleRate);
		return true;
	}

	// --- process MONO audio delay
	virtual double processAudioSample(double xn)
	{
		double delayOut = integerDelay.readDelay();
		double filterOut = loopFilter.processAudioSample(xn + delayOut);
		double apfOut = fracDelayAPF.processAudioSample(filterOut);
		integerDelay.writeDelay(apfOut*decay);

		// --- done
		return apfOut;
	}

	// --- just mono
	virtual bool canProcessAudioFrame() { return false; }
	
	// --- note event handlers
	bool doNoteOn(double midiPitch, uint32_t midiNoteNumber, uint32_t midiNoteVelocity)
	{
		// --- tune the tube
		setResonatorTuning_Hz(midiPitch);

		// --- flush the tube
		clear();

		return true;
	}

	bool doNoteOff(double midiPitch, uint32_t midiNoteNumber, uint32_t midiNoteVelocity)
	{
		return true;
	}

	// --- clear delay line for a new run
	void clear()
	{
		integerDelay.clear();
	}

	void setDecay(double _decay) { decay = _decay; }
	void setResonatorTuning_Hz(double newResonatorTuning_Hz) { updateResonatorTuning(newResonatorTuning_Hz); }

private:

	void updateResonatorTuning(double _resonatorTuning_Hz)
	{
		// --- calculate delay time for this pitch
		double delayTime = sampleRate / _resonatorTuning_Hz;

		// --- the tube + LPF = L + 1/2 samples, so back calculate to get the delay length
		double delayLength = delayTime - 0.5;

		// --- now take integer portion
		unsigned int intDelayLen = (int)delayLength;

		// --- this guarantees that apfDelta will be betwen [0.0, 1.0] or fractional delay
		double apfDelta = delayTime - ((double)intDelayLen + 0.5);

		// --- calculate normalized frequency in Hz
		double omega_0 = kTwoPi*_resonatorTuning_Hz / sampleRate;
		double omega_0_half = omega_0 / 2.0;

		// --- calcuate APF coefficients using desired fractional delay, apfDelta
		double alpha = sin((1.0 - apfDelta)*omega_0_half) / sin((1.0 + apfDelta)*omega_0_half);

		// --- delay is -1 because of the way the CircularBuffer works, expecting read/write
		integerDelay.setDelayTime(intDelayLen - 1);

		// --- set APF for fractional delay
		fracDelayAPF.setAlpha(alpha);

	}

	IntegerSampleDelay integerDelay;
	TinyAPF fracDelayAPF;
	TinyLPF loopFilter;
	double sampleRate = 0.0;
	double decay = 1.0;
	double baseResonatorTuning_Hz = 0.0;
	double currentresonatorTuning_Hz = 0.0;
};

#endif /* defined(__rezonator_h__) */
