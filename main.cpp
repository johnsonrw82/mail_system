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


#include <algorithm>
#include <exception>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Addresses/Address.hpp"
#include "Companies/Company.hpp"
#include "Employees/Employee.hpp"
#include "Utilities/Exceptions.hpp"
#include "Utilities/Associations.hpp"



namespace // unnamed, anonymous namespace for internal (local) linkage
{
  struct RegressionTestException  : Utilities::AbstractException<> { using AbstractException::AbstractException; };              // Regression test exception base class
  struct   UndetectedException    : RegressionTestException        { using RegressionTestException::RegressionTestException; };  // Inherit base class constructors
  struct   RelationalTestFailure  : RegressionTestException        { using RegressionTestException::RegressionTestException; };
  struct   SemmetricalIOFailure   : RegressionTestException        { using RegressionTestException::RegressionTestException; };
  struct   PropertyValueException : RegressionTestException { using RegressionTestException::RegressionTestException; };         // new exception used to mark when a property is incorrectly set

  /****************************************************************************
  ** Address Verification & Regression Test
  ****************************************************************************/
  void runAddressTest()
  {
    using namespace Addresses;

    std::vector<Address> properties =
    {
      {"157 S. Howard Street", "Spokane", "WA", 99201UL},            // Ralph's/Kroger
      {"1014 Vine Street", "Cincinnati", "Ohio", "45202-1100"},      // Albertson's
      {"1313 S. Harbor Boulevard", "Anaheim", "CA", "92803-1313"},   // Disneyland
      {}
    };

    // Modify the property by chaining the update functions together
    properties[3].street( "8039 Beach Boulevard" ).city( "Buena Park" ).state( "CA" ).zipCode( 90620 );        // Knotts Berry Farm
    for( const auto & property : properties )  std::cout << property << '\n';
    std::cout << '\n';

    // verify a couple logical operators by sorting
    std::sort( std::begin( properties ), std::end( properties ), Addresses::operator< );
    for( const auto & property : properties )  std::cout << property << '\n';
    std::cout << '\n';

    std::sort( std::begin( properties ), std::end( properties ), Addresses::operator> );
    for( const auto & property : properties )  std::cout << property << '\n';

    // Verify state codes
    try
    {
      properties[1].state( "CSUF" );
      throw UndetectedException( "Undetected Wrong State or State Code", __LINE__, __func__, __FILE__ );
    }
    catch( Address::StateCodeException &) {} // catch and ignore expected exception type, let all other propagate up

	// verify nothing was changed
	if (properties[1].state() != "Ohio") {
		throw PropertyValueException("State should not have been modified", __LINE__, __func__, __FILE__);
	}

	// verify abbreviation check
	try {
		properties[1].state("WS");
		throw UndetectedException("Undetected Wrong State or State Code", __LINE__, __func__, __FILE__);
	}
	catch (Address::StateCodeException &) {}

	// verify correct state setting
	try {
		properties[2].state("WI");  // wisconsin
		// check if set properly
		if (properties[2].state() != "Wisconsin") {
			throw PropertyValueException("State name should have changed", __LINE__, __func__, __FILE__);
		}
	}
	catch (Address::StateCodeException & ex) {
		throw PropertyValueException(ex, "The state code is valid", __LINE__, __func__, __FILE__);
	}

	// verify correct state name
	try {
		properties[2].state("District of Columbia");  // DC
		// check if set properly
		if (properties[2].state() != "District of Columbia") {
			throw PropertyValueException("State name should have changed", __LINE__, __func__, __FILE__);
		}
	}
	catch (Address::StateCodeException & ex) {
		throw PropertyValueException(ex, "The state name is valid", __LINE__, __func__, __FILE__);
	}

	try {
		properties[2].state("Wa");  // correct abbreviation, wrong case
		// check if set properly
		if (properties[2].state() != "Washington") {
			throw PropertyValueException("State name should have changed", __LINE__, __func__, __FILE__);
		}
	}
	catch (Address::StateCodeException & ex) {
		throw PropertyValueException(ex, "The state name is valid", __LINE__, __func__, __FILE__);
	}

	try {
		properties[2].state("alabama");  // AL
		 // check if set properly
		if (properties[2].state() != "Alabama") {
			throw PropertyValueException("State name should have changed", __LINE__, __func__, __FILE__);
		}
	}
	catch (Address::StateCodeException & ex) {
		throw PropertyValueException(ex, "The state name is valid", __LINE__, __func__, __FILE__);
	}

    // verify zip codes
	// valid
	try {
		properties[0].zipCode(12);
		properties[0].zipCode(12345UL);
		properties[0].zipCode("12345-1234");
	}
	catch (Address::ZipCodeException & ex) {
		throw PropertyValueException(ex, "Zip code is valid", __LINE__, __func__, __FILE__);
	}
    

	// invalid
    try
    {
      properties[0].zipCode("ABC");
      throw UndetectedException("Undetected Wrong Zip Code", __LINE__, __func__, __FILE__);
    }
    catch (Address::ZipCodeException &) {} // catch and ignore expected exception type, let all other propagate up

    try
    {
      properties[0].zipCode( "00000" );
      throw UndetectedException( "Undetected Wrong Zip Code", __LINE__, __func__, __FILE__ );
    }
    catch( Address::ZipCodeException & ) {} // catch and ignore expected exception type, let all other propagate up
    
    try
    {
      properties[0].zipCode( "99999" );
      throw UndetectedException( "Undetected Wrong Zip Code", __LINE__, __func__, __FILE__ );
    }
    catch( Address::ZipCodeException & ) {} // catch and ignore expected exception type, let all other propagate up
    
    try
    {
      properties[0].zipCode( "12345-0000" );
      throw UndetectedException( "Undetected Wrong Zip Code", __LINE__, __func__, __FILE__ );
    }
    catch( Address::ZipCodeException & ) {} // catch and ignore expected exception type, let all other propagate up
    
    try
    {
      properties[0].zipCode( "12345-9999" );
      throw UndetectedException( "Undetected Wrong Zip Code", __LINE__, __func__, __FILE__ );
    }
    catch( Address::ZipCodeException & ) {} // catch and ignore expected exception type, let all other propagate up

	try
	{
		properties[0].zipCode("12345-12345");
		throw UndetectedException("Undetected Wrong Zip Code", __LINE__, __func__, __FILE__);
	}
	catch (Address::ZipCodeException &) {} // catch and ignore expected exception type, let all other propagate up

    // verify some equality checks
    if( properties[0] != properties[0] )  throw RelationalTestFailure( "Equality Failure", __LINE__, __func__, __FILE__ );
	if( properties[1] == properties[0] ) throw RelationalTestFailure("Equality Failure", __LINE__, __func__, __FILE__);
	if (!(properties[1] <= properties[1])) throw RelationalTestFailure("Equality Failure", __LINE__, __func__, __FILE__);
	if (!(properties[1] >= properties[1])) throw RelationalTestFailure("Equality Failure", __LINE__, __func__, __FILE__);

    // symmetric insertion and extraction checks
    {
      std::stringstream s;
      s << properties[2];

      Address temp;
      s >> temp;

      if( properties[2] != temp )  throw SemmetricalIOFailure( "Symmetric Insertion/Extraction Failure", __LINE__, __func__, __FILE__ );
    }

    // symmetric insertion and extraction checks with casting
    {
      Address temp;
      std::istringstream( static_cast<std::string>(properties[1]) ) >> temp;

      if (properties[1] != temp)  throw SemmetricalIOFailure("Symmetric Insertion/Extraction Failure", __LINE__, __func__, __FILE__);
    }

    // pointer stream verification
	std::stringstream ss;
	Address * addrPtr1 = new Address();

	// initialize a stream with good data
	ss << properties[2];
	// use pointer
	ss >> addrPtr1;

	// verify equality
	if (*addrPtr1 != properties[2]) {
		throw RelationalTestFailure("Pointer was not initialized properly", __LINE__, __func__, __FILE__);
	}

	// now buffer out to another pointer
	Address * addrPtr2 = new Address();
	ss.seekg(0);     // reset stream
	ss << addrPtr1;  // buffer out
	ss >> addrPtr2;  // buffer in

	// test
	if (*addrPtr1 != *addrPtr2) {
		throw SemmetricalIOFailure("Pointer symmetric insertion/extraction failure", __LINE__, __func__, __FILE__);
	}

	// clean up
	delete addrPtr1;
	delete addrPtr2;

    std::cout << "Success:  " << __func__ << "\v\n";
  } // void runAddressTest()




