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
#ifndef Ism__Factory__SARIdentifierFactory_h
#define Ism__Factory__SARIdentifierFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Ism__Type__SARIdentifier_h)
# include "ism/type/SARIdentifier.h"
#endif


//  
namespace ism {

   //  The namespace in which all factories are declared
   namespace factory {

      class SARIdentifierFactory;
   } // Namespace: factory
} // Namespace: ism


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new SARIdentifierFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT ism::factory::SARIdentifierFactory* ism_getSARIdentifierFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a SARIdentifierFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void ism_destroySARIdentifierFactory(ism::factory::SARIdentifierFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  
namespace ism {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class SARIdentifierFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~SARIdentifierFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  ism::factory::SARIdentifierFactory FactoryType;
         typedef  ism::type::SARIdentifier ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT ism::factory::SARIdentifierFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return ism_getSARIdentifierFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( ism::factory::SARIdentifierFactory* factory )
         {
            ism_destroySARIdentifierFactory( factory );
         }

         /** This method constructs a new SARIdentifier message, returning an accessor to that message that can be used to access
           * that message.
           *
           * @return A message accessor that can be used to access the newly created SARIdentifier message
           */
         virtual ism::type::SARIdentifier& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new SARIdentifier message, returning an accessor to that message that can be used to access
           * that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created SARIdentifier message
           */
         virtual ism::type::SARIdentifier& create(const ism::type::SARIdentifier& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a SARIdentifier message accessor, along with the message that the accessor was providing access
           * to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(ism::type::SARIdentifier& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SARIdentifierFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SARIdentifierFactory to copy from
           */
         SARIdentifierFactory(const SARIdentifierFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SARIdentifierFactory to the contents of the SARIdentifierFactory
           * on the right hand side (rhs) of the assignment operator.SARIdentifierFactory [only available to derived classes]
           *
           * @param rhs The SARIdentifierFactory on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this ism::factory::SARIdentifierFactory
           * @return A constant reference to this SARIdentifierFactory.
           */
         const SARIdentifierFactory& operator=(const SARIdentifierFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SARIdentifierFactory


   } // Namespace: factory
} // Namespace: ism

#endif // Ism__Factory__SARIdentifierFactory_h

