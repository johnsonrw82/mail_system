/**
 * File: Company.cpp
 * Author: Ryan Johnson
 * Email: johnsonrw82@csu.fullerton.edu
 *
 * Description: This file is the class implementation for a Company class.
 *				A company consists of a free-text name.
 **/

#include <string>
#include <sstream>
#include <iostream>

#include "Companies/Company.hpp"


namespace Companies {
	// Company headquarters is located at the address with this key.
	Company::Company(std::string name) {
		this->name(name);
	}


	/**********************
	* Queries
	**********************/
	std::string Company::name() const noexcept {
		return _name;
	}


	/**********************
	* Conversions
	**********************/
	Company::operator std::string() const {
		std::ostringstream oss;
		oss << *this;
		return oss.str();
	}


	/**********************
	* Modifiers
	**********************/
	Company & Company::name(std::string newName)  noexcept {
		_name = std::move(newName);

		return *this;
	}

	/**********************
	* Stream operators
	**********************/
	// reference stream overloads
	std::ostream & operator<< (std::ostream & s, const Company & company) {
		s << company.name() <<
			Company::RECORD_SEPARATOR;
		return s;
	}
	std::istream & operator>> (std::istream & s, Company & company) {
		// company name
		std::string name;

		// try to get the name from the stream
		if (std::getline(s, name, Company::RECORD_SEPARATOR)) {
			company = Company(name);
		}
		return s;
	}
	// pointer stream overloads
	std::ostream & operator<< (std::ostream & s, const Company * company) {
		if (company != nullptr) {
			s << *company;
		}

		return s;
	}
	std::istream & operator>> (std::istream & s, Company * company) {
		if (company != nullptr) {
			s >> *company;
		}

		return s;
	}

	/**********************
	 * Logical operators
	 **********************/
	// equal to
	bool operator==(const Company & lhs, const Company & rhs) {
		return lhs.name() == rhs.name();
	}
	// less than
	bool operator< (const Company & lhs, const Company & rhs) {
		return lhs.name() < rhs.name();
	}
	// not equal to
	bool operator!=(const Company & lhs, const Company & rhs) {
		return !(lhs == rhs);
	}
	// greater than
	bool operator> (const Company & lhs, const Company & rhs) {
		return lhs.name() > rhs.name();
	}
	// less than or equal
	bool operator<=(const Company & lhs, const Company & rhs) {
		return lhs.name() <= rhs.name();
	}
	// greater than or equal
	bool operator>=(const Company & lhs, const Company & rhs) {
		return lhs.name() >= rhs.name();
	}
}