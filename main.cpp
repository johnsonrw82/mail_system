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
#include <type_traits>
#include <set>
#include <list>

#include "Addresses/Address.hpp"
#include "Companies/Company.hpp"
#include "Employees/Employee.hpp"
#include "Utilities/Exceptions.hpp"
#include "Utilities/Associations.hpp"

#include "Common/AddressCompanyDB.hpp"
#include "Common/EmployeeCompanyDB.hpp"
#include "Common/EmployeeAddressDB.hpp"
#include "Common/EmployeeMessageDB.hpp"

#include "MailSystem/Message.hpp"
#include "MailSystem/Document.hpp"
#include "MailSystem/MessageDB.hpp"



namespace // unnamed, anonymous namespace for internal (local) linkage
{
  struct RegressionTestException  : Utilities::AbstractException<> { using AbstractException::AbstractException; };              // Regression test exception base class
  struct   UndetectedException    : RegressionTestException        { using RegressionTestException::RegressionTestException; };  // Inherit base class constructors
  struct   RelationalTestFailure  : RegressionTestException        { using RegressionTestException::RegressionTestException; };
  struct   SemmetricalIOFailure   : RegressionTestException        { using RegressionTestException::RegressionTestException; };
  struct   PropertyValueException : RegressionTestException { using RegressionTestException::RegressionTestException; };         // new exception used to mark when a property is incorrectly set
  struct   InvalidStateException : RegressionTestException { using RegressionTestException::RegressionTestException; };  // used when the object is in an invalid state

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

