// This file was generated  on 12/5/2018 at 1:1:45 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__COMINT_InteractiveType_h
#define Uci__Type__COMINT_InteractiveType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MilliwattPowerRatioType_h)
# include "uci/type/MilliwattPowerRatioType.h"
#endif

#if !defined(Uci__Type__NoiseAdaptiveThresholdStatusEnum_h)
# include "uci/type/NoiseAdaptiveThresholdStatusEnum.h"
#endif

#if !defined(Uci__Type__GainControlMethodEnum_h)
# include "uci/type/GainControlMethodEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__FrequencyType_h)
# include "uci/type/FrequencyType.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Type__COMINT_AcquisitionTargetType_h)
# include "uci/type/COMINT_AcquisitionTargetType.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the COMINT_InteractiveType sequence accessor class */
      class COMINT_InteractiveType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~COMINT_InteractiveType()
         { }

         /** Returns this accessor's type constant, i.e. COMINT_InteractiveType
           *
           * @return This accessor's type constant, i.e. COMINT_InteractiveType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::cOMINT_InteractiveType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const COMINT_InteractiveType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Define a center frequency or a list of center frequencies to be scanned [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::FrequencyType, uci::type::accessorType::frequencyType> CenterFrequency;

         /** The list of frequency ranges that form the scan list [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> ScanRange;

         /** The list of frequencies that the subsystem should not scan [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> ScanLockout;

         /** Defines targets to be ignored [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::COMINT_AcquisitionTargetType, uci::type::accessorType::cOMINT_AcquisitionTargetType> IgnoreTarget;

         /** The Technique ID to be used by the application. This represents a technique selectable within an application, and is
           * separate from the ApplicationID index. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ForeignKeyType, uci::type::accessorType::foreignKeyType> TechniqueInUse;

         /** Application-specific parameters for the technique in use. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ForeignKeyType, uci::type::accessorType::foreignKeyType> TechniqueParameters;

         /** Returns the value of the SimplePrimitive data type that is identified by the Threshold.
           *
           * @return The value of the simple primitive data type identified by Threshold.
           */
         virtual uci::type::MilliwattPowerRatioTypeValue getThreshold() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Threshold.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setThreshold(uci::type::MilliwattPowerRatioTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Threshold exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Threshold is emabled or not
           */
         virtual bool hasThreshold() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Threshold
           *
           * @param type = uci::type::accessorType::milliwattPowerRatioType This Accessor's accessor type
           */
         virtual void enableThreshold(uci::base::accessorType::AccessorType type = uci::type::accessorType::milliwattPowerRatioType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Threshold */
         virtual void clearThreshold()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the NoiseAdaptiveThresholdStatus.
           *
           * @return The value of the enumeration identified by NoiseAdaptiveThresholdStatus.
           */
         virtual const uci::type::NoiseAdaptiveThresholdStatusEnum& getNoiseAdaptiveThresholdStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the NoiseAdaptiveThresholdStatus.
           *
           * @return The value of the enumeration identified by NoiseAdaptiveThresholdStatus.
           */
         virtual uci::type::NoiseAdaptiveThresholdStatusEnum& getNoiseAdaptiveThresholdStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the NoiseAdaptiveThresholdStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setNoiseAdaptiveThresholdStatus(const uci::type::NoiseAdaptiveThresholdStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the NoiseAdaptiveThresholdStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setNoiseAdaptiveThresholdStatus(uci::type::NoiseAdaptiveThresholdStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield NoiseAdaptiveThresholdStatusis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasNoiseAdaptiveThresholdStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getNoiseAdaptiveThresholdStatus will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::noiseAdaptiveThresholdStatusEnum This Accessor's accessor type
           */
         virtual void enableNoiseAdaptiveThresholdStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::noiseAdaptiveThresholdStatusEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearNoiseAdaptiveThresholdStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the GainControlMethod.
           *
           * @return The value of the enumeration identified by GainControlMethod.
           */
         virtual const uci::type::GainControlMethodEnum& getGainControlMethod() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the GainControlMethod.
           *
           * @return The value of the enumeration identified by GainControlMethod.
           */
         virtual uci::type::GainControlMethodEnum& getGainControlMethod()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the GainControlMethod.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setGainControlMethod(const uci::type::GainControlMethodEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the GainControlMethod.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setGainControlMethod(uci::type::GainControlMethodEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield GainControlMethodis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasGainControlMethod() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getGainControlMethod will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::gainControlMethodEnum This Accessor's accessor type
           */
         virtual void enableGainControlMethod(uci::base::accessorType::AccessorType type = uci::type::accessorType::gainControlMethodEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearGainControlMethod()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Gain.
           *
           * @return The value of the simple primitive data type identified by Gain.
           */
         virtual uci::type::MilliwattPowerRatioTypeValue getGain() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Gain.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setGain(uci::type::MilliwattPowerRatioTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Gain exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Gain is emabled or not
           */
         virtual bool hasGain() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Gain
           *
           * @param type = uci::type::accessorType::milliwattPowerRatioType This Accessor's accessor type
           */
         virtual void enableGain(uci::base::accessorType::AccessorType type = uci::type::accessorType::milliwattPowerRatioType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Gain */
         virtual void clearGain()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CenterFrequency.
           *
           * @return The bounded list identified by CenterFrequency.
           */
         virtual const uci::type::COMINT_InteractiveType::CenterFrequency& getCenterFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CenterFrequency.
           *
           * @return The bounded list identified by CenterFrequency.
           */
         virtual uci::type::COMINT_InteractiveType::CenterFrequency& getCenterFrequency()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CenterFrequency.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCenterFrequency(const uci::type::COMINT_InteractiveType::CenterFrequency& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ScanRange.
           *
           * @return The bounded list identified by ScanRange.
           */
         virtual const uci::type::COMINT_InteractiveType::ScanRange& getScanRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ScanRange.
           *
           * @return The bounded list identified by ScanRange.
           */
         virtual uci::type::COMINT_InteractiveType::ScanRange& getScanRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ScanRange.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setScanRange(const uci::type::COMINT_InteractiveType::ScanRange& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ScanLockout.
           *
           * @return The bounded list identified by ScanLockout.
           */
         virtual const uci::type::COMINT_InteractiveType::ScanLockout& getScanLockout() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ScanLockout.
           *
           * @return The bounded list identified by ScanLockout.
           */
         virtual uci::type::COMINT_InteractiveType::ScanLockout& getScanLockout()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ScanLockout.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setScanLockout(const uci::type::COMINT_InteractiveType::ScanLockout& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DwellTarget.
           *
           * @return The acecssor that provides access to the complex content that is identified by DwellTarget.
           */
         virtual const uci::type::COMINT_AcquisitionTargetType& getDwellTarget() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DwellTarget.
           *
           * @return The acecssor that provides access to the complex content that is identified by DwellTarget.
           */
         virtual uci::type::COMINT_AcquisitionTargetType& getDwellTarget()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DwellTarget to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DwellTarget
           */
         virtual void setDwellTarget(const uci::type::COMINT_AcquisitionTargetType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DwellTarget exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DwellTarget is emabled or not
           */
         virtual bool hasDwellTarget() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DwellTarget
           *
           * @param type = uci::type::accessorType::cOMINT_AcquisitionTargetType This Accessor's accessor type
           */
         virtual void enableDwellTarget(uci::base::accessorType::AccessorType type = uci::type::accessorType::cOMINT_AcquisitionTargetType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DwellTarget */
         virtual void clearDwellTarget()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the IgnoreTarget.
           *
           * @return The bounded list identified by IgnoreTarget.
           */
         virtual const uci::type::COMINT_InteractiveType::IgnoreTarget& getIgnoreTarget() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the IgnoreTarget.
           *
           * @return The bounded list identified by IgnoreTarget.
           */
         virtual uci::type::COMINT_InteractiveType::IgnoreTarget& getIgnoreTarget()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the IgnoreTarget.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setIgnoreTarget(const uci::type::COMINT_InteractiveType::IgnoreTarget& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TechniqueInUse.
           *
           * @return The bounded list identified by TechniqueInUse.
           */
         virtual const uci::type::COMINT_InteractiveType::TechniqueInUse& getTechniqueInUse() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TechniqueInUse.
           *
           * @return The bounded list identified by TechniqueInUse.
           */
         virtual uci::type::COMINT_InteractiveType::TechniqueInUse& getTechniqueInUse()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TechniqueInUse.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTechniqueInUse(const uci::type::COMINT_InteractiveType::TechniqueInUse& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TechniqueParameters.
           *
           * @return The bounded list identified by TechniqueParameters.
           */
         virtual const uci::type::COMINT_InteractiveType::TechniqueParameters& getTechniqueParameters() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TechniqueParameters.
           *
           * @return The bounded list identified by TechniqueParameters.
           */
         virtual uci::type::COMINT_InteractiveType::TechniqueParameters& getTechniqueParameters()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TechniqueParameters.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTechniqueParameters(const uci::type::COMINT_InteractiveType::TechniqueParameters& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AudioEnable.
           *
           * @return The value of the simple primitive data type identified by AudioEnable.
           */
         virtual xs::Boolean getAudioEnable() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AudioEnable.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAudioEnable(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AudioEnable exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AudioEnable is emabled or not
           */
         virtual bool hasAudioEnable() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AudioEnable
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableAudioEnable(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AudioEnable */
         virtual void clearAudioEnable()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IQ_Enable.
           *
           * @return The value of the simple primitive data type identified by IQ_Enable.
           */
         virtual xs::Boolean getIQ_Enable() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IQ_Enable.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIQ_Enable(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IQ_Enable exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IQ_Enable is emabled or not
           */
         virtual bool hasIQ_Enable() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IQ_Enable
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableIQ_Enable(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IQ_Enable */
         virtual void clearIQ_Enable()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         COMINT_InteractiveType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The COMINT_InteractiveType to copy from
           */
         COMINT_InteractiveType(const COMINT_InteractiveType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this COMINT_InteractiveType to the contents of the
           * COMINT_InteractiveType on the right hand side (rhs) of the assignment operator.COMINT_InteractiveType [only available
           * to derived classes]
           *
           * @param rhs The COMINT_InteractiveType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::COMINT_InteractiveType
           * @return A constant reference to this COMINT_InteractiveType.
           */
         const COMINT_InteractiveType& operator=(const COMINT_InteractiveType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // COMINT_InteractiveType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__COMINT_InteractiveType_h

