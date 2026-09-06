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
#ifndef Uci__Type__RoutePathType_h
#define Uci__Type__RoutePathType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PathID_Type_h)
# include "uci/type/PathID_Type.h"
#endif

#if !defined(Uci__Type__PathTypeEnum_h)
# include "uci/type/PathTypeEnum.h"
#endif

#if !defined(Uci__Type__SegmentID_Type_h)
# include "uci/type/SegmentID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__PathSegmentType_h)
# include "uci/type/PathSegmentType.h"
#endif

#if !defined(Uci__Type__MetricsType_h)
# include "uci/type/MetricsType.h"
#endif

#if !defined(Uci__Type__PlanningLocationType_h)
# include "uci/type/PlanningLocationType.h"
#endif

#if !defined(Uci__Type__RunwayID_Type_h)
# include "uci/type/RunwayID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RoutePathType sequence accessor class */
      class RoutePathType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RoutePathType()
         { }

         /** Returns this accessor's type constant, i.e. RoutePathType
           *
           * @return This accessor's type constant, i.e. RoutePathType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::routePathType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RoutePathType& accessor)
            throw(uci::base::UCIException) = 0;


         /** A PathSegment is defined from the previous path segment EndPoint (or current vehicle state if no previous path
           * segment) to the current path segment EndPoint. Elements given here are not necessarily in mission/flight order; it is
           * necessary to follow the linkages provided in NextPathSegment and/or ConditionalPathSegment to traverse the segments
           * in mission/flight order. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PathSegmentType, uci::type::accessorType::pathSegmentType> PathSegment;

         /** Returns the accessor that provides access to the complex content that is identified by the PathID.
           *
           * @return The acecssor that provides access to the complex content that is identified by PathID.
           */
         virtual const uci::type::PathID_Type& getPathID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PathID.
           *
           * @return The acecssor that provides access to the complex content that is identified by PathID.
           */
         virtual uci::type::PathID_Type& getPathID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PathID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PathID
           */
         virtual void setPathID(const uci::type::PathID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PathType.
           *
           * @return The value of the enumeration identified by PathType.
           */
         virtual const uci::type::PathTypeEnum& getPathType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PathType.
           *
           * @return The value of the enumeration identified by PathType.
           */
         virtual uci::type::PathTypeEnum& getPathType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PathType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPathType(const uci::type::PathTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PathType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPathType(uci::type::PathTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FirstInPathSegmentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FirstInPathSegmentID.
           */
         virtual const uci::type::SegmentID_Type& getFirstInPathSegmentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FirstInPathSegmentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FirstInPathSegmentID.
           */
         virtual uci::type::SegmentID_Type& getFirstInPathSegmentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FirstInPathSegmentID to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FirstInPathSegmentID
           */
         virtual void setFirstInPathSegmentID(const uci::type::SegmentID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PathSegment.
           *
           * @return The bounded list identified by PathSegment.
           */
         virtual const uci::type::RoutePathType::PathSegment& getPathSegment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PathSegment.
           *
           * @return The bounded list identified by PathSegment.
           */
         virtual uci::type::RoutePathType::PathSegment& getPathSegment()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the PathSegment.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPathSegment(const uci::type::RoutePathType::PathSegment& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Metrics.
           *
           * @return The acecssor that provides access to the complex content that is identified by Metrics.
           */
         virtual const uci::type::MetricsType& getMetrics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Metrics.
           *
           * @return The acecssor that provides access to the complex content that is identified by Metrics.
           */
         virtual uci::type::MetricsType& getMetrics()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Metrics to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Metrics
           */
         virtual void setMetrics(const uci::type::MetricsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Metrics exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Metrics is emabled or not
           */
         virtual bool hasMetrics() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Metrics
           *
           * @param type = uci::type::accessorType::metricsType This Accessor's accessor type
           */
         virtual void enableMetrics(uci::base::accessorType::AccessorType type = uci::type::accessorType::metricsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Metrics */
         virtual void clearMetrics()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the InitialConditions.
           *
           * @return The acecssor that provides access to the complex content that is identified by InitialConditions.
           */
         virtual const uci::type::PlanningLocationType& getInitialConditions() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the InitialConditions.
           *
           * @return The acecssor that provides access to the complex content that is identified by InitialConditions.
           */
         virtual uci::type::PlanningLocationType& getInitialConditions()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the InitialConditions to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by InitialConditions
           */
         virtual void setInitialConditions(const uci::type::PlanningLocationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by InitialConditions exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by InitialConditions is emabled or not
           */
         virtual bool hasInitialConditions() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by InitialConditions
           *
           * @param type = uci::type::accessorType::planningLocationType This Accessor's accessor type
           */
         virtual void enableInitialConditions(uci::base::accessorType::AccessorType type = uci::type::accessorType::planningLocationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by InitialConditions */
         virtual void clearInitialConditions()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RunwayID.
           *
           * @return The acecssor that provides access to the complex content that is identified by RunwayID.
           */
         virtual const uci::type::RunwayID_Type& getRunwayID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RunwayID.
           *
           * @return The acecssor that provides access to the complex content that is identified by RunwayID.
           */
         virtual uci::type::RunwayID_Type& getRunwayID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RunwayID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RunwayID
           */
         virtual void setRunwayID(const uci::type::RunwayID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RunwayID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RunwayID is emabled or not
           */
         virtual bool hasRunwayID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RunwayID
           *
           * @param type = uci::type::accessorType::runwayID_Type This Accessor's accessor type
           */
         virtual void enableRunwayID(uci::base::accessorType::AccessorType type = uci::type::accessorType::runwayID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RunwayID */
         virtual void clearRunwayID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RoutePathType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RoutePathType to copy from
           */
         RoutePathType(const RoutePathType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RoutePathType to the contents of the RoutePathType on the right
           * hand side (rhs) of the assignment operator.RoutePathType [only available to derived classes]
           *
           * @param rhs The RoutePathType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::RoutePathType
           * @return A constant reference to this RoutePathType.
           */
         const RoutePathType& operator=(const RoutePathType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RoutePathType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RoutePathType_h

