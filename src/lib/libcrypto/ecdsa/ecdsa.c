/* $OpenBSD: ecdsa.c,v 1.3 2023/07/05 12:49:42 tb Exp $ */
/* ====================================================================
 * Copyright (c) 2000-2002 The OpenSSL Project.  All rights reserved.
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
 *    for use in the OpenSSL Toolkit. (http://www.OpenSSL.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    licensing@OpenSSL.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.OpenSSL.org/)"
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

#include <string.h>

#include <openssl/opensslconf.h>

#include <openssl/asn1t.h>
#include <openssl/bn.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/objects.h>

#include "bn_local.h"
#include "ec_local.h"
#include "ecdsa_local.h"

static const ASN1_TEMPLATE ECDSA_SIG_seq_tt[] = {
	{
		.flags = 0,
		.tag = 0,
		.offset = offsetof(ECDSA_SIG, r),
		.field_name = "r",
		.item = &BIGNUM_it,
	},
	{
		.flags = 0,
		.tag = 0,
		.offset = offsetof(ECDSA_SIG, s),
		.field_name = "s",
		.item = &BIGNUM_it,
	},
};

const ASN1_ITEM ECDSA_SIG_it = {
	.itype = ASN1_ITYPE_SEQUENCE,
	.utype = V_ASN1_SEQUENCE,
	.templates = ECDSA_SIG_seq_tt,
	.tcount = sizeof(ECDSA_SIG_seq_tt) / sizeof(ASN1_TEMPLATE),
	.funcs = NULL,
	.size = sizeof(ECDSA_SIG),
	.sname = "ECDSA_SIG",
};

ECDSA_SIG *ECDSA_SIG_new(void);
void ECDSA_SIG_free(ECDSA_SIG *a);
ECDSA_SIG *d2i_ECDSA_SIG(ECDSA_SIG **a, const unsigned char **in, long len);
int i2d_ECDSA_SIG(const ECDSA_SIG *a, unsigned char **out);

ECDSA_SIG *
d2i_ECDSA_SIG(ECDSA_SIG **a, const unsigned char **in, long len)
{
	return (ECDSA_SIG *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &ECDSA_SIG_it);
}

int
i2d_ECDSA_SIG(const ECDSA_SIG *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &ECDSA_SIG_it);
}

ECDSA_SIG *
ECDSA_SIG_new(void)
{
	return (ECDSA_SIG *)ASN1_item_new(&ECDSA_SIG_it);
}

void
ECDSA_SIG_free(ECDSA_SIG *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &ECDSA_SIG_it);
}

void
ECDSA_SIG_get0(const ECDSA_SIG *sig, const BIGNUM **pr, const BIGNUM **ps)
{
	if (pr != NULL)
		*pr = sig->r;
	if (ps != NULL)
		*ps = sig->s;
}

const BIGNUM *
ECDSA_SIG_get0_r(const ECDSA_SIG *sig)
{
	return sig->r;
}

const BIGNUM *
ECDSA_SIG_get0_s(const ECDSA_SIG *sig)
{
	return sig->s;
}

int
ECDSA_SIG_set0(ECDSA_SIG *sig, BIGNUM *r, BIGNUM *s)
{
	if (r == NULL || s == NULL)
		return 0;

	BN_free(sig->r);
	BN_free(sig->s);
	sig->r = r;
	sig->s = s;
	return 1;
}

int
ECDSA_size(const EC_KEY *r)
{
	const EC_GROUP *group;
	const BIGNUM *order = NULL;
	ECDSA_SIG sig;
	int ret = 0;

	if (r == NULL)
		goto err;

	if ((group = EC_KEY_get0_group(r)) == NULL)
		goto err;

	if ((order = EC_GROUP_get0_order(group)) == NULL)
		goto err;

	sig.r = (BIGNUM *)order;
	sig.s = (BIGNUM *)order;

	if ((ret = i2d_ECDSA_SIG(&sig, NULL)) < 0)
		ret = 0;

 err:
	return ret;
}

/*
 * FIPS 186-5, section 6.4.1, step 2: convert hashed message into an integer.
 * Use the order_bits leftmost bits if it exceeds the group order.
 */
