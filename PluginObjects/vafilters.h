#ifndef __vaFilters_h__
#define __vaFilters_h__

// --- includes
#include "synthdefs.h"

const int NUM_SUBFILTERS = 4;
const double freqModLow = 20.0;
const double freqModHigh = 18000.0;

/**
\class ZVAFilterEx
\ingroup SynthClasses
\brief Extends ZVAFilter for use in MOOG filter

\author Will Pirkle
\version Revision : 1.0
\date Date : 2019 / 11 / 04
*/
class ZVAFilterEx : public ZVAFilter
{
public:
	ZVAFilterEx() {}
	~ZVAFilterEx() {}

	// --- added for MOOG - you need access to the s-ports for the first order filter
	double getS0Port() { return integrator_z[0]; }
	double getS1Port() { return integrator_z[1]; } // <--- not used in MOOG 

	// --- need alpha
	double getAlpha() { return alpha; }
	double getLittle_g() 
	{
		double fc = zvaFilterParameters.fc;
		double Q = zvaFilterParameters.Q;
		vaFilterAlgorithm filterAlgorithm = zvaFilterParameters.filterAlgorithm;

		// --- normal Zavalishin SVF calculations here
		//     prewarp the cutoff- these are bilinear-transform filters
		double wd = kTwoPi*fc;
		double T = 1.0 / sampleRate;
		double wa = (2.0 / T)*tan(wd*T / 2.0);
		double g = wa*T / 2.0;

		return g; 
	}

protected:
};

// --- audio filters, 1st and 2nd order currently supported
enum class moogFilterAlgorithm {
	kLPF2, kHPF2, kLPF4, kHPF4

}; 

/**
	\struct MoogFilterParameters
	\ingroup SynthClasses
	\brief custom parameter structure for a Moog ladder filter
	\author Will Pirkle
	\version Revision : 1.0
	\date Date : 2017 / 09 / 24
*/
struct MoogFilterParameters
{
	MoogFilterParameters() {}
	MoogFilterParameters& operator=(const MoogFilterParameters& params)	// need this override for collections to work
	{
		if (this == &params)
			return *this;

		filterAlgorithm = params.filterAlgorithm;
		fc = params.fc;
		Q = params.Q;
		filterOutputGain_dB = params.filterOutputGain_dB;
		enableGainComp = params.enableGainComp;
		enableNLP = params.enableNLP;
		return *this;
	}

	// --- individual parameters
	moogFilterAlgorithm filterAlgorithm = moogFilterAlgorithm::kLPF4;
	double fc = 1000.0;
	double Q = 0.707;
	double filterOutputGain_dB = 0.0; // usually not used
	bool enableGainComp = false;
	bool enableNLP = false;
};


/**
\class MoogFilter
\ingroup SynthClasses
\brief Encapsulates a Moog Ladder Filter.

\author Will Pirkle
\version Revision : 1.0
\date Date : 2019 / 11 / 04
*/
class MoogFilter : public ISynthProcessor
{
public:
	MoogFilter(const std::shared_ptr<MidiInputData> _midiInputData, std::shared_ptr<MoogFilterParameters> _parameters) :
		midiInputData(_midiInputData)
		, parameters(_parameters)
	{
		// --- setup four identical ZVAFilters as LPF1 types
		ZVAFilterParameters params = subFilter[0].getParameters();
		params.filterAlgorithm = vaFilterAlgorithm::kLPF1;

		// --- this does not work because it screws up the phase response at Nyquist
		// params.matchAnalogNyquistLPF = true;

		for (int i = 0; i < NUM_SUBFILTERS; i++)
		{
			subFilter[i].setParameters(params);
		}
	}		/* C-TOR */
	~MoogFilter() {}		/* D-TOR */


	// --- ISynthProcessor
	virtual bool update(bool updateAllModRoutings = true);
	virtual bool doNoteOn(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity);
	virtual bool doNoteOff(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity);

	// --- set sample rate, then update coeffs
	virtual bool reset(double _sampleRate)
	{
		// --- initialize four identical ZVAFilters as LPF1 types
		for (int i = 0; i < NUM_SUBFILTERS; i++)
		{
			subFilter[i].reset(_sampleRate);
		}

		// --- setup
		calculateFilterCoeffs();
		return true;
	}

	// --- the processor function
	virtual bool processSynthAudio(SynthProcessorData* audioData);

