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
#ifndef Uci__Type__PulseType_h
#define Uci__Type__PulseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ComponentID_Type_h)
# include "uci/type/ComponentID_Type.h"
#endif

#if !defined(Uci__Type__DateTimeSigmaType_h)
# include "uci/type/DateTimeSigmaType.h"
#endif

#if !defined(Uci__Type__FrequencyType_h)
# include "uci/type/FrequencyType.h"
#endif

#if !defined(Uci__Type__MilliwattPowerRatioType_h)
# include "uci/type/MilliwattPowerRatioType.h"
#endif

#if !defined(Uci__Type__DecibelType_h)
# include "uci/type/DecibelType.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SignalLOB_Type_h)
# include "uci/type/SignalLOB_Type.h"
#endif

#if !defined(Uci__Type__ModulationOnPulseDataType_h)
# include "uci/type/ModulationOnPulseDataType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PulseType sequence accessor class */
      class PulseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PulseType()
         { }

         /** Returns this accessor's type constant, i.e. PulseType
           *
           * @return This accessor's type constant, i.e. PulseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pulseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PulseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the line of bearing (LOB) to the emitter of the Pulse, otherwise known as the angle of arrival (AOA).
           * [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SignalLOB_Type, uci::type::accessorType::signalLOB_Type> LOB;

         /** Returns the accessor that provides access to the complex content that is identified by the ComponentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ComponentID.
           */
         virtual const uci::type::ComponentID_Type& getComponentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ComponentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ComponentID.
           */
         virtual uci::type::ComponentID_Type& getComponentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ComponentID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ComponentID
           */
         virtual void setComponentID(const uci::type::ComponentID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Timestamp.
           *
           * @return The acecssor that provides access to the complex content that is identified by Timestamp.
           */
         virtual const uci::type::DateTimeSigmaType& getTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Timestamp.
           *
           * @return The acecssor that provides access to the complex content that is identified by Timestamp.
           */
         virtual uci::type::DateTimeSigmaType& getTimestamp()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Timestamp to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Timestamp
           */
         virtual void setTimestamp(const uci::type::DateTimeSigmaType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Frequency.
           *
           * @return The value of the simple primitive data type identified by Frequency.
           */
         virtual uci::type::FrequencyTypeValue getFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Frequency.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFrequency(uci::type::FrequencyTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FrequencyDeviation.
           *
           * @return The value of the simple primitive data type identified by FrequencyDeviation.
           */
         virtual uci::type::FrequencyTypeValue getFrequencyDeviation() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FrequencyDeviation.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFrequencyDeviation(uci::type::FrequencyTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FrequencyDeviation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FrequencyDeviation is emabled or not
           */
         virtual bool hasFrequencyDeviation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FrequencyDeviation
           *
           * @param type = uci::type::accessorType::frequencyType This Accessor's accessor type
           */
         virtual void enableFrequencyDeviation(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FrequencyDeviation */
         virtual void clearFrequencyDeviation()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Amplitude.
           *
           * @return The value of the simple primitive data type identified by Amplitude.
           */
         virtual uci::type::MilliwattPowerRatioTypeValue getAmplitude() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Amplitude.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAmplitude(uci::type::MilliwattPowerRatioTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Amplitude exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Amplitude is emabled or not
           */
         virtual bool hasAmplitude() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Amplitude
           *
           * @param type = uci::type::accessorType::milliwattPowerRatioType This Accessor's accessor type
           */
         virtual void enableAmplitude(uci::base::accessorType::AccessorType type = uci::type::accessorType::milliwattPowerRatioType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Amplitude */
         virtual void clearAmplitude()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RelativeAmplitude.
           *
           * @return The value of the simple primitive data type identified by RelativeAmplitude.
           */
         virtual uci::type::DecibelTypeValue getRelativeAmplitude() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RelativeAmplitude.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRelativeAmplitude(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RelativeAmplitude exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RelativeAmplitude is emabled or not
           */
         virtual bool hasRelativeAmplitude() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RelativeAmplitude
           *
           * @param type = uci::type::accessorType::decibelType This Accessor's accessor type
           */
         virtual void enableRelativeAmplitude(uci::base::accessorType::AccessorType type = uci::type::accessorType::decibelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RelativeAmplitude */
         virtual void clearRelativeAmplitude()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PulseWidth.
           *
           * @return The value of the simple primitive data type identified by PulseWidth.
           */
         virtual uci::type::DurationTypeValue getPulseWidth() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PulseWidth.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPulseWidth(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PulseWidth exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PulseWidth is emabled or not
           */
         virtual bool hasPulseWidth() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PulseWidth
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enablePulseWidth(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PulseWidth */
         virtual void clearPulseWidth()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PulseWidthDeviation.
           *
           * @return The value of the simple primitive data type identified by PulseWidthDeviation.
           */
         virtual uci::type::DurationTypeValue getPulseWidthDeviation() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PulseWidthDeviation.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPulseWidthDeviation(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PulseWidthDeviation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PulseWidthDeviation is emabled or not
           */
         virtual bool hasPulseWidthDeviation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PulseWidthDeviation
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enablePulseWidthDeviation(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PulseWidthDeviation */
         virtual void clearPulseWidthDeviation()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PulseWidthChopped.
           *
           * @return The value of the simple primitive data type identified by PulseWidthChopped.
           */
         virtual xs::Boolean getPulseWidthChopped() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PulseWidthChopped.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPulseWidthChopped(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PulseWidthChopped exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PulseWidthChopped is emabled or not
           */
         virtual bool hasPulseWidthChopped() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PulseWidthChopped
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enablePulseWidthChopped(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PulseWidthChopped */
         virtual void clearPulseWidthChopped()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PulseOnPulse.
           *
           * @return The value of the simple primitive data type identified by PulseOnPulse.
           */
         virtual xs::Boolean getPulseOnPulse() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PulseOnPulse.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPulseOnPulse(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PulseOnPulse exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PulseOnPulse is emabled or not
           */
         virtual bool hasPulseOnPulse() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PulseOnPulse
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enablePulseOnPulse(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PulseOnPulse */
         virtual void clearPulseOnPulse()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SNR.
           *
           * @return The value of the simple primitive data type identified by SNR.
           */
         virtual uci::type::DecibelTypeValue getSNR() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SNR.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSNR(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SNR exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SNR is emabled or not
           */
         virtual bool hasSNR() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SNR
           *
           * @param type = uci::type::accessorType::decibelType This Accessor's accessor type
           */
         virtual void enableSNR(uci::base::accessorType::AccessorType type = uci::type::accessorType::decibelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SNR */
         virtual void clearSNR()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the LOB.
           *
           * @return The bounded list identified by LOB.
           */
         virtual const uci::type::PulseType::LOB& getLOB() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the LOB.
           *
           * @return The bounded list identified by LOB.
           */
         virtual uci::type::PulseType::LOB& getLOB()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the LOB.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setLOB(const uci::type::PulseType::LOB& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ModulationOnPulse.
           *
           * @return The acecssor that provides access to the complex content that is identified by ModulationOnPulse.
           */
         virtual const uci::type::ModulationOnPulseDataType& getModulationOnPulse() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ModulationOnPulse.
           *
           * @return The acecssor that provides access to the complex content that is identified by ModulationOnPulse.
           */
         virtual uci::type::ModulationOnPulseDataType& getModulationOnPulse()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ModulationOnPulse to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ModulationOnPulse
           */
         virtual void setModulationOnPulse(const uci::type::ModulationOnPulseDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ModulationOnPulse exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ModulationOnPulse is emabled or not
           */
         virtual bool hasModulationOnPulse() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ModulationOnPulse
           *
           * @param type = uci::type::accessorType::modulationOnPulseDataType This Accessor's accessor type
           */
         virtual void enableModulationOnPulse(uci::base::accessorType::AccessorType type = uci::type::accessorType::modulationOnPulseDataType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ModulationOnPulse */
         virtual void clearModulationOnPulse()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DynamicRangeExceeded.
           *
           * @return The value of the simple primitive data type identified by DynamicRangeExceeded.
           */
         virtual xs::Boolean getDynamicRangeExceeded() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DynamicRangeExceeded.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDynamicRangeExceeded(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DynamicRangeExceeded exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DynamicRangeExceeded is emabled or not
           */
         virtual bool hasDynamicRangeExceeded() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DynamicRangeExceeded
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableDynamicRangeExceeded(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DynamicRangeExceeded */
         virtual void clearDynamicRangeExceeded()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ThresholdDetection.
           *
           * @return The value of the simple primitive data type identified by ThresholdDetection.
           */
         virtual xs::Boolean getThresholdDetection() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ThresholdDetection.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setThresholdDetection(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ThresholdDetection exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ThresholdDetection is emabled or not
           */
         virtual bool hasThresholdDetection() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ThresholdDetection
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableThresholdDetection(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ThresholdDetection */
         virtual void clearThresholdDetection()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the HighPowerCorruption.
           *
           * @return The value of the simple primitive data type identified by HighPowerCorruption.
           */
         virtual xs::Boolean getHighPowerCorruption() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the HighPowerCorruption.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHighPowerCorruption(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by HighPowerCorruption exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by HighPowerCorruption is emabled or not
           */
         virtual bool hasHighPowerCorruption() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by HighPowerCorruption
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableHighPowerCorruption(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by HighPowerCorruption */
         virtual void clearHighPowerCorruption()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the OnboardInterference.
           *
           * @return The value of the simple primitive data type identified by OnboardInterference.
           */
         virtual xs::Boolean getOnboardInterference() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the OnboardInterference.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setOnboardInterference(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OnboardInterference exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OnboardInterference is emabled or not
           */
         virtual bool hasOnboardInterference() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OnboardInterference
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableOnboardInterference(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OnboardInterference */
         virtual void clearOnboardInterference()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PulseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PulseType to copy from
           */
         PulseType(const PulseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PulseType to the contents of the PulseType on the right hand side
           * (rhs) of the assignment operator.PulseType [only available to derived classes]
           *
           * @param rhs The PulseType on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::PulseType
           * @return A constant reference to this PulseType.
           */
         const PulseType& operator=(const PulseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PulseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PulseType_h