static int
ecdsa_prepare_digest(const unsigned char *digest, int digest_len,
    const EC_KEY *key, BIGNUM *e)
{
	const EC_GROUP *group;
	int digest_bits, order_bits;

	if (BN_bin2bn(digest, digest_len, e) == NULL) {
		ECDSAerror(ERR_R_BN_LIB);
		return 0;
	}

	if ((group = EC_KEY_get0_group(key)) == NULL)
		return 0;
	order_bits = EC_GROUP_order_bits(group);

	digest_bits = 8 * digest_len;
	if (digest_bits <= order_bits)
		return 1;

	return BN_rshift(e, e, digest_bits - order_bits);
}

int
ecdsa_sign(int type, const unsigned char *digest, int digest_len,
    unsigned char *signature, unsigned int *signature_len, const BIGNUM *kinv,
    const BIGNUM *r, EC_KEY *key)
{
	ECDSA_SIG *sig;
	int out_len = 0;
	int ret = 0;

	if ((sig = ECDSA_do_sign_ex(digest, digest_len, kinv, r, key)) == NULL)
		goto err;

	if ((out_len = i2d_ECDSA_SIG(sig, &signature)) < 0) {
		out_len = 0;
		goto err;
	}

	ret = 1;

 err:
	*signature_len = out_len;
	ECDSA_SIG_free(sig);

	return ret;
}

/*
 * FIPS 186-5, section 6.4.1, steps 3-8 and 11: Generate k, calculate r and
 * kinv, and clear it. If r == 0, try again with a new random k.
 */

int
ecdsa_sign_setup(EC_KEY *key, BN_CTX *in_ctx, BIGNUM **out_kinv, BIGNUM **out_r)
{
	const EC_GROUP *group;
	EC_POINT *point = NULL;
	BN_CTX *ctx = NULL;
	BIGNUM *k = NULL, *r = NULL;
	const BIGNUM *order;
	BIGNUM *x;
	int order_bits;
	int ret = 0;

	BN_free(*out_kinv);
	*out_kinv = NULL;

	BN_free(*out_r);
	*out_r = NULL;

	if (key == NULL) {
		ECDSAerror(ERR_R_PASSED_NULL_PARAMETER);
		goto err;
	}
	if ((group = EC_KEY_get0_group(key)) == NULL) {
		ECDSAerror(ERR_R_PASSED_NULL_PARAMETER);
		goto err;
	}

	if ((k = BN_new()) == NULL)
		goto err;
	if ((r = BN_new()) == NULL)
		goto err;

	if ((ctx = in_ctx) == NULL)
		ctx = BN_CTX_new();
	if (ctx == NULL) {
		ECDSAerror(ERR_R_MALLOC_FAILURE);
		goto err;
	}

	BN_CTX_start(ctx);

	if ((x = BN_CTX_get(ctx)) == NULL)
		goto err;

	if ((point = EC_POINT_new(group)) == NULL) {
		ECDSAerror(ERR_R_EC_LIB);
		goto err;
	}
	if ((order = EC_GROUP_get0_order(group)) == NULL) {
		ECDSAerror(ERR_R_EC_LIB);
		goto err;
	}

	if (BN_cmp(order, BN_value_one()) <= 0) {
		ECDSAerror(EC_R_INVALID_GROUP_ORDER);
		goto err;
	}

	/* Reject curves with an order that is smaller than 80 bits. */
	if ((order_bits = BN_num_bits(order)) < 80) {
		ECDSAerror(EC_R_INVALID_GROUP_ORDER);
		goto err;
	}

	/* Preallocate space. */
	if (!BN_set_bit(k, order_bits) ||
	    !BN_set_bit(r, order_bits) ||
	    !BN_set_bit(x, order_bits))
		goto err;

	/* Step 11: repeat until r != 0. */
	do {
		/* Step 3: generate random k. */
		if (!bn_rand_interval(k, BN_value_one(), order)) {
			ECDSAerror(ECDSA_R_RANDOM_NUMBER_GENERATION_FAILED);
			goto err;
		}

		/*
		 * We do not want timing information to leak the length of k,
		 * so we compute G * k using an equivalent scalar of fixed
		 * bit-length.
		 *
		 * We unconditionally perform both of these additions to prevent
		 * a small timing information leakage.  We then choose the sum
		 * that is one bit longer than the order.  This guarantees the
		 * code path used in the constant time implementations
		 * elsewhere.
		 *
		 * TODO: revisit the bn_copy aiming for a memory access agnostic
		 * conditional copy.
		 */
		if (!BN_add(r, k, order) ||
		    !BN_add(x, r, order) ||
		    !bn_copy(k, BN_num_bits(r) > order_bits ? r : x))
			goto err;

		BN_set_flags(k, BN_FLG_CONSTTIME);

		/* Step 5: P = k * G. */
		if (!EC_POINT_mul(group, point, k, NULL, NULL, ctx)) {
			ECDSAerror(ERR_R_EC_LIB);
			goto err;
		}
		/* Steps 6 (and 7): from P = (x, y) retain the x-coordinate. */
		if (!EC_POINT_get_affine_coordinates(group, point, x, NULL,
		    ctx)) {
			ECDSAerror(ERR_R_EC_LIB);
			goto err;
		}
		/* Step 8: r = x (mod order). */
		if (!BN_nnmod(r, x, order, ctx)) {
			ECDSAerror(ERR_R_BN_LIB);
			goto err;
		}
	} while (BN_is_zero(r));

	/* Step 4: calculate kinv. */
	if (BN_mod_inverse_ct(k, k, order, ctx) == NULL) {
		ECDSAerror(ERR_R_BN_LIB);
		goto err;
	}

	*out_kinv = k;
	k = NULL;

	*out_r = r;
	r = NULL;

	ret = 1;

 err:
	BN_CTX_end(ctx);
	if (ctx != in_ctx)
		BN_CTX_free(ctx);
	BN_free(k);
	BN_free(r);
	EC_POINT_free(point);

	return ret;
}

