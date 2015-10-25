/**
 * File: Address.cpp
 * Author: Ryan Johnson
 * Email: johnsonrw82@csu.fullerton.edu
 *
 * Description: This file is the class implementation for an Address class.
 *				An Address consists of a Street, City, State, and Zip code
 **/

#include <string>
#include <regex>
#include <sstream>
#include <iostream>
#include <map>
#include <algorithm>
#include <locale>

#include "Addresses/Address.hpp"

namespace Addresses {

	// constructors
	Address::Address(std::string   street,
		std::string   city,
		const std::string & stateCode,
		const std::string & zip) {

		// set the street and city
		this->street(street);
		this->city(city);

		// try to set the state and zip
		try {
			this->state(stateCode);
			this->zipCode(zip);
		}
		// build up the stack trace
		catch (StateCodeException & ex) {
			throw StateCodeException(ex, "", __LINE__, __func__, __FILE__);
		}
		catch (ZipCodeException & ex) {
			throw ZipCodeException(ex, "", __LINE__, __func__, __FILE__);
		}
	}

	Address::Address(std::string      street,
		std::string      city,
		const std::string &    stateCode,
		unsigned long    zip) {

		// set the street and city
		this->street(street);
		this->city(city);

		// try to set the state and zip
		try {
			this->state(stateCode);
			this->zipCode(zip);
		}
		// build up the stack trace
		catch (StateCodeException & ex) {
			throw StateCodeException(ex, "", __LINE__, __func__, __FILE__);
		}
		catch (ZipCodeException & ex) {
			throw ZipCodeException(ex, "", __LINE__, __func__, __FILE__);
		}
	}

	/****************************
	* Modifier section
	*****************************/
	Address &   Address::street(std::string     numbersAndName) noexcept {
		_street = numbersAndName;

		return *this;
	}
	Address &   Address::city(std::string     name) noexcept {
		_city = name;

		return *this;
	}
	// Must be a valid two digit code, state name, or standard state abbreviation
	Address &   Address::state(std::string     code) {

		// map containing the state codes and their names
		const std::map<std::string, std::string> states = 
		{
			{ "AL","Alabama" },
			{ "AK","Alaska" },
			{ "AZ","Arizona" },
			{ "AR","Arkansas" },
			{ "CA","California" },
			{ "CO","Colorado" },
			{ "CT","Connecticut" },
			{ "DE","Delaware" },
			{ "FL","Florida" },
			{ "GA","Georgia" },
			{ "HI","Hawaii" },
			{ "ID","Idaho" },
			{ "IL","Illinois" },
			{ "IN","Indiana" },
			{ "IA","Iowa" },
			{ "KS","Kansas" },
			{ "KY","Kentucky" },
			{ "LA","Louisiana" },
			{ "ME","Maine" },
			{ "MD","Maryland" },
			{ "MA","Massachusetts" },
			{ "MI","Michigan" },
			{ "MN","Minnesota" },
			{ "MS","Mississippi" },
			{ "MO","Missouri" },
			{ "MT","Montana" },
			{ "NE","Nebraska" },
			{ "NV","Nevada" },
			{ "NH","New Hampshire" },
			{ "NJ","New Jersey" },
			{ "NM","New Mexico" },
			{ "NY","New York" },
			{ "NC","North Carolina" },
			{ "ND","North Dakota" },
			{ "OH","Ohio" },
			{ "OK","Oklahoma" },
			{ "OR","Oregon" },
			{ "PA","Pennsylvania" },
			{ "RI","Rhode Island" },
			{ "SC","South Carolina" },
			{ "SD","South Dakota" },
			{ "TN","Tennessee" },
			{ "TX","Texas" },
			{ "UT","Utah" },
			{ "VT","Vermont" },
			{ "VA","Virginia" },
			{ "WA","Washington" },
			{ "WV","West Virginia" },
			{ "WI","Wisconsin" },
			{ "WY","Wyoming" },
			{ "DC","District of Columbia" },
		};

		// conversion functions
		auto toupper = [](char c) {
			return std::toupper(c, std::locale());
		};
		auto tolower = [](char c) {
			return std::tolower(c, std::locale());
		};

		// if the length is 2, it's an abbreviation. Look it up in the map
		if (code.length() == 2) {
			// convert to uppercase -- supports input such as "Wa" or "wa"
			std::transform(code.begin(), code.end(), code.begin(), toupper);

			std::map<std::string, std::string>::const_iterator itr = states.find(code);

			// if the key was found
			if (itr != states.cend()) {
				_state = itr->second;
			}
			// throw an exception
			else {
				throw StateCodeException("State abbreviation not valid", __LINE__, __func__, __FILE__);
			}
		}
		// it's not an abbreviation, look up the long name and make sure it's valid
		else {
			// convert to first letter capitalized, supporting input such as "WAshington" or "washington"
			std::transform(code.begin(), code.begin() + 1, code.begin(), toupper);
			std::transform(code.begin() + 1, code.end(), code.begin() + 1, tolower);

			// function to return whether or not the value matches the function parameter supplied
			auto nameExists = [code,toupper,tolower](auto value) {
				// convert the value in map to same format as code
				std::transform(value.second.begin(), value.second.begin() + 1, value.second.begin(), toupper);
				std::transform(value.second.begin() + 1, value.second.end(), value.second.begin() + 1, tolower);

				// compare
				return value.second == code;
			};

			// find the state in the map
			std::map<std::string, std::string>::const_iterator itr = std::find_if(states.cbegin(), states.cend(), nameExists);

			// the long name was found in the map
			if (itr != states.cend()) {
				_state = itr->second; // use the value pulled from the map
			}
			// throw exception
			else {
				throw StateCodeException("State name not valid", __LINE__, __func__, __FILE__);
			}
		}

		return *this;
	}

