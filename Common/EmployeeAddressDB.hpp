/**
* File: EmployeeAddressDB.hpp
* Author: Ryan Johnson
* Description: Header file for the database type alias for storing employee-address associations
*/

#ifndef COMMON_Employee_Address_DB_hpp
#define COMMON_Employee_Address_DB_hpp

#include "Utilities/Database.hpp"
#include "Utilities/Associations.hpp"
#include "Employees/Employee.hpp"
#include "Employees/EmployeeDB.hpp"
#include "Addresses/Address.hpp"
#include "Addresses/AddressDB.hpp"

namespace Common {
	extern const char EA_SOURCE_DATA_FILE[];
	extern const char EA_DESTINATION_DATA_FILE[];

	// employee address database alias
	using EmployeeAddressDB = Utilities::Database<
		Utilities::Associations<Employees::Employee, Addresses::Address, Utilities::Many2OnePolicy>,
		EA_SOURCE_DATA_FILE, EA_DESTINATION_DATA_FILE>;

	extern const char EA_SOURCE_KEY_DATA_FILE[];
	extern const char EA_DESTINATION_KEY_DATA_FILE[];

	// employee address key database alias
	using EmployeeAddressKeyDB = Utilities::Database<
		Utilities::Associations<Employees::EmployeeDB::Key, Addresses::AddressDB::Key, Utilities::Many2OnePolicy>,
		EA_SOURCE_KEY_DATA_FILE, EA_DESTINATION_KEY_DATA_FILE>;
}


#endif
