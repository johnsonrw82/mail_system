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


#ifndef COMPANIES_Company_hpp
#define COMPANIES_Company_hpp

#include <iostream>
#include <string>

#include "Utilities/Exceptions.hpp"



namespace Companies
{
  class Company
  {
    friend std::ostream & operator<< (std::ostream & s, const Company & company);
    friend std::istream & operator>> (std::istream & s,       Company & company);

    friend bool operator==(const Company & lhs, const Company & rhs);
    friend bool operator< (const Company & lhs, const Company & rhs);




    public:
      // Inner Exception Type Hierarchy Definition
      struct CompanyExceptions : Utilities::AbstractException<> { using AbstractException::AbstractException; };  // Class Address exception base class

      // Constructors and Destructor
      Company             (                      )          = default;
      Company             ( const Company &  rhs )          = default;
      Company             (       Company && rhs )          = default;
      Company & operator= ( const Company &  rhs )          = default;
      Company & operator= (       Company && rhs )          = default;
     ~Company             (                      ) noexcept = default;

      // Company headquarters is located at the address with this key.
      Company( std::string name );


      // Queries
      std::string name() const noexcept;


      // Conversions
      explicit operator std::string() const;


      // Modifiers
      Company & name( std::string newName )  noexcept;




    private:
      // Instance attribute (aka object state attributes)
      std::string                  _name;


      // Class attributes
      static constexpr char FIELD_SEPARATOR  = '\x03';  // ETX (End of Text) character
      static constexpr char RECORD_SEPARATOR = '\x04';  // EOT (End of Transmission) character ('\n' turns out to not be a good choice because data
                                                        //     files are not then portable between Windows, Unix, and Mac OSs.)
  };  // class Company






  // Non-member functions
  bool operator==(const Company & lhs, const Company & rhs);
  bool operator< (const Company & lhs, const Company & rhs);
  bool operator!=(const Company & lhs, const Company & rhs);
  bool operator> (const Company & lhs, const Company & rhs);
  bool operator<=(const Company & lhs, const Company & rhs);
  bool operator>=(const Company & lhs, const Company & rhs);

  std::ostream & operator<< (std::ostream & s, const Company & company);
  std::istream & operator>> (std::istream & s,       Company & company);
  std::ostream & operator<< (std::ostream & s, const Company * company);
  std::istream & operator>> (std::istream & s,       Company * company);
} // namespace Companies
#endif
