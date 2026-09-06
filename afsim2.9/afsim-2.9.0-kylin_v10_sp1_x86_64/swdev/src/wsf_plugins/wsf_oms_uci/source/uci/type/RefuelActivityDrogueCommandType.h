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
#ifndef Uci__Type__RefuelActivityDrogueCommandType_h
#define Uci__Type__RefuelActivityDrogueCommandType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MassType_h)
# include "uci/type/MassType.h"
#endif

#if !defined(Uci__Type__FuelFlowModeEnum_h)
# include "uci/type/FuelFlowModeEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RefuelActivityDrogueCommandType sequence accessor class */
      class RefuelActivityDrogueCommandType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RefuelActivityDrogueCommandType()
         { }

         /** Returns this accessor's type constant, i.e. RefuelActivityDrogueCommandType
           *
           * @return This accessor's type constant, i.e. RefuelActivityDrogueCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::refuelActivityDrogueCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RefuelActivityDrogueCommandType& accessor)
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


         /** Returns the value of the enumeration that is identified by the FuelTransferToReceiver.
           *
           * @return The value of the enumeration identified by FuelTransferToReceiver.
           */
         virtual const uci::type::FuelFlowModeEnum& getFuelTransferToReceiver() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FuelTransferToReceiver.
           *
           * @return The value of the enumeration identified by FuelTransferToReceiver.
           */
         virtual uci::type::FuelFlowModeEnum& getFuelTransferToReceiver()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FuelTransferToReceiver.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFuelTransferToReceiver(const uci::type::FuelFlowModeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FuelTransferToReceiver.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFuelTransferToReceiver(uci::type::FuelFlowModeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield FuelTransferToReceiveris available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasFuelTransferToReceiver() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getFuelTransferToReceiver will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::fuelFlowModeEnum This Accessor's accessor type
           */
         virtual void enableFuelTransferToReceiver(uci::base::accessorType::AccessorType type = uci::type::accessorType::fuelFlowModeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearFuelTransferToReceiver()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RefuelActivityDrogueCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RefuelActivityDrogueCommandType to copy from
           */
         RefuelActivityDrogueCommandType(const RefuelActivityDrogueCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RefuelActivityDrogueCommandType to the contents of the
           * RefuelActivityDrogueCommandType on the right hand side (rhs) of the assignment
           * operator.RefuelActivityDrogueCommandType [only available to derived classes]
           *
           * @param rhs The RefuelActivityDrogueCommandType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::RefuelActivityDrogueCommandType
           * @return A constant reference to this RefuelActivityDrogueCommandType.
           */
         const RefuelActivityDrogueCommandType& operator=(const RefuelActivityDrogueCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RefuelActivityDrogueCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RefuelActivityDrogueCommandType_h

