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
#ifndef Uci__Type__PRI_DataType_h
#define Uci__Type__PRI_DataType_h 1

#if !defined(Uci__Type__PRI_DataBaseType_h)
# include "uci/type/PRI_DataBaseType.h"
#endif

#if !defined(Uci__Type__PRI_SwitchingPatternEnum_h)
# include "uci/type/PRI_SwitchingPatternEnum.h"
#endif

#if !defined(Uci__Type__PRI_AgilePatternEnum_h)
# include "uci/type/PRI_AgilePatternEnum.h"
#endif

#if !defined(Uci__Type__PRI_MeasurementEnum_h)
# include "uci/type/PRI_MeasurementEnum.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EmptyType_h)
# include "uci/type/EmptyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PRI_DataType sequence accessor class */
      class PRI_DataType : public virtual uci::type::PRI_DataBaseType {
      public:

         /** The destructor */
         virtual ~PRI_DataType()
         { }

         /** Returns this accessor's type constant, i.e. PRI_DataType
           *
           * @return This accessor's type constant, i.e. PRI_DataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pRI_DataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PRI_DataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a measured PRI pattern interval. The intervals are generally reported in the order they are measured, in
           * order to preserve the pattern sequence. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DurationType, uci::type::accessorType::durationType> PRI_PatternInterval;

         /** Returns the value of the enumeration that is identified by the PRI_SwitchingPattern.
           *
           * @return The value of the enumeration identified by PRI_SwitchingPattern.
           */
         virtual const uci::type::PRI_SwitchingPatternEnum& getPRI_SwitchingPattern() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PRI_SwitchingPattern.
           *
           * @return The value of the enumeration identified by PRI_SwitchingPattern.
           */
         virtual uci::type::PRI_SwitchingPatternEnum& getPRI_SwitchingPattern()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PRI_SwitchingPattern.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPRI_SwitchingPattern(const uci::type::PRI_SwitchingPatternEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PRI_SwitchingPattern.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPRI_SwitchingPattern(uci::type::PRI_SwitchingPatternEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield PRI_SwitchingPatternis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasPRI_SwitchingPattern() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getPRI_SwitchingPattern will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::pRI_SwitchingPatternEnum This Accessor's accessor type
           */
         virtual void enablePRI_SwitchingPattern(uci::base::accessorType::AccessorType type = uci::type::accessorType::pRI_SwitchingPatternEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearPRI_SwitchingPattern()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PRI_AgilePattern.
           *
           * @return The value of the enumeration identified by PRI_AgilePattern.
           */
         virtual const uci::type::PRI_AgilePatternEnum& getPRI_AgilePattern() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PRI_AgilePattern.
           *
           * @return The value of the enumeration identified by PRI_AgilePattern.
           */
         virtual uci::type::PRI_AgilePatternEnum& getPRI_AgilePattern()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PRI_AgilePattern.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPRI_AgilePattern(const uci::type::PRI_AgilePatternEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PRI_AgilePattern.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPRI_AgilePattern(uci::type::PRI_AgilePatternEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield PRI_AgilePatternis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasPRI_AgilePattern() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getPRI_AgilePattern will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::pRI_AgilePatternEnum This Accessor's accessor type
           */
         virtual void enablePRI_AgilePattern(uci::base::accessorType::AccessorType type = uci::type::accessorType::pRI_AgilePatternEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearPRI_AgilePattern()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PRI_Measurement.
           *
           * @return The value of the enumeration identified by PRI_Measurement.
           */
         virtual const uci::type::PRI_MeasurementEnum& getPRI_Measurement() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PRI_Measurement.
           *
           * @return The value of the enumeration identified by PRI_Measurement.
           */
         virtual uci::type::PRI_MeasurementEnum& getPRI_Measurement()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PRI_Measurement.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPRI_Measurement(const uci::type::PRI_MeasurementEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PRI_Measurement.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPRI_Measurement(uci::type::PRI_MeasurementEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield PRI_Measurementis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasPRI_Measurement() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getPRI_Measurement will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::pRI_MeasurementEnum This Accessor's accessor type
           */
         virtual void enablePRI_Measurement(uci::base::accessorType::AccessorType type = uci::type::accessorType::pRI_MeasurementEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearPRI_Measurement()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PRI_JitterDeviation.
           *
           * @return The value of the simple primitive data type identified by PRI_JitterDeviation.
           */
         virtual uci::type::DurationTypeValue getPRI_JitterDeviation() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PRI_JitterDeviation.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPRI_JitterDeviation(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PRI_JitterDeviation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PRI_JitterDeviation is emabled or not
           */
         virtual bool hasPRI_JitterDeviation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PRI_JitterDeviation
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enablePRI_JitterDeviation(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PRI_JitterDeviation */
         virtual void clearPRI_JitterDeviation()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PRI_PulseGroupsPresent.
           *
           * @return The value of the simple primitive data type identified by PRI_PulseGroupsPresent.
           */
         virtual xs::Boolean getPRI_PulseGroupsPresent() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PRI_PulseGroupsPresent.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPRI_PulseGroupsPresent(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PRI_PulseGroupsPresent exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PRI_PulseGroupsPresent is emabled or not
           */
         virtual bool hasPRI_PulseGroupsPresent() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PRI_PulseGroupsPresent
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enablePRI_PulseGroupsPresent(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PRI_PulseGroupsPresent */
         virtual void clearPRI_PulseGroupsPresent()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PRI_Quantized.
           *
           * @return The value of the simple primitive data type identified by PRI_Quantized.
           */
         virtual xs::Boolean getPRI_Quantized() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PRI_Quantized.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPRI_Quantized(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PRI_Quantized exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PRI_Quantized is emabled or not
           */
         virtual bool hasPRI_Quantized() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PRI_Quantized
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enablePRI_Quantized(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PRI_Quantized */
         virtual void clearPRI_Quantized()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PRI_QuantizationPeriod.
           *
           * @return The value of the simple primitive data type identified by PRI_QuantizationPeriod.
           */
         virtual uci::type::DurationTypeValue getPRI_QuantizationPeriod() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PRI_QuantizationPeriod.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPRI_QuantizationPeriod(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PRI_QuantizationPeriod exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PRI_QuantizationPeriod is emabled or not
           */
         virtual bool hasPRI_QuantizationPeriod() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PRI_QuantizationPeriod
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enablePRI_QuantizationPeriod(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PRI_QuantizationPeriod */
         virtual void clearPRI_QuantizationPeriod()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PRI_Squirrelly.
           *
           * @return The value of the simple primitive data type identified by PRI_Squirrelly.
           */
         virtual xs::Boolean getPRI_Squirrelly() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PRI_Squirrelly.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPRI_Squirrelly(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PRI_Squirrelly exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PRI_Squirrelly is emabled or not
           */
         virtual bool hasPRI_Squirrelly() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PRI_Squirrelly
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enablePRI_Squirrelly(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PRI_Squirrelly */
         virtual void clearPRI_Squirrelly()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PRI_MissingPulse.
           *
           * @return The value of the simple primitive data type identified by PRI_MissingPulse.
           */
         virtual xs::Boolean getPRI_MissingPulse() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PRI_MissingPulse.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPRI_MissingPulse(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PRI_MissingPulse exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PRI_MissingPulse is emabled or not
           */
         virtual bool hasPRI_MissingPulse() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PRI_MissingPulse
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enablePRI_MissingPulse(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PRI_MissingPulse */
         virtual void clearPRI_MissingPulse()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PRI_PatternPeriod.
           *
           * @return The value of the simple primitive data type identified by PRI_PatternPeriod.
           */
         virtual uci::type::DurationTypeValue getPRI_PatternPeriod() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PRI_PatternPeriod.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPRI_PatternPeriod(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PRI_PatternPeriod exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PRI_PatternPeriod is emabled or not
           */
         virtual bool hasPRI_PatternPeriod() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PRI_PatternPeriod
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enablePRI_PatternPeriod(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PRI_PatternPeriod */
         virtual void clearPRI_PatternPeriod()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PRI_PatternLevels.
           *
           * @return The value of the simple primitive data type identified by PRI_PatternLevels.
           */
         virtual xs::UnsignedInt getPRI_PatternLevels() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PRI_PatternLevels.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPRI_PatternLevels(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PRI_PatternLevels exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PRI_PatternLevels is emabled or not
           */
         virtual bool hasPRI_PatternLevels() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PRI_PatternLevels
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enablePRI_PatternLevels(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PRI_PatternLevels */
         virtual void clearPRI_PatternLevels()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PRI_PatternInterval.
           *
           * @return The bounded list identified by PRI_PatternInterval.
           */
         virtual const uci::type::PRI_DataType::PRI_PatternInterval& getPRI_PatternInterval() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the PRI_PatternInterval.
           *
           * @return The bounded list identified by PRI_PatternInterval.
           */
         virtual uci::type::PRI_DataType::PRI_PatternInterval& getPRI_PatternInterval()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the PRI_PatternInterval.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPRI_PatternInterval(const uci::type::PRI_DataType::PRI_PatternInterval& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the PRI_JitterDetected.
           *
           * @return The value of the string data type identified by PRI_JitterDetected.
           */
         virtual const uci::type::EmptyType& getPRI_JitterDetected() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the PRI_JitterDetected.
           *
           * @return The value of the string data type identified by PRI_JitterDetected.
           */
         virtual uci::type::EmptyType& getPRI_JitterDetected()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PRI_JitterDetected to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setPRI_JitterDetected(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PRI_JitterDetected to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setPRI_JitterDetected(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PRI_JitterDetected to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setPRI_JitterDetected(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PRI_JitterDetected exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PRI_JitterDetected is emabled or not
           */
         virtual bool hasPRI_JitterDetected() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PRI_JitterDetected
           *
           * @param type = uci::type::accessorType::emptyType This Accessor's accessor type
           */
         virtual void enablePRI_JitterDetected(uci::base::accessorType::AccessorType type = uci::type::accessorType::emptyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PRI_JitterDetected */
         virtual void clearPRI_JitterDetected()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PRI_DataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PRI_DataType to copy from
           */
         PRI_DataType(const PRI_DataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PRI_DataType to the contents of the PRI_DataType on the right hand
           * side (rhs) of the assignment operator.PRI_DataType [only available to derived classes]
           *
           * @param rhs The PRI_DataType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::PRI_DataType
           * @return A constant reference to this PRI_DataType.
           */
         const PRI_DataType& operator=(const PRI_DataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PRI_DataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PRI_DataType_h

