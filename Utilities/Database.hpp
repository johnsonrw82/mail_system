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


#ifndef UTILITIES_Database_hpp
#define UTILITIES_Database_hpp

#include <cstddef>
#include <map>
#include <set>
#include <string>

#include "Utilities/Exceptions.hpp"
#include "Utilities/Key.hpp"




namespace Utilities
{
  // Exception Type Hierarchy Definition (Outside the template definitions so they can be caught a bit more generically
  struct DatabaseExceptions         : Utilities::AbstractException<> { using AbstractException::AbstractException;   }; // Class Address exception base class
  struct   RecordLookupException    : DatabaseExceptions             { using DatabaseExceptions::DatabaseExceptions; }; // Inherit base class constructors
  struct   IOFailureException       : DatabaseExceptions             { using DatabaseExceptions::DatabaseExceptions; }; // Inherit base class constructors



  /****************************************************************************
  ** Note:  This class assumes class Data and class Key_t implements symmetric insertion and extraction operators.
  **        Class Data must be default constructible.
  ****************************************************************************/
  template<class Data, const char sourceFileName[], const char destinationFilename[] = sourceFileName>
  class Database
  {
    public:
      // Public types
      using Value_t  = Data;
      using Key      = typename Utilities::Key_t<Data>;
      using Keys     = std::set<Key>;

      // Singleton discovery and management functions
      static Database & getInstance();
      void              checkpoint ();

      // Query functions
      std::size_t   size()                       const noexcept;
      Keys          keys()                       const;
      const Data &  operator[] (const Key & key) const;

      // Data modification functions
      Key           insert     ( const Data & data                   );
      void          insert     ( const Key  & key, const Data & data );
      void          remove     ( const Key  & key                    );
      Data &        operator[] ( const Key  & key                    );




    private:
      // Private types
      using Records = std::map< Key, Data >;

      // Singleton - created and destroyed via the GetInstance function.
      Database             ();
      ~Database            ();
      Database             (const Database & ) = delete;
      Database & operator= (const Database & ) = delete;
      Database             (      Database &&) = delete;
      Database & operator= (      Database &&) = delete;

      // Instance attributes
      Records             _dataBase;
      bool                _dirty = true;

      // Class attributes
      static           Key  _nextAvail;
  };
}    // namespace Utilities




// Inline functions and template definitions must be part of the header file. To help keep the size of the header file manageable
// and to keep implementation definitions separated from interface declaration, the header file has been divided into two.
// Utilities/Database.hxx is not intended to be included by clients and serves only to allow Database and related declarations to be
// separated from implementation whist making inline function and template definitions visible to all translation units. This
// technique is primary used as a teaching tool. I don't see this in real-world applications often, but it's out there. It allows me
// to distribute the declarations without the definitions. Normally, the contents of "Utilities/Database.hxx" would be physically
// entered below instead of telling the pre-processor to include the contents.
#include "Utilities/Database.hxx"

#endif
