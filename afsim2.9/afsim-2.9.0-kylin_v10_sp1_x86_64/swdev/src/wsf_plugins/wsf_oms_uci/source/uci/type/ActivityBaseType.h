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
#ifndef Uci__Type__ActivityBaseType_h
#define Uci__Type__ActivityBaseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ActivityID_Type_h)
# include "uci/type/ActivityID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__RankingType_h)
# include "uci/type/RankingType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__ActivityStateEnum_h)
# include "uci/type/ActivityStateEnum.h"
#endif

#if !defined(Uci__Type__CannotComplyType_h)
# include "uci/type/CannotComplyType.h"
#endif

#if !defined(Uci__Type__ActivitySourceType_h)
# include "uci/type/ActivitySourceType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ActivityBaseType sequence accessor class */
      class ActivityBaseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ActivityBaseType()
         { }

         /** Returns this accessor's type constant, i.e. ActivityBaseType
           *
           * @return This accessor's type constant, i.e. ActivityBaseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::activityBaseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ActivityBaseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a Capability which the Activity is an instance of. Generally an Activity is an instance of a single
           * Capability. An Activity can be associated with multiple Capabilities when, for example, a Subsystem has distinct
           * Capabilities for automated and manual. In this case, if a manual command is issued while an automated Activity that
           * achieves the desired result already exists, a single Activity represents both Capabilities. [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityID_Type, uci::type::accessorType::capabilityID_Type> CapabilityID;

         /** Indicates an originating "source" of the Activity; the command or other item which invoked this specific Activity of
           * a Capability. A single Activity can fully or partially satisfy Activity sources. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ActivitySourceType, uci::type::accessorType::activitySourceType> Source;

         /** Returns the accessor that provides access to the complex content that is identified by the ActivityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActivityID.
           */
         virtual const uci::type::ActivityID_Type& getActivityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActivityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActivityID.
           */
         virtual uci::type::ActivityID_Type& getActivityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ActivityID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ActivityID
           */
         virtual void setActivityID(const uci::type::ActivityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual const uci::type::ActivityBaseType::CapabilityID& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual uci::type::ActivityBaseType::CapabilityID& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityID(const uci::type::ActivityBaseType::CapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActivityRank.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActivityRank.
           */
         virtual const uci::type::RankingType& getActivityRank() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActivityRank.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActivityRank.
           */
         virtual uci::type::RankingType& getActivityRank()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ActivityRank to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ActivityRank
           */
         virtual void setActivityRank(const uci::type::RankingType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Interactive.
           *
           * @return The value of the simple primitive data type identified by Interactive.
           */
         virtual xs::Boolean getInteractive() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Interactive.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setInteractive(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ActivityState.
           *
           * @return The value of the enumeration identified by ActivityState.
           */
         virtual const uci::type::ActivityStateEnum& getActivityState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ActivityState.
           *
           * @return The value of the enumeration identified by ActivityState.
           */
         virtual uci::type::ActivityStateEnum& getActivityState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ActivityState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setActivityState(const uci::type::ActivityStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ActivityState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setActivityState(uci::type::ActivityStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActivityReason.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActivityReason.
           */
         virtual const uci::type::CannotComplyType& getActivityReason() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActivityReason.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActivityReason.
           */
         virtual uci::type::CannotComplyType& getActivityReason()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ActivityReason to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ActivityReason
           */
         virtual void setActivityReason(const uci::type::CannotComplyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ActivityReason exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ActivityReason is emabled or not
           */
         virtual bool hasActivityReason() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ActivityReason
           *
           * @param type = uci::type::accessorType::cannotComplyType This Accessor's accessor type
           */
         virtual void enableActivityReason(uci::base::accessorType::AccessorType type = uci::type::accessorType::cannotComplyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ActivityReason */
         virtual void clearActivityReason()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Source.
           *
           * @return The bounded list identified by Source.
           */
         virtual const uci::type::ActivityBaseType::Source& getSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Source.
           *
           * @return The bounded list identified by Source.
           */
         virtual uci::type::ActivityBaseType::Source& getSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Source.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSource(const uci::type::ActivityBaseType::Source& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EstimatedCompletionTime.
           *
           * @return The value of the simple primitive data type identified by EstimatedCompletionTime.
           */
         virtual uci::type::DateTimeTypeValue getEstimatedCompletionTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EstimatedCompletionTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEstimatedCompletionTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EstimatedCompletionTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EstimatedCompletionTime is emabled or not
           */
         virtual bool hasEstimatedCompletionTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EstimatedCompletionTime
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableEstimatedCompletionTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EstimatedCompletionTime */
         virtual void clearEstimatedCompletionTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EstimatedPercentComplete.
           *
           * @return The value of the simple primitive data type identified by EstimatedPercentComplete.
           */
         virtual uci::type::PercentTypeValue getEstimatedPercentComplete() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EstimatedPercentComplete.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEstimatedPercentComplete(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EstimatedPercentComplete exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EstimatedPercentComplete is emabled or not
           */
         virtual bool hasEstimatedPercentComplete() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EstimatedPercentComplete
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableEstimatedPercentComplete(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EstimatedPercentComplete */
         virtual void clearEstimatedPercentComplete()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ActivityBaseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ActivityBaseType to copy from
           */
         ActivityBaseType(const ActivityBaseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ActivityBaseType to the contents of the ActivityBaseType on the
           * right hand side (rhs) of the assignment operator.ActivityBaseType [only available to derived classes]
           *
           * @param rhs The ActivityBaseType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::ActivityBaseType
           * @return A constant reference to this ActivityBaseType.
           */
         const ActivityBaseType& operator=(const ActivityBaseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ActivityBaseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ActivityBaseType_h