  // test Associations by object
  void testObjectAssociations() 
  {
	  using namespace std::string_literals;

	  // create some objects
	  Companies::Company homeDepot, safeway, microsoft;
	  Employees::Employee employee1, employee2;
	  Addresses::Address headquarters1, headquarters2, headquarters3;

	  // set up each object
	  // give the companies names
	  homeDepot.name("The Home Depot");
	  safeway.name("Safeway Companies");
	  microsoft.name("Microsoft Corporation");

	  // addresses
	  headquarters1.city("Atlanta").state("GA").street("2600 Cumberland Pkwy SE").zipCode("30339");
	  headquarters2.city("Pleasanton").state("California").street("5918 Stoneridge Mall Rd").zipCode("94588");
	  headquarters3.city("Redmond").state("WA").street("One Microsoft Way").zipCode("98052");

	  // people
	  employee1.name("Smith, Peter");
	  employee2.firstName("Bill").lastName("Gates");

	  // association types/policies
	  using AddressCompany = Utilities::Associations<Addresses::Address, Companies::Company>;
	  using EmployeeAddress = Utilities::Associations<Employees::Employee, Addresses::Address, Utilities::Many2OnePolicy>;
	  using EmployeeCompany = Utilities::Associations<Employees::Employee, Companies::Company>;

	  // association objects
	  AddressCompany companyAddresses;
	  EmployeeAddress employeeDirectory;
	  EmployeeCompany employeeCompanies;

	  // set up the mappings for companies and addresses
	  companyAddresses.insert(headquarters1, homeDepot); // home depot's corporate headquarters
	  companyAddresses.insert(headquarters2, safeway); // safeway's corporate headquarters
	  companyAddresses.insert(headquarters3, microsoft); // microsoft's headquarters

	  // verify insert worked
	  if (companyAddresses.size() != 3UL) {
		  throw InvalidStateException("Size is not the expected value", __LINE__, __func__, __FILE__);
	  }

	  // try to remove a company (we'll add it right back...)
	  companyAddresses.remove(headquarters1, homeDepot);
	  
	  // verify remove worked
	  if (companyAddresses.size() != 2UL) {
		  throw InvalidStateException("Size is not the expected value", __LINE__, __func__, __FILE__);
	  }

	  // clean up after that last remove
	  companyAddresses.insert(headquarters1, homeDepot); // home depot's corporate headquarters

	  // try to remove a record that doesn't exist
	  companyAddresses.remove(headquarters2, homeDepot);
	  // verify remove was a no-op
	  if (companyAddresses.size() != 3UL) {
		  throw InvalidStateException("Size is not the expected value", __LINE__, __func__, __FILE__);
	  }

	  // add some store locations
	  AddressCompany::LeftCollectionType storeLocations = {
		  {"3350 N Woodlawn", "Wichita", "KS", 67226UL},
		  {"1740 Fleischli Pkwy", "Cheyenne", "WY", 82001UL},
		  {"10120 SE Washington St", "Portland", "OR", 97216UL}
	  };
	  companyAddresses.insert(storeLocations, homeDepot);

	  // safeway locations
	  companyAddresses.insert({
		  { "1110 W Alameda Ave"s, "Burbank"s, "CA"s, "91506"s },
		  { "6194 SW Murray Blvd"s, "Beaverton"s, "OR"s, "97008"s }
	  }, safeway);

	  // employee addresses
	  Addresses::Address billsHouse("1835 73rd Ave NE", "Medina", "WA", "98039");
	  employeeDirectory.insert(employee2, billsHouse); // bill gates' house
	  employeeDirectory.insert(employee1, { "1245 Fake St"s, "Austin"s, "TX"s, 78701UL });
	  // Bill's wife also likely lives with him
	  employeeDirectory.insert(EmployeeAddress::LeftCollectionType({ "Gates, Melinda"s }), billsHouse);

	  // try to put another employee in another house (constraint violation)
	  try {
		  employeeDirectory.insert(employee1, billsHouse); // this employee already has an address
		  throw UndetectedException("Only one address is allowed per employee", __LINE__, __func__, __FILE__);
	  }
	  catch (Utilities::PolicyConstraintException) {}

	  // set up worker locations
	  employeeCompanies.insert(employee2, microsoft); // bill gates (used to -- but he's still involved) works for microsoft
	  employeeCompanies.insert(employee1, companyAddresses.retrieveRight()); // peter works everywhere
	  employeeCompanies.insert(EmployeeCompany::LeftCollectionType({"Jacobs, Brian"s}), homeDepot); // brian will work for home depot

	  // safeway employees
	  employeeCompanies.insert(EmployeeCompany::LeftCollectionType({
		  { "Jones, Fred"s },
		  { "Anderson, Mildred" },
		  { "Burd, Steve" },
	  }), safeway);

	  // they have to live somewhere...
	  employeeDirectory.insert(EmployeeAddress::LeftCollectionType({ "Jones, Fred"s }) , { "65 Walnut Ave"s, "Beaverton"s, "OR"s, 97008UL });
	  employeeDirectory.insert(EmployeeAddress::LeftCollectionType({ "Burd, Steve"s }) , { "189 Fern Hill Road"s, "Pleasanton"s, "CA"s, 94588UL });
	  employeeDirectory.insert(EmployeeAddress::LeftCollectionType({ "Anderson, Mildred"s }) , { "23456 SW 18th St"s, "Cheyenne"s, "WY"s, "82001"s });

	  // make a copy and test the complete removal of these people (test firing)
	  EmployeeCompany employeeCompaniesCopy = employeeCompanies;
	  employeeCompaniesCopy.removeRight(safeway);

	  // verify that company no longer has anyone working for it
	  if (employeeCompaniesCopy.retrieveLeft(safeway).size() != 0 ) {
		  throw InvalidStateException("Collection should be empty", __LINE__, __func__, __FILE__);
	  }

	  // reset and try the other removal -- this time, Peter got fired from all his jobs -- 
	  // something about going crazy and stealing all the office supplies -- it's just a rumor though, so we test it out
	  employeeCompaniesCopy = employeeCompanies;
	  employeeCompaniesCopy.removeLeft(employee1);

	  // verify peter has been laid off
	  if (employeeCompaniesCopy.retrieveRight(employee1).size() != 0) {
		  throw InvalidStateException("Collection should be empty", __LINE__, __func__, __FILE__);
	  }

	  // now, lets test everyone getting the boot
	  employeeCompaniesCopy = employeeCompanies;
	  employeeCompaniesCopy.clear();

	  // verify nobody works anywhere
	  if (employeeCompaniesCopy.size() != 0UL) {
		  throw InvalidStateException("Associations map should be empty", __LINE__, __func__, __FILE__);
	  }

	  // print out the employee addresses
	  std::cout << "Employee Directory:\n" << employeeDirectory << '\n';
	  // print out the company directory
	  std::cout << "\nCompany Directory:\n" << companyAddresses << '\n';
	  // print out the employee company map
	  std::cout << "\nEmployee Companies:\n" << employeeCompanies << '\n';

	  // verify self equality
	  if (employeeDirectory != employeeDirectory) {
		  throw RelationalTestFailure("Equality failure", __LINE__, __func__, __FILE__);
	  }

	  // Make a copy and test equality
	  AddressCompany tmp = companyAddresses;
	  if (tmp != companyAddresses) {
		  throw RelationalTestFailure("Equality failure", __LINE__, __func__, __FILE__);
	  }

	  // stream test
	  std::stringstream ss;
	  ss << employeeCompanies;

	  EmployeeCompany tmp2;
	  ss >> tmp2;

	  // make sure the objects are equal
	  if (employeeCompanies != tmp2) {
		  throw SemmetricalIOFailure("Symmetric I/O insertion/extraction failure", __LINE__, __func__, __FILE__);
	  }

	  // run some queries
	  // where does bill gates live?
	  std::cout << employee2 << " lives at: " << *(employeeDirectory.retrieveRight(employee2).cbegin()) << '\n';
	  // who lives at his house?
	  std::cout << "\nThese people live at: " << billsHouse << '\n';
	  for (const auto & e : employeeDirectory.retrieveLeft(billsHouse)) {
		  std::cout << '\t' << e << '\n';
	  }
	  
	  // where does Peter work?
	  std::cout << '\n' << employee1 << " works at:\n";
	  for (const auto & c : employeeCompanies.retrieveRight(employee1)) {
		  std::cout << '\t' << c.name() << '\n';
	  }

	  // where are safeway's locations?
	  std::cout << '\n' << safeway.name() << " has locations at:\n";
	  for (const auto & a : companyAddresses.retrieveLeft(safeway)) {
		  std::cout << '\t' << a << '\n';
	  }

	  // who works at the home depot?
	  std::cout << '\n' << "These people work at " << homeDepot.name() << ":\n";
	  for (const auto & e : employeeCompanies.retrieveLeft(homeDepot)) {
		  std::cout << '\t' << e << '\n';
	  }

	  // who works at Safeway and where do they live?
	  std::cout << '\n' << "The names and addresses of the people working at " << safeway.name() << " are:\n";
	  for (const auto & e : employeeCompanies.retrieveLeft(safeway)) {
		  std::cout << '\t' << e << " lives at: " << *(employeeDirectory.retrieveRight(e).cbegin()) << '\n';
	  }

	  // success
	  std::cout << "Success:  " << __func__ << "\v\n";
  }

