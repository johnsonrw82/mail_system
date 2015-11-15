/**
* File: MessageDB.hpp
* Author: Ryan Johnson
* Description: Header file for the database type alias for storing messages
*/

#ifndef MAILSYSTEM_Message_DB_hpp
#define MAILSYSTEM_Message_DB_hpp

#include "MailSystem/Message.hpp"
#include "Utilities/Database.hpp"

namespace MailSystem {
	extern const char SOURCE_DATA_FILE[];
	extern const char DESTINATION_DATA_FILE[];

	// type alias
	using MessageDB = Utilities::Database<MailSystem::Message, SOURCE_DATA_FILE, DESTINATION_DATA_FILE>;
}

#endif