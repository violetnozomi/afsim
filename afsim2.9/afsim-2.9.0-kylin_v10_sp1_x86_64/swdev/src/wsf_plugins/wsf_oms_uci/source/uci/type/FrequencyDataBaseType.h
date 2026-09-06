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
#ifndef Uci__Type__FrequencyDataBaseType_h
#define Uci__Type__FrequencyDataBaseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__FrequencyType_h)
# include "uci/type/FrequencyType.h"
#endif

#if !defined(Uci__Type__DeviationType_h)
# include "uci/type/DeviationType.h"
#endif

#if !defined(Uci__Type__FrequencyModulationVariationEnum_h)
# include "uci/type/FrequencyModulationVariationEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the FrequencyDataBaseType sequence accessor class */
      class FrequencyDataBaseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~FrequencyDataBaseType()
         { }

         /** Returns this accessor's type constant, i.e. FrequencyDataBaseType
           *
           * @return This accessor's type constant, i.e. FrequencyDataBaseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::frequencyDataBaseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const FrequencyDataBaseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FrequencyAverage.
           *
           * @return The value of the simple primitive data type identified by FrequencyAverage.
           */
         virtual uci::type::FrequencyTypeValue getFrequencyAverage() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FrequencyAverage.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFrequencyAverage(uci::type::FrequencyTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FrequencyAverage exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FrequencyAverage is emabled or not
           */
         virtual bool hasFrequencyAverage() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FrequencyAverage
           *
           * @param type = uci::type::accessorType::frequencyType This Accessor's accessor type
           */
         virtual void enableFrequencyAverage(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FrequencyAverage */
         virtual void clearFrequencyAverage()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FrequencyMin.
           *
           * @return The value of the simple primitive data type identified by FrequencyMin.
           */
         virtual uci::type::FrequencyTypeValue getFrequencyMin() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FrequencyMin.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFrequencyMin(uci::type::FrequencyTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FrequencyMin exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FrequencyMin is emabled or not
           */
         virtual bool hasFrequencyMin() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FrequencyMin
           *
           * @param type = uci::type::accessorType::frequencyType This Accessor's accessor type
           */
         virtual void enableFrequencyMin(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FrequencyMin */
         virtual void clearFrequencyMin()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FrequencyMax.
           *
           * @return The value of the simple primitive data type identified by FrequencyMax.
           */
         virtual uci::type::FrequencyTypeValue getFrequencyMax() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FrequencyMax.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFrequencyMax(uci::type::FrequencyTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FrequencyMax exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FrequencyMax is emabled or not
           */
         virtual bool hasFrequencyMax() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FrequencyMax
           *
           * @param type = uci::type::accessorType::frequencyType This Accessor's accessor type
           */
         virtual void enableFrequencyMax(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FrequencyMax */
         virtual void clearFrequencyMax()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FrequencyLast.
           *
           * @return The value of the simple primitive data type identified by FrequencyLast.
           */
         virtual uci::type::FrequencyTypeValue getFrequencyLast() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FrequencyLast.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFrequencyLast(uci::type::FrequencyTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FrequencyLast exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FrequencyLast is emabled or not
           */
         virtual bool hasFrequencyLast() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FrequencyLast
           *
           * @param type = uci::type::accessorType::frequencyType This Accessor's accessor type
           */
         virtual void enableFrequencyLast(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FrequencyLast */
         virtual void clearFrequencyLast()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FrequencyDeviation.
           *
           * @return The value of the simple primitive data type identified by FrequencyDeviation.
           */
         virtual uci::type::DeviationTypeValue getFrequencyDeviation() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FrequencyDeviation.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFrequencyDeviation(uci::type::DeviationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FrequencyDeviation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FrequencyDeviation is emabled or not
           */
         virtual bool hasFrequencyDeviation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FrequencyDeviation
           *
           * @param type = uci::type::accessorType::deviationType This Accessor's accessor type
           */
         virtual void enableFrequencyDeviation(uci::base::accessorType::AccessorType type = uci::type::accessorType::deviationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FrequencyDeviation */
         virtual void clearFrequencyDeviation()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FrequencyModulationVariation.
           *
           * @return The value of the enumeration identified by FrequencyModulationVariation.
           */
         virtual const uci::type::FrequencyModulationVariationEnum& getFrequencyModulationVariation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FrequencyModulationVariation.
           *
           * @return The value of the enumeration identified by FrequencyModulationVariation.
           */
         virtual uci::type::FrequencyModulationVariationEnum& getFrequencyModulationVariation()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FrequencyModulationVariation.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFrequencyModulationVariation(const uci::type::FrequencyModulationVariationEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FrequencyModulationVariation.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFrequencyModulationVariation(uci::type::FrequencyModulationVariationEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield FrequencyModulationVariationis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasFrequencyModulationVariation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getFrequencyModulationVariation will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::frequencyModulationVariationEnum This Accessor's accessor type
           */
         virtual void enableFrequencyModulationVariation(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyModulationVariationEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearFrequencyModulationVariation()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         FrequencyDataBaseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The FrequencyDataBaseType to copy from
           */
         FrequencyDataBaseType(const FrequencyDataBaseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this FrequencyDataBaseType to the contents of the FrequencyDataBaseType
           * on the right hand side (rhs) of the assignment operator.FrequencyDataBaseType [only available to derived classes]
           *
           * @param rhs The FrequencyDataBaseType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::FrequencyDataBaseType
           * @return A constant reference to this FrequencyDataBaseType.
           */
         const FrequencyDataBaseType& operator=(const FrequencyDataBaseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // FrequencyDataBaseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__FrequencyDataBaseType_h

