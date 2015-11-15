/**
* File:		Document.cpp
* Author:	Ryan Johnson
* Email:	johnsonrw82@cs.fullerton.edu
* Purpose:	This file is the implementation of the non-inline document functions.
*/

#include <unordered_map>
#include <set>
#include <locale>

#include "MailSystem/Document.hpp"

namespace MailSystem {
        // default constructor
	Document::Document() = default;
	
	// constructor initializing with input stream
	Document::Document(std::istream & in) {
		load(in);
	}

	// constructor initializing with string
	Document::Document(const std::string & phrase) {
		// construct an input stringstream and load with that
		std::istringstream iss(phrase);
		load(iss);
	}

	// copy constructor
	Document::Document(const Document & other) : 
		_document(other._document), _snapshots(other._snapshots), _copy_buffer(other._copy_buffer) {}

	// rvalue reference copy constructor
	Document::Document(Document && other) : 
		_document(other._document), _snapshots(other._snapshots), _copy_buffer(other._copy_buffer) {}

	// destructor
	Document::~Document() noexcept
	{

	}
        
        // operator overloads ------------------------------------------------
	// operator = overload for lvalue reference
	// use the copy-and-swap idiom, to guarantee that no exception will be thrown during construction
	Document & Document::operator=(const Document &  rhs) {
		Document copy(rhs);  // copy
		// swap
		std::swap(_copy_buffer, copy._copy_buffer);
		std::swap(_snapshots, copy._snapshots);
		std::swap(_document, copy._document);
		// return
		return *this;
	}

	// operator = overload for rvalue reference
	Document & Document::operator=(Document &&  rhs) {
		this->_copy_buffer = std::move(rhs._copy_buffer);
		this->_document = std::move(rhs._document);
		this->_snapshots = std::move(rhs._snapshots);		

		return *this;
	}
        
        // copy a range of text between start phrase and end phrase
	void Document::copy_range(const std::string & startingPhrase, const std::string & endingPhrase) {
		// determine if the phrases are valid
		if ( !(startingPhrase.empty() || endingPhrase.empty()) ) {
			// use the string find function to find the phrase positions
                        auto startPos = _document.find(startingPhrase);
                    
                        // if the start pos is not the end of the string
                        if ( startPos != std::string::npos) {
                                auto endPos = _document.find(endingPhrase, startPos + startingPhrase.length());
                                if ( endPos != std::string::npos ) {
                                        // if positions are good, assign the substring to _copy_buffer
                                        // length of substring is endPos - startPos + endPhrase length
                                        _copy_buffer = _document.substr(startPos, ((endPos + endingPhrase.length()) - startPos));
                                }
                        }
		}
	}

	// paste the contents of copy_buffer after the occurence of phrase
	void Document::paste_after(const std::string & pasteAfterPhrase) {
		// check if the phrase is empty
		if (!pasteAfterPhrase.empty()) {
			// find the position of pasteAfter phrase
                        auto pos = _document.find(pasteAfterPhrase);
                        if ( pos != std::string::npos ) {
                                _document.insert(pos + pasteAfterPhrase.length(), _copy_buffer);
                        }
		}
	}

	// erase all instances of phrase
	void Document::erase(const std::string & phrase) {
		// check to make sure phrase is valid
		if (!phrase.empty()) {
                        // look for the phrase
                        auto pos = _document.find(phrase);

                        // while position is valid
			while (pos != std::string::npos) {
				// erase the word
                                _document.erase(pos, phrase.length());
                                
				// search from the returned position
                                pos = _document.find(phrase, pos);
			}
		}
	}

	// substitute all instances of a phrase with another phrase
	void Document::substitute(const std::string & oldPhrase, const std::string & newPhrase) {
		// check validity
		if (!(oldPhrase.empty() || newPhrase.empty())) {
			// find position of old phrase
                        auto oldPos = _document.find(oldPhrase);

			// if we're not at the end of the document
			while (oldPos != std::string::npos) {
				// erase the old phrase
				_document.erase(oldPos, oldPhrase.length());
				// insert the new one at the position returned from last operation
				_document.insert(oldPos, newPhrase);

				// keep searching for the phrase
				oldPos = _document.find(oldPhrase, oldPos + newPhrase.length());
			}
		}

	}
        
        // show statistics for this document, printed to supplied ostream
       	void Document::showStats(std::ostream & stream) {
                if ( _document.empty() ) {
                        stream << "Document is empty, no stats generated\n";
                        return;
                }
                
                // type alias for container type
                using StatContainerType = std::unordered_map<std::string, std::string::size_type>;
            
                // statistics container
                StatContainerType statistics;
                std::string::size_type maxOcc = 0;
                
                // define regular expression to match a string starting with an _ or letter, 
                // followed by zero or more alphanumeric characters
                std::regex wordRegex("[_a-zA-Z]\\w*");
                // read through each word and gather the statistics
                for ( std::sregex_iterator wordIterator(_document.cbegin(), _document.cend(), wordRegex);
                        wordIterator != std::sregex_iterator();
                        ++wordIterator ) {
                        // get the word from the iterator
                        std::string word(wordIterator->str());
                        
                        // transform the word to lowercase
                        // use lambda function to perform the transform and utilize the specified locale
                        std::transform(word.cbegin(), word.cend(), word.begin(), 
                                [](char c) { return std::tolower(c, std::locale() ); });
                        
                        // count occurrences
                        if ( ++statistics[word] > maxOcc ) { maxOcc = statistics[word]; }
                }
                
                
                
                // lambda compare function, used for ordering the set
                auto compareTo = [](const auto * lhs, const auto * rhs) {
                        // order by count first
                        if ( lhs->second != rhs->second ) {
                                return lhs->second > rhs->second;
                        }
                        // then by word
                        return lhs->first < rhs->first;
                };
                
                // create the chart object
                // use a set, storing pointers to pairs (determined by the stat container type)
                std::set<typename StatContainerType::value_type *, decltype(compareTo)> chart(compareTo);
                
                // insert pair pointers into the set, from the stat map
                for ( auto & pair : statistics ) {
                        chart.insert(&pair);
                }
                
                // save the flags of the stream
                auto fmtFlags = stream.flags();
                
                // format the chart, but need to establish some column sizing
                const unsigned int keySize = 20;
                const unsigned int countSize = 5;
                const unsigned int histSize = 90;
                
                // compute the scaled size based on max occurrences (ensure at least 1)
                std::string::size_type scaleValue = maxOcc / histSize + 1;
                
                // display the chart header
                // use setw to set the width, right/left to set alignment
                stream << std::setw(keySize) << std::right << "Word " << " | " <<
                        std::setw(countSize) << std::right << "Count" <<
                        std::left << "   (* is rounded to nearest " << scaleValue << " occurrences)\n" <<
                        std::string(keySize + countSize + histSize + 5, '-') << '\n';
                
                // chart is now sorted, let's display the info
                for ( const auto & pair : chart ) {
                        stream << std::setw(keySize) << std::right << pair->first.substr(0, keySize) <<
                                (pair->first.size() > keySize ? "* |" : " | ") << // let user know word was truncated, if so
                                std::setw(countSize) << std::right << pair->second <<
                                "   " << std::left << std::string((pair->second + scaleValue/2) / scaleValue, '*') << '\n';
                                
                }
                
                // restore
                stream.setf(fmtFlags);
        }
}