  // test associations using pointer-to-object types
  void testPointerAssociations() {
	  using namespace std::string_literals;

	  // create some objects
	  Companies::Company homeDepot, safeway, microsoft;
	  Employees::Employee employee1, employee2;
	  Addresses::Address headquarters1, headquarters2, headquarters3;

	  // set up each object
	  // give the companies names
	  homeDepot.name("The Home Depot");
	  safeway.name("Safeway Companies");
	  microsoft.name("Microsoft Corporation");

	  // addresses
	  headquarters1.city("Atlanta").state("GA").street("2600 Cumberland Pkwy SE").zipCode("30339");
	  headquarters2.city("Pleasanton").state("California").street("5918 Stoneridge Mall Rd").zipCode("94588");
	  headquarters3.city("Redmond").state("WA").street("One Microsoft Way").zipCode("98052");

	  // people
	  employee1.name("Smith, Peter");
	  employee2.firstName("Bill").lastName("Gates");

	  // association types/policies
	  using AddressCompany = Utilities::Associations<Addresses::Address*, Companies::Company*>;
	  using EmployeeAddress = Utilities::Associations<Employees::Employee*, Addresses::Address*, Utilities::Many2OnePolicy>;
	  using EmployeeCompany = Utilities::Associations<Employees::Employee*, Companies::Company*>;

	  // association objects
	  AddressCompany companyAddresses;
	  EmployeeAddress employeeDirectory;
	  EmployeeCompany employeeCompanies;

	  // set up the mappings for companies and addresses
	  companyAddresses.insert(&headquarters1, &homeDepot); // home depot's corporate headquarters
	  companyAddresses.insert(&headquarters2, &safeway); // safeway's corporate headquarters
	  companyAddresses.insert(&headquarters3, &microsoft); // microsoft's headquarters

	  // verify insert worked
	  if (companyAddresses.size() != 3UL) {
		  throw InvalidStateException("Size is not the expected value", __LINE__, __func__, __FILE__);
	  }

	  // add some store locations
	  Addresses::Address hd1("3350 N Woodlawn", "Wichita", "KS", 67226UL);
	  Addresses::Address hd2("1740 Fleischli Pkwy", "Cheyenne", "WY", 82001UL);
	  Addresses::Address hd3("10120 SE Washington St", "Portland", "OR", 97216UL);
	  
	  // insert store locations
	  companyAddresses.insert(&hd1, &homeDepot);
	  companyAddresses.insert(&hd2, &homeDepot);
	  companyAddresses.insert(&hd3, &homeDepot);
	  
	  // safeway locations
	  Addresses::Address sw1("1110 W Alameda Ave", "Burbank", "CA", "91506");
	  Addresses::Address sw2("6194 SW Murray Blvd", "Beaverton", "OR", "97008");
	  
	  // insert store locations
	  companyAddresses.insert(&sw1, &safeway);
	  companyAddresses.insert(&sw2, &safeway);

	  // employee addresses
	  Addresses::Address billsHouse("1835 73rd Ave NE", "Medina", "WA", "98039");
	  employeeDirectory.insert(&employee2, &billsHouse); // bill gates' house
	  // Peter's house
	  Addresses::Address petersHouse("1245 Fake St", "Austin", "TX", 78701UL);
	  employeeDirectory.insert(&employee1, &petersHouse);
	  // Bill's wife also likely lives with him
	  Employees::Employee melinda("Gates, Melinda");
	  employeeDirectory.insert(&melinda, &billsHouse);

	  // try to put another employee in another house (constraint violation)
	  try {
		  employeeDirectory.insert(&employee1, &billsHouse); // this employee already has an address
		  throw UndetectedException("Only one address is allowed per employee", __LINE__, __func__, __FILE__);
	  }
	  catch (Utilities::PolicyConstraintException) {}

	  // set up worker locations
	  employeeCompanies.insert(&employee2, &microsoft); // bill gates (used to -- but he's still involved) works for microsoft
	  employeeCompanies.insert(&employee1, companyAddresses.retrieveRight()); // peter works everywhere
	  Employees::Employee brian("Jacobs, Brian");
	  employeeCompanies.insert(&brian, &homeDepot); // brian will work for home depot

			
	  // safeway employees
	  Employees::Employee fred("Jones, Fred");
	  Employees::Employee mildred("Anderson, Mildred");
	  Employees::Employee steve("Burd, Steve");
	  // they work for safeway
	  employeeCompanies.insert(&fred, &safeway);
	  employeeCompanies.insert(&mildred, &safeway);
	  employeeCompanies.insert(&steve, &safeway);

	  // they have to live somewhere...
	  Addresses::Address fredsHouse("65 Walnut Ave", "Beaverton", "OR", 97008UL);
	  Addresses::Address stevesHouse("189 Fern Hill Road", "Pleasanton", "CA", 94588UL);
	  Addresses::Address mildredsHouse("23456 SW 18th St", "Cheyenne", "WY", "82001");
	  employeeDirectory.insert(&fred, &fredsHouse);
	  employeeDirectory.insert(&steve, &stevesHouse);
	  employeeDirectory.insert(&mildred, &mildredsHouse);

	  // print out the employee addresses
	  std::cout << "Employee Directory:\n" << employeeDirectory << '\n';
	  // print out the company directory
	  std::cout << "\nCompany Directory:\n" << companyAddresses << '\n';
	  // print out the employee company map
	  std::cout << "\nEmployee Companies:\n" << employeeCompanies << '\n';

	  // verify self equality
	  if (employeeDirectory != employeeDirectory) {
		  throw RelationalTestFailure("Equality failure", __LINE__, __func__, __FILE__);
	  }
	  
	  // need function to help compare mapst containing pointers to objects
	  auto isEqual = [](const auto & ptr_lhs, const auto & rhs) {
		  // sizes need to at least be the same, but doesn't guarantee equality if they are the same
		  if (ptr_lhs.size() != rhs.size()) {
			  return false;
		  }

		  // compare keys of left and right side
		  auto leftKeys = ptr_lhs.retrieveLeft();
		  auto rightKeys = rhs.retrieveLeft();

		  // use std::isequal to compare the two collections
		  if (!std::equal(leftKeys.cbegin(), leftKeys.cend(), rightKeys.cbegin(), rightKeys.cend(),
			  [](const auto * const lhs, const auto & rhs) {
			      return *lhs == rhs;
		      })
		  ) {
			  return false;
		  }

		  // now check the right set against each left key
		  for (const auto & key : leftKeys) {
			  auto leftSet = ptr_lhs.retrieveRight(key);
			  auto rightSet = rhs.retrieveRight(*key);
			  if (!std::equal(leftSet.cbegin(), leftSet.cend(), rightSet.cbegin(), rightSet.cend(),
				  [](const auto * const lhs, const auto & rhs) {
				  return *lhs == rhs;
			      })
			  ) {
				  return false;
			  }
		  }

		  // the number of associations, the keys, and every association mapped to those keys are equal
		  return true;
	  };	  

	  // stream test
	  std::stringstream ss;
	  ss << employeeCompanies;

	  // >> not defined for pointers, so we can use a regular object type
	  Utilities::Associations<Employees::Employee, Companies::Company> tmp2;
	  ss >> tmp2;

	  // make sure the objects are equal
	  if (!isEqual(employeeCompanies,tmp2)) {
		  throw SemmetricalIOFailure("Symmetric I/O insertion/extraction failure", __LINE__, __func__, __FILE__);
	  }

	  // run some queries
	  // where does bill gates live?
	  std::cout << employee2 << " lives at: " << *(employeeDirectory.retrieveRight(&employee2).cbegin()) << '\n';
	  // who lives at his house?
	  std::cout << "\nThese people live at: " << billsHouse << '\n';
	  for (const auto & e : employeeDirectory.retrieveLeft(&billsHouse)) {
		  std::cout << '\t' << e << '\n';
	  }

	  // where does Peter work?
	  std::cout << '\n' << employee1 << " works at:\n";
	  for (const auto & c : employeeCompanies.retrieveRight(&employee1)) {
		  std::cout << '\t' << c->name() << '\n';
	  }

	  // where are safeway's locations?
	  std::cout << '\n' << safeway.name() << " has locations at:\n";
	  for (const auto & a : companyAddresses.retrieveLeft(&safeway)) {
		  std::cout << '\t' << a << '\n';
	  }

	  // who works at the home depot?
	  std::cout << '\n' << "These people work at " << homeDepot.name() << ":\n";
	  for (const auto & e : employeeCompanies.retrieveLeft(&homeDepot)) {
		  std::cout << '\t' << e << '\n';
	  }

	  // who works at Safeway and where do they live?
	  std::cout << '\n' << "The names and addresses of the people working at " << safeway.name() << " are:\n";
	  for (const auto & e : employeeCompanies.retrieveLeft(&safeway)) {
		  std::cout << '\t' << e << " lives at: " << *(employeeDirectory.retrieveRight(e).cbegin()) << '\n';
	  }

	  // success
	  std::cout << "Success:  " << __func__ << "\v\n";
  }

