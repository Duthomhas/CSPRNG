// Interface to the OS Cryptographically-Secure Pseudo-Random Number Generator

// Copyright 2017 Michael Thomas Greer
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file ../../LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )

#ifndef DUTHOMHAS_CSPRNG_HPP
#define DUTHOMHAS_CSPRNG_HPP

#include <initializer_list>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>

#include <duthomhas/csprng.h>
#include <duthomhas/is_iterable.hpp>

//-------------------------------------------------------------------------------------------------
namespace duthomhas
{
  struct csprng
  {
    // Standard C++ Library PRNG boilerplate  . . . . . . . . . . . . . . . . . . . . . .
    typedef unsigned long result_type;

    static constexpr result_type min() { return std::numeric_limits <result_type> ::min(); }
    static constexpr result_type max() { return std::numeric_limits <result_type> ::max(); }

    template <typename Sseq>
    void seed( Sseq& ) { }
    void seed( result_type ) { }
    void discard( unsigned long long ) { }

  public:
    // Standard C++ std::seed_seq boilerplate . . . . . . . . . . . . . . . . . . . . . .
    template <typename Iterator>
    csprng( Iterator begin, Iterator end ):
      internal( csprng_create() ),
      sseq( internal, std::distance( begin, end ) )
      { }

    template <typename T>
    csprng( std::initializer_list <T> xs ):
      internal( csprng_create() ),
      sseq( internal, xs.size() )
      { }

    struct sseq_type
    {
      // ( This is separated out like this otherwise objects that take a SeedSeq
      //   object may get confused by the conversion operators in the main class. )

      template <typename Iterator>
      void generate( Iterator begin, Iterator end )
      {
        while (begin != end)
          *begin++ = csprng_get_int( internal );
      }

      std::size_t size() const { return seed_seq_size; }

      template <typename Iterator>
      void param( Iterator dest ) const
      {
        for (auto n = seed_seq_size; n--; )
          *dest++ = csprng_get_int( internal );
      }

      CSPRNG& internal;
      std::size_t seed_seq_size;

      sseq_type( CSPRNG& internal, std::size_t seed_seq_size ):
        internal( internal ),
        seed_seq_size( seed_seq_size )
        { }
    };

  public:
    // Runtime errors . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    struct exception: public std::runtime_error
    {
      exception( const char*        message ): std::runtime_error( message ) { }
      exception( const std::string& message ): std::runtime_error( message ) { }
    };

    // Constructors . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    csprng(): internal( csprng_create() ), sseq( internal, 0 )
    {
      if (!internal)
        throw exception( "duthomhas::CSPRNG: Failed to initialize the OS CSPRNG" );
    }

    csprng( const csprng& that ):
      internal( csprng_create() ),
      sseq( internal, that.sseq.seed_seq_size )
    {
      if (!internal)
        throw exception( "duthomhas::CSPRNG: Failed to initialize the OS CSPRNG" );
    }

    // Destructor . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
   ~csprng()
    {
      internal = csprng_destroy( internal );
    }

    // Typed buffer fill  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    //   int* xs = rng( new int[ 5 ], 5 );
    template <typename T>
    T* operator () ( T* buffer, std::size_t n )
    {
      if (!csprng_get( internal, (void*)buffer, n * sizeof(T) ))
        throw exception( "duthomhas::CSPRNG: Failed to read the OS CSPRNG" );
      return buffer;
    }

    // Untyped buffer fill  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    //   void* p = get_some_unknown_with_known_size_in_bytes();
    //   rng( p, size_in_bytes );
//    template <typename T>
    void* operator () ( void* buffer, std::size_t n )
    {
      return operator () <unsigned char> ( (unsigned char*)buffer, n );
    }

    // Typed assignment . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    //   int x = rng();
    template <typename T>
    operator T ()
    {
      T result;
      return operator () ( result );
    }

    // Untyped function --> unsigned long . . . . . . . . . . . . . . . . . . . . . . . .
    //   rng()
    result_type operator () ()
    {
      result_type result;
      return *operator () ( &result, 1 );
    }

    // Fundamental / assignable from fundamental types function . . . . . . . . . . . . .
    //   rng( int() )
    //
    //   int x;
    //   rng( x )
    template <typename T>
    typename std::enable_if <std::is_fundamental <typename std::remove_reference <T> ::type> ::value, T&> ::type
    operator () ( T&& value )
    {
      operator () ( &value, 1 );
      return value;
    }

    // Iterable types function  . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    //   auto s = rng( std::string( 20, 0 ) );
    //   auto v = rng( std::vector <int> ( 20 ) );
    template <typename Iterable>
    typename std::enable_if <is_iterable <Iterable> ::value, Iterable&> ::type
    operator () ( Iterable&& value )
    {
      for (auto& v : value)
        operator () ( v );
      return value;
    }

    // Array types function . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    //   int xs[ 20 ];
    //   rng( xs )
    template <typename T, std::size_t N>
    T* operator () ( T* (&array)[ N ] )
    {
      for (auto& v : array)
        operator () ( v );
      return &(array[0]);
    }

  private:
    // Internal state is hidden from user . . . . . . . . . . . . . . . . . . . . . . . .
    CSPRNG internal;
    
  public:
    // This thing is public, though . . . . . . . . . . . . . . . . . . . . . . . . . . .
    sseq_type sseq;

  };

}

#endif
