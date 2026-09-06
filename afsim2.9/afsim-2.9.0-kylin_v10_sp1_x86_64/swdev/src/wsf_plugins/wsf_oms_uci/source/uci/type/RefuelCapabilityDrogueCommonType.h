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
#ifndef Uci__Type__RefuelCapabilityDrogueCommonType_h
#define Uci__Type__RefuelCapabilityDrogueCommonType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__DrogueCommandEnum_h)
# include "uci/type/DrogueCommandEnum.h"
#endif

#if !defined(Uci__Type__MassType_h)
# include "uci/type/MassType.h"
#endif

#if !defined(Uci__Type__TailConeLightEnum_h)
# include "uci/type/TailConeLightEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RefuelCapabilityDrogueCommonType sequence accessor class */
      class RefuelCapabilityDrogueCommonType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RefuelCapabilityDrogueCommonType()
         { }

         /** Returns this accessor's type constant, i.e. RefuelCapabilityDrogueCommonType
           *
           * @return This accessor's type constant, i.e. RefuelCapabilityDrogueCommonType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::refuelCapabilityDrogueCommonType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RefuelCapabilityDrogueCommonType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Power.
           *
           * @return The value of the simple primitive data type identified by Power.
           */
         virtual xs::Boolean getPower() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Power.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPower(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Power exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Power is emabled or not
           */
         virtual bool hasPower() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Power
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enablePower(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Power */
         virtual void clearPower()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Hose.
           *
           * @return The value of the enumeration identified by Hose.
           */
         virtual const uci::type::DrogueCommandEnum& getHose() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Hose.
           *
           * @return The value of the enumeration identified by Hose.
           */
         virtual uci::type::DrogueCommandEnum& getHose()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Hose.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setHose(const uci::type::DrogueCommandEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Hose.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setHose(uci::type::DrogueCommandEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Hoseis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasHose() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getHose will return the optional field and not throw an exception when invoked.
           *
           * @param type = uci::type::accessorType::drogueCommandEnum This Accessor's accessor type
           */
         virtual void enableHose(uci::base::accessorType::AccessorType type = uci::type::accessorType::drogueCommandEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearHose()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SelectForJettison.
           *
           * @return The value of the simple primitive data type identified by SelectForJettison.
           */
         virtual xs::Boolean getSelectForJettison() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SelectForJettison.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSelectForJettison(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SelectForJettison exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SelectForJettison is emabled or not
           */
         virtual bool hasSelectForJettison() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SelectForJettison
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableSelectForJettison(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SelectForJettison */
         virtual void clearSelectForJettison()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ScheduledFuelToBeDelivered.
           *
           * @return The value of the simple primitive data type identified by ScheduledFuelToBeDelivered.
           */
         virtual uci::type::MassTypeValue getScheduledFuelToBeDelivered() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ScheduledFuelToBeDelivered.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setScheduledFuelToBeDelivered(uci::type::MassTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ScheduledFuelToBeDelivered exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ScheduledFuelToBeDelivered is emabled or not
           */
         virtual bool hasScheduledFuelToBeDelivered() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ScheduledFuelToBeDelivered
           *
           * @param type = uci::type::accessorType::massType This Accessor's accessor type
           */
         virtual void enableScheduledFuelToBeDelivered(uci::base::accessorType::AccessorType type = uci::type::accessorType::massType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ScheduledFuelToBeDelivered */
         virtual void clearScheduledFuelToBeDelivered()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TailConeLightsIntensity.
           *
           * @return The value of the enumeration identified by TailConeLightsIntensity.
           */
         virtual const uci::type::TailConeLightEnum& getTailConeLightsIntensity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TailConeLightsIntensity.
           *
           * @return The value of the enumeration identified by TailConeLightsIntensity.
           */
         virtual uci::type::TailConeLightEnum& getTailConeLightsIntensity()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TailConeLightsIntensity.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTailConeLightsIntensity(const uci::type::TailConeLightEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TailConeLightsIntensity.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTailConeLightsIntensity(uci::type::TailConeLightEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield TailConeLightsIntensityis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasTailConeLightsIntensity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getTailConeLightsIntensity will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::tailConeLightEnum This Accessor's accessor type
           */
         virtual void enableTailConeLightsIntensity(uci::base::accessorType::AccessorType type = uci::type::accessorType::tailConeLightEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearTailConeLightsIntensity()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RefuelCapabilityDrogueCommonType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RefuelCapabilityDrogueCommonType to copy from
           */
         RefuelCapabilityDrogueCommonType(const RefuelCapabilityDrogueCommonType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RefuelCapabilityDrogueCommonType to the contents of the
           * RefuelCapabilityDrogueCommonType on the right hand side (rhs) of the assignment
           * operator.RefuelCapabilityDrogueCommonType [only available to derived classes]
           *
           * @param rhs The RefuelCapabilityDrogueCommonType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::RefuelCapabilityDrogueCommonType
           * @return A constant reference to this RefuelCapabilityDrogueCommonType.
           */
         const RefuelCapabilityDrogueCommonType& operator=(const RefuelCapabilityDrogueCommonType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RefuelCapabilityDrogueCommonType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RefuelCapabilityDrogueCommonType_h