  /****************************************************************************
  ** Company Verification & Regression Test
  ****************************************************************************/
  void runCompanyTest()
  {
    using namespace Companies;

    std::vector<Company> companies =
    {
      {"The Kroger Co"},
      {"Albertson's, Inc"},
      {"The Walt Disney Company - Disneyland"},
      {"Cedar Fair Entertainment Company - Knott's Berry Farm"},
      {}
    };

    // Modify the property by chaining the update functions together
    companies[4].name( "CSUF UEE" );
    for( const auto & company : companies )  std::cout << company << '\n';
    std::cout << '\n';

    // verify a couple logical operators by sorting
    std::sort( std::begin( companies ), std::end( companies ), Companies::operator< );
    for( const auto & company : companies )  std::cout << company << '\n';
    std::cout << '\n';

    std::sort( std::begin( companies ), std::end( companies ), Companies::operator> );
    for( const auto & company : companies )  std::cout << company << '\n';

	// test setting name
	companies[1].name("The ABC Company");
	if (companies[1].name() != "The ABC Company") {
		throw RegressionTestException("Company name not set properly", __LINE__, __func__, __FILE__);
	}

    // verify some equality checks
    if( companies[0] != companies[0] )  throw RelationalTestFailure( "Equality Failure", __LINE__, __func__, __FILE__ );
	if (companies[1] == companies[0])  throw RelationalTestFailure("Equality Failure", __LINE__, __func__, __FILE__);
	if (!(companies[0] <= companies[0]))  throw RelationalTestFailure("Equality Failure", __LINE__, __func__, __FILE__);
	if (!(companies[0] >= companies[0]))  throw RelationalTestFailure("Equality Failure", __LINE__, __func__, __FILE__);

    // symmetric insertion and extraction checks
    {
      std::stringstream s;
      s << companies[2];

      Company temp;
      s >> temp;

      if( companies[2] != temp )  throw SemmetricalIOFailure( "Symmetric Insertion/Extraction Failure", __LINE__, __func__, __FILE__ );
    }
    
    // symmetric insertion and extraction checks with casting
    {
      Company temp;
      std::istringstream(static_cast<std::string>(companies[1])) >> temp;

      if (companies[1] != temp)  throw SemmetricalIOFailure("Symmetric Insertion/Extraction Failure", __LINE__, __func__, __FILE__);
    }

	// pointer stream verification
	std::stringstream ss;
	Company * compPtr1 = new Company();

	// initialize a stream with good data
	ss << companies[2];
	// use pointer
	ss >> compPtr1;

	// verify equality
	if (*compPtr1 != companies[2]) {
		throw RelationalTestFailure("Pointer was not initialized properly", __LINE__, __func__, __FILE__);
	}

	// now buffer out to another pointer
	Company * compPtr2 = new Company();
	ss.seekg(0);     // reset stream
	ss << compPtr1;  // buffer out
	ss >> compPtr2;  // buffer in

	// test
	if (*compPtr1 != *compPtr2) {
		throw SemmetricalIOFailure("Pointer symmetric insertion/extraction failure", __LINE__, __func__, __FILE__);
	}

	// clean up
	delete compPtr1;
	delete compPtr2;

    std::cout << "Success:  " << __func__ << "\v\n";
  }  // void runCompanyTest()