/*
 * FIPS 186-5, section 6.4.1, step 9: compute s = inv(k)(e + xr) mod order.
 * In order to reduce the possibility of a side-channel attack, the following
 * is calculated using a random blinding value b in [1, order):
 * s = inv(b)(be + bxr)inv(k) mod order.
 */

static int
ecdsa_compute_s(BIGNUM **out_s, const BIGNUM *e, const BIGNUM *kinv,
    const BIGNUM *r, const EC_KEY *key, BN_CTX *ctx)
{
	const EC_GROUP *group;
	const BIGNUM *order, *priv_key;
	BIGNUM *b, *binv, *be, *bxr;
	BIGNUM *s = NULL;
	int ret = 0;

	*out_s = NULL;

	BN_CTX_start(ctx);

	if ((group = EC_KEY_get0_group(key)) == NULL) {
		ECDSAerror(ERR_R_PASSED_NULL_PARAMETER);
		goto err;
	}
	if ((order = EC_GROUP_get0_order(group)) == NULL) {
		ECDSAerror(ERR_R_EC_LIB);
		goto err;
	}
	if ((priv_key = EC_KEY_get0_private_key(key)) == NULL) {
		ECDSAerror(ERR_R_PASSED_NULL_PARAMETER);
		goto err;
	}

	if ((b = BN_CTX_get(ctx)) == NULL)
		goto err;
	if ((binv = BN_CTX_get(ctx)) == NULL)
		goto err;
	if ((be = BN_CTX_get(ctx)) == NULL)
		goto err;
	if ((bxr = BN_CTX_get(ctx)) == NULL)
		goto err;

	if ((s = BN_new()) == NULL)
		goto err;

	/*
	 * In a valid ECDSA signature, r must be in [1, order). Since r can be
	 * caller provided - either directly or by replacing sign_setup() - we
	 * can't rely on this being the case.
	 */
	if (BN_cmp(r, BN_value_one()) < 0 || BN_cmp(r, order) >= 0) {
		ECDSAerror(ECDSA_R_BAD_SIGNATURE);
		goto err;
	}

	if (!bn_rand_interval(b, BN_value_one(), order)) {
		ECDSAerror(ERR_R_BN_LIB);
		goto err;
	}

	if (BN_mod_inverse_ct(binv, b, order, ctx) == NULL) {
		ECDSAerror(ERR_R_BN_LIB);
		goto err;
	}

	if (!BN_mod_mul(bxr, b, priv_key, order, ctx)) {
		ECDSAerror(ERR_R_BN_LIB);
		goto err;
	}
	if (!BN_mod_mul(bxr, bxr, r, order, ctx)) {
		ECDSAerror(ERR_R_BN_LIB);
		goto err;
	}
	if (!BN_mod_mul(be, b, e, order, ctx)) {
		ECDSAerror(ERR_R_BN_LIB);
		goto err;
	}
	if (!BN_mod_add(s, be, bxr, order, ctx)) {
		ECDSAerror(ERR_R_BN_LIB);
		goto err;
	}
	/* s = b(e + xr)k^-1 */
	if (!BN_mod_mul(s, s, kinv, order, ctx)) {
		ECDSAerror(ERR_R_BN_LIB);
		goto err;
	}
	/* s = (e + xr)k^-1 */
	if (!BN_mod_mul(s, s, binv, order, ctx)) {
		ECDSAerror(ERR_R_BN_LIB);
		goto err;
	}

	/* Step 11: if s == 0 start over. */
	if (!BN_is_zero(s)) {
		*out_s = s;
		s = NULL;
	}

	ret = 1;

 err:
	BN_CTX_end(ctx);
	BN_free(s);

	return ret;
}

