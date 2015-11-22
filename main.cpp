/******************************************************************************
** (C) Copyright 2015 by Thomas Bettens. All Rights Reserved.
**
** DISCLAIMER: The authors have used their best efforts in preparing this
** code. These efforts include the development, research, and testing of the
** theories and programs to determine their effectiveness. The authors make no
** warranty of any kind, expressed or implied, with regard to these programs or
** to the documentation contained within. The authors shall not be liable in
** any event for incidental or consequential damages in connection with, or
** arising out of, the furnishing, performance, or use of these libraries and
** programs.  Distribution without written consent from the authors is
** prohibited.
******************************************************************************/

/**************************************************
** Intermediate C++ Mail System Project Possible Solution
**
** Thomas Bettens
** Last modified:  26-April-2015
** Last Verified:  12-June-2015
** Verified with:  VC++2015 RC, GCC 5.1,  Clang 3.5
***************************************************/

#include <algorithm>
#include <exception>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <type_traits>
#include <set>
#include <list>

#include "Employees/Employee.hpp"

#include "Utilities/Exceptions.hpp"

#include "Common/EmployeeMessageDB.hpp"

#include "MailSystem/Message.hpp"
#include "MailSystem/Document.hpp"
#include "MailSystem/MessageDB.hpp"



namespace // unnamed, anonymous namespace for internal (local) linkage
{	
	struct RegressionTestException : Utilities::AbstractException<> { using AbstractException::AbstractException; };              // Regression test exception base class
	struct   UndetectedException : RegressionTestException { using RegressionTestException::RegressionTestException; };  // Inherit base class constructors
	struct   RelationalTestFailure : RegressionTestException { using RegressionTestException::RegressionTestException; };
	struct   SemmetricalIOFailure : RegressionTestException { using RegressionTestException::RegressionTestException; };
	struct   PropertyValueException : RegressionTestException { using RegressionTestException::RegressionTestException; };         // new exception used to mark when a property is incorrectly set
	struct   InvalidStateException : RegressionTestException { using RegressionTestException::RegressionTestException; };  // used when the object is in an invalid state

	// template function to get a database record (or a new insertion if not found)
	template <typename T>
	typename T::Value_t & getDatabaseRecord(typename T::Key dbKey) {
		auto & database = T::getInstance();

		// try to look up the record in the database
		try {
			return database[dbKey];
		}
		catch (Utilities::RecordLookupException &) {}

		// if here, then the record wasn't found, let's make one and then return it
		database.insert(dbKey, {});
		return database[dbKey];
	}

	// template function to lookup a key by value
	template <typename T>
	typename T::Key findByRecordData(typename T::Value_t & recordData) {
		// get database
		auto & database = T::getInstance();

		//  loop over the keys and if the employee is in there, return the key
		for (auto & key : database.keys()) {
			if (database[key] == recordData) {
				return key;
			}
		}
		// if not found, return the new key by inserting
		return database.insert(recordData);
	}

