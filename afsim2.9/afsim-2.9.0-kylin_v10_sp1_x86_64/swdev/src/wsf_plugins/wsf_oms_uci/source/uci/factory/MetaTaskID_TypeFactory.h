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
#ifndef Uci__Factory__MetaTaskID_TypeFactory_h
#define Uci__Factory__MetaTaskID_TypeFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__MetaTaskID_Type_h)
# include "uci/type/MetaTaskID_Type.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class MetaTaskID_TypeFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new MetaTaskID_TypeFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::MetaTaskID_TypeFactory* uci_getMetaTaskID_TypeFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a MetaTaskID_TypeFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyMetaTaskID_TypeFactory(uci::factory::MetaTaskID_TypeFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class MetaTaskID_TypeFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~MetaTaskID_TypeFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  uci::factory::MetaTaskID_TypeFactory FactoryType;
         typedef  uci::type::MetaTaskID_Type ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::MetaTaskID_TypeFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getMetaTaskID_TypeFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::MetaTaskID_TypeFactory* factory )
         {
            uci_destroyMetaTaskID_TypeFactory( factory );
         }

         /** This method constructs a new MetaTaskID_Type message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @return A message accessor that can be used to access the newly created MetaTaskID_Type message
           */
         virtual uci::type::MetaTaskID_Type& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new MetaTaskID_Type message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created MetaTaskID_Type message
           */
         virtual uci::type::MetaTaskID_Type& create(const uci::type::MetaTaskID_Type& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a MetaTaskID_Type message accessor, along with the message that the accessor was providing
           * access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::MetaTaskID_Type& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MetaTaskID_TypeFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MetaTaskID_TypeFactory to copy from
           */
         MetaTaskID_TypeFactory(const MetaTaskID_TypeFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MetaTaskID_TypeFactory to the contents of the
           * MetaTaskID_TypeFactory on the right hand side (rhs) of the assignment operator.MetaTaskID_TypeFactory [only available
           * to derived classes]
           *
           * @param rhs The MetaTaskID_TypeFactory on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::factory::MetaTaskID_TypeFactory
           * @return A constant reference to this MetaTaskID_TypeFactory.
           */
         const MetaTaskID_TypeFactory& operator=(const MetaTaskID_TypeFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MetaTaskID_TypeFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__MetaTaskID_TypeFactory_h

