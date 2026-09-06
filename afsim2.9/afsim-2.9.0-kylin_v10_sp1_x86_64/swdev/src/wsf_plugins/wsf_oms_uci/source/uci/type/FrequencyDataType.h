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
#ifndef Uci__Type__FrequencyDataType_h
#define Uci__Type__FrequencyDataType_h 1

#if !defined(Uci__Type__FrequencyDataBaseType_h)
# include "uci/type/FrequencyDataBaseType.h"
#endif

#if !defined(Uci__Type__FrequencyAgilityPatternEnum_h)
# include "uci/type/FrequencyAgilityPatternEnum.h"
#endif

#if !defined(Uci__Type__FrequencySwitchingEnum_h)
# include "uci/type/FrequencySwitchingEnum.h"
#endif

#if !defined(Uci__Type__FrequencyPatternPeriodicityEnum_h)
# include "uci/type/FrequencyPatternPeriodicityEnum.h"
#endif

#if !defined(Uci__Type__EmptyType_h)
# include "uci/type/EmptyType.h"
#endif

#if !defined(Uci__Type__FrequencySwitchingPatternEnum_h)
# include "uci/type/FrequencySwitchingPatternEnum.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the FrequencyDataType sequence accessor class */
      class FrequencyDataType : public virtual uci::type::FrequencyDataBaseType {
      public:

         /** The destructor */
         virtual ~FrequencyDataType()
         { }

         /** Returns this accessor's type constant, i.e. FrequencyDataType
           *
           * @return This accessor's type constant, i.e. FrequencyDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::frequencyDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const FrequencyDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AgilityPattern.
           *
           * @return The value of the enumeration identified by AgilityPattern.
           */
         virtual const uci::type::FrequencyAgilityPatternEnum& getAgilityPattern() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AgilityPattern.
           *
           * @return The value of the enumeration identified by AgilityPattern.
           */
         virtual uci::type::FrequencyAgilityPatternEnum& getAgilityPattern()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AgilityPattern.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAgilityPattern(const uci::type::FrequencyAgilityPatternEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AgilityPattern.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAgilityPattern(uci::type::FrequencyAgilityPatternEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield AgilityPatternis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasAgilityPattern() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getAgilityPattern will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::frequencyAgilityPatternEnum This Accessor's accessor type
           */
         virtual void enableAgilityPattern(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyAgilityPatternEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearAgilityPattern()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AgilityValues.
           *
           * @return The value of the enumeration identified by AgilityValues.
           */
         virtual const uci::type::FrequencySwitchingEnum& getAgilityValues() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AgilityValues.
           *
           * @return The value of the enumeration identified by AgilityValues.
           */
         virtual uci::type::FrequencySwitchingEnum& getAgilityValues()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AgilityValues.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAgilityValues(const uci::type::FrequencySwitchingEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AgilityValues.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAgilityValues(uci::type::FrequencySwitchingEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield AgilityValuesis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasAgilityValues() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getAgilityValues will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::frequencySwitchingEnum This Accessor's accessor type
           */
         virtual void enableAgilityValues(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencySwitchingEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearAgilityValues()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AgilityPeriodicity.
           *
           * @return The value of the enumeration identified by AgilityPeriodicity.
           */
         virtual const uci::type::FrequencyPatternPeriodicityEnum& getAgilityPeriodicity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AgilityPeriodicity.
           *
           * @return The value of the enumeration identified by AgilityPeriodicity.
           */
         virtual uci::type::FrequencyPatternPeriodicityEnum& getAgilityPeriodicity()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AgilityPeriodicity.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAgilityPeriodicity(const uci::type::FrequencyPatternPeriodicityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AgilityPeriodicity.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAgilityPeriodicity(uci::type::FrequencyPatternPeriodicityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield AgilityPeriodicityis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasAgilityPeriodicity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getAgilityPeriodicity will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::frequencyPatternPeriodicityEnum This Accessor's accessor type
           */
         virtual void enableAgilityPeriodicity(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyPatternPeriodicityEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearAgilityPeriodicity()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the AgileEmitter.
           *
           * @return The value of the string data type identified by AgileEmitter.
           */
         virtual const uci::type::EmptyType& getAgileEmitter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the AgileEmitter.
           *
           * @return The value of the string data type identified by AgileEmitter.
           */
         virtual uci::type::EmptyType& getAgileEmitter()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the AgileEmitter to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setAgileEmitter(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the AgileEmitter to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setAgileEmitter(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the AgileEmitter to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setAgileEmitter(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AgileEmitter exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AgileEmitter is emabled or not
           */
         virtual bool hasAgileEmitter() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AgileEmitter
           *
           * @param type = uci::type::accessorType::emptyType This Accessor's accessor type
           */
         virtual void enableAgileEmitter(uci::base::accessorType::AccessorType type = uci::type::accessorType::emptyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AgileEmitter */
         virtual void clearAgileEmitter()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SwitchingValues.
           *
           * @return The value of the enumeration identified by SwitchingValues.
           */
         virtual const uci::type::FrequencySwitchingPatternEnum& getSwitchingValues() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SwitchingValues.
           *
           * @return The value of the enumeration identified by SwitchingValues.
           */
         virtual uci::type::FrequencySwitchingPatternEnum& getSwitchingValues()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SwitchingValues.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSwitchingValues(const uci::type::FrequencySwitchingPatternEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SwitchingValues.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSwitchingValues(uci::type::FrequencySwitchingPatternEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield SwitchingValuesis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasSwitchingValues() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getSwitchingValues will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::frequencySwitchingPatternEnum This Accessor's accessor type
           */
         virtual void enableSwitchingValues(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencySwitchingPatternEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearSwitchingValues()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ModulationPatternPeriod.
           *
           * @return The value of the simple primitive data type identified by ModulationPatternPeriod.
           */
         virtual uci::type::DurationTypeValue getModulationPatternPeriod() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ModulationPatternPeriod.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setModulationPatternPeriod(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ModulationPatternPeriod exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ModulationPatternPeriod is emabled or not
           */
         virtual bool hasModulationPatternPeriod() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ModulationPatternPeriod
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableModulationPatternPeriod(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ModulationPatternPeriod */
         virtual void clearModulationPatternPeriod()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FrequencyBins.
           *
           * @return The value of the simple primitive data type identified by FrequencyBins.
           */
         virtual xs::UnsignedInt getFrequencyBins() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FrequencyBins.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFrequencyBins(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FrequencyBins exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FrequencyBins is emabled or not
           */
         virtual bool hasFrequencyBins() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FrequencyBins
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableFrequencyBins(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FrequencyBins */
         virtual void clearFrequencyBins()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         FrequencyDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The FrequencyDataType to copy from
           */
         FrequencyDataType(const FrequencyDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this FrequencyDataType to the contents of the FrequencyDataType on the
           * right hand side (rhs) of the assignment operator.FrequencyDataType [only available to derived classes]
           *
           * @param rhs The FrequencyDataType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::FrequencyDataType
           * @return A constant reference to this FrequencyDataType.
           */
         const FrequencyDataType& operator=(const FrequencyDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // FrequencyDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__FrequencyDataType_h

