/*	$OpenBSD: ec_point_conversion.c,v 1.17 2024/10/23 15:06:46 tb Exp $ */
/*
 * Copyright (c) 2021 Theo Buehler <tb@openbsd.org>
 * Copyright (c) 2021 Joel Sing <jsing@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/objects.h>

int forms[] = {
	POINT_CONVERSION_COMPRESSED,
	POINT_CONVERSION_UNCOMPRESSED,
	POINT_CONVERSION_HYBRID,
};

static const size_t N_FORMS = sizeof(forms) / sizeof(forms[0]);
#define N_RANDOM_POINTS 10

static const char *
form2str(int form)
{
	switch (form) {
	case POINT_CONVERSION_COMPRESSED:
		return "compressed form";
	case POINT_CONVERSION_UNCOMPRESSED:
		return "uncompressed form";
	case POINT_CONVERSION_HYBRID:
		return "hybrid form";
	default:
		return "unknown form";
	}
}

static void
hexdump(const unsigned char *buf, size_t len)
{
	size_t i;

	for (i = 1; i <= len; i++)
		fprintf(stderr, " 0x%02hhx,%s", buf[i - 1], i % 8 ? "" : "\n");
	if (len % 8)
		fprintf(stderr, "\n");
}

static int
roundtrip(EC_GROUP *group, EC_POINT *point, int form, BIGNUM *x, BIGNUM *y)
{
	BIGNUM *x_out = NULL, *y_out = NULL;
	size_t len;
	uint8_t *buf = NULL;
	int failed = 1;

	if ((len = EC_POINT_point2oct(group, point, form, NULL, 0, NULL)) == 0)
		errx(1, "point2oct");
	if ((buf = malloc(len)) == NULL)
		errx(1, "malloc");
	if (EC_POINT_point2oct(group, point, form, buf, len, NULL) != len)
		errx(1, "point2oct");

	if (!EC_POINT_oct2point(group, point, buf, len, NULL))
		errx(1, "%s oct2point", form2str(form));

	if ((x_out = BN_new()) == NULL)
		errx(1, "new x_out");
	if ((y_out = BN_new()) == NULL)
		errx(1, "new y_out");

	if (!EC_POINT_get_affine_coordinates(group, point, x_out, y_out, NULL))
		errx(1, "get affine");

	if (BN_cmp(x, x_out) != 0) {
		warnx("%s: x", form2str(form));
		goto err;
	}
	if (BN_cmp(y, y_out) != 0) {
		warnx("%s: y", form2str(form));
		goto err;
	}

	failed = 0;

 err:
	if (failed)
		hexdump(buf, len);

	free(buf);
	BN_free(x_out);
	BN_free(y_out);

	return failed;
}

/* XXX This only tests multiples of the generator for now... */
static int
test_random_points_on_curve(EC_builtin_curve *curve)
{
	EC_GROUP *group;
	BIGNUM *order = NULL;
	BIGNUM *random;
	BIGNUM *x, *y;
	size_t i, j;
	int failed = 0;

	if ((group = EC_GROUP_new_by_curve_name(curve->nid)) == NULL)
		errx(1, "EC_GROUP_new_by_curve_name(%s)",
		    OBJ_nid2sn(curve->nid));

	if ((order = BN_new()) == NULL)
		errx(1, "BN_new order");
	if ((random = BN_new()) == NULL)
		errx(1, "BN_new random");
	if ((x = BN_new()) == NULL)
		errx(1, "BN_new x");
	if ((y = BN_new()) == NULL)
		errx(1, "BN_new y");

	if (!EC_GROUP_get_order(group, order, NULL))
		errx(1, "EC_group_get_order");

	for (i = 0; i < N_RANDOM_POINTS; i++) {
		EC_POINT *random_point;

		do {
			if (!BN_rand_range(random, order))
				errx(1, "BN_rand_range");
		} while (BN_is_zero(random));

		if ((random_point = EC_POINT_new(group)) == NULL)
			errx(1, "EC_POINT_new");

		if (!EC_POINT_mul(group, random_point, random, NULL, NULL, NULL))
			errx(1, "EC_POINT_mul");

		if (EC_POINT_is_at_infinity(group, random_point)) {
			EC_POINT_free(random_point);

			warnx("info: got infinity");
			fprintf(stderr, "random = ");
			BN_print_fp(stderr, random);
			fprintf(stderr, "\n");

			continue;
		}

		if (!EC_POINT_get_affine_coordinates(group, random_point,
		    x, y, NULL))
			errx(1, "EC_POINT_get_affine_coordinates");

		for (j = 0; j < N_FORMS; j++)
			failed |= roundtrip(group, random_point, forms[j], x, y);

		EC_POINT_free(random_point);
	}

	BN_free(order);
	BN_free(random);
	BN_free(x);
	BN_free(y);
	EC_GROUP_free(group);

	return failed;
}

