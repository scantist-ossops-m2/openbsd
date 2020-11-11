/* $OpenBSD: ssl_locl.h,v 1.307 2020/11/11 18:14:12 jsing Exp $ */
/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 *
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 *
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 *
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
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
 * ECC cipher suite support in OpenSSL originally developed by
 * SUN MICROSYSTEMS, INC., and contributed to the OpenSSL project.
 */
/* ====================================================================
 * Copyright 2005 Nokia. All rights reserved.
 *
 * The portions of the attached software ("Contribution") is developed by
 * Nokia Corporation and is licensed pursuant to the OpenSSL open source
 * license.
 *
 * The Contribution, originally written by Mika Kousa and Pasi Eronen of
 * Nokia Corporation, consists of the "PSK" (Pre-Shared Key) ciphersuites
 * support (see RFC 4279) to OpenSSL.
 *
 * No patent licenses or other rights except those expressly stated in
 * the OpenSSL open source license shall be deemed granted or received
 * expressly, by implication, estoppel, or otherwise.
 *
 * No assurances are provided by Nokia that the Contribution does not
 * infringe the patent or other intellectual property rights of any third
 * party or that the license provides you with all the necessary rights
 * to make use of the Contribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. IN
 * ADDITION TO THE DISCLAIMERS INCLUDED IN THE LICENSE, NOKIA
 * SPECIFICALLY DISCLAIMS ANY LIABILITY FOR CLAIMS BROUGHT BY YOU OR ANY
 * OTHER ENTITY BASED ON INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS OR
 * OTHERWISE.
 */

#ifndef HEADER_SSL_LOCL_H
#define HEADER_SSL_LOCL_H

#include <sys/types.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <openssl/opensslconf.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/dsa.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/ssl.h>
#include <openssl/stack.h>

#include "bytestring.h"
#include "ssl_sigalgs.h"
#include "tls13_internal.h"

__BEGIN_HIDDEN_DECLS

#define CTASSERT(x)	extern char  _ctassert[(x) ? 1 : -1 ]   \
			    __attribute__((__unused__))

#ifndef LIBRESSL_HAS_TLS1_3_CLIENT
#define LIBRESSL_HAS_TLS1_3_CLIENT
#endif

#ifndef LIBRESSL_HAS_TLS1_3_SERVER
#define LIBRESSL_HAS_TLS1_3_SERVER
#endif

#if defined(LIBRESSL_HAS_TLS1_3_CLIENT) || defined(LIBRESSL_HAS_TLS1_3_SERVER)
#define LIBRESSL_HAS_TLS1_3
#endif

/* LOCAL STUFF */

#define SSL_DECRYPT	0
#define SSL_ENCRYPT	1

/*
 * Define the Bitmasks for SSL_CIPHER.algorithms.
 * This bits are used packed as dense as possible. If new methods/ciphers
 * etc will be added, the bits a likely to change, so this information
 * is for internal library use only, even though SSL_CIPHER.algorithms
 * can be publicly accessed.
 * Use the according functions for cipher management instead.
 *
 * The bit mask handling in the selection and sorting scheme in
 * ssl_create_cipher_list() has only limited capabilities, reflecting
 * that the different entities within are mutually exclusive:
 * ONLY ONE BIT PER MASK CAN BE SET AT A TIME.
 */

/* Bits for algorithm_mkey (key exchange algorithm) */
#define SSL_kRSA		0x00000001L /* RSA key exchange */
#define SSL_kDHE		0x00000008L /* tmp DH key no DH cert */
#define SSL_kECDHE		0x00000080L /* ephemeral ECDH */
#define SSL_kGOST		0x00000200L /* GOST key exchange */
#define SSL_kTLS1_3		0x00000400L /* TLSv1.3 key exchange */

/* Bits for algorithm_auth (server authentication) */
#define SSL_aRSA		0x00000001L /* RSA auth */
#define SSL_aDSS 		0x00000002L /* DSS auth */
#define SSL_aNULL 		0x00000004L /* no auth (i.e. use ADH or AECDH) */
#define SSL_aECDSA              0x00000040L /* ECDSA auth*/
#define SSL_aGOST01 		0x00000200L /* GOST R 34.10-2001 signature auth */
#define SSL_aTLS1_3		0x00000400L /* TLSv1.3 authentication */

/* Bits for algorithm_enc (symmetric encryption) */
#define SSL_DES			0x00000001L
#define SSL_3DES		0x00000002L
#define SSL_RC4			0x00000004L
#define SSL_IDEA		0x00000008L
#define SSL_eNULL		0x00000010L
#define SSL_AES128		0x00000020L
#define SSL_AES256		0x00000040L
#define SSL_CAMELLIA128		0x00000080L
#define SSL_CAMELLIA256		0x00000100L
#define SSL_eGOST2814789CNT	0x00000200L
#define SSL_AES128GCM		0x00000400L
#define SSL_AES256GCM		0x00000800L
#define SSL_CHACHA20POLY1305	0x00001000L

#define SSL_AES        		(SSL_AES128|SSL_AES256|SSL_AES128GCM|SSL_AES256GCM)
#define SSL_CAMELLIA		(SSL_CAMELLIA128|SSL_CAMELLIA256)


/* Bits for algorithm_mac (symmetric authentication) */

#define SSL_MD5			0x00000001L
#define SSL_SHA1		0x00000002L
#define SSL_GOST94      0x00000004L
#define SSL_GOST89MAC   0x00000008L
#define SSL_SHA256		0x00000010L
#define SSL_SHA384		0x00000020L
/* Not a real MAC, just an indication it is part of cipher */
#define SSL_AEAD		0x00000040L
#define SSL_STREEBOG256		0x00000080L

/* Bits for algorithm_ssl (protocol version) */
#define SSL_SSLV3		0x00000002L
#define SSL_TLSV1		SSL_SSLV3	/* for now */
#define SSL_TLSV1_2		0x00000004L
#define SSL_TLSV1_3		0x00000008L


/* Bits for algorithm2 (handshake digests and other extra flags) */

#define SSL_HANDSHAKE_MAC_MASK		0xff0
#define SSL_HANDSHAKE_MAC_MD5		0x010
#define SSL_HANDSHAKE_MAC_SHA		0x020
#define SSL_HANDSHAKE_MAC_GOST94	0x040
#define SSL_HANDSHAKE_MAC_SHA256	0x080
#define SSL_HANDSHAKE_MAC_SHA384	0x100
#define SSL_HANDSHAKE_MAC_STREEBOG256	0x200
#define SSL_HANDSHAKE_MAC_DEFAULT (SSL_HANDSHAKE_MAC_MD5 | SSL_HANDSHAKE_MAC_SHA)

#define SSL3_CK_ID		0x03000000
#define SSL3_CK_VALUE_MASK	0x0000ffff

#define TLS1_PRF_DGST_MASK	(0xff << TLS1_PRF_DGST_SHIFT)

