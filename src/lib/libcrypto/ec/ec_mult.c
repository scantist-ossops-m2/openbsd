/* $OpenBSD: ec_mult.c,v 1.51 2024/11/23 12:56:31 tb Exp $ */
/*
 * Originally written by Bodo Moeller and Nils Larsch for the OpenSSL project.
 */
/* ====================================================================
 * Copyright (c) 1998-2007 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.openssl.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    openssl-core@openssl.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.openssl.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */
/* ====================================================================
 * Copyright 2002 Sun Microsystems, Inc. ALL RIGHTS RESERVED.
 * Portions of this software developed by SUN MICROSYSTEMS, INC.,
 * and contributed to the OpenSSL project.
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/err.h>

#include "ec_local.h"

static int
ec_window_bits(const BIGNUM *bn)
{
	int bits = BN_num_bits(bn);

	if (bits >= 2000)
		return 6;
	if (bits >= 800)
		return 5;
	if (bits >= 300)
		return 4;
	if (bits >= 70)
		return 3;
	if (bits >= 20)
		return 2;

	return 1;
}

/*
 * Width-(w+1) non-adjacent form of bn = \sum_j n_j 2^j, with odd n_j,
 * where at most one of any (w+1) consecutive digits is non-zero.
 */

static int
ec_compute_wNAF(const BIGNUM *bn, signed char **out_wNAF, size_t *out_wNAF_len,
    size_t *out_len)
{
	signed char *wNAF = NULL;
	size_t i, wNAF_len, len;
	int digit, bit, next, sign, wbits, window;
	int ret = 0;

	wNAF_len = BN_num_bits(bn) + 1;
	if ((wNAF = calloc(1, wNAF_len)) == NULL) {
		ECerror(ERR_R_MALLOC_FAILURE);
		goto err;
	}

	wbits = ec_window_bits(bn);
	len = 1 << (wbits - 1);

	sign = BN_is_negative(bn) ? -1 : 1;

	bit = 1 << wbits;
	next = bit << 1;

	/* Extract the wbits + 1 lowest bits from bn into window. */
	window = 0;
	for (i = 0; i < wbits + 1; i++) {
		if (BN_is_bit_set(bn, i))
			window |= (1 << i);
	}

	/* Instead of bn >>= 1 in each iteration, slide window to the left. */
	for (i = 0; i < wNAF_len; i++) {
		digit = 0;

		/*
		 * If window is odd, the i-th wNAF digit is window (mods 2^w),
		 * where mods is the signed modulo in (-2^w-1, 2^w-1]. Subtract
		 * the digit from window, so window is 0 or next, and add the
		 * digit to the wNAF digits.
		 */
		if ((window & 1) != 0) {
			digit = window;
			if ((window & bit) != 0)
				digit = window - next;
			window -= digit;
		}

		wNAF[i] = sign * digit;

		/* Slide the window to the left. */
		window >>= 1;
		window += bit * BN_is_bit_set(bn, i + wbits + 1);
	}

	*out_wNAF = wNAF;
	wNAF = NULL;
	*out_wNAF_len = wNAF_len;
	*out_len = len;

	ret = 1;

 err:
	free(wNAF);

	return ret;
}

static void
free_row(EC_POINT **row, size_t row_len)
{
	size_t i;

	if (row == NULL)
		return;

	for (i = 0; i < row_len; i++)
		EC_POINT_free(row[i]);
	free(row);
}

static int
ec_compute_odd_multiples(const EC_GROUP *group, const EC_POINT *point,
    EC_POINT ***out_row, size_t row_len, BN_CTX *ctx)
{
	EC_POINT **row = NULL;
	EC_POINT *doubled = NULL;
	size_t i;
	int ret = 0;

	if (row_len < 1)
		goto err;

	if ((row = calloc(row_len, sizeof(*row))) == NULL)
		goto err;

	if ((row[0] = EC_POINT_dup(point, group)) == NULL)
		goto err;

	if ((doubled = EC_POINT_new(group)) == NULL)
		goto err;
	if (!EC_POINT_dbl(group, doubled, point, ctx))
		goto err;
	for (i = 1; i < row_len; i++) {
		if ((row[i] = EC_POINT_new(group)) == NULL)
			goto err;
		if (!EC_POINT_add(group, row[i], row[i - 1], doubled, ctx))
			goto err;
	}

	*out_row = row;
	row = NULL;

	ret = 1;

 err:
	EC_POINT_free(doubled);
	free_row(row, row_len);

	return ret;
}

