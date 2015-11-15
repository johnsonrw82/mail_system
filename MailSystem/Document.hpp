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
** Intermediate C++ Homework Assignment #2 Possible Solution
**
** Thomas Bettens
** Last modified:  04-April-2015
** Last Verified:  04-April-2015
** Verified with:  VC++2015 CTP 6, GCC 4.9.2,  Clang 3.5
***************************************************/


#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include <cstddef>      // size_t
#include <iostream>
#include <stack>
#include <string>

#include "Utilities/Exceptions.hpp"

namespace MailSystem
{
  class Document
  {
    friend std::ostream & operator<< (std::ostream & s, const Document & document);
	friend std::istream & operator>> (std::istream & s, Document & document);

	friend bool operator==(const Document & lhs, const Document & rhs);
	friend bool operator< (const Document & lhs, const Document & rhs);

    public:
	  // exceptions
		struct DocumentExceptions : Utilities::AbstractException<> { using AbstractException::AbstractException;  };
		struct SymmetricSyncException : DocumentExceptions { using DocumentExceptions::DocumentExceptions;  };

      // Constructors and destructor
      Document();
      Document( std::istream & in );
      Document( const std::string & phrase );

      Document( const Document &  other );
      Document(       Document && other );

      Document & operator=(const Document &  rhs);
      Document & operator=(      Document && rhs);

      ~Document() noexcept;



      // Load this object with the contents of a stream
      void load(std::istream & stream = std::cin);


      // Snapshot a point in time the document can be restored to later
      void snap();
      void undo( std::size_t quantity = 1U);


      // Copy a section of the document so it can be inserted later
      void copy_range   (const std::string & startingPhrase, const std::string & endingPhrase); 
      void paste_after  (const std::string & pasteAfterPhrase);                              


      // Remove all occurrences of a phrase. The assignment wants a delete function, but "delete" is a C++ reserved key word.  So,
      // following the STL's precedence, I used erase instead.
      void erase        (const std::string & phrase);


      // Replace all occurrences of a phrase with another phrase
      void substitute   (const std::string & oldPhrase, const std::string & newPhrase);


      // Displays word statistics in table format
      void showStats( std::ostream & stream = std::cout );


    private:
      std::string               _document;
      std::stack<std::string>   _snapshots;      // Undo buffer allowing multiple levels of undo
      std::string               _copy_buffer;    // Copy buffer holding a copy of some portion of the document

	  // Class attributes
	  static constexpr char FIELD_SEPARATOR = '\x03';  // ETX (End of Text) character
	  static constexpr char RECORD_SEPARATOR = '\x04';  // EOT (End of Transmission) character ('\n' turns out to not be a good choice because data
  };  // class Document

  // Non-member functions - document
  bool operator==(const Document & lhs, const Document & rhs);
  bool operator< (const Document & lhs, const Document & rhs);
  bool operator!=(const Document & lhs, const Document & rhs);
  bool operator> (const Document & lhs, const Document & rhs);
  bool operator<=(const Document & lhs, const Document & rhs);
  bool operator>=(const Document & lhs, const Document & rhs);
}  // namespace MailSystem





// Inline functions and template definitions must be part of the header file. To help keep the size of the header file manageable
// and to keep implementation definitions separated from interface declaration, the header file has been divided into two.
// Library/Document.hxx is not intended to be included by clients and serves only to allow Document and related declarations to be
// separated from implementation whist making inline function and template definitions visible to all translation units. This
// technique is primary used as a teaching tool. I don't see this in real-world applications often, but it's out there. It allows me
// to distribute the declarations without the definitions. Normally, the contents of "Library/Document.hxx" would be physically
// entered below instead of telling the pre-processor to include the contents.
#include "MailSystem/Document.hxx"       // inline and function Document definitions

#endif