/*
 * It is too expensive to check curve parameters on every sign operation.
 * Instead, cap the number of retries. A single retry is very unlikely, so
 * allowing 32 retries is amply enough.
 */
#define ECDSA_MAX_SIGN_ITERATIONS		32

/*
 * FIPS 186-5: Section 6.4.1: ECDSA signature generation, steps 2-12.
 * The caller provides the hash of the message, thus performs step 1.
 * Step 10, zeroing k and kinv, is done by BN_free().
 */

ECDSA_SIG *
ecdsa_sign_sig(const unsigned char *digest, int digest_len,
    const BIGNUM *in_kinv, const BIGNUM *in_r, EC_KEY *key)
{
	BN_CTX *ctx = NULL;
	BIGNUM *kinv = NULL, *r = NULL, *s = NULL;
	BIGNUM *e;
	int caller_supplied_values = 0;
	int attempts = 0;
	ECDSA_SIG *sig = NULL;

	if ((ctx = BN_CTX_new()) == NULL) {
		ECDSAerror(ERR_R_MALLOC_FAILURE);
		goto err;
	}

	BN_CTX_start(ctx);

	if ((e = BN_CTX_get(ctx)) == NULL)
		goto err;

	/* Step 2: convert hash into an integer. */
	if (!ecdsa_prepare_digest(digest, digest_len, key, e))
		goto err;

	if (in_kinv != NULL && in_r != NULL) {
		/*
		 * Use the caller's kinv and r. Don't call ECDSA_sign_setup().
		 * If we're unable to compute a valid signature, the caller
		 * must provide new values.
		 */
		caller_supplied_values = 1;

		if ((kinv = BN_dup(in_kinv)) == NULL) {
			ECDSAerror(ERR_R_MALLOC_FAILURE);
			goto err;
		}
		if ((r = BN_dup(in_r)) == NULL) {
			ECDSAerror(ERR_R_MALLOC_FAILURE);
			goto err;
		}
	}

	do {
		/* Steps 3-8: calculate kinv and r. */
		if (!caller_supplied_values) {
			if (!ECDSA_sign_setup(key, ctx, &kinv, &r)) {
				ECDSAerror(ERR_R_ECDSA_LIB);
				goto err;
			}
		}

		/*
		 * Steps 9 and 11: if s is non-NULL, we have a valid signature.
		 */
		if (!ecdsa_compute_s(&s, e, kinv, r, key, ctx))
			goto err;
		if (s != NULL)
			break;

		if (caller_supplied_values) {
			ECDSAerror(ECDSA_R_NEED_NEW_SETUP_VALUES);
			goto err;
		}

		if (++attempts > ECDSA_MAX_SIGN_ITERATIONS) {
			ECDSAerror(EC_R_WRONG_CURVE_PARAMETERS);
			goto err;
		}
	} while (1);

	/* Step 12: output (r, s). */
	if ((sig = ECDSA_SIG_new()) == NULL) {
		ECDSAerror(ERR_R_MALLOC_FAILURE);
		goto err;
	}
	if (!ECDSA_SIG_set0(sig, r, s)) {
		ECDSA_SIG_free(sig);
		goto err;
	}
	r = NULL;
	s = NULL;

 err:
	BN_CTX_end(ctx);
	BN_CTX_free(ctx);
	BN_free(kinv);
	BN_free(r);
	BN_free(s);

	return sig;
}

