//$ nobt
//$ nocpp

/**
 * @file r8butil.h
 *
 * @brief The inclusion file with several utility functions.
 *
 * This file includes several utility functions used by various utility
 * programs like "calcErrorTable.cpp".
 *
 * r8brain-free-src Copyright (c) 2013-2014 Aleksey Vaneev
 * See the "License.txt" file for license.
 */

#ifndef R8BUTIL_INCLUDED
#define R8BUTIL_INCLUDED

#include "r8bbase.h"

namespace r8b {

/**
 * @param re Real part of the frequency response.
 * @param im Imaginary part of the frequency response.
 * @return A magnitude response value converted from the linear scale to the
 * logarithmic scale.
 */

inline R8B_FLOAT convertResponseToLog( const R8B_FLOAT re, const R8B_FLOAT im )
{
	return( 4.34294481903251828 * log( re * re + im * im + 1e-100 ));
}

/**
 * An utility function that performs frequency response scanning step update
 * based on the current magnitude response's slope.
 *
 * @param[in,out] step The current scanning step. Will be updated on
 * function's return. Must be a positive value.
 * @param curg Squared magnitude response at the current frequency point.
 * @param[in,out] prevg_log Previous magnitude response, log scale. Will be
 * updated on function's return.
 * @param prec Precision multiplier, affects the size of the step.
 * @param maxstep The maximal allowed step.
 * @param minstep The minimal allowed step.
 */

inline void updateScanStep( R8B_FLOAT& step, const R8B_FLOAT curg,
	R8B_FLOAT& prevg_log, const R8B_FLOAT prec, const R8B_FLOAT maxstep,
	const R8B_FLOAT minstep = 1e-11 )
{
	R8B_FLOAT curg_log = 4.34294481903251828 * log( curg + 1e-100 );
	curg_log += ( prevg_log - curg_log ) * 0.7;

	const R8B_FLOAT slope = fabs( curg_log - prevg_log );
	prevg_log = curg_log;

	if( slope > 0.0 )
	{
		step /= prec * slope;
		step = max( min( step, maxstep ), minstep );
	}
}

/**
 * Function locates normalized frequency at which the minimum filter gain
 * is reached. The scanning is performed from lower (left) to higher
 * (right) frequencies, the whole range is scanned.
 *
 * Function expects that the magnitude response is always reducing from lower
 * to high frequencies, starting at "minth".
 *
 * @param flt Filter response.
 * @param fltlen Filter response's length in samples (taps).
 * @param[out] ming The current minimal gain (squared). On function's return
 * will contain the minimal gain value found (squared).
 * @param[out] minth The normalized frequency where the minimal gain is
 * currently at. On function's return will point to the normalized frequency
 * where the new minimum was found.
 * @param thend The ending frequency, inclusive.
 */

inline void findFIRFilterResponseMinLtoR( const R8B_FLOAT* const flt,
	const int fltlen, R8B_FLOAT& ming, R8B_FLOAT& minth, const R8B_FLOAT thend )
{
	const R8B_FLOAT maxstep = minth * 2e-3;
	R8B_FLOAT curth = minth;
	R8B_FLOAT re;
	R8B_FLOAT im;
	calcFIRFilterResponse( flt, fltlen, M_PI * curth, re, im );
	R8B_FLOAT prevg_log = convertResponseToLog( re, im );
	R8B_FLOAT step = 1e-11;

	while( true )
	{
		curth += step;

		if( curth > thend )
		{
			break;
		}

		calcFIRFilterResponse( flt, fltlen, M_PI * curth, re, im );
		const R8B_FLOAT curg = re * re + im * im;

		if( curg > ming )
		{
			ming = curg;
			minth = curth;
			break;
		}

		ming = curg;
		minth = curth;

		updateScanStep( step, curg, prevg_log, 0.31, maxstep );
	}
}

/**
 * Function locates normalized frequency at which the maximal filter gain
 * is reached. The scanning is performed from lower (left) to higher
 * (right) frequencies, the whole range is scanned.
 *
 * Note: this function may "stall" in very rare cases if the magnitude
 * response happens to be "saw-tooth" like, requiring a very small stepping to
 * be used. If this happens, it may take dozens of seconds to complete.
 *
 * @param flt Filter response.
 * @param fltlen Filter response's length in samples (taps).
 * @param[out] maxg The current maximal gain (squared). On function's return
 * will contain the maximal gain value (squared).
 * @param[out] maxth The normalized frequency where the maximal gain is
 * currently at. On function's return will point to the normalized frequency
 * where the maximum was reached.
 * @param thend The ending frequency, inclusive.
 */

inline void findFIRFilterResponseMaxLtoR( const R8B_FLOAT* const flt,
	const int fltlen, R8B_FLOAT& maxg, R8B_FLOAT& maxth, const R8B_FLOAT thend )
{
	const R8B_FLOAT maxstep = maxth * 1e-4;
	R8B_FLOAT premaxth = maxth;
	R8B_FLOAT premaxg = maxg;
	R8B_FLOAT postmaxth = maxth;
	R8B_FLOAT postmaxg = maxg;

	R8B_FLOAT prevth = maxth;
	R8B_FLOAT prevg = maxg;
	R8B_FLOAT curth = maxth;
	R8B_FLOAT re;
	R8B_FLOAT im;
	calcFIRFilterResponse( flt, fltlen, M_PI * curth, re, im );
	R8B_FLOAT prevg_log = convertResponseToLog( re, im );
	R8B_FLOAT step = 1e-11;

	bool WasPeak = false;
	int AfterPeakCount = 0;

	while( true )
	{
		curth += step;

		if( curth > thend )
		{
			break;
		}

		calcFIRFilterResponse( flt, fltlen, M_PI * curth, re, im );
		const R8B_FLOAT curg = re * re + im * im;

		if( curg > maxg )
		{
			premaxth = prevth;
			premaxg = prevg;
			maxg = curg;
			maxth = curth;
			WasPeak = true;
			AfterPeakCount = 0;
		}
		else
		if( WasPeak )
		{
			if( AfterPeakCount == 0 )
			{
				postmaxth = curth;
				postmaxg = curg;
			}

			if( AfterPeakCount == 5 )
			{
				// Perform 2 approximate binary searches.

				int k;

				for( k = 0; k < 2; k++ )
				{
					R8B_FLOAT l = ( k == 0 ? premaxth : maxth );
					R8B_FLOAT curgl = ( k == 0 ? premaxg : maxg );
					R8B_FLOAT r = ( k == 0 ? maxth : postmaxth );
					R8B_FLOAT curgr = ( k == 0 ? maxg : postmaxg );

					while( true )
					{
						const R8B_FLOAT c = ( l + r ) * 0.5;
						calcFIRFilterResponse( flt, fltlen, M_PI * c,
							re, im );

						const R8B_FLOAT curg = re * re + im * im;

						if( curgl > curgr )
						{
							r = c;
							curgr = curg;
						}
						else
						{
							l = c;
							curgl = curg;
						}

						if( r - l < 1e-11 )
						{
							if( curgl > curgr )
							{
								maxth = l;
								maxg = curgl;
							}
							else
							{
								maxth = r;
								maxg = curgr;
							}

							break;
						}
					}
				}

				break;
			}

			AfterPeakCount++;
		}

		prevth = curth;
		prevg = curg;

		updateScanStep( step, curg, prevg_log, 1.0, maxstep );
	}
}

/**
 * Function locates normalized frequency at which the specified maximum
 * filter gain is reached. The scanning is performed from higher (right)
 * to lower (left) frequencies, scanning stops when the required gain
 * value was crossed. Function uses an extremely efficient binary search and
 * thus expects that the magnitude response has the "main lobe" form produced
 * by windowing, with a minimal pass-band ripple.
 *
 * @param flt Filter response.
 * @param fltlen Filter response's length in samples (taps).
 * @param maxg Maximal gain (squared).
 * @param[out] th The current normalized frequency. On function's return will
 * point to the normalized frequency where "maxg" is reached.
 * @param thend The leftmost frequency to scan, inclusive.
 */

inline void findFIRFilterResponseLevelRtoL( const R8B_FLOAT* const flt,
	const int fltlen, const R8B_FLOAT maxg, R8B_FLOAT& th, const R8B_FLOAT thend )
{
	// Perform exact binary search.

	R8B_FLOAT l = thend;
	R8B_FLOAT r = th;

	while( true )
	{
		const R8B_FLOAT c = ( l + r ) * 0.5;

		if( r - l < 1e-14 )
		{
			th = c;
			break;
		}

		R8B_FLOAT re;
		R8B_FLOAT im;
		calcFIRFilterResponse( flt, fltlen, M_PI * c, re, im );
		const R8B_FLOAT curg = re * re + im * im;

		if( curg > maxg )
		{
			l = c;
		}
		else
		{
			r = c;
		}
	}
}

} // namespace r8b

#endif // R8BUTIL_INCLUDED
