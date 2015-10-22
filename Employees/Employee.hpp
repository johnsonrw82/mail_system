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


#ifndef EMPLOYEES_Employee_hpp
#define EMPLOYEES_Employee_hpp

#include <iostream>
#include <string>

#include "Utilities/Exceptions.hpp"



namespace Employees
{
  class Employee
  {
    friend std::ostream & operator<< (std::ostream & s, const Employee & employee);
    friend std::istream & operator>> (std::istream & s,       Employee & employee);

    friend bool operator==(const Employee & lhs, const Employee & rhs);
    friend bool operator< (const Employee & lhs, const Employee & rhs);




    public:
      // Inner Exception Type Hierarchy Definition
      struct EmployeeExceptions       : Utilities::AbstractException<> { using AbstractException::AbstractException; };  // Class Address exception base class


      // Constructors and Destructor
      Employee             (                       )          = default;
      Employee             ( const Employee &  rhs )          = default;
      Employee             (       Employee && rhs )          = default;
      Employee & operator= ( const Employee &  rhs )          = default;
      Employee & operator= (       Employee && rhs )          = default;
     ~Employee             (                       ) noexcept = default;

      Employee( const std::string & name );                              // last name [, first name]
      Employee( std::string firstName, std::string lastName ) noexcept;


      // Queries
      std::string name()        const;
      std::string firstName()   const;
      std::string lastName()    const;


      // Conversions
      explicit operator std::string() const;


      // Modifiers
      Employee & name      ( const std::string & newName )  noexcept;    // last name [, first name]
      Employee & firstName (       std::string   newName )  noexcept;
      Employee & lastName  (       std::string   newName )  noexcept;




    private:
      // Instance attribute (aka object state attributes)
      std::string   _firstName;
      std::string   _lastName;


      // Class attributes
      static constexpr char FIELD_SEPARATOR  = '\x03';  // ETX (End of Text) character
      static constexpr char RECORD_SEPARATOR = '\x04';  // EOT (End of Transmission) character ('\n' turns out to not be a good choice because data
                                                        //     files are not then portable between Windows, Unix, and Mac OSs.)
  };  // class Employee






  // Non-member functions
  bool operator==(const Employee & lhs, const Employee & rhs);
  bool operator< (const Employee & lhs, const Employee & rhs);
  bool operator!=(const Employee & lhs, const Employee & rhs);
  bool operator> (const Employee & lhs, const Employee & rhs);
  bool operator<=(const Employee & lhs, const Employee & rhs);
  bool operator>=(const Employee & lhs, const Employee & rhs);

  std::ostream & operator<< (std::ostream & s, const Employee & employee);
  std::istream & operator>> (std::istream & s, Employee & employee);
  std::ostream & operator<< (std::ostream & s, const Employee * employee);
  std::istream & operator>> (std::istream & s,       Employee * employee);
} // namespace Employees

#endif
