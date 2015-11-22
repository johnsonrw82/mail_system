#include <algorithm>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

#include "Addresses/Address.hpp"
#include "Companies/Company.hpp"
#include "Employees/Employee.hpp"
#include "Utilities/Exceptions.hpp"
#include "Utilities/Associations.hpp"

namespace Utilities {
	namespace RegressionTest {
		namespace {
			struct RegressionTestException : Utilities::AbstractException<> { using AbstractException::AbstractException; };              // Regression test exception base class
			struct   UndetectedException : RegressionTestException { using RegressionTestException::RegressionTestException; };  // Inherit base class constructors
			struct   RelationalTestFailure : RegressionTestException { using RegressionTestException::RegressionTestException; };
			struct   SemmetricalIOFailure : RegressionTestException { using RegressionTestException::RegressionTestException; };
			struct   PropertyValueException : RegressionTestException { using RegressionTestException::RegressionTestException; };         // new exception used to mark when a property is incorrectly set
			struct   InvalidStateException : RegressionTestException { using RegressionTestException::RegressionTestException; };  // used when the object is in an invalid state

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
					{ "3350 N Woodlawn", "Wichita", "KS", 67226UL },
					{ "1740 Fleischli Pkwy", "Cheyenne", "WY", 82001UL },
					{ "10120 SE Washington St", "Portland", "OR", 97216UL }
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
				employeeCompanies.insert(EmployeeCompany::LeftCollectionType({ "Jacobs, Brian"s }), homeDepot); // brian will work for home depot

																												// safeway employees
				employeeCompanies.insert(EmployeeCompany::LeftCollectionType({
					{ "Jones, Fred"s },
					{ "Anderson, Mildred" },
					{ "Burd, Steve" },
				}), safeway);

				// they have to live somewhere...
				employeeDirectory.insert(EmployeeAddress::LeftCollectionType({ "Jones, Fred"s }), { "65 Walnut Ave"s, "Beaverton"s, "OR"s, 97008UL });
				employeeDirectory.insert(EmployeeAddress::LeftCollectionType({ "Burd, Steve"s }), { "189 Fern Hill Road"s, "Pleasanton"s, "CA"s, 94588UL });
				employeeDirectory.insert(EmployeeAddress::LeftCollectionType({ "Anderson, Mildred"s }), { "23456 SW 18th St"s, "Cheyenne"s, "WY"s, "82001"s });

				// make a copy and test the complete removal of these people (test firing)
				EmployeeCompany employeeCompaniesCopy = employeeCompanies;
				employeeCompaniesCopy.removeRight(safeway);

				// verify that company no longer has anyone working for it
				if (employeeCompaniesCopy.retrieveLeft(safeway).size() != 0) {
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
				if (!isEqual(employeeCompanies, tmp2)) {
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

			// execute test struct
			struct ExecuteTest {
				ExecuteTest() {
					try {
						const auto seperator = std::string(80, '=');

						// run the test(s)
						std::clog << "INFO: Running regression tests for \"class Utilities::Associations\"\n";
						testObjectAssociations();
						testPointerAssociations();
						std::cout << seperator << '\n';

						std::clog << "Tests completed successfully" << std::endl;
					}
					// catch and log failure
					catch (std::exception & ex)
					{
						std::clog << "failed.\n" << std::endl;
						std::cerr << "FAILURE: Regression tests for \"class Utilities::Associations\" failed.\nTerminating Program\n\n\n" << ex.what() << std::endl;
						std::exit(-__LINE__);
					}
				}
			} runTest; // run via constructor
		}
	}
}
