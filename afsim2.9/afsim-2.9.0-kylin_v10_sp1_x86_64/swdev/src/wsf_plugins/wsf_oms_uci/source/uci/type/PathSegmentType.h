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
#ifndef Uci__Type__PathSegmentType_h
#define Uci__Type__PathSegmentType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SegmentID_Type_h)
# include "uci/type/SegmentID_Type.h"
#endif

#if !defined(Uci__Type__PathSegmentSourceEnum_h)
# include "uci/type/PathSegmentSourceEnum.h"
#endif

#if !defined(Uci__Type__EndPointType_h)
# include "uci/type/EndPointType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__PathSegmentSpeedType_h)
# include "uci/type/PathSegmentSpeedType.h"
#endif

#if !defined(Uci__Type__ClimbType_h)
# include "uci/type/ClimbType.h"
#endif

#if !defined(Uci__Type__AngleType_h)
# include "uci/type/AngleType.h"
#endif

#if !defined(Uci__Type__AccelerationType_h)
# include "uci/type/AccelerationType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__PathSegmentActionType_h)
# include "uci/type/PathSegmentActionType.h"
#endif

#if !defined(Uci__Type__NextPathSegmentType_h)
# include "uci/type/NextPathSegmentType.h"
#endif

#if !defined(Uci__Type__ConditionalPathSegmentType_h)
# include "uci/type/ConditionalPathSegmentType.h"
#endif

#if !defined(Uci__Type__MetricsType_h)
# include "uci/type/MetricsType.h"
#endif

