/*
* File: Message.cpp
* Author: Ryan Johnson
* Description: This file is the source implementation for a Message and its nested Envelope class.
* This class provides the basic functionality to represent a message and an envelope -- where the message will be delivered,
* and optional attachments, represented by Documents.
*/

#include <set>
#include <unordered_map>
#include <string>
#include <iostream>
#include <iterator>
#include <istream>
#include <utility>
#include <cstdlib>
#include <algorithm>
#include <stdexcept>

#include "MailSystem/Message.hpp"
#include "MailSystem/MessageDB.hpp"
#include "Common/EmployeeMessageDB.hpp"

namespace MailSystem {
	/********************************************************************
	* Message
	*********************************************************************/
	// constructors
	Message::Message(Document body)								: Message(Envelope(), std::move(body)) {}						// self delegation
	Message::Message(Envelope envelope)							: Message(std::move(envelope), Document()) {}					// self delegation
	Message::Message(Envelope envelope, Document body)			:
																	_body(std::move(body)), 
																	_envelope(std::move(envelope)) {}							// initialize each property
	Message::Message(Envelope envelope, std::string body)		: _body(std::move(body)), _envelope(std::move(envelope)) {}		// initialize each property
	Message::Message(std::string body)							: _body(std::move(body)) {}										// initialize property

	// add an attachment with given name
	void Message::addAttachment(const std::string & name, Document attachment) {
		// insert/update
		_attachments[name] = std::move(attachment);
	}

	// add to the bcc list
	Message & Message::addBCC(const RecipientList & bcc) {
		_envelope.bcc.insert(bcc.cbegin(), bcc.cend());

		return *this;
	}
	// add to the cc list
	Message & Message::addCC(const RecipientList & cc) {
		_envelope.cc.insert(cc.cbegin(), cc.cend());

		return *this;
	}
	// add to the to list
	Message & Message::addTO(const RecipientList & to) {
		_envelope.to.insert(to.cbegin(), to.cend());

		return *this;
	}
	// set the to list/from employee ID
	void Message::address(EmployeeID from, RecipientList to) {		
		_envelope.from = std::move(from);
		_envelope.to = std::move(to);
	}
	// set the to list/from ID, with CC
	void Message::address(EmployeeID from, RecipientList to, RecipientList cc) {
		address(from, to);
		_envelope.cc = std::move(cc);
	}
	// set the to list/from ID, with CC and BCC
	void Message::address(EmployeeID from, RecipientList to, RecipientList cc, RecipientList bcc) {		
		address(Envelope(from, to, cc, bcc));
	}
	// set address based on envelope
	void Message::address(Envelope envelope) {
		_envelope = std::move(envelope);
	}
	// return a copy of the attachment with given name
	Document Message::attachment(const std::string & name) const {
		try {
			// if the name doesn't exist, catch and re-throw exception
			return _attachments.at(name);
		}
		catch (std::out_of_range & ex) {
			throw AttachmentNotFoundException(ex, "Attachment not found with given name", __LINE__, __func__, __FILE__);
		}
	}	
	// return attachment reference with given name
	Document & Message::attachment(const std::string & name) {
		try {
			// if the name doesn't exist, catch and re-throw exception
			return _attachments.at(name);
		}
		catch (std::out_of_range & ex) {
			throw AttachmentNotFoundException(ex, "Attachment not found with given name", __LINE__, __func__, __FILE__);
		}
	}
	// return a vector of names for all attachments
	std::vector<std::string> Message::attachments() {
		std::vector<std::string> result(_attachments.size());   // preallocate space for each attachment
		std::vector<std::string>::size_type i = 0U;

		// add each attachment to the vector
		for (const auto & attachment : _attachments) {
			result[i++] = attachment.first;
		}

		return result;
	}

