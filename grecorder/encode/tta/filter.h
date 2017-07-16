/*
 * filter.h
 *
 * Description: TTA hybrid filter functions
 * Copyright (c) 1999-2011 Aleksander Djuric. All rights reserved.
 * SSE4 optimization copyright (c) 2008 Kazuki Oikawa
 * Distributed under the GNU Lesser General Public License (LGPL).
 * The complete text of the license can be found in the COPYING
 * file included in the distribution.
 *
 */

#ifndef _FILTER_H
#define _FILTER_H

#if defined(CPU_ARM) && defined(ENABLE_ASM) // implements in filter_arm.S
	extern int hybrid_filter_dec(TTA_fltst *fs, int *in);
	extern int hybrid_filter_enc(TTA_fltst *fs, int *in);
#elif defined(ENABLE_SSE2) || defined(ENABLE_SSE4)

#if defined(ENABLE_SSE4)
#include <tmmintrin.h>
#define mullo_epi32(a, b) _mm_mullo_epi32(a, b)
#else // ENABLE_SSE2
#include <emmintrin.h>
#define mullo_epi32(a, b) \
	_mm_unpacklo_epi32(_mm_shuffle_epi32(_mm_mul_epu32(a, b), 0xd8), \
	_mm_shuffle_epi32(_mm_mul_epu32(_mm_shuffle_epi32(a, 0xb1), \
	_mm_shuffle_epi32(b, 0xb1)), 0xd8))
#endif // ENABLE_SSE4

////////////////////////// hybrid_filter_sse4_dec ///////////////////////////
/////////////////////////////////////////////////////////////////////////////

__inline void hybrid_filter_dec(TTA_fltst *fs, TTAint32 *in) {
	register TTAint32 *pA = fs->dl;
	register TTAint32 *pB = fs->qm;
	register TTAint32 *pM = fs->dx;
	register TTAint32 sum = fs->round;
	register __m128i xmA1, xmA2, xmB1, xmB2, xmM1, xmM2, xmDP;

	xmA1 = _mm_load_si128((__m128i*)pA);
	xmA2 = _mm_load_si128((__m128i*)(pA + 4));
	xmB1 = _mm_load_si128((__m128i*)pB);
	xmB2 = _mm_load_si128((__m128i*)(pB + 4));
	xmM1 = _mm_load_si128((__m128i*)pM);
	xmM2 = _mm_load_si128((__m128i*)(pM + 4));

	if (fs->error < 0) {
		xmB1 = _mm_sub_epi32(xmB1, xmM1);
		xmB2 = _mm_sub_epi32(xmB2, xmM2);
		_mm_store_si128((__m128i*)pB, xmB1);
		_mm_store_si128((__m128i*)(pB + 4), xmB2);
	} else if (fs->error > 0) {
		xmB1 = _mm_add_epi32(xmB1, xmM1);
		xmB2 = _mm_add_epi32(xmB2, xmM2);
		_mm_store_si128((__m128i*)pB, xmB1);
		_mm_store_si128((__m128i*)(pB + 4), xmB2);
	}

	xmDP = _mm_add_epi32(mullo_epi32(xmA1, xmB1), mullo_epi32(xmA2, xmB2));
	xmDP = _mm_add_epi32(xmDP, _mm_unpackhi_epi64(xmDP, xmDP));
	sum += _mm_cvtsi128_si32(xmDP) + _mm_cvtsi128_si32(_mm_shuffle_epi32(xmDP, 1));

	xmM1 = _mm_or_si128(_mm_srli_si128(xmM1, 4), _mm_slli_si128(xmM2, 12));
	xmA1 = _mm_or_si128(_mm_srli_si128(xmA1, 4), _mm_slli_si128(xmA2, 12));
	xmM2 = _mm_andnot_si128(_mm_setr_epi32(0, 1, 1, 3),
		_mm_or_si128(_mm_srai_epi32(xmA2, 30), _mm_setr_epi32(1, 2, 2, 4)));

	_mm_store_si128((__m128i*)pA, xmA1);
	_mm_store_si128((__m128i*)pM, xmM1);
	_mm_store_si128((__m128i*)(pM + 4), xmM2);

	fs->error = *in;
	*in += (sum >> fs->shift);

	pA[4] = -pA[5]; pA[5] = -pA[6];
	pA[6] = *in - pA[7]; pA[7] = *in;
	pA[5] += pA[6]; pA[4] += pA[5];
} // hybrid_filter_dec

