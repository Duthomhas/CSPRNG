// C++ Tests for duthomhas::CSPRNG

// Copyright 2017 Michael Thomas Greer
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file ../LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )

#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include <duthomhas/csprng.hpp>

#define STRINGIFY(s) STRINGIFY0(s)
#define STRINGIFY0(s) # s

#define LABEL(msg) std::cout << "\n" msg << " " << std::string( 69 - std::string(msg).size(), '~' ) << "\n\n"
#define CHECK(a,b) (a == b ? " : OK\n" : (" : FAIL (should be " STRINGIFY(b) ")\n"))

int main( int argc, char** argv )
{
  duthomhas::csprng rng;

  std::cout << "\n"
    << std::string( 70, '-' ) << "\n"
    << "test.cpp\n"
    << std::string( 70, '-' ) << "\n";

  std::cout << "\n  duthomhas::csprng rng;\n";
    
  LABEL( "No-Operation Methods" );
  { 
    // NOP: "seed" by seed_seq
    std::seed_seq sseq{ 74 };
    rng.seed( sseq );
  
    // NOP: "seed" by integer
    rng.seed( -7 );
    
    // NOP: advance state
    rng.discard( 5000 );
    
    // min()/max()
    std::cout << "  rng.min() = " << rng.min() << CHECK( rng.min(), 0 );
    std::cout << "  rng.max() = " << rng.max() << CHECK( rng.max(), 4294967295 );
  }
  
  
  LABEL( "seed_seq compatibility" );
  { 
    // seed_seq: "initialize" with iterator
    std::vector <int> vs { 1, 2, 3 };
    duthomhas::csprng rng1( vs.begin(), vs.end() );
    std::cout << "  std::vector <int> vs { 1, 2, 3 };\n"
                 "  duthomhas::csprng rng1( vs.begin(), vs.end() );\n\n";
    
    // seed_seq: "initialize" with initializer_list
    duthomhas::csprng rng2 = { 1, 2, 3 };
    std::cout << "  duthomhas::csprng rng2 = { 1, 2, 3 };\n\n";
    
    // seed_seq: "generate" random data
    rng2.sseq.generate( vs.begin(), vs.end() );
    std::cout << "  rng2.sseq.generate()  --> {";
    for (auto v : vs) std::cout << " " << v;
    std::cout << " }\n\n";
    
    // seed_seq.size()
    std::cout << "  rng.sseq.size()  = " << rng .sseq.size() << CHECK( rng .sseq.size(), 0 );
    std::cout << "  rng1.sseq.size() = " << rng1.sseq.size() << CHECK( rng1.sseq.size(), 3 );
    std::cout << "  rng2.sseq.size() = " << rng2.sseq.size() << CHECK( rng2.sseq.size(), 3 ) << "\n";
    
    std::cout << "  rng1.sseq.param()  --> { ";
    rng1.sseq.param( std::ostream_iterator <int> ( std::cout, " " ) );
    std::cout << "}\n";
  }
  
  
  LABEL( "Pretends to be copyable" );
  {
    duthomhas::csprng rng3 {rng};
    std::cout << "  csprng rng3 {rng};\n" 
                 "  rng3();\n"
                 "  --> " << rng3() << "\n";
  }
  
  
  LABEL( "Typed buffer fill" );
  {
    int* xs = rng( new int[ 5 ], 5 );
    std::cout << "  int* xs = rng( new int[ 5 ], 5 );\n"
                 "  --> { ";
    for (int n = 0; n < 5; n++)
      std::cout << xs[ n ] << " ";
    std::cout << "}\n";
    delete [] xs;
  }
  
  
  LABEL( "Untyped buffer fill (displayed as byte values)" );
  {
    void* p = rng( malloc( 5 ), 5 );
    std::cout << "  void* p = rng( malloc( 5 ), 5 );\n"
                 "  --> { ";
    for (int n = 0; n < 5; n++)
      std::cout << (unsigned)(((unsigned char*)p)[ n ]) << " ";
    std::cout << "}\n";
    free( p );
  }
  
  
  LABEL( "Typed assignment" );
  {
    double f = rng();
    std::cout << "  double f = rng();  --> " << f << "\n";
  }
  
  
  LABEL( "Untyped function --> unsigned long long" );
  {
    std::cout << "  rng()  --> " << rng() << "\n";
  }
  
  
  LABEL( "Fundamental / assignable from fundamental types function" );
  {
    std::cout << "  rng( double() )  --> " << rng( double() ) << "\n\n";
    float x; 
    std::cout << "  float x;\n"
                 "  rng( x )  --> " << rng( x ) << "\n";
  }
  
  
  LABEL( "Iterable types function" );
  {
    auto s = rng( std::string( 5, 0 ) );
    std::cout << "  auto s = rng( std::string( 5, 0 ) );\n"
                 "  --> \"";
    for (auto& c : s) c = ((unsigned)c % 95) + 32;  // (make them displayable)
    std::cout << s << "\"\n\n";
    
    auto v = rng( std::vector <int> ( 5 ) );
    std::cout << "  auto v = rng( std::vector <int> ( 5 ) );\n"
                 "  --> { ";
    for (auto n : v) std::cout << n << " ";
    std::cout << "}\n";
  }
  
  
  LABEL( "Array types function" );
  {
    int xs[ 5 ];
    rng( xs );
    std::cout << "  int xs[ 5 ];\n" 
                 "  rng( xs );\n"
                 "  --> { ";
    for (int x : xs) std::cout << x << " ";
    std::cout << "}\n";
  }
  
  
  LABEL( "Seeds a Mersenne Twister" );
  {
    std::mt19937 mt( rng.sseq );
    std::cout << "  std::mt19937 mt( rng.sseq );\n" 
                 "  mt();\n"
                 "  --> " << mt() << "\n\n";
  }
  {
    std::mt19937 mt;
    mt.seed( rng.sseq );
    std::cout << "  std::mt19937 mt;\n" 
                 "  mt.seed( rng.sseq );\n" 
                 "  mt();\n"
                 "  --> " << mt() << "\n\n";
  }

  std::cout << std::string( 70, '-' ) << "\n";
}
