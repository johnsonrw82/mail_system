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


#ifndef UTILITIES_Database_hxx
#define UTILITIES_Database_hxx

#include <cstddef>    // size_t
#include <exception>  // uncaught_exception
#include <fstream>
#include <ios>        // ios::failure
#include <stdexcept>
#include <typeinfo>   // type_info returned from typeid()
#include <utility>    // make_pair()

#include "Utilities/Database.hpp"
#include "Utilities/SetStreamExState.hpp"




namespace Utilities
{
  /****************************************************************************
  ** NOTE:  [[ISO/IEC ISO/IEC 14882:2014(E) 14.5.1.3 (1) Static data members of class templates]] tells us to define
  **        class template static members in a similar way as we do for a "normal" class's static data members.  The catch is that
  **        "normal" class static data members must be defined with external linkage in a single source file.  This way, one and
  **        only one instance of the class attribute exists across all translation units.  This is simply the ODR (One Definition
  **        Rule) applied to class wide attributes. However, template class static members are themselves templates and thus the
  **        templated definition belong in the header file, as do the templated definitions of the template's functions and other
  **        stuff. It's the linker's responsibility to resolve identical instantiations across multiple translation units into a
  **        single instance.
  **
  **        Here's the problem.  Visual Studio's linker, as far back as I can remember including VS2015 RC, doesn't do this for
  **        DEBUG builds.  It does it correctly for Release builds, however.  (There must be some setting I haven't figured out yet)
  **        The results, when linking a DEBUG MSVC++ build, each translation unit has it's own copy of the static data member.  Each
  **        TU initializes and manipulates its own copy, and changes made in one TU are not seen in any other TU. Again, RELEASE
  **        builds do it correctly, its just the DEBUG builds. I've not seen this bug in any other vender's linker. In particular,
  **        gcc and clang do not have this problem.
  ****************************************************************************/
  ///////////////////////////////////  Class attributes  //////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
  template< class Data, const char sourceFileName[], const char destinationFileName[] >
  typename Database<Data, sourceFileName, destinationFileName>::Key Database<Data, sourceFileName, destinationFileName>::_nextAvail = 0L;




  //////////////////////////////  Constructors & Destructor  //////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
  template< class Data, const char sourceFileName[], const char destinationFileName[] >
  
  Database< Data, sourceFileName, destinationFileName >::Database()
  {
    // Create an input stream and turn on exceptions for the customer database file. The stream is closed after construction of the database.
    std::ifstream inFile;
    Utilities::SetStreamExState sstate( inFile );

    try
    {
		// open the file using the supplied source file name
		inFile.open(sourceFileName);

		// load each key/data pair from the stream
		Key key;
		Data data;
		while (inFile >> std::ws >> key >> data) { // std::ws will allow us to ignore the newline that may be present in the file
			insert(key, data);  // insert into the database
		}

    }

    // g++ under MinGW64 throws the wrong exception, so let's catch the exception a little higher in the exception tree as a work
    // around.  g++ in other environments, as well as clang and MSVC++ do just fine.
    catch( const std::exception & ex ) 
//  catch( const std::ios::failure & ex )   // Preferred type to catch
    {
      using namespace std::string_literals;

      // ignore failure to open input file, assume an empty database
      if( !inFile.is_open() )
      {
        std::clog << "Info:         Unable to locate or open file to load database.  Empty database is assumed.\n"
                  << "   Filename:  \"" << sourceFileName << "\"\n"
                  << "   Database:  \"" << typeid(Data).name() << "\"\n";
      }

      // ignore anticipate EOF.  Likely cause is an incomplete extraction of a data object
      else if( ! inFile.eof() ) throw IOFailureException(ex, "ERROR: Unexpected std::ios::failure in database \""s + typeid(Data).name() + "\"" ,  __LINE__, __func__, __FILE__ ); 
    }

    _dirty = false; // want to start out with clean db, remembering that insert() sets the _dirty bit.
  }




  // Persist the in-memory content to the disk file as part of destroying the database object
  template< class Data, const char sourceFileName[], const char destinationFileName[] >
  
  Database< Data, sourceFileName, destinationFileName >::~Database ()
  {
    // Make sure we're not already handling an exception before we checkpoint the db, which
    // may also throw an exception, and that wouldn't be good for this destruction process.
    if ( ! std::uncaught_exception() )   checkpoint();
  }




  ////////////////?////  Singleton discovery and management functions  ////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
  template< class Data, const char sourceFileName[], const char destinationFileName[] >
  