////////////////////////// hybrid_filter_sse4_enc ///////////////////////////
/////////////////////////////////////////////////////////////////////////////

__inline void hybrid_filter_enc(TTA_fltst *fs, TTAint32 *in) {
	register TTAint32 *pA = fs->dl;
	register TTAint32 *pB = fs->qm;
	register TTAint32 *pM = fs->dx;
	register TTAint32 sum = fs->round;
	register __m128i xmA1, xmA2, xmB1, xmB2, xmM1, xmM2, xmDP;

	xmA1 = _mm_load_si128((__m128i*)pA);
	xmA2 = _mm_load_si128((__m128i*)(pA + 4));
	xmB1 = _mm_load_si128((__m128i*)pB);
	xmB2 = _mm_load_si128((__m128i*)(pB + 4));
	xmM1 = _mm_load_si128((__m128i*)pM);
	xmM2 = _mm_load_si128((__m128i*)(pM + 4));

	if (fs->error < 0) {
		xmB1 = _mm_sub_epi32(xmB1, xmM1);
		xmB2 = _mm_sub_epi32(xmB2, xmM2);
		_mm_store_si128((__m128i*)pB, xmB1);
		_mm_store_si128((__m128i*)(pB + 4), xmB2);
	} else if (fs->error > 0) {
		xmB1 = _mm_add_epi32(xmB1, xmM1);
		xmB2 = _mm_add_epi32(xmB2, xmM2);
		_mm_store_si128((__m128i*)pB, xmB1);
		_mm_store_si128((__m128i*)(pB + 4), xmB2);
	}

	xmDP = _mm_add_epi32(mullo_epi32(xmA1, xmB1), mullo_epi32(xmA2, xmB2));
	xmDP = _mm_add_epi32(xmDP, _mm_unpackhi_epi64(xmDP, xmDP));
	sum += _mm_cvtsi128_si32(xmDP) + _mm_cvtsi128_si32(_mm_shuffle_epi32(xmDP, 1));

	xmM1 = _mm_or_si128(_mm_srli_si128(xmM1, 4), _mm_slli_si128(xmM2, 12));
	xmA1 = _mm_or_si128(_mm_srli_si128(xmA1, 4), _mm_slli_si128(xmA2, 12));
	xmM2 = _mm_andnot_si128(_mm_setr_epi32(0, 1, 1, 3),
		_mm_or_si128(_mm_srai_epi32(xmA2, 30), _mm_setr_epi32(1, 2, 2, 4)));

	_mm_store_si128((__m128i*)pA, xmA1);
	_mm_store_si128((__m128i*)pM, xmM1);
	_mm_store_si128((__m128i*)(pM + 4), xmM2);

	pA[4] = -pA[5]; pA[5] = -pA[6];
	pA[6] = *in - pA[7]; pA[7] = *in;
	pA[5] += pA[6]; pA[4] += pA[5];

	*in -= (sum >> fs->shift);
	fs->error = *in;
} // hybrid_filter_enc

#else // PORTABLE

///////////////////////// hybrid_filter_compat_dec //////////////////////////
/////////////////////////////////////////////////////////////////////////////

