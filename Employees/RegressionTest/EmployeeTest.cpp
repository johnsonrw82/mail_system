#include <algorithm>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Employees/Employee.hpp"
#include "Utilities/Exceptions.hpp"

namespace Employees {
	namespace RegressionTest {
		namespace {
			struct RegressionTestException : Utilities::AbstractException<> { using AbstractException::AbstractException; };              // Regression test exception base class
			struct   UndetectedException : RegressionTestException { using RegressionTestException::RegressionTestException; };  // Inherit base class constructors
			struct   RelationalTestFailure : RegressionTestException { using RegressionTestException::RegressionTestException; };
			struct   SemmetricalIOFailure : RegressionTestException { using RegressionTestException::RegressionTestException; };
			struct   PropertyValueException : RegressionTestException { using RegressionTestException::RegressionTestException; };         // new exception used to mark when a property is incorrectly set
			struct   InvalidStateException : RegressionTestException { using RegressionTestException::RegressionTestException; };  // used when the object is in an invalid state

			/****************************************************************************
		    ** Employee Verification & Regression Test
		    ****************************************************************************/
			void runEmployeeTest()
			{
				using namespace Employees;

				std::vector<Employee> workers =
				{
					{ "Tom", "Bettens" },
					{ "Disney, Walt" },
					{ "Bjarne", "Stroustrup" },
					{ "Skywalker" },
					{}
				};

				// Modify the property by chaining the update functions together
				workers[4].name("Holmes, Sherlock");
				for (const auto & worker : workers)  std::cout << worker << '\n';
				std::cout << '\n';


				// verify a couple logical operators by sorting
				std::sort(std::begin(workers), std::end(workers), Employees::operator<);
				for (const auto & worker : workers)  std::cout << worker << '\n';
				std::cout << '\n';

				std::sort(std::begin(workers), std::end(workers), Employees::operator>);
				for (const auto & worker : workers)  std::cout << worker << '\n';


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
				if (workers[0] != workers[0])  throw RelationalTestFailure("Equality Failure", __LINE__, __func__, __FILE__);
				if (workers[1] == workers[0])  throw RelationalTestFailure("Equality Failure", __LINE__, __func__, __FILE__);
				if (!(workers[0] <= workers[0]))  throw RelationalTestFailure("Equality Failure", __LINE__, __func__, __FILE__);
				if (!(workers[0] >= workers[0]))  throw RelationalTestFailure("Equality Failure", __LINE__, __func__, __FILE__);

				// symmetric insertion and extraction checks
				{
					std::stringstream s;
					s << workers[2];

					Employee temp;
					s >> temp;

					if (workers[2] != temp)  throw SemmetricalIOFailure("Symmetric Insertion/Extraction Failure", __LINE__, __func__, __FILE__);
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

			// execute test struct
			struct ExecuteTest {
				ExecuteTest() {
					try {
						const auto seperator = std::string(80, '=');

						// run the test(s)
						std::clog << "INFO: Running regression tests for \"class Employees::Employee\"\n";
						runEmployeeTest();
						std::cout << seperator << '\n';

						std::clog << "Tests completed successfully" << std::endl;
					}
					// catch and log failure
					catch (std::exception & ex)
					{
						std::clog << "failed.\n" << std::endl;
						std::cerr << "FAILURE: Regression tests for \"class Employees::Employee\" failed.\nTerminating Program\n\n\n" << ex.what() << std::endl;
						std::exit(-__LINE__);
					}
				}
			} runTest; // run via constructor
		}
	}
}