#define TLS1_PRF_DGST_SHIFT 10
#define TLS1_PRF_MD5 (SSL_HANDSHAKE_MAC_MD5 << TLS1_PRF_DGST_SHIFT)
#define TLS1_PRF_SHA1 (SSL_HANDSHAKE_MAC_SHA << TLS1_PRF_DGST_SHIFT)
#define TLS1_PRF_SHA256 (SSL_HANDSHAKE_MAC_SHA256 << TLS1_PRF_DGST_SHIFT)
#define TLS1_PRF_SHA384 (SSL_HANDSHAKE_MAC_SHA384 << TLS1_PRF_DGST_SHIFT)
#define TLS1_PRF_GOST94 (SSL_HANDSHAKE_MAC_GOST94 << TLS1_PRF_DGST_SHIFT)
#define TLS1_PRF_STREEBOG256 (SSL_HANDSHAKE_MAC_STREEBOG256 << TLS1_PRF_DGST_SHIFT)
#define TLS1_PRF (TLS1_PRF_MD5 | TLS1_PRF_SHA1)

/*
 * Stream MAC for GOST ciphersuites from cryptopro draft
 * (currently this also goes into algorithm2).
 */
#define TLS1_STREAM_MAC 0x04

/*
 * SSL_CIPHER_ALGORITHM2_VARIABLE_NONCE_IN_RECORD is an algorithm2 flag that
 * indicates that the variable part of the nonce is included as a prefix of
 * the record (AES-GCM, for example, does this with an 8-byte variable nonce.)
 */
#define SSL_CIPHER_ALGORITHM2_VARIABLE_NONCE_IN_RECORD (1 << 22)

/*
 * SSL_CIPHER_AEAD_FIXED_NONCE_LEN returns the number of bytes of fixed nonce
 * for an SSL_CIPHER with an algorithm_mac of SSL_AEAD.
 */
#define SSL_CIPHER_AEAD_FIXED_NONCE_LEN(ssl_cipher) \
	(((ssl_cipher->algorithm2 >> 24) & 0xf) * 2)

/*
 * Cipher strength information.
 */
#define SSL_STRONG_MASK		0x000001fcL
#define SSL_STRONG_NONE		0x00000004L
#define SSL_LOW			0x00000020L
#define SSL_MEDIUM		0x00000040L
#define SSL_HIGH		0x00000080L

/*
 * The keylength (measured in RSA key bits, I guess)  for temporary keys.
 * Cipher argument is so that this can be variable in the future.
 */
#define SSL_C_PKEYLENGTH(c)	1024

/* See if we use signature algorithms extension. */
#define SSL_USE_SIGALGS(s) \
	(s->method->internal->enc_flags & SSL_ENC_FLAG_SIGALGS)

/* See if we use SHA256 default PRF. */
#define SSL_USE_SHA256_PRF(s) \
	(s->method->internal->enc_flags & SSL_ENC_FLAG_SHA256_PRF)

/* Allow TLS 1.2 ciphersuites: applies to DTLS 1.2 as well as TLS 1.2. */
#define SSL_USE_TLS1_2_CIPHERS(s) \
	(s->method->internal->enc_flags & SSL_ENC_FLAG_TLS1_2_CIPHERS)

/* Allow TLS 1.3 ciphersuites only. */
#define SSL_USE_TLS1_3_CIPHERS(s) \
	(s->method->internal->enc_flags & SSL_ENC_FLAG_TLS1_3_CIPHERS)

#define SSL_PKEY_RSA		0
#define SSL_PKEY_ECC		1
#define SSL_PKEY_GOST01		2
#define SSL_PKEY_NUM		3

#define SSL_MAX_EMPTY_RECORDS	32

/* SSL_kRSA <- RSA_ENC | (RSA_TMP & RSA_SIGN) |
 * 	    <- (EXPORT & (RSA_ENC | RSA_TMP) & RSA_SIGN)
 * SSL_kDH  <- DH_ENC & (RSA_ENC | RSA_SIGN | DSA_SIGN)
 * SSL_kDHE <- RSA_ENC | RSA_SIGN | DSA_SIGN
 * SSL_aRSA <- RSA_ENC | RSA_SIGN
 * SSL_aDSS <- DSA_SIGN
 */

/*
#define CERT_INVALID		0
#define CERT_PUBLIC_KEY		1
#define CERT_PRIVATE_KEY	2
*/

/* From ECC-TLS draft, used in encoding the curve type in
 * ECParameters
 */
#define EXPLICIT_PRIME_CURVE_TYPE  1
#define EXPLICIT_CHAR2_CURVE_TYPE  2
#define NAMED_CURVE_TYPE           3

typedef struct ssl_method_internal_st {
	int dtls;
	int version;

	uint16_t min_version;
	uint16_t max_version;

	int (*ssl_new)(SSL *s);
	void (*ssl_clear)(SSL *s);
	void (*ssl_free)(SSL *s);

	int (*ssl_accept)(SSL *s);
	int (*ssl_connect)(SSL *s);
	int (*ssl_shutdown)(SSL *s);

	int (*ssl_renegotiate)(SSL *s);
	int (*ssl_renegotiate_check)(SSL *s);

	int (*ssl_pending)(const SSL *s);
	int (*ssl_read_bytes)(SSL *s, int type, unsigned char *buf, int len,
	    int peek);
	int (*ssl_write_bytes)(SSL *s, int type, const void *buf_, int len);

	unsigned int enc_flags;		/* SSL_ENC_FLAG_* */
} SSL_METHOD_INTERNAL;

typedef struct ssl_session_internal_st {
	CRYPTO_EX_DATA ex_data; /* application specific data */

	/* These are used to make removal of session-ids more
	 * efficient and to implement a maximum cache size. */
	struct ssl_session_st *prev, *next;

	/* Used to indicate that session resumption is not allowed.
	 * Applications can also set this bit for a new session via
	 * not_resumable_session_cb to disable session caching and tickets. */
	int not_resumable;

	/* The cert is the certificate used to establish this connection */
	struct sess_cert_st /* SESS_CERT */ *sess_cert;

	size_t tlsext_ecpointformatlist_length;
	uint8_t *tlsext_ecpointformatlist; /* peer's list */
	size_t tlsext_supportedgroups_length;
	uint16_t *tlsext_supportedgroups; /* peer's list */
} SSL_SESSION_INTERNAL;
#define SSI(s) (s->session->internal)

typedef struct ssl_handshake_st {
	/* state contains one of the SSL3_ST_* values. */
	int state;

	/* used when SSL_ST_FLUSH_DATA is entered */
	int next_state;

	/*  new_cipher is the cipher being negotiated in this handshake. */
	const SSL_CIPHER *new_cipher;

	/* key_block is the record-layer key block for TLS 1.2 and earlier. */
	size_t key_block_len;
	unsigned char *key_block;

	/* Extensions seen in this handshake. */
	uint32_t extensions_seen;

	/* sigalgs offered in this handshake in wire form */
	size_t sigalgs_len;
	uint8_t *sigalgs;
} SSL_HANDSHAKE;

typedef struct cert_pkey_st {
	X509 *x509;
	EVP_PKEY *privatekey;
	STACK_OF(X509) *chain;
} CERT_PKEY;

