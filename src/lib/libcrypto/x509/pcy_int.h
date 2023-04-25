/* $OpenBSD: pcy_int.h,v 1.4 2023/04/25 18:53:42 tb Exp $ */
/* Written by Dr Stephen N Henson (steve@openssl.org) for the OpenSSL
 * project 2004.
 */
/* ====================================================================
 * Copyright (c) 2004 The OpenSSL Project.  All rights reserved.
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

/* Needed to pull in the typedefs for X509_POLICY_* */
#include "x509_local.h"

__BEGIN_HIDDEN_DECLS

typedef struct X509_POLICY_DATA_st X509_POLICY_DATA;

DECLARE_STACK_OF(X509_POLICY_DATA)
DECLARE_STACK_OF(X509_POLICY_NODE)

#define sk_X509_POLICY_DATA_new(cmp) SKM_sk_new(X509_POLICY_DATA, (cmp))
#define sk_X509_POLICY_DATA_new_null() SKM_sk_new_null(X509_POLICY_DATA)
#define sk_X509_POLICY_DATA_free(st) SKM_sk_free(X509_POLICY_DATA, (st))
#define sk_X509_POLICY_DATA_num(st) SKM_sk_num(X509_POLICY_DATA, (st))
#define sk_X509_POLICY_DATA_value(st, i) SKM_sk_value(X509_POLICY_DATA, (st), (i))
#define sk_X509_POLICY_DATA_set(st, i, val) SKM_sk_set(X509_POLICY_DATA, (st), (i), (val))
#define sk_X509_POLICY_DATA_zero(st) SKM_sk_zero(X509_POLICY_DATA, (st))
#define sk_X509_POLICY_DATA_push(st, val) SKM_sk_push(X509_POLICY_DATA, (st), (val))
#define sk_X509_POLICY_DATA_unshift(st, val) SKM_sk_unshift(X509_POLICY_DATA, (st), (val))
#define sk_X509_POLICY_DATA_find(st, val) SKM_sk_find(X509_POLICY_DATA, (st), (val))
#define sk_X509_POLICY_DATA_find_ex(st, val) SKM_sk_find_ex(X509_POLICY_DATA, (st), (val))
#define sk_X509_POLICY_DATA_delete(st, i) SKM_sk_delete(X509_POLICY_DATA, (st), (i))
#define sk_X509_POLICY_DATA_delete_ptr(st, ptr) SKM_sk_delete_ptr(X509_POLICY_DATA, (st), (ptr))
#define sk_X509_POLICY_DATA_insert(st, val, i) SKM_sk_insert(X509_POLICY_DATA, (st), (val), (i))
#define sk_X509_POLICY_DATA_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(X509_POLICY_DATA, (st), (cmp))
#define sk_X509_POLICY_DATA_dup(st) SKM_sk_dup(X509_POLICY_DATA, st)
#define sk_X509_POLICY_DATA_pop_free(st, free_func) SKM_sk_pop_free(X509_POLICY_DATA, (st), (free_func))
#define sk_X509_POLICY_DATA_shift(st) SKM_sk_shift(X509_POLICY_DATA, (st))
#define sk_X509_POLICY_DATA_pop(st) SKM_sk_pop(X509_POLICY_DATA, (st))
#define sk_X509_POLICY_DATA_sort(st) SKM_sk_sort(X509_POLICY_DATA, (st))
#define sk_X509_POLICY_DATA_is_sorted(st) SKM_sk_is_sorted(X509_POLICY_DATA, (st))