	// return a copy of the body
	Document Message::body() const {
		return _body;
	}
	// return a reference to the body
	Document & Message::body() {
		return _body;
	}
	// return a copy of the envelope for this message
	Message::Envelope Message::envelope() const {
		return _envelope;
	}
	// return a reference to the envelope for this message
	Message::Envelope & Message::envelope() {
		return _envelope;
	}
	// remove attachment with given name
	void Message::removeAttachment(const std::string & name) {
		_attachments.erase(name);
	}


	// send the message
	void Message::send() {
		// get the message database
		auto & database = MessageDB::getInstance();

		// add this message to it
		auto message = database.insert(*this);

		// now, get the sent mailbox and inboxes for all employees in the envelope
		auto getMailBox = [](auto mailboxKey)->Common::EmployeeMessageKeyDB::Value_t & {
			auto & database = Common::EmployeeMessageKeyDB::getInstance();

			// try to look up the record in the database
			try {
				return database[mailboxKey];
			}
			catch (Utilities::RecordLookupException &) {}

			// if here, then the record wasn't found, let's make one and then return it
			database.insert(mailboxKey, {});
			return database[mailboxKey];
		};


		auto & sentBox = getMailBox(MailBoxes::SENT);
		auto & inBox = getMailBox(MailBoxes::IN);

		sentBox.insert(envelope().from, message);
		inBox.insert(envelope().all(), message);	// send to all recipients
	}
	// send the message using supplied envelope
	void Message::send(Envelope envelope) {
		// send message using supplied envelope
		// assign this object envelope, and send
		_envelope = std::move(envelope);
		send();
	}


	// stream operators
	// message ostream
	std::ostream & operator << (std::ostream & s, const Message & message) {
		s << message._envelope << Message::FIELD_SEPARATOR <<
			message._body << Message::FIELD_SEPARATOR;

		// attachments
		s << message._attachments.size() << Message::RECORD_SEPARATOR;
		// loop through
		for (const auto & attachment : message._attachments) {
			s << attachment.first << Message::FIELD_SEPARATOR << attachment.second << Message::RECORD_SEPARATOR;
		}

		return s;
	}
	// message istream
	std::istream & operator >> (std::istream & s, Message & message) {
		Message temp;

		char delimiter;

		// get the envelope and body from the stream;
		s >> temp._envelope >> delimiter;
		// sync exception
		if (delimiter != Message::FIELD_SEPARATOR) {
			throw Message::SymmetricSyncException("Message symmetric sync exception", __LINE__, __func__, __FILE__);
		}

		// body
		s >> temp._body >> delimiter;
		// sync exception
		if (delimiter != Message::FIELD_SEPARATOR) {
			throw Message::SymmetricSyncException("Message symmetric sync exception", __LINE__, __func__, __FILE__);
		}

		// get attachments
		std::size_t attachmentsSize = 0UL;
		s >> attachmentsSize >> delimiter;

		// sync exception
		if (delimiter != Message::RECORD_SEPARATOR) {
			throw Message::SymmetricSyncException("Message symmetric sync exception", __LINE__, __func__, __FILE__);
		}

		for (unsigned int i = 0; i < attachmentsSize; ++i) {
			std::string name;
			MailSystem::Document doc;

			// sync exception
			if (!std::getline(s, name, Message::FIELD_SEPARATOR)) {
				throw Message::SymmetricSyncException("Message symmetric sync exception", __LINE__, __func__, __FILE__);
			}

			s >> doc >> delimiter;
			// sync exception
			if (delimiter != Message::RECORD_SEPARATOR) {
				throw Message::SymmetricSyncException("Message symmetric sync exception", __LINE__, __func__, __FILE__);
			}

			// insert the attachments
			temp.addAttachment(name, doc);
		}

		// all should be good
		message = std::move(temp);

		return s;
	}
	// pointer streams
	std::ostream & operator<< (std::ostream & s, const Message * message) {
		if (message != nullptr) {
			s << *message;
		}

		return s;
	}
	std::istream & operator>> (std::istream & s, Message * message) {
		if (message != nullptr) {
			s >> *message;
		}

		return s;
	}

