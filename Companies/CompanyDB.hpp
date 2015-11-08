#ifndef COMPANIES_Company_DB_hpp
#define COMPANIES_Company_DB_hpp

// include required headers
#include "Companies/Company.hpp"
#include "Utilities/Database.hpp"

namespace Companies {
	extern const char SOURCE_DATA_FILE[];
	extern const char DESTINATION_DATA_FILE[];

	// set up the company database type
	// instantiate the database
	using CompanyDB = Utilities::Database<Company, SOURCE_DATA_FILE, DESTINATION_DATA_FILE>;
}

#endif
