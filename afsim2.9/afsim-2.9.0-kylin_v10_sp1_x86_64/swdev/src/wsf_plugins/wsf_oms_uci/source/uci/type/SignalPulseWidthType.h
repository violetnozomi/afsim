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
#ifndef Uci__Type__SignalPulseWidthType_h
#define Uci__Type__SignalPulseWidthType_h 1

#if !defined(Uci__Type__SignalPulseWidthBaseType_h)
# include "uci/type/SignalPulseWidthBaseType.h"
#endif

#if !defined(Uci__Type__MeasurementQualityEnum_h)
# include "uci/type/MeasurementQualityEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SignalPulseWidthType sequence accessor class */
      class SignalPulseWidthType : public virtual uci::type::SignalPulseWidthBaseType {
      public:

         /** The destructor */
         virtual ~SignalPulseWidthType()
         { }

         /** Returns this accessor's type constant, i.e. SignalPulseWidthType
           *
           * @return This accessor's type constant, i.e. SignalPulseWidthType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::signalPulseWidthType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SignalPulseWidthType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PulseWidthQuality.
           *
           * @return The value of the enumeration identified by PulseWidthQuality.
           */
         virtual const uci::type::MeasurementQualityEnum& getPulseWidthQuality() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PulseWidthQuality.
           *
           * @return The value of the enumeration identified by PulseWidthQuality.
           */
         virtual uci::type::MeasurementQualityEnum& getPulseWidthQuality()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PulseWidthQuality.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPulseWidthQuality(const uci::type::MeasurementQualityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PulseWidthQuality.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPulseWidthQuality(uci::type::MeasurementQualityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield PulseWidthQualityis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasPulseWidthQuality() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getPulseWidthQuality will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::measurementQualityEnum This Accessor's accessor type
           */
         virtual void enablePulseWidthQuality(uci::base::accessorType::AccessorType type = uci::type::accessorType::measurementQualityEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearPulseWidthQuality()
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



      protected:

         /** The constructor [only available to derived classes]. */
         SignalPulseWidthType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SignalPulseWidthType to copy from
           */
         SignalPulseWidthType(const SignalPulseWidthType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SignalPulseWidthType to the contents of the SignalPulseWidthType
           * on the right hand side (rhs) of the assignment operator.SignalPulseWidthType [only available to derived classes]
           *
           * @param rhs The SignalPulseWidthType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::SignalPulseWidthType
           * @return A constant reference to this SignalPulseWidthType.
           */
         const SignalPulseWidthType& operator=(const SignalPulseWidthType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SignalPulseWidthType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SignalPulseWidthType_h

