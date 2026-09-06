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
#ifndef Uci__Factory__SecurityIntrusionDetectionTypeFactory_h
#define Uci__Factory__SecurityIntrusionDetectionTypeFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__SecurityIntrusionDetectionType_h)
# include "uci/type/SecurityIntrusionDetectionType.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class SecurityIntrusionDetectionTypeFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new SecurityIntrusionDetectionTypeFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::SecurityIntrusionDetectionTypeFactory* uci_getSecurityIntrusionDetectionTypeFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a SecurityIntrusionDetectionTypeFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroySecurityIntrusionDetectionTypeFactory(uci::factory::SecurityIntrusionDetectionTypeFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class SecurityIntrusionDetectionTypeFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~SecurityIntrusionDetectionTypeFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  uci::factory::SecurityIntrusionDetectionTypeFactory FactoryType;
         typedef  uci::type::SecurityIntrusionDetectionType ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::SecurityIntrusionDetectionTypeFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getSecurityIntrusionDetectionTypeFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::SecurityIntrusionDetectionTypeFactory* factory )
         {
            uci_destroySecurityIntrusionDetectionTypeFactory( factory );
         }

         /** This method constructs a new SecurityIntrusionDetectionType message, returning an accessor to that message that can
           * be used to access that message.
           *
           * @return A message accessor that can be used to access the newly created SecurityIntrusionDetectionType message
           */
         virtual uci::type::SecurityIntrusionDetectionType& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new SecurityIntrusionDetectionType message, returning an accessor to that message that can
           * be used to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created SecurityIntrusionDetectionType message
           */
         virtual uci::type::SecurityIntrusionDetectionType& create(const uci::type::SecurityIntrusionDetectionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a SecurityIntrusionDetectionType message accessor, along with the message that the accessor was
           * providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::SecurityIntrusionDetectionType& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SecurityIntrusionDetectionTypeFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SecurityIntrusionDetectionTypeFactory to copy from
           */
         SecurityIntrusionDetectionTypeFactory(const SecurityIntrusionDetectionTypeFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SecurityIntrusionDetectionTypeFactory to the contents of the
           * SecurityIntrusionDetectionTypeFactory on the right hand side (rhs) of the assignment
           * operator.SecurityIntrusionDetectionTypeFactory [only available to derived classes]
           *
           * @param rhs The SecurityIntrusionDetectionTypeFactory on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::factory::SecurityIntrusionDetectionTypeFactory
           * @return A constant reference to this SecurityIntrusionDetectionTypeFactory.
           */
         const SecurityIntrusionDetectionTypeFactory& operator=(const SecurityIntrusionDetectionTypeFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SecurityIntrusionDetectionTypeFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__SecurityIntrusionDetectionTypeFactory_h