  // template function to get a database record (or a new insertion if not found)
  template <typename T>
  typename T::Value_t & getDatabaseRecord(typename T::Key dbKey) {
	  auto & database = T::getInstance();

	  // try to look up the record in the database
	  try {
		  return database[dbKey];
	  }
	  catch (Utilities::RecordLookupException &) {}

	  // if here, then the record wasn't found, let's make one and then return it
	  database.insert(dbKey, {});
	  return database[dbKey];
  }

  // template function to lookup a key by value
  template <typename T>
  typename T::Key findByRecordData(typename T::Value_t & recordData) {
	  // get database
	  auto & database = T::getInstance();

	  //  loop over the keys and if the employee is in there, return the key
	  for (auto & key : database.keys()) {
		  if (database[key] == recordData) {
			  return key;
		  }
	  }
	  // if not found, return the new key by inserting
	  return database.insert(recordData);
  }

  // set up and populate a database, then run some queries on the data
  void testDbOperations1() {
	  using namespace std::string_literals;

	  auto & aDB = Addresses::AddressDB::getInstance();
	  auto & cDB = Companies::CompanyDB::getInstance();
	  auto & eDB = Employees::EmployeeDB::getInstance();

	  // create some objects
	  Companies::Company homeDepot, safeway, microsoft;
	  Employees::Employee employee1, employee2;
	  Addresses::Address headquarters1, headquarters2, headquarters3;

	  // set up each object
	  // give the companies names
	  homeDepot.name("The Home Depot");
	  safeway.name("Safeway Companies");
	  microsoft.name("Microsoft Corporation");

	  // addresses
	  headquarters1.city("Atlanta").state("GA").street("2345 Miller Ave W").zipCode("30339");
	  headquarters2.city("Pleasanton").state("California").street("5918 Stoneridge Mall Rd").zipCode("94588");
	  headquarters3.city("Redmond").state("WA").street("One Microsoft Way").zipCode("98052");

	  // people
	  employee1.name("Smith, Peter");
	  employee2.firstName("Bill").lastName("Gates");

	  // insert them into their databases
	  // addresses
	  auto hq1Key = aDB.insert(headquarters1);
	  auto hq2Key = aDB.insert(headquarters2);
	  auto hq3Key = aDB.insert(headquarters3);

	  // change one of the addresses and re-insert at that key -- it was a mistaken entry
	  headquarters1.street("2600 Cumberland Pkwy SE");
	  aDB.insert(hq1Key, headquarters1);

	  // companies
	  auto homeDepotKey = cDB.insert(homeDepot);
	  auto safewayKey = cDB.insert(safeway);
	  auto microsoftKey = cDB.insert(microsoft);

	  // employees
	  auto peterKey = eDB.insert(employee1);
	  auto billKey = eDB.insert(employee2);

	  // insert into the address-company associations record
	  auto & acRecord = getDatabaseRecord<Common::AddressCompanyKeyDB>(0UL);

	  acRecord.insert(hq1Key, homeDepotKey); // home depot's corporate headquarters
	  acRecord.insert(hq2Key, safewayKey); // safeway's corporate headquarters
	  acRecord.insert(hq3Key, microsoftKey); // microsoft's headquarters

      // add some store locations
	  // home depot
	  std::vector<Addresses::Address> storeLocations = {
		  { "3350 N Woodlawn"s, "Wichita"s, "KS"s, 67226UL },
		  { "1740 Fleischli Pkwy"s, "Cheyenne"s, "WY"s, 82001UL },
		  { "10120 SE Washington St"s, "Portland"s, "OR"s, 97216UL }
	  };
	  for (auto & a : storeLocations) {
		  acRecord.insert(aDB.insert(a), homeDepotKey);
	  }
	  // safeway locations
	  storeLocations = {
		  { "1110 W Alameda Ave"s, "Burbank"s, "CA"s, "91506"s },
		  { "6194 SW Murray Blvd"s, "Beaverton"s, "OR"s, "97008"s }
	  };	  
	  for (auto & a : storeLocations) {
		  acRecord.insert(aDB.insert(a), safewayKey);
	  }

	  // insert into the employee-address associations record
	  auto & eaRecord = getDatabaseRecord<Common::EmployeeAddressKeyDB>(0UL);

	  Addresses::Address billsHouse("1835 73rd Ave NE", "Medina", "WA", "98039");
	  auto billsHouseKey = aDB.insert(billsHouse);
	  eaRecord.insert(billKey, billsHouseKey); // bill gates' house

	  // Peter's house
	  Addresses::Address petersHouse("1245 Fake St", "Austin", "TX", 78701UL);
	  auto petersHouseKey = aDB.insert(petersHouse);
	  eaRecord.insert(peterKey, petersHouseKey);

	  // Bill's wife also likely lives with him
	  Employees::Employee melinda("Gates, Melinda");
	  auto melindaKey = eDB.insert(melinda);
	  eaRecord.insert(melindaKey, billsHouseKey);	  
	 
	  // insert into the employee-company associations record
	  auto & ecRecord = getDatabaseRecord<Common::EmployeeCompanyKeyDB>(0UL);

	  ecRecord.insert(billKey, microsoftKey); // bill gates (used to -- but he's still involved) works for microsoft
	  ecRecord.insert(peterKey, acRecord.retrieveRight()); // peter works everywhere
	  Employees::Employee brian("Jacobs, Brian");
	  ecRecord.insert(eDB.insert(brian), homeDepotKey); // brian will work for home depot (we insert him inline...)

      // safeway employees
	  Employees::Employee fred("Jones, Fred");
	  Employees::Employee mildred("Anderson, Mildred");
	  Employees::Employee steve("Burd, Steve");
	  auto fredKey = eDB.insert(fred);
	  auto mildredKey = eDB.insert(mildred);
	  auto steveKey = eDB.insert(steve);

	  // they work for safeway
	  ecRecord.insert(fredKey, safewayKey);
	  ecRecord.insert(mildredKey, safewayKey);
	  ecRecord.insert(steveKey, safewayKey);

	  // they have to live somewhere...
	  Addresses::Address fredsHouse("65 Walnut Ave", "Beaverton", "OR", 97008UL);
	  Addresses::Address stevesHouse("189 Fern Hill Road", "Pleasanton", "CA", 94588UL);
	  Addresses::Address mildredsHouse("23456 SW 18th St", "Cheyenne", "WY", "82001");
	  eaRecord.insert(fredKey, aDB.insert(fredsHouse));
	  eaRecord.insert(steveKey, aDB.insert(stevesHouse));
	  eaRecord.insert(mildredKey, aDB.insert(mildredsHouse));

	  // now run some queries...
	  // where does bill gates live?
	  std::cout << '\n' << employee2 << " lives at: " << aDB[*eaRecord.retrieveRight(billKey).cbegin()] << '\n'; // get the entry at key pointed at by iterator
	  // who lives at his house?
	  std::cout << "\nThese people live at: " << billsHouse << '\n';
	  for (const auto & e : eaRecord.retrieveLeft(billsHouseKey)) {
		  std::cout << '\t' << eDB[e] << '\n';
	  }

	  // where does Peter work?
	  std::cout << '\n' << employee1 << " works at:\n";
	  for (const auto & c : ecRecord.retrieveRight(peterKey)) {
		  std::cout << '\t' << cDB[c].name() << '\n';
	  }

	  // where are safeway's locations?
	  std::cout << '\n' << safeway.name() << " has locations at:\n";
	  for (const auto & a : acRecord.retrieveLeft(safewayKey)) {
		  std::cout << '\t' << aDB[a] << '\n';
	  }

	  // who works at the home depot?
	  std::cout << '\n' << "These people work at " << homeDepot.name() << ":\n";
	  for (const auto & e : ecRecord.retrieveLeft(homeDepotKey)) {
		  std::cout << '\t' << eDB[e] << '\n';
	  }

	  // who works at Safeway and where do they live?
	  std::cout << '\n' << "The names and addresses of the people working at " << safeway.name() << " are:\n";
	  for (const auto & e : ecRecord.retrieveLeft(safewayKey)) {
		  std::cout << '\t' << eDB[e] << " lives at: " << aDB[*eaRecord.retrieveRight(e).cbegin()] << '\n';
	  }

	  // success
	  std::cout << "Success:  " << __func__ << "\v\n";
  }

