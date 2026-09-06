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
#ifndef Uci__Factory__POST_SubCapabilityTypeFactory_h
#define Uci__Factory__POST_SubCapabilityTypeFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__POST_SubCapabilityType_h)
# include "uci/type/POST_SubCapabilityType.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class POST_SubCapabilityTypeFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new POST_SubCapabilityTypeFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::POST_SubCapabilityTypeFactory* uci_getPOST_SubCapabilityTypeFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a POST_SubCapabilityTypeFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyPOST_SubCapabilityTypeFactory(uci::factory::POST_SubCapabilityTypeFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class POST_SubCapabilityTypeFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~POST_SubCapabilityTypeFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  uci::factory::POST_SubCapabilityTypeFactory FactoryType;
         typedef  uci::type::POST_SubCapabilityType ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::POST_SubCapabilityTypeFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getPOST_SubCapabilityTypeFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::POST_SubCapabilityTypeFactory* factory )
         {
            uci_destroyPOST_SubCapabilityTypeFactory( factory );
         }

         /** This method constructs a new POST_SubCapabilityType message, returning an accessor to that message that can be used
           * to access that message.
           *
           * @return A message accessor that can be used to access the newly created POST_SubCapabilityType message
           */
         virtual uci::type::POST_SubCapabilityType& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new POST_SubCapabilityType message, returning an accessor to that message that can be used
           * to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created POST_SubCapabilityType message
           */
         virtual uci::type::POST_SubCapabilityType& create(const uci::type::POST_SubCapabilityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a POST_SubCapabilityType message accessor, along with the message that the accessor was
           * providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::POST_SubCapabilityType& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_SubCapabilityTypeFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_SubCapabilityTypeFactory to copy from
           */
         POST_SubCapabilityTypeFactory(const POST_SubCapabilityTypeFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_SubCapabilityTypeFactory to the contents of the
           * POST_SubCapabilityTypeFactory on the right hand side (rhs) of the assignment operator.POST_SubCapabilityTypeFactory
           * [only available to derived classes]
           *
           * @param rhs The POST_SubCapabilityTypeFactory on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::factory::POST_SubCapabilityTypeFactory
           * @return A constant reference to this POST_SubCapabilityTypeFactory.
           */
         const POST_SubCapabilityTypeFactory& operator=(const POST_SubCapabilityTypeFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_SubCapabilityTypeFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__POST_SubCapabilityTypeFactory_h

