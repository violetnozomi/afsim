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
#ifndef Uci__Type__SignalType_h
#define Uci__Type__SignalType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SignalID_Type_h)
# include "uci/type/SignalID_Type.h"
#endif

#if !defined(Uci__Type__MessageStateEnum_h)
# include "uci/type/MessageStateEnum.h"
#endif

#if !defined(Uci__Type__SignalSourceType_h)
# include "uci/type/SignalSourceType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__PulseDataID_Type_h)
# include "uci/type/PulseDataID_Type.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__MeasurementQualityEnum_h)
# include "uci/type/MeasurementQualityEnum.h"
#endif

#if !defined(Uci__Type__FrequencyDataType_h)
# include "uci/type/FrequencyDataType.h"
#endif

#if !defined(Uci__Type__PRI_PW_Type_h)
# include "uci/type/PRI_PW_Type.h"
#endif

#if !defined(Uci__Type__SignalAmplitudeType_h)
# include "uci/type/SignalAmplitudeType.h"
#endif

#if !defined(Uci__Type__PolarizationEnum_h)
# include "uci/type/PolarizationEnum.h"
#endif

#if !defined(Uci__Type__ModulationOnPulseDataType_h)
# include "uci/type/ModulationOnPulseDataType.h"
#endif

#if !defined(Uci__Type__ScanDataType_h)
# include "uci/type/ScanDataType.h"
#endif

#if !defined(Uci__Type__EmitterFunctionType_h)
# include "uci/type/EmitterFunctionType.h"
#endif

#if !defined(Uci__Type__SignalIdentityType_h)
# include "uci/type/SignalIdentityType.h"
#endif

#if !defined(Uci__Type__SignalEmitterKinematicsType_h)
# include "uci/type/SignalEmitterKinematicsType.h"
#endif

#if !defined(Uci__Type__SignalNavDataType_h)
# include "uci/type/SignalNavDataType.h"
#endif

#if !defined(Uci__Type__EmptyType_h)
# include "uci/type/EmptyType.h"
#endif

#if !defined(Uci__Type__LockIndicatorEnum_h)
# include "uci/type/LockIndicatorEnum.h"
#endif

#if !defined(Uci__Type__ActivityByType_h)
# include "uci/type/ActivityByType.h"
#endif

#if !defined(Uci__Type__SignalRemoveInfoType_h)
# include "uci/type/SignalRemoveInfoType.h"
#endif

#if !defined(Uci__Type__ModulationEnum_h)
# include "uci/type/ModulationEnum.h"
#endif