  // test database operations on pre-existing data
  void testDbOperations2() {
	  using namespace std::string_literals;

	  auto & cDB = Companies::CompanyDB::getInstance();
	  auto & eDB = Employees::EmployeeDB::getInstance();

	  // lookup the record by using the key to identify it
	  auto & ecRecord = getDatabaseRecord<Common::EmployeeCompanyKeyDB>(0UL);

	  // lets pull all the employees out of the database -- tragically, they all died :'(
	  for (const auto & e : eDB.keys()) {
		  eDB.remove(e);
	  }

	  // verify the db is empty
	  if (eDB.size() != 0UL) {
		  throw InvalidStateException("Size is not the expected value", __LINE__, __func__, __FILE__);
	  }

	  // insert a new employee
	  auto simonKey = eDB.insert({ "Cooper, Simon" });

	  // lets make a new company and hire simon there
	  auto simonsCompanyKey = cDB.insert({ "Simon's Perfection Co." });
	  ecRecord.insert(simonKey, simonsCompanyKey);

	  // unfortunately, simon's time at that company was short lived
	  ecRecord.remove(simonKey, simonsCompanyKey);

	  // he did find some new companies to go work for though
	  auto c1Key = cDB.insert({ "Another Fabulous Company, Inc." });
	  auto c2Key = cDB.insert({ "This Place Isn't as Good, LLC" });
	  ecRecord.insert(simonKey, { c1Key, c2Key });

	  // let's now query where simon works
	  std::cout << '\n' << eDB[simonKey].name() << " now works for:\n";
	  for (const auto & c : ecRecord.retrieveRight(simonKey)) {
		  std::cout << '\t' << cDB[c] << '\n';
	  }

	  // success
	  std::cout << "Success:  " << __func__ << "\v\n";
  }

