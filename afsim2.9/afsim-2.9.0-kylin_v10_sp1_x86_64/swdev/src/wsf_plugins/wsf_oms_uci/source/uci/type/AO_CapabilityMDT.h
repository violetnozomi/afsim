// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__AO_CapabilityMDT_h
#define Uci__Type__AO_CapabilityMDT_h 1

#if !defined(Uci__Type__CapabilityBaseType_h)
# include "uci/type/CapabilityBaseType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__AO_CapabilityType_h)
# include "uci/type/AO_CapabilityType.h"
#endif

#if !defined(Uci__Type__AO_ComponentType_h)
# include "uci/type/AO_ComponentType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AO_CapabilityMDT sequence accessor class */
      class AO_CapabilityMDT : public virtual uci::type::CapabilityBaseType {
      public:

         /** The destructor */
         virtual ~AO_CapabilityMDT()
         { }

         /** Returns this accessor's type constant, i.e. AO_CapabilityMDT
           *
           * @return This accessor's type constant, i.e. AO_CapabilityMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::aO_CapabilityMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AO_CapabilityMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Describes an installed AO Capability, its configurable characteristics and its controllability. [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::AO_CapabilityType, uci::type::accessorType::aO_CapabilityType> Capability;

         /** Indicates a Subsystem Component used to implement one or more AO Capabilities. An AO Component's characteristics and
           * performance directly influence those of the Capabilities. A single Component can support multiple Capabilities.
           * Components may have settings/configuration independent of Capabilities. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::AO_ComponentType, uci::type::accessorType::aO_ComponentType> AO_Component;

         /** Returns the bounded list that is identified by the Capability.
           *
           * @return The bounded list identified by Capability.
           */
         virtual const uci::type::AO_CapabilityMDT::Capability& getCapability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Capability.
           *
           * @return The bounded list identified by Capability.
           */
         virtual uci::type::AO_CapabilityMDT::Capability& getCapability()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Capability.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapability(const uci::type::AO_CapabilityMDT::Capability& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AO_Component.
           *
           * @return The bounded list identified by AO_Component.
           */
         virtual const uci::type::AO_CapabilityMDT::AO_Component& getAO_Component() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AO_Component.
           *
           * @return The bounded list identified by AO_Component.
           */
         virtual uci::type::AO_CapabilityMDT::AO_Component& getAO_Component()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AO_Component.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAO_Component(const uci::type::AO_CapabilityMDT::AO_Component& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AO_CapabilityMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AO_CapabilityMDT to copy from
           */
         AO_CapabilityMDT(const AO_CapabilityMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AO_CapabilityMDT to the contents of the AO_CapabilityMDT on the
           * right hand side (rhs) of the assignment operator.AO_CapabilityMDT [only available to derived classes]
           *
           * @param rhs The AO_CapabilityMDT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::AO_CapabilityMDT
           * @return A constant reference to this AO_CapabilityMDT.
           */
         const AO_CapabilityMDT& operator=(const AO_CapabilityMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AO_CapabilityMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AO_CapabilityMDT_h

