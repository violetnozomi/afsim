// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__MTI_ActivityStatusBaseType_h
#define Uci__Type__MTI_ActivityStatusBaseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ConcurrentCollectionDetailsType_h)
# include "uci/type/ConcurrentCollectionDetailsType.h"
#endif

#if !defined(Uci__Type__RelativeDirectionEnum_h)
# include "uci/type/RelativeDirectionEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Set of metrics used to describe the progress of an activity (sensor collection). This base type is intended to be
        * reused across multiple domains.
        */
      class MTI_ActivityStatusBaseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MTI_ActivityStatusBaseType()
         { }

         /** Returns this accessor's type constant, i.e. MTI_ActivityStatusBaseType
           *
           * @return This accessor's type constant, i.e. MTI_ActivityStatusBaseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::mTI_ActivityStatusBaseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MTI_ActivityStatusBaseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the activity IDs and time windows of activities which ran concurrent with this activity. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ConcurrentCollectionDetailsType, uci::type::accessorType::concurrentCollectionDetailsType> ConcurrentActivity;

         /** Returns the value of the SimplePrimitive data type that is identified by the NumberOfTimesSearched.
           *
           * @return The value of the simple primitive data type identified by NumberOfTimesSearched.
           */
         virtual xs::UnsignedInt getNumberOfTimesSearched() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NumberOfTimesSearched.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNumberOfTimesSearched(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NumberOfTimesSearched exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NumberOfTimesSearched is emabled or not
           */
         virtual bool hasNumberOfTimesSearched() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NumberOfTimesSearched
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableNumberOfTimesSearched(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NumberOfTimesSearched */
         virtual void clearNumberOfTimesSearched()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TimeLastSearched.
           *
           * @return The value of the simple primitive data type identified by TimeLastSearched.
           */
         virtual uci::type::DateTimeTypeValue getTimeLastSearched() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TimeLastSearched.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTimeLastSearched(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TimeLastSearched exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TimeLastSearched is emabled or not
           */
         virtual bool hasTimeLastSearched() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TimeLastSearched
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableTimeLastSearched(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TimeLastSearched */
         virtual void clearTimeLastSearched()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Attempts.
           *
           * @return The value of the simple primitive data type identified by Attempts.
           */
         virtual xs::UnsignedInt getAttempts() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Attempts.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAttempts(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Attempts exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Attempts is emabled or not
           */
         virtual bool hasAttempts() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Attempts
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableAttempts(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Attempts */
         virtual void clearAttempts()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ActualRevisitPeriod.
           *
           * @return The value of the simple primitive data type identified by ActualRevisitPeriod.
           */
         virtual uci::type::DurationTypeValue getActualRevisitPeriod() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ActualRevisitPeriod.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setActualRevisitPeriod(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ActualRevisitPeriod exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ActualRevisitPeriod is emabled or not
           */
         virtual bool hasActualRevisitPeriod() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ActualRevisitPeriod
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableActualRevisitPeriod(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ActualRevisitPeriod */
         virtual void clearActualRevisitPeriod()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PercentageInFOR.
           *
           * @return The value of the simple primitive data type identified by PercentageInFOR.
           */
         virtual uci::type::PercentTypeValue getPercentageInFOR() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PercentageInFOR.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPercentageInFOR(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PercentageInFOR exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PercentageInFOR is emabled or not
           */
         virtual bool hasPercentageInFOR() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PercentageInFOR
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enablePercentageInFOR(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PercentageInFOR */
         virtual void clearPercentageInFOR()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ActualStartTime.
           *
           * @return The value of the simple primitive data type identified by ActualStartTime.
           */
         virtual uci::type::DateTimeTypeValue getActualStartTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ActualStartTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setActualStartTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ActualStartTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ActualStartTime is emabled or not
           */
         virtual bool hasActualStartTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ActualStartTime
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableActualStartTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ActualStartTime */
         virtual void clearActualStartTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ActualStopTime.
           *
           * @return The value of the simple primitive data type identified by ActualStopTime.
           */
         virtual uci::type::DateTimeTypeValue getActualStopTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ActualStopTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setActualStopTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ActualStopTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ActualStopTime is emabled or not
           */
         virtual bool hasActualStopTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ActualStopTime
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableActualStopTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ActualStopTime */
         virtual void clearActualStopTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PercentSearched.
           *
           * @return The value of the simple primitive data type identified by PercentSearched.
           */
         virtual uci::type::PercentTypeValue getPercentSearched() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PercentSearched.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPercentSearched(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PercentSearched exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PercentSearched is emabled or not
           */
         virtual bool hasPercentSearched() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PercentSearched
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enablePercentSearched(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PercentSearched */
         virtual void clearPercentSearched()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EntitiesReported.
           *
           * @return The value of the simple primitive data type identified by EntitiesReported.
           */
         virtual xs::UnsignedInt getEntitiesReported() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EntitiesReported.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEntitiesReported(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EntitiesReported exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EntitiesReported is emabled or not
           */
         virtual bool hasEntitiesReported() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EntitiesReported
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableEntitiesReported(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EntitiesReported */
         virtual void clearEntitiesReported()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CueAcquired.
           *
           * @return The value of the simple primitive data type identified by CueAcquired.
           */
         virtual xs::Boolean getCueAcquired() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CueAcquired.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCueAcquired(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CueAcquired exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CueAcquired is emabled or not
           */
         virtual bool hasCueAcquired() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CueAcquired
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableCueAcquired(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CueAcquired */
         virtual void clearCueAcquired()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ConcurrentActivity.
           *
           * @return The bounded list identified by ConcurrentActivity.
           */
         virtual const uci::type::MTI_ActivityStatusBaseType::ConcurrentActivity& getConcurrentActivity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ConcurrentActivity.
           *
           * @return The bounded list identified by ConcurrentActivity.
           */
         virtual uci::type::MTI_ActivityStatusBaseType::ConcurrentActivity& getConcurrentActivity()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ConcurrentActivity.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setConcurrentActivity(const uci::type::MTI_ActivityStatusBaseType::ConcurrentActivity& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PercentOccluded.
           *
           * @return The value of the simple primitive data type identified by PercentOccluded.
           */
         virtual uci::type::PercentTypeValue getPercentOccluded() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PercentOccluded.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPercentOccluded(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PercentOccluded exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PercentOccluded is emabled or not
           */
         virtual bool hasPercentOccluded() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PercentOccluded
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enablePercentOccluded(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PercentOccluded */
         virtual void clearPercentOccluded()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PercentRadarTimeline.
           *
           * @return The value of the simple primitive data type identified by PercentRadarTimeline.
           */
         virtual uci::type::PercentTypeValue getPercentRadarTimeline() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PercentRadarTimeline.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPercentRadarTimeline(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PercentRadarTimeline exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PercentRadarTimeline is emabled or not
           */
         virtual bool hasPercentRadarTimeline() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PercentRadarTimeline
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enablePercentRadarTimeline(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PercentRadarTimeline */
         virtual void clearPercentRadarTimeline()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PercentageInFOV.
           *
           * @return The value of the simple primitive data type identified by PercentageInFOV.
           */
         virtual uci::type::PercentTypeValue getPercentageInFOV() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PercentageInFOV.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPercentageInFOV(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PercentageInFOV exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PercentageInFOV is emabled or not
           */
         virtual bool hasPercentageInFOV() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PercentageInFOV
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enablePercentageInFOV(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PercentageInFOV */
         virtual void clearPercentageInFOV()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RelativeDirectionOfTarget.
           *
           * @return The value of the enumeration identified by RelativeDirectionOfTarget.
           */
         virtual const uci::type::RelativeDirectionEnum& getRelativeDirectionOfTarget() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RelativeDirectionOfTarget.
           *
           * @return The value of the enumeration identified by RelativeDirectionOfTarget.
           */
         virtual uci::type::RelativeDirectionEnum& getRelativeDirectionOfTarget()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RelativeDirectionOfTarget.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRelativeDirectionOfTarget(const uci::type::RelativeDirectionEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RelativeDirectionOfTarget.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRelativeDirectionOfTarget(uci::type::RelativeDirectionEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield RelativeDirectionOfTargetis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasRelativeDirectionOfTarget() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getRelativeDirectionOfTarget will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::relativeDirectionEnum This Accessor's accessor type
           */
         virtual void enableRelativeDirectionOfTarget(uci::base::accessorType::AccessorType type = uci::type::accessorType::relativeDirectionEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearRelativeDirectionOfTarget()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MTI_ActivityStatusBaseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MTI_ActivityStatusBaseType to copy from
           */
         MTI_ActivityStatusBaseType(const MTI_ActivityStatusBaseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MTI_ActivityStatusBaseType to the contents of the
           * MTI_ActivityStatusBaseType on the right hand side (rhs) of the assignment operator.MTI_ActivityStatusBaseType [only
           * available to derived classes]
           *
           * @param rhs The MTI_ActivityStatusBaseType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::MTI_ActivityStatusBaseType
           * @return A constant reference to this MTI_ActivityStatusBaseType.
           */
         const MTI_ActivityStatusBaseType& operator=(const MTI_ActivityStatusBaseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MTI_ActivityStatusBaseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MTI_ActivityStatusBaseType_h

