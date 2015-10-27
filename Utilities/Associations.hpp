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


#ifndef UTILITIES_Associations_hpp
#define UTILITIES_Associations_hpp

#include <cstddef>
#include <iostream>
#include <map>
#include <set>

#include "Utilities/Exceptions.hpp"




namespace Utilities
{
  // Functoid allowing objects or pointer-to-objects to be compared
  struct Less;

  // Policy Declarations
  template <class Left, class Right>  class One2OnePolicy;
  template <class Left, class Right>  class One2ManyPolicy;
  template <class Left, class Right>  class Many2OnePolicy;
  template <class Left, class Right>  class Many2ManyPolicy;


  // Exception Type Hierarchy Definition (Outside the template definitions so they can be caught a bit more generically
  struct AssociationsExceptions      : Utilities::AbstractException<>     { using AbstractException::AbstractException ; };            // Class Associations exception base class
  struct   SymmetricSyncException    : AssociationsExceptions             { using AssociationsExceptions::AssociationsExceptions; };   // Inherit base class constructors
  struct   PolicyConstraintException : AssociationsExceptions             { using AssociationsExceptions::AssociationsExceptions; };
  struct   InternalStateErrException : AssociationsExceptions             { using AssociationsExceptions::AssociationsExceptions; };



  // Maintains an association relationship between two sets of objects.  The type of association is enforced by mix-in policy.
  template< class Left, class Right, template <class, class> class Policy = Many2ManyPolicy>

  class Associations : public Policy<Left, Right>
  {
    template< class L, class R, template <class, class> class P >
    friend std::ostream & operator<< (std::ostream & s, const Associations<L, R, P> & theMap);

    template< class L, class R, template <class, class> class P >
    friend std::istream & operator>> (std::istream & s,       Associations<L, R, P> & theMap);

    template< class L, class R, template <class, class> class P >
    friend bool operator==(const Associations<L, R, P> & lhs, const Associations<L, R, P> & rhs);

    template< class L, class R, template <class, class> class P >
    friend bool operator< (const Associations<L, R, P> & lhs, const Associations<L, R, P> & rhs);



    public:
      using LeftCollectionType  = typename Policy<Left, Right>::LeftCollectionType;
      using RightCollectionType = typename Policy<Left, Right>::RightCollectionType;

      // Constructors and Destructor
      Associations             (                       );
     ~Associations             (                       ) noexcept;
      Associations             ( const Associations &  );
      Associations             (       Associations && );
      Associations & operator= ( const Associations &  );
      Associations & operator= (       Associations && );

      // Insert operations are specified by the Association Policy

      // Remove the specific left-to-right relationship, or every left-to-right*, left*-to-right, or left*-to-right* permutation of relationships
      void remove(const Left               & left, const Right               & right);
      void remove(const Left               & left, const RightCollectionType & right);
      void remove(const LeftCollectionType & left, const Right               & right);
      void remove(const LeftCollectionType & left, const RightCollectionType & right);

      // Remove every left-to-*, or every *-to-right relationship
      void removeLeft ( const Left  & left  );
      void removeRight( const Right & right );

      // Remove all relationships
      void        clear();


      // Provides the unique set of right elements in all, specific, or a set of left elements in the relationship
      RightCollectionType retrieveRight(                               ) const;
      RightCollectionType retrieveRight(const Left               & left) const;
      RightCollectionType retrieveRight(const LeftCollectionType & left) const;


      // Provides the unique set of left elements in all, a specific, or set of right elements in the relationship
      LeftCollectionType  retrieveLeft(                                 ) const;
      LeftCollectionType  retrieveLeft(const Right               & right) const;
      LeftCollectionType  retrieveLeft(const RightCollectionType & right) const;


      // Provides the number of unique relationships
      std::size_t size () const;

  private:
      // Class attributes
      static constexpr char FIELD_SEPARATOR  = '\x03';  // ETX (End of Text) character
      static constexpr char RECORD_SEPARATOR = '\x04';  // EOT (End of Transmission) character ('\n' turns out to not be a good choice because data
  };  // class Associations


  template<class Left, class Right, template <class, class> class Policy>  bool operator!=(const Associations<Left, Right, Policy> & lhs, const Associations<Left, Right, Policy> & rhs);
  template<class Left, class Right, template <class, class> class Policy>  bool operator> (const Associations<Left, Right, Policy> & lhs, const Associations<Left, Right, Policy> & rhs);
  template<class Left, class Right, template <class, class> class Policy>  bool operator<=(const Associations<Left, Right, Policy> & lhs, const Associations<Left, Right, Policy> & rhs);
  template<class Left, class Right, template <class, class> class Policy>  bool operator>=(const Associations<Left, Right, Policy> & lhs, const Associations<Left, Right, Policy> & rhs);







  /************************************************************************************************
  ****************************** Policy Interface Definitions *************************************
  ************************************************************************************************/
  // Base policy from which all policies should derive. Note that there are no virtual functions and
  // that by making the constructors protected, a policy is prevented by design from being directly instantiated.
  template <class Left, class Right>
  class BasePolicy
  {
    public:
      using  LeftCollectionType    = std::set< Left,  Less >;
      using  RightCollectionType   = std::set< Right, Less >;