static int
test_random_points(void)
{
	EC_builtin_curve *all_curves = NULL;
	size_t ncurves = 0;
	size_t curve_id;
	int failed = 0;

	ncurves = EC_get_builtin_curves(NULL, 0);
	if ((all_curves = calloc(ncurves, sizeof(EC_builtin_curve))) == NULL)
		err(1, "calloc builtin curves");
	EC_get_builtin_curves(all_curves, ncurves);

	for (curve_id = 0; curve_id < ncurves; curve_id++)
		failed |= test_random_points_on_curve(&all_curves[curve_id]);

	fprintf(stderr, "%s %s\n", __func__, failed ? ": FAILED" : "");

	free(all_curves);
	return failed;
}

static const struct point_conversion {
	const char *description;
	int nid;
	uint8_t octets[256];
	size_t octets_len;
	int valid;
	int point_at_infinity;
} point_conversions[] = {
	/* XXX - now that sect571 is no longer tested, add another test? */
	{
		.description = "point at infinity on secp256r1",
		.nid = NID_X9_62_prime256v1,
		.octets = { 0x00 },
		.octets_len = 1,
		.valid = 1,
		.point_at_infinity = 1,
	},
	{
		.description = "point at infinity on secp256r1 (flipped y_bit)",
		.nid = NID_X9_62_prime256v1,
		.octets = { 0x01 },
		.octets_len = 1,
		.valid = 0,
		.point_at_infinity = 1,
	},
	{
		.description = "zero x compressed point on secp256r1",
		.nid = NID_X9_62_prime256v1,
		.octets = {
			0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00,
		},
		.octets_len = 33,
		.valid = 1,
	},
	{
		.description =
		    "zero x compressed point on secp256r1 (flipped y_bit)",
		.nid = NID_X9_62_prime256v1,
		.octets = {
			0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00,
		},
		.octets_len = 33,
		.valid = 1,
	},
	{
		.description = "generic compressed point on secp256r1",
		.nid = NID_X9_62_prime256v1,
		.octets = {
			0x03, 0xa3, 0x96, 0xa0, 0x42, 0x73, 0x1a, 0x8b,
			0x90, 0xd8, 0xcb, 0xae, 0xda, 0x1b, 0x23, 0x11,
			0x77, 0x5f, 0x6a, 0x4c, 0xb4, 0x57, 0xbf, 0xe0,
			0x65, 0xd4, 0x09, 0x11, 0x5f, 0x54, 0xe4, 0xee,
			0xdd,
		},
		.octets_len = 33,
		.valid = 1,
	},
	{
		.description =
		    "generic compressed point on secp256r1 (flipped y_bit)",
		.nid = NID_X9_62_prime256v1,
		.octets = {
			0x02, 0xa3, 0x96, 0xa0, 0x42, 0x73, 0x1a, 0x8b,
			0x90, 0xd8, 0xcb, 0xae, 0xda, 0x1b, 0x23, 0x11,
			0x77, 0x5f, 0x6a, 0x4c, 0xb4, 0x57, 0xbf, 0xe0,
			0x65, 0xd4, 0x09, 0x11, 0x5f, 0x54, 0xe4, 0xee,
			0xdd,
		},
		.octets_len = 33,
		.valid = 1,
	},
	{
		.description = "zero x uncompressed point #1 on secp256r1",
		.nid = NID_X9_62_prime256v1,
		.octets = {
			0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x66, 0x48, 0x5c, 0x78, 0x0e, 0x2f, 0x83,
			0xd7, 0x24, 0x33, 0xbd, 0x5d, 0x84, 0xa0, 0x6b,
			0xb6, 0x54, 0x1c, 0x2a, 0xf3, 0x1d, 0xae, 0x87,
			0x17, 0x28, 0xbf, 0x85, 0x6a, 0x17, 0x4f, 0x93,
			0xf4,
		},
		.octets_len = 65,
		.valid = 1,
	},
	{
		.description =
		    "zero x uncompressed point #1 on secp256r1 (flipped y_bit)",
		.nid = NID_X9_62_prime256v1,
		.octets = {
			0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x66, 0x48, 0x5c, 0x78, 0x0e, 0x2f, 0x83,
			0xd7, 0x24, 0x33, 0xbd, 0x5d, 0x84, 0xa0, 0x6b,
			0xb6, 0x54, 0x1c, 0x2a, 0xf3, 0x1d, 0xae, 0x87,
			0x17, 0x28, 0xbf, 0x85, 0x6a, 0x17, 0x4f, 0x93,
			0xf4,
		},
		.octets_len = 65,
		.valid = 0,
	},
	{
		.description = "zero x uncompressed point #2 on secp256r1",
		.nid = NID_X9_62_prime256v1,
		.octets = {
			0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x99, 0xb7, 0xa3, 0x86, 0xf1, 0xd0, 0x7c,
			0x29, 0xdb, 0xcc, 0x42, 0xa2, 0x7b, 0x5f, 0x94,
			0x49, 0xab, 0xe3, 0xd5, 0x0d, 0xe2, 0x51, 0x78,
			0xe8, 0xd7, 0x40, 0x7a, 0x95, 0xe8, 0xb0, 0x6c,
			0x0b,
		},
		.octets_len = 65,
		.valid = 1,
	},
	{
		.description =
		    "zero x uncompressed point #2 on secp256r1 (flipped y_bit)",
		.nid = NID_X9_62_prime256v1,
		.octets = {
			0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x99, 0xb7, 0xa3, 0x86, 0xf1, 0xd0, 0x7c,
			0x29, 0xdb, 0xcc, 0x42, 0xa2, 0x7b, 0x5f, 0x94,
			0x49, 0xab, 0xe3, 0xd5, 0x0d, 0xe2, 0x51, 0x78,
			0xe8, 0xd7, 0x40, 0x7a, 0x95, 0xe8, 0xb0, 0x6c,
			0x0b,
		},
		.octets_len = 65,
		.valid = 0,
	},
	{
		.description = "generic uncompressed point on secp256r1",
		.nid = NID_X9_62_prime256v1,
		.octets = {
			0x04, 0x23, 0xe5, 0x85, 0xa5, 0x4b, 0xda, 0x34,
			0x7e, 0xe5, 0x65, 0x53, 0x7f, 0x3b, 0xce, 0xe4,
			0x54, 0xd8, 0xa4, 0x5a, 0x53, 0x4b, 0xb0, 0x4c,
			0xb9, 0x31, 0x09, 0x29, 0xa2, 0x03, 0x4c, 0x73,
			0x20, 0xd2, 0xc6, 0x17, 0xca, 0xe3, 0xcf, 0xc2,
			0xd8, 0x31, 0xfe, 0xf1, 0x7c, 0x6f, 0x9d, 0x7a,
			0x01, 0x7c, 0x34, 0x65, 0x42, 0x05, 0xaf, 0xcc,
			0x04, 0xa3, 0x2f, 0x44, 0x14, 0xbe, 0xd8, 0xc2,
			0x03,
		},
		.octets_len = 65,
		.valid = 1,
	},
	{
		.description =
		    "generic uncompressed point on secp256r1 (flipped y_bit)",
		.nid = NID_X9_62_prime256v1,
		.octets = {
			0x05, 0x23, 0xe5, 0x85, 0xa5, 0x4b, 0xda, 0x34,
			0x7e, 0xe5, 0x65, 0x53, 0x7f, 0x3b, 0xce, 0xe4,
			0x54, 0xd8, 0xa4, 0x5a, 0x53, 0x4b, 0xb0, 0x4c,
			0xb9, 0x31, 0x09, 0x29, 0xa2, 0x03, 0x4c, 0x73,
			0x20, 0xd2, 0xc6, 0x17, 0xca, 0xe3, 0xcf, 0xc2,
			0xd8, 0x31, 0xfe, 0xf1, 0x7c, 0x6f, 0x9d, 0x7a,
			0x01, 0x7c, 0x34, 0x65, 0x42, 0x05, 0xaf, 0xcc,
			0x04, 0xa3, 0x2f, 0x44, 0x14, 0xbe, 0xd8, 0xc2,
			0x03,
		},
		.octets_len = 65,
		.valid = 0,
	},
	{
		.description = "zero x hybrid point #1 on secp256r1",
		.nid = NID_X9_62_prime256v1,
		.octets = {
			0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x66, 0x48, 0x5c, 0x78, 0x0e, 0x2f, 0x83,
			0xd7, 0x24, 0x33, 0xbd, 0x5d, 0x84, 0xa0, 0x6b,
			0xb6, 0x54, 0x1c, 0x2a, 0xf3, 0x1d, 0xae, 0x87,
			0x17, 0x28, 0xbf, 0x85, 0x6a, 0x17, 0x4f, 0x93,
			0xf4,
		},
		.octets_len = 65,
		.valid = 1,
	},
	{
		.description =
		    "zero x hybrid point #1 on secp256r1 (flipped y_bit)",
		.nid = NID_X9_62_prime256v1,
		.octets = {
			0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x66, 0x48, 0x5c, 0x78, 0x0e, 0x2f, 0x83,
			0xd7, 0x24, 0x33, 0xbd, 0x5d, 0x84, 0xa0, 0x6b,
			0xb6, 0x54, 0x1c, 0x2a, 0xf3, 0x1d, 0xae, 0x87,
			0x17, 0x28, 0xbf, 0x85, 0x6a, 0x17, 0x4f, 0x93,
			0xf4,
		},
		.octets_len = 65,
		.valid = 0,
	},
	{
		.description = "zero x hybrid point #2 on secp256r1",
		.nid = NID_X9_62_prime256v1,
		.octets = {
			0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x99, 0xb7, 0xa3, 0x86, 0xf1, 0xd0, 0x7c,
			0x29, 0xdb, 0xcc, 0x42, 0xa2, 0x7b, 0x5f, 0x94,
			0x49, 0xab, 0xe3, 0xd5, 0x0d, 0xe2, 0x51, 0x78,
			0xe8, 0xd7, 0x40, 0x7a, 0x95, 0xe8, 0xb0, 0x6c,
			0x0b,
		},
		.octets_len = 65,
		.valid = 1,
	},
	{
		.description =
		    "zero x hybrid point #2 on secp256r1 (flipped y_bit)",
		.nid = NID_X9_62_prime256v1,
		.octets = {
			0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x99, 0xb7, 0xa3, 0x86, 0xf1, 0xd0, 0x7c,
			0x29, 0xdb, 0xcc, 0x42, 0xa2, 0x7b, 0x5f, 0x94,
			0x49, 0xab, 0xe3, 0xd5, 0x0d, 0xe2, 0x51, 0x78,
			0xe8, 0xd7, 0x40, 0x7a, 0x95, 0xe8, 0xb0, 0x6c,
			0x0b,
		},
		.octets_len = 65,
		.valid = 0,
	},
	{
		.description = "generic hybrid point on secp256r1",
		.nid = NID_X9_62_prime256v1,
		.octets = {
			0x07, 0x38, 0xb2, 0x98, 0x38, 0x21, 0x6b, 0xec,
			0x87, 0xcf, 0x50, 0xbb, 0x65, 0x11, 0x96, 0x63,
			0xf3, 0x90, 0x64, 0xc3, 0x5c, 0x59, 0xa5, 0x6f,
			0xaf, 0x56, 0x2a, 0x0c, 0xc0, 0x3a, 0x9b, 0x92,
			0x85, 0x95, 0x54, 0xf3, 0x08, 0x0f, 0x78, 0x59,
			0xa2, 0x44, 0x2f, 0x19, 0x5d, 0xd5, 0xcd, 0xf6,
			0xa5, 0xbe, 0x2f, 0x83, 0x70, 0x94, 0xf5, 0xcd,
			0x8c, 0x40, 0x7f, 0xd8, 0x97, 0x92, 0x14, 0xf7,
			0xc5,
		},
		.octets_len = 65,
		.valid = 1,
	},
	{
		.description =
		    "generic hybrid point on secp256r1 (flipped y_bit)",
		.nid = NID_X9_62_prime256v1,
		.octets = {
			0x06, 0x38, 0xb2, 0x98, 0x38, 0x21, 0x6b, 0xec,
			0x87, 0xcf, 0x50, 0xbb, 0x65, 0x11, 0x96, 0x63,
			0xf3, 0x90, 0x64, 0xc3, 0x5c, 0x59, 0xa5, 0x6f,
			0xaf, 0x56, 0x2a, 0x0c, 0xc0, 0x3a, 0x9b, 0x92,
			0x85, 0x95, 0x54, 0xf3, 0x08, 0x0f, 0x78, 0x59,
			0xa2, 0x44, 0x2f, 0x19, 0x5d, 0xd5, 0xcd, 0xf6,
			0xa5, 0xbe, 0x2f, 0x83, 0x70, 0x94, 0xf5, 0xcd,
			0x8c, 0x40, 0x7f, 0xd8, 0x97, 0x92, 0x14, 0xf7,
			0xc5,
		},
		.octets_len = 65,
		.valid = 0,
	},
};