typedef struct ssl_handshake_tls13_st {
	uint16_t min_version;
	uint16_t max_version;
	uint16_t version;

	int use_legacy;
	int hrr;

	/* Certificate and sigalg selected for use (static pointers). */
	const CERT_PKEY *cpk;
	const struct ssl_sigalg *sigalg;

	/* Version proposed by peer server. */
	uint16_t server_version;

	uint16_t server_group;
	struct tls13_key_share *key_share;
	struct tls13_secrets *secrets;

	uint8_t *cookie;
	size_t cookie_len;

	/* Preserved transcript hash. */
	uint8_t transcript_hash[EVP_MAX_MD_SIZE];
	size_t transcript_hash_len;

	/* Legacy session ID. */
	uint8_t legacy_session_id[SSL_MAX_SSL_SESSION_ID_LENGTH];
	size_t legacy_session_id_len;

	/* ClientHello hash, used to validate following HelloRetryRequest */
	EVP_MD_CTX *clienthello_md_ctx;
	unsigned char *clienthello_hash;
	unsigned int clienthello_hash_len;

} SSL_HANDSHAKE_TLS13;

struct tls12_record_layer;

struct tls12_record_layer *tls12_record_layer_new(void);
void tls12_record_layer_free(struct tls12_record_layer *rl);
void tls12_record_layer_alert(struct tls12_record_layer *rl,
    uint8_t *alert_desc);
void tls12_record_layer_set_version(struct tls12_record_layer *rl,
    uint16_t version);
void tls12_record_layer_set_read_epoch(struct tls12_record_layer *rl,
    uint16_t epoch);
void tls12_record_layer_set_write_epoch(struct tls12_record_layer *rl,
    uint16_t epoch);
void tls12_record_layer_clear_read_state(struct tls12_record_layer *rl);
void tls12_record_layer_clear_write_state(struct tls12_record_layer *rl);
void tls12_record_layer_set_read_seq_num(struct tls12_record_layer *rl,
    uint8_t *seq_num);
void tls12_record_layer_set_write_seq_num(struct tls12_record_layer *rl,
    uint8_t *seq_num);
int tls12_record_layer_set_read_aead(struct tls12_record_layer *rl,
    SSL_AEAD_CTX *aead_ctx);
int tls12_record_layer_set_write_aead(struct tls12_record_layer *rl,
    SSL_AEAD_CTX *aead_ctx);
int tls12_record_layer_set_read_cipher_hash(struct tls12_record_layer *rl,
    EVP_CIPHER_CTX *cipher_ctx, EVP_MD_CTX *hash_ctx, int stream_mac);
int tls12_record_layer_set_write_cipher_hash(struct tls12_record_layer *rl,
    EVP_CIPHER_CTX *cipher_ctx, EVP_MD_CTX *hash_ctx, int stream_mac);
int tls12_record_layer_set_read_mac_key(struct tls12_record_layer *rl,
    const uint8_t *mac_key, size_t mac_key_len);
int tls12_record_layer_open_record(struct tls12_record_layer *rl,
    uint8_t *buf, size_t buf_len, uint8_t **out, size_t *out_len);
int tls12_record_layer_seal_record(struct tls12_record_layer *rl,
    uint8_t content_type, const uint8_t *content, size_t content_len,
    CBB *out);

typedef struct ssl_ctx_internal_st {
	uint16_t min_version;
	uint16_t max_version;

	unsigned long options;
	unsigned long mode;

	/* If this callback is not null, it will be called each
	 * time a session id is added to the cache.  If this function
	 * returns 1, it means that the callback will do a
	 * SSL_SESSION_free() when it has finished using it.  Otherwise,
	 * on 0, it means the callback has finished with it.
	 * If remove_session_cb is not null, it will be called when
	 * a session-id is removed from the cache.  After the call,
	 * OpenSSL will SSL_SESSION_free() it. */
	int (*new_session_cb)(struct ssl_st *ssl, SSL_SESSION *sess);
	void (*remove_session_cb)(struct ssl_ctx_st *ctx, SSL_SESSION *sess);
	SSL_SESSION *(*get_session_cb)(struct ssl_st *ssl,
	    const unsigned char *data, int len, int *copy);

	/* if defined, these override the X509_verify_cert() calls */
	int (*app_verify_callback)(X509_STORE_CTX *, void *);
	    void *app_verify_arg;

	/* get client cert callback */
	int (*client_cert_cb)(SSL *ssl, X509 **x509, EVP_PKEY **pkey);

	/* cookie generate callback */
	int (*app_gen_cookie_cb)(SSL *ssl, unsigned char *cookie,
	    unsigned int *cookie_len);

	/* verify cookie callback */
	int (*app_verify_cookie_cb)(SSL *ssl, const unsigned char *cookie,
	    unsigned int cookie_len);

	void (*info_callback)(const SSL *ssl,int type,int val); /* used if SSL's info_callback is NULL */

	/* callback that allows applications to peek at protocol messages */
	void (*msg_callback)(int write_p, int version, int content_type,
	    const void *buf, size_t len, SSL *ssl, void *arg);
	void *msg_callback_arg;

	int (*default_verify_callback)(int ok,X509_STORE_CTX *ctx); /* called 'verify_callback' in the SSL */

	/* Default generate session ID callback. */
	GEN_SESSION_CB generate_session_id;

	/* TLS extensions servername callback */
	int (*tlsext_servername_callback)(SSL*, int *, void *);
	void *tlsext_servername_arg;

	/* Callback to support customisation of ticket key setting */
	int (*tlsext_ticket_key_cb)(SSL *ssl, unsigned char *name,
	    unsigned char *iv, EVP_CIPHER_CTX *ectx, HMAC_CTX *hctx, int enc);

	/* certificate status request info */
	/* Callback for status request */
	int (*tlsext_status_cb)(SSL *ssl, void *arg);
	void *tlsext_status_arg;

	struct lhash_st_SSL_SESSION *sessions;

	/* Most session-ids that will be cached, default is
	 * SSL_SESSION_CACHE_MAX_SIZE_DEFAULT. 0 is unlimited. */
	unsigned long session_cache_size;
	struct ssl_session_st *session_cache_head;
	struct ssl_session_st *session_cache_tail;

	/* This can have one of 2 values, ored together,
	 * SSL_SESS_CACHE_CLIENT,
	 * SSL_SESS_CACHE_SERVER,
	 * Default is SSL_SESSION_CACHE_SERVER, which means only
	 * SSL_accept which cache SSL_SESSIONS. */
	int session_cache_mode;

	struct {
		int sess_connect;	/* SSL new conn - started */
		int sess_connect_renegotiate;/* SSL reneg - requested */
		int sess_connect_good;	/* SSL new conne/reneg - finished */
		int sess_accept;	/* SSL new accept - started */
		int sess_accept_renegotiate;/* SSL reneg - requested */
		int sess_accept_good;	/* SSL accept/reneg - finished */
		int sess_miss;		/* session lookup misses  */
		int sess_timeout;	/* reuse attempt on timeouted session */
		int sess_cache_full;	/* session removed due to full cache */
		int sess_hit;		/* session reuse actually done */
		int sess_cb_hit;	/* session-id that was not
					 * in the cache was
					 * passed back via the callback.  This
					 * indicates that the application is
					 * supplying session-id's from other
					 * processes - spooky :-) */
	} stats;

	CRYPTO_EX_DATA ex_data;

	STACK_OF(SSL_CIPHER) *cipher_list_tls13;

	struct cert_st /* CERT */ *cert;

	/* Default values used when no per-SSL value is defined follow */

	/* what we put in client cert requests */
	STACK_OF(X509_NAME) *client_CA;

	long max_cert_list;

	int read_ahead;

	int quiet_shutdown;

	/* Maximum amount of data to send in one fragment.
	 * actual record size can be more than this due to
	 * padding and MAC overheads.
	 */
	unsigned int max_send_fragment;

#ifndef OPENSSL_NO_ENGINE
	/* Engine to pass requests for client certs to
	 */
	ENGINE *client_cert_engine;
#endif

	/* RFC 4507 session ticket keys */
	unsigned char tlsext_tick_key_name[16];
	unsigned char tlsext_tick_hmac_key[16];
	unsigned char tlsext_tick_aes_key[16];

	/* SRTP profiles we are willing to do from RFC 5764 */
	STACK_OF(SRTP_PROTECTION_PROFILE) *srtp_profiles;

	/*
	 * ALPN information.
	 */

	/*
	 * Server callback function that allows the server to select the
	 * protocol for the connection.
	 *   out: on successful return, this must point to the raw protocol
	 *       name (without the length prefix).
	 *   outlen: on successful return, this contains the length of out.
	 *   in: points to the client's list of supported protocols in
	 *       wire-format.
	 *   inlen: the length of in.
	 */
	int (*alpn_select_cb)(SSL *s, const unsigned char **out,
	    unsigned char *outlen, const unsigned char *in, unsigned int inlen,
	    void *arg);
	void *alpn_select_cb_arg;

	/* Client list of supported protocols in wire format. */
	unsigned char *alpn_client_proto_list;
	unsigned int alpn_client_proto_list_len;

	size_t tlsext_ecpointformatlist_length;
	uint8_t *tlsext_ecpointformatlist; /* our list */
	size_t tlsext_supportedgroups_length;
	uint16_t *tlsext_supportedgroups; /* our list */
} SSL_CTX_INTERNAL;