__inline void hybrid_filter_dec(TTA_fltst *fs, TTAint32 *in) {
	register TTAint32 *pA = fs->dl;
	register TTAint32 *pB = fs->qm;
	register TTAint32 *pM = fs->dx;
	register TTAint32 sum = fs->round;

	if (fs->error < 0) {
		pB[0] -= pM[0]; pB[1] -= pM[1]; pB[2] -= pM[2]; pB[3] -= pM[3];
		pB[4] -= pM[4]; pB[5] -= pM[5]; pB[6] -= pM[6]; pB[7] -= pM[7];
	} else if (fs->error > 0) {
		pB[0] += pM[0]; pB[1] += pM[1]; pB[2] += pM[2]; pB[3] += pM[3];
		pB[4] += pM[4]; pB[5] += pM[5]; pB[6] += pM[6]; pB[7] += pM[7];
	}

	sum +=	pA[0] * pB[0] + pA[1] * pB[1] + pA[2] * pB[2] + pA[3] * pB[3] +
		pA[4] * pB[4] + pA[5] * pB[5] + pA[6] * pB[6] + pA[7] * pB[7];

	pM[0] = pM[1]; pM[1] = pM[2]; pM[2] = pM[3]; pM[3] = pM[4];
	pA[0] = pA[1]; pA[1] = pA[2]; pA[2] = pA[3]; pA[3] = pA[4];

	pM[4] = ((pA[4] >> 30) | 1);
	pM[5] = ((pA[5] >> 30) | 2) & ~1;
	pM[6] = ((pA[6] >> 30) | 2) & ~1;
	pM[7] = ((pA[7] >> 30) | 4) & ~3;

	fs->error = *in;
	*in += (sum >> fs->shift);

	pA[4] = -pA[5]; pA[5] = -pA[6];
	pA[6] = *in - pA[7]; pA[7] = *in;
	pA[5] += pA[6]; pA[4] += pA[5];
} // hybrid_filter_dec

///////////////////////// hybrid_filter_compat_enc //////////////////////////
/////////////////////////////////////////////////////////////////////////////

__inline void hybrid_filter_enc(TTA_fltst *fs, TTAint32 *in) {
	register TTAint32 *pA = fs->dl;
	register TTAint32 *pB = fs->qm;
	register TTAint32 *pM = fs->dx;
	register TTAint32 sum = fs->round;

	if (fs->error < 0) {
		pB[0] -= pM[0]; pB[1] -= pM[1]; pB[2] -= pM[2]; pB[3] -= pM[3];
		pB[4] -= pM[4]; pB[5] -= pM[5]; pB[6] -= pM[6]; pB[7] -= pM[7];
	} else if (fs->error > 0) {
		pB[0] += pM[0]; pB[1] += pM[1]; pB[2] += pM[2]; pB[3] += pM[3];
		pB[4] += pM[4]; pB[5] += pM[5]; pB[6] += pM[6]; pB[7] += pM[7];
	}

	sum +=	pA[0] * pB[0] + pA[1] * pB[1] + pA[2] * pB[2] + pA[3] * pB[3] +
		pA[4] * pB[4] + pA[5] * pB[5] + pA[6] * pB[6] + pA[7] * pB[7];

	pM[0] = pM[1]; pM[1] = pM[2]; pM[2] = pM[3]; pM[3] = pM[4];
	pA[0] = pA[1]; pA[1] = pA[2]; pA[2] = pA[3]; pA[3] = pA[4];

	pM[4] = ((pA[4] >> 30) | 1);
	pM[5] = ((pA[5] >> 30) | 2) & ~1;
	pM[6] = ((pA[6] >> 30) | 2) & ~1;
	pM[7] = ((pA[7] >> 30) | 4) & ~3;

	pA[4] = -pA[5]; pA[5] = -pA[6];
	pA[6] = *in - pA[7]; pA[7] = *in;
	pA[5] += pA[6]; pA[4] += pA[5];

	*in -= (sum >> fs->shift);
	fs->error = *in;
} // hybrid_filter_enc

#endif // PORTABLE

#endif // _FILTER_H

