/*
// Interface to the OS Cryptographically-Secure Pseudo-Random Number Generator

// Copyright 2017 Michael Thomas Greer
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file ../../LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )
*/

#pragma once
#ifndef DUTHOMHAS_CSPRNG_H
#define DUTHOMHAS_CSPRNG_H

/* ------------------------------------------------------------------------------------------------
 * CSPRNG
 */
typedef void* CSPRNG;

#ifdef __cplusplus
extern "C" {
#endif

CSPRNG csprng_create();
/* Returns a new CSPRNG object, 0 on error.
 */

int csprng_get( CSPRNG, void* dest, unsigned long long size );
/* Fills $dest with $size bytes worth of random data.
 * Returns 1 on succes, 0 on failure.
 */

long csprng_get_int( CSPRNG );
/* Return a random value
 * There is no way to know if it failed.
 * (If it did fail, the result will be 0, which is fairly unlikely
 *  to occur multiple times in sequence.
 *  It is also unlikely to fail at all if csprng_create() succeeded.)
 */

CSPRNG csprng_destroy( CSPRNG );
/* Destroy an existing CSPRNG object. Returns 0.
 * Use it as:
 *   v = csprng_destroy( v );
 */

#ifdef __cplusplus
}
#endif

#endif
