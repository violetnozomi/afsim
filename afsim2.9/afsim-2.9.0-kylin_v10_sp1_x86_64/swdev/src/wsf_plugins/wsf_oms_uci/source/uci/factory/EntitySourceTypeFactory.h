// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Factory__EntitySourceTypeFactory_h
#define Uci__Factory__EntitySourceTypeFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__EntitySourceType_h)
# include "uci/type/EntitySourceType.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class EntitySourceTypeFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new EntitySourceTypeFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::EntitySourceTypeFactory* uci_getEntitySourceTypeFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a EntitySourceTypeFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyEntitySourceTypeFactory(uci::factory::EntitySourceTypeFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class EntitySourceTypeFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~EntitySourceTypeFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  uci::factory::EntitySourceTypeFactory FactoryType;
         typedef  uci::type::EntitySourceType ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::EntitySourceTypeFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getEntitySourceTypeFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::EntitySourceTypeFactory* factory )
         {
            uci_destroyEntitySourceTypeFactory( factory );
         }

         /** This method constructs a new EntitySourceType message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @return A message accessor that can be used to access the newly created EntitySourceType message
           */
         virtual uci::type::EntitySourceType& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new EntitySourceType message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created EntitySourceType message
           */
         virtual uci::type::EntitySourceType& create(const uci::type::EntitySourceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a EntitySourceType message accessor, along with the message that the accessor was providing
           * access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::EntitySourceType& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EntitySourceTypeFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EntitySourceTypeFactory to copy from
           */
         EntitySourceTypeFactory(const EntitySourceTypeFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EntitySourceTypeFactory to the contents of the
           * EntitySourceTypeFactory on the right hand side (rhs) of the assignment operator.EntitySourceTypeFactory [only
           * available to derived classes]
           *
           * @param rhs The EntitySourceTypeFactory on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::factory::EntitySourceTypeFactory
           * @return A constant reference to this EntitySourceTypeFactory.
           */
         const EntitySourceTypeFactory& operator=(const EntitySourceTypeFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EntitySourceTypeFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__EntitySourceTypeFactory_h

