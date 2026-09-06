// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
// @warning  This file was automatically generated, edit at your own risk

/**
* Unclassified               U N C L A S S I F I E D               Unclassified
*
* DEVELOPMENT:
*    This document wholly developed with USG funds.
*    For additional information, contact the AFRCO.
*
* ‒  DISTRIBUTION STATEMENT D.  Distribution authorized to the Department
*    of Defense and U.S. DoD contractors only; Critical Technology; 17 Sep 2015.
*    Other requests shall be referred to Air Force Rapid Capabilities Office,
*    Bolling AFB, Washington DC 20032-6400. 
*
* EXPORT CONTROL:
*    WARNING - ITAR CONTROLLED - US ONLY
*    This distribution contains technical data whose export is restricted by
*    the Arms Export Control Act (Title 22, U.S.C., Sec. 2751 et seq. or the
*    Export Administration Act of 1979 as amended Title 50, U.S.C., App.
*    2401-2420 et seq.), as amended. Violation of these export laws are subject
*    to severe criminal penalties.  Disseminate in accordance with provisions of DoD
*    Directive 5230.25.
*/
#ifndef Uci__Base__SimpleList_h
#define Uci__Base__SimpleList_h 1

#if !defined(SysLibStdint_h)
#  include <stdint.h>
//  @cond
#  define SysLibStdint_h 1
//  @endcond
#endif


#if !defined(StdLibCstddef)
#  include <cstddef>
//  @cond
#  define StdLibCstddef 1
//  @endcond
#endif

#if !defined(StdLibClimits)
#  include <climits>
//  @cond
#  define StdLibClimits 1
//  @endcond
#endif