/*
 * Compute the wNAF representation of m and a list of odd multiples of point.
 */

static int
ec_compute_row(const EC_GROUP *group, const BIGNUM *m, const EC_POINT *point,
    signed char **wNAF, size_t *wNAF_len, EC_POINT ***out_row, size_t *out_row_len,
    BN_CTX *ctx)
{
	if (!ec_compute_wNAF(m, wNAF, wNAF_len, out_row_len))
		return 0;
	if (!ec_compute_odd_multiples(group, point, out_row, *out_row_len, ctx))
		return 0;
	return 1;
}

static int
ec_normalize_rows(const EC_GROUP *group, EC_POINT **row0, size_t len0,
    EC_POINT **row1, size_t len1, BN_CTX *ctx)
{
	EC_POINT **val = NULL;
	size_t len = 0;
	int ret = 0;

	if (len1 > SIZE_MAX - len0)
		goto err;
	len = len0 + len1;

	if ((val = calloc(len, sizeof(*val))) == NULL) {
		ECerror(ERR_R_MALLOC_FAILURE);
		goto err;
	}
	memcpy(&val[0], row0, sizeof(*val) * len0);
	memcpy(&val[len0], row1, sizeof(*val) * len1);

	if (!EC_POINTs_make_affine(group, len, val, ctx))
		goto err;

	ret = 1;

 err:
	free(val);

	return ret;
}

/*
 * Compute r = generator * m + point * n in non-constant time.
 */

int
ec_wNAF_mul(const EC_GROUP *group, EC_POINT *r, const BIGNUM *m,
    const EC_POINT *point, const BIGNUM *n, BN_CTX *ctx)
{
	const EC_POINT *generator;
	signed char *wNAF[2] = { 0 };
	size_t wNAF_len[2] = { 0 };
	EC_POINT **row[2] = { 0 };
	size_t row_len[2] = { 0 };
	size_t i;
	int k;
	int r_is_inverted = 0;
	size_t max_len = 0;
	int ret = 0;

	if (m == NULL || n == NULL) {
		ECerror(ERR_R_PASSED_NULL_PARAMETER);
		goto err;
	}
	if (group->meth != r->meth || group->meth != point->meth) {
		ECerror(EC_R_INCOMPATIBLE_OBJECTS);
		goto err;
	}

	if ((generator = EC_GROUP_get0_generator(group)) == NULL) {
		ECerror(EC_R_UNDEFINED_GENERATOR);
		goto err;
	}

	if (!ec_compute_row(group, m, generator, &wNAF[0], &wNAF_len[0],
	    &row[0], &row_len[0], ctx))
		goto err;
	if (!ec_compute_row(group, n, point, &wNAF[1], &wNAF_len[1],
	    &row[1], &row_len[1], ctx))
		goto err;
	if (!ec_normalize_rows(group, row[0], row_len[0], row[1], row_len[1], ctx))
		goto err;

	max_len = wNAF_len[0];
	if (wNAF_len[1] > max_len)
		max_len = wNAF_len[1];

	/*
	 * Set r to the neutral element. Scan through the wNAF representations
	 * of m and n, starting at the most significant digit. Double r and for
	 * each wNAF digit of m add the digit times the generator, and for each
	 * wNAF digit of n add the digit times the point, adjusting the signs
	 * as appropriate.
	 */

	if (!EC_POINT_set_to_infinity(group, r))
		goto err;

	for (k = max_len - 1; k >= 0; k--) {
		if (!EC_POINT_dbl(group, r, r, ctx))
			goto err;

		for (i = 0; i < 2; i++) {
			int digit;
			int is_neg = 0;

			if (k >= wNAF_len[i])
				continue;

			if ((digit = wNAF[i][k]) == 0)
				continue;

			if (digit < 0) {
				is_neg = 1;
				digit = -digit;
			}

			if (is_neg != r_is_inverted) {
				if (!EC_POINT_invert(group, r, ctx))
					goto err;
				r_is_inverted = !r_is_inverted;
			}

			if (!EC_POINT_add(group, r, r, row[i][digit >> 1], ctx))
				goto err;
		}
	}

	if (r_is_inverted) {
		if (!EC_POINT_invert(group, r, ctx))
			goto err;
	}

	ret = 1;

 err:
	free(wNAF[0]);
	free(wNAF[1]);
	free_row(row[0], row_len[0]);
	free_row(row[1], row_len[1]);

	return ret;
}
