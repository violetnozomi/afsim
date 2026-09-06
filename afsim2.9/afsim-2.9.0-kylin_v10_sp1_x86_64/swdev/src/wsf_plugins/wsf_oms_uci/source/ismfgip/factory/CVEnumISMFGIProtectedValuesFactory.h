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
#ifndef Ismfgip__Factory__CVEnumISMFGIProtectedValuesFactory_h
#define Ismfgip__Factory__CVEnumISMFGIProtectedValuesFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Ismfgip__Type__CVEnumISMFGIProtectedValues_h)
# include "ismfgip/type/CVEnumISMFGIProtectedValues.h"
#endif


//  
namespace ismfgip {

   //  The namespace in which all factories are declared
   namespace factory {

      class CVEnumISMFGIProtectedValuesFactory;
   } // Namespace: factory
} // Namespace: ismfgip


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new CVEnumISMFGIProtectedValuesFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT ismfgip::factory::CVEnumISMFGIProtectedValuesFactory* ismfgip_getCVEnumISMFGIProtectedValuesFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a CVEnumISMFGIProtectedValuesFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void ismfgip_destroyCVEnumISMFGIProtectedValuesFactory(ismfgip::factory::CVEnumISMFGIProtectedValuesFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  
namespace ismfgip {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class CVEnumISMFGIProtectedValuesFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~CVEnumISMFGIProtectedValuesFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  ismfgip::factory::CVEnumISMFGIProtectedValuesFactory FactoryType;
         typedef  ismfgip::type::CVEnumISMFGIProtectedValues ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT ismfgip::factory::CVEnumISMFGIProtectedValuesFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return ismfgip_getCVEnumISMFGIProtectedValuesFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( ismfgip::factory::CVEnumISMFGIProtectedValuesFactory* factory )
         {
            ismfgip_destroyCVEnumISMFGIProtectedValuesFactory( factory );
         }

         /** This method constructs a new CVEnumISMFGIProtectedValues message, returning an accessor to that message that can be
           * used to access that message.
           *
           * @return A message accessor that can be used to access the newly created CVEnumISMFGIProtectedValues message
           */
         virtual ismfgip::type::CVEnumISMFGIProtectedValues& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new CVEnumISMFGIProtectedValues message, returning an accessor to that message that can be
           * used to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created CVEnumISMFGIProtectedValues message
           */
         virtual ismfgip::type::CVEnumISMFGIProtectedValues& create(const ismfgip::type::CVEnumISMFGIProtectedValues& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a CVEnumISMFGIProtectedValues message accessor, along with the message that the accessor was
           * providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(ismfgip::type::CVEnumISMFGIProtectedValues& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CVEnumISMFGIProtectedValuesFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CVEnumISMFGIProtectedValuesFactory to copy from
           */
         CVEnumISMFGIProtectedValuesFactory(const CVEnumISMFGIProtectedValuesFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CVEnumISMFGIProtectedValuesFactory to the contents of the
           * CVEnumISMFGIProtectedValuesFactory on the right hand side (rhs) of the assignment
           * operator.CVEnumISMFGIProtectedValuesFactory [only available to derived classes]
           *
           * @param rhs The CVEnumISMFGIProtectedValuesFactory on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this ismfgip::factory::CVEnumISMFGIProtectedValuesFactory
           * @return A constant reference to this CVEnumISMFGIProtectedValuesFactory.
           */
         const CVEnumISMFGIProtectedValuesFactory& operator=(const CVEnumISMFGIProtectedValuesFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CVEnumISMFGIProtectedValuesFactory


   } // Namespace: factory
} // Namespace: ismfgip

#endif // Ismfgip__Factory__CVEnumISMFGIProtectedValuesFactory_h

