#include <algorithm>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Companies/Company.hpp"
#include "Utilities/Exceptions.hpp"

namespace Companies {
	namespace RegressionTest {
		namespace {
			struct RegressionTestException : Utilities::AbstractException<> { using AbstractException::AbstractException; };              // Regression test exception base class
			struct   UndetectedException : RegressionTestException { using RegressionTestException::RegressionTestException; };  // Inherit base class constructors
			struct   RelationalTestFailure : RegressionTestException { using RegressionTestException::RegressionTestException; };
			struct   SemmetricalIOFailure : RegressionTestException { using RegressionTestException::RegressionTestException; };
			struct   PropertyValueException : RegressionTestException { using RegressionTestException::RegressionTestException; };         // new exception used to mark when a property is incorrectly set
			struct   InvalidStateException : RegressionTestException { using RegressionTestException::RegressionTestException; };  // used when the object is in an invalid state

			/****************************************************************************
			** Company Verification & Regression Test
			****************************************************************************/
			void runCompanyTest()
			{
				using namespace Companies;

				std::vector<Company> companies =
				{
					{ "The Kroger Co" },
					{ "Albertson's, Inc" },
					{ "The Walt Disney Company - Disneyland" },
					{ "Cedar Fair Entertainment Company - Knott's Berry Farm" },
					{}
				};

				// Modify the property by chaining the update functions together
				companies[4].name("CSUF UEE");
				for (const auto & company : companies)  std::cout << company << '\n';
				std::cout << '\n';

				// verify a couple logical operators by sorting
				std::sort(std::begin(companies), std::end(companies), Companies::operator<);
				for (const auto & company : companies)  std::cout << company << '\n';
				std::cout << '\n';

				std::sort(std::begin(companies), std::end(companies), Companies::operator>);
				for (const auto & company : companies)  std::cout << company << '\n';

				// test setting name
				companies[1].name("The ABC Company");
				if (companies[1].name() != "The ABC Company") {
					throw RegressionTestException("Company name not set properly", __LINE__, __func__, __FILE__);
				}

				// verify some equality checks
				if (companies[0] != companies[0])  throw RelationalTestFailure("Equality Failure", __LINE__, __func__, __FILE__);
				if (companies[1] == companies[0])  throw RelationalTestFailure("Equality Failure", __LINE__, __func__, __FILE__);
				if (!(companies[0] <= companies[0]))  throw RelationalTestFailure("Equality Failure", __LINE__, __func__, __FILE__);
				if (!(companies[0] >= companies[0]))  throw RelationalTestFailure("Equality Failure", __LINE__, __func__, __FILE__);

				// symmetric insertion and extraction checks
				{
					std::stringstream s;
					s << companies[2];

					Company temp;
					s >> temp;

					if (companies[2] != temp)  throw SemmetricalIOFailure("Symmetric Insertion/Extraction Failure", __LINE__, __func__, __FILE__);
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

			// execute test struct
			struct ExecuteTest {
				ExecuteTest() {
					try {
						const auto seperator = std::string(80, '=');

						// run the test(s)
						std::clog << "INFO: Running regression tests for \"class Companies::Company\"\n";
						runCompanyTest();
						std::cout << seperator << '\n';

						std::clog << "Tests completed successfully" << std::endl;
					}
					// catch and log failure
					catch (std::exception & ex)
					{
						std::clog << "failed.\n" << std::endl;
						std::cerr << "FAILURE: Regression tests for \"class Companies::Company\" failed.\nTerminating Program\n\n\n" << ex.what() << std::endl;
						std::exit(-__LINE__);
					}
				}
			} runTest; // run via constructor
		}
	}
}