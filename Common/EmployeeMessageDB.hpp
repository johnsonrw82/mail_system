#ifndef COMMON_Employee_Message_DB_hpp
#define COMMON_Employee_Message_DB_hpp

#include "Utilities/Database.hpp"
#include "Utilities/Associations.hpp"
#include "Employees/Employee.hpp"
#include "Employees/EmployeeDB.hpp"
#include "MailSystem/Message.hpp"
#include "MailSystem/MessageDB.hpp"

namespace Common {
	extern const char EM_SOURCE_DATA_FILE[];
	extern const char EM_DESTINATION_DATA_FILE[];

	// employee address database alias
	using EmployeeMessageDB = Utilities::Database<
		Utilities::Associations<Employees::Employee, MailSystem::Message>,
		EM_SOURCE_DATA_FILE, EM_DESTINATION_DATA_FILE>;

	extern const char EM_SOURCE_KEY_DATA_FILE[];
	extern const char EM_DESTINATION_KEY_DATA_FILE[];

	// employee address key database alias
	using EmployeeMessageKeyDB = Utilities::Database<
		Utilities::Associations<Employees::EmployeeDB::Key, MailSystem::MessageDB::Key>,
		EM_SOURCE_KEY_DATA_FILE, EM_DESTINATION_KEY_DATA_FILE>;
}


#endif

