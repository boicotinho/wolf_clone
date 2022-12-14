/* rsa.h
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
    \file wolfssl/wolfcrypt/rsa.h
*/

/*

DESCRIPTION
This library provides the interface to the RSA.
RSA keys can be used to encrypt, decrypt, sign and verify data.

*/
#ifndef WOLF_CRYPT_RSA_H
#define WOLF_CRYPT_RSA_H

#include <wolfssl/wolfcrypt/types.h>



/* RSA default exponent */
#ifndef WC_RSA_EXPONENT
    #define WC_RSA_EXPONENT 65537L
#endif

#if defined(WC_RSA_NONBLOCK)
    /* enable support for fast math based non-blocking exptmod */
    /* this splits the RSA function into many smaller operations */
    #ifndef TFM_TIMING_RESISTANT
      #error RSA non-blocking mode only supported with timing resistance enabled
    #endif

    /* RSA bounds check is not supported with RSA non-blocking mode */
    #undef  NO_RSA_BOUNDS_CHECK
    #define NO_RSA_BOUNDS_CHECK
#endif

/* allow for user to plug in own crypto */
#if defined(HAVE_USER_RSA) || defined(HAVE_FAST_RSA)
    #include "user_rsa.h"
#else

    #include <wolfssl/wolfcrypt/integer.h>
    #include <wolfssl/wolfcrypt/random.h>

/* header file needed for OAEP padding */
#include <wolfssl/wolfcrypt/hash.h>

#ifdef WOLFSSL_XILINX_CRYPT
#include "xsecure_rsa.h"
#endif


#if defined(WOLFSSL_KCAPI_RSA)
    #include <wolfssl/wolfcrypt/port/kcapi/kcapi_rsa.h>
#endif

#if defined(WOLFSSL_DEVCRYPTO_RSA)
    #include <wolfssl/wolfcrypt/port/devcrypto/wc_devcrypto.h>
#endif