typedef struct ssl_internal_st {
	struct tls13_ctx *tls13;

	uint16_t min_version;
	uint16_t max_version;

	unsigned long options; /* protocol behaviour */
	unsigned long mode; /* API behaviour */

	/* Client list of supported protocols in wire format. */
	unsigned char *alpn_client_proto_list;
	unsigned int alpn_client_proto_list_len;

	/* XXX Callbacks */

	/* true when we are actually in SSL_accept() or SSL_connect() */
	int in_handshake;
	int (*handshake_func)(SSL *);
	/* callback that allows applications to peek at protocol messages */
	void (*msg_callback)(int write_p, int version, int content_type,
	    const void *buf, size_t len, SSL *ssl, void *arg);
	void *msg_callback_arg;

	/* Default generate session ID callback. */
	GEN_SESSION_CB generate_session_id;

	int (*verify_callback)(int ok,X509_STORE_CTX *ctx); /* fail if callback returns 0 */

	void (*info_callback)(const SSL *ssl,int type,int val); /* optional informational callback */

	/* TLS extension debug callback */
	void (*tlsext_debug_cb)(SSL *s, int client_server, int type,
	    unsigned char *data, int len, void *arg);
	void *tlsext_debug_arg;

	/* TLS Session Ticket extension callback */
	tls_session_ticket_ext_cb_fn tls_session_ticket_ext_cb;
	void *tls_session_ticket_ext_cb_arg;

	/* TLS pre-shared secret session resumption */
	tls_session_secret_cb_fn tls_session_secret_cb;
	void *tls_session_secret_cb_arg;

	/* XXX non-callback */

	int type; /* SSL_ST_CONNECT or SSL_ST_ACCEPT */

	/* This holds a variable that indicates what we were doing
	 * when a 0 or -1 is returned.  This is needed for
	 * non-blocking IO so we know what request needs re-doing when
	 * in SSL_accept or SSL_connect */
	int rwstate;

	/* Imagine that here's a boolean member "init" that is
	 * switched as soon as SSL_set_{accept/connect}_state
	 * is called for the first time, so that "state" and
	 * "handshake_func" are properly initialized.  But as
	 * handshake_func is == 0 until then, we use this
	 * test instead of an "init" member.
	 */

	int new_session;/* Generate a new session or reuse an old one.
			 * NB: For servers, the 'new' session may actually be a previously
			 * cached session or even the previous session unless
			 * SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION is set */
	int quiet_shutdown;/* don't send shutdown packets */
	int shutdown;	/* we have shut things down, 0x01 sent, 0x02
			 * for received */
	BUF_MEM *init_buf;	/* buffer used during init */
	void *init_msg;		/* pointer to handshake message body, set by ssl3_get_message() */
	int init_num;		/* amount read/written */
	int init_off;		/* amount read/written */

	/* used internally to point at a raw packet */
	unsigned char *packet;
	unsigned int packet_length;

	int read_ahead;		/* Read as many input bytes as possible
				 * (for non-blocking reads) */

	int hit;		/* reusing a previous session */

	STACK_OF(SSL_CIPHER) *cipher_list_tls13;

	SSL_AEAD_CTX *aead_read_ctx;	/* AEAD context. If non-NULL, then
					   enc_read_ctx and read_hash are
					   ignored. */

	SSL_AEAD_CTX *aead_write_ctx;	/* AEAD context. If non-NULL, then
					   enc_write_ctx and write_hash are
					   ignored. */

	EVP_CIPHER_CTX *enc_write_ctx;		/* cryptographic state */
	EVP_MD_CTX *write_hash;			/* used for mac generation */

	struct tls12_record_layer *rl;

	/* session info */

	/* extra application data */
	CRYPTO_EX_DATA ex_data;

	/* client cert? */
	/* for server side, keep the list of CA_dn we can use */
	STACK_OF(X509_NAME) *client_CA;

	/* set this flag to 1 and a sleep(1) is put into all SSL_read()
	 * and SSL_write() calls, good for nbio debuging :-) */
	int debug;
	long max_cert_list;
	int first_packet;

	/* Expect OCSP CertificateStatus message */
	int tlsext_status_expected;
	/* OCSP status request only */
	STACK_OF(OCSP_RESPID) *tlsext_ocsp_ids;
	X509_EXTENSIONS *tlsext_ocsp_exts;

	/* OCSP response received or to be sent */
	unsigned char *tlsext_ocsp_resp;
	size_t tlsext_ocsp_resp_len;

	/* RFC4507 session ticket expected to be received or sent */
	int tlsext_ticket_expected;

	size_t tlsext_ecpointformatlist_length;
	uint8_t *tlsext_ecpointformatlist; /* our list */
	size_t tlsext_supportedgroups_length;
	uint16_t *tlsext_supportedgroups; /* our list */

	/* TLS Session Ticket extension override */
	TLS_SESSION_TICKET_EXT *tlsext_session_ticket;

	STACK_OF(SRTP_PROTECTION_PROFILE) *srtp_profiles;	/* What we'll do */
	const SRTP_PROTECTION_PROFILE *srtp_profile;		/* What's been chosen */

	int renegotiate;/* 1 if we are renegotiating.
		 	 * 2 if we are a server and are inside a handshake
	                 * (i.e. not just sending a HelloRequest) */

	int rstate;	/* where we are when reading */

	int mac_packet;

	int empty_record_count;
} SSL_INTERNAL;

