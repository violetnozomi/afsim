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
#ifndef Uci__Factory__IO_PortCommandStatusQFTFactory_h
#define Uci__Factory__IO_PortCommandStatusQFTFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__IO_PortCommandStatusQFT_h)
# include "uci/type/IO_PortCommandStatusQFT.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class IO_PortCommandStatusQFTFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new IO_PortCommandStatusQFTFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::IO_PortCommandStatusQFTFactory* uci_getIO_PortCommandStatusQFTFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a IO_PortCommandStatusQFTFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyIO_PortCommandStatusQFTFactory(uci::factory::IO_PortCommandStatusQFTFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class IO_PortCommandStatusQFTFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~IO_PortCommandStatusQFTFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  uci::factory::IO_PortCommandStatusQFTFactory FactoryType;
         typedef  uci::type::IO_PortCommandStatusQFT ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::IO_PortCommandStatusQFTFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getIO_PortCommandStatusQFTFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::IO_PortCommandStatusQFTFactory* factory )
         {
            uci_destroyIO_PortCommandStatusQFTFactory( factory );
         }

         /** This method constructs a new IO_PortCommandStatusQFT message, returning an accessor to that message that can be used
           * to access that message.
           *
           * @return A message accessor that can be used to access the newly created IO_PortCommandStatusQFT message
           */
         virtual uci::type::IO_PortCommandStatusQFT& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new IO_PortCommandStatusQFT message, returning an accessor to that message that can be used
           * to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created IO_PortCommandStatusQFT message
           */
         virtual uci::type::IO_PortCommandStatusQFT& create(const uci::type::IO_PortCommandStatusQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a IO_PortCommandStatusQFT message accessor, along with the message that the accessor was
           * providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::IO_PortCommandStatusQFT& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         IO_PortCommandStatusQFTFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The IO_PortCommandStatusQFTFactory to copy from
           */
         IO_PortCommandStatusQFTFactory(const IO_PortCommandStatusQFTFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this IO_PortCommandStatusQFTFactory to the contents of the
           * IO_PortCommandStatusQFTFactory on the right hand side (rhs) of the assignment operator.IO_PortCommandStatusQFTFactory
           * [only available to derived classes]
           *
           * @param rhs The IO_PortCommandStatusQFTFactory on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::factory::IO_PortCommandStatusQFTFactory
           * @return A constant reference to this IO_PortCommandStatusQFTFactory.
           */
         const IO_PortCommandStatusQFTFactory& operator=(const IO_PortCommandStatusQFTFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // IO_PortCommandStatusQFTFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__IO_PortCommandStatusQFTFactory_h

