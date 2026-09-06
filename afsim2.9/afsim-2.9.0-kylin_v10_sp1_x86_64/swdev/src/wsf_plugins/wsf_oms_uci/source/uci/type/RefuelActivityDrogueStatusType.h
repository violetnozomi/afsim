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
#ifndef Uci__Type__RefuelActivityDrogueStatusType_h
#define Uci__Type__RefuelActivityDrogueStatusType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__FuelFlowModeEnum_h)
# include "uci/type/FuelFlowModeEnum.h"
#endif

#if !defined(Uci__Type__MassType_h)
# include "uci/type/MassType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RefuelActivityDrogueStatusType sequence accessor class */
      class RefuelActivityDrogueStatusType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RefuelActivityDrogueStatusType()
         { }

         /** Returns this accessor's type constant, i.e. RefuelActivityDrogueStatusType
           *
           * @return This accessor's type constant, i.e. RefuelActivityDrogueStatusType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::refuelActivityDrogueStatusType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RefuelActivityDrogueStatusType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Transfer.
           *
           * @return The value of the simple primitive data type identified by Transfer.
           */
         virtual xs::Boolean getTransfer() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Transfer.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTransfer(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Transfer exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Transfer is emabled or not
           */
         virtual bool hasTransfer() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Transfer
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableTransfer(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Transfer */
         virtual void clearTransfer()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FuelTransfer.
           *
           * @return The value of the enumeration identified by FuelTransfer.
           */
         virtual const uci::type::FuelFlowModeEnum& getFuelTransfer() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FuelTransfer.
           *
           * @return The value of the enumeration identified by FuelTransfer.
           */
         virtual uci::type::FuelFlowModeEnum& getFuelTransfer()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FuelTransfer.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFuelTransfer(const uci::type::FuelFlowModeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FuelTransfer.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFuelTransfer(uci::type::FuelFlowModeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield FuelTransferis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasFuelTransfer() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getFuelTransfer will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::fuelFlowModeEnum This Accessor's accessor type
           */
         virtual void enableFuelTransfer(uci::base::accessorType::AccessorType type = uci::type::accessorType::fuelFlowModeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearFuelTransfer()
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



      protected:

         /** The constructor [only available to derived classes]. */
         RefuelActivityDrogueStatusType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RefuelActivityDrogueStatusType to copy from
           */
         RefuelActivityDrogueStatusType(const RefuelActivityDrogueStatusType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RefuelActivityDrogueStatusType to the contents of the
           * RefuelActivityDrogueStatusType on the right hand side (rhs) of the assignment operator.RefuelActivityDrogueStatusType
           * [only available to derived classes]
           *
           * @param rhs The RefuelActivityDrogueStatusType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::RefuelActivityDrogueStatusType
           * @return A constant reference to this RefuelActivityDrogueStatusType.
           */
         const RefuelActivityDrogueStatusType& operator=(const RefuelActivityDrogueStatusType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RefuelActivityDrogueStatusType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RefuelActivityDrogueStatusType_h