typedef struct ssl3_record_internal_st {
	int type;               /* type of record */
	unsigned int length;    /* How many bytes available */
	unsigned int padding_length; /* Number of padding bytes. */
	unsigned int off;       /* read/write offset into 'buf' */
	unsigned char *data;    /* pointer to the record data */
	unsigned char *input;   /* where the decode bytes are */
	unsigned long epoch;    /* epoch number, needed by DTLS1 */
	unsigned char seq_num[8]; /* sequence number, needed by DTLS1 */
} SSL3_RECORD_INTERNAL;

typedef struct ssl3_buffer_internal_st {
	unsigned char *buf;	/* at least SSL3_RT_MAX_PACKET_SIZE bytes,
	                         * see ssl3_setup_buffers() */
	size_t len;		/* buffer size */
	int offset;		/* where to 'copy from' */
	int left;		/* how many bytes left */
} SSL3_BUFFER_INTERNAL;

typedef struct ssl3_state_internal_st {
	unsigned char read_sequence[SSL3_SEQUENCE_SIZE];
	int read_mac_secret_size;
	unsigned char read_mac_secret[EVP_MAX_MD_SIZE];
	unsigned char write_sequence[SSL3_SEQUENCE_SIZE];

	SSL3_BUFFER_INTERNAL rbuf;	/* read IO goes into here */
	SSL3_BUFFER_INTERNAL wbuf;	/* write IO goes into here */

	/* we allow one fatal and one warning alert to be outstanding,
	 * send close alert via the warning alert */
	int alert_dispatch;
	unsigned char send_alert[2];

	/* flags for countermeasure against known-IV weakness */
	int need_empty_fragments;
	int empty_fragment_done;

	SSL3_RECORD_INTERNAL rrec;	/* each decoded record goes in here */

	/* storage for Alert/Handshake protocol data received but not
	 * yet processed by ssl3_read_bytes: */
	unsigned char alert_fragment[2];
	unsigned int alert_fragment_len;
	unsigned char handshake_fragment[4];
	unsigned int handshake_fragment_len;

	/* partial write - check the numbers match */
	unsigned int wnum;	/* number of bytes sent so far */
	int wpend_tot;		/* number bytes written */
	int wpend_type;
	int wpend_ret;		/* number of bytes submitted */
	const unsigned char *wpend_buf;

	/* Transcript of handshake messages that have been sent and received. */
	BUF_MEM *handshake_transcript;

	/* Rolling hash of handshake messages. */
	EVP_MD_CTX *handshake_hash;

	/* this is set whenerver we see a change_cipher_spec message
	 * come in when we are not looking for one */
	int change_cipher_spec;

	int warn_alert;
	int fatal_alert;

	/* This flag is set when we should renegotiate ASAP, basically when
	 * there is no more data in the read or write buffers */
	int renegotiate;
	int total_renegotiations;
	int num_renegotiations;

	int in_read_app_data;

	SSL_HANDSHAKE hs;
	SSL_HANDSHAKE_TLS13 hs_tls13;

	struct	{
		int new_mac_secret_size;

		/* actually only needs to be 16+20 */
		unsigned char cert_verify_md[EVP_MAX_MD_SIZE*2];

		/* actually only need to be 16+20 for SSLv3 and 12 for TLS */
		unsigned char finish_md[EVP_MAX_MD_SIZE*2];
		int finish_md_len;
		unsigned char peer_finish_md[EVP_MAX_MD_SIZE*2];
		int peer_finish_md_len;

		unsigned long message_size;
		int message_type;

		DH *dh;

		EC_KEY *ecdh; /* holds short lived ECDH key */
		int ecdh_nid;

		uint8_t *x25519;

		int reuse_message;

		/* used for certificate requests */
		int cert_req;
		int ctype_num;
		char ctype[SSL3_CT_NUMBER];
		STACK_OF(X509_NAME) *ca_names;

		const EVP_CIPHER *new_sym_enc;
		const EVP_AEAD *new_aead;
		const EVP_MD *new_hash;
		int new_mac_pkey_type;
		int cert_request;
	} tmp;

	/* Connection binding to prevent renegotiation attacks */
	unsigned char previous_client_finished[EVP_MAX_MD_SIZE];
	unsigned char previous_client_finished_len;
	unsigned char previous_server_finished[EVP_MAX_MD_SIZE];
	unsigned char previous_server_finished_len;
	int send_connection_binding; /* TODOEKR */

	/* Set if we saw a Renegotiation Indication extension from our peer. */
	int renegotiate_seen;

	/*
	 * ALPN information.
	 *
	 * In a server these point to the selected ALPN protocol after the
	 * ClientHello has been processed. In a client these contain the
	 * protocol that the server selected once the ServerHello has been
	 * processed.
	 */
	unsigned char *alpn_selected;
	size_t alpn_selected_len;
} SSL3_STATE_INTERNAL;
#define S3I(s) (s->s3->internal)

typedef struct dtls1_record_data_internal_st {
	unsigned char *packet;
	unsigned int packet_length;
	SSL3_BUFFER_INTERNAL rbuf;
	SSL3_RECORD_INTERNAL rrec;
} DTLS1_RECORD_DATA_INTERNAL;

typedef struct dtls1_state_internal_st {
	unsigned int send_cookie;
	unsigned char cookie[DTLS1_COOKIE_LENGTH];
	unsigned char rcvd_cookie[DTLS1_COOKIE_LENGTH];
	unsigned int cookie_len;

	/*
	 * The current data and handshake epoch.  This is initially
	 * undefined, and starts at zero once the initial handshake is
	 * completed
	 */
	unsigned short r_epoch;
	unsigned short w_epoch;

	/* records being received in the current epoch */
	DTLS1_BITMAP bitmap;

	/* renegotiation starts a new set of sequence numbers */
	DTLS1_BITMAP next_bitmap;

	/* handshake message numbers */
	unsigned short handshake_write_seq;
	unsigned short next_handshake_write_seq;

	unsigned short handshake_read_seq;

	/* save last sequence number for retransmissions */
	unsigned char last_write_sequence[SSL3_SEQUENCE_SIZE];

	/* Received handshake records (processed and unprocessed) */
	record_pqueue unprocessed_rcds;
	record_pqueue processed_rcds;

	/* Buffered handshake messages */
	struct _pqueue *buffered_messages;

	/* Buffered application records.
	 * Only for records between CCS and Finished
	 * to prevent either protocol violation or
	 * unnecessary message loss.
	 */
	record_pqueue buffered_app_data;

	/* Is set when listening for new connections with dtls1_listen() */
	unsigned int listen;

	unsigned int mtu; /* max DTLS packet size */

	struct hm_header_st w_msg_hdr;
	struct hm_header_st r_msg_hdr;

	struct dtls1_timeout_st timeout;

	/* storage for Alert/Handshake protocol data received but not
	 * yet processed by ssl3_read_bytes: */
	unsigned char alert_fragment[DTLS1_AL_HEADER_LENGTH];
	unsigned int alert_fragment_len;
	unsigned char handshake_fragment[DTLS1_HM_HEADER_LENGTH];
	unsigned int handshake_fragment_len;

	unsigned int retransmitting;
	unsigned int change_cipher_spec_ok;
} DTLS1_STATE_INTERNAL;
#define D1I(s) (s->d1->internal)