#if !defined(Uci__Type__InertialStateType_h)
# include "uci/type/InertialStateType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PathSegmentType sequence accessor class */
      class PathSegmentType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PathSegmentType()
         { }

         /** Returns this accessor's type constant, i.e. PathSegmentType
           *
           * @return This accessor's type constant, i.e. PathSegmentType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pathSegmentType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PathSegmentType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates an action that is planned to occur on the PathSegment at the time/location specified by the Trigger
           * element, Multiple Actions occurring at different times along a straight segment can be specified with 1) an EndPoint
           * for each action to accurately describe where they will occur or 2) using the Trigger element to arrange the Actions
           * along a single PathSegment with a single EndPoint. The latter approach is required for simultaneous Actions. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PathSegmentActionType, uci::type::accessorType::pathSegmentActionType> Action;

         /** This element is used to specify one or more conditional PathSegment. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ConditionalPathSegmentType, uci::type::accessorType::conditionalPathSegmentType> ConditionalPathSegment;

         /** This element is intended to provide detailed inertial state information at user defined increments within a path
           * segment. This element provides data which may be required for pre-mission analysis of a Route. [Minimum occurrences:
           * 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::InertialStateType, uci::type::accessorType::inertialStateType> InertialState;

         /** Returns the accessor that provides access to the complex content that is identified by the PathSegmentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by PathSegmentID.
           */
         virtual const uci::type::SegmentID_Type& getPathSegmentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PathSegmentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by PathSegmentID.
           */
         virtual uci::type::SegmentID_Type& getPathSegmentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PathSegmentID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PathSegmentID
           */
         virtual void setPathSegmentID(const uci::type::SegmentID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Source.
           *
           * @return The value of the enumeration identified by Source.
           */
         virtual const uci::type::PathSegmentSourceEnum& getSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Source.
           *
           * @return The value of the enumeration identified by Source.
           */
         virtual uci::type::PathSegmentSourceEnum& getSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Source.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSource(const uci::type::PathSegmentSourceEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Source.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSource(uci::type::PathSegmentSourceEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EndPoint.
           *
           * @return The acecssor that provides access to the complex content that is identified by EndPoint.
           */
         virtual const uci::type::EndPointType& getEndPoint() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EndPoint.
           *
           * @return The acecssor that provides access to the complex content that is identified by EndPoint.
           */
         virtual uci::type::EndPointType& getEndPoint()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EndPoint to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EndPoint
           */
         virtual void setEndPoint(const uci::type::EndPointType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Locked.
           *
           * @return The value of the simple primitive data type identified by Locked.
           */
         virtual xs::Boolean getLocked() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Locked.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setLocked(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Locked exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Locked is emabled or not
           */
         virtual bool hasLocked() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Locked
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableLocked(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Locked */
         virtual void clearLocked()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Modified.
           *
           * @return The value of the simple primitive data type identified by Modified.
           */
         virtual xs::Boolean getModified() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Modified.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setModified(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Modified exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Modified is emabled or not
           */
         virtual bool hasModified() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Modified
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableModified(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Modified */
         virtual void clearModified()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Speed.
           *
           * @return The acecssor that provides access to the complex content that is identified by Speed.
           */
         virtual const uci::type::PathSegmentSpeedType& getSpeed() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Speed.
           *
           * @return The acecssor that provides access to the complex content that is identified by Speed.
           */
         virtual uci::type::PathSegmentSpeedType& getSpeed()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Speed to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Speed
           */
         virtual void setSpeed(const uci::type::PathSegmentSpeedType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Speed exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Speed is emabled or not
           */
         virtual bool hasSpeed() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Speed
           *
           * @param type = uci::type::accessorType::pathSegmentSpeedType This Accessor's accessor type
           */
         virtual void enableSpeed(uci::base::accessorType::AccessorType type = uci::type::accessorType::pathSegmentSpeedType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Speed */
         virtual void clearSpeed()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Climb.
           *
           * @return The acecssor that provides access to the complex content that is identified by Climb.
           */
         virtual const uci::type::ClimbType& getClimb() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Climb.
           *
           * @return The acecssor that provides access to the complex content that is identified by Climb.
           */
         virtual uci::type::ClimbType& getClimb()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Climb to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Climb
           */
         virtual void setClimb(const uci::type::ClimbType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Climb exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Climb is emabled or not
           */
         virtual bool hasClimb() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Climb
           *
           * @param type = uci::type::accessorType::climbType This Accessor's accessor type
           */
         virtual void enableClimb(uci::base::accessorType::AccessorType type = uci::type::accessorType::climbType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Climb */
         virtual void clearClimb()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MaximumRoll.
           *
           * @return The value of the simple primitive data type identified by MaximumRoll.
           */
         virtual uci::type::AngleTypeValue getMaximumRoll() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MaximumRoll.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMaximumRoll(uci::type::AngleTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MaximumRoll exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MaximumRoll is emabled or not
           */
         virtual bool hasMaximumRoll() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MaximumRoll
           *
           * @param type = uci::type::accessorType::angleType This Accessor's accessor type
           */
         virtual void enableMaximumRoll(uci::base::accessorType::AccessorType type = uci::type::accessorType::angleType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MaximumRoll */
         virtual void clearMaximumRoll()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Acceleration.
           *
           * @return The value of the simple primitive data type identified by Acceleration.
           */
         virtual uci::type::AccelerationTypeValue getAcceleration() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Acceleration.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAcceleration(uci::type::AccelerationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Acceleration exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Acceleration is emabled or not
           */
         virtual bool hasAcceleration() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Acceleration
           *
           * @param type = uci::type::accessorType::accelerationType This Accessor's accessor type
           */
         virtual void enableAcceleration(uci::base::accessorType::AccessorType type = uci::type::accessorType::accelerationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Acceleration */
         virtual void clearAcceleration()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Action.
           *
           * @return The bounded list identified by Action.
           */
         virtual const uci::type::PathSegmentType::Action& getAction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Action.
           *
           * @return The bounded list identified by Action.
           */
         virtual uci::type::PathSegmentType::Action& getAction()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Action.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAction(const uci::type::PathSegmentType::Action& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NextPathSegment.
           *
           * @return The acecssor that provides access to the complex content that is identified by NextPathSegment.
           */
         virtual const uci::type::NextPathSegmentType& getNextPathSegment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NextPathSegment.
           *
           * @return The acecssor that provides access to the complex content that is identified by NextPathSegment.
           */
         virtual uci::type::NextPathSegmentType& getNextPathSegment()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the NextPathSegment to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by NextPathSegment
           */
         virtual void setNextPathSegment(const uci::type::NextPathSegmentType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NextPathSegment exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NextPathSegment is emabled or not
           */
         virtual bool hasNextPathSegment() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NextPathSegment
           *
           * @param type = uci::type::accessorType::nextPathSegmentType This Accessor's accessor type
           */
         virtual void enableNextPathSegment(uci::base::accessorType::AccessorType type = uci::type::accessorType::nextPathSegmentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NextPathSegment */
         virtual void clearNextPathSegment()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ConditionalPathSegment.
           *
           * @return The bounded list identified by ConditionalPathSegment.
           */
         virtual const uci::type::PathSegmentType::ConditionalPathSegment& getConditionalPathSegment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ConditionalPathSegment.
           *
           * @return The bounded list identified by ConditionalPathSegment.
           */
         virtual uci::type::PathSegmentType::ConditionalPathSegment& getConditionalPathSegment()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ConditionalPathSegment.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setConditionalPathSegment(const uci::type::PathSegmentType::ConditionalPathSegment& value)
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


         /** Returns the bounded list that is identified by the InertialState.
           *
           * @return The bounded list identified by InertialState.
           */
         virtual const uci::type::PathSegmentType::InertialState& getInertialState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the InertialState.
           *
           * @return The bounded list identified by InertialState.
           */
         virtual uci::type::PathSegmentType::InertialState& getInertialState()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the InertialState.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setInertialState(const uci::type::PathSegmentType::InertialState& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PathSegmentType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PathSegmentType to copy from
           */
         PathSegmentType(const PathSegmentType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PathSegmentType to the contents of the PathSegmentType on the
           * right hand side (rhs) of the assignment operator.PathSegmentType [only available to derived classes]
           *
           * @param rhs The PathSegmentType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::PathSegmentType
           * @return A constant reference to this PathSegmentType.
           */
         const PathSegmentType& operator=(const PathSegmentType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PathSegmentType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PathSegmentType_h