#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all base data types are declared
   namespace base {

      template<class T, uci::base::accessorType::AccessorType V>
      /** The SimpleList is used to provide access to all simple lists. */
      class SimpleList : public virtual uci::base::Accessor {
      public:

         /** The size_type typedef is an unsigned integral type that can represent any non-negative "size" values. e.g. the number
           * of elements in a boundedList.
           */
         typedef std::size_t size_type;

         /** The reference typedef is an unsigned integral type that can represent any non-negative "size" values. e.g. the number
           * of elements in a boundedList.
           */
         typedef T& reference;

         /** The const_reference typedef is an unsigned integral type that can represent any non-negative "size" values. e.g. the
           * number of elements in a boundedList.
           */
         typedef const T& const_reference;

         /** The UNBOUNDED_BOUND is the maximum size of a bounded list. If the valud returned by the getMaximumLength() method
           * equals this value, then no bounds were specified in the XSD specification and this list is bounded simply by natural
           * indexing.
           */
         static const size_type MAXIMUM_LENGTH = ULONG_MAX;

         /** The destructor */
         virtual ~SimpleList()
         { }

         /** Returns the number of elements in the boundedList. This number is the number of actual objects held in the
           * boundedList, which is not necessarily equal to its storage capacity.
           *
           * @return The number of actual objects held in the boundedList.
           */
         virtual size_type size() const
            throw() = 0;


         /** Returns the maximum number of elements that the boundedList should hold in order to be consider valid with respect to
           * the XSD specification used to generate this containers specification. The value returned is the same that is returned
           * by the getMaxOccurs() method. If not specified in the XSD, then this is the maximum size the container can reach due
           * to known system or library implementation limitations, but the container is by no means guaranteed to be able to
           * reach that size: it can still fail to allocate storage at any point before that size is reached.
           *
           * @return The maximum number of elements that the boundedList should hold in order to be considered valid.
           */
         virtual size_type max_size() const
            throw() = 0;


         /** Returns the minimum number of elements the container should hold in order to be considered valid with respect to the
           * XSD specification used to generate this containers specification. The value returned is the same that is returned by
           * the getMinOccurs() method.
           *
           * @return The minimum number of elements that the boundedList should hold in order to be considered valid.
           */
         virtual size_type min_size() const
            throw() = 0;


         /** Resizes the container so that it contains n elements.
           *
           * @param newSize The size of the container after it has been resized. If newSize is smaller than the current container
           *      size, the content of the boundedList is reduced to its first n elements, removing those beyond (and destroying
           *      them). If newSize is greater than the current container size, the content of the boundedList is expanded by
           *      inserting at the end as many elements as needed to reach a size of newSize. The newly added elements are value
           *      initialized (initialized according to their type). If newSize is greater than the current container capacity, an
           *      automatic reallocation of the allocated storage space takes place. Notice that this method changes the actual
           *      content of the container by inserting or erasing elements from it.
           * @param type = V The type of data to be added when this BoundedList is resized. The use of this parameter provides
           *      support for inheritable types. This parameter defaults to null that is used to indicate that the type of the
           *      elements to be inserted are of the type specified by the template argument used when this BoundedList's class
           *      was declared, i.e. the template's T argument. This type is known as the BoundedList's base type. If specified,
           *      this argument must either be a type ID associated with the BoundedList's base type or a accessor derived from
           *      the BoundedList's base type
           */
         virtual void resize(size_type newSize, uci::base::accessorType::AccessorType type = V)
            throw(uci::base::UCIException) = 0;


         /** Returns the size of allocated storage capacity expressed in terms of elements
           *
           * @return The size of the storage space currently allocated for the boundedList, expressed in terms of elements. This
           *      capacity is not necessarily equal to the boundedList size. It can be equal or greater, with the extra space
           *      allowing to accommodate for growth without the need to reallocate on each insertion. Notice that this capacity
           *      does not suppose a limit on the size of the boundedList. When this capacity is exhausted and more is needed, it
           *      is automatically expanded by the container (reallocating it storage space). The theoretical limit on the size of
           *      a boundedList is given by member max_size. The capacity of a boundedList can be explicitly altered by calling
           *      member boundedList::reserve.
           */
         virtual size_type capacity() const
            throw() = 0;


         /** Tests whether boundedList is empty.
           *
           * @return Returns whether the boundedList is empty, i.e. whether its size is 0, (true) or not (false).
           * @note This function does not modify the container in any way. To clear the contents of a boundedList, see
           *      boundedList::clear.
           * @note This function is equivalent to "size() == 0" but may offer better performance due to implementation
           *      constraints.
           */
         virtual bool empty() const
            throw() = 0;


         /** Request a change in the boundedList's capacity such that the capacity be at least large enough to contain n elements
           *
           * @param newCapacity If newCapacity is greater than the boundedList's current capacity, then the boundedList's capacity
           *      will be increased to the new capacity. This action may result in a reallocation of the boundedList's storage
           *      using new storage whose capacity is equal to or greater than newCapacity. Such a reallocation will cause all
           *      elements to be copied to the new storage and the original storage to be deleted. In all other cases, the
           *      function call does not cause a reallocation and the boundedList capacity is not affected.
           * @note This function has no effect on the boundedList size and cannot alter its elements.
           */
         virtual void reserve(size_type newCapacity)
            throw(uci::base::UCIException) = 0;


         /** Removes the last element in the boundedList, effectively reducing the container size by one. This destroys the
           * removed element.
           */
         virtual void pop_back()
            throw() = 0;


         /** Removes all elements from the boundedList (which are destroyed), leaving the boundedList with a size of 0. */
         virtual void clear()
            throw() = 0;


         /** The returns this boundedList's "maxOccurs" as specified in the XML Schema specification.
           *
           * @return The boundedList's "maxOccurs".
           * @note The value returned is the same as that returned by max_size()
           */
         virtual size_type getMaximumLength() const
            throw() = 0;


         /** The returns this boundedList's "minOccurs" as specified in the XML Schema specification.
           *
           * @return The boundedList's "minOccurs".
           * @note The value returned is the same as that returned by min_size()
           */
         virtual size_type getMinimumLength() const
            throw() = 0;


         /**  Returns this SimpleList's "length" facet as specified in the XML Schema specification.
           *
           * @return The SimpleList's "maxLength".
           */
         virtual size_type getLength() const
            throw() = 0;


         /** Returns a reference to the enumerated item at position n in the boundedList.
           *
           * @param index The index of the element in this boundedList a reference to which is to be returned.
           * @return The reference to the element at position n in the boundedList.
           * @note No bounds checking is performed by this operator. See boundedList::at() for a version of this operator that
           *      does perform bounds checking. Unexpected results, including the throwing of exceptions, may occur if the index
           *      is out of range.
           */
         virtual reference operator[](size_type index) = 0;


         /** Returns a reference to the enumerated item at position n in the boundedList.
           *
           * @param index The index of the element in this boundedList a reference to which is to be returned.
           * @return The constant reference to the element at position n in the boundedList.
           * @note No bounds checking is performed by this operator. See boundedList::at() for a version of this operator that
           *      does perform bounds checking. Unexpected results, including the throwing of exceptions, may occur if the index
           *      is out of range.
           */
         virtual const_reference operator[](size_type index) const = 0;


         /** Returns a reference to the element at position n in the boundedList container. This method automatically checks
           * whether the index is within the bounds of valid elements in the boundedList, throwing an out_of_range exception if it
           * is not (i.e., if n is greater or equal than its size). This is in contrast with member operator[], that does not
           * check against bounds.
           *
           * @param index The index of the element in this boundedList a reference to which is to be returned. If this is greater
           *      than or equal to the boundedList size, an exception of type out_of_range is thrown. Notice that the first
           *      element has a position of 0 (not 1).
           * @return The reference to the element at position index in the boundedList.
           */
         virtual reference at(size_type index)
            throw(uci::base::UCIException) = 0;


         /** Returns a reference to the element at position n in the boundedList container. This method automatically checks
           * whether the index is within the bounds of valid elements in the boundedList, throwing an out_of_range exception if it
           * is not (i.e., if n is greater or equal than its size). This is in contrast with member operator[], that does not
           * check against bounds.
           *
           * @param index The index of the element in this boundedList a reference to which is to be returned. If this is greater
           *      than or equal to the boundedList size, an exception of type out_of_range is thrown. Notice that the first
           *      element has a position of 0 (not 1).
           * @return The reference to the enumerate item at position index in the boundedList.
           */
         virtual const_reference at(size_type index) const
            throw(uci::base::UCIException) = 0;


         /** Adds a new element at the end of the boundedList, after its current last element. The content of value is copied (or
           * moved) to the new element. This effectively increases the container size by one, which causes an automatic
           * reallocation of the allocated storage space if, and only if, the new boundedList size surpasses the current
           * boundedList capacity.
           *
           * @param value The value to be copied (or moved) into the new element.
           */
         virtual void push_back(const_reference value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SimpleList()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SimpleList<T,V> to copy from
           */
         SimpleList(const SimpleList<T,V>& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SimpleList<T,V> to the contents of the SimpleList<T,V> on the
           * right hand side (rhs) of the assignment operator.SimpleList<T,V> [only available to derived classes]
           *
           * @param rhs The SimpleList<T,V> on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::base::SimpleList<T,V>
           * @return A constant reference to this SimpleList<T,V>.
           */
         const SimpleList<T,V>& operator=(const SimpleList<T,V>& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SimpleList


   } // Namespace: base
} // Namespace: uci

#endif // Uci__Base__SimpleList_h

