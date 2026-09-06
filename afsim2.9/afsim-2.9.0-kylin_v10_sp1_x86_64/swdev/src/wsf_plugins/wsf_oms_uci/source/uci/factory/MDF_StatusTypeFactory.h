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
#ifndef Uci__Factory__MDF_StatusTypeFactory_h
#define Uci__Factory__MDF_StatusTypeFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__MDF_StatusType_h)
# include "uci/type/MDF_StatusType.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class MDF_StatusTypeFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new MDF_StatusTypeFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::MDF_StatusTypeFactory* uci_getMDF_StatusTypeFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a MDF_StatusTypeFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyMDF_StatusTypeFactory(uci::factory::MDF_StatusTypeFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class MDF_StatusTypeFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~MDF_StatusTypeFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  uci::factory::MDF_StatusTypeFactory FactoryType;
         typedef  uci::type::MDF_StatusType ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::MDF_StatusTypeFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getMDF_StatusTypeFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::MDF_StatusTypeFactory* factory )
         {
            uci_destroyMDF_StatusTypeFactory( factory );
         }

         /** This method constructs a new MDF_StatusType message, returning an accessor to that message that can be used to access
           * that message.
           *
           * @return A message accessor that can be used to access the newly created MDF_StatusType message
           */
         virtual uci::type::MDF_StatusType& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new MDF_StatusType message, returning an accessor to that message that can be used to access
           * that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created MDF_StatusType message
           */
         virtual uci::type::MDF_StatusType& create(const uci::type::MDF_StatusType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a MDF_StatusType message accessor, along with the message that the accessor was providing access
           * to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::MDF_StatusType& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MDF_StatusTypeFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MDF_StatusTypeFactory to copy from
           */
         MDF_StatusTypeFactory(const MDF_StatusTypeFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MDF_StatusTypeFactory to the contents of the MDF_StatusTypeFactory
           * on the right hand side (rhs) of the assignment operator.MDF_StatusTypeFactory [only available to derived classes]
           *
           * @param rhs The MDF_StatusTypeFactory on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::factory::MDF_StatusTypeFactory
           * @return A constant reference to this MDF_StatusTypeFactory.
           */
         const MDF_StatusTypeFactory& operator=(const MDF_StatusTypeFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MDF_StatusTypeFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__MDF_StatusTypeFactory_h