int
ecdsa_verify(int type, const unsigned char *digest, int digest_len,
    const unsigned char *sigbuf, int sig_len, EC_KEY *key)
{
	ECDSA_SIG *s;
	unsigned char *der = NULL;
	const unsigned char *p;
	int der_len = 0;
	int ret = -1;

	if ((s = ECDSA_SIG_new()) == NULL)
		goto err;

	p = sigbuf;
	if (d2i_ECDSA_SIG(&s, &p, sig_len) == NULL)
		goto err;

	/* Ensure signature uses DER and doesn't have trailing garbage */
	if ((der_len = i2d_ECDSA_SIG(s, &der)) != sig_len)
		goto err;
	if (timingsafe_memcmp(sigbuf, der, der_len))
		goto err;

	ret = ECDSA_do_verify(digest, digest_len, s, key);

 err:
	freezero(der, der_len);
	ECDSA_SIG_free(s);

	return ret;
}

/*
 * FIPS 186-5, section 6.4.2: ECDSA signature verification.
 * The caller provides us with the hash of the message, so has performed step 2.
 */

int
ecdsa_verify_sig(const unsigned char *digest, int digest_len,
    const ECDSA_SIG *sig, EC_KEY *key)
{
	const EC_GROUP *group;
	const EC_POINT *pub_key;
	EC_POINT *point = NULL;
	const BIGNUM *order;
	BN_CTX *ctx = NULL;
	BIGNUM *e, *sinv, *u, *v, *x;
	int ret = -1;

	if (key == NULL || sig == NULL) {
		ECDSAerror(ECDSA_R_MISSING_PARAMETERS);
		goto err;
	}
	if ((group = EC_KEY_get0_group(key)) == NULL) {
		ECDSAerror(ECDSA_R_MISSING_PARAMETERS);
		goto err;
	}
	if ((pub_key = EC_KEY_get0_public_key(key)) == NULL) {
		ECDSAerror(ECDSA_R_MISSING_PARAMETERS);
		goto err;
	}

	if ((ctx = BN_CTX_new()) == NULL) {
		ECDSAerror(ERR_R_MALLOC_FAILURE);
		goto err;
	}

	BN_CTX_start(ctx);

	if ((e = BN_CTX_get(ctx)) == NULL)
		goto err;
	if ((sinv = BN_CTX_get(ctx)) == NULL)
		goto err;
	if ((u = BN_CTX_get(ctx)) == NULL)
		goto err;
	if ((v = BN_CTX_get(ctx)) == NULL)
		goto err;
	if ((x = BN_CTX_get(ctx)) == NULL)
		goto err;

	if ((order = EC_GROUP_get0_order(group)) == NULL) {
		ECDSAerror(ERR_R_EC_LIB);
		goto err;
	}

	/* Step 1: verify that r and s are in the range [1, order). */
	if (BN_cmp(sig->r, BN_value_one()) < 0 || BN_cmp(sig->r, order) >= 0) {
		ECDSAerror(ECDSA_R_BAD_SIGNATURE);
		ret = 0;
		goto err;
	}
	if (BN_cmp(sig->s, BN_value_one()) < 0 || BN_cmp(sig->s, order) >= 0) {
		ECDSAerror(ECDSA_R_BAD_SIGNATURE);
		ret = 0;
		goto err;
	}

	/* Step 3: convert the hash into an integer. */
	if (!ecdsa_prepare_digest(digest, digest_len, key, e))
		goto err;

	/* Step 4: compute the inverse of s modulo order. */
	if (BN_mod_inverse_ct(sinv, sig->s, order, ctx) == NULL) {
		ECDSAerror(ERR_R_BN_LIB);
		goto err;
	}
	/* Step 5: compute u = s^-1 * e and v = s^-1 * r (modulo order). */
	if (!BN_mod_mul(u, e, sinv, order, ctx)) {
		ECDSAerror(ERR_R_BN_LIB);
		goto err;
	}
	if (!BN_mod_mul(v, sig->r, sinv, order, ctx)) {
		ECDSAerror(ERR_R_BN_LIB);
		goto err;
	}

	/*
	 * Steps 6 and 7: compute R = G * u + pub_key * v = (x, y). Reject if
	 * it's the point at infinity - getting affine coordinates fails. Keep
	 * the x coordinate.
	 */
	if ((point = EC_POINT_new(group)) == NULL) {
		ECDSAerror(ERR_R_MALLOC_FAILURE);
		goto err;
	}
	if (!EC_POINT_mul(group, point, u, pub_key, v, ctx)) {
		ECDSAerror(ERR_R_EC_LIB);
		goto err;
	}
	if (!EC_POINT_get_affine_coordinates(group, point, x, NULL, ctx)) {
		ECDSAerror(ERR_R_EC_LIB);
		goto err;
	}
	/* Step 8: convert x to a number in [0, order). */
	if (!BN_nnmod(x, x, order, ctx)) {
		ECDSAerror(ERR_R_BN_LIB);
		goto err;
	}

	/* Step 9: the signature is valid iff the x-coordinate is equal to r. */
	ret = (BN_cmp(x, sig->r) == 0);

 err:
	BN_CTX_end(ctx);
	BN_CTX_free(ctx);
	EC_POINT_free(point);

	return ret;
}

