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


#ifndef ADDRESSES_Address_hpp
#define ADDRESSES_Address_hpp

#include <iostream>
#include <string>

#include "Utilities/Exceptions.hpp"




namespace Addresses
{
  class Address
  {
    friend std::ostream & operator<< (std::ostream & s, const Address & address);
    friend std::istream & operator>> (std::istream & s,       Address & address);

    friend bool operator==(const Address & lhs, const Address & rhs);
    friend bool operator< (const Address & lhs, const Address & rhs);




    public:
      // Inner Exception Type Hierarchy Definition
      struct AddressExceptions       : Utilities::AbstractException<> { using AbstractException::AbstractException; };  // Class Address exception base class
      struct   StateCodeException    : AddressExceptions              { using AddressExceptions::AddressExceptions; };  // Inherit base class constructors
      struct   ZipCodeException      : AddressExceptions              { using AddressExceptions::AddressExceptions; };


      // Constructors and Destructor
      Address             (                  )          = default;
      Address             ( const Address &  )          = default;
      Address             (       Address && )          = default;
      Address & operator= ( const Address &  )          = default;
      Address & operator= (       Address && )          = default;
     ~Address             (                  ) noexcept = default;

      Address(       std::string   street,
                     std::string   city,
               const std::string & stateCode,
               const std::string & zip          = {} );

      Address(       std::string      street,
                     std::string      city,
               const std::string &    stateCode,
                     unsigned long    zip );


      // Queries
      std::string street    () const noexcept;
      std::string city      () const noexcept;
      std::string state     () const noexcept;
      std::string zipCode   () const noexcept;


      // Conversions
      explicit operator std::string () const;


      // Modifiers
      Address &   street  ( std::string     numbersAndName ) noexcept;
      Address &   city    ( std::string     name           ) noexcept;
      Address &   state   ( std::string     code           );  // Must be a valid two digit code, state name, or standard state abbreviation
      Address &   zipCode ( std::string     code           );  // Zip code rules:  5 digits, not all are zero and not all are nine, optionally followed
      Address &   zipCode ( unsigned long   code           );  //                   by a hyphen and 4 digits, not all are zero and not all are nine.




    private:
      // Instance attribute (aka object state attributes)
      std::string   _street;
      std::string   _city;
      std::string   _state;
      std::string   _zip;


      // Class attributes
      static constexpr char FIELD_SEPARATOR  = '\x03';  // ETX (End of Text) character
      static constexpr char RECORD_SEPARATOR = '\x04';  // EOT (End of Transmission) character ('\n' turns out to not be a good choice because data
                                                        //     files are not then portable between Windows, Unix, and Mac OSs.)
  };  // class Address




  // Non-member functions
  bool operator==(const Address & lhs, const Address & rhs);
  bool operator< (const Address & lhs, const Address & rhs);
  bool operator!=(const Address & lhs, const Address & rhs);
  bool operator> (const Address & lhs, const Address & rhs);
  bool operator<=(const Address & lhs, const Address & rhs);
  bool operator>=(const Address & lhs, const Address & rhs);

  std::ostream & operator<< (std::ostream & s, const Address & address);
  std::istream & operator>> (std::istream & s,       Address & address);
  std::ostream & operator<< (std::ostream & s, const Address * address);
  std::istream & operator>> (std::istream & s,       Address * address);
} // namespace Addresses
#endif
