/*
// C language test for the OS Cryptographically-Secure Pseudo-Random Number Generator

// Copyright 2017 Michael Thomas Greer
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file ../LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )
*/

#include <stdio.h>
#include <duthomhas/csprng.h>

int main()
{
  int n;

  CSPRNG rng = csprng_create();
  if (!rng)
  {
    fprintf( stderr, "%s\n", "Alas, failed to initialize the OS CSPRNG." );
    return 1;
  }

  puts( "----------------------------------------------------------------------\n"
        "test.c\n"
        "----------------------------------------------------------------------" );

  printf( "five ints\n" );
  for (n = 0; n < 5; n++)
    printf( "  %ld", csprng_get_int( rng ) );

  printf( "\n\nfive chars\n  \"" );
  for (n = 0; n < 5; n++)
  {
    char c = ((unsigned)csprng_get_int( rng ) % 95) + 32;
    printf( "%c", c );
  }
  
  printf( "\"\n\nfive floats\n" );
  for (n = 0; n < 5; n++)
  {
    float x;
    csprng_get( rng, &x, sizeof(x) );
    printf( "  %g", x );
  }

  printf( "\n\na five-integer array\n  {" );
  {
    int xs[ 5 ];
    csprng_get( rng, xs, 5 * sizeof(xs[0]) );
    for (n = 0; n < 5; n++)
      printf( "%s %d", n ? "," : "", xs[ n ] );
    puts( " }" );
  }

  puts( "----------------------------------------------------------------------" );
  
  rng = csprng_destroy( rng );
  return 0;
}
