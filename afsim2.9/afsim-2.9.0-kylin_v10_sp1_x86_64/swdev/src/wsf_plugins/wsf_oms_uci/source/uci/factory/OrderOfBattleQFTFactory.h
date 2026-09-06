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
#ifndef Uci__Factory__OrderOfBattleQFTFactory_h
#define Uci__Factory__OrderOfBattleQFTFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__OrderOfBattleQFT_h)
# include "uci/type/OrderOfBattleQFT.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class OrderOfBattleQFTFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new OrderOfBattleQFTFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::OrderOfBattleQFTFactory* uci_getOrderOfBattleQFTFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a OrderOfBattleQFTFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyOrderOfBattleQFTFactory(uci::factory::OrderOfBattleQFTFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class OrderOfBattleQFTFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~OrderOfBattleQFTFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  uci::factory::OrderOfBattleQFTFactory FactoryType;
         typedef  uci::type::OrderOfBattleQFT ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::OrderOfBattleQFTFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getOrderOfBattleQFTFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::OrderOfBattleQFTFactory* factory )
         {
            uci_destroyOrderOfBattleQFTFactory( factory );
         }

         /** This method constructs a new OrderOfBattleQFT message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @return A message accessor that can be used to access the newly created OrderOfBattleQFT message
           */
         virtual uci::type::OrderOfBattleQFT& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new OrderOfBattleQFT message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created OrderOfBattleQFT message
           */
         virtual uci::type::OrderOfBattleQFT& create(const uci::type::OrderOfBattleQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a OrderOfBattleQFT message accessor, along with the message that the accessor was providing
           * access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::OrderOfBattleQFT& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OrderOfBattleQFTFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OrderOfBattleQFTFactory to copy from
           */
         OrderOfBattleQFTFactory(const OrderOfBattleQFTFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OrderOfBattleQFTFactory to the contents of the
           * OrderOfBattleQFTFactory on the right hand side (rhs) of the assignment operator.OrderOfBattleQFTFactory [only
           * available to derived classes]
           *
           * @param rhs The OrderOfBattleQFTFactory on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::factory::OrderOfBattleQFTFactory
           * @return A constant reference to this OrderOfBattleQFTFactory.
           */
         const OrderOfBattleQFTFactory& operator=(const OrderOfBattleQFTFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OrderOfBattleQFTFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__OrderOfBattleQFTFactory_h

