/**
* File: AddressDB.hpp
* Author: Ryan Johnson
* Description: Header file for the database type alias for storing addresses
*/

#ifndef ADDRESSES_Address_DB_hpp
#define ADDRESSES_Address_DB_hpp

#include "Utilities/Database.hpp"
#include "Addresses/Address.hpp"

namespace Addresses 
{
	extern const char SOURCE_DATA_FILE[];
	extern const char DESTINATION_DATA_FILE[];

	// define address database alias
	using AddressDB = Utilities::Database<Address, SOURCE_DATA_FILE, DESTINATION_DATA_FILE>;
}

#endif
