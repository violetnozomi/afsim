// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__SAR_PredictedActivityDetailType_h
#define Uci__Type__SAR_PredictedActivityDetailType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ActivityStateEnum_h)
# include "uci/type/ActivityStateEnum.h"
#endif

#if !defined(Uci__Type__CannotComplyType_h)
# include "uci/type/CannotComplyType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__DecibelType_h)
# include "uci/type/DecibelType.h"
#endif

#if !defined(Uci__Base__FloatAccessor_h)
# include "uci/base/FloatAccessor.h"
#endif

#if !defined(Uci__Base__DoubleAccessor_h)
# include "uci/base/DoubleAccessor.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SAR_PredictedActivityDetailType sequence accessor class */
      class SAR_PredictedActivityDetailType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SAR_PredictedActivityDetailType()
         { }

         /** Returns this accessor's type constant, i.e. SAR_PredictedActivityDetailType
           *
           * @return This accessor's type constant, i.e. SAR_PredictedActivityDetailType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sAR_PredictedActivityDetailType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SAR_PredictedActivityDetailType& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum inclusive: -4.0] [Maximum inclusive: 4.0] */
         typedef uci::base::FloatAccessor EstimatedAchievableEllipticity;
         //  The type of primitive data accessed by EstimatedAchievableEllipticity
         typedef xs::Float EstimatedAchievableEllipticityValue;

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


         /** Returns whether this optionalfield ActivityStateis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasActivityState() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getActivityState will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::activityStateEnum This Accessor's accessor type
           */
         virtual void enableActivityState(uci::base::accessorType::AccessorType type = uci::type::accessorType::activityStateEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearActivityState()
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


         /** Returns the value of the SimplePrimitive data type that is identified by the EstimatedStartTime.
           *
           * @return The value of the simple primitive data type identified by EstimatedStartTime.
           */
         virtual uci::type::DateTimeTypeValue getEstimatedStartTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EstimatedStartTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEstimatedStartTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EstimatedStartTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EstimatedStartTime is emabled or not
           */
         virtual bool hasEstimatedStartTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EstimatedStartTime
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableEstimatedStartTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EstimatedStartTime */
         virtual void clearEstimatedStartTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EstimatedStopTime.
           *
           * @return The value of the simple primitive data type identified by EstimatedStopTime.
           */
         virtual uci::type::DateTimeTypeValue getEstimatedStopTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EstimatedStopTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEstimatedStopTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EstimatedStopTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EstimatedStopTime is emabled or not
           */
         virtual bool hasEstimatedStopTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EstimatedStopTime
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableEstimatedStopTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EstimatedStopTime */
         virtual void clearEstimatedStopTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EstimatedERP.
           *
           * @return The value of the simple primitive data type identified by EstimatedERP.
           */
         virtual uci::type::DecibelTypeValue getEstimatedERP() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EstimatedERP.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEstimatedERP(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EstimatedERP exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EstimatedERP is emabled or not
           */
         virtual bool hasEstimatedERP() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EstimatedERP
           *
           * @param type = uci::type::accessorType::decibelType This Accessor's accessor type
           */
         virtual void enableEstimatedERP(uci::base::accessorType::AccessorType type = uci::type::accessorType::decibelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EstimatedERP */
         virtual void clearEstimatedERP()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EstimatedMinimumCNR.
           *
           * @return The value of the simple primitive data type identified by EstimatedMinimumCNR.
           */
         virtual uci::type::DecibelTypeValue getEstimatedMinimumCNR() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EstimatedMinimumCNR.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEstimatedMinimumCNR(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EstimatedMinimumCNR exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EstimatedMinimumCNR is emabled or not
           */
         virtual bool hasEstimatedMinimumCNR() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EstimatedMinimumCNR
           *
           * @param type = uci::type::accessorType::decibelType This Accessor's accessor type
           */
         virtual void enableEstimatedMinimumCNR(uci::base::accessorType::AccessorType type = uci::type::accessorType::decibelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EstimatedMinimumCNR */
         virtual void clearEstimatedMinimumCNR()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EstimatedAchievableEllipticity.
           *
           * @return The value of the simple primitive data type identified by EstimatedAchievableEllipticity.
           */
         virtual uci::type::SAR_PredictedActivityDetailType::EstimatedAchievableEllipticityValue getEstimatedAchievableEllipticity() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EstimatedAchievableEllipticity.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEstimatedAchievableEllipticity(uci::type::SAR_PredictedActivityDetailType::EstimatedAchievableEllipticityValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EstimatedAchievableEllipticity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EstimatedAchievableEllipticity is emabled or not
           */
         virtual bool hasEstimatedAchievableEllipticity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EstimatedAchievableEllipticity
           *
           * @param type = uci::type::sAR_PredictedActivityDetailType::accessorType::estimatedAchievableEllipticity This
           *      Accessor's accessor type
           */
         virtual void enableEstimatedAchievableEllipticity(uci::base::accessorType::AccessorType type = uci::type::sAR_PredictedActivityDetailType::accessorType::estimatedAchievableEllipticity)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EstimatedAchievableEllipticity */
         virtual void clearEstimatedAchievableEllipticity()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EstimatedAchievableResolution.
           *
           * @return The value of the simple primitive data type identified by EstimatedAchievableResolution.
           */
         virtual xs::Double getEstimatedAchievableResolution() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EstimatedAchievableResolution.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEstimatedAchievableResolution(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EstimatedAchievableResolution exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EstimatedAchievableResolution is emabled or not
           */
         virtual bool hasEstimatedAchievableResolution() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EstimatedAchievableResolution
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enableEstimatedAchievableResolution(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EstimatedAchievableResolution */
         virtual void clearEstimatedAchievableResolution()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EstimatedPercentageInFOR.
           *
           * @return The value of the simple primitive data type identified by EstimatedPercentageInFOR.
           */
         virtual uci::type::PercentTypeValue getEstimatedPercentageInFOR() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EstimatedPercentageInFOR.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEstimatedPercentageInFOR(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EstimatedPercentageInFOR exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EstimatedPercentageInFOR is emabled or not
           */
         virtual bool hasEstimatedPercentageInFOR() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EstimatedPercentageInFOR
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableEstimatedPercentageInFOR(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EstimatedPercentageInFOR */
         virtual void clearEstimatedPercentageInFOR()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EstimatedPercentageOccluded.
           *
           * @return The value of the simple primitive data type identified by EstimatedPercentageOccluded.
           */
         virtual uci::type::PercentTypeValue getEstimatedPercentageOccluded() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EstimatedPercentageOccluded.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEstimatedPercentageOccluded(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EstimatedPercentageOccluded exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EstimatedPercentageOccluded is emabled or not
           */
         virtual bool hasEstimatedPercentageOccluded() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EstimatedPercentageOccluded
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableEstimatedPercentageOccluded(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EstimatedPercentageOccluded */
         virtual void clearEstimatedPercentageOccluded()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ConcurrentExecutionAssumed.
           *
           * @return The value of the simple primitive data type identified by ConcurrentExecutionAssumed.
           */
         virtual xs::Boolean getConcurrentExecutionAssumed() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ConcurrentExecutionAssumed.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setConcurrentExecutionAssumed(xs::Boolean value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SAR_PredictedActivityDetailType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SAR_PredictedActivityDetailType to copy from
           */
         SAR_PredictedActivityDetailType(const SAR_PredictedActivityDetailType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SAR_PredictedActivityDetailType to the contents of the
           * SAR_PredictedActivityDetailType on the right hand side (rhs) of the assignment
           * operator.SAR_PredictedActivityDetailType [only available to derived classes]
           *
           * @param rhs The SAR_PredictedActivityDetailType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::SAR_PredictedActivityDetailType
           * @return A constant reference to this SAR_PredictedActivityDetailType.
           */
         const SAR_PredictedActivityDetailType& operator=(const SAR_PredictedActivityDetailType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SAR_PredictedActivityDetailType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SAR_PredictedActivityDetailType_h