#ifdef __cplusplus
    extern "C" {
#endif

#ifndef RSA_MIN_SIZE
#define RSA_MIN_SIZE 512
#endif

#ifndef RSA_MAX_SIZE
#define RSA_MAX_SIZE 4096
#endif

/* avoid redefinition of structs */


enum {
    RSA_PUBLIC   = 0,
    RSA_PRIVATE  = 1,

    RSA_TYPE_UNKNOWN    = -1,
    RSA_PUBLIC_ENCRYPT  = 0,
    RSA_PUBLIC_DECRYPT  = 1,
    RSA_PRIVATE_ENCRYPT = 2,
    RSA_PRIVATE_DECRYPT = 3,

    RSA_BLOCK_TYPE_1 = 1,
    RSA_BLOCK_TYPE_2 = 2,

    RSA_MIN_PAD_SZ   = 11,     /* separator + 0 + pad value + 8 pads */

    RSA_PSS_PAD_SZ = 8,
    RSA_PSS_SALT_MAX_SZ = 62,


    RSA_PSS_SALT_LEN_DEFAULT  = -1,
#ifdef WOLFSSL_PSS_SALT_LEN_DISCOVER
    RSA_PSS_SALT_LEN_DISCOVER = -2,
#endif

};

#ifdef WC_RSA_NONBLOCK
typedef struct RsaNb {
    exptModNb_t exptmod; /* non-block expt_mod */
    mp_int tmp;
} RsaNb;
#endif

/* RSA */
struct RsaKey {
    mp_int n, e;
#ifndef WOLFSSL_RSA_PUBLIC_ONLY
    mp_int d, p, q;
#if defined(WOLFSSL_KEY_GEN) || !defined(RSA_LOW_MEM)
    mp_int dP, dQ, u;
#endif
#endif
    void* heap;                               /* for user memory overrides */
    byte* data;                               /* temp buffer for async RSA */
    int   type;                               /* public or private */
    int   state;
    word32 dataLen;
#ifdef WC_RSA_BLINDING
    WC_RNG* rng;                              /* for PrivateDecrypt blinding */
#endif
#ifdef WOLFSSL_XILINX_CRYPT
    word32 pubExp; /* to keep values in scope they are here in struct */
    byte*  mod;
    XSecure_Rsa xRsa;
#endif
#if defined(WOLFSSL_KCAPI_RSA)
    struct kcapi_handle* handle;
#endif
#if !defined(WOLFSSL_RSA_VERIFY_INLINE)
    byte   dataIsAlloc;
#endif
#ifdef WC_RSA_NONBLOCK
    RsaNb* nb;
#endif
#ifdef WOLFSSL_AFALG_XILINX_RSA
    int alFd;
    int rdFd;
#endif
#if defined(WOLFSSL_CAAM)
    word32 blackKey;
#endif
#if defined(WOLFSSL_DEVCRYPTO_RSA)
    WC_CRYPTODEV ctx;
#endif
};

#ifndef WC_RSAKEY_TYPE_DEFINED
    typedef struct RsaKey RsaKey;
    #define WC_RSAKEY_TYPE_DEFINED
#endif


WOLFSSL_API int  wc_InitRsaKey(RsaKey* key, void* heap);
WOLFSSL_API int  wc_InitRsaKey_ex(RsaKey* key, void* heap, int devId);
WOLFSSL_API int  wc_FreeRsaKey(RsaKey* key);
WOLFSSL_API int  wc_CheckRsaKey(RsaKey* key);
#ifdef WOLFSSL_XILINX_CRYPT
WOLFSSL_LOCAL int wc_InitRsaHw(RsaKey* key);
#endif /* WOLFSSL_XILINX_CRYPT */

WOLFSSL_API int  wc_RsaFunction(const byte* in, word32 inLen, byte* out,
                           word32* outLen, int type, RsaKey* key, WC_RNG* rng);

WOLFSSL_API int  wc_RsaPublicEncrypt(const byte* in, word32 inLen, byte* out,
                                 word32 outLen, RsaKey* key, WC_RNG* rng);
WOLFSSL_API int  wc_RsaPrivateDecryptInline(byte* in, word32 inLen, byte** out,
                                        RsaKey* key);
WOLFSSL_API int  wc_RsaPrivateDecrypt(const byte* in, word32 inLen, byte* out,
                                  word32 outLen, RsaKey* key);
WOLFSSL_API int  wc_RsaSSL_Sign(const byte* in, word32 inLen, byte* out,
                            word32 outLen, RsaKey* key, WC_RNG* rng);
WOLFSSL_API int  wc_RsaPSS_Sign(const byte* in, word32 inLen, byte* out,
                                word32 outLen, enum wc_HashType hash, int mgf,
                                RsaKey* key, WC_RNG* rng);
WOLFSSL_API int  wc_RsaPSS_Sign_ex(const byte* in, word32 inLen, byte* out,
                                   word32 outLen, enum wc_HashType hash,
                                   int mgf, int saltLen, RsaKey* key,
                                   WC_RNG* rng);
WOLFSSL_API int  wc_RsaSSL_VerifyInline(byte* in, word32 inLen, byte** out,
                                    RsaKey* key);
WOLFSSL_API int  wc_RsaSSL_Verify(const byte* in, word32 inLen, byte* out,
                              word32 outLen, RsaKey* key);
WOLFSSL_API int  wc_RsaSSL_Verify_ex(const byte* in, word32 inLen, byte* out,
                              word32 outLen, RsaKey* key, int pad_type);
WOLFSSL_API int  wc_RsaSSL_Verify_ex2(const byte* in, word32 inLen, byte* out,
                              word32 outLen, RsaKey* key, int pad_type,
                              enum wc_HashType hash);
WOLFSSL_API int  wc_RsaPSS_VerifyInline(byte* in, word32 inLen, byte** out,
                                        enum wc_HashType hash, int mgf,
                                        RsaKey* key);
WOLFSSL_API int  wc_RsaPSS_VerifyInline_ex(byte* in, word32 inLen, byte** out,
                                           enum wc_HashType hash, int mgf,
                                           int saltLen, RsaKey* key);
WOLFSSL_API int  wc_RsaPSS_Verify(byte* in, word32 inLen, byte* out,
                                  word32 outLen, enum wc_HashType hash, int mgf,
                                  RsaKey* key);
WOLFSSL_API int  wc_RsaPSS_Verify_ex(byte* in, word32 inLen, byte* out,
                                     word32 outLen, enum wc_HashType hash,
                                     int mgf, int saltLen, RsaKey* key);
WOLFSSL_API int  wc_RsaPSS_CheckPadding(const byte* in, word32 inLen, byte* sig,
                                        word32 sigSz,
                                        enum wc_HashType hashType);
WOLFSSL_API int  wc_RsaPSS_CheckPadding_ex(const byte* in, word32 inLen,
                                           byte* sig, word32 sigSz,
                                           enum wc_HashType hashType,
                                           int saltLen, int bits);
WOLFSSL_API int  wc_RsaPSS_CheckPadding_ex2(const byte* in, word32 inLen,
                                           byte* sig, word32 sigSz,
                                           enum wc_HashType hashType,
                                           int saltLen, int bits, void* heap);
WOLFSSL_API int  wc_RsaPSS_VerifyCheckInline(byte* in, word32 inLen, byte** out,
                               const byte* digest, word32 digentLen,
                               enum wc_HashType hash, int mgf,
                               RsaKey* key);
WOLFSSL_API int  wc_RsaPSS_VerifyCheck(byte* in, word32 inLen,
                               byte* out, word32 outLen,
                               const byte* digest, word32 digestLen,
                               enum wc_HashType hash, int mgf,
                               RsaKey* key);

WOLFSSL_API int  wc_RsaEncryptSize(const RsaKey* key);

/* to avoid asn duplicate symbols @wc_fips */
WOLFSSL_API int  wc_RsaPrivateKeyDecode(const byte* input, word32* inOutIdx,
                                        RsaKey* key, word32 inSz);
WOLFSSL_API int  wc_RsaPublicKeyDecode(const byte* input, word32* inOutIdx,
                                       RsaKey* key, word32 inSz);
WOLFSSL_API int  wc_RsaPublicKeyDecodeRaw(const byte* n, word32 nSz,
                                        const byte* e, word32 eSz, RsaKey* key);
#if defined(WOLFSSL_KEY_GEN) ||  defined(WOLFSSL_KCAPI_RSA)
    WOLFSSL_API int wc_RsaKeyToDer(RsaKey* key, byte* output, word32 inLen);
#endif

#ifdef WC_RSA_BLINDING
    WOLFSSL_API int wc_RsaSetRNG(RsaKey* key, WC_RNG* rng);
#endif
#ifdef WC_RSA_NONBLOCK
    WOLFSSL_API int wc_RsaSetNonBlock(RsaKey* key, RsaNb* nb);
    #ifdef WC_RSA_NONBLOCK_TIME
    WOLFSSL_API int wc_RsaSetNonBlockTime(RsaKey* key, word32 maxBlockUs,
                                          word32 cpuMHz);
    #endif
#endif

/*
   choice of padding added after fips, so not available when using fips RSA
 */

/* Mask Generation Function Identifiers */
#define WC_MGF1NONE       0
#define WC_MGF1SHA1       26
#define WC_MGF1SHA224     4
#define WC_MGF1SHA256     1
#define WC_MGF1SHA384     2
#define WC_MGF1SHA512     3
#define WC_MGF1SHA512_224 5
#define WC_MGF1SHA512_256 6

/* Padding types */
#define WC_RSA_PKCSV15_PAD 0
#define WC_RSA_OAEP_PAD    1
#define WC_RSA_PSS_PAD     2
#define WC_RSA_NO_PAD      3

WOLFSSL_API int  wc_RsaPublicEncrypt_ex(const byte* in, word32 inLen, byte* out,
                   word32 outLen, RsaKey* key, WC_RNG* rng, int type,
                   enum wc_HashType hash, int mgf, byte* label, word32 labelSz);
WOLFSSL_API int  wc_RsaPrivateDecrypt_ex(const byte* in, word32 inLen,
                   byte* out, word32 outLen, RsaKey* key, int type,
                   enum wc_HashType hash, int mgf, byte* label, word32 labelSz);
WOLFSSL_API int  wc_RsaPrivateDecryptInline_ex(byte* in, word32 inLen,
                      byte** out, RsaKey* key, int type, enum wc_HashType hash,
                      int mgf, byte* label, word32 labelSz);
#if defined(WC_RSA_DIRECT) || defined(WC_RSA_NO_PADDING)
WOLFSSL_API int wc_RsaDirect(byte* in, word32 inLen, byte* out, word32* outSz,
                   RsaKey* key, int type, WC_RNG* rng);
#endif


WOLFSSL_API int  wc_RsaFlattenPublicKey(RsaKey* key, byte* e, word32* eSz,
                                        byte* n, word32* nSz);
WOLFSSL_API int wc_RsaExportKey(RsaKey* key,
                                byte* e, word32* eSz,
                                byte* n, word32* nSz,
                                byte* d, word32* dSz,
                                byte* p, word32* pSz,
                                byte* q, word32* qSz);

#ifdef WOLFSSL_KEY_GEN
    WOLFSSL_API int wc_MakeRsaKey(RsaKey* key, int size, long e, WC_RNG* rng);
    WOLFSSL_API int wc_CheckProbablePrime_ex(const byte* p, word32 pSz,
                                          const byte* q, word32 qSz,
                                          const byte* e, word32 eSz,
                                          int nlen, int* isPrime, WC_RNG* rng);
    WOLFSSL_API int wc_CheckProbablePrime(const byte* p, word32 pSz,
                                          const byte* q, word32 qSz,
                                          const byte* e, word32 eSz,
                                          int nlen, int* isPrime);
#endif

WOLFSSL_LOCAL int wc_RsaPad_ex(const byte* input, word32 inputLen, byte* pkcsBlock,
        word32 pkcsBlockLen, byte padValue, WC_RNG* rng, int padType,
        enum wc_HashType hType, int mgf, byte* optLabel, word32 labelLen,
        int saltLen, int bits, void* heap);
WOLFSSL_LOCAL int wc_RsaUnPad_ex(byte* pkcsBlock, word32 pkcsBlockLen, byte** out,
                                   byte padValue, int padType, enum wc_HashType hType,
                                   int mgf, byte* optLabel, word32 labelLen, int saltLen,
                                   int bits, void* heap);

WOLFSSL_LOCAL int wc_hash2mgf(enum wc_HashType hType);

#endif /* HAVE_USER_RSA */

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLF_CRYPT_RSA_H */

