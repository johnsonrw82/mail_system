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
	namespace {
		// class to define the lookup capability for states
		class StateLookup {
		public:
			// map containing state key and full name
			std::map<std::string, const std::string *> _map;
			// track locale for performing transforms
			std::locale _locale;

			// Default constructor
			StateLookup();
		private:
			// state id structure
			struct StateId {
				// constructor
				StateId(const std::string code, const std::string name) : _code(std::move(code)), _name(std::move(name)) {}

				std::string _code;		// state code
				std::string _name;		// state name
			};

			// state values
			const std::vector<StateId> _stateIds = // map containing the state codes and their names
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
				{ "GU","Guam"},
				{ "VI","Virgin Islands" },
				{ "AS", "American Samoa" },
				{ "PR", "Puerto Rico" },
			};
		};

		// implement state lookup constructor
		StateLookup::StateLookup() {
			// function to add the value to the lookup map
			// perform mapping and transform here so it's done once at construction
			auto addToMap = [&](auto key, auto data) {
				if (!key.empty()) {
					// convert the key to lowercase and add to the map
					// supports case-insensitive lookup
					for ( auto & c : key ) { 
						c = std::tolower(c, _locale);
					}
					// add to map
					_map[key] = data;
				}
			};

			// for each state id, add a mapping that will be used to locate the state name
			for (const auto & stateId : _stateIds) {
				addToMap(stateId._code, &stateId._name);  // map case insensitive code to actual name
				addToMap(stateId._name, &stateId._name);  // map case insensitive name to actual name
			}
		}
	}


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
		_street = std::move(numbersAndName);

		return *this;
	}
	Address &   Address::city(std::string     name) noexcept {
		_city = std::move(name);

		return *this;
	}
	// Must be a valid two digit code, state name, or standard state abbreviation
	Address &   Address::state(std::string     code) {
		// state lookup class instance
		static const StateLookup states;
		// convert input to lowercase
		for (auto & c : code) {
			c = std::tolower(c, states._locale);
		}

		// find it in the map
		auto state = states._map.find(code);

		// if the value was found, set the state
		if (state != states._map.end() ) {
			_state = *(state->second); // use the value pulled from the map
		}
		// else throw exception
		else {
			throw StateCodeException("State not valid", __LINE__, __func__, __FILE__);
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
			_zip = std::move(code);
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

		// address strings
		std::string street;
		std::string city;
		std::string state;
		std::string zip;
		
		// try to get each field from the stream
		if (std::getline(s, street, Address::FIELD_SEPARATOR) &&
			std::getline(s, city, Address::FIELD_SEPARATOR) &&
			std::getline(s, state, Address::FIELD_SEPARATOR) &&
			std::getline(s, zip, Address::RECORD_SEPARATOR)) {

			try {
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
		return &lhs == &rhs ||
			(lhs._state == rhs._state &&
				lhs._city == rhs._city &&
				lhs._zip.substr(0, 5) == rhs._zip.substr(0, 5) &&
				lhs._street == rhs._street);
	}

	// not equal
	bool operator!=(const Address & lhs, const Address & rhs) {
		return !(lhs == rhs);
	}

	// less than
	// fixed this because the old implementation was causing problems when inserting into a set
	bool operator< (const Address & lhs, const Address & rhs) {
		int result = 0;

		// self compare (same address)
		if (&lhs == &rhs) {
			return false;
		}

		// use of compare function will return a value corresponding to the equality
		else if ( (result = lhs._state.compare(rhs._state)) != 0) return result < 0;
		else if ((result = lhs._city.compare(rhs._city)) != 0) return result < 0;
		else if ((result = lhs._zip.compare(0,5,rhs._zip, 0, 5)) != 0) return result < 0;
		
		return lhs._street < rhs._street;
	}

	// greater than
	bool operator> (const Address & lhs, const Address & rhs) {
		return (rhs < lhs);
	}

	// less than or equal
	bool operator<=(const Address & lhs, const Address & rhs) {
		return !(rhs < lhs);
	}

	// greater than or equal
	bool operator>=(const Address & lhs, const Address & rhs) {
		return !(lhs < rhs);
	}
}