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
#ifndef Uci__Factory__SecurityAuditManagementEnumFactory_h
#define Uci__Factory__SecurityAuditManagementEnumFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__SecurityAuditManagementEnum_h)
# include "uci/type/SecurityAuditManagementEnum.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class SecurityAuditManagementEnumFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new SecurityAuditManagementEnumFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::SecurityAuditManagementEnumFactory* uci_getSecurityAuditManagementEnumFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a SecurityAuditManagementEnumFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroySecurityAuditManagementEnumFactory(uci::factory::SecurityAuditManagementEnumFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class SecurityAuditManagementEnumFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~SecurityAuditManagementEnumFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  uci::factory::SecurityAuditManagementEnumFactory FactoryType;
         typedef  uci::type::SecurityAuditManagementEnum ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::SecurityAuditManagementEnumFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getSecurityAuditManagementEnumFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::SecurityAuditManagementEnumFactory* factory )
         {
            uci_destroySecurityAuditManagementEnumFactory( factory );
         }

         /** This method constructs a new SecurityAuditManagementEnum message, returning an accessor to that message that can be
           * used to access that message.
           *
           * @return A message accessor that can be used to access the newly created SecurityAuditManagementEnum message
           */
         virtual uci::type::SecurityAuditManagementEnum& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new SecurityAuditManagementEnum message, returning an accessor to that message that can be
           * used to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created SecurityAuditManagementEnum message
           */
         virtual uci::type::SecurityAuditManagementEnum& create(const uci::type::SecurityAuditManagementEnum& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a SecurityAuditManagementEnum message accessor, along with the message that the accessor was
           * providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::SecurityAuditManagementEnum& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SecurityAuditManagementEnumFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SecurityAuditManagementEnumFactory to copy from
           */
         SecurityAuditManagementEnumFactory(const SecurityAuditManagementEnumFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SecurityAuditManagementEnumFactory to the contents of the
           * SecurityAuditManagementEnumFactory on the right hand side (rhs) of the assignment
           * operator.SecurityAuditManagementEnumFactory [only available to derived classes]
           *
           * @param rhs The SecurityAuditManagementEnumFactory on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::factory::SecurityAuditManagementEnumFactory
           * @return A constant reference to this SecurityAuditManagementEnumFactory.
           */
         const SecurityAuditManagementEnumFactory& operator=(const SecurityAuditManagementEnumFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SecurityAuditManagementEnumFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__SecurityAuditManagementEnumFactory_h