  // mail system test use case 1
  void testMailSystemUC1() {
	  auto & eDB = Employees::EmployeeDB::getInstance();
	  auto & mDB = MailSystem::MessageDB::getInstance();

	  // Use Case 1 - Tom, Sue, and Rob
	  // Tom sends a message with attachment to Sue and copies Rob
	  Employees::Employee tom = { "Tom" };
	  Employees::Employee sue = { "Sue" };
	  Employees::Employee rob = { "Rob" };

	  // find out if they are already in the database
	  auto findEmployee = [](Employees::Employee & emp)->Employees::EmployeeDB::Key {
		  return findByRecordData<Employees::EmployeeDB>(emp);
	  };

	  // keys for tom, sue, and rob
	  auto tomKey = findEmployee(tom);
	  auto sueKey = findEmployee(sue);
	  auto robKey = findEmployee(rob);

	  // construct the message - from, to, cc
	  MailSystem::Message fired({ { tomKey },{ sueKey }, {robKey} }, "Hello Sue,\nI just wanted to let you know that you're fired.\nSee attached performance report.\nI'm too chicken to do this in person, so you get an email. Sorry. :(\n");

	  // add attachment
	  MailSystem::Document performanceReport("Customer Relations = POOR\nEmployee Contributions = POOR\nProject Manangement = POOR\nHygeine = FAIR\n");
	  fired.addAttachment("Performance Report", performanceReport);

	  // send the message
	  fired.send();

	  // small lambda function to look up a mailbox
	  // built around a generic template function for looking up database records
	  auto getMailBox = [](auto mailboxKey)->Common::EmployeeMessageKeyDB::Value_t & {
		  return getDatabaseRecord<Common::EmployeeMessageKeyDB>(mailboxKey);
	  };

	  // get the sent/inbox database records
	  auto & sentBox = getMailBox(MailSystem::MailBoxes::SENT);
	  auto & inbox = getMailBox(MailSystem::MailBoxes::IN);

	  // loop through them and show the contents
	  for (const auto & emp : { tomKey, sueKey, robKey } ) {
		  // outbox
		  std::cout << eDB[emp].name() << "'s Sent Items:\n\n";
		  for (const auto & msg : sentBox.retrieveRight(emp)) {
			  std::cout << mDB[msg].body() << '\n';
			  std::cout << "With attachments:\n\n";
			  for (const auto & attachment : mDB[msg].attachments()) {
				  std::cout << "Attachment name: " << attachment << '\n' << mDB[msg].attachment(attachment) << '\n';
			  }
		  }

		  // inbox
		  std::cout << eDB[emp].name() << "'s Received Items:\n\n";
		  for (const auto & msg : inbox.retrieveRight(emp)) {
			  std::cout << mDB[msg].body() << '\n';
			  std::cout << "With attachments:\n\n";
			  for (const auto & attachment : mDB[msg].attachments()) {
				  std::cout << "Attachment name: " << attachment << '\n' << mDB[msg].attachment(attachment) << '\n';
			  }
		  }
	  }

	  // test symmetric sync
	  std::stringstream ss;
	  ss << fired;

	  MailSystem::Message temp;
	  ss >> temp;

	  if (fired != temp) {
		  throw SemmetricalIOFailure("Message symmetric IO failure", __LINE__, __func__, __FILE__);
	  }

	  // success
	  std::cout << "Success:  " << __func__ << "\v\n";
  }

