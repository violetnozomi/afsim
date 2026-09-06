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
#ifndef Ism__Factory__NonUSControlsFactory_h
#define Ism__Factory__NonUSControlsFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Ism__Type__NonUSControls_h)
# include "ism/type/NonUSControls.h"
#endif


//  
namespace ism {

   //  The namespace in which all factories are declared
   namespace factory {

      class NonUSControlsFactory;
   } // Namespace: factory
} // Namespace: ism


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new NonUSControlsFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT ism::factory::NonUSControlsFactory* ism_getNonUSControlsFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a NonUSControlsFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void ism_destroyNonUSControlsFactory(ism::factory::NonUSControlsFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  
namespace ism {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class NonUSControlsFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~NonUSControlsFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  ism::factory::NonUSControlsFactory FactoryType;
         typedef  ism::type::NonUSControls ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT ism::factory::NonUSControlsFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return ism_getNonUSControlsFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( ism::factory::NonUSControlsFactory* factory )
         {
            ism_destroyNonUSControlsFactory( factory );
         }

         /** This method constructs a new NonUSControls message, returning an accessor to that message that can be used to access
           * that message.
           *
           * @return A message accessor that can be used to access the newly created NonUSControls message
           */
         virtual ism::type::NonUSControls& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new NonUSControls message, returning an accessor to that message that can be used to access
           * that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created NonUSControls message
           */
         virtual ism::type::NonUSControls& create(const ism::type::NonUSControls& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a NonUSControls message accessor, along with the message that the accessor was providing access
           * to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(ism::type::NonUSControls& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         NonUSControlsFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The NonUSControlsFactory to copy from
           */
         NonUSControlsFactory(const NonUSControlsFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this NonUSControlsFactory to the contents of the NonUSControlsFactory
           * on the right hand side (rhs) of the assignment operator.NonUSControlsFactory [only available to derived classes]
           *
           * @param rhs The NonUSControlsFactory on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this ism::factory::NonUSControlsFactory
           * @return A constant reference to this NonUSControlsFactory.
           */
         const NonUSControlsFactory& operator=(const NonUSControlsFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // NonUSControlsFactory


   } // Namespace: factory
} // Namespace: ism

#endif // Ism__Factory__NonUSControlsFactory_h

