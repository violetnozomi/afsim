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
#ifndef Uci__Factory__PO_ComponentSettingsTypeFactory_h
#define Uci__Factory__PO_ComponentSettingsTypeFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__PO_ComponentSettingsType_h)
# include "uci/type/PO_ComponentSettingsType.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class PO_ComponentSettingsTypeFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new PO_ComponentSettingsTypeFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::PO_ComponentSettingsTypeFactory* uci_getPO_ComponentSettingsTypeFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a PO_ComponentSettingsTypeFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyPO_ComponentSettingsTypeFactory(uci::factory::PO_ComponentSettingsTypeFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class PO_ComponentSettingsTypeFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~PO_ComponentSettingsTypeFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  uci::factory::PO_ComponentSettingsTypeFactory FactoryType;
         typedef  uci::type::PO_ComponentSettingsType ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::PO_ComponentSettingsTypeFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getPO_ComponentSettingsTypeFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::PO_ComponentSettingsTypeFactory* factory )
         {
            uci_destroyPO_ComponentSettingsTypeFactory( factory );
         }

         /** This method constructs a new PO_ComponentSettingsType message, returning an accessor to that message that can be used
           * to access that message.
           *
           * @return A message accessor that can be used to access the newly created PO_ComponentSettingsType message
           */
         virtual uci::type::PO_ComponentSettingsType& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new PO_ComponentSettingsType message, returning an accessor to that message that can be used
           * to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created PO_ComponentSettingsType message
           */
         virtual uci::type::PO_ComponentSettingsType& create(const uci::type::PO_ComponentSettingsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a PO_ComponentSettingsType message accessor, along with the message that the accessor was
           * providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::PO_ComponentSettingsType& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PO_ComponentSettingsTypeFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PO_ComponentSettingsTypeFactory to copy from
           */
         PO_ComponentSettingsTypeFactory(const PO_ComponentSettingsTypeFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PO_ComponentSettingsTypeFactory to the contents of the
           * PO_ComponentSettingsTypeFactory on the right hand side (rhs) of the assignment
           * operator.PO_ComponentSettingsTypeFactory [only available to derived classes]
           *
           * @param rhs The PO_ComponentSettingsTypeFactory on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::factory::PO_ComponentSettingsTypeFactory
           * @return A constant reference to this PO_ComponentSettingsTypeFactory.
           */
         const PO_ComponentSettingsTypeFactory& operator=(const PO_ComponentSettingsTypeFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PO_ComponentSettingsTypeFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__PO_ComponentSettingsTypeFactory_h

