#ifndef COMMON_Employee_Company_DB_hpp
#define COMMON_Employee_Company_DB_hpp

#include "Utilities/Database.hpp"
#include "Utilities/Associations.hpp"
#include "Employees/Employee.hpp"
#include "Employees/EmployeeDB.hpp"
#include "Companies/Company.hpp"
#include "Companies/CompanyDB.hpp"

namespace Common {
	extern const char EC_SOURCE_DATA_FILE[];
	extern const char EC_DESTINATION_DATA_FILE[];

	// employee company database alias
	using EmployeeCompanyDB = Utilities::Database<
		Utilities::Associations<Employees::Employee, Companies::Company>,
		EC_SOURCE_DATA_FILE, EC_DESTINATION_DATA_FILE>;

	extern const char EC_SOURCE_KEY_DATA_FILE[];
	extern const char EC_DESTINATION_KEY_DATA_FILE[];

	// employee company key database alias
	using EmployeeCompanyKeyDB = Utilities::Database<
		Utilities::Associations<Employees::EmployeeDB::Key, Companies::CompanyDB::Key>,
		EC_SOURCE_KEY_DATA_FILE, EC_DESTINATION_KEY_DATA_FILE>;
}

#endif
