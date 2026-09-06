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
#ifndef Ismatomic__Factory__CVEnumISMatomicEnergyMarkingsFactory_h
#define Ismatomic__Factory__CVEnumISMatomicEnergyMarkingsFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Ismatomic__Type__CVEnumISMatomicEnergyMarkings_h)
# include "ismatomic/type/CVEnumISMatomicEnergyMarkings.h"
#endif


//  
namespace ismatomic {

   //  The namespace in which all factories are declared
   namespace factory {

      class CVEnumISMatomicEnergyMarkingsFactory;
   } // Namespace: factory
} // Namespace: ismatomic


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new CVEnumISMatomicEnergyMarkingsFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT ismatomic::factory::CVEnumISMatomicEnergyMarkingsFactory* ismatomic_getCVEnumISMatomicEnergyMarkingsFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a CVEnumISMatomicEnergyMarkingsFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void ismatomic_destroyCVEnumISMatomicEnergyMarkingsFactory(ismatomic::factory::CVEnumISMatomicEnergyMarkingsFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  
namespace ismatomic {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class CVEnumISMatomicEnergyMarkingsFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~CVEnumISMatomicEnergyMarkingsFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  ismatomic::factory::CVEnumISMatomicEnergyMarkingsFactory FactoryType;
         typedef  ismatomic::type::CVEnumISMatomicEnergyMarkings ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT ismatomic::factory::CVEnumISMatomicEnergyMarkingsFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return ismatomic_getCVEnumISMatomicEnergyMarkingsFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( ismatomic::factory::CVEnumISMatomicEnergyMarkingsFactory* factory )
         {
            ismatomic_destroyCVEnumISMatomicEnergyMarkingsFactory( factory );
         }

         /** This method constructs a new CVEnumISMatomicEnergyMarkings message, returning an accessor to that message that can be
           * used to access that message.
           *
           * @return A message accessor that can be used to access the newly created CVEnumISMatomicEnergyMarkings message
           */
         virtual ismatomic::type::CVEnumISMatomicEnergyMarkings& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new CVEnumISMatomicEnergyMarkings message, returning an accessor to that message that can be
           * used to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created CVEnumISMatomicEnergyMarkings message
           */
         virtual ismatomic::type::CVEnumISMatomicEnergyMarkings& create(const ismatomic::type::CVEnumISMatomicEnergyMarkings& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a CVEnumISMatomicEnergyMarkings message accessor, along with the message that the accessor was
           * providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(ismatomic::type::CVEnumISMatomicEnergyMarkings& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CVEnumISMatomicEnergyMarkingsFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CVEnumISMatomicEnergyMarkingsFactory to copy from
           */
         CVEnumISMatomicEnergyMarkingsFactory(const CVEnumISMatomicEnergyMarkingsFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CVEnumISMatomicEnergyMarkingsFactory to the contents of the
           * CVEnumISMatomicEnergyMarkingsFactory on the right hand side (rhs) of the assignment
           * operator.CVEnumISMatomicEnergyMarkingsFactory [only available to derived classes]
           *
           * @param rhs The CVEnumISMatomicEnergyMarkingsFactory on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this ismatomic::factory::CVEnumISMatomicEnergyMarkingsFactory
           * @return A constant reference to this CVEnumISMatomicEnergyMarkingsFactory.
           */
         const CVEnumISMatomicEnergyMarkingsFactory& operator=(const CVEnumISMatomicEnergyMarkingsFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CVEnumISMatomicEnergyMarkingsFactory


   } // Namespace: factory
} // Namespace: ismatomic

#endif // Ismatomic__Factory__CVEnumISMatomicEnergyMarkingsFactory_h

