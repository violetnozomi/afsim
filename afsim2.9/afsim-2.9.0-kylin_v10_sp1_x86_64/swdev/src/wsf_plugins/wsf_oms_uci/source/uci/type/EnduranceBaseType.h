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
#ifndef Uci__Type__EnduranceBaseType_h
#define Uci__Type__EnduranceBaseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MassType_h)
# include "uci/type/MassType.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates endurance in terms of maximum distance that can be reached given the current system state. */
      class EnduranceBaseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EnduranceBaseType()
         { }

         /** Returns this accessor's type constant, i.e. EnduranceBaseType
           *
           * @return This accessor's type constant, i.e. EnduranceBaseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::enduranceBaseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EnduranceBaseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Fuel.
           *
           * @return The value of the simple primitive data type identified by Fuel.
           */
         virtual uci::type::MassTypeValue getFuel() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Fuel.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFuel(uci::type::MassTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Fuel exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Fuel is emabled or not
           */
         virtual bool hasFuel() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Fuel
           *
           * @param type = uci::type::accessorType::massType This Accessor's accessor type
           */
         virtual void enableFuel(uci::base::accessorType::AccessorType type = uci::type::accessorType::massType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Fuel */
         virtual void clearFuel()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Duration.
           *
           * @return The value of the simple primitive data type identified by Duration.
           */
         virtual uci::type::DurationTypeValue getDuration() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Duration.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDuration(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Duration exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Duration is emabled or not
           */
         virtual bool hasDuration() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Duration
           *
           * @param type = uci::type::accessorType::durationType This Accessor's accessor type
           */
         virtual void enableDuration(uci::base::accessorType::AccessorType type = uci::type::accessorType::durationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Duration */
         virtual void clearDuration()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DurationEnd.
           *
           * @return The value of the simple primitive data type identified by DurationEnd.
           */
         virtual uci::type::DateTimeTypeValue getDurationEnd() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DurationEnd.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDurationEnd(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DurationEnd exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DurationEnd is emabled or not
           */
         virtual bool hasDurationEnd() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DurationEnd
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableDurationEnd(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DurationEnd */
         virtual void clearDurationEnd()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Percent.
           *
           * @return The value of the simple primitive data type identified by Percent.
           */
         virtual uci::type::PercentTypeValue getPercent() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Percent.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPercent(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Percent exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Percent is emabled or not
           */
         virtual bool hasPercent() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Percent
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enablePercent(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Percent */
         virtual void clearPercent()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EnduranceBaseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EnduranceBaseType to copy from
           */
         EnduranceBaseType(const EnduranceBaseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EnduranceBaseType to the contents of the EnduranceBaseType on the
           * right hand side (rhs) of the assignment operator.EnduranceBaseType [only available to derived classes]
           *
           * @param rhs The EnduranceBaseType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::EnduranceBaseType
           * @return A constant reference to this EnduranceBaseType.
           */
         const EnduranceBaseType& operator=(const EnduranceBaseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EnduranceBaseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EnduranceBaseType_h

