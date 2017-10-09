# duthomhas::CSPRNG

CSPRNG is a small library that uses your **Operating System**’s native 
Cryptographically-Secure Pseudo-Random Number Generator.

It is a superior replacement for the clunky, ill-designed 
[std::random_device](http://en.cppreference.com/w/cpp/numeric/random/random_device).
It won’t throw. It won’t give you non-CS random numbers. It won’t 
pretend to tell you anything about entropy. It just works.


## C++

Using it couldn’t be simpler:

    #include <iostream>
    #include <duthomhas/csprng.hpp>
    
    int main()
    {
      duthomhas::csprng rng;
      std::cout << rng() << "\n";
      
Want a **double**?

      double e;
      std::cout << rng( e )        << "\n";
      std::cout << rng( double() ) << "\n";
    
Or an **array**/**string**/**vector**/whatever?

      char r[ 20 ];
      rng( r );
      
      auto s = rng( std::string( 20, 0 ) );
      auto v = rng( std::vector <int> ( 20 ) );
      
      int* xs = rng( new int[ 20 ], 20 );
      int* vs = rng( malloc( 20*sizeof(int) ), 20*sizeof(int) );
    
Perhaps you’d just like to have a proper 
[std::seed_seq](http://en.cppreference.com/w/cpp/numeric/random/seed_seq) 
for your Mersenne Twister?

      duthomhas::csprng seed;
      std::mt19937 rng( seed.sseq );

      
## But wait, I’m using C

Still easy.

    #include <stdio.h>
    #include <duthomhas/csprng.h>
    
    int main()
    {
      CSPRNG rng = csprng_create( rng );        // Constructor
      if (!rng)
      {
        fprintf( stderr, "%s\n", "No CSPRNG! Fooey!" );
        return 1;
      }
      
      long n = csprng_get_int( rng );           // Get an int
      
      double f;                                 // Get a double
      csprng_get( rng, &f, sizeof(f) );
      
      int xs[ 20 ];                             // Get an array
      csprng_get( rng, xs, sizeof(xs) );
      
      void* p = malloc(20);                     // et cetera
      csprng_get( rng, p, 20 );
      free( p );
      
      rng = csprng_destroy( rng );              // Destructor
      return 0;
    }

See the examples in the ~test/ subdirectory for more.


## Installing

This is such a small library, it works just fine if all you do is copy 
everything found in the ~source/ directory to your program’s source directory. 

    Directory of myprogram/source:
    
      duthomhas/       ←   copied from ~csprng/source/
      csprng.cpp       ←   copied from ~csprng/source/
      myprogram.cpp    ←   your program code
      ...              ←   anything else in your program’s source directory
    
Compile the `csprng.cpp` file along with your code. Make sure to add your 
program’s directory to the compiler’s include path. (If you are using C,
everything is the same except files end in `.c` and `.h` instead of `.cpp` 
and `.hpp`.) Command-line examples:

  * `g++ myprogram.cpp csprng.cpp -I.`
  
  * `cl /EHsc myprogram.cpp csprng.cpp /I.`
  
If you would prefer a static library you can do that too. 
See [below](#test--examples) for more.


## Design Rationale    
    
CSPRNG was not designed to be “header only” for one important reason: 
namespace pollution. Using the OS facility requires pulling _a lot_ of 
symbols from OS-headers, some of which are disruptive if you aren’t aware 
of them. 

This also makes it easy to modularize the C and C++ parts of the code.


## Test / Examples

To compile the examples you will need three things:

  1. Have [CMake](https://cmake.org/) version 3.1 or later.
  2. Have [Tcl/Tk](https://www.activestate.com/activetcl) Wish interpreter version 8.6 or later.
  3. Initialize your command line properly for your compiler.

Once you have your command shell open and ready to compile, change to the 
~csprng/ directory, and run the BUILD script:

    BUILD               (Windows)
    wish BUILD          (Linux)
    
To clean up, type 

    BUILD clean         (Windows)
    wish BUILD clean    (Linux)

You can use CMake directly if you wish. Please read the `CMakeFiles.txt` file 
for how to use it.

### Static Linkage

If you really want a static library (`libcsprng.a` or `csprng.lib`), build 
the examples with static linkage. When done you will find the static library 
in the appropriate `lib` directory. Copy that to your compiler’s correct 
`lib` directory, and copy the `~csprng/source/duthomhas` directory to your 
compiler’s `include` directory (so that it is `include/duthomhas`). 
Thereafter you can link normally with your program, for example: 

    g++ ... my_foo.cpp ... -lcsprng

## License

Copyright 2017 Michael Thomas Greer<br>
Distributed under the Boost Software License, Version 1.0.<br>
(See accompanying file [LICENSE_1_0.txt](LICENSE_1_0.txt) or copy at [http://www.boost.org/LICENSE_1_0.txt](http://www.boost.org/LICENSE_1_0.txt).)