	// Zip code rules:  5 digits, not all are zero and not all are nine, optionally followed
	// by a hyphen and 4 digits, not all are zero and not all are nine.
	Address &   Address::zipCode(std::string     code) {
		// regex to validate the zip code (raw string delimiter /( )/ )
		std::regex zipRegex( R"/((?!0{5})(?!9{5})\d{5}(-(?!0{4})(?!9{4})\d{4})?)/" );

		// if the match is successful, assign it
		if (std::regex_match(code, zipRegex)) {
			_zip = code;
		}
		// else, throw exception
		else {
			throw ZipCodeException("Invalid zip code string entered", __LINE__, __func__, __FILE__);
		}

		return *this;
	}
	
	Address &   Address::zipCode(unsigned long   code) {
		try {
			// convert to std::string
			std::string zipStr(std::to_string(code));

			// need to pad the code with 0's if the zip code is less than 10000
			std::string pad(5 - zipStr.length(), '0');
			zipStr = pad + zipStr;

			// call std::string version
			zipCode(zipStr);
		}
		catch (ZipCodeException & ex) {
			throw ZipCodeException(ex, "Invalid zip code long value entered", __LINE__, __func__, __FILE__);
		}

		return *this;
	}

	/********************
	 * Queries
	 ********************/
	std::string Address::street() const noexcept {
		return _street;
	}
	std::string Address::city() const noexcept {
		return _city;
	}
	std::string Address::state() const noexcept {
		return _state;
	}
	std::string Address::zipCode() const noexcept {
		return _zip;
	}

	// conversion operator
	Address::operator std::string() const
	{
		// use the << operator to produce a stringstream
		std::ostringstream oss;
		oss << *this;

		// return the string representation
		return oss.str();
	}

	/***********************
	* stream operators
	**********************/
	// << operator overload
	std::ostream & operator<< (std::ostream & s, const Address & address) {
		// construct the string from each field
		s << address.street() <<
			Address::FIELD_SEPARATOR <<
			address.city() <<
			Address::FIELD_SEPARATOR <<
			address.state() <<
			Address::FIELD_SEPARATOR <<
			address.zipCode() <<
			Address::RECORD_SEPARATOR;

		return s;
	}

	// >> operator overload
	std::istream & operator>> (std::istream & s, Address & address) {
		using StateCodeException = Address::StateCodeException;
		using ZipCodeException = Address::ZipCodeException;

		// record string
		std::string record;

		// get the record from the stream
		if (std::getline(s, record, Address::RECORD_SEPARATOR)) {
			try {
				// convert record to stream and extract each piece
				std::stringstream ss(record);
				std::string street;
				std::string city;
				std::string state;
				std::string zip;

				// get the data from the stream, using the appropriate delimiter
				std::getline(ss, street, Address::FIELD_SEPARATOR);
				std::getline(ss, city, Address::FIELD_SEPARATOR);
				std::getline(ss, state, Address::FIELD_SEPARATOR);
				std::getline(ss, zip, Address::FIELD_SEPARATOR);

				// construct the object from the supplied data
				address = Address(street, city, state, zip);
			}
			catch (StateCodeException & ex) {
				throw StateCodeException(ex, "", __LINE__, __func__, __FILE__);
			}
			catch (ZipCodeException & ex) {
				throw ZipCodeException(ex, "", __LINE__, __func__, __FILE__);
			}
		}

		return s;
	}

	// pointer stream overloads
	std::ostream & operator<< (std::ostream & s, const Address * address) {
		if (address != nullptr) {
			s << *address;
		}

		return s;
	}
	std::istream & operator>> (std::istream & s, Address * address) {
		if (address != nullptr) {
			s >> *address;
		}

		return s;
	}

	/***********************
	 * comparison operators
	 **********************/
	// equals
	bool operator==(const Address & lhs, const Address & rhs) {
		return lhs.state() == rhs.state() &&
			lhs.city() == rhs.city() &&
			lhs.zipCode().substr(0,5) == rhs.zipCode().substr(0,5) &&
			lhs.street() == rhs.street();
	}

	// not equal
	bool operator!=(const Address & lhs, const Address & rhs) {
		return !(lhs == rhs);
	}

	// less than
	bool operator< (const Address & lhs, const Address & rhs) {
		return lhs.state() < rhs.state() &&
			lhs.city() < rhs.city() &&
			lhs.zipCode().substr(0, 5) < rhs.zipCode().substr(0, 5) &&
			lhs.street() < rhs.street();
	}

	// greater than
	bool operator> (const Address & lhs, const Address & rhs) {
		return lhs.state() > rhs.state() &&
			lhs.city() > rhs.city() &&
			lhs.zipCode().substr(0, 5) > rhs.zipCode().substr(0, 5) &&
			lhs.street() > rhs.street();
	}

	// less than or equal
	bool operator<=(const Address & lhs, const Address & rhs) {
		return lhs.state() <= rhs.state() &&
			lhs.city() <= rhs.city() &&
			lhs.zipCode().substr(0, 5) <= rhs.zipCode().substr(0, 5) &&
			lhs.street() <= rhs.street();
	}

	// greater than or equal
	bool operator>=(const Address & lhs, const Address & rhs) {
		return lhs.state() >= rhs.state() &&
			lhs.city() >= rhs.city() &&
			lhs.zipCode().substr(0, 5) >= rhs.zipCode().substr(0, 5) &&
			lhs.street() >= rhs.street();
	}
}