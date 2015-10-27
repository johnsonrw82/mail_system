/******************************************************************************
** (C) Copyright 2015 by Thomas Bettens. All Rights Reserve.dstd::
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


#ifndef UTILITIES_Associations_hxx
#define UTILITIES_Associations_hxx

#include <algorithm>      // set_difference()
#include <cstddef>        // size_t
#include <iostream>
#include <iterator>       // inserter()
#include <sstream>
#include <type_traits>    // is_pointer

#include "Utilities/Associations.hpp"





namespace Utilities
{
  /************************************************************************************************
  ************************* Associations Map Interface Definitions ********************************
  ************************************************************************************************/

  //////////////////////////////  Constructors & Destructor  //////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
  template< class Left, class Right, template <class, class> class Policy >
  Associations<Left, Right, Policy>::Associations() = default;

  template< class Left, class Right, template <class, class> class Policy >
  Associations<Left, Right, Policy>::~Associations() noexcept = default;

  template< class Left, class Right, template <class, class> class Policy >
  Associations<Left, Right, Policy>::Associations( const Associations & ) = default;

  template< class Left, class Right, template <class, class> class Policy >
  Associations<Left, Right, Policy>::Associations( Associations && ) = default;

  template< class Left, class Right, template <class, class> class Policy >
  Associations<Left, Right, Policy> & Associations<Left, Right, Policy>::operator=( const Associations & ) = default;

  template< class Left, class Right, template <class, class> class Policy >
  Associations<Left, Right, Policy> & Associations<Left, Right, Policy>::operator=( Associations && ) = default;





  ////////////////////////////////////////  Remove  ///////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Remove the specific left-to-right relationship
  template< class Left, class Right, template <class, class> class Policy >

  void Associations<Left, Right, Policy>::remove( const Left & left, const Right & right )
  {
    #ifndef TO_DO_BY_STUDENT  // This section to be completed by the student
    #endif
  }




  // Remove every left-to-right* permutation of relationships
  template< class Left, class Right, template <class, class> class Policy >

  void Associations<Left, Right, Policy>::remove( const Left & left, const RightCollectionType & right )
  {
    #ifndef TO_DO_BY_STUDENT  // This section to be completed by the student
    #endif
  }




  // Remove every left*-to-right permutation of relationships
  template< class Left, class Right, template <class, class> class Policy >

  void Associations<Left, Right, Policy>::remove( const LeftCollectionType & left, const Right & right )
  {
    for( const auto & i : left)   remove( i, right);
  }




  // Remove every left*-to-right* permutation of relationships
  template< class Left, class Right, template <class, class> class Policy >

  void Associations<Left, Right, Policy>::remove( const LeftCollectionType & left, const RightCollectionType & right )
  {
    for( const auto & i : left)   remove( i, right);
  }




  // Remove every left-to-* relationship
  template< class Left, class Right, template <class, class> class Policy >

  void Associations<Left, Right, Policy>::removeLeft( const Left & left )
  {
    this->_m2mMap.erase( left );
  }




  // Remove every *-to-right relationship
  template< class Left, class Right, template <class, class> class Policy >

  void Associations<Left, Right, Policy>::removeRight( const Right & right )
  {
    // We must remove the right element from every left element.  Iterators may become invalid as items from this->_m2mMap are removed, we
    // can't call
    //    remove(i->first, right)
    // So, let's build a left hand collection and use that to walk the list.

    remove( retrieveLeft(), right );
  }




  // Remove all relationships
  template< class Left, class Right, template <class, class> class Policy >

  void Associations<Left, Right, Policy>::clear()
  {
    this->_m2mMap.clear();
  }





  ///////////////////////////////////////  Queries  ///////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Provides the unique set of right elements in all relationships
  template< class Left, class Right, template <class, class> class Policy >

  typename Associations<Left, Right, Policy>::RightCollectionType   Associations<Left, Right, Policy>::retrieveRight() const
  {
    // return the union of all right hand sets
    RightCollectionType theUnion;

    for (const auto & i : this->_m2mMap)   theUnion.insert(i.second.begin(), i.second.end());

    return theUnion;
  }




  // Provides the unique set of right elements for a specific left element in the relationship
  template< class Left, class Right, template <class, class> class Policy >

  typename Associations<Left, Right, Policy>::RightCollectionType   Associations<Left, Right, Policy>::retrieveRight(const Left & left) const
  {
    // Can't use the map.operator[] function here because that would insert new items that weren't already there ...

    auto i = this->_m2mMap.find(left);

    if (i != this->_m2mMap.end()) return i->second;
    else                          return{}; // didn't find the left key, return the empty set
  }




  // Provides the unique set of right elements in a set of left elements in the relationship
  template< class Left, class Right, template <class, class> class Policy >

  typename Associations<Left, Right, Policy>::RightCollectionType   Associations<Left, Right, Policy>::retrieveRight(const LeftCollectionType & left) const
  {
    // return the union of right hand sets associated with each left item in the applied list
    RightCollectionType theUnion;

    for (const auto & i : left)
    {
      auto j = this->_m2mMap.find(i);

      if (j != this->_m2mMap.end())   theUnion.insert(j->second.begin(), j->second.end());
    }

    return theUnion;
  }




  // Provides the unique set of left elements in all relationships
  template< class Left, class Right, template <class, class> class Policy >

  typename Associations<Left, Right, Policy>::LeftCollectionType   Associations<Left, Right, Policy>::retrieveLeft() const
  {
    // return a collection of all left hand keys
    LeftCollectionType left;

    for (const auto & i : this->_m2mMap)   left.insert(i.first);

    return left;
  }




  // Provides the unique set of left elements for a specific right element in the relationship
  template< class Left, class Right, template <class, class> class Policy >

  typename Associations<Left, Right, Policy>::LeftCollectionType   Associations<Left, Right, Policy>::retrieveLeft(const Right & right) const
  {
    // return a collection of left hand keys for which the right hand key appears in the associate set
    LeftCollectionType left;

    for (const auto & i : this->_m2mMap)
    {
      if (i.second.find(right) != i.second.end())  left.insert(i.first);
    }

    return left;
  }




  // Provides the unique set of left elements in a set of right elements in the relationship
  template< class Left, class Right, template <class, class> class Policy >

  typename Associations<Left, Right, Policy>::LeftCollectionType   Associations<Left, Right, Policy>::retrieveLeft(const RightCollectionType & right) const
  {
    // return a collection of left hand keys for which the right hand key appears in the associate set
    LeftCollectionType totalAnswer;

    for (const auto & i : right)
    {
      LeftCollectionType partialAnswer = retrieveLeft(i);
      totalAnswer.insert(partialAnswer.begin(), partialAnswer.end());
    }

    return totalAnswer;
  }




  // Provides the number of unique relationships
  template< class Left, class Right, template <class, class> class Policy >

  std::size_t Associations<Left, Right, Policy>::size() const
  {
    // Count and return the number of unique pairs
    std::size_t quantity = 0U;
    for( const auto & left : this->_m2mMap )   quantity += left.second.size();

    return quantity;
  }





  //////////////////////////////// Insertion and Extraction ///////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
  template< class Left, class Right, template <class, class> class Policy >

  std::ostream & operator<< (std::ostream & s, const Associations<Left, Right, Policy> & theMap)
  {
    // Write the number of pairs
    s << theMap.size() << theMap.RECORD_SEPARATOR << '\n';

    // for each pair
    for( const auto & left : theMap._m2mMap )  for( const auto & right : left.second )
    {
      #ifndef TO_DO_BY_STUDENT  // This section to be completed by the student
      #endif
    }

    return s;
  }




  template< class Left, class Right, template <class, class> class Policy >

  std::istream & operator>> (std::istream & s, Associations<Left, Right, Policy> & theMap)
  {
    // static_assert is a compile time assertion. Don't confuse it with the legacy assert macro which is a runtime assertion. Unlike
    // assert, static_assert generates no run time code.
    static_assert(! std::is_pointer<Left>::value,  "Error:  operator>>(istream, Associations<Left, Right, Policy> not defined for Left where Left is a pointer type");
    static_assert(! std::is_pointer<Right>::value, "Error:  operator>>(istream, Associations<Left, Right, Policy> not defined for Right where Right is a pointer type");

    char delimiter;

    // Get the number of pairs to process
    std::size_t quantity = 0U;
    s >> std::ws >> quantity >> delimiter;
    if( delimiter != theMap.RECORD_SEPARATOR )     throw SymmetricSyncException( "Error:  Symmetric sync error", __LINE__, __func__, __FILE__ );


    // for each pair
    Left   left;               // This is where things go wrong if Left  is a pointer type.  There is no memory behind the pointer to store data
    Right  right;              // This is where things go wrong if Right is a pointer type.  There is no memory behind the pointer to store data
    
    Associations<Left, Right, Policy> temp;

    for( ; quantity != 0;  --quantity )
    {
      s >> std::ws >> left >> delimiter;
      if( delimiter != temp.FIELD_SEPARATOR )      throw SymmetricSyncException( "Error:  Symmetric sync error", __LINE__, __func__, __FILE__ );

      s >> std::ws >> right >> delimiter;
      if( delimiter != temp.RECORD_SEPARATOR )     throw SymmetricSyncException( "Error:  Symmetric sync error", __LINE__, __func__, __FILE__ );

      temp._m2mMap[left].insert( right );
    }

    // Okay, all processed and validated
    theMap = std::move(temp);
    return s;
  }





  //////////////////////////////////// Logical Operators //////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////
  template< class Left, class Right, template <class, class> class Policy >
  bool operator==(const Associations<Left, Right, Policy> & lhs, const Associations<Left, Right, Policy> & rhs)
  { return lhs._m2mMap == rhs._m2mMap; }




  template< class Left, class Right, template <class, class> class Policy >
  bool operator< (const Associations<Left, Right, Policy> & lhs, const Associations<Left, Right, Policy> & rhs)
  { return lhs._m2mMap < rhs._m2mMap; }




  template<class Left, class Right, template <class, class> class Policy>
  bool operator!=( const Associations<Left, Right, Policy> & lhs, const Associations<Left, Right, Policy> & rhs )
  { return !(lhs == rhs); }




  template<class Left, class Right, template <class, class> class Policy>
  bool operator> ( const Associations<Left, Right, Policy> & lhs, const Associations<Left, Right, Policy> & rhs )
  { return  (rhs <  lhs); }




  template<class Left, class Right, template <class, class> class Policy>
  bool operator<=( const Associations<Left, Right, Policy> & lhs, const Associations<Left, Right, Policy> & rhs )
  { return !(rhs <  lhs); }




  template<class Left, class Right, template <class, class> class Policy>
  bool operator>=( const Associations<Left, Right, Policy> & lhs, const Associations<Left, Right, Policy> & rhs )
  { return !(lhs <  rhs); }










  /************************************************************************************************
  *********************************** Less Interface Definitions **********************************
  ************************************************************************************************/

  // Allow the map to contain objects, or pointer to objects. But be sure to compare the objects and not their pointers. Note that,
  // unlike the predefined function objects in <functional> and lambda expressions, objects of this class support multiple call
  // signatures.
  struct Less
  {
    // When T is a pointer type, compare objects after dereferencing.
    template <class T>
    bool operator()( T * lhs, T * rhs ) const  // Note: Template argument types of const T* are a closer match to const T& (below).
    { return *lhs < *rhs; }                    //       So don't be tempted to make this this signature be pointers to constant T.

    // When T is not a pointer type, compare objects directly
    template <class T>
    bool operator()( const T & lhs, const T & rhs ) const
    { return lhs < rhs; }
  };










  /************************************************************************************************
  ************************** One to One Policy Interface Definitions ******************************
  ************************************************************************************************/
  //  {A:x}, {B,x}  = constraint error
  //  {A:x}, {A,y}  = constraint error
  //  {A:x}, {B,y}  = okay

  // Insert or update a one-to-one relationship
  template< class Left, class Right >

  void One2OnePolicy<Left, Right>::insert( const Left & left, const Right & right )
  {
    #ifndef TO_DO_BY_STUDENT  // This section to be completed by the student
    #endif
  }










  /************************************************************************************************
  ************************* One to Many Policy Interface Definitions ******************************
  ************************************************************************************************/
  //  {A:x}, {B,x}  = constraint error
  //  {A:x}, {A,y}  = ok

  // Insert or update a one-to-one relationship
  template< class Left, class Right >

  void One2ManyPolicy<Left, Right>::insert( const Left & left, const Right & right )
  {
    #ifndef TO_DO_BY_STUDENT  // This section to be completed by the student
    #endif
  }




  // Insert or update a one-to-many relationship
  template< class Left, class Right >

  void One2ManyPolicy<Left, Right>::insert( const Left & left, const RightCollectionType & right )
  {
    #ifndef TO_DO_BY_STUDENT  // This section to be completed by the student
    #endif
  }










  /************************************************************************************************
  ************************ Many to One Policy Interface Definitions *******************************
  ************************************************************************************************/
  //  {A:x}, {B,x}  = ok
  //  {A:x}, {A,y}  = constraint error

  // Insert or update a one-to-one relationship
  template< class Left, class Right >

  void Many2OnePolicy<Left, Right>::insert( const Left & left, const Right & right )
  {
    #ifndef TO_DO_BY_STUDENT  // This section to be completed by the student
    #endif
  }




  // Insert or update a many-to-one relationship
  template< class Left, class Right >

  void Many2OnePolicy<Left, Right>::insert( const LeftCollectionType & left, const Right & right )
  {
    #ifndef TO_DO_BY_STUDENT  // This section to be completed by the student
    #endif
  }










  /************************************************************************************************
  ************************ Many to Many Policy Interface Definitions ******************************
  ************************************************************************************************/
  //  {A:x}, {B,x}  = ok
  //  {A:x}, {A,y}  = ok

  // Insert or update a one-to-one relationship
  template< class Left, class Right >

  void Many2ManyPolicy<Left, Right>::insert( const Left & left, const Right & right )
  {
    #ifndef TO_DO_BY_STUDENT  // This section to be completed by the student
    #endif
  }




  // Insert or update a one-to-many relationship
  template< class Left, class Right >

  void Many2ManyPolicy<Left, Right>::insert( const Left & left, const RightCollectionType & right )
  {
    #ifndef TO_DO_BY_STUDENT  // This section to be completed by the student
    #endif
  }




  // Insert or update a many-to-one relationship
  template< class Left, class Right >

  void Many2ManyPolicy<Left, Right>::insert( const LeftCollectionType & left, const Right & right )
  {
    #ifndef TO_DO_BY_STUDENT  // This section to be completed by the student
    #endif
  }




  // Insert or update a many-to-many relationship
  template< class Left, class Right >

  void Many2ManyPolicy<Left, Right>::insert( const LeftCollectionType & left, const RightCollectionType & right )
  {
    #ifndef TO_DO_BY_STUDENT  // This section to be completed by the student
    #endif
  }

}  // namespace Utilities
#endif
