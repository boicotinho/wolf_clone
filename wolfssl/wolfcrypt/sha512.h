/* sha512.h
 *
 * Copyright (C) 2006-2021 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

/*!
    \file wolfssl/wolfcrypt/sha512.h
*/


#ifndef WOLF_CRYPT_SHA512_H
#define WOLF_CRYPT_SHA512_H

#include <wolfssl/wolfcrypt/types.h>





#ifdef __cplusplus
    extern "C" {
#endif

/* avoid redefinition of structs */

#ifdef WOLFSSL_ESP32WROOM32_CRYPT
    #include <wolfssl/wolfcrypt/port/Espressif/esp32-crypt.h>
#endif
#if defined(WOLFSSL_PSOC6_CRYPTO)
    #include "cy_crypto_core_sha.h"
    #include "cy_device_headers.h"
    #include "cy_crypto_common.h"
    #include "cy_crypto_core.h"
#endif
#if defined(WOLFSSL_KCAPI_HASH)
    #include <wolfssl/wolfcrypt/port/kcapi/kcapi_hash.h>
#endif

#if defined(__IAR_SYSTEMS_ICC__) || defined(__GNUC__)
    #define SHA512_NOINLINE __attribute__((noinline))
#else
    #define SHA512_NOINLINE
#endif


#if !defined(NO_OLD_SHA_NAMES)
    #define SHA512             WC_SHA512
#endif

#if !defined(NO_OLD_WC_NAMES)
    #define Sha512             wc_Sha512
    #define SHA512_BLOCK_SIZE  WC_SHA512_BLOCK_SIZE
    #define SHA512_DIGEST_SIZE WC_SHA512_DIGEST_SIZE
    #define SHA512_PAD_SIZE    WC_SHA512_PAD_SIZE
#endif


/* in bytes */
enum {
    WC_SHA512              =   WC_HASH_TYPE_SHA512,
    #ifndef WOLFSSL_NOSHA512_224
    WC_SHA512_224          =   WC_HASH_TYPE_SHA512_224,
    #endif
    #ifndef WOLFSSL_NOSHA512_256
    WC_SHA512_256          =   WC_HASH_TYPE_SHA512_256,
    #endif
    WC_SHA512_BLOCK_SIZE   = 128,
    WC_SHA512_DIGEST_SIZE  =  64,
    WC_SHA512_PAD_SIZE     = 112,

    WC_SHA512_224_BLOCK_SIZE  = WC_SHA512_BLOCK_SIZE,
    WC_SHA512_224_DIGEST_SIZE = 28,
    WC_SHA512_224_PAD_SIZE    = WC_SHA512_PAD_SIZE,

    WC_SHA512_256_BLOCK_SIZE  = WC_SHA512_BLOCK_SIZE,
    WC_SHA512_256_DIGEST_SIZE = 32,
    WC_SHA512_256_PAD_SIZE    = WC_SHA512_PAD_SIZE,
};


#if defined(WOLFSSL_IMX6_CAAM)
    #include "wolfssl/wolfcrypt/port/caam/wolfcaam_sha.h"
#else
/* wc_Sha512 digest */
struct wc_Sha512 {
#ifdef WOLFSSL_PSOC6_CRYPTO
    cy_stc_crypto_sha_state_t hash_state;
    cy_en_crypto_sha_mode_t sha_mode;
    cy_stc_crypto_v2_sha512_buffers_t sha_buffers;
#else
    word64  digest[WC_SHA512_DIGEST_SIZE / sizeof(word64)];
    word64  buffer[WC_SHA512_BLOCK_SIZE  / sizeof(word64)];
    word32  buffLen;   /* in bytes          */
    word64  loLen;     /* length in bytes   */
    word64  hiLen;     /* length in bytes   */
    void*   heap;
    const byte* data;
#ifdef WOLFSSL_SMALL_STACK_CACHE
    word64* W;
#endif
#if defined(WOLFSSL_ESP32WROOM32_CRYPT) && \
   !defined(NO_WOLFSSL_ESP32WROOM32_CRYPT_HASH)
    WC_ESP32SHA ctx;
#endif
#ifdef WOLFSSL_KCAPI_HASH
    wolfssl_KCAPI_Hash kcapi;
#endif
#if defined(WOLFSSL_HASH_KEEP)
    byte*  msg;
    word32 used;
    word32 len;
#endif
#endif /* WOLFSSL_PSOC6_CRYPTO */
};


#ifndef WC_SHA512_TYPE_DEFINED
    typedef struct wc_Sha512 wc_Sha512;
    typedef struct wc_Sha512 wc_Sha512_224;
    typedef struct wc_Sha512 wc_Sha512_256;

#define WC_SHA512_TYPE_DEFINED
#endif
#endif /* WOLFSSL_IMX6_CAAM && !WOLFSSL_QNX_CAAM */




WOLFSSL_API int wc_InitSha512(wc_Sha512* sha);
WOLFSSL_API int wc_InitSha512_ex(wc_Sha512* sha, void* heap, int devId);
WOLFSSL_API int wc_Sha512Update(wc_Sha512* sha, const byte* data, word32 len);
WOLFSSL_API int wc_Sha512FinalRaw(wc_Sha512* sha512, byte* hash);
WOLFSSL_API int wc_Sha512Final(wc_Sha512* sha512, byte* hash);
WOLFSSL_API void wc_Sha512Free(wc_Sha512* sha);

WOLFSSL_API int wc_Sha512GetHash(wc_Sha512* sha512, byte* hash);
WOLFSSL_API int wc_Sha512Copy(wc_Sha512* src, wc_Sha512* dst);

#if defined(WOLFSSL_HASH_KEEP)
    WOLFSSL_API int wc_Sha512_Grow(wc_Sha512* sha512, const byte* in, int inSz);
#endif


#if !defined(WOLFSSL_NOSHA512_224)
WOLFSSL_API int wc_InitSha512_224(wc_Sha512* sha);
WOLFSSL_API int wc_InitSha512_224_ex(wc_Sha512* sha, void* heap, int devId);
WOLFSSL_API int wc_Sha512_224Update(wc_Sha512* sha, const byte* data, word32 len);
WOLFSSL_API int wc_Sha512_224FinalRaw(wc_Sha512* sha512, byte* hash);
WOLFSSL_API int wc_Sha512_224Final(wc_Sha512* sha512, byte* hash);
WOLFSSL_API void wc_Sha512_224Free(wc_Sha512* sha);
WOLFSSL_API int wc_Sha512_224GetHash(wc_Sha512* sha512, byte* hash);
WOLFSSL_API int wc_Sha512_224Copy(wc_Sha512* src, wc_Sha512* dst);

#endif /* !WOLFSSL_NOSHA512_224 */

#if !defined(WOLFSSL_NOSHA512_256)
WOLFSSL_API int wc_InitSha512_256(wc_Sha512* sha);
WOLFSSL_API int wc_InitSha512_256_ex(wc_Sha512* sha, void* heap, int devId);
WOLFSSL_API int wc_Sha512_256Update(wc_Sha512* sha, const byte* data, word32 len);
WOLFSSL_API int wc_Sha512_256FinalRaw(wc_Sha512* sha512, byte* hash);
WOLFSSL_API int wc_Sha512_256Final(wc_Sha512* sha512, byte* hash);
WOLFSSL_API void wc_Sha512_256Free(wc_Sha512* sha);
WOLFSSL_API int wc_Sha512_256GetHash(wc_Sha512* sha512, byte* hash);
WOLFSSL_API int wc_Sha512_256Copy(wc_Sha512* src, wc_Sha512* dst);

#endif /* !WOLFSSL_NOSHA512_256 */





/* avoid redefinition of structs */

#if !defined(NO_OLD_SHA_NAMES)
    #define SHA384             WC_SHA384
#endif

#if !defined(NO_OLD_WC_NAMES)
    #define Sha384             wc_Sha384
    #define SHA384_BLOCK_SIZE  WC_SHA384_BLOCK_SIZE
    #define SHA384_DIGEST_SIZE WC_SHA384_DIGEST_SIZE
    #define SHA384_PAD_SIZE    WC_SHA384_PAD_SIZE
#endif

/* in bytes */
enum {
    WC_SHA384              =   WC_HASH_TYPE_SHA384,
    WC_SHA384_BLOCK_SIZE   =   WC_SHA512_BLOCK_SIZE,
    WC_SHA384_DIGEST_SIZE  =   48,
    WC_SHA384_PAD_SIZE     =   WC_SHA512_PAD_SIZE
};


#ifndef WC_SHA384_TYPE_DEFINED
    typedef struct wc_Sha512 wc_Sha384;
    #define WC_SHA384_TYPE_DEFINED
#endif

WOLFSSL_API int wc_InitSha384(wc_Sha384* sha);
WOLFSSL_API int wc_InitSha384_ex(wc_Sha384* sha, void* heap, int devId);
WOLFSSL_API int wc_Sha384Update(wc_Sha384* sha, const byte* data, word32 len);
WOLFSSL_API int wc_Sha384FinalRaw(wc_Sha384* sha384, byte* hash);
WOLFSSL_API int wc_Sha384Final(wc_Sha384* sha384, byte* hash);
WOLFSSL_API void wc_Sha384Free(wc_Sha384* sha);

WOLFSSL_API int wc_Sha384GetHash(wc_Sha384* sha384, byte* hash);
WOLFSSL_API int wc_Sha384Copy(wc_Sha384* src, wc_Sha384* dst);

#if defined(WOLFSSL_HASH_KEEP)
    WOLFSSL_API int wc_Sha384_Grow(wc_Sha384* sha384, const byte* in, int inSz);
#endif


#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLF_CRYPT_SHA512_H */