  Database< Data, sourceFileName, destinationFileName > & Database< Data, sourceFileName, destinationFileName >::getInstance()
  {
	  // create a static instance of the database given the template parameters
	  static Database<Data, sourceFileName, destinationFileName> instance;
	  // return instance
	  return instance;
  }




  template< class Data, const char sourceFileName[], const char destinationFileName[] >
  
  void Database< Data, sourceFileName, destinationFileName >::checkpoint()
  {
    try
    {
	  // if the database is dirty (IE, there have been modifications), need to take a snapshot
      if( _dirty )
      {
        // Create an output stream and turn on exceptions for the database file.
        std::ofstream outFile;
        Utilities::SetStreamExState sstate( outFile, std::ios::trunc );  // overwrite

		// open the output stream
		outFile.open(destinationFileName);

		for (const auto & record : _dataBase) {
			outFile << record.first << record.second << '\n'; // append newline so that it may be easier to read the data
		}
      }
    }

    catch( const std::ios::failure & ex )
    {
      using namespace std::string_literals;

      // Error strategy needs more work - stubbed for now
      throw IOFailureException(ex, "ERROR: Unexpected std::ios::failure in database \""s + typeid(Data).name() + "\"", __LINE__, __func__, __FILE__);
    }

	_dirty = false; // snapshot taken, set bit to false
  }




  ////////////////////////////////////////  Queries  //////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
  template< class Data, const char sourceFileName[], const char destinationFileName[] >
  
  std::size_t Database< Data, sourceFileName, destinationFileName >::size() const noexcept
  {
    return _dataBase.size();
  }




  template< class Data, const char sourceFileName[], const char destinationFileName[] >
  
  typename Database< Data, sourceFileName, destinationFileName >::Keys   Database< Data, sourceFileName, destinationFileName >::keys() const
  {
    Keys theKeys;
    for( const auto & record : _dataBase )    theKeys.emplace( record.first );

    return theKeys;
  }




  template< class Data, const char sourceFileName[], const char destinationFileName[] >
  
  const Data & Database< Data, sourceFileName, destinationFileName >::operator[] ( const Key & key ) const
  {
    using namespace std::string_literals;

    // Look up the given key, which must already exist (we don't want to invent Data content)
    auto i = _dataBase.find( key );

    if( i == _dataBase.end() ) throw RecordLookupException( "Found no record in database \""s + typeid(Data).name() + "\" matching supplied key \""s + static_cast<std::string>(key) + '"',
                                                            __LINE__, __func__, __FILE__ );

    return i->second;
  }




  ///////////////////////////////////  Data modification  /////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
  template< class Data, const char sourceFileName[], const char destinationFileName[] >
  
  typename Database< Data, sourceFileName, destinationFileName >::Key  Database< Data, sourceFileName, destinationFileName >::insert( const Data & data )
  {
	  // get the next available key
	  auto key = ++_nextAvail;
	  // insert
	  insert(key, data);
	  // return the key
	  return key;
  }




  template< class Data, const char sourceFileName[], const char destinationFileName[] >
  
  void Database< Data, sourceFileName, destinationFileName >::insert( const Key & key, const Data & data )
  {
	  // use the [] operator to insert into the database
	  _dataBase[key] = data;
	  if (_nextAvail < key) {
		  _nextAvail = key; // set the next available key to the one just inserted
	  }
	  _dirty = true; // set the dirty flag
  }




  template< class Data, const char sourceFileName[], const char destinationFileName[] >
  
  void Database< Data, sourceFileName, destinationFileName >::remove( const Key & key )
  {
    using namespace std::string_literals;

    // Erase the information corresponding to the given key, if it exists.
    if( _dataBase.erase( key ) == 0 )  throw RecordLookupException( "Found no record in database \""s + typeid(Data).name() + "\" matching supplied key \""s + static_cast<std::string>(key) + '"',
                                                                    __LINE__, __func__, __FILE__ );

    _dirty = true;
  }




  template< class Data, const char sourceFileName[], const char destinationFileName[] >
 
  Data & Database< Data, sourceFileName, destinationFileName >::operator[] ( const Key & key )
  {
    using namespace std::string_literals;

    // Look up the given key, which must already exist (we don't want to invent Data content)
    auto i = _dataBase.find( key );

    if( i == _dataBase.end() ) throw RecordLookupException( "Found no record in database \""s + typeid(Data).name() + "\" matching supplied key \""s + static_cast<std::string>(key) + '"',
                                                            __LINE__, __func__, __FILE__ );

    _dirty = true; // Assume the data will be modified by the client sometime later
    return i->second;
  }

}    // namespace Utilities
#endif