    protected:
      // Prevented by design from being directly instantiated
      BasePolicy             (                   ) = default;
      BasePolicy             (const BasePolicy & ) = default;
      BasePolicy             (      BasePolicy &&) = default;
      BasePolicy & operator= (const BasePolicy & ) = default;
      BasePolicy & operator= (      BasePolicy &&) = default;

      // Implementation note:  Inside a template definition there are dependent and non-dependent names(which do or don't depend on
      // the template parameters). The name in template base class is not visible (from derived classes or clients) according to the
      // Standard lookup rules(See[temp.dep], 14.6.2(3)). Use dependent name lookup to mane the name visible.  Adding "this->"
      // before the name, or using explicit scoped names causes dependent name lookup. In 14.6.1/2d: "Within the scope of a class
      // template, when the unqualified name ... is referred to, it is equivalent to the name of the ... qualified ... name of the
      // enclosing class template.  
      
      // Want two "columns", one for Left and one for Right.  But we want to enforce that a Left/Right pair is unique.
      using  MappingType = std::map<Left, RightCollectionType, Less>;
      MappingType          _m2mMap;
  };


  template <class Left, class Right>
  class One2OnePolicy : public BasePolicy<Left, Right>
  {
    public:
      // Insert or update a one-to-one relationship
      void insert(const Left & left, const Right & right);
 
    protected:
      // Prevented by design from being directly instantiated
      One2OnePolicy             (                      ) = default;
      One2OnePolicy             (const One2OnePolicy & ) = default;
      One2OnePolicy             (      One2OnePolicy &&) = default;
      One2OnePolicy & operator= (const One2OnePolicy & ) = default;
      One2OnePolicy & operator= (      One2OnePolicy &&) = default;
  };


  template <class Left, class Right>
  class One2ManyPolicy : public BasePolicy<Left, Right>
  {
    public:
      using  RightCollectionType   = typename BasePolicy<Left, Right>::RightCollectionType;

      // Insert or update a one-to-one, or one-to-many relationship
      void insert(const Left & left, const Right               & right);
      void insert(const Left & left, const RightCollectionType & right);
 
    protected:
      // Prevented by design from being directly instantiated
      One2ManyPolicy             (                       ) = default;
      One2ManyPolicy             (const One2ManyPolicy & ) = default;
      One2ManyPolicy             (      One2ManyPolicy &&) = default;
      One2ManyPolicy & operator= (const One2ManyPolicy & ) = default;
      One2ManyPolicy & operator= (      One2ManyPolicy &&) = default;
  };


  template <class Left, class Right>
  class Many2OnePolicy : public BasePolicy<Left, Right>
  {
    public:
      using  LeftCollectionType    = typename BasePolicy<Left, Right>::LeftCollectionType;

      // Insert or update a one-to-one, or many-to-one relationship
      void insert(const Left               & left, const Right & right);
      void insert(const LeftCollectionType & left, const Right & right);
 
    protected:
      // Prevented by design from being directly instantiated
      Many2OnePolicy             (                       ) = default;
      Many2OnePolicy             (const Many2OnePolicy & ) = default;
      Many2OnePolicy             (      Many2OnePolicy &&) = default;
      Many2OnePolicy & operator= (const Many2OnePolicy & ) = default;
      Many2OnePolicy & operator= (      Many2OnePolicy &&) = default;
  };


  template <class Left, class Right>
  class Many2ManyPolicy : public BasePolicy<Left, Right>
  {
    public:
      using  LeftCollectionType    = typename BasePolicy<Left, Right>::LeftCollectionType;
      using  RightCollectionType   = typename BasePolicy<Left, Right>::RightCollectionType;

      // Insert or update a one-to-one, one-to-many, many-to-one, or many-to-many relationship
      void insert(const Left               & left, const Right               & right);
      void insert(const Left               & left, const RightCollectionType & right);
      void insert(const LeftCollectionType & left, const Right               & right);
      void insert(const LeftCollectionType & left, const RightCollectionType & right);
 
    protected:
      // Prevented by design from being directly instantiated
      Many2ManyPolicy             (                        ) = default;
      Many2ManyPolicy             (const Many2ManyPolicy & ) = default;
      Many2ManyPolicy             (      Many2ManyPolicy &&) = default;
      Many2ManyPolicy & operator= (const Many2ManyPolicy & ) = default;
      Many2ManyPolicy & operator= (      Many2ManyPolicy &&) = default;
  };

} // namespace Utilities






// Inline functions and template definitions must be part of the header file. To help keep the size of the header file manageable
// and to keep implementation definitions separated from interface declaration, the header file has been divided into two.
// Utilities/Associations.hxx is not intended to be included by clients and serves only to allow Associations and related
// declarations to be separated from implementation whist making inline function and template definitions visible to all translation
// units. This technique is primary used as a teaching tool. I don't see this in real-world applications often, but it's out there.
// It allows me to distribute the declarations without the definitions. Normally, the contents of "Utilities/Associations.hxx" would
// be physically entered below instead of telling the pre-processor to include the contents.
#include "Utilities/Associations.hxx"  // Template definitions
#endif  // UTILITIES_Associations_hpp