ECDSA_SIG *
ECDSA_do_sign(const unsigned char *digest, int digest_len, EC_KEY *key)
{
	return ECDSA_do_sign_ex(digest, digest_len, NULL, NULL, key);
}

ECDSA_SIG *
ECDSA_do_sign_ex(const unsigned char *digest, int digest_len,
    const BIGNUM *kinv, const BIGNUM *out_r, EC_KEY *key)
{
	if (key->meth->sign_sig == NULL) {
		ECDSAerror(EVP_R_METHOD_NOT_SUPPORTED);
		return 0;
	}
	return key->meth->sign_sig(digest, digest_len, kinv, out_r, key);
}

int
ECDSA_sign(int type, const unsigned char *digest, int digest_len,
    unsigned char *signature, unsigned int *signature_len, EC_KEY *key)
{
	return ECDSA_sign_ex(type, digest, digest_len, signature, signature_len,
	    NULL, NULL, key);
}

int
ECDSA_sign_ex(int type, const unsigned char *digest, int digest_len,
    unsigned char *signature, unsigned int *signature_len, const BIGNUM *kinv,
    const BIGNUM *r, EC_KEY *key)
{
	if (key->meth->sign == NULL) {
		ECDSAerror(EVP_R_METHOD_NOT_SUPPORTED);
		return 0;
	}
	return key->meth->sign(type, digest, digest_len, signature,
	    signature_len, kinv, r, key);
}

int
ECDSA_sign_setup(EC_KEY *key, BN_CTX *in_ctx, BIGNUM **out_kinv,
    BIGNUM **out_r)
{
	if (key->meth->sign_setup == NULL) {
		ECDSAerror(EVP_R_METHOD_NOT_SUPPORTED);
		return 0;
	}
	return key->meth->sign_setup(key, in_ctx, out_kinv, out_r);
}

int
ECDSA_do_verify(const unsigned char *digest, int digest_len,
    const ECDSA_SIG *sig, EC_KEY *key)
{
	if (key->meth->verify_sig == NULL) {
		ECDSAerror(EVP_R_METHOD_NOT_SUPPORTED);
		return 0;
	}
	return key->meth->verify_sig(digest, digest_len, sig, key);
}

int
ECDSA_verify(int type, const unsigned char *digest, int digest_len,
    const unsigned char *sigbuf, int sig_len, EC_KEY *key)
{
	if (key->meth->verify == NULL) {
		ECDSAerror(EVP_R_METHOD_NOT_SUPPORTED);
		return 0;
	}
	return key->meth->verify(type, digest, digest_len, sigbuf, sig_len, key);
}