typedef struct cert_st {
	/* Current active set */
	CERT_PKEY *key; /* ALWAYS points to an element of the pkeys array
			 * Probably it would make more sense to store
			 * an index, not a pointer. */

	/* The following masks are for the key and auth
	 * algorithms that are supported by the certs below */
	int valid;
	unsigned long mask_k;
	unsigned long mask_a;

	DH *dh_tmp;
	DH *(*dh_tmp_cb)(SSL *ssl, int is_export, int keysize);
	int dh_tmp_auto;

	CERT_PKEY pkeys[SSL_PKEY_NUM];

	int references; /* >1 only if SSL_copy_session_id is used */
} CERT;


typedef struct sess_cert_st {
	STACK_OF(X509) *cert_chain; /* as received from peer */

	/* The 'peer_...' members are used only by clients. */
	int peer_cert_type;

	CERT_PKEY *peer_key; /* points to an element of peer_pkeys (never NULL!) */
	CERT_PKEY peer_pkeys[SSL_PKEY_NUM];
	/* Obviously we don't have the private keys of these,
	 * so maybe we shouldn't even use the CERT_PKEY type here. */

	int peer_nid;
	DH *peer_dh_tmp;
	EC_KEY *peer_ecdh_tmp;
	uint8_t *peer_x25519_tmp;

	int references; /* actually always 1 at the moment */
} SESS_CERT;

/*#define SSL_DEBUG	*/
/*#define RSA_DEBUG	*/

/*
 * Flag values for enc_flags.
 */

/* Uses signature algorithms extension. */
#define SSL_ENC_FLAG_SIGALGS            (1 << 1)

/* Uses SHA256 default PRF. */
#define SSL_ENC_FLAG_SHA256_PRF         (1 << 2)

/* Allow TLS 1.2 ciphersuites: applies to DTLS 1.2 as well as TLS 1.2. */
#define SSL_ENC_FLAG_TLS1_2_CIPHERS     (1 << 4)

/* Allow TLS 1.3 ciphersuites only. */
#define SSL_ENC_FLAG_TLS1_3_CIPHERS     (1 << 5)

#define TLSV1_ENC_FLAGS		0
#define TLSV1_1_ENC_FLAGS	0
#define TLSV1_2_ENC_FLAGS	(SSL_ENC_FLAG_SIGALGS		| \
				 SSL_ENC_FLAG_SHA256_PRF	| \
				 SSL_ENC_FLAG_TLS1_2_CIPHERS)
#define TLSV1_3_ENC_FLAGS	(SSL_ENC_FLAG_SIGALGS		| \
				 SSL_ENC_FLAG_TLS1_3_CIPHERS)

/*
 * ssl_aead_ctx_st contains information about an AEAD that is being used to
 * encrypt an SSL connection.
 */
struct ssl_aead_ctx_st {
	EVP_AEAD_CTX ctx;
	/*
	 * fixed_nonce contains any bytes of the nonce that are fixed for all
	 * records.
	 */
	unsigned char fixed_nonce[12];
	unsigned char fixed_nonce_len;
	unsigned char variable_nonce_len;
	unsigned char xor_fixed_nonce;
	unsigned char tag_len;
	/*
	 * variable_nonce_in_record is non-zero if the variable nonce
	 * for a record is included as a prefix before the ciphertext.
	 */
	char variable_nonce_in_record;
};

extern const SSL_CIPHER ssl3_ciphers[];

const char *ssl_version_string(int ver);
int ssl_enabled_version_range(SSL *s, uint16_t *min_ver, uint16_t *max_ver);
int ssl_supported_version_range(SSL *s, uint16_t *min_ver, uint16_t *max_ver);
int ssl_max_shared_version(SSL *s, uint16_t peer_ver, uint16_t *max_ver);
int ssl_version_set_min(const SSL_METHOD *meth, uint16_t ver, uint16_t max_ver,
    uint16_t *out_ver);
int ssl_version_set_max(const SSL_METHOD *meth, uint16_t ver, uint16_t min_ver,
    uint16_t *out_ver);
int ssl_downgrade_max_version(SSL *s, uint16_t *max_ver);
int ssl_cipher_in_list(STACK_OF(SSL_CIPHER) *ciphers, const SSL_CIPHER *cipher);
int ssl_cipher_allowed_in_version_range(const SSL_CIPHER *cipher,
    uint16_t min_ver, uint16_t max_ver);

const SSL_METHOD *tls_legacy_method(void);
const SSL_METHOD *ssl_get_method(uint16_t version);

void ssl_clear_cipher_state(SSL *s);
void ssl_clear_cipher_read_state(SSL *s);
void ssl_clear_cipher_write_state(SSL *s);
int ssl_clear_bad_session(SSL *s);

CERT *ssl_cert_new(void);
CERT *ssl_cert_dup(CERT *cert);
void ssl_cert_free(CERT *c);
int ssl_cert_set0_chain(CERT *c, STACK_OF(X509) *chain);
int ssl_cert_set1_chain(CERT *c, STACK_OF(X509) *chain);
int ssl_cert_add0_chain_cert(CERT *c, X509 *cert);
int ssl_cert_add1_chain_cert(CERT *c, X509 *cert);

SESS_CERT *ssl_sess_cert_new(void);
void ssl_sess_cert_free(SESS_CERT *sc);
int ssl_get_new_session(SSL *s, int session);
int ssl_get_prev_session(SSL *s, CBS *session_id, CBS *ext_block,
    int *alert);
int ssl_cipher_id_cmp(const SSL_CIPHER *a, const SSL_CIPHER *b);
SSL_CIPHER *OBJ_bsearch_ssl_cipher_id(SSL_CIPHER *key, SSL_CIPHER const *base,
    int num);
int ssl_cipher_list_to_bytes(SSL *s, STACK_OF(SSL_CIPHER) *ciphers, CBB *cbb);
STACK_OF(SSL_CIPHER) *ssl_bytes_to_cipher_list(SSL *s, CBS *cbs);
STACK_OF(SSL_CIPHER) *ssl_create_cipher_list(const SSL_METHOD *meth,
    STACK_OF(SSL_CIPHER) **pref, STACK_OF(SSL_CIPHER) *tls13,
    const char *rule_str);
int ssl_parse_ciphersuites(STACK_OF(SSL_CIPHER) **out_ciphers, const char *str);
int ssl_merge_cipherlists(STACK_OF(SSL_CIPHER) *cipherlist,
    STACK_OF(SSL_CIPHER) *cipherlist_tls13,
    STACK_OF(SSL_CIPHER) **out_cipherlist);
void ssl_update_cache(SSL *s, int mode);
int ssl_cipher_get_evp(const SSL_SESSION *s, const EVP_CIPHER **enc,
    const EVP_MD **md, int *mac_pkey_type, int *mac_secret_size);
