/**
 * File: Employee.cpp
 * Author: Ryan Johnson
 * Email: johnsonrw82@csu.fullerton.edu
 *
 * Description: This file is the class implementation for an Employee class.
 *				An Employee consists of a last and (optional) first name.
 **/

#include <string>
#include <sstream>
#include <regex>
#include <iostream>

#include "Employees/Employee.hpp"

namespace Employees {
	/**********************
	* Constructors
	**********************/
	// last name [, first name]
	Employee::Employee(const std::string & name) {
		this->name(name);
	}
	Employee::Employee(std::string firstName, std::string lastName) noexcept {
		this->firstName(firstName);
		this->lastName(lastName);
	}


	/**********************
	* Queries
	**********************/
	std::string Employee::name()        const {
		std::ostringstream oss;
		oss << this->firstName() <<
			FIELD_SEPARATOR <<
			this->lastName() << 
			RECORD_SEPARATOR;

		return oss.str();
	}
	std::string Employee::firstName()   const {
		return _firstName;
	}
	std::string Employee::lastName()    const {
		return _lastName;
	}


	/**********************
	* Conversions
	**********************/
	Employee::operator std::string() const {
		std::ostringstream oss;
		oss << *this;
		return oss.str();
	}


	/**********************
	* Modifiers
	**********************/
	// last name [, first name]
	Employee & Employee::name(const std::string & newName)  noexcept {
		// find a delimiting comma
		std::regex nameRegex("([[:space:]]*,[[:space:]]*)");
		std::smatch match;

		// try to match the string using supplied regex
		if (std::regex_search(newName, match, nameRegex) ) {
			// the match was successful, the names will be prefix and suffix
			this->lastName(match.prefix());
			this->firstName(match.suffix());			
		}
		// no comma provided, last name only
		else {
			this->lastName(newName);
			this->firstName(""); // clear first name
		}

		return *this;
	}
	Employee & Employee::firstName(std::string   newName)  noexcept {
		// find a delimiting comma (just to make sure the name is properly formatted)
		std::regex nameRegex("([[:space:]]*,[[:space:]]*)");

		// try to match the string using supplied regex
		if (std::regex_search(newName, nameRegex)) {
			// call the name modifier instead
			this->name(newName);
		}
		else {
			_firstName = newName;
		}

		return *this;
	}
	Employee & Employee::lastName(std::string   newName)  noexcept {
		// find a delimiting comma (just to make sure the name is properly formatted)
		std::regex nameRegex("([[:space:]]*,[[:space:]]*)");		

		// try to match the string using supplied regex
		if (std::regex_search(newName, nameRegex)) {
			// call the name modifier instead
			this->name(newName);
		}
		else {
			_lastName = newName;
		}

		return *this;
	}

	/**********************
	* Logical operators
	**********************/
	// equal to
	bool operator==(const Employee & lhs, const Employee & rhs) {
		return lhs.lastName() == rhs.lastName() &&
			lhs.firstName() == rhs.firstName();
	}
	// less than
	bool operator< (const Employee & lhs, const Employee & rhs) {
		return lhs.lastName() < rhs.lastName() &&
			lhs.firstName() < rhs.firstName();
	}
	// not equal to
	bool operator!=(const Employee & lhs, const Employee & rhs) {
		return !(lhs == rhs);
	}
	// greater than
	bool operator> (const Employee & lhs, const Employee & rhs) {
		return lhs.lastName() > rhs.lastName() &&
			lhs.firstName() > rhs.firstName();
	}
	// less than or equal
	bool operator<=(const Employee & lhs, const Employee & rhs) {
		return lhs.lastName() <= rhs.lastName() &&
			lhs.firstName() <= rhs.firstName();
	}
	// greater than or equal
	bool operator>=(const Employee & lhs, const Employee & rhs) {
		return lhs.lastName() >= rhs.lastName() &&
			lhs.firstName() >= rhs.firstName();
	}

	/**********************
	* Stream operators
	**********************/
	// reference stream operators
	std::ostream & operator<< (std::ostream & s, const Employee & employee) {
		s << employee.name();

		return s;
	}
	std::istream & operator>> (std::istream & s, Employee & employee) {
		// record string
		std::string record;

		// try to get the record from the stream
		if (std::getline(s, record, Employee::RECORD_SEPARATOR)) {
			// convert to stringstream
			std::stringstream ss(record);

			std::string lastName;
			std::string firstName;

			// try to get first name first
			if (std::getline(ss, firstName, Employee::FIELD_SEPARATOR)) {
				// last name if separated by the field separator
				if (std::getline(ss, lastName, Employee::FIELD_SEPARATOR)) {
					employee = Employee(firstName, lastName);
				}
				// might be a comma separated name, try it out
				else {
					employee = Employee(firstName);
				}
			}
		}
		return s;
	}
	// pointer stream operators
	std::ostream & operator<< (std::ostream & s, const Employee * employee) {
		if (employee != nullptr) {
			s << *employee;
		}

		return s;
	}
	std::istream & operator>> (std::istream & s, Employee * employee) {
		if (employee != nullptr) {
			s >> *employee;
		}

		return s;
	}
}