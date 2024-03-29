/*	$OpenBSD: whirlpool_test.c,v 1.1 2024/03/29 07:13:38 joshua Exp $ */
/*
 * Copyright (c) 2024 Joshua Sing <joshua@joshuasing.dev>
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

#include <openssl/evp.h>
#include <openssl/whrlpool.h>

#include <stdint.h>
#include <string.h>

struct whirlpool_test {
	const uint8_t in[128];
	const size_t in_len;
	const uint8_t out[EVP_MAX_MD_SIZE];
};

static const struct whirlpool_test whirlpool_tests[] = {
	{
		.in = "",
		.in_len = 0,
		.out = {
			0x19, 0xfa, 0x61, 0xd7, 0x55, 0x22, 0xa4, 0x66,
			0x9b, 0x44, 0xe3, 0x9c, 0x1d, 0x2e, 0x17, 0x26,
			0xc5, 0x30, 0x23, 0x21, 0x30, 0xd4, 0x07, 0xf8,
			0x9a, 0xfe, 0xe0, 0x96, 0x49, 0x97, 0xf7, 0xa7,
			0x3e, 0x83, 0xbe, 0x69, 0x8b, 0x28, 0x8f, 0xeb,
			0xcf, 0x88, 0xe3, 0xe0, 0x3c, 0x4f, 0x07, 0x57,
			0xea, 0x89, 0x64, 0xe5, 0x9b, 0x63, 0xd9, 0x37,
			0x08, 0xb1, 0x38, 0xcc, 0x42, 0xa6, 0x6e, 0xb3,
		},
	},
	{
		.in = "a",
		.in_len = 1,
		.out = {
			0x8a,  0xca,  0x26,  0x02,  0x79,  0x2a,  0xec,  0x6f, 
			0x11,  0xa6,  0x72,  0x06,  0x53,  0x1f,  0xb7,  0xd7, 
			0xf0,  0xdf,  0xf5,  0x94,  0x13,  0x14,  0x5e,  0x69, 
			0x73,  0xc4,  0x50,  0x01,  0xd0,  0x08,  0x7b,  0x42, 
			0xd1,  0x1b,  0xc6,  0x45,  0x41,  0x3a,  0xef,  0xf6, 
			0x3a,  0x42,  0x39,  0x1a,  0x39,  0x14,  0x5a,  0x59, 
			0x1a,  0x92,  0x20,  0x0d,  0x56,  0x01,  0x95,  0xe5, 
			0x3b,  0x47,  0x85,  0x84,  0xfd,  0xae,  0x23,  0x1a, 
		},
	},
	{
		.in = "abc",
		.in_len = 3,
		.out = {
			0x4e,  0x24,  0x48,  0xa4,  0xc6,  0xf4,  0x86,  0xbb, 
			0x16,  0xb6,  0x56,  0x2c,  0x73,  0xb4,  0x02,  0x0b, 
			0xf3,  0x04,  0x3e,  0x3a,  0x73,  0x1b,  0xce,  0x72, 
			0x1a,  0xe1,  0xb3,  0x03,  0xd9,  0x7e,  0x6d,  0x4c, 
			0x71,  0x81,  0xee,  0xbd,  0xb6,  0xc5,  0x7e,  0x27, 
			0x7d,  0x0e,  0x34,  0x95,  0x71,  0x14,  0xcb,  0xd6, 
			0xc7,  0x97,  0xfc,  0x9d,  0x95,  0xd8,  0xb5,  0x82, 
			0xd2,  0x25,  0x29,  0x20,  0x76,  0xd4,  0xee,  0xf5, 
		},
	},
	{
		.in = "message digest",
		.in_len = 14,
		.out = {
			0x37,  0x8c,  0x84,  0xa4,  0x12,  0x6e,  0x2d,  0xc6, 
			0xe5,  0x6d,  0xcc,  0x74,  0x58,  0x37,  0x7a,  0xac, 
			0x83,  0x8d,  0x00,  0x03,  0x22,  0x30,  0xf5,  0x3c, 
			0xe1,  0xf5,  0x70,  0x0c,  0x0f,  0xfb,  0x4d,  0x3b, 
			0x84,  0x21,  0x55,  0x76,  0x59,  0xef,  0x55,  0xc1, 
			0x06,  0xb4,  0xb5,  0x2a,  0xc5,  0xa4,  0xaa,  0xa6, 
			0x92,  0xed,  0x92,  0x00,  0x52,  0x83,  0x8f,  0x33, 
			0x62,  0xe8,  0x6d,  0xbd,  0x37,  0xa8,  0x90,  0x3e, 
		},
	},
	{
		.in = "abcdefghijklmnopqrstuvwxyz",
		.in_len = 26,
		.out = {
			0xf1,  0xd7,  0x54,  0x66,  0x26,  0x36,  0xff,  0xe9, 
			0x2c,  0x82,  0xeb,  0xb9,  0x21,  0x2a,  0x48,  0x4a, 
			0x8d,  0x38,  0x63,  0x1e,  0xad,  0x42,  0x38,  0xf5, 
			0x44,  0x2e,  0xe1,  0x3b,  0x80,  0x54,  0xe4,  0x1b, 
			0x08,  0xbf,  0x2a,  0x92,  0x51,  0xc3,  0x0b,  0x6a, 
			0x0b,  0x8a,  0xae,  0x86,  0x17,  0x7a,  0xb4,  0xa6, 
			0xf6,  0x8f,  0x67,  0x3e,  0x72,  0x07,  0x86,  0x5d, 
			0x5d,  0x98,  0x19,  0xa3,  0xdb,  0xa4,  0xeb,  0x3b, 
		},
	},
	{
		.in = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
		.in_len = 62,
		.out = {
			0xdc,  0x37,  0xe0,  0x08,  0xcf,  0x9e,  0xe6,  0x9b, 
			0xf1,  0x1f,  0x00,  0xed,  0x9a,  0xba,  0x26,  0x90, 
			0x1d,  0xd7,  0xc2,  0x8c,  0xde,  0xc0,  0x66,  0xcc, 
			0x6a,  0xf4,  0x2e,  0x40,  0xf8,  0x2f,  0x3a,  0x1e, 
			0x08,  0xeb,  0xa2,  0x66,  0x29,  0x12,  0x9d,  0x8f, 
			0xb7,  0xcb,  0x57,  0x21,  0x1b,  0x92,  0x81,  0xa6, 
			0x55,  0x17,  0xcc,  0x87,  0x9d,  0x7b,  0x96,  0x21, 
			0x42,  0xc6,  0x5f,  0x5a,  0x7a,  0xf0,  0x14,  0x67, 
		},
	},
	{
		.in = "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
		.in_len = 80,
		.out = {
			0x46,  0x6e,  0xf1,  0x8b,  0xab,  0xb0,  0x15,  0x4d, 
			0x25,  0xb9,  0xd3,  0x8a,  0x64,  0x14,  0xf5,  0xc0, 
			0x87,  0x84,  0x37,  0x2b,  0xcc,  0xb2,  0x04,  0xd6, 
			0x54,  0x9c,  0x4a,  0xfa,  0xdb,  0x60,  0x14,  0x29, 
			0x4d,  0x5b,  0xd8,  0xdf,  0x2a,  0x6c,  0x44,  0xe5, 
			0x38,  0xcd,  0x04,  0x7b,  0x26,  0x81,  0xa5,  0x1a, 
			0x2c,  0x60,  0x48,  0x1e,  0x88,  0xc5,  0xa2,  0x0b, 
			0x2c,  0X2A,  0X80,  0XCF,  0X3A,  0X9A,  0X08,  0X3B,
		},
	}, 
	{
		.in = "abcdbcdecdefdefgefghfghighijhijk",
		.in_len = 32,
		.out = {
			0x2a,  0x98,  0x7e,  0xa4,  0x0f,  0x91,  0x70,  0x61, 
			0xf5,  0xd6,  0xf0,  0xa0,  0xe4,  0x64,  0x4f,  0x48, 
			0x8a,  0x7a,  0x5a,  0x52,  0xde,  0xee,  0x65,  0x62, 
			0x07,  0xc5,  0x62,  0xf9,  0x88,  0xe9,  0x5c,  0x69, 
			0x16,  0xbd,  0xc8,  0x03,  0x1b,  0xc5,  0xbe,  0x1b, 
			0x7b,  0x94,  0x76,  0x39,  0xfe,  0x05,  0x0b,  0x56, 
			0x93,  0x9b,  0xaa,  0xa0,  0xad,  0xff,  0x9a,  0xe6, 
			0x74,  0x5b,  0x7b,  0x18,  0x1c,  0x3b,  0xe3,  0xfd, 
		},
	},
};

#define N_WHIRLPOOL_TESTS (sizeof(whirlpool_tests) / sizeof(whirlpool_tests[0]))

static int
whirlpool_test(void)
{
	const struct whirlpool_test *wt;
	EVP_MD_CTX *md_ctx = NULL;
	const EVP_MD *md = EVP_whirlpool();
	uint8_t out[EVP_MAX_MD_SIZE];
	size_t i, l, in_len;
	int failed = 1;

	if ((md_ctx = EVP_MD_CTX_new()) == NULL) {
		fprintf(stderr, "FAIL: EVP_MD_CTX_new() failed\n");
		goto failed;		
	}

	for (i = 0; i < N_WHIRLPOOL_TESTS; i++) {
		wt = &whirlpool_tests[i];

		/* Digest */
		memset(out, 0, sizeof(out));
		WHIRLPOOL(wt->in, wt->in_len, out);
		if (memcmp(wt->out, out, WHIRLPOOL_DIGEST_LENGTH) != 0) {
			fprintf(stderr, "FAIL (%zu): digest mismatch\n", i);
			goto failed;
		}

		/* EVP single-shot digest */
		memset(out, 0, sizeof(out));
		if (!EVP_Digest(wt->in, wt->in_len, out, NULL, md, NULL)) {
			fprintf(stderr, "FAIL (%zu): EVP_Digest failed\n", i);
			goto failed;
		}

		if (memcmp(wt->out, out, WHIRLPOOL_DIGEST_LENGTH) != 0) {
			fprintf(stderr,
			    "FAIL (%zu): EVP single-shot mismatch\n", i);
			goto failed;
		}

		/* EVP digest */
		memset(out, 0, sizeof(out));
		if (!EVP_DigestInit_ex(md_ctx, md, NULL)) {
			fprintf(stderr,
			    "FAIL (%zu): EVP_DigestInit_ex failed\n", i);
			goto failed;
		}

		for (l = 0; l < wt->in_len;) {
			in_len = arc4random_uniform(wt->in_len / 2);
			if (in_len < 1)
				in_len = 1;
			if (in_len > wt->in_len - l)
				in_len = wt->in_len - l;

			if (!EVP_DigestUpdate(md_ctx, wt->in + l, in_len)) {
				fprintf(stderr,
				    "FAIL(%zu, %zu): EVP_DigestUpdate failed\n",
				    i, l);
				goto failed;
			}

			l += in_len;
		}

		if (!EVP_DigestFinal_ex(md_ctx, out, NULL)) {
			fprintf(stderr,
			    "FAIL (%zu): EVP_DigestFinal_ex failed\n",
			    i);
			goto failed;
		}

		if (memcmp(wt->out, out, WHIRLPOOL_DIGEST_LENGTH) != 0) {
			fprintf(stderr, "FAIL (%zu): EVP mismatch\n", i);
			goto failed;
		}
	}

	failed = 0;

 failed:
	EVP_MD_CTX_free(md_ctx);

	return failed;
}

int
main(int argc, char **argv)
{
	int failed = 0;

	failed |= whirlpool_test();

	return failed;
}
