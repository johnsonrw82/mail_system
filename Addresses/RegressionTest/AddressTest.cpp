#include <algorithm>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Addresses/Address.hpp"
#include "Utilities/Exceptions.hpp"

namespace Addresses {
	namespace RegressionTest {
		namespace {
			struct RegressionTestException : Utilities::AbstractException<> { using AbstractException::AbstractException; };              // Regression test exception base class
			struct   UndetectedException : RegressionTestException { using RegressionTestException::RegressionTestException; };  // Inherit base class constructors
			struct   RelationalTestFailure : RegressionTestException { using RegressionTestException::RegressionTestException; };
			struct   SemmetricalIOFailure : RegressionTestException { using RegressionTestException::RegressionTestException; };
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
					{ "157 S. Howard Street", "Spokane", "WA", 99201UL },            // Ralph's/Kroger
					{ "1014 Vine Street", "Cincinnati", "Ohio", "45202-1100" },      // Albertson's
					{ "1313 S. Harbor Boulevard", "Anaheim", "CA", "92803-1313" },   // Disneyland
					{}
				};

				// Modify the property by chaining the update functions together
				properties[3].street("8039 Beach Boulevard").city("Buena Park").state("CA").zipCode(90620);        // Knotts Berry Farm
				for (const auto & property : properties)  std::cout << property << '\n';
				std::cout << '\n';

				// verify a couple logical operators by sorting
				std::sort(std::begin(properties), std::end(properties), Addresses::operator<);
				for (const auto & property : properties)  std::cout << property << '\n';
				std::cout << '\n';

				std::sort(std::begin(properties), std::end(properties), Addresses::operator>);
				for (const auto & property : properties)  std::cout << property << '\n';

				// Verify state codes
				try
				{
					properties[1].state("CSUF");
					throw UndetectedException("Undetected Wrong State or State Code", __LINE__, __func__, __FILE__);
				}
				catch (Address::StateCodeException &) {} // catch and ignore expected exception type, let all other propagate up

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
					properties[0].zipCode("00000");
					throw UndetectedException("Undetected Wrong Zip Code", __LINE__, __func__, __FILE__);
				}
				catch (Address::ZipCodeException &) {} // catch and ignore expected exception type, let all other propagate up

				try
				{
					properties[0].zipCode("99999");
					throw UndetectedException("Undetected Wrong Zip Code", __LINE__, __func__, __FILE__);
				}
				catch (Address::ZipCodeException &) {} // catch and ignore expected exception type, let all other propagate up

				try
				{
					properties[0].zipCode("12345-0000");
					throw UndetectedException("Undetected Wrong Zip Code", __LINE__, __func__, __FILE__);
				}
				catch (Address::ZipCodeException &) {} // catch and ignore expected exception type, let all other propagate up

				try
				{
					properties[0].zipCode("12345-9999");
					throw UndetectedException("Undetected Wrong Zip Code", __LINE__, __func__, __FILE__);
				}
				catch (Address::ZipCodeException &) {} // catch and ignore expected exception type, let all other propagate up

				try
				{
					properties[0].zipCode("12345-12345");
					throw UndetectedException("Undetected Wrong Zip Code", __LINE__, __func__, __FILE__);
				}
				catch (Address::ZipCodeException &) {} // catch and ignore expected exception type, let all other propagate up

													   // verify some equality checks
				if (properties[0] != properties[0])  throw RelationalTestFailure("Equality Failure", __LINE__, __func__, __FILE__);
				if (properties[1] == properties[0]) throw RelationalTestFailure("Equality Failure", __LINE__, __func__, __FILE__);
				if (!(properties[1] <= properties[1])) throw RelationalTestFailure("Equality Failure", __LINE__, __func__, __FILE__);
				if (!(properties[1] >= properties[1])) throw RelationalTestFailure("Equality Failure", __LINE__, __func__, __FILE__);

				// symmetric insertion and extraction checks
				{
					std::stringstream s;
					s << properties[2];

					Address temp;
					s >> temp;

					if (properties[2] != temp)  throw SemmetricalIOFailure("Symmetric Insertion/Extraction Failure", __LINE__, __func__, __FILE__);
				}

				// symmetric insertion and extraction checks with casting
				{
					Address temp;
					std::istringstream(static_cast<std::string>(properties[1])) >> temp;

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

			// execute test struct
			struct ExecuteTest {
				ExecuteTest() {
					try {
						const auto seperator = std::string(80, '=');

						// run the test(s)
						std::clog << "INFO: Running regression tests for \"class Addresses::Address\"\n";
						runAddressTest();
						std::cout << seperator << '\n';

						std::clog << "Tests completed successfully" << std::endl;
					}
					// catch and log failure
					catch (std::exception & ex)
					{
						std::clog << "failed.\n" << std::endl;
						std::cerr << "FAILURE: Regression tests for \"class Addresses::Address\" failed.\nTerminating Program\n\n\n" << ex.what() << std::endl;
						std::exit(-__LINE__);
					}
				}
			} runTest; // run via constructor
		}
	}
}