static const size_t N_POINT_CONVERSIONS =
    sizeof(point_conversions) / sizeof(point_conversions[0]);

static int
check_point_at_infinity(const EC_GROUP *group, const EC_POINT *point,
    const struct point_conversion *test)
{
	const uint8_t conversion_forms[4] = { 0x00, 0x02, 0x04, 0x06, };
	uint8_t buf[1];
	uint8_t form;
	size_t i, ret;
	int failed = 0;

	/* The form for the point at infinity is expected to fail. */
	form = conversion_forms[0];

	ret = EC_POINT_point2oct(group, point, form, buf, sizeof(buf), NULL);
	if (ret != 0) {
		fprintf(stderr, "FAIL: %s: expected encoding with form 0x%02x "
		    "to fail, got %zu\n", test->description, form, ret);
		failed |= 1;
	}

	/* For all other forms we expect the zero octet. */
	for (i = 1; i < sizeof(conversion_forms); i++) {
		form = conversion_forms[i];

		ret = EC_POINT_point2oct(group, point, form, buf, sizeof(buf), NULL);
		if (ret != 1) {
			fprintf(stderr, "FAIL: %s: expected success, got %zu\n",
			    test->description, ret);
			failed |= 1;
			continue;
		}

		if (memcmp(buf, test->octets, test->octets_len) != 0) {
			fprintf(stderr, "FAIL: %s: want 0x%02x, got 0x%02x\n",
			    test->description, test->octets[0], buf[0]);
			failed |= 1;
			continue;
		}
	}

	return failed;
}

