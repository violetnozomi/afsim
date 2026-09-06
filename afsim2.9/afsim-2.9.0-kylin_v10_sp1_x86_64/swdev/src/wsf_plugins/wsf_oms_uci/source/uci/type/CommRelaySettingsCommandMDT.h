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
#ifndef Uci__Type__CommRelaySettingsCommandMDT_h
#define Uci__Type__CommRelaySettingsCommandMDT_h 1

#if !defined(Uci__Type__CapabilitySettingsCommandBaseType_h)
# include "uci/type/CapabilitySettingsCommandBaseType.h"
#endif

#if !defined(Uci__Type__CommRelayStateEnum_h)
# include "uci/type/CommRelayStateEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommRelaySettingsCommandMDT sequence accessor class */
      class CommRelaySettingsCommandMDT : public virtual uci::type::CapabilitySettingsCommandBaseType {
      public:

         /** The destructor */
         virtual ~CommRelaySettingsCommandMDT()
         { }

         /** Returns this accessor's type constant, i.e. CommRelaySettingsCommandMDT
           *
           * @return This accessor's type constant, i.e. CommRelaySettingsCommandMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commRelaySettingsCommandMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommRelaySettingsCommandMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RelayState.
           *
           * @return The value of the enumeration identified by RelayState.
           */
         virtual const uci::type::CommRelayStateEnum& getRelayState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RelayState.
           *
           * @return The value of the enumeration identified by RelayState.
           */
         virtual uci::type::CommRelayStateEnum& getRelayState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RelayState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRelayState(const uci::type::CommRelayStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RelayState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRelayState(uci::type::CommRelayStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommRelaySettingsCommandMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommRelaySettingsCommandMDT to copy from
           */
         CommRelaySettingsCommandMDT(const CommRelaySettingsCommandMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommRelaySettingsCommandMDT to the contents of the
           * CommRelaySettingsCommandMDT on the right hand side (rhs) of the assignment operator.CommRelaySettingsCommandMDT [only
           * available to derived classes]
           *
           * @param rhs The CommRelaySettingsCommandMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CommRelaySettingsCommandMDT
           * @return A constant reference to this CommRelaySettingsCommandMDT.
           */
         const CommRelaySettingsCommandMDT& operator=(const CommRelaySettingsCommandMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommRelaySettingsCommandMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommRelaySettingsCommandMDT_h

