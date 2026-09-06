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
#ifndef Ismfgip__Factory__CVEnumISMFGIProtectedFactory_h
#define Ismfgip__Factory__CVEnumISMFGIProtectedFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Ismfgip__Type__CVEnumISMFGIProtected_h)
# include "ismfgip/type/CVEnumISMFGIProtected.h"
#endif


//  
namespace ismfgip {

   //  The namespace in which all factories are declared
   namespace factory {

      class CVEnumISMFGIProtectedFactory;
   } // Namespace: factory
} // Namespace: ismfgip


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new CVEnumISMFGIProtectedFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT ismfgip::factory::CVEnumISMFGIProtectedFactory* ismfgip_getCVEnumISMFGIProtectedFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a CVEnumISMFGIProtectedFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void ismfgip_destroyCVEnumISMFGIProtectedFactory(ismfgip::factory::CVEnumISMFGIProtectedFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  
namespace ismfgip {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class CVEnumISMFGIProtectedFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~CVEnumISMFGIProtectedFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  ismfgip::factory::CVEnumISMFGIProtectedFactory FactoryType;
         typedef  ismfgip::type::CVEnumISMFGIProtected ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT ismfgip::factory::CVEnumISMFGIProtectedFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return ismfgip_getCVEnumISMFGIProtectedFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( ismfgip::factory::CVEnumISMFGIProtectedFactory* factory )
         {
            ismfgip_destroyCVEnumISMFGIProtectedFactory( factory );
         }

         /** This method constructs a new CVEnumISMFGIProtected message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @return A message accessor that can be used to access the newly created CVEnumISMFGIProtected message
           */
         virtual ismfgip::type::CVEnumISMFGIProtected& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new CVEnumISMFGIProtected message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created CVEnumISMFGIProtected message
           */
         virtual ismfgip::type::CVEnumISMFGIProtected& create(const ismfgip::type::CVEnumISMFGIProtected& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a CVEnumISMFGIProtected message accessor, along with the message that the accessor was providing
           * access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(ismfgip::type::CVEnumISMFGIProtected& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CVEnumISMFGIProtectedFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CVEnumISMFGIProtectedFactory to copy from
           */
         CVEnumISMFGIProtectedFactory(const CVEnumISMFGIProtectedFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CVEnumISMFGIProtectedFactory to the contents of the
           * CVEnumISMFGIProtectedFactory on the right hand side (rhs) of the assignment operator.CVEnumISMFGIProtectedFactory
           * [only available to derived classes]
           *
           * @param rhs The CVEnumISMFGIProtectedFactory on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this ismfgip::factory::CVEnumISMFGIProtectedFactory
           * @return A constant reference to this CVEnumISMFGIProtectedFactory.
           */
         const CVEnumISMFGIProtectedFactory& operator=(const CVEnumISMFGIProtectedFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CVEnumISMFGIProtectedFactory


   } // Namespace: factory
} // Namespace: ismfgip

#endif // Ismfgip__Factory__CVEnumISMFGIProtectedFactory_h

