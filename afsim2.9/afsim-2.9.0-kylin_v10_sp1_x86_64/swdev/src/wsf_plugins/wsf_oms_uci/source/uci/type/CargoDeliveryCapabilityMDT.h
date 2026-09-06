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
#ifndef Uci__Type__CargoDeliveryCapabilityMDT_h
#define Uci__Type__CargoDeliveryCapabilityMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CargoDeliveryCapabilityType_h)
# include "uci/type/CargoDeliveryCapabilityType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CargoDeliveryCapabilityMDT sequence accessor class */
      class CargoDeliveryCapabilityMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CargoDeliveryCapabilityMDT()
         { }

         /** Returns this accessor's type constant, i.e. CargoDeliveryCapabilityMDT
           *
           * @return This accessor's type constant, i.e. CargoDeliveryCapabilityMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::cargoDeliveryCapabilityMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CargoDeliveryCapabilityMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Describes an installed Cargo Delivery capability, its configurable characteristics and its controllability. [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CargoDeliveryCapabilityType, uci::type::accessorType::cargoDeliveryCapabilityType> Capability;

         /** Returns the bounded list that is identified by the Capability.
           *
           * @return The bounded list identified by Capability.
           */
         virtual const uci::type::CargoDeliveryCapabilityMDT::Capability& getCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Capability.
           *
           * @return The bounded list identified by Capability.
           */
         virtual uci::type::CargoDeliveryCapabilityMDT::Capability& getCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Capability.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapability(const uci::type::CargoDeliveryCapabilityMDT::Capability& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CargoDeliveryCapabilityMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CargoDeliveryCapabilityMDT to copy from
           */
         CargoDeliveryCapabilityMDT(const CargoDeliveryCapabilityMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CargoDeliveryCapabilityMDT to the contents of the
           * CargoDeliveryCapabilityMDT on the right hand side (rhs) of the assignment operator.CargoDeliveryCapabilityMDT [only
           * available to derived classes]
           *
           * @param rhs The CargoDeliveryCapabilityMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CargoDeliveryCapabilityMDT
           * @return A constant reference to this CargoDeliveryCapabilityMDT.
           */
         const CargoDeliveryCapabilityMDT& operator=(const CargoDeliveryCapabilityMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CargoDeliveryCapabilityMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CargoDeliveryCapabilityMDT_h

