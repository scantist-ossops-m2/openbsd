/*	$OpenBSD: bn_unit.c,v 1.1 2022/11/22 08:56:33 tb Exp $ */

/*
 * Copyright (c) 2022 Theo Buehler <tb@openbsd.org>
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

#include <sys/resource.h>

#include <err.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/bn.h>

static int
test_bn_print_wrapper(char *a, size_t size, const char *descr,
    int (*to_bn)(BIGNUM **, const char *))
{
	int ret;

	ret = to_bn(NULL, a);
	if (ret != 0 && (ret < 0 || (size_t)ret != size - 1)) {
		fprintf(stderr, "unexpected %s() return"
		    "want 0 or %zu, got %d\n", descr, size - 1, ret);
		return 1;
	}

	return 0;
}

static int
test_bn_print_null_derefs(void)
{
	struct rlimit rlimit;
	size_t size = INT_MAX / 4 + 4;
	size_t datalimit = (size + 500 * 1024) / 1024;
	char *a;
	int failed = 0;

	if (getrlimit(RLIMIT_DATA, &rlimit) != 0)
		err(1, "getrlimit");

	if ((rlimit.rlim_cur + 1023) / 1024 < datalimit) {
		printf("%s: Insufficient data limit\n", __func__);
		printf("Need more than %zu kB\n", datalimit);
		printf("SKIPPED\n");
		return 0;
	}

	if ((a = malloc(size)) == NULL)
		err(1, "malloc(%zu) failed", size);

	memset(a, '0', size - 1);
	a[size - 1] = '\0';

	failed |= test_bn_print_wrapper(a, size, "BN_dec2bn", BN_dec2bn);
	failed |= test_bn_print_wrapper(a, size, "BN_hex2bn", BN_hex2bn);

	free(a);

	return failed;
}

int
main(void)
{
	int failed = 0;

	failed |= test_bn_print_null_derefs();

	if (!failed)
		printf("SUCCESS\n");

	return failed;
}
