/**
 * \file md5.h
 *
 * \brief MD5 message digest algorithm (hash function)
 *
 * \warning   MD5 is considered a weak message digest and its use constitutes a
 *            security risk. We recommend considering stronger message
 *            digests instead.
 */
/*
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
#ifndef MBEDTLS_MD5_H
#define MBEDTLS_MD5_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif



/**
 * \brief          MD5 context structure
 *
 * \warning        MD5 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
typedef struct
{
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[4];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
}
mbedtls_md5_context;


/**
 * \brief          Initialize MD5 context
 *
 * \param ctx      MD5 context to be initialized
 *
 * \warning        MD5 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
void mbedtls_md5_init( mbedtls_md5_context *ctx );

/**
 * \brief          Clear MD5 context
 *
 * \param ctx      MD5 context to be cleared
 *
 * \warning        MD5 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
void mbedtls_md5_free( mbedtls_md5_context *ctx );

/**
 * \brief          Clone (the state of) an MD5 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 *
 * \warning        MD5 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
void mbedtls_md5_clone( mbedtls_md5_context *dst,
                        const mbedtls_md5_context *src );

/**
 * \brief          MD5 context setup
 *
 * \param ctx      context to be initialized
 *
 * \return         0 if successful
 *
 * \warning        MD5 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
int mbedtls_md5_starts_ret( mbedtls_md5_context *ctx );

/**
 * \brief          MD5 process buffer
 *
 * \param ctx      MD5 context
 * \param input    buffer holding the data
 * \param ilen     length of the input data
 *
 * \return         0 if successful
 *
 * \warning        MD5 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
int mbedtls_md5_update_ret( mbedtls_md5_context *ctx,
                            const unsigned char *input,
                            size_t ilen );

/**
 * \brief          MD5 final digest
 *
 * \param ctx      MD5 context
 * \param output   MD5 checksum result
 *
 * \return         0 if successful
 *
 * \warning        MD5 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
int mbedtls_md5_finish_ret( mbedtls_md5_context *ctx,
                            unsigned char output[16] );

/**
 * \brief          Output = MD5( input buffer )
 *
 * \param input    buffer holding the data
 * \param ilen     length of the input data
 * \param output   MD5 checksum result
 *
 * \return         0 if successful
 *
 * \warning        MD5 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
int mbedtls_md5_ret( const unsigned char *input, size_t ilen, unsigned char output[16] );


#endif /* mbedtls_md5.h */
