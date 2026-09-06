// This file was generated  on 12/5/2018 at 1:1:50 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Factory__NetworkLinkActionTypeEnumFactory_h
#define Uci__Factory__NetworkLinkActionTypeEnumFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__NetworkLinkActionTypeEnum_h)
# include "uci/type/NetworkLinkActionTypeEnum.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class NetworkLinkActionTypeEnumFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new NetworkLinkActionTypeEnumFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::NetworkLinkActionTypeEnumFactory* uci_getNetworkLinkActionTypeEnumFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a NetworkLinkActionTypeEnumFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyNetworkLinkActionTypeEnumFactory(uci::factory::NetworkLinkActionTypeEnumFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class NetworkLinkActionTypeEnumFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~NetworkLinkActionTypeEnumFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  uci::factory::NetworkLinkActionTypeEnumFactory FactoryType;
         typedef  uci::type::NetworkLinkActionTypeEnum ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::NetworkLinkActionTypeEnumFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getNetworkLinkActionTypeEnumFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::NetworkLinkActionTypeEnumFactory* factory )
         {
            uci_destroyNetworkLinkActionTypeEnumFactory( factory );
         }

         /** This method constructs a new NetworkLinkActionTypeEnum message, returning an accessor to that message that can be
           * used to access that message.
           *
           * @return A message accessor that can be used to access the newly created NetworkLinkActionTypeEnum message
           */
         virtual uci::type::NetworkLinkActionTypeEnum& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new NetworkLinkActionTypeEnum message, returning an accessor to that message that can be
           * used to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created NetworkLinkActionTypeEnum message
           */
         virtual uci::type::NetworkLinkActionTypeEnum& create(const uci::type::NetworkLinkActionTypeEnum& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a NetworkLinkActionTypeEnum message accessor, along with the message that the accessor was
           * providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::NetworkLinkActionTypeEnum& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         NetworkLinkActionTypeEnumFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The NetworkLinkActionTypeEnumFactory to copy from
           */
         NetworkLinkActionTypeEnumFactory(const NetworkLinkActionTypeEnumFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this NetworkLinkActionTypeEnumFactory to the contents of the
           * NetworkLinkActionTypeEnumFactory on the right hand side (rhs) of the assignment
           * operator.NetworkLinkActionTypeEnumFactory [only available to derived classes]
           *
           * @param rhs The NetworkLinkActionTypeEnumFactory on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::factory::NetworkLinkActionTypeEnumFactory
           * @return A constant reference to this NetworkLinkActionTypeEnumFactory.
           */
         const NetworkLinkActionTypeEnumFactory& operator=(const NetworkLinkActionTypeEnumFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // NetworkLinkActionTypeEnumFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__NetworkLinkActionTypeEnumFactory_h

