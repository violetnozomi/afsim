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
#ifndef Ism__Factory__DerivedFromFactory_h
#define Ism__Factory__DerivedFromFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Ism__Type__DerivedFrom_h)
# include "ism/type/DerivedFrom.h"
#endif


//  
namespace ism {

   //  The namespace in which all factories are declared
   namespace factory {

      class DerivedFromFactory;
   } // Namespace: factory
} // Namespace: ism


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new DerivedFromFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT ism::factory::DerivedFromFactory* ism_getDerivedFromFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a DerivedFromFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void ism_destroyDerivedFromFactory(ism::factory::DerivedFromFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  
namespace ism {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class DerivedFromFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~DerivedFromFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  ism::factory::DerivedFromFactory FactoryType;
         typedef  ism::type::DerivedFrom ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT ism::factory::DerivedFromFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return ism_getDerivedFromFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( ism::factory::DerivedFromFactory* factory )
         {
            ism_destroyDerivedFromFactory( factory );
         }

         /** This method constructs a new DerivedFrom message, returning an accessor to that message that can be used to access
           * that message.
           *
           * @return A message accessor that can be used to access the newly created DerivedFrom message
           */
         virtual ism::type::DerivedFrom& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new DerivedFrom message, returning an accessor to that message that can be used to access
           * that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created DerivedFrom message
           */
         virtual ism::type::DerivedFrom& create(const ism::type::DerivedFrom& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a DerivedFrom message accessor, along with the message that the accessor was providing access
           * to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(ism::type::DerivedFrom& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DerivedFromFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DerivedFromFactory to copy from
           */
         DerivedFromFactory(const DerivedFromFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DerivedFromFactory to the contents of the DerivedFromFactory on
           * the right hand side (rhs) of the assignment operator.DerivedFromFactory [only available to derived classes]
           *
           * @param rhs The DerivedFromFactory on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this ism::factory::DerivedFromFactory
           * @return A constant reference to this DerivedFromFactory.
           */
         const DerivedFromFactory& operator=(const DerivedFromFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DerivedFromFactory


   } // Namespace: factory
} // Namespace: ism

#endif // Ism__Factory__DerivedFromFactory_h

