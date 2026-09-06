// This file was generated  on 12/5/2018 at 1:1:49 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Factory__DistanceTypeFactory_h
#define Uci__Factory__DistanceTypeFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__DistanceType_h)
# include "uci/type/DistanceType.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class DistanceTypeFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new DistanceTypeFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::DistanceTypeFactory* uci_getDistanceTypeFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a DistanceTypeFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyDistanceTypeFactory(uci::factory::DistanceTypeFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class DistanceTypeFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~DistanceTypeFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  uci::factory::DistanceTypeFactory FactoryType;
         typedef  uci::type::DistanceType ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::DistanceTypeFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getDistanceTypeFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::DistanceTypeFactory* factory )
         {
            uci_destroyDistanceTypeFactory( factory );
         }

         /** This method constructs a new Double message, returning an accessor to that message that can be used to access that
           * message.
           *
           * @return A message accessor that can be used to access the newly created Double message
           */
         virtual uci::type::DistanceType& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new Double message, returning an accessor to that message that can be used to access that
           * message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created Double message
           */
         virtual uci::type::DistanceType& create(const uci::type::DistanceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a Double message accessor, along with the message that the accessor was providing access to,
           * that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::DistanceType& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DistanceTypeFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DistanceTypeFactory to copy from
           */
         DistanceTypeFactory(const DistanceTypeFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DistanceTypeFactory to the contents of the DistanceTypeFactory on
           * the right hand side (rhs) of the assignment operator.DistanceTypeFactory [only available to derived classes]
           *
           * @param rhs The DistanceTypeFactory on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::factory::DistanceTypeFactory
           * @return A constant reference to this DistanceTypeFactory.
           */
         const DistanceTypeFactory& operator=(const DistanceTypeFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DistanceTypeFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__DistanceTypeFactory_h

