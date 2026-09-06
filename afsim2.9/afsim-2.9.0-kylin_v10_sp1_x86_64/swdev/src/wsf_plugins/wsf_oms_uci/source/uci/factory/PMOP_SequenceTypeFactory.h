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
#ifndef Uci__Factory__PMOP_SequenceTypeFactory_h
#define Uci__Factory__PMOP_SequenceTypeFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__PMOP_SequenceType_h)
# include "uci/type/PMOP_SequenceType.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class PMOP_SequenceTypeFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new PMOP_SequenceTypeFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::PMOP_SequenceTypeFactory* uci_getPMOP_SequenceTypeFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a PMOP_SequenceTypeFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyPMOP_SequenceTypeFactory(uci::factory::PMOP_SequenceTypeFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class PMOP_SequenceTypeFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~PMOP_SequenceTypeFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  uci::factory::PMOP_SequenceTypeFactory FactoryType;
         typedef  uci::type::PMOP_SequenceType ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::PMOP_SequenceTypeFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getPMOP_SequenceTypeFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::PMOP_SequenceTypeFactory* factory )
         {
            uci_destroyPMOP_SequenceTypeFactory( factory );
         }

         /** This method constructs a new PMOP_SequenceType message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @return A message accessor that can be used to access the newly created PMOP_SequenceType message
           */
         virtual uci::type::PMOP_SequenceType& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new PMOP_SequenceType message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created PMOP_SequenceType message
           */
         virtual uci::type::PMOP_SequenceType& create(const uci::type::PMOP_SequenceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a PMOP_SequenceType message accessor, along with the message that the accessor was providing
           * access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::PMOP_SequenceType& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PMOP_SequenceTypeFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PMOP_SequenceTypeFactory to copy from
           */
         PMOP_SequenceTypeFactory(const PMOP_SequenceTypeFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PMOP_SequenceTypeFactory to the contents of the
           * PMOP_SequenceTypeFactory on the right hand side (rhs) of the assignment operator.PMOP_SequenceTypeFactory [only
           * available to derived classes]
           *
           * @param rhs The PMOP_SequenceTypeFactory on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::factory::PMOP_SequenceTypeFactory
           * @return A constant reference to this PMOP_SequenceTypeFactory.
           */
         const PMOP_SequenceTypeFactory& operator=(const PMOP_SequenceTypeFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PMOP_SequenceTypeFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__PMOP_SequenceTypeFactory_h