#define sk_X509_POLICY_NODE_new(cmp) SKM_sk_new(X509_POLICY_NODE, (cmp))
#define sk_X509_POLICY_NODE_new_null() SKM_sk_new_null(X509_POLICY_NODE)
#define sk_X509_POLICY_NODE_free(st) SKM_sk_free(X509_POLICY_NODE, (st))
#define sk_X509_POLICY_NODE_num(st) SKM_sk_num(X509_POLICY_NODE, (st))
#define sk_X509_POLICY_NODE_value(st, i) SKM_sk_value(X509_POLICY_NODE, (st), (i))
#define sk_X509_POLICY_NODE_set(st, i, val) SKM_sk_set(X509_POLICY_NODE, (st), (i), (val))
#define sk_X509_POLICY_NODE_zero(st) SKM_sk_zero(X509_POLICY_NODE, (st))
#define sk_X509_POLICY_NODE_push(st, val) SKM_sk_push(X509_POLICY_NODE, (st), (val))
#define sk_X509_POLICY_NODE_unshift(st, val) SKM_sk_unshift(X509_POLICY_NODE, (st), (val))
#define sk_X509_POLICY_NODE_find(st, val) SKM_sk_find(X509_POLICY_NODE, (st), (val))
#define sk_X509_POLICY_NODE_find_ex(st, val) SKM_sk_find_ex(X509_POLICY_NODE, (st), (val))
#define sk_X509_POLICY_NODE_delete(st, i) SKM_sk_delete(X509_POLICY_NODE, (st), (i))
#define sk_X509_POLICY_NODE_delete_ptr(st, ptr) SKM_sk_delete_ptr(X509_POLICY_NODE, (st), (ptr))
#define sk_X509_POLICY_NODE_insert(st, val, i) SKM_sk_insert(X509_POLICY_NODE, (st), (val), (i))
#define sk_X509_POLICY_NODE_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(X509_POLICY_NODE, (st), (cmp))
#define sk_X509_POLICY_NODE_dup(st) SKM_sk_dup(X509_POLICY_NODE, st)
#define sk_X509_POLICY_NODE_pop_free(st, free_func) SKM_sk_pop_free(X509_POLICY_NODE, (st), (free_func))
#define sk_X509_POLICY_NODE_shift(st) SKM_sk_shift(X509_POLICY_NODE, (st))
#define sk_X509_POLICY_NODE_pop(st) SKM_sk_pop(X509_POLICY_NODE, (st))
#define sk_X509_POLICY_NODE_sort(st) SKM_sk_sort(X509_POLICY_NODE, (st))
#define sk_X509_POLICY_NODE_is_sorted(st) SKM_sk_is_sorted(X509_POLICY_NODE, (st))

/* This structure and the field names correspond to the Policy 'node' of
 * RFC3280. NB this structure contains no pointers to parent or child
 * data: X509_POLICY_NODE contains that. This means that the main policy data
 * can be kept static and cached with the certificate.
 */

struct X509_POLICY_DATA_st {
	unsigned int flags;
	/* Policy OID and qualifiers for this data */
	ASN1_OBJECT *valid_policy;
	STACK_OF(POLICYQUALINFO) *qualifier_set;
	STACK_OF(ASN1_OBJECT) *expected_policy_set;
};

/* X509_POLICY_DATA flags values */

/* This flag indicates the structure has been mapped using a policy mapping
 * extension. If policy mapping is not active its references get deleted.
 */

#define POLICY_DATA_FLAG_MAPPED			0x1

/* This flag indicates the data doesn't correspond to a policy in Certificate
 * Policies: it has been mapped to any policy.
 */

#define POLICY_DATA_FLAG_MAPPED_ANY		0x2

/* AND with flags to see if any mapping has occurred */

#define POLICY_DATA_FLAG_MAP_MASK		0x3

/* qualifiers are shared and shouldn't be freed */

#define POLICY_DATA_FLAG_SHARED_QUALIFIERS	0x4

/* Parent node is an extra node and should be freed */

#define POLICY_DATA_FLAG_EXTRA_NODE		0x8

/* Corresponding CertificatePolicies is critical */

#define POLICY_DATA_FLAG_CRITICAL		0x10

/* This structure is cached with a certificate */

struct X509_POLICY_CACHE_st {
	/* anyPolicy data or NULL if no anyPolicy */
	X509_POLICY_DATA *anyPolicy;
	/* other policy data */
	STACK_OF(X509_POLICY_DATA) *data;
	/* If InhibitAnyPolicy present this is its value or -1 if absent. */
	long any_skip;
	/* If policyConstraints and requireExplicitPolicy present this is its
	 * value or -1 if absent.
	 */
	long explicit_skip;
	/* If policyConstraints and policyMapping present this is its
	 * value or -1 if absent.
         */
	long map_skip;
};