int ssl_cipher_get_evp_aead(const SSL_SESSION *s, const EVP_AEAD **aead);
int ssl_get_handshake_evp_md(SSL *s, const EVP_MD **md);

int ssl_verify_cert_chain(SSL *s, STACK_OF(X509) *sk);
int ssl_undefined_function(SSL *s);
int ssl_undefined_void_function(void);
int ssl_undefined_const_function(const SSL *s);
CERT_PKEY *ssl_get_server_send_pkey(const SSL *s);
EVP_PKEY *ssl_get_sign_pkey(SSL *s, const SSL_CIPHER *c, const EVP_MD **pmd,
    const struct ssl_sigalg **sap);
DH *ssl_get_auto_dh(SSL *s);
int ssl_cert_type(X509 *x, EVP_PKEY *pkey);
void ssl_set_cert_masks(CERT *c, const SSL_CIPHER *cipher);
STACK_OF(SSL_CIPHER) *ssl_get_ciphers_by_id(SSL *s);
int ssl_has_ecc_ciphers(SSL *s);
int ssl_verify_alarm_type(long type);

int SSL_SESSION_ticket(SSL_SESSION *ss, unsigned char **out, size_t *out_len);

const SSL_CIPHER *ssl3_get_cipher_by_char(const unsigned char *p);
int ssl3_put_cipher_by_char(const SSL_CIPHER *c, unsigned char *p);
int ssl3_send_server_certificate(SSL *s);
int ssl3_send_newsession_ticket(SSL *s);
int ssl3_send_cert_status(SSL *s);
int ssl3_get_finished(SSL *s, int state_a, int state_b);
int ssl3_send_change_cipher_spec(SSL *s, int state_a, int state_b);
int ssl3_do_write(SSL *s, int type);
int ssl3_send_alert(SSL *s, int level, int desc);
int ssl3_get_req_cert_types(SSL *s, CBB *cbb);
long ssl3_get_message(SSL *s, int st1, int stn, int mt, long max, int *ok);
int ssl3_send_finished(SSL *s, int a, int b, const char *sender, int slen);
int ssl3_num_ciphers(void);
const SSL_CIPHER *ssl3_get_cipher(unsigned int u);
const SSL_CIPHER *ssl3_get_cipher_by_id(unsigned int id);
const SSL_CIPHER *ssl3_get_cipher_by_value(uint16_t value);
uint16_t ssl3_cipher_get_value(const SSL_CIPHER *c);
int ssl3_renegotiate(SSL *ssl);

int ssl3_renegotiate_check(SSL *ssl);

int ssl3_dispatch_alert(SSL *s);
int ssl3_read_bytes(SSL *s, int type, unsigned char *buf, int len, int peek);
int ssl3_write_bytes(SSL *s, int type, const void *buf, int len);
int ssl3_output_cert_chain(SSL *s, CBB *cbb, CERT_PKEY *cpk);
SSL_CIPHER *ssl3_choose_cipher(SSL *ssl, STACK_OF(SSL_CIPHER) *clnt,
    STACK_OF(SSL_CIPHER) *srvr);
int	ssl3_setup_buffers(SSL *s);
int	ssl3_setup_init_buffer(SSL *s);
void ssl3_release_init_buffer(SSL *s);
int	ssl3_setup_read_buffer(SSL *s);
int	ssl3_setup_write_buffer(SSL *s);
void ssl3_release_buffer(SSL3_BUFFER_INTERNAL *b);
void ssl3_release_read_buffer(SSL *s);
void ssl3_release_write_buffer(SSL *s);
int	ssl3_new(SSL *s);
void	ssl3_free(SSL *s);
int	ssl3_accept(SSL *s);
int	ssl3_connect(SSL *s);
int	ssl3_read(SSL *s, void *buf, int len);
int	ssl3_peek(SSL *s, void *buf, int len);
int	ssl3_write(SSL *s, const void *buf, int len);
int	ssl3_shutdown(SSL *s);
void	ssl3_clear(SSL *s);
long	ssl3_ctrl(SSL *s, int cmd, long larg, void *parg);
long	ssl3_ctx_ctrl(SSL_CTX *s, int cmd, long larg, void *parg);
long	ssl3_callback_ctrl(SSL *s, int cmd, void (*fp)(void));
long	ssl3_ctx_callback_ctrl(SSL_CTX *s, int cmd, void (*fp)(void));
int	ssl3_pending(const SSL *s);

int ssl3_handshake_msg_hdr_len(SSL *s);
int ssl3_handshake_msg_start(SSL *s, CBB *handshake, CBB *body,
    uint8_t msg_type);
int ssl3_handshake_msg_finish(SSL *s, CBB *handshake);
int ssl3_handshake_write(SSL *s);
int ssl3_record_write(SSL *s, int type);

void tls1_record_sequence_increment(unsigned char *seq);
int ssl3_do_change_cipher_spec(SSL *ssl);

int dtls1_do_write(SSL *s, int type);
int ssl3_packet_read(SSL *s, int plen);
int ssl3_packet_extend(SSL *s, int plen);
int ssl_server_legacy_first_packet(SSL *s);
int dtls1_read_bytes(SSL *s, int type, unsigned char *buf, int len, int peek);
int ssl3_write_pending(SSL *s, int type, const unsigned char *buf,
    unsigned int len);
void dtls1_set_message_header(SSL *s, unsigned char mt, unsigned long len,
    unsigned long frag_off, unsigned long frag_len);
void dtls1_set_message_header_int(SSL *s, unsigned char mt,
    unsigned long len, unsigned short seq_num, unsigned long frag_off,
    unsigned long frag_len);

int dtls1_write_app_data_bytes(SSL *s, int type, const void *buf, int len);
int dtls1_write_bytes(SSL *s, int type, const void *buf, int len);

int dtls1_read_failed(SSL *s, int code);
int dtls1_buffer_message(SSL *s, int ccs);
int dtls1_retransmit_message(SSL *s, unsigned short seq,
    unsigned long frag_off, int *found);
int dtls1_get_queue_priority(unsigned short seq, int is_ccs);
int dtls1_retransmit_buffered_messages(SSL *s);
void dtls1_clear_record_buffer(SSL *s);
int dtls1_get_message_header(unsigned char *data,
    struct hm_header_st *msg_hdr);
void dtls1_get_ccs_header(unsigned char *data, struct ccs_header_st *ccs_hdr);
void dtls1_reset_seq_numbers(SSL *s, int rw);
void dtls1_build_sequence_number(unsigned char *dst, unsigned char *seq,
    unsigned short epoch);
struct timeval* dtls1_get_timeout(SSL *s, struct timeval* timeleft);
int dtls1_check_timeout_num(SSL *s);
int dtls1_handle_timeout(SSL *s);
const SSL_CIPHER *dtls1_get_cipher(unsigned int u);
void dtls1_start_timer(SSL *s);
void dtls1_stop_timer(SSL *s);
int dtls1_is_timer_expired(SSL *s);
void dtls1_double_timeout(SSL *s);
unsigned int dtls1_min_mtu(void);

