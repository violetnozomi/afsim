// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Factory__SystemMetadataMDTFactory_h
#define Uci__Factory__SystemMetadataMDTFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__SystemMetadataMDT_h)
# include "uci/type/SystemMetadataMDT.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class SystemMetadataMDTFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new SystemMetadataMDTFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::SystemMetadataMDTFactory* uci_getSystemMetadataMDTFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a SystemMetadataMDTFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroySystemMetadataMDTFactory(uci::factory::SystemMetadataMDTFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class SystemMetadataMDTFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~SystemMetadataMDTFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  uci::factory::SystemMetadataMDTFactory FactoryType;
         typedef  uci::type::SystemMetadataMDT ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::SystemMetadataMDTFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getSystemMetadataMDTFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::SystemMetadataMDTFactory* factory )
         {
            uci_destroySystemMetadataMDTFactory( factory );
         }

         /** This method constructs a new SystemMetadataMDT message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @return A message accessor that can be used to access the newly created SystemMetadataMDT message
           */
         virtual uci::type::SystemMetadataMDT& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new SystemMetadataMDT message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created SystemMetadataMDT message
           */
         virtual uci::type::SystemMetadataMDT& create(const uci::type::SystemMetadataMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a SystemMetadataMDT message accessor, along with the message that the accessor was providing
           * access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::SystemMetadataMDT& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SystemMetadataMDTFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SystemMetadataMDTFactory to copy from
           */
         SystemMetadataMDTFactory(const SystemMetadataMDTFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SystemMetadataMDTFactory to the contents of the
           * SystemMetadataMDTFactory on the right hand side (rhs) of the assignment operator.SystemMetadataMDTFactory [only
           * available to derived classes]
           *
           * @param rhs The SystemMetadataMDTFactory on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::factory::SystemMetadataMDTFactory
           * @return A constant reference to this SystemMetadataMDTFactory.
           */
         const SystemMetadataMDTFactory& operator=(const SystemMetadataMDTFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SystemMetadataMDTFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__SystemMetadataMDTFactory_h

