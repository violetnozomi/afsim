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
#ifndef Uci__Type__RefuelCapabilityMDT_h
#define Uci__Type__RefuelCapabilityMDT_h 1

#if !defined(Uci__Type__CapabilityBaseType_h)
# include "uci/type/CapabilityBaseType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__RefuelCapabilityType_h)
# include "uci/type/RefuelCapabilityType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RefuelCapabilityMDT sequence accessor class */
      class RefuelCapabilityMDT : public virtual uci::type::CapabilityBaseType {
      public:

         /** The destructor */
         virtual ~RefuelCapabilityMDT()
         { }

         /** Returns this accessor's type constant, i.e. RefuelCapabilityMDT
           *
           * @return This accessor's type constant, i.e. RefuelCapabilityMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::refuelCapabilityMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RefuelCapabilityMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates an installed Refuel Capability, its configurable characteristics and its controllability. [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::RefuelCapabilityType, uci::type::accessorType::refuelCapabilityType> Capability;

         /** Returns the bounded list that is identified by the Capability.
           *
           * @return The bounded list identified by Capability.
           */
         virtual const uci::type::RefuelCapabilityMDT::Capability& getCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Capability.
           *
           * @return The bounded list identified by Capability.
           */
         virtual uci::type::RefuelCapabilityMDT::Capability& getCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Capability.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapability(const uci::type::RefuelCapabilityMDT::Capability& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RefuelCapabilityMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RefuelCapabilityMDT to copy from
           */
         RefuelCapabilityMDT(const RefuelCapabilityMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RefuelCapabilityMDT to the contents of the RefuelCapabilityMDT on
           * the right hand side (rhs) of the assignment operator.RefuelCapabilityMDT [only available to derived classes]
           *
           * @param rhs The RefuelCapabilityMDT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::RefuelCapabilityMDT
           * @return A constant reference to this RefuelCapabilityMDT.
           */
         const RefuelCapabilityMDT& operator=(const RefuelCapabilityMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RefuelCapabilityMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RefuelCapabilityMDT_h