  // mail system test use case 2
  void testMailSystemUC2() {
	  auto & mDB = MailSystem::MessageDB::getInstance();

	  // Use Case 2, tom decides to delete his message sent in the previous use case.
	  // the system admin then removes the message from the inboxes of the recipients. 
	  // He recalled his nasty message...
	  Employees::Employee tom = { "Tom" };
	  Employees::Employee sue = { "Sue" };
	  Employees::Employee rob = { "Rob" };

	  // find out if they are already in the database
	  auto findEmployee = [](Employees::Employee & emp)->Employees::EmployeeDB::Key {
		  return findByRecordData<Employees::EmployeeDB>(emp);
	  };

	  // keys for tom, sue, and rob
	  auto tomKey = findEmployee(tom);
	  auto sueKey = findEmployee(sue);
	  auto robKey = findEmployee(rob);

	  // small lambda function to look up a mailbox
	  // built around a generic template function for looking up database records
	  auto getMailBox = [](auto mailboxKey)->Common::EmployeeMessageKeyDB::Value_t & {
		  return getDatabaseRecord<Common::EmployeeMessageKeyDB>(mailboxKey);
	  };

	  // get the sent/inbox database records
	  auto & sentBox = getMailBox(MailSystem::MailBoxes::SENT);
	  auto & inbox = getMailBox(MailSystem::MailBoxes::IN);

	  // find the message ID from Tom's sent box -- assume it's his only sent item
	  auto tomsMessageKey = *(sentBox.retrieveRight(tomKey).cbegin());
	  std::size_t tomsSize = sentBox.retrieveRight(tomKey).size();

	  // display his message -- out to stdout
	  std::cout << "Tom's message:\n" << mDB[tomsMessageKey] << '\n';

	  // now he deletes it from his sent items
	  sentBox.remove(tomKey, tomsMessageKey);

	  // verify it's not there	  
	  if (sentBox.retrieveRight(tomKey).size() != tomsSize - 1) {
		  throw InvalidStateException("Message should have been removed from box.", __LINE__, __func__, __FILE__);
	  }

	  // now the system admin removes the message from the inbox of sue and rob
	  for (const auto & emp : { sueKey, robKey }) {
		  // get size
		  std::size_t boxSize = inbox.retrieveRight(emp).size();

		  inbox.remove(emp, tomsMessageKey);
		  // assert the size is reduced
		  if (inbox.retrieveRight(emp).size() != boxSize - 1) {
			  throw InvalidStateException("Message should have been removed from box.", __LINE__, __func__, __FILE__);
		  }
	  }

	  // now, the admin removes the message from the databse. as if it never happened
	  mDB.remove(tomsMessageKey);
	  
	  // assert it's not there now
	  try {
		  mDB[tomsMessageKey];
		  throw UndetectedException("Record lookup should have failed.", __LINE__, __func__, __FILE__);
	  }
	  catch (Utilities::RecordLookupException &) {}

	  // success
	  std::cout << "Success:  " << __func__ << "\v\n";
  }

