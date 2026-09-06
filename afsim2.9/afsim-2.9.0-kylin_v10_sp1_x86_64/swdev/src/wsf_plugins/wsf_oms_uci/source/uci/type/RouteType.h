// This file was generated  on 12/5/2018 at 1:1:47 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__RouteType_h
#define Uci__Type__RouteType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__PathID_Type_h)
# include "uci/type/PathID_Type.h"
#endif

#if !defined(Uci__Type__LineProjectionEnum_h)
# include "uci/type/LineProjectionEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__RoutePathType_h)
# include "uci/type/RoutePathType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RouteType sequence accessor class */
      class RouteType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RouteType()
         { }

         /** Returns this accessor's type constant, i.e. RouteType
           *
           * @return This accessor's type constant, i.e. RouteType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::routeType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RouteType& accessor)
            throw(uci::base::UCIException) = 0;


         /** A series of path segments. A route can contain many paths. Some paths provide alternate routes and contingency routes
           * which branch from the primary path. Other paths can be standalone, disconnected paths. Elements given here are not
           * necessarily in mission/flight order; it is necessary to follow the linkages provided in NextPathSegment and/or
           * ConditionalPathSegment to traverse the segments in mission/flight order. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::RoutePathType, uci::type::accessorType::routePathType> Path;

         /** Returns the value of the SimplePrimitive data type that is identified by the Detailed.
           *
           * @return The value of the simple primitive data type identified by Detailed.
           */
         virtual xs::Boolean getDetailed() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Detailed.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDetailed(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FirstInRoutePathID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FirstInRoutePathID.
           */
         virtual const uci::type::PathID_Type& getFirstInRoutePathID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FirstInRoutePathID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FirstInRoutePathID.
           */
         virtual uci::type::PathID_Type& getFirstInRoutePathID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FirstInRoutePathID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FirstInRoutePathID
           */
         virtual void setFirstInRoutePathID(const uci::type::PathID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RouteProjection.
           *
           * @return The value of the enumeration identified by RouteProjection.
           */
         virtual const uci::type::LineProjectionEnum& getRouteProjection() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RouteProjection.
           *
           * @return The value of the enumeration identified by RouteProjection.
           */
         virtual uci::type::LineProjectionEnum& getRouteProjection()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RouteProjection.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRouteProjection(const uci::type::LineProjectionEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RouteProjection.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRouteProjection(uci::type::LineProjectionEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Path.
           *
           * @return The bounded list identified by Path.
           */
         virtual const uci::type::RouteType::Path& getPath() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Path.
           *
           * @return The bounded list identified by Path.
           */
         virtual uci::type::RouteType::Path& getPath()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Path.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPath(const uci::type::RouteType::Path& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RouteType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RouteType to copy from
           */
         RouteType(const RouteType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RouteType to the contents of the RouteType on the right hand side
           * (rhs) of the assignment operator.RouteType [only available to derived classes]
           *
           * @param rhs The RouteType on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::RouteType
           * @return A constant reference to this RouteType.
           */
         const RouteType& operator=(const RouteType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RouteType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RouteType_h