	// equality/comparison operators
	bool operator==(const Message & lhs, const Message & rhs) {
		return (&lhs == &rhs) ||
			(lhs._envelope == rhs._envelope &&
				lhs._body == rhs._body &&
				lhs._attachments == rhs._attachments);
	}

	bool operator< (const Message & lhs, const Message & rhs) {
		// self equality
		if (&lhs == &rhs) {
			return false;
		}

		// sort by envelope
		return lhs._envelope < rhs._envelope;

	}
	bool operator!=(const Message & lhs, const Message & rhs) {
		return !(lhs == rhs);
	}
	bool operator> (const Message & lhs, const Message & rhs) {
		return rhs < lhs;
	}
	bool operator<=(const Message & lhs, const Message & rhs) {
		return !(rhs < lhs);
	}
	bool operator>=(const Message & lhs, const Message & rhs) {
		return !(lhs < rhs);
	}

	/********************************************************************
	* Envelope
	*********************************************************************/
	// constructor
	Message::Envelope::Envelope(EmployeeID from, RecipientList to_, RecipientList cc_, RecipientList bcc_) : 
		from(std::move(from)), to(std::move(to_)), cc(std::move(cc_)), bcc(std::move(bcc_)) {}

	// get all recipients
	Message::Envelope::RecipientList Message::Envelope::all() const {
		RecipientList result;

		// union of to and cc
		std::set_union(to.cbegin(), to.cend(), cc.cbegin(), cc.cend(), std::inserter(result, result.begin()));
		// union of result and bcc
		std::set_union(bcc.cbegin(), bcc.cend(), result.cbegin(), result.cend(), std::inserter(result, result.begin()));

		// remove 0UL from the set, it would be an invalid key
		result.erase(0UL);

		return result;
	}

	// stream operators
	// envelope ostream
	std::ostream & operator << (std::ostream & s, const Message::Envelope & envelope) {
		s << envelope.from << Message::Envelope::FIELD_SEPARATOR;
		
		// write out the size to denote how many elements follow
		s << envelope.to.size() << Message::Envelope::RECORD_SEPARATOR;
		for (const auto & toAddr : envelope.to) {
			s << toAddr << Message::Envelope::FIELD_SEPARATOR;
		}
		s << envelope.cc.size() << Message::Envelope::RECORD_SEPARATOR;
		for (const auto & ccAddr : envelope.cc) {
			s << ccAddr << Message::Envelope::FIELD_SEPARATOR;
		}
		s << envelope.bcc.size() << Message::Envelope::RECORD_SEPARATOR;
		for (const auto & bccAddr : envelope.bcc) {
			s << bccAddr << Message::Envelope::FIELD_SEPARATOR;
		}
			
		s << Message::Envelope::RECORD_SEPARATOR;
		return s;
	}

