/******************************************************************************
** (C) Copyright 2015 by Thomas Bettens. All Rights Reserved.
**
** DISCLAIMER: The authors have used their best efforts in preparing this
** code. These efforts include the development, research, and testing of the
** theories and programs to determine their effectiveness. The authors make no
** warranty of any kind, expressed or implied, with regard to these programs or
** to the documentation contained within. The authors shall not be liable in
** any event for incidental or consequential damages in connection with, or
** arising out of, the furnishing, performance, or use of these libraries and
** programs.  Distribution without written consent from the authors is
** prohibited.
******************************************************************************/

/**************************************************
** Intermediate C++ Mail System Project Possible Solution
**
** Thomas Bettens
** Last modified:  26-April-2015
** Last Verified:  12-June-2015
** Verified with:  VC++2015 RC, GCC 5.1,  Clang 3.5
***************************************************/


#ifndef UTILITIES_Key_hpp
#define UTILITIES_Key_hpp

#include <cstddef>  // size_t
#include <iostream>
#include <sstream>
#include <string>




namespace Utilities
{
  // Want to differentiate the different kinds of keys in the system to make it hard for users to use keys incorrectly. For example
  // comparing the key of an Address to the key of an Employee is semantic nonsense, so let's not allow it and have the
  // compiler tell the user in the form of a compiler error something is wrong. Better to find out at compile time than at run
  // time.

  template <class T>
  class Key_t
  {
    friend std::istream & operator >> (std::istream & s, Key_t & key)
    {
      std::string token;
      std::getline( s, token, key.RECORD_SEPARATOR );

      std::istringstream( token ) >> key._value;

      return s;
    }

    friend std::ostream & operator << (std::ostream & s, const Key_t & key)
    {
      s << key._value << key.RECORD_SEPARATOR;
      return s;
    }


    friend bool operator == (const Key_t & lhs, const Key_t & rhs) noexcept { return  lhs._value == rhs._value; }
    friend bool operator <  (const Key_t & lhs, const Key_t & rhs) noexcept { return  lhs._value <  rhs._value; }


    public:
      constexpr Key_t()                           noexcept     = default;
      constexpr Key_t( const Key_t &  )           noexcept     = default;
      constexpr Key_t(       Key_t && )           noexcept     = default;

      Key_t &   operator = ( const Key_t &  )     noexcept     = default;
      Key_t &   operator = (       Key_t && )     noexcept     = default;

      constexpr Key_t( std::size_t initialValue ) noexcept : _value( initialValue ) {}

      Key_t & operator += ( const Key_t & rhs )   noexcept     { _value += rhs._value;  return *this; }
      Key_t & operator -= ( const Key_t & rhs )   noexcept     { _value -= rhs._value;  return *this; }

      Key_t & operator ++ ()                      noexcept     { ++_value;  return *this; }
      Key_t & operator -- ()                      noexcept     { --_value;  return *this; }

      Key_t   operator ++ (int)                   noexcept     { Key_t temp( *this );  ++(*this);  return temp; }
      Key_t   operator -- (int)                   noexcept     { Key_t temp( *this );  --(*this);  return temp; }

      ~Key_t()                                    noexcept     = default;

      explicit operator std::string() const                    { return std::to_string( _value ); }

    private:
      std::size_t           _value           =  0U;
      static constexpr char FIELD_SEPARATOR  =  '\x03';  // ETX (End of Text) character
      static constexpr char RECORD_SEPARATOR =  '\x04';  // EOT (End of Transmission) character

  };  // struct Key_t




  template <class T>
  inline bool operator <= ( const Key_t<T> & lhs, const Key_t<T> & rhs ) noexcept      { return !(rhs <  lhs); } // note the position of rhs and lhs here

  template <class T>
  inline bool operator != ( const Key_t<T> & lhs, const Key_t<T> & rhs ) noexcept      { return !(lhs == rhs); }

  template <class T>
  inline bool operator >  ( const Key_t<T> & lhs, const Key_t<T> & rhs ) noexcept      { return  (rhs <  lhs); } // note the position of rhs and lhs here

  template <class T>
  inline bool operator >= ( const Key_t<T> & lhs, const Key_t<T> & rhs ) noexcept      { return !(lhs <  rhs); }




  template <class T>
  inline Key_t<T>   operator + ( Key_t<T> lhs, const Key_t<T> & rhs )    noexcept      { return lhs += rhs; }

  template <class T>
  inline Key_t<T>   operator - ( Key_t<T> lhs, const Key_t<T> & rhs )    noexcept      { return lhs -= rhs; }

} // namespace Utilities
#endif
