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
#ifndef Uci__Factory__AirTargetVolumeCommandTypeFactory_h
#define Uci__Factory__AirTargetVolumeCommandTypeFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__AirTargetVolumeCommandType_h)
# include "uci/type/AirTargetVolumeCommandType.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class AirTargetVolumeCommandTypeFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new AirTargetVolumeCommandTypeFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::AirTargetVolumeCommandTypeFactory* uci_getAirTargetVolumeCommandTypeFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a AirTargetVolumeCommandTypeFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyAirTargetVolumeCommandTypeFactory(uci::factory::AirTargetVolumeCommandTypeFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class AirTargetVolumeCommandTypeFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~AirTargetVolumeCommandTypeFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  uci::factory::AirTargetVolumeCommandTypeFactory FactoryType;
         typedef  uci::type::AirTargetVolumeCommandType ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::AirTargetVolumeCommandTypeFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getAirTargetVolumeCommandTypeFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::AirTargetVolumeCommandTypeFactory* factory )
         {
            uci_destroyAirTargetVolumeCommandTypeFactory( factory );
         }

         /** This method constructs a new AirTargetVolumeCommandType message, returning an accessor to that message that can be
           * used to access that message.
           *
           * @return A message accessor that can be used to access the newly created AirTargetVolumeCommandType message
           */
         virtual uci::type::AirTargetVolumeCommandType& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new AirTargetVolumeCommandType message, returning an accessor to that message that can be
           * used to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created AirTargetVolumeCommandType message
           */
         virtual uci::type::AirTargetVolumeCommandType& create(const uci::type::AirTargetVolumeCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a AirTargetVolumeCommandType message accessor, along with the message that the accessor was
           * providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::AirTargetVolumeCommandType& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AirTargetVolumeCommandTypeFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AirTargetVolumeCommandTypeFactory to copy from
           */
         AirTargetVolumeCommandTypeFactory(const AirTargetVolumeCommandTypeFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AirTargetVolumeCommandTypeFactory to the contents of the
           * AirTargetVolumeCommandTypeFactory on the right hand side (rhs) of the assignment
           * operator.AirTargetVolumeCommandTypeFactory [only available to derived classes]
           *
           * @param rhs The AirTargetVolumeCommandTypeFactory on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::factory::AirTargetVolumeCommandTypeFactory
           * @return A constant reference to this AirTargetVolumeCommandTypeFactory.
           */
         const AirTargetVolumeCommandTypeFactory& operator=(const AirTargetVolumeCommandTypeFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AirTargetVolumeCommandTypeFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__AirTargetVolumeCommandTypeFactory_h

