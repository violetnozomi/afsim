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
#ifndef Uci__Type__PulseDataCollectCommandType_h
#define Uci__Type__PulseDataCollectCommandType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CollectionFrequencyType_h)
# include "uci/type/CollectionFrequencyType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Type__ModulationEnum_h)
# include "uci/type/ModulationEnum.h"
#endif

#if !defined(Uci__Type__SignalTypeEnum_h)
# include "uci/type/SignalTypeEnum.h"
#endif

#if !defined(Uci__Base__FloatAccessor_h)
# include "uci/base/FloatAccessor.h"
#endif

#if !defined(Uci__Base__IntAccessor_h)
# include "uci/base/IntAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PulseDataCollectCommandType sequence accessor class */
      class PulseDataCollectCommandType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PulseDataCollectCommandType()
         { }

         /** Returns this accessor's type constant, i.e. PulseDataCollectCommandType
           *
           * @return This accessor's type constant, i.e. PulseDataCollectCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pulseDataCollectCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PulseDataCollectCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionFrequency.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionFrequency.
           */
         virtual const uci::type::CollectionFrequencyType& getCollectionFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionFrequency.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionFrequency.
           */
         virtual uci::type::CollectionFrequencyType& getCollectionFrequency()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CollectionFrequency to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CollectionFrequency
           */
         virtual void setCollectionFrequency(const uci::type::CollectionFrequencyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the StartTime.
           *
           * @return The value of the simple primitive data type identified by StartTime.
           */
         virtual uci::type::DateTimeTypeValue getStartTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the StartTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setStartTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DwellDuration.
           *
           * @return The value of the simple primitive data type identified by DwellDuration.
           */
         virtual uci::type::DurationTypeValue getDwellDuration() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DwellDuration.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDwellDuration(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Dwells.
           *
           * @return The value of the simple primitive data type identified by Dwells.
           */
         virtual xs::UnsignedInt getDwells() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Dwells.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDwells(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Dwells exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Dwells is emabled or not
           */
         virtual bool hasDwells() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Dwells
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableDwells(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Dwells */
         virtual void clearDwells()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RepetitionInterval.
           *
           * @return The value of the simple primitive data type identified by RepetitionInterval.
           */
         virtual uci::type::DurationTypeValue getRepetitionInterval() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RepetitionInterval.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRepetitionInterval(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RepetitionInterval exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RepetitionInterval is emabled or not
           */
         virtual bool hasRepetitionInterval() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RepetitionInterval
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableRepetitionInterval(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RepetitionInterval */
         virtual void clearRepetitionInterval()
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


         /** Returns the value of the SimplePrimitive data type that is identified by the PRI.
           *
           * @return The value of the simple primitive data type identified by PRI.
           */
         virtual uci::type::DurationTypeValue getPRI() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PRI.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPRI(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PRI exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PRI is emabled or not
           */
         virtual bool hasPRI() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PRI
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enablePRI(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PRI */
         virtual void clearPRI()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SignalBandwidth.
           *
           * @return The acecssor that provides access to the complex content that is identified by SignalBandwidth.
           */
         virtual const uci::type::FrequencyRangeType& getSignalBandwidth() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SignalBandwidth.
           *
           * @return The acecssor that provides access to the complex content that is identified by SignalBandwidth.
           */
         virtual uci::type::FrequencyRangeType& getSignalBandwidth()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SignalBandwidth to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SignalBandwidth
           */
         virtual void setSignalBandwidth(const uci::type::FrequencyRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SignalBandwidth exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SignalBandwidth is emabled or not
           */
         virtual bool hasSignalBandwidth() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SignalBandwidth
           *
           * @param type = uci::type::accessorType::frequencyRangeType This Accessor's accessor type
           */
         virtual void enableSignalBandwidth(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SignalBandwidth */
         virtual void clearSignalBandwidth()
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


         /** Returns the value of the enumeration that is identified by the DataCollectType.
           *
           * @return The value of the enumeration identified by DataCollectType.
           */
         virtual const uci::type::SignalTypeEnum& getDataCollectType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the DataCollectType.
           *
           * @return The value of the enumeration identified by DataCollectType.
           */
         virtual uci::type::SignalTypeEnum& getDataCollectType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the DataCollectType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDataCollectType(const uci::type::SignalTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the DataCollectType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDataCollectType(uci::type::SignalTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield DataCollectTypeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasDataCollectType() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getDataCollectType will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::signalTypeEnum This Accessor's accessor type
           */
         virtual void enableDataCollectType(uci::base::accessorType::AccessorType type = uci::type::accessorType::signalTypeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearDataCollectType()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SampleRate.
           *
           * @return The value of the simple primitive data type identified by SampleRate.
           */
         virtual xs::Float getSampleRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SampleRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSampleRate(xs::Float value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SampleRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SampleRate is emabled or not
           */
         virtual bool hasSampleRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SampleRate
           *
           * @param type = uci::base::accessorType::floatAccessor This Accessor's accessor type
           */
         virtual void enableSampleRate(uci::base::accessorType::AccessorType type = uci::base::accessorType::floatAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SampleRate */
         virtual void clearSampleRate()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FFT_PointsPerSample.
           *
           * @return The value of the simple primitive data type identified by FFT_PointsPerSample.
           */
         virtual xs::Int getFFT_PointsPerSample() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FFT_PointsPerSample.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFFT_PointsPerSample(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FFT_PointsPerSample exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FFT_PointsPerSample is emabled or not
           */
         virtual bool hasFFT_PointsPerSample() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FFT_PointsPerSample
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableFFT_PointsPerSample(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FFT_PointsPerSample */
         virtual void clearFFT_PointsPerSample()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PulseDataCollectCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PulseDataCollectCommandType to copy from
           */
         PulseDataCollectCommandType(const PulseDataCollectCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PulseDataCollectCommandType to the contents of the
           * PulseDataCollectCommandType on the right hand side (rhs) of the assignment operator.PulseDataCollectCommandType [only
           * available to derived classes]
           *
           * @param rhs The PulseDataCollectCommandType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::PulseDataCollectCommandType
           * @return A constant reference to this PulseDataCollectCommandType.
           */
         const PulseDataCollectCommandType& operator=(const PulseDataCollectCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PulseDataCollectCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PulseDataCollectCommandType_h