#if !defined(Uci__Type__ModulationParameterType_h)
# include "uci/type/ModulationParameterType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SignalType sequence accessor class */
      class SignalType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SignalType()
         { }

         /** Returns this accessor's type constant, i.e. SignalType
           *
           * @return This accessor's type constant, i.e. SignalType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::signalType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SignalType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the unique ID of a PulseData collection associated with the Signal. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PulseDataID_Type, uci::type::accessorType::pulseDataID_Type> AssociatedPulseDataID;

         /** Indicates the measured frequency data for the Signal. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::FrequencyDataType, uci::type::accessorType::frequencyDataType> Frequency;

         /** Indicates Pulse Repetition Interval (PRI) and Pulse Width (PW) data for the Signal. When both are given, they are a
           * "paired" set of PRI and PW. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PRI_PW_Type, uci::type::accessorType::pRI_PW_Type> PRI_PW;

         /** Indicates the kinematic state of the collecting system and antenna at the time of collection of the Signal. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SignalNavDataType, uci::type::accessorType::signalNavDataType> CollectorKinematics;

         /** Indicates a custom modulation parameter that is not covered by other elements in the Signal data structure. This will
           * typically be used to fully cover the fields defined in other interoperability standards such as the JICD. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ModulationParameterType, uci::type::accessorType::modulationParameterType> ModulationParameter;

         /** Returns the accessor that provides access to the complex content that is identified by the SignalID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SignalID.
           */
         virtual const uci::type::SignalID_Type& getSignalID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SignalID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SignalID.
           */
         virtual uci::type::SignalID_Type& getSignalID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SignalID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SignalID
           */
         virtual void setSignalID(const uci::type::SignalID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SignalState.
           *
           * @return The value of the enumeration identified by SignalState.
           */
         virtual const uci::type::MessageStateEnum& getSignalState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SignalState.
           *
           * @return The value of the enumeration identified by SignalState.
           */
         virtual uci::type::MessageStateEnum& getSignalState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SignalState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSignalState(const uci::type::MessageStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SignalState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSignalState(uci::type::MessageStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Source.
           *
           * @return The acecssor that provides access to the complex content that is identified by Source.
           */
         virtual const uci::type::SignalSourceType& getSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Source.
           *
           * @return The acecssor that provides access to the complex content that is identified by Source.
           */
         virtual uci::type::SignalSourceType& getSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Source to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Source
           */
         virtual void setSource(const uci::type::SignalSourceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Source exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Source is emabled or not
           */
         virtual bool hasSource() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Source
           *
           * @param type = uci::type::accessorType::signalSourceType This Accessor's accessor type
           */
         virtual void enableSource(uci::base::accessorType::AccessorType type = uci::type::accessorType::signalSourceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Source */
         virtual void clearSource()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociatedPulseDataID.
           *
           * @return The bounded list identified by AssociatedPulseDataID.
           */
         virtual const uci::type::SignalType::AssociatedPulseDataID& getAssociatedPulseDataID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociatedPulseDataID.
           *
           * @return The bounded list identified by AssociatedPulseDataID.
           */
         virtual uci::type::SignalType::AssociatedPulseDataID& getAssociatedPulseDataID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AssociatedPulseDataID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAssociatedPulseDataID(const uci::type::SignalType::AssociatedPulseDataID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the InitialDetectionTimestamp.
           *
           * @return The value of the simple primitive data type identified by InitialDetectionTimestamp.
           */
         virtual uci::type::DateTimeTypeValue getInitialDetectionTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the InitialDetectionTimestamp.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setInitialDetectionTimestamp(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by InitialDetectionTimestamp exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by InitialDetectionTimestamp is emabled or not
           */
         virtual bool hasInitialDetectionTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by InitialDetectionTimestamp
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableInitialDetectionTimestamp(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by InitialDetectionTimestamp */
         virtual void clearInitialDetectionTimestamp()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the LatestDetectionTimestamp.
           *
           * @return The value of the simple primitive data type identified by LatestDetectionTimestamp.
           */
         virtual uci::type::DateTimeTypeValue getLatestDetectionTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the LatestDetectionTimestamp.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setLatestDetectionTimestamp(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LatestDetectionTimestamp exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LatestDetectionTimestamp is emabled or not
           */
         virtual bool hasLatestDetectionTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LatestDetectionTimestamp
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableLatestDetectionTimestamp(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LatestDetectionTimestamp */
         virtual void clearLatestDetectionTimestamp()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TimestampQuality.
           *
           * @return The value of the enumeration identified by TimestampQuality.
           */
         virtual const uci::type::MeasurementQualityEnum& getTimestampQuality() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TimestampQuality.
           *
           * @return The value of the enumeration identified by TimestampQuality.
           */
         virtual uci::type::MeasurementQualityEnum& getTimestampQuality()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TimestampQuality.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTimestampQuality(const uci::type::MeasurementQualityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TimestampQuality.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTimestampQuality(uci::type::MeasurementQualityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield TimestampQualityis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasTimestampQuality() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getTimestampQuality will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::measurementQualityEnum This Accessor's accessor type
           */
         virtual void enableTimestampQuality(uci::base::accessorType::AccessorType type = uci::type::accessorType::measurementQualityEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearTimestampQuality()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Frequency.
           *
           * @return The bounded list identified by Frequency.
           */
         virtual const uci::type::SignalType::Frequency& getFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Frequency.
           *
           * @return The bounded list identified by Frequency.
           */
         virtual uci::type::SignalType::Frequency& getFrequency()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Frequency.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFrequency(const uci::type::SignalType::Frequency& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PRI_PW.
           *
           * @return The bounded list identified by PRI_PW.
           */
         virtual const uci::type::SignalType::PRI_PW& getPRI_PW() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PRI_PW.
           *
           * @return The bounded list identified by PRI_PW.
           */
         virtual uci::type::SignalType::PRI_PW& getPRI_PW()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the PRI_PW.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPRI_PW(const uci::type::SignalType::PRI_PW& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Amplitude.
           *
           * @return The acecssor that provides access to the complex content that is identified by Amplitude.
           */
         virtual const uci::type::SignalAmplitudeType& getAmplitude() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Amplitude.
           *
           * @return The acecssor that provides access to the complex content that is identified by Amplitude.
           */
         virtual uci::type::SignalAmplitudeType& getAmplitude()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Amplitude to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Amplitude
           */
         virtual void setAmplitude(const uci::type::SignalAmplitudeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Amplitude exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Amplitude is emabled or not
           */
         virtual bool hasAmplitude() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Amplitude
           *
           * @param type = uci::type::accessorType::signalAmplitudeType This Accessor's accessor type
           */
         virtual void enableAmplitude(uci::base::accessorType::AccessorType type = uci::type::accessorType::signalAmplitudeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Amplitude */
         virtual void clearAmplitude()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Polarization.
           *
           * @return The value of the enumeration identified by Polarization.
           */
         virtual const uci::type::PolarizationEnum& getPolarization() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Polarization.
           *
           * @return The value of the enumeration identified by Polarization.
           */
         virtual uci::type::PolarizationEnum& getPolarization()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Polarization.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPolarization(const uci::type::PolarizationEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Polarization.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPolarization(uci::type::PolarizationEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Polarizationis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasPolarization() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getPolarization will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::polarizationEnum This Accessor's accessor type
           */
         virtual void enablePolarization(uci::base::accessorType::AccessorType type = uci::type::accessorType::polarizationEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearPolarization()
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


         /** Returns the accessor that provides access to the complex content that is identified by the Scan.
           *
           * @return The acecssor that provides access to the complex content that is identified by Scan.
           */
         virtual const uci::type::ScanDataType& getScan() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Scan.
           *
           * @return The acecssor that provides access to the complex content that is identified by Scan.
           */
         virtual uci::type::ScanDataType& getScan()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Scan to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Scan
           */
         virtual void setScan(const uci::type::ScanDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Scan exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Scan is emabled or not
           */
         virtual bool hasScan() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Scan
           *
           * @param type = uci::type::accessorType::scanDataType This Accessor's accessor type
           */
         virtual void enableScan(uci::base::accessorType::AccessorType type = uci::type::accessorType::scanDataType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Scan */
         virtual void clearScan()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EmitterFunction.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmitterFunction.
           */
         virtual const uci::type::EmitterFunctionType& getEmitterFunction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EmitterFunction.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmitterFunction.
           */
         virtual uci::type::EmitterFunctionType& getEmitterFunction()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EmitterFunction to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EmitterFunction
           */
         virtual void setEmitterFunction(const uci::type::EmitterFunctionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EmitterFunction exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EmitterFunction is emabled or not
           */
         virtual bool hasEmitterFunction() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EmitterFunction
           *
           * @param type = uci::type::accessorType::emitterFunctionType This Accessor's accessor type
           */
         virtual void enableEmitterFunction(uci::base::accessorType::AccessorType type = uci::type::accessorType::emitterFunctionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EmitterFunction */
         virtual void clearEmitterFunction()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EmitterIdentity.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmitterIdentity.
           */
         virtual const uci::type::SignalIdentityType& getEmitterIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EmitterIdentity.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmitterIdentity.
           */
         virtual uci::type::SignalIdentityType& getEmitterIdentity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EmitterIdentity to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EmitterIdentity
           */
         virtual void setEmitterIdentity(const uci::type::SignalIdentityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EmitterIdentity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EmitterIdentity is emabled or not
           */
         virtual bool hasEmitterIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EmitterIdentity
           *
           * @param type = uci::type::accessorType::signalIdentityType This Accessor's accessor type
           */
         virtual void enableEmitterIdentity(uci::base::accessorType::AccessorType type = uci::type::accessorType::signalIdentityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EmitterIdentity */
         virtual void clearEmitterIdentity()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EmitterKinematics.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmitterKinematics.
           */
         virtual const uci::type::SignalEmitterKinematicsType& getEmitterKinematics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EmitterKinematics.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmitterKinematics.
           */
         virtual uci::type::SignalEmitterKinematicsType& getEmitterKinematics()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EmitterKinematics to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EmitterKinematics
           */
         virtual void setEmitterKinematics(const uci::type::SignalEmitterKinematicsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EmitterKinematics exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EmitterKinematics is emabled or not
           */
         virtual bool hasEmitterKinematics() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EmitterKinematics
           *
           * @param type = uci::type::accessorType::signalEmitterKinematicsType This Accessor's accessor type
           */
         virtual void enableEmitterKinematics(uci::base::accessorType::AccessorType type = uci::type::accessorType::signalEmitterKinematicsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EmitterKinematics */
         virtual void clearEmitterKinematics()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CollectorKinematics.
           *
           * @return The bounded list identified by CollectorKinematics.
           */
         virtual const uci::type::SignalType::CollectorKinematics& getCollectorKinematics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CollectorKinematics.
           *
           * @return The bounded list identified by CollectorKinematics.
           */
         virtual uci::type::SignalType::CollectorKinematics& getCollectorKinematics()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CollectorKinematics.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCollectorKinematics(const uci::type::SignalType::CollectorKinematics& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the WartimeReserveMode.
           *
           * @return The value of the string data type identified by WartimeReserveMode.
           */
         virtual const uci::type::EmptyType& getWartimeReserveMode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the WartimeReserveMode.
           *
           * @return The value of the string data type identified by WartimeReserveMode.
           */
         virtual uci::type::EmptyType& getWartimeReserveMode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the WartimeReserveMode to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setWartimeReserveMode(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the WartimeReserveMode to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setWartimeReserveMode(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the WartimeReserveMode to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setWartimeReserveMode(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by WartimeReserveMode exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by WartimeReserveMode is emabled or not
           */
         virtual bool hasWartimeReserveMode() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by WartimeReserveMode
           *
           * @param type = uci::type::accessorType::emptyType This Accessor's accessor type
           */
         virtual void enableWartimeReserveMode(uci::base::accessorType::AccessorType type = uci::type::accessorType::emptyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by WartimeReserveMode */
         virtual void clearWartimeReserveMode()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LockIndicator.
           *
           * @return The value of the enumeration identified by LockIndicator.
           */
         virtual const uci::type::LockIndicatorEnum& getLockIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LockIndicator.
           *
           * @return The value of the enumeration identified by LockIndicator.
           */
         virtual uci::type::LockIndicatorEnum& getLockIndicator()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LockIndicator.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLockIndicator(const uci::type::LockIndicatorEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LockIndicator.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLockIndicator(uci::type::LockIndicatorEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield LockIndicatoris available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasLockIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getLockIndicator will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::lockIndicatorEnum This Accessor's accessor type
           */
         virtual void enableLockIndicator(uci::base::accessorType::AccessorType type = uci::type::accessorType::lockIndicatorEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearLockIndicator()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActivityBy.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActivityBy.
           */
         virtual const uci::type::ActivityByType& getActivityBy() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActivityBy.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActivityBy.
           */
         virtual uci::type::ActivityByType& getActivityBy()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ActivityBy to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ActivityBy
           */
         virtual void setActivityBy(const uci::type::ActivityByType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ActivityBy exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ActivityBy is emabled or not
           */
         virtual bool hasActivityBy() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ActivityBy
           *
           * @param type = uci::type::accessorType::activityByType This Accessor's accessor type
           */
         virtual void enableActivityBy(uci::base::accessorType::AccessorType type = uci::type::accessorType::activityByType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ActivityBy */
         virtual void clearActivityBy()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RemoveInfo.
           *
           * @return The acecssor that provides access to the complex content that is identified by RemoveInfo.
           */
         virtual const uci::type::SignalRemoveInfoType& getRemoveInfo() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RemoveInfo.
           *
           * @return The acecssor that provides access to the complex content that is identified by RemoveInfo.
           */
         virtual uci::type::SignalRemoveInfoType& getRemoveInfo()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RemoveInfo to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RemoveInfo
           */
         virtual void setRemoveInfo(const uci::type::SignalRemoveInfoType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RemoveInfo exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RemoveInfo is emabled or not
           */
         virtual bool hasRemoveInfo() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RemoveInfo
           *
           * @param type = uci::type::accessorType::signalRemoveInfoType This Accessor's accessor type
           */
         virtual void enableRemoveInfo(uci::base::accessorType::AccessorType type = uci::type::accessorType::signalRemoveInfoType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RemoveInfo */
         virtual void clearRemoveInfo()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Modulation.
           *
           * @return The value of the enumeration identified by Modulation.
           */
         virtual const uci::type::ModulationEnum& getModulation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Modulation.
           *
           * @return The value of the enumeration identified by Modulation.
           */
         virtual uci::type::ModulationEnum& getModulation()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Modulation.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setModulation(const uci::type::ModulationEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Modulation.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setModulation(uci::type::ModulationEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Modulationis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasModulation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getModulation will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::modulationEnum This Accessor's accessor type
           */
         virtual void enableModulation(uci::base::accessorType::AccessorType type = uci::type::accessorType::modulationEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearModulation()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ModulationParameter.
           *
           * @return The bounded list identified by ModulationParameter.
           */
         virtual const uci::type::SignalType::ModulationParameter& getModulationParameter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ModulationParameter.
           *
           * @return The bounded list identified by ModulationParameter.
           */
         virtual uci::type::SignalType::ModulationParameter& getModulationParameter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ModulationParameter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setModulationParameter(const uci::type::SignalType::ModulationParameter& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SignalType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SignalType to copy from
           */
         SignalType(const SignalType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SignalType to the contents of the SignalType on the right hand
           * side (rhs) of the assignment operator.SignalType [only available to derived classes]
           *
           * @param rhs The SignalType on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::SignalType
           * @return A constant reference to this SignalType.
           */
         const SignalType& operator=(const SignalType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SignalType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SignalType_h

