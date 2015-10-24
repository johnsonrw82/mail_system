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
		_name = newName;

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
		// record string
		std::string record;
		// try to get the record from the stream
		if (std::getline(s, record, Company::RECORD_SEPARATOR)) {
			// convert to stringstream
			std::stringstream ss(record);

			std::string name;

			// get the data from the stream using the proper delimiter
			if (std::getline(ss, name, Company::FIELD_SEPARATOR)) {

				// construct the company object
				company = Company(name);
			}
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