// is_iterable.hpp

// Copyright 2017 Michael Thomas Greer
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file ../../LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt )

#ifndef DUTHOMHAS_IS_ITERABLE_HPP
#define DUTHOMHAS_IS_ITERABLE_HPP

namespace duthomhas
{

  // The basis for this code was found at
  // https://stackoverflow.com/a/29634934/2706707
  using std::begin;
  using std::end;

  template <typename T>
  class is_iterable
  {
    template <typename U>
    static constexpr auto is_iterable_impl( int ) 
      -> decltype(
           begin( std::declval <U&> () ) != end( std::declval <U&> () ),   // begin/end and operator !=
           void(),                                                         // Handle evil operator ,
           ++std::declval <decltype( begin( std::declval <U&> () ) )&> (), // operator ++
           void( *begin( std::declval <U&> () ) ),                         // operator*
           std::true_type {}
         )
    { return std::true_type {}; }
  
    template <typename U>
    static constexpr std::false_type is_iterable_impl(...)
    { return std::false_type {}; }
  
    typedef decltype( is_iterable_impl <T> ( 0 ) ) type;
    
  public:
    //static constexpr bool value = type::value;
    enum : bool { value = type::value };
  };

}

#endif