  // use case 3 for mail system
  void testMailSystemUC3() {
	  // use case 3: Bill has a bunch of messages he wants to send to a single group of people
	  // he will make a set of messages and then send them to this group all at once
	  // he's going to send them to everyone in the employee database, including himself

	  // employee database
	  auto & eDB = Employees::EmployeeDB::getInstance();
	  // message database
	  auto & mDB = MailSystem::MessageDB::getInstance();

	  Employees::Employee bill = { "Bill" };

	  // find out if they are already in the database
	  auto findEmployee = [](Employees::Employee & emp)->Employees::EmployeeDB::Key {
		  return findByRecordData<Employees::EmployeeDB>(emp);
	  };

	  // bill's key
	  auto billKey = findEmployee(bill);

	  // bill constructs his distribution list -- bcc'ing everyone
	  MailSystem::Message::Envelope billsEnvelope({ billKey }, {}, {}, { 0UL, 2UL });
	  billsEnvelope.from = billKey;
	  
	  for (const auto & key : eDB.keys()) {
		  billsEnvelope.bcc.insert(key);
	  }

	  // bill's messages
	  std::list<MailSystem::Message> billsMessages = {
		  {"This is my shopping list:\n- eggs\n- ointment\n- vanilla wafers\n- frosting\n"},
		  {"You all need to stop sleeping at work.\n"},
		  {"I like turtles\n"},
		  {"Disgregard that last message, someone hacked my account.\n"}
	  };

	  // send them all!
	  for (auto & msg : billsMessages) {
		  msg.send(billsEnvelope);
	  }

	  // small lambda function to look up a mailbox
	  // built around a generic template function for looking up database records
	  auto getMailBox = [](auto mailboxKey)->Common::EmployeeMessageKeyDB::Value_t & {
		  return getDatabaseRecord<Common::EmployeeMessageKeyDB>(mailboxKey);
	  };

	  // get the inbox database records
	  auto & inbox = getMailBox(MailSystem::MailBoxes::IN);

	  // now let's loop through everyone's inboxes
	  for (const auto & emp : billsEnvelope.all()) {
		  std::cout << eDB[emp].name() << "'s Inbox contains:\n";
		  unsigned int msgCount = 0;
		  for (const auto & msg : inbox.retrieveRight(emp)) {
			  std::cout << mDB[msg].body() << '\n';

			  // lets make sure they got all of bills messages!
			  if (std::find(billsMessages.cbegin(), billsMessages.cend(), mDB[msg]) != billsMessages.cend()) {
				  msgCount++;
			  }
		  }
		  // if the counts aren't equal, then a message was missed
		  if (msgCount != billsMessages.size()) {
			  throw InvalidStateException("A message was not passed to the inbox of a recipient!", __LINE__, __func__, __FILE__);
		  }
	  }

	  // success
	  std::cout << "Success:  " << __func__ << "\v\n";
  }

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
	
	::testObjectAssociations();
	std::cout << seperator << '\n';

	::testPointerAssociations();
	std::cout << seperator << '\n';

	::testDbOperations1();
	std::cout << seperator << '\n';

	// commenting this one out so the employee database isn't cleared out
	// useful for verifying lookup capability in the next test case
	//::testDbOperations2();
	//std::cout << seperator << '\n';

	::testMailSystemUC1();
	std::cout << seperator << '\n';

	::testMailSystemUC2();
	std::cout << seperator << '\n';

	::testMailSystemUC3();
	std::cout << seperator << '\n';

    std::cout << "Success:  " << __func__ << "\v\n";
  }
  catch( std::exception & ex )
  {
    std::cerr << "\n\nFailure - \n" << ex.what() << "\n\n";
  }
}