static int
point_conversion_form_y_bit(const struct point_conversion *test)
{
	EC_GROUP *group = NULL;
	EC_POINT *point = NULL;
	int ret;
	int failed = 0;

	if ((group = EC_GROUP_new_by_curve_name(test->nid)) == NULL)
		errx(1, "group");
	if ((point = EC_POINT_new(group)) == NULL)
		errx(1, "point");

	ret = EC_POINT_oct2point(group, point, test->octets, test->octets_len,
	    NULL);
	if (ret != test->valid) {
		fprintf(stderr, "%s want %d got %d\n", test->description,
		    test->valid, ret);
		failed |= 1;
	}

	if (test->valid && test->point_at_infinity) {
		failed |= check_point_at_infinity(group, point, test);
	} else if (test->valid) {
		uint8_t buf[256];
		uint8_t form = test->octets[0] & 0x06;
		size_t len;

		len = EC_POINT_point2oct(group, point, form, buf, sizeof(buf), NULL);

		if (len != test->octets_len) {
			fprintf(stderr, "%s: EC_POINT_point2oct: want %zu, got %zu\n",
			    test->description, test->octets_len, len);
			failed |= 1;
			goto failed;
		}
		if (memcmp(test->octets, buf, len) != 0) {
			fprintf(stderr, "%s: unexpected encoding\nwant:\n",
			    test->description);
			hexdump(test->octets, test->octets_len);
			fprintf(stderr, "\ngot:\n");
			hexdump(buf, len);
			failed |= 1;
			goto failed;
		}
	}

 failed:
	EC_GROUP_free(group);
	EC_POINT_free(point);

	return failed;
}

static int
test_point_conversions(void)
{
	size_t i;
	int failed = 0;

	for (i = 0; i < N_POINT_CONVERSIONS; i++)
		failed |= point_conversion_form_y_bit(&point_conversions[i]);

	fprintf(stderr, "%s %s\n", __func__, failed ? ": FAILED" : "");

	return failed;
}

int
main(int argc, char **argv)
{
	int failed = 0;

	failed |= test_random_points();
	failed |= test_point_conversions();

	return failed;
}
