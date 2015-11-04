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


#ifndef UTILITIES_SetStreamEXState_hpp
#define UTILITIES_SetStreamEXState_hpp

#include <iostream>




namespace Utilities
{
  // Turns on the exception flags of a stream upon construction, and then resets then to original state upon destruction using the
  // Resource Acquisition Is Initialization (RAII) pattern (http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization)
  class SetStreamExState
  {
    SetStreamExState            ( const SetStreamExState & ) = delete;
    SetStreamExState & operator=( const SetStreamExState & ) = delete;

    public:

      SetStreamExState(std::ios & stream, std::ios::iostate newState = (std::ios::badbit | std::ios::failbit | std::ios::eofbit) )
        : _state( stream.exceptions() ), _stream(stream)
      {
        // Setting the exception flags will throw that exception if the corresponding state bit is already set.  Clear the state
        // bits to prevent throwing an exception here.
        _stream.clear();
        _stream.exceptions(newState);   // Set new state
      }


      ~SetStreamExState()
      {
        // It's likely this destructor will be called during the handling of an exception, in which case
        //   1) we don't want to generate a new exception. and
        //   2) we want to preserve the state flags so the exception handler can interrogate the stream.
        _stream.clear();
        _stream.exceptions(_state);     // Restore state
      }


    private:
      std::ios::iostate    _state;
      std::ios           & _stream;
  };
}       // namespace Utilities
#endif  //  UTILITIES_SetStreamEXState_hpp
