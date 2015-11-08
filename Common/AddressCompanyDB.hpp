#ifndef COMMON_Address_Company_DB_hpp
#define COMMON_Address_Company_DB_hpp

#include "Utilities/Database.hpp"
#include "Utilities/Associations.hpp"
#include "Addresses/Address.hpp"
#include "Addresses/AddressDB.hpp"
#include "Companies/Company.hpp"
#include "Companies/CompanyDB.hpp"

namespace Common {
	extern const char AC_SOURCE_DATA_FILE[];
	extern const char AC_DESTINATION_DATA_FILE[];

	// employee company database alias
	using AddressCompanyDB = Utilities::Database<
		Utilities::Associations<Addresses::Address, Companies::Company>,
		AC_SOURCE_DATA_FILE, AC_DESTINATION_DATA_FILE>;

	extern const char AC_SOURCE_KEY_DATA_FILE[];
	extern const char AC_DESTINATION_KEY_DATA_FILE[];

	// employee company key database alias
	using AddressCompanyKeyDB = Utilities::Database<
		Utilities::Associations<Addresses::AddressDB::Key, Companies::CompanyDB::Key>,
		AC_SOURCE_KEY_DATA_FILE, AC_DESTINATION_KEY_DATA_FILE>;
}

#endif
