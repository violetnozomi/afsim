// This file was generated  on 12/5/2018 at 1:1:45 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Factory__ControlStatusControlTypeFactory_h
#define Uci__Factory__ControlStatusControlTypeFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__ControlStatusControlType_h)
# include "uci/type/ControlStatusControlType.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class ControlStatusControlTypeFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new ControlStatusControlTypeFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::ControlStatusControlTypeFactory* uci_getControlStatusControlTypeFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a ControlStatusControlTypeFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyControlStatusControlTypeFactory(uci::factory::ControlStatusControlTypeFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class ControlStatusControlTypeFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~ControlStatusControlTypeFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  uci::factory::ControlStatusControlTypeFactory FactoryType;
         typedef  uci::type::ControlStatusControlType ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::ControlStatusControlTypeFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getControlStatusControlTypeFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::ControlStatusControlTypeFactory* factory )
         {
            uci_destroyControlStatusControlTypeFactory( factory );
         }

         /** This method constructs a new ControlStatusControlType message, returning an accessor to that message that can be used
           * to access that message.
           *
           * @return A message accessor that can be used to access the newly created ControlStatusControlType message
           */
         virtual uci::type::ControlStatusControlType& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new ControlStatusControlType message, returning an accessor to that message that can be used
           * to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created ControlStatusControlType message
           */
         virtual uci::type::ControlStatusControlType& create(const uci::type::ControlStatusControlType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a ControlStatusControlType message accessor, along with the message that the accessor was
           * providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::ControlStatusControlType& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ControlStatusControlTypeFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ControlStatusControlTypeFactory to copy from
           */
         ControlStatusControlTypeFactory(const ControlStatusControlTypeFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ControlStatusControlTypeFactory to the contents of the
           * ControlStatusControlTypeFactory on the right hand side (rhs) of the assignment
           * operator.ControlStatusControlTypeFactory [only available to derived classes]
           *
           * @param rhs The ControlStatusControlTypeFactory on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::factory::ControlStatusControlTypeFactory
           * @return A constant reference to this ControlStatusControlTypeFactory.
           */
         const ControlStatusControlTypeFactory& operator=(const ControlStatusControlTypeFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ControlStatusControlTypeFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__ControlStatusControlTypeFactory_h

