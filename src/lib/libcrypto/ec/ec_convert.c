/* $OpenBSD: ec_convert.c,v 1.1 2024/10/30 18:14:49 tb Exp $ */
/*
 * Originally written by Bodo Moeller for the OpenSSL project.
 */
/* ====================================================================
 * Copyright (c) 1998-2003 The OpenSSL Project.  All rights reserved.
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
 * Binary polynomial ECC support in OpenSSL originally developed by
 * SUN MICROSYSTEMS, INC., and contributed to the OpenSSL project.
 */

#include <string.h>

#include <openssl/asn1.h>
#include <openssl/err.h>

#include "asn1_local.h"
#include "ec_local.h"

int
ec_point_to_octets(const EC_GROUP *group, const EC_POINT *point, int form,
    unsigned char **out_buf, size_t *out_len, BN_CTX *ctx)
{
	unsigned char *buf = NULL;
	size_t len = 0;
	int ret = 0;

	if (out_buf != NULL && *out_buf != NULL)
		goto err;

	*out_len = 0;

	if ((len = EC_POINT_point2oct(group, point, form, NULL, 0, ctx)) == 0)
		goto err;

	if (out_buf == NULL)
		goto done;

	if ((buf = calloc(1, len)) == NULL)
		goto err;
	if (EC_POINT_point2oct(group, point, form, buf, len, ctx) != len)
		goto err;

	*out_buf = buf;
	buf = NULL;

 done:
	*out_len = len;

	ret = 1;

 err:
	freezero(buf, len);

	return ret;
}

int
ec_point_from_octets(const EC_GROUP *group, const unsigned char *buf, size_t buf_len,
    EC_POINT **out_point, uint8_t *out_form, BN_CTX *ctx)
{
	EC_POINT *point;
	int ret = 0;

	if ((point = *out_point) == NULL)
		point = EC_POINT_new(group);
	if (point == NULL)
		goto err;

	if (!EC_POINT_oct2point(group, point, buf, buf_len, ctx))
		goto err;

	if (out_form != NULL)
		*out_form = buf[0] & ~1U;	/* XXX - EC_OCT_YBIT */

	*out_point = point;
	point = NULL;

	ret = 1;

 err:
	if (*out_point != point)
		EC_POINT_free(point);

	return ret;
}

size_t
EC_POINT_point2oct(const EC_GROUP *group, const EC_POINT *point,
    point_conversion_form_t form, unsigned char *buf, size_t len,
    BN_CTX *ctx_in)
{
	BN_CTX *ctx;
	size_t ret = 0;

	if ((ctx = ctx_in) == NULL)
		ctx = BN_CTX_new();
	if (ctx == NULL)
		goto err;

	if (group->meth->point2oct == NULL) {
		ECerror(ERR_R_SHOULD_NOT_HAVE_BEEN_CALLED);
		goto err;
	}
	if (group->meth != point->meth) {
		ECerror(EC_R_INCOMPATIBLE_OBJECTS);
		goto err;
	}
	ret = group->meth->point2oct(group, point, form, buf, len, ctx);

 err:
	if (ctx != ctx_in)
		BN_CTX_free(ctx);

	return ret;
}
LCRYPTO_ALIAS(EC_POINT_point2oct);

int
EC_POINT_oct2point(const EC_GROUP *group, EC_POINT *point,
    const unsigned char *buf, size_t len, BN_CTX *ctx_in)
{
	BN_CTX *ctx;
	int ret = 0;

	if ((ctx = ctx_in) == NULL)
		ctx = BN_CTX_new();
	if (ctx == NULL)
		goto err;

	if (group->meth->oct2point == NULL) {
		ECerror(ERR_R_SHOULD_NOT_HAVE_BEEN_CALLED);
		goto err;
	}
	if (group->meth != point->meth) {
		ECerror(EC_R_INCOMPATIBLE_OBJECTS);
		goto err;
	}
	ret = group->meth->oct2point(group, point, buf, len, ctx);

 err:
	if (ctx != ctx_in)
		BN_CTX_free(ctx);

	return ret;
}
LCRYPTO_ALIAS(EC_POINT_oct2point);
