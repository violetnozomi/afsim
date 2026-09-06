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
#ifndef Uci__Factory__ServiceConfigurationChangeRequestStatusMDTFactory_h
#define Uci__Factory__ServiceConfigurationChangeRequestStatusMDTFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__ServiceConfigurationChangeRequestStatusMDT_h)
# include "uci/type/ServiceConfigurationChangeRequestStatusMDT.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class ServiceConfigurationChangeRequestStatusMDTFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new ServiceConfigurationChangeRequestStatusMDTFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::ServiceConfigurationChangeRequestStatusMDTFactory* uci_getServiceConfigurationChangeRequestStatusMDTFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a ServiceConfigurationChangeRequestStatusMDTFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyServiceConfigurationChangeRequestStatusMDTFactory(uci::factory::ServiceConfigurationChangeRequestStatusMDTFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class ServiceConfigurationChangeRequestStatusMDTFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~ServiceConfigurationChangeRequestStatusMDTFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  uci::factory::ServiceConfigurationChangeRequestStatusMDTFactory FactoryType;
         typedef  uci::type::ServiceConfigurationChangeRequestStatusMDT ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::ServiceConfigurationChangeRequestStatusMDTFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getServiceConfigurationChangeRequestStatusMDTFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::ServiceConfigurationChangeRequestStatusMDTFactory* factory )
         {
            uci_destroyServiceConfigurationChangeRequestStatusMDTFactory( factory );
         }

         /** This method constructs a new ServiceConfigurationChangeRequestStatusMDT message, returning an accessor to that
           * message that can be used to access that message.
           *
           * @return A message accessor that can be used to access the newly created ServiceConfigurationChangeRequestStatusMDT
           *      message
           */
         virtual uci::type::ServiceConfigurationChangeRequestStatusMDT& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new ServiceConfigurationChangeRequestStatusMDT message, returning an accessor to that
           * message that can be used to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created ServiceConfigurationChangeRequestStatusMDT
           *      message
           */
         virtual uci::type::ServiceConfigurationChangeRequestStatusMDT& create(const uci::type::ServiceConfigurationChangeRequestStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a ServiceConfigurationChangeRequestStatusMDT message accessor, along with the message that the
           * accessor was providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::ServiceConfigurationChangeRequestStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ServiceConfigurationChangeRequestStatusMDTFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ServiceConfigurationChangeRequestStatusMDTFactory to copy from
           */
         ServiceConfigurationChangeRequestStatusMDTFactory(const ServiceConfigurationChangeRequestStatusMDTFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ServiceConfigurationChangeRequestStatusMDTFactory to the contents
           * of the ServiceConfigurationChangeRequestStatusMDTFactory on the right hand side (rhs) of the assignment
           * operator.ServiceConfigurationChangeRequestStatusMDTFactory [only available to derived classes]
           *
           * @param rhs The ServiceConfigurationChangeRequestStatusMDTFactory on the right hand side (rhs) of the assignment
           *      operator whose contents are used to set the contents of this
           *      uci::factory::ServiceConfigurationChangeRequestStatusMDTFactory
           * @return A constant reference to this ServiceConfigurationChangeRequestStatusMDTFactory.
           */
         const ServiceConfigurationChangeRequestStatusMDTFactory& operator=(const ServiceConfigurationChangeRequestStatusMDTFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ServiceConfigurationChangeRequestStatusMDTFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__ServiceConfigurationChangeRequestStatusMDTFactory_h

