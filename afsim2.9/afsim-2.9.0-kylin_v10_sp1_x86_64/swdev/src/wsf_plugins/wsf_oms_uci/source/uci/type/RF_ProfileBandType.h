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
#ifndef Uci__Type__RF_ProfileBandType_h
#define Uci__Type__RF_ProfileBandType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__MilliwattPowerRatioType_h)
# include "uci/type/MilliwattPowerRatioType.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RF_ProfileBandType sequence accessor class */
      class RF_ProfileBandType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RF_ProfileBandType()
         { }

         /** Returns this accessor's type constant, i.e. RF_ProfileBandType
           *
           * @return This accessor's type constant, i.e. RF_ProfileBandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::rF_ProfileBandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RF_ProfileBandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Frequency.
           *
           * @return The acecssor that provides access to the complex content that is identified by Frequency.
           */
         virtual const uci::type::FrequencyRangeType& getFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Frequency.
           *
           * @return The acecssor that provides access to the complex content that is identified by Frequency.
           */
         virtual uci::type::FrequencyRangeType& getFrequency()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Frequency to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Frequency
           */
         virtual void setFrequency(const uci::type::FrequencyRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ReceiveAllowed.
           *
           * @return The value of the simple primitive data type identified by ReceiveAllowed.
           */
         virtual xs::Boolean getReceiveAllowed() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ReceiveAllowed.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setReceiveAllowed(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TransmitAllowed.
           *
           * @return The value of the simple primitive data type identified by TransmitAllowed.
           */
         virtual xs::Boolean getTransmitAllowed() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TransmitAllowed.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTransmitAllowed(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MaximumTransmitPower.
           *
           * @return The value of the simple primitive data type identified by MaximumTransmitPower.
           */
         virtual uci::type::MilliwattPowerRatioTypeValue getMaximumTransmitPower() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MaximumTransmitPower.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMaximumTransmitPower(uci::type::MilliwattPowerRatioTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MaximumTransmitPower exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MaximumTransmitPower is emabled or not
           */
         virtual bool hasMaximumTransmitPower() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MaximumTransmitPower
           *
           * @param type = uci::type::accessorType::milliwattPowerRatioType This Accessor's accessor type
           */
         virtual void enableMaximumTransmitPower(uci::base::accessorType::AccessorType type = uci::type::accessorType::milliwattPowerRatioType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MaximumTransmitPower */
         virtual void clearMaximumTransmitPower()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DutyCycle.
           *
           * @return The value of the simple primitive data type identified by DutyCycle.
           */
         virtual uci::type::PercentTypeValue getDutyCycle() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DutyCycle.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDutyCycle(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DutyCycle exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DutyCycle is emabled or not
           */
         virtual bool hasDutyCycle() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DutyCycle
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableDutyCycle(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DutyCycle */
         virtual void clearDutyCycle()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RF_ProfileBandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RF_ProfileBandType to copy from
           */
         RF_ProfileBandType(const RF_ProfileBandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RF_ProfileBandType to the contents of the RF_ProfileBandType on
           * the right hand side (rhs) of the assignment operator.RF_ProfileBandType [only available to derived classes]
           *
           * @param rhs The RF_ProfileBandType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::RF_ProfileBandType
           * @return A constant reference to this RF_ProfileBandType.
           */
         const RF_ProfileBandType& operator=(const RF_ProfileBandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RF_ProfileBandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RF_ProfileBandType_h