/* some client-only functions */
int ssl3_send_client_hello(SSL *s);
int ssl3_get_dtls_hello_verify(SSL *s);
int ssl3_get_server_hello(SSL *s);
int ssl3_get_certificate_request(SSL *s);
int ssl3_get_new_session_ticket(SSL *s);
int ssl3_get_cert_status(SSL *s);
int ssl3_get_server_done(SSL *s);
int ssl3_send_client_verify(SSL *s);
int ssl3_send_client_certificate(SSL *s);
int ssl_do_client_cert_cb(SSL *s, X509 **px509, EVP_PKEY **ppkey);
int ssl3_send_client_key_exchange(SSL *s);
int ssl3_get_server_key_exchange(SSL *s);
int ssl3_get_server_certificate(SSL *s);
int ssl3_check_cert_and_algorithm(SSL *s);
int ssl3_check_finished(SSL *s);

/* some server-only functions */
int ssl3_get_client_hello(SSL *s);
int ssl3_send_dtls_hello_verify_request(SSL *s);
int ssl3_send_server_hello(SSL *s);
int ssl3_send_hello_request(SSL *s);
int ssl3_send_server_key_exchange(SSL *s);
int ssl3_send_certificate_request(SSL *s);
int ssl3_send_server_done(SSL *s);
int ssl3_get_client_certificate(SSL *s);
int ssl3_get_client_key_exchange(SSL *s);
int ssl3_get_cert_verify(SSL *s);

int ssl_kex_dummy_ecdhe_x25519(EVP_PKEY *pkey);
int ssl_kex_generate_ecdhe_ecp(EC_KEY *ecdh, int nid);
int ssl_kex_public_ecdhe_ecp(EC_KEY *ecdh, CBB *cbb);
int ssl_kex_peer_public_ecdhe_ecp(EC_KEY *ecdh, int nid, CBS *cbs);
int ssl_kex_derive_ecdhe_ecp(EC_KEY *ecdh, EC_KEY *ecdh_peer,
    uint8_t **shared_key, size_t *shared_key_len);

int tls1_new(SSL *s);
void tls1_free(SSL *s);
void tls1_clear(SSL *s);

int dtls1_new(SSL *s);
void dtls1_free(SSL *s);
void dtls1_clear(SSL *s);
long dtls1_ctrl(SSL *s, int cmd, long larg, void *parg);

long dtls1_get_message(SSL *s, int st1, int stn, int mt, long max, int *ok);
int dtls1_get_record(SSL *s);
int dtls1_dispatch_alert(SSL *s);

int ssl_init_wbio_buffer(SSL *s, int push);
void ssl_free_wbio_buffer(SSL *s);

int tls1_transcript_hash_init(SSL *s);
int tls1_transcript_hash_update(SSL *s, const unsigned char *buf, size_t len);
int tls1_transcript_hash_value(SSL *s, const unsigned char *out, size_t len,
    size_t *outlen);
void tls1_transcript_hash_free(SSL *s);

int tls1_transcript_init(SSL *s);
void tls1_transcript_free(SSL *s);
void tls1_transcript_reset(SSL *s);
int tls1_transcript_append(SSL *s, const unsigned char *buf, size_t len);
int tls1_transcript_data(SSL *s, const unsigned char **data, size_t *len);
void tls1_transcript_freeze(SSL *s);
void tls1_transcript_unfreeze(SSL *s);
int tls1_transcript_record(SSL *s, const unsigned char *buf, size_t len);

void tls1_cleanup_key_block(SSL *s);
int tls1_change_cipher_state(SSL *s, int which);
int tls1_setup_key_block(SSL *s);
int tls1_final_finish_mac(SSL *s, const char *str, int slen, unsigned char *p);
int tls1_generate_master_secret(SSL *s, unsigned char *out,
    unsigned char *p, int len);
int tls1_export_keying_material(SSL *s, unsigned char *out, size_t olen,
    const char *label, size_t llen, const unsigned char *p, size_t plen,
    int use_context);
int tls1_alert_code(int code);
int ssl_ok(SSL *s);

int ssl_using_ecc_cipher(SSL *s);
int ssl_check_srvr_ecc_cert_and_alg(X509 *x, SSL *s);

void tls1_get_formatlist(SSL *s, int client_formats, const uint8_t **pformats,
    size_t *pformatslen);
void tls1_get_group_list(SSL *s, int client_groups, const uint16_t **pgroups,
    size_t *pgroupslen);

int tls1_set_groups(uint16_t **out_group_ids, size_t *out_group_ids_len,
    const int *groups, size_t ngroups);
int tls1_set_group_list(uint16_t **out_group_ids, size_t *out_group_ids_len,
    const char *groups);

int tls1_ec_curve_id2nid(const uint16_t curve_id);
uint16_t tls1_ec_nid2curve_id(const int nid);
int tls1_check_curve(SSL *s, const uint16_t group_id);
int tls1_get_shared_curve(SSL *s);

int ssl_parse_serverhello_tlsext(SSL *s, unsigned char **data,
    size_t n, int *al);
int ssl_check_clienthello_tlsext_early(SSL *s);
int ssl_check_clienthello_tlsext_late(SSL *s);
int ssl_check_serverhello_tlsext(SSL *s);

#define TLS1_TICKET_FATAL_ERROR		-1
#define TLS1_TICKET_NONE		 0
#define TLS1_TICKET_EMPTY		 1
#define TLS1_TICKET_NOT_DECRYPTED	 2
#define TLS1_TICKET_DECRYPTED		 3

int tls1_process_ticket(SSL *s, CBS *ext_block, int *alert, SSL_SESSION **ret);

long ssl_get_algorithm2(SSL *s);

int tls1_check_ec_server_key(SSL *s);

/* s3_cbc.c */
void ssl3_cbc_copy_mac(unsigned char *out, const SSL3_RECORD_INTERNAL *rec,
    unsigned int md_size, unsigned int orig_len);
int ssl3_cbc_remove_padding(SSL3_RECORD_INTERNAL *rec, unsigned int eiv_len,
    unsigned int mac_size);
char ssl3_cbc_record_digest_supported(const EVP_MD_CTX *ctx);
int ssl3_cbc_digest_record(const EVP_MD_CTX *ctx, unsigned char *md_out,
    size_t *md_out_size, const unsigned char header[13],
    const unsigned char *data, size_t data_plus_mac_size,
    size_t data_plus_mac_plus_padding_size, const unsigned char *mac_secret,
    unsigned int mac_secret_length);
int SSL_state_func_code(int _state);

#define SSLerror(s, r) SSL_error_internal(s, r, __FILE__, __LINE__)
#define SSLerrorx(r) ERR_PUT_error(ERR_LIB_SSL,(0xfff),(r),__FILE__,__LINE__)
void SSL_error_internal(const SSL *s, int r, char *f, int l);

#ifndef OPENSSL_NO_SRTP

int srtp_find_profile_by_name(const char *profile_name,
    const SRTP_PROTECTION_PROFILE **pptr, unsigned int len);
int srtp_find_profile_by_num(unsigned int profile_num,
    const SRTP_PROTECTION_PROFILE **pptr);

#endif /* OPENSSL_NO_SRTP */

__END_HIDDEN_DECLS

#endif
