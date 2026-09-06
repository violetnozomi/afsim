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
#ifndef Uci__Type__COMINT_MeasurementResultsType_h
#define Uci__Type__COMINT_MeasurementResultsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__FrequencyDataType_h)
# include "uci/type/FrequencyDataType.h"
#endif

#if !defined(Uci__Type__ModulationEnum_h)
# include "uci/type/ModulationEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the COMINT_MeasurementResultsType sequence accessor class */
      class COMINT_MeasurementResultsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~COMINT_MeasurementResultsType()
         { }

         /** Returns this accessor's type constant, i.e. COMINT_MeasurementResultsType
           *
           * @return This accessor's type constant, i.e. COMINT_MeasurementResultsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::cOMINT_MeasurementResultsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const COMINT_MeasurementResultsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Frequency.
           *
           * @return The acecssor that provides access to the complex content that is identified by Frequency.
           */
         virtual const uci::type::FrequencyDataType& getFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Frequency.
           *
           * @return The acecssor that provides access to the complex content that is identified by Frequency.
           */
         virtual uci::type::FrequencyDataType& getFrequency()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Frequency to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Frequency
           */
         virtual void setFrequency(const uci::type::FrequencyDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Frequency exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Frequency is emabled or not
           */
         virtual bool hasFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Frequency
           *
           * @param type = uci::type::accessorType::frequencyDataType This Accessor's accessor type
           */
         virtual void enableFrequency(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyDataType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Frequency */
         virtual void clearFrequency()
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



      protected:

         /** The constructor [only available to derived classes]. */
         COMINT_MeasurementResultsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The COMINT_MeasurementResultsType to copy from
           */
         COMINT_MeasurementResultsType(const COMINT_MeasurementResultsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this COMINT_MeasurementResultsType to the contents of the
           * COMINT_MeasurementResultsType on the right hand side (rhs) of the assignment operator.COMINT_MeasurementResultsType
           * [only available to derived classes]
           *
           * @param rhs The COMINT_MeasurementResultsType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::COMINT_MeasurementResultsType
           * @return A constant reference to this COMINT_MeasurementResultsType.
           */
         const COMINT_MeasurementResultsType& operator=(const COMINT_MeasurementResultsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // COMINT_MeasurementResultsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__COMINT_MeasurementResultsType_h

