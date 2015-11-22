/*
* File: Message.hpp
* Author: Ryan Johnson
* Description: This file is the header definition for a Message and its nested Envelope class.
* This class provides the basic functionality to represent a message and an envelope -- where the message will be delivered,
* and optional attachments, represented by Documents.
*/

#ifndef MAILSYSTEM_Message_hpp
#define MAILSYSTEM_Message_hpp

#include <iostream>
#include <string>
#include <unordered_map>
#include <set>
#include <vector>

#include "MailSystem/Document.hpp"
#include "Employees/EmployeeDB.hpp"
#include "Utilities/Associations.hpp"
#include "Utilities/Exceptions.hpp"

namespace MailSystem {
	class Message {
		friend std::ostream & operator << (std::ostream & s, const Message & message);
		friend std::istream & operator >> (std::istream & s, Message & message);

		friend bool operator==(const Message & lhs, const Message & rhs);
		friend bool operator< (const Message & lhs, const Message & rhs);
		public:
			// exceptions
			struct MessageExceptions : Utilities::AbstractException<> { using AbstractException::AbstractException; };
			struct SymmetricSyncException : MessageExceptions { using MessageExceptions::MessageExceptions; };
			struct AttachmentNotFoundException : MessageExceptions { using MessageExceptions::MessageExceptions; };

			// nested class Envelope
			class Envelope {
				friend std::ostream & operator << (std::ostream & s, const Envelope & envelope);
				friend std::istream & operator >> (std::istream & s, Envelope & envelope);

				public:
					// exceptions
					struct EnvelopeExceptions : MessageExceptions { using MessageExceptions::MessageExceptions; };
					struct SymmetricSyncException : EnvelopeExceptions { using EnvelopeExceptions::EnvelopeExceptions; };

					// type aliases
					using EmployeeID		= Employees::EmployeeDB::Key;
					using RecipientList		= std::set<EmployeeID, Utilities::Less>;

					RecipientList			all() const;				// return a copy of all of the recipents for this envelope

					// constructors/destructor
					Envelope()											= default;
					Envelope(const Envelope & envelope)					= default;
					Envelope(Envelope && envelope)						= default;
					Envelope & operator= (const Envelope & envelope)	= default;
					Envelope & operator= (Envelope && envelope)			= default;
					Envelope(EmployeeID from, RecipientList to_, RecipientList cc_ = {}, RecipientList bcc_ = {});
					~Envelope() noexcept								= default;
				
					// public fields
					EmployeeID			from;
					RecipientList		to;
					RecipientList		cc;
					RecipientList		bcc;					
					
				private:		
					// Class attributes
					static constexpr char FIELD_SEPARATOR = '\x03';  // ETX (End of Text) character
					static constexpr char RECORD_SEPARATOR = '\x04';  // EOT (End of Transmission) character 

			};

			// type aliases (from Envelope)
			using EmployeeID			= Envelope::EmployeeID;
			using RecipientList			= Envelope::RecipientList;

			// message functions
			void						addAttachment(const std::string & name, Document attachment);		// add an attachment with given name
			Message &					addBCC(const RecipientList & bcc);									// add to the bcc list
			Message &					addCC(const RecipientList & cc);									// add to the cc list
			Message &					addTO(const RecipientList & to);									// add to the to list
			void						address(EmployeeID from, RecipientList to);							// set to the to list/from employee ID
			void						address(EmployeeID from, RecipientList to, RecipientList cc);		// set to the to list/from ID, with CC
			void						address(EmployeeID from, RecipientList to, RecipientList cc, RecipientList bcc);	// set the the to list/from ID, with CC and BCC
			void						address(Envelope envelope);											// set address based on envelope
			Document 					attachment(const std::string & name) const;							// return a copy of the attachment with given name
			Document &					attachment(const std::string & name);								// return attachment reference with given name
			std::vector<std::string>	attachments();														// return a vector of names for all attachments
			Document					body() const;														// return a copy of the body
			Document &					body();																// return a reference to the body
			Envelope					envelope() const;													// return a copy of the envelope for this message
			Envelope &					envelope();															// return a reference to the envelope for this message
			void						removeAttachment(const std::string & name);							// remove attachment with given name
			void						send();																// send the message
			void						send(Envelope envelope);											// send the message using supplied envelope


			// constructors/destructor
			Message()										= default;
			Message(const Message & message)				= default;
			Message(Message && message)						= default;
			Message & operator = (const Message & message)	= default;
			Message & operator = (Message && message)		= default;
			Message(Document body);
			Message(Envelope envelope);
			Message(Envelope envelope, Document body);
			Message(Envelope envelope, std::string body);
			Message(std::string body);
			~Message() noexcept								= default;

		private:
			// fields
			std::unordered_map<std::string, MailSystem::Document>	_attachments;
			MailSystem::Document									_body;
			Envelope												_envelope;
	
			// Class attributes
			static constexpr char FIELD_SEPARATOR = '\x03';  // ETX (End of Text) character
			static constexpr char RECORD_SEPARATOR = '\x04';  // EOT (End of Transmission) character
	};

	namespace MailBoxes {
		enum : std::size_t { IN = 0UL, SENT, DRAFT, DELETED };
	}

	// Non-member functions - message 
	bool operator==(const Message & lhs, const Message & rhs);
	bool operator< (const Message & lhs, const Message & rhs);
	bool operator!=(const Message & lhs, const Message & rhs);
	bool operator> (const Message & lhs, const Message & rhs);
	bool operator<=(const Message & lhs, const Message & rhs);
	bool operator>=(const Message & lhs, const Message & rhs);

	std::ostream & operator<< (std::ostream & s, const Message & message);
	std::istream & operator>> (std::istream & s, Message & message);
	std::ostream & operator<< (std::ostream & s, const Message * message);
	std::istream & operator>> (std::istream & s, Message * message);

	// Non-member functions - envelope
	bool operator==(const Message::Envelope & lhs, const Message::Envelope & rhs);
	bool operator< (const Message::Envelope & lhs, const Message::Envelope & rhs);
	bool operator!=(const Message::Envelope & lhs, const Message::Envelope & rhs);
	bool operator> (const Message::Envelope & lhs, const Message::Envelope & rhs);
	bool operator<=(const Message::Envelope & lhs, const Message::Envelope & rhs);
	bool operator>=(const Message::Envelope & lhs, const Message::Envelope & rhs);

	std::ostream & operator<< (std::ostream & s, const Message::Envelope & envelope);
	std::istream & operator>> (std::istream & s, Message::Envelope & envelope);
	std::ostream & operator<< (std::ostream & s, const Message::Envelope * envelope);
	std::istream & operator>> (std::istream & s, Message::Envelope * envelope);

}

#endif