	// envelope istream
	std::istream & operator >> (std::istream & s, Message::Envelope & envelope) {
		Message::Envelope temp;

		char delimiter;

		// get the from ID
		s >> temp.from >> delimiter;
		if (delimiter != Message::Envelope::FIELD_SEPARATOR) {
			throw Message::Envelope::SymmetricSyncException("Envelope symmetric sync exception", __LINE__, __func__, __FILE__);
		}

		// sizes
		std::size_t toSize = 0UL, ccSize = 0UL, bccSize = 0UL;

		// next, get the 'to' size
		s >> toSize >> delimiter;
		if (delimiter != Message::Envelope::RECORD_SEPARATOR) {
			throw Message::Envelope::SymmetricSyncException("Envelope symmetric sync exception", __LINE__, __func__, __FILE__);
		}

		// get the items from the list
		for (unsigned int i = 0; i < toSize; ++i) {
			Message::Envelope::EmployeeID record;
			s >> record >> delimiter;
			if (delimiter != Message::Envelope::FIELD_SEPARATOR) {
				throw Message::Envelope::SymmetricSyncException("Envelope symmetric sync exception", __LINE__, __func__, __FILE__);
			}
			// insert into the list
			temp.to.insert(record);
		}

		// next, get the 'cc' size
		s >> ccSize >> delimiter;
		if (delimiter != Message::Envelope::RECORD_SEPARATOR) {
			throw Message::Envelope::SymmetricSyncException("Envelope symmetric sync exception", __LINE__, __func__, __FILE__);
		}

		// get the items from the list
		for (unsigned int i = 0; i < ccSize; ++i) {
			Message::Envelope::EmployeeID record;
			s >> record >> delimiter;
			if (delimiter != Message::Envelope::FIELD_SEPARATOR) {
				throw Message::Envelope::SymmetricSyncException("Envelope symmetric sync exception", __LINE__, __func__, __FILE__);
			}
			// insert into the list
			temp.cc.insert(record);
		}

		// next, get the 'bcc' size
		s >> bccSize >> delimiter;
		if (delimiter != Message::Envelope::RECORD_SEPARATOR) {
			throw Message::Envelope::SymmetricSyncException("Envelope symmetric sync exception", __LINE__, __func__, __FILE__);
		}

		// get the items from the list
		for (unsigned int i = 0; i < bccSize; ++i) {
			Message::Envelope::EmployeeID record;
			s >> record >> delimiter;
			if (delimiter != Message::Envelope::FIELD_SEPARATOR) {
				throw Message::Envelope::SymmetricSyncException("Envelope symmetric sync exception", __LINE__, __func__, __FILE__);
			}
			// insert into the list
			temp.bcc.insert(record);
		}

		// the last delimiter
		s >> delimiter;
		if (delimiter != Message::Envelope::RECORD_SEPARATOR) {
			throw Message::Envelope::SymmetricSyncException("Envelope symmetric sync exception", __LINE__, __func__, __FILE__);
		}
		// everything should be good!
		envelope = std::move(temp);

		return s;
	}

	// pointer stream operations
	std::ostream & operator<< (std::ostream & s, const Message::Envelope * envelope) {
		if (envelope != nullptr) {
			s << *envelope;
		}

		return s;
	}
	std::istream & operator>> (std::istream & s, Message::Envelope * envelope) {
		if (envelope != nullptr) {
			s >> *envelope;
		}

		return s;
	}

	// equality/comparison operator functions - envelope
	bool operator==(const Message::Envelope & lhs, const Message::Envelope & rhs) {
		return (&lhs == &rhs) ||
			(lhs.from == rhs.from &&
				lhs.to == rhs.to &&
				lhs.cc == rhs.cc &&
				lhs.bcc == rhs.bcc);
	}
	bool operator< (const Message::Envelope & lhs, const Message::Envelope & rhs) {
		// self equality
		if (&lhs == &rhs) {
			return false;
		}

		// sort by from, to, cc, bcc
		if (rhs.from != lhs.from) {
			return lhs.from < rhs.from;
		}
		if (lhs.to != rhs.to) {
			return lhs.to < rhs.to;
		}
		if (lhs.cc != rhs.cc) {
			return lhs.cc < rhs.cc;
		}

		// last item to sort by
		return lhs.bcc < rhs.bcc;		
	}
	bool operator!=(const Message::Envelope & lhs, const Message::Envelope & rhs) {
		return !(lhs == rhs);
	}
	bool operator> (const Message::Envelope & lhs, const Message::Envelope & rhs) {
		return rhs < lhs;
	}
	bool operator<=(const Message::Envelope & lhs, const Message::Envelope & rhs) {
		return !(rhs < lhs);
	}
	bool operator>=(const Message::Envelope & lhs, const Message::Envelope & rhs) {
		return !(lhs < rhs);
	}
}