/**
* File: EmployeeDB.hpp
* Author: Ryan Johnson
* Description: Header file for the database type alias for storing employees
*/

#ifndef EMPLOYEES_Employee_DB_hpp
#define EMPLOYEES_Employee_DB_hpp

#include "Utilities/Database.hpp"
#include "Employees/Employee.hpp"

namespace Employees {
	extern const char SOURCE_DATA_FILE[];
	extern const char DESTINATION_DATA_FILE[];

	// define the employee database alias
	using EmployeeDB = Utilities::Database<Employee, SOURCE_DATA_FILE, DESTINATION_DATA_FILE>;
}

#endif
