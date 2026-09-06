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
#ifndef Uci__Factory__DoorStatusMTFactory_h
#define Uci__Factory__DoorStatusMTFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__DoorStatusMT_h)
# include "uci/type/DoorStatusMT.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class DoorStatusMTFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new DoorStatusMTFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::DoorStatusMTFactory* uci_getDoorStatusMTFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a DoorStatusMTFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyDoorStatusMTFactory(uci::factory::DoorStatusMTFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class DoorStatusMTFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~DoorStatusMTFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  uci::factory::DoorStatusMTFactory FactoryType;
         typedef  uci::type::DoorStatusMT ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::DoorStatusMTFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getDoorStatusMTFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::DoorStatusMTFactory* factory )
         {
            uci_destroyDoorStatusMTFactory( factory );
         }

         /** This method constructs a new DoorStatusMT message, returning an accessor to that message that can be used to access
           * that message.
           *
           * @return A message accessor that can be used to access the newly created DoorStatusMT message
           */
         virtual uci::type::DoorStatusMT& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new DoorStatusMT message, returning an accessor to that message that can be used to access
           * that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created DoorStatusMT message
           */
         virtual uci::type::DoorStatusMT& create(const uci::type::DoorStatusMT& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a DoorStatusMT message accessor, along with the message that the accessor was providing access
           * to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::DoorStatusMT& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DoorStatusMTFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DoorStatusMTFactory to copy from
           */
         DoorStatusMTFactory(const DoorStatusMTFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DoorStatusMTFactory to the contents of the DoorStatusMTFactory on
           * the right hand side (rhs) of the assignment operator.DoorStatusMTFactory [only available to derived classes]
           *
           * @param rhs The DoorStatusMTFactory on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::factory::DoorStatusMTFactory
           * @return A constant reference to this DoorStatusMTFactory.
           */
         const DoorStatusMTFactory& operator=(const DoorStatusMTFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DoorStatusMTFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__DoorStatusMTFactory_h

