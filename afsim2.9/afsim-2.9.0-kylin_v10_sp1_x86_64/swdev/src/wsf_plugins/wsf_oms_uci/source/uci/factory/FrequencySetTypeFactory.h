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
#ifndef Uci__Factory__FrequencySetTypeFactory_h
#define Uci__Factory__FrequencySetTypeFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__FrequencySetType_h)
# include "uci/type/FrequencySetType.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class FrequencySetTypeFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new FrequencySetTypeFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::FrequencySetTypeFactory* uci_getFrequencySetTypeFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a FrequencySetTypeFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyFrequencySetTypeFactory(uci::factory::FrequencySetTypeFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class FrequencySetTypeFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~FrequencySetTypeFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  uci::factory::FrequencySetTypeFactory FactoryType;
         typedef  uci::type::FrequencySetType ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::FrequencySetTypeFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getFrequencySetTypeFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::FrequencySetTypeFactory* factory )
         {
            uci_destroyFrequencySetTypeFactory( factory );
         }

         /** This method constructs a new FrequencySetType message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @return A message accessor that can be used to access the newly created FrequencySetType message
           */
         virtual uci::type::FrequencySetType& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new FrequencySetType message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created FrequencySetType message
           */
         virtual uci::type::FrequencySetType& create(const uci::type::FrequencySetType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a FrequencySetType message accessor, along with the message that the accessor was providing
           * access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::FrequencySetType& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         FrequencySetTypeFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The FrequencySetTypeFactory to copy from
           */
         FrequencySetTypeFactory(const FrequencySetTypeFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this FrequencySetTypeFactory to the contents of the
           * FrequencySetTypeFactory on the right hand side (rhs) of the assignment operator.FrequencySetTypeFactory [only
           * available to derived classes]
           *
           * @param rhs The FrequencySetTypeFactory on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::factory::FrequencySetTypeFactory
           * @return A constant reference to this FrequencySetTypeFactory.
           */
         const FrequencySetTypeFactory& operator=(const FrequencySetTypeFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // FrequencySetTypeFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__FrequencySetTypeFactory_h

