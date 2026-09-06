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
#ifndef Uci__Factory__PassiveActiveEnumFactory_h
#define Uci__Factory__PassiveActiveEnumFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__PassiveActiveEnum_h)
# include "uci/type/PassiveActiveEnum.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class PassiveActiveEnumFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new PassiveActiveEnumFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::PassiveActiveEnumFactory* uci_getPassiveActiveEnumFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a PassiveActiveEnumFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyPassiveActiveEnumFactory(uci::factory::PassiveActiveEnumFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class PassiveActiveEnumFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~PassiveActiveEnumFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  uci::factory::PassiveActiveEnumFactory FactoryType;
         typedef  uci::type::PassiveActiveEnum ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::PassiveActiveEnumFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getPassiveActiveEnumFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::PassiveActiveEnumFactory* factory )
         {
            uci_destroyPassiveActiveEnumFactory( factory );
         }

         /** This method constructs a new PassiveActiveEnum message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @return A message accessor that can be used to access the newly created PassiveActiveEnum message
           */
         virtual uci::type::PassiveActiveEnum& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new PassiveActiveEnum message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created PassiveActiveEnum message
           */
         virtual uci::type::PassiveActiveEnum& create(const uci::type::PassiveActiveEnum& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a PassiveActiveEnum message accessor, along with the message that the accessor was providing
           * access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::PassiveActiveEnum& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PassiveActiveEnumFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PassiveActiveEnumFactory to copy from
           */
         PassiveActiveEnumFactory(const PassiveActiveEnumFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PassiveActiveEnumFactory to the contents of the
           * PassiveActiveEnumFactory on the right hand side (rhs) of the assignment operator.PassiveActiveEnumFactory [only
           * available to derived classes]
           *
           * @param rhs The PassiveActiveEnumFactory on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::factory::PassiveActiveEnumFactory
           * @return A constant reference to this PassiveActiveEnumFactory.
           */
         const PassiveActiveEnumFactory& operator=(const PassiveActiveEnumFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PassiveActiveEnumFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__PassiveActiveEnumFactory_h