    // mail system test use case 1
    void testMailSystemUC1() {
	  auto & eDB = Employees::EmployeeDB::getInstance();
	  auto & mDB = MailSystem::MessageDB::getInstance();

	  // Use Case 1 - Tom, Sue, and Rob
	  // Tom sends a message with attachment to Sue and copies Rob
	  Employees::Employee tom = { "Tom" };
	  Employees::Employee sue = { "Sue" };
	  Employees::Employee rob = { "Rob" };

	  // find out if they are already in the database
	  auto findEmployee = [](Employees::Employee & emp)->Employees::EmployeeDB::Key {
		  return findByRecordData<Employees::EmployeeDB>(emp);
	  };

	  // keys for tom, sue, and rob
	  auto tomKey = findEmployee(tom);
	  auto sueKey = findEmployee(sue);
	  auto robKey = findEmployee(rob);

	  // construct the message - from, to, cc
	  MailSystem::Message fired({ { tomKey },{ sueKey }, {robKey} }, "Hello Sue,\nI just wanted to let you know that you're fired.\nSee attached performance report.\nI'm too chicken to do this in person, so you get an email. Sorry. :(\n");

	  // add attachment
	  MailSystem::Document performanceReport("Customer Relations = POOR\nEmployee Contributions = POOR\nProject Manangement = POOR\nHygeine = FAIR\n");
	  fired.addAttachment("Performance Report", performanceReport);

	  // send the message
	  fired.send();

	  // small lambda function to look up a mailbox
	  // built around a generic template function for looking up database records
	  auto getMailBox = [](auto mailboxKey)->Common::EmployeeMessageKeyDB::Value_t & {
		  return getDatabaseRecord<Common::EmployeeMessageKeyDB>(mailboxKey);
	  };

	  // get the sent/inbox database records
	  auto & sentBox = getMailBox(MailSystem::MailBoxes::SENT);
	  auto & inbox = getMailBox(MailSystem::MailBoxes::IN);

	  // loop through them and show the contents
	  for (const auto & emp : { tomKey, sueKey, robKey }) {
		  // outbox
		  std::cout << eDB[emp].name() << "'s Sent Items:\n\n";
		  for (const auto & msg : sentBox.retrieveRight(emp)) {
			  std::cout << mDB[msg].body() << '\n';
			  std::cout << "With attachments:\n\n";
			  for (const auto & attachment : mDB[msg].attachments()) {
				  std::cout << "Attachment name: " << attachment << '\n' << mDB[msg].attachment(attachment) << '\n';
			  }
		  }

		  // inbox
		  std::cout << eDB[emp].name() << "'s Received Items:\n\n";
		  for (const auto & msg : inbox.retrieveRight(emp)) {
			  std::cout << mDB[msg].body() << '\n';
			  std::cout << "With attachments:\n\n";
			  for (const auto & attachment : mDB[msg].attachments()) {
				  std::cout << "Attachment name: " << attachment << '\n' << mDB[msg].attachment(attachment) << '\n';
			  }
		  }
	  }

	  // test symmetric sync
	  std::stringstream ss;
	  ss << fired;

	  MailSystem::Message temp;
	  ss >> temp;

	  if (fired != temp) {
		  throw SemmetricalIOFailure("Message symmetric IO failure", __LINE__, __func__, __FILE__);
	  }

	  // let's try finding an attachment that doesn't exist
	  try {
		  MailSystem::Document att = fired.attachment("Not there");
		  throw UndetectedException("Attachment with that name does not exist", __LINE__, __func__, __FILE__);
	  }
	  catch (MailSystem::Message::AttachmentNotFoundException &) {}

	  // get the real attachment
	  try {
		  MailSystem::Document att = fired.attachment("Performance Report");

		  // compare just to make sure
		  if (att != performanceReport) {
			  throw RelationalTestFailure("Attachments not equal", __LINE__, __func__, __FILE__);
		  }
	  }
	  catch (MailSystem::Message::AttachmentNotFoundException & ex) {
		  throw InvalidStateException(ex, "Attachment should have been retreived", __LINE__, __func__, __FILE__);
	  }

	  // success
	  std::cout << "Success:  " << __func__ << "\v\n";
    }

    // mail system test use case 2
    void testMailSystemUC2() {
	  auto & mDB = MailSystem::MessageDB::getInstance();

	  // Use Case 2, tom decides to delete his message sent in the previous use case.
	  // the system admin then removes the message from the inboxes of the recipients. 
	  // He recalled his nasty message...
	  Employees::Employee tom = { "Tom" };
	  Employees::Employee sue = { "Sue" };
	  Employees::Employee rob = { "Rob" };

	  // find out if they are already in the database
	  auto findEmployee = [](Employees::Employee & emp)->Employees::EmployeeDB::Key {
		  return findByRecordData<Employees::EmployeeDB>(emp);
	  };

	  // keys for tom, sue, and rob
	  auto tomKey = findEmployee(tom);
	  auto sueKey = findEmployee(sue);
	  auto robKey = findEmployee(rob);

	  // small lambda function to look up a mailbox
	  // built around a generic template function for looking up database records
	  auto getMailBox = [](auto mailboxKey)->Common::EmployeeMessageKeyDB::Value_t & {
		  return getDatabaseRecord<Common::EmployeeMessageKeyDB>(mailboxKey);
	  };

	  // get the sent/inbox database records
	  auto & sentBox = getMailBox(MailSystem::MailBoxes::SENT);
	  auto & inbox = getMailBox(MailSystem::MailBoxes::IN);

	  // find the message ID from Tom's sent box -- assume it's his only sent item
	  auto tomsMessageKey = *(sentBox.retrieveRight(tomKey).cbegin());
	  std::size_t tomsSize = sentBox.retrieveRight(tomKey).size();

	  // display his message -- out to stdout
	  std::cout << "Tom's message:\n" << mDB[tomsMessageKey] << '\n';

	  // now he deletes it from his sent items
	  sentBox.remove(tomKey, tomsMessageKey);

	  // verify it's not there	  
	  if (sentBox.retrieveRight(tomKey).size() != tomsSize - 1) {
		  throw InvalidStateException("Message should have been removed from box.", __LINE__, __func__, __FILE__);
	  }

	  // now the system admin removes the message from the inbox of sue and rob
	  for (const auto & emp : { sueKey, robKey }) {
		  // get size
		  std::size_t boxSize = inbox.retrieveRight(emp).size();

		  inbox.remove(emp, tomsMessageKey);
		  // assert the size is reduced
		  if (inbox.retrieveRight(emp).size() != boxSize - 1) {
			  throw InvalidStateException("Message should have been removed from box.", __LINE__, __func__, __FILE__);
		  }
	  }

	  // now, the admin removes the message from the databse. as if it never happened
	  mDB.remove(tomsMessageKey);
	  
	  // assert it's not there now
	  try {
		  mDB[tomsMessageKey];
		  throw UndetectedException("Record lookup should have failed.", __LINE__, __func__, __FILE__);
	  }
	  catch (Utilities::RecordLookupException &) {}

	  // success
	  std::cout << "Success:  " << __func__ << "\v\n";
    }