/*#define POLICY_CACHE_FLAG_CRITICAL		POLICY_DATA_FLAG_CRITICAL*/

/* This structure represents the relationship between nodes */

struct X509_POLICY_NODE_st {
	/* node data this refers to */
	const X509_POLICY_DATA *data;
	/* Parent node */
	X509_POLICY_NODE *parent;
	/* Number of child nodes */
	int nchild;
};

struct X509_POLICY_LEVEL_st {
	/* Cert for this level */
	X509 *cert;
	/* nodes at this level */
	STACK_OF(X509_POLICY_NODE) *nodes;
	/* anyPolicy node */
	X509_POLICY_NODE *anyPolicy;
	/* Extra data */
	/*STACK_OF(X509_POLICY_DATA) *extra_data;*/
	unsigned int flags;
};

struct X509_POLICY_TREE_st {
	/* This is the tree 'level' data */
	X509_POLICY_LEVEL *levels;
	int nlevel;
	/* Extra policy data when additional nodes (not from the certificate)
	 * are required.
	 */
	STACK_OF(X509_POLICY_DATA) *extra_data;
	/* This is the authority constained policy set */
	STACK_OF(X509_POLICY_NODE) *auth_policies;
	STACK_OF(X509_POLICY_NODE) *user_policies;
	unsigned int flags;
};

/* Set if anyPolicy present in user policies */
#define POLICY_FLAG_ANY_POLICY		0x2

/* Useful macros */

#define node_data_critical(data) (data->flags & POLICY_DATA_FLAG_CRITICAL)
#define node_critical(node) node_data_critical(node->data)

/* Internal functions */

X509_POLICY_DATA *policy_data_new(POLICYINFO *policy, const ASN1_OBJECT *id,
    int crit);
void policy_data_free(X509_POLICY_DATA *data);

X509_POLICY_DATA *policy_cache_find_data(const X509_POLICY_CACHE *cache,
    const ASN1_OBJECT *id);
int policy_cache_set_mapping(X509 *x, POLICY_MAPPINGS *maps);


STACK_OF(X509_POLICY_NODE) *policy_node_cmp_new(void);

void policy_cache_init(void);

void policy_cache_free(X509_POLICY_CACHE *cache);

X509_POLICY_NODE *level_find_node(const X509_POLICY_LEVEL *level,
    const X509_POLICY_NODE *parent, const ASN1_OBJECT *id);

X509_POLICY_NODE *tree_find_sk(STACK_OF(X509_POLICY_NODE) *sk,
    const ASN1_OBJECT *id);

int level_add_node(X509_POLICY_LEVEL *level,
    const X509_POLICY_DATA *data, X509_POLICY_NODE *parent,
    X509_POLICY_TREE *tree, X509_POLICY_NODE **nodep);
void policy_node_free(X509_POLICY_NODE *node);
int policy_node_match(const X509_POLICY_LEVEL *lvl,
    const X509_POLICY_NODE *node, const ASN1_OBJECT *oid);

const X509_POLICY_CACHE *policy_cache_set(X509 *x);

int X509_policy_tree_level_count(const X509_POLICY_TREE *tree);
X509_POLICY_LEVEL *
	X509_policy_tree_get0_level(const X509_POLICY_TREE *tree, int i);

STACK_OF(X509_POLICY_NODE) *
	X509_policy_tree_get0_policies(const X509_POLICY_TREE *tree);

STACK_OF(X509_POLICY_NODE) *
	X509_policy_tree_get0_user_policies(const X509_POLICY_TREE *tree);

int X509_policy_level_node_count(X509_POLICY_LEVEL *level);

X509_POLICY_NODE *X509_policy_level_get0_node(X509_POLICY_LEVEL *level, int i);

const ASN1_OBJECT *X509_policy_node_get0_policy(const X509_POLICY_NODE *node);

STACK_OF(POLICYQUALINFO) *
	X509_policy_node_get0_qualifiers(const X509_POLICY_NODE *node);
const X509_POLICY_NODE *
	X509_policy_node_get0_parent(const X509_POLICY_NODE *node);


__END_HIDDEN_DECLS
