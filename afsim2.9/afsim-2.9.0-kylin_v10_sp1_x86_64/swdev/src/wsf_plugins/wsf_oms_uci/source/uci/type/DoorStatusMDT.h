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
#ifndef Uci__Type__DoorStatusMDT_h
#define Uci__Type__DoorStatusMDT_h 1

#if !defined(Uci__Type__SupportCapabilityStatusBaseType_h)
# include "uci/type/SupportCapabilityStatusBaseType.h"
#endif

#if !defined(Uci__Type__DoorStateEnum_h)
# include "uci/type/DoorStateEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This type represents the status and settings for a door that is relevant to mission operations. */
      class DoorStatusMDT : public virtual uci::type::SupportCapabilityStatusBaseType {
      public:

         /** The destructor */
         virtual ~DoorStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. DoorStatusMDT
           *
           * @return This accessor's type constant, i.e. DoorStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::doorStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DoorStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the ID of a Capability whose use requires operation of the door. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityID_Type, uci::type::accessorType::capabilityID_Type> CapabilityID;

         /** Returns the value of the enumeration that is identified by the DoorState.
           *
           * @return The value of the enumeration identified by DoorState.
           */
         virtual const uci::type::DoorStateEnum& getDoorState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the DoorState.
           *
           * @return The value of the enumeration identified by DoorState.
           */
         virtual uci::type::DoorStateEnum& getDoorState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the DoorState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDoorState(const uci::type::DoorStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the DoorState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setDoorState(uci::type::DoorStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CloseOnHungStore.
           *
           * @return The value of the simple primitive data type identified by CloseOnHungStore.
           */
         virtual xs::Boolean getCloseOnHungStore() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CloseOnHungStore.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCloseOnHungStore(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual const uci::type::DoorStatusMDT::CapabilityID& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual uci::type::DoorStatusMDT::CapabilityID& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityID(const uci::type::DoorStatusMDT::CapabilityID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DoorStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DoorStatusMDT to copy from
           */
         DoorStatusMDT(const DoorStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DoorStatusMDT to the contents of the DoorStatusMDT on the right
           * hand side (rhs) of the assignment operator.DoorStatusMDT [only available to derived classes]
           *
           * @param rhs The DoorStatusMDT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::DoorStatusMDT
           * @return A constant reference to this DoorStatusMDT.
           */
         const DoorStatusMDT& operator=(const DoorStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DoorStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DoorStatusMDT_h

