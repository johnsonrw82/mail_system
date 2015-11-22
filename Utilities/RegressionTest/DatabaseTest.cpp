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

#include "Common/AddressCompanyDB.hpp"
#include "Common/EmployeeCompanyDB.hpp"
#include "Common/EmployeeAddressDB.hpp"
#include "Common/EmployeeMessageDB.hpp"

namespace Utilities {
	namespace RegressionTest {
		namespace {
			struct RegressionTestException : Utilities::AbstractException<> { using AbstractException::AbstractException; };              // Regression test exception base class
			struct   UndetectedException : RegressionTestException { using RegressionTestException::RegressionTestException; };  // Inherit base class constructors
			struct   RelationalTestFailure : RegressionTestException { using RegressionTestException::RegressionTestException; };
			struct   SemmetricalIOFailure : RegressionTestException { using RegressionTestException::RegressionTestException; };
			struct   PropertyValueException : RegressionTestException { using RegressionTestException::RegressionTestException; };         // new exception used to mark when a property is incorrectly set
			struct   InvalidStateException : RegressionTestException { using RegressionTestException::RegressionTestException; };  // used when the object is in an invalid state

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

			// execute test struct
			struct ExecuteTest {
				ExecuteTest() {
					try {
						const auto seperator = std::string(80, '=');

						// run the test(s)
						std::clog << "INFO: Running regression tests for \"class Utilities::Database\"\n";
						testDbOperations1();
						std::cout << seperator << '\n';

						std::clog << "Tests completed successfully" << std::endl;
					}
					// catch and log failure
					catch (std::exception & ex)
					{
						std::clog << "failed.\n" << std::endl;
						std::cerr << "FAILURE: Regression tests for \"class Utilities::Database\" failed.\nTerminating Program\n\n\n" << ex.what() << std::endl;
						std::exit(-__LINE__);
					}
				}
			} runTest; // run via constructor
		}
	}
}