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
#ifndef Uci__Type__SAR_ActivityStatusDetailType_h
#define Uci__Type__SAR_ActivityStatusDetailType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
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

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ConcurrentCollectionDetailsType_h)
# include "uci/type/ConcurrentCollectionDetailsType.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Type__RelativeDirectionEnum_h)
# include "uci/type/RelativeDirectionEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SAR_ActivityStatusDetailType sequence accessor class */
      class SAR_ActivityStatusDetailType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SAR_ActivityStatusDetailType()
         { }

         /** Returns this accessor's type constant, i.e. SAR_ActivityStatusDetailType
           *
           * @return This accessor's type constant, i.e. SAR_ActivityStatusDetailType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sAR_ActivityStatusDetailType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SAR_ActivityStatusDetailType& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum inclusive: -4.0] [Maximum inclusive: 4.0] */
         typedef uci::base::FloatAccessor AchievableEllipticity;
         //  The type of primitive data accessed by AchievableEllipticity
         typedef xs::Float AchievableEllipticityValue;

         /** This is the list of other activities that were executed concurrently with this activity. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ConcurrentCollectionDetailsType, uci::type::accessorType::concurrentCollectionDetailsType> ConcurrentActivity;

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


         /** Returns the value of the SimplePrimitive data type that is identified by the PercentCoverage.
           *
           * @return The value of the simple primitive data type identified by PercentCoverage.
           */
         virtual uci::type::PercentTypeValue getPercentCoverage() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PercentCoverage.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPercentCoverage(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PercentCoverage exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PercentCoverage is emabled or not
           */
         virtual bool hasPercentCoverage() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PercentCoverage
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enablePercentCoverage(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PercentCoverage */
         virtual void clearPercentCoverage()
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


         /** Returns the value of the SimplePrimitive data type that is identified by the ProductsProduced.
           *
           * @return The value of the simple primitive data type identified by ProductsProduced.
           */
         virtual xs::UnsignedInt getProductsProduced() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ProductsProduced.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setProductsProduced(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ProductsProduced exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ProductsProduced is emabled or not
           */
         virtual bool hasProductsProduced() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ProductsProduced
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableProductsProduced(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ProductsProduced */
         virtual void clearProductsProduced()
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


         /** Returns the value of the SimplePrimitive data type that is identified by the MinimumCNR.
           *
           * @return The value of the simple primitive data type identified by MinimumCNR.
           */
         virtual uci::type::DecibelTypeValue getMinimumCNR() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MinimumCNR.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMinimumCNR(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MinimumCNR exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MinimumCNR is emabled or not
           */
         virtual bool hasMinimumCNR() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MinimumCNR
           *
           * @param type = uci::type::accessorType::decibelType This Accessor's accessor type
           */
         virtual void enableMinimumCNR(uci::base::accessorType::AccessorType type = uci::type::accessorType::decibelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MinimumCNR */
         virtual void clearMinimumCNR()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AchievableEllipticity.
           *
           * @return The value of the simple primitive data type identified by AchievableEllipticity.
           */
         virtual uci::type::SAR_ActivityStatusDetailType::AchievableEllipticityValue getAchievableEllipticity() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AchievableEllipticity.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAchievableEllipticity(uci::type::SAR_ActivityStatusDetailType::AchievableEllipticityValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AchievableEllipticity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AchievableEllipticity is emabled or not
           */
         virtual bool hasAchievableEllipticity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AchievableEllipticity
           *
           * @param type = uci::type::sAR_ActivityStatusDetailType::accessorType::achievableEllipticity This Accessor's accessor
           *      type
           */
         virtual void enableAchievableEllipticity(uci::base::accessorType::AccessorType type = uci::type::sAR_ActivityStatusDetailType::accessorType::achievableEllipticity)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AchievableEllipticity */
         virtual void clearAchievableEllipticity()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AchievableResolution.
           *
           * @return The value of the simple primitive data type identified by AchievableResolution.
           */
         virtual xs::Double getAchievableResolution() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AchievableResolution.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAchievableResolution(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AchievableResolution exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AchievableResolution is emabled or not
           */
         virtual bool hasAchievableResolution() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AchievableResolution
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enableAchievableResolution(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AchievableResolution */
         virtual void clearAchievableResolution()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ConcurrentActivity.
           *
           * @return The bounded list identified by ConcurrentActivity.
           */
         virtual const uci::type::SAR_ActivityStatusDetailType::ConcurrentActivity& getConcurrentActivity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ConcurrentActivity.
           *
           * @return The bounded list identified by ConcurrentActivity.
           */
         virtual uci::type::SAR_ActivityStatusDetailType::ConcurrentActivity& getConcurrentActivity()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ConcurrentActivity.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setConcurrentActivity(const uci::type::SAR_ActivityStatusDetailType::ConcurrentActivity& value)
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


         /** Returns the value of the SimplePrimitive data type that is identified by the TimeToService.
           *
           * @return The value of the simple primitive data type identified by TimeToService.
           */
         virtual uci::type::DurationTypeValue getTimeToService() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TimeToService.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTimeToService(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TimeToService exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TimeToService is emabled or not
           */
         virtual bool hasTimeToService() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TimeToService
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableTimeToService(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TimeToService */
         virtual void clearTimeToService()
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
         SAR_ActivityStatusDetailType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SAR_ActivityStatusDetailType to copy from
           */
         SAR_ActivityStatusDetailType(const SAR_ActivityStatusDetailType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SAR_ActivityStatusDetailType to the contents of the
           * SAR_ActivityStatusDetailType on the right hand side (rhs) of the assignment operator.SAR_ActivityStatusDetailType
           * [only available to derived classes]
           *
           * @param rhs The SAR_ActivityStatusDetailType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::SAR_ActivityStatusDetailType
           * @return A constant reference to this SAR_ActivityStatusDetailType.
           */
         const SAR_ActivityStatusDetailType& operator=(const SAR_ActivityStatusDetailType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SAR_ActivityStatusDetailType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SAR_ActivityStatusDetailType_h