	// --- calculate MOOG coefficients
	//     
	void calculateFilterCoeffs()
	{
		if (parameters->filterAlgorithm == moogFilterAlgorithm::kLPF4 ||
			parameters->filterAlgorithm == moogFilterAlgorithm::kHPF4 ||
			parameters->filterAlgorithm == moogFilterAlgorithm::kHPF2)
		{
			// --- Q is 1 -> 10 for my plugins; just map it to 0 -> 4
			K = (4.0)*(parameters->Q - 1.0) / (10.0 - 1.0);

			double G = subFilter[0].getAlpha();
			double g = subFilter[0].getLittle_g();
			double onePlus_g = 1.0 + g;

			// --- follow cookbook instructions
			beta[0] = (G*G*G / onePlus_g);
			beta[1] = (G*G / onePlus_g);
			beta[2] = (G / onePlus_g);
			beta[3] = (1.0 / onePlus_g);

			// --- alpha0 = 1/(1 + K*G*G*G*G)
			alpha0 = 1.0 / (1.0 + K*G*G*G*G);
		}
		else if (parameters->filterAlgorithm == moogFilterAlgorithm::kLPF2)
		{
			// --- Q controls always 1 -> 10  kFilterGUI_Q_Range = (10.0 - 1.0)
			//	   this maps qControl = 1 -> 10   to   K = 0 -> 2
			K = (2.0)*(parameters->Q - 1.0) / (10.0 - 1.0);

			// the allpass G value
			double G = subFilter[0].getAlpha();
			double g = subFilter[0].getLittle_g();
			double onePlus_g = 1.0 + g;
			double GA = 2.0*G - 1;

			// --- half ladder calcs
			// --- follow cookbook instructions
			beta[0] = (GA*G / onePlus_g);
			beta[1] = (GA / onePlus_g);
			beta[2] = (2.0 / onePlus_g);
			beta[3] = 0.0;

			// calculate alpha0
			alpha0 = 1.0 / (1.0 + K*GA*G*G);
		}
	}

	// --- process audio: run the filter on one sample
	//     NOTE: this is not a virtual or required function, but may be used for aggregated filters
	double processAudioSample(double xn)
	{
		// --- 4th order MOOG:
		double sigma = 0.0;
		if (parameters->filterAlgorithm == moogFilterAlgorithm::kLPF2)
		{
			sigma = beta[0] * subFilter[0].getS0Port() +
					beta[1] * subFilter[1].getS0Port() +
					beta[2] * subFilter[2].getS0Port();
		}
		else // all others
		{
			sigma = beta[0] * subFilter[0].getS0Port() +
					beta[1] * subFilter[1].getS0Port() +
					beta[2] * subFilter[2].getS0Port() +
					beta[3] * subFilter[3].getS0Port();
		}

		// --- gain comp is a simple on/off switch LPF ONLY!!!!
		if(parameters->enableGainComp)
			xn *= 1.0 + 0.5*K; // --- increase 0.5 for MORE bass

		// --- now figure out u(n) = alpha0*[x(n) - K*sigma]
		double u = alpha0*(xn - K*sigma);

		if (parameters->filterAlgorithm == moogFilterAlgorithm::kLPF4)
		{
			// --- send u -> LPF1 and then cascade the outputs to form y(n)
			double y0 = subFilter[0].processAudioSample(u); //< --- NOT x(n), u(n)
			double y1 = subFilter[1].processAudioSample(y0);
			double y2 = subFilter[2].processAudioSample(y1);
			double yn = subFilter[3].processAudioSample(y2);

			// --- done
			return yn;
		}
		else if (parameters->filterAlgorithm == moogFilterAlgorithm::kLPF2)
		{
			// --- send u -> LPF1 and then cascade the outputs to form y(n)
			double y0 = subFilter[0].processAudioSample(u); //< --- NOT x(n), u(n)
			double y1 = subFilter[1].processAudioSample(y0);
			double yn = subFilter[2].processAudioSample(y1);

			// --- done
			return yn;
		}
		else if (parameters->filterAlgorithm == moogFilterAlgorithm::kHPF4 ||
				 parameters->filterAlgorithm == moogFilterAlgorithm::kHPF2)
		{
			double A, B, C, D, E = 0.0;
			if (parameters->filterAlgorithm == moogFilterAlgorithm::kHPF4)
			{
				A = 1.0;
				B = -4.0;
				C = 6.0;
				D = -4.0;
				E = 1.0;
			}
			else
			{
				A = 1.0;
				B = -2.0;
				C = 1.0;
				D = 0.0;
				E = 0.0;
			}
			// --- cascade of 4 filters
			// --- send u -> LPF1 and then cascade the outputs to form y(n)
			double y0 = subFilter[0].processAudioSample(u); //< --- NOT x(n), u(n)
			double y1 = subFilter[1].processAudioSample(y0);
			double y2 = subFilter[2].processAudioSample(y1);
			double yn = subFilter[3].processAudioSample(y2);

			yn = A*u + B*y0 + C*y1 + D*y2+ E*yn;
			return yn;
		}

		return xn; // should never happen
	}

	// --- access to modulators
	// --- get our modulators
	virtual std::shared_ptr<ModInputData> getModulators() {
		return modulators;
	}

	// --- not used here;
	virtual void setModulators(std::shared_ptr<ModInputData> _modulators) {
		modulators = _modulators;
	}

protected:
	// --- MIDI Data Interface
	const const std::shared_ptr<MidiInputData> midiInputData = nullptr;

	// --- we share Parameters with other voice's same-components
	std::shared_ptr<MoogFilterParameters> parameters = nullptr;

	// --- set of double[MAX_MODULATION_CHANNELS]
	std::shared_ptr<ModInputData> modulators = std::make_shared<ModInputData>();

	// --- 4 subfilters
	ZVAFilterEx subFilter[NUM_SUBFILTERS];				// --- NUM_SUBFILTERS filters

	// --- four beta values
	double beta[NUM_SUBFILTERS] = { 0.0 };
	double alpha0 = 1.0;	// --- our delay free loop correction coeffient (different from sub filters which also have their own)
	double K = 0.0;			// --- this is 0.0 --> 4.0 = Q from 1 --> 10
};

#endif /* defined(__vaFilters_h__) */