    // use case 3 for mail system
    void testMailSystemUC3() {
	  // use case 3: Bill has a bunch of messages he wants to send to a single group of people
	  // he will make a set of messages and then send them to this group all at once
	  // he's going to send them to everyone in the employee database, including himself

	  // employee database
	  auto & eDB = Employees::EmployeeDB::getInstance();
	  // message database
	  auto & mDB = MailSystem::MessageDB::getInstance();

	  Employees::Employee bill = { "Bill" };

	  // find out if they are already in the database
	  auto findEmployee = [](Employees::Employee & emp)->Employees::EmployeeDB::Key {
		  return findByRecordData<Employees::EmployeeDB>(emp);
	  };

	  // bill's key
	  auto billKey = findEmployee(bill);

	  // bill constructs his distribution list -- bcc'ing everyone
	  MailSystem::Message::Envelope billsEnvelope({ billKey }, {}, {}, { 0UL, 2UL });
	  billsEnvelope.from = billKey;
	  
	  for (const auto & key : eDB.keys()) {
		  billsEnvelope.bcc.insert(key);
	  }

	  // bill's messages
	  std::list<MailSystem::Message> billsMessages = {
		  {"This is my shopping list:\n- eggs\n- ointment\n- vanilla wafers\n- frosting\n"},
		  {"You all need to stop sleeping at work.\n"},
		  {"I like turtles\n"},
		  {"Disgregard that last message, someone hacked my account.\n"}
	  };

	  // send them all!
	  for (auto & msg : billsMessages) {
		  msg.send(billsEnvelope);
	  }

	  // small lambda function to look up a mailbox
	  // built around a generic template function for looking up database records
	  auto getMailBox = [](auto mailboxKey)->Common::EmployeeMessageKeyDB::Value_t & {
		  return getDatabaseRecord<Common::EmployeeMessageKeyDB>(mailboxKey);
	  };

	  // get the inbox database records
	  auto & inbox = getMailBox(MailSystem::MailBoxes::IN);

	  // now let's loop through everyone's inboxes
	  for (const auto & emp : billsEnvelope.all()) {
		  std::cout << eDB[emp].name() << "'s Inbox contains:\n";
		  unsigned int msgCount = 0;
		  for (const auto & msg : inbox.retrieveRight(emp)) {
			  std::cout << mDB[msg].body() << '\n';

			  // lets make sure they got all of bills messages!
			  if (std::find(billsMessages.cbegin(), billsMessages.cend(), mDB[msg]) != billsMessages.cend()) {
				  msgCount++;
			  }
		  }
		  // if the counts aren't equal, then a message was missed
		  if (msgCount != billsMessages.size()) {
			  throw InvalidStateException("A message was not passed to the inbox of a recipient!", __LINE__, __func__, __FILE__);
		  }
	  }

	  // success
	  std::cout << "Success:  " << __func__ << "\v\n";
  }

}// unnamed, anonymous namespace

int main()
{
  try
  {
    const auto seperator = std::string( 80, '=' );
	::testMailSystemUC1();
	std::cout << seperator << '\n';

	::testMailSystemUC2();
	std::cout << seperator << '\n';

	::testMailSystemUC3();
	std::cout << seperator << '\n';

    std::cout << "Success:  " << __func__ << "\v\n";
  }
  catch( std::exception & ex )
  {
    std::cerr << "\n\nFailure - \n" << ex.what() << "\n\n";
  }
}