  /****************************************************************************
  ** Employee Verification & Regression Test
  ****************************************************************************/
  void runEmployeeTest()
  {
    using namespace Employees;

    std::vector<Employee> workers =
    {
      {"Tom", "Bettens"},
      {"Disney, Walt"},
      {"Bjarne", "Stroustrup"},
      {"Skywalker"},
      {}
    };

    // Modify the property by chaining the update functions together
    workers[4].name( "Holmes, Sherlock" );
    for( const auto & worker : workers )  std::cout << worker << '\n';
    std::cout << '\n';


    // verify a couple logical operators by sorting
    std::sort( std::begin( workers ), std::end( workers ), Employees::operator< );
    for( const auto & worker : workers )  std::cout << worker << '\n';
    std::cout << '\n';

    std::sort( std::begin( workers ), std::end( workers ), Employees::operator> );
    for( const auto & worker : workers )  std::cout << worker << '\n';


	// check setting names
	workers[1].name("Smith");
	// verify first and last name
	if (workers[1].firstName() != "" && workers[1].lastName() != "Smith") {
		throw PropertyValueException("Name was not set properly", __LINE__, __func__, __FILE__);
	}
	workers[1].lastName("Jacob").firstName("John");
	// verify first and last name
	if (workers[1].firstName() != "John" && workers[1].lastName() != "Jacob") {
		throw PropertyValueException("Name was not set properly", __LINE__, __func__, __FILE__);
	}
	workers[1].lastName("Clark, Samuel"); // this is an invalid format for last name, 
										  // but should set the name properly by calling the appropriate modifier under the hood
	// verify first and last name
	if (workers[1].firstName() != "Samuel" && workers[1].lastName() != "Clark") {
		throw PropertyValueException("Name was not set properly", __LINE__, __func__, __FILE__);
	}

    // verify some equality checks
    if( workers[0] != workers[0] )  throw RelationalTestFailure( "Equality Failure", __LINE__, __func__, __FILE__ );
	if (workers[1] == workers[0])  throw RelationalTestFailure("Equality Failure", __LINE__, __func__, __FILE__);
	if (!(workers[0] <= workers[0]))  throw RelationalTestFailure("Equality Failure", __LINE__, __func__, __FILE__);
	if (!(workers[0] >= workers[0]))  throw RelationalTestFailure("Equality Failure", __LINE__, __func__, __FILE__);

    // symmetric insertion and extraction checks
    {
      std::stringstream s;
      s << workers[2];

      Employee temp;
      s >> temp;

      if( workers[2] != temp )  throw SemmetricalIOFailure( "Symmetric Insertion/Extraction Failure", __LINE__, __func__, __FILE__ );
    }

    // symmetric insertion and extraction checks with casting
    {
      Employee temp;
      std::istringstream(static_cast<std::string>(workers[1])) >> temp;

      if (workers[1] != temp)  throw SemmetricalIOFailure("Symmetric Insertion/Extraction Failure", __LINE__, __func__, __FILE__);
    }

	// pointer stream verification
	std::stringstream ss;
	Employee * empPtr1 = new Employee();

	// initialize a stream with good data
	ss << workers[2];
	// use pointer
	ss >> empPtr1;

	// verify equality
	if (*empPtr1 != workers[2]) {
		throw RelationalTestFailure("Pointer was not initialized properly", __LINE__, __func__, __FILE__);
	}

	// now buffer out to another pointer
	Employee * empPtr2 = new Employee();
	ss.seekg(0);     // reset stream
	ss << empPtr1;  // buffer out
	ss >> empPtr2;  // buffer in

	// test
	if (*empPtr1 != *empPtr2) {
		throw SemmetricalIOFailure("Pointer symmetric insertion/extraction failure", __LINE__, __func__, __FILE__);
	}

	// clean up
	delete empPtr1;
	delete empPtr2;

    std::cout << "Success:  " << __func__ << "\v\n";
  }  //  void runEmployeeTest() 

}// unnamed, anonymous namespace

int main()
{
  try
  {
    const auto seperator = std::string( 80, '=' );
	
    ::runAddressTest();
    std::cout << seperator << '\n';

    ::runCompanyTest();
    std::cout << seperator << '\n';

    ::runEmployeeTest();
    std::cout << seperator << '\n';

	Companies::Company c1, c2;
	Employees::Employee e1, e2;

	Utilities::Associations<Companies::Company, Employees::Employee, Utilities::One2OnePolicy> EmpComp;

	c1.name("Company 1");
	c2.name("Company 2");
	e1.name("Smith, John");
	e2.name("Hines, Gregory");

	EmpComp.insert(c1, e2);
	EmpComp.insert(c2, e2);

	Utilities::Associations<Companies::Company, Employees::Employee, Utilities::One2OnePolicy>::LeftCollectionType comps;
	comps = EmpComp.retrieveLeft();

    std::cout << "Success:  " << __func__ << "\v\n";
  }
  catch( std::exception & ex )
  {
    std::cerr << "\n\nFailure - \n" << ex.what() << "\n\n";
  }
}
