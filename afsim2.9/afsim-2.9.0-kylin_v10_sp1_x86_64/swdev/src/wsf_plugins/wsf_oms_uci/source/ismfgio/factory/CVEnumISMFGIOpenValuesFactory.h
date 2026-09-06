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
#ifndef Ismfgio__Factory__CVEnumISMFGIOpenValuesFactory_h
#define Ismfgio__Factory__CVEnumISMFGIOpenValuesFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Ismfgio__Type__CVEnumISMFGIOpenValues_h)
# include "ismfgio/type/CVEnumISMFGIOpenValues.h"
#endif


//  
namespace ismfgio {

   //  The namespace in which all factories are declared
   namespace factory {

      class CVEnumISMFGIOpenValuesFactory;
   } // Namespace: factory
} // Namespace: ismfgio


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new CVEnumISMFGIOpenValuesFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT ismfgio::factory::CVEnumISMFGIOpenValuesFactory* ismfgio_getCVEnumISMFGIOpenValuesFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a CVEnumISMFGIOpenValuesFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void ismfgio_destroyCVEnumISMFGIOpenValuesFactory(ismfgio::factory::CVEnumISMFGIOpenValuesFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  
namespace ismfgio {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class CVEnumISMFGIOpenValuesFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~CVEnumISMFGIOpenValuesFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  ismfgio::factory::CVEnumISMFGIOpenValuesFactory FactoryType;
         typedef  ismfgio::type::CVEnumISMFGIOpenValues ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT ismfgio::factory::CVEnumISMFGIOpenValuesFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return ismfgio_getCVEnumISMFGIOpenValuesFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( ismfgio::factory::CVEnumISMFGIOpenValuesFactory* factory )
         {
            ismfgio_destroyCVEnumISMFGIOpenValuesFactory( factory );
         }

         /** This method constructs a new CVEnumISMFGIOpenValues message, returning an accessor to that message that can be used
           * to access that message.
           *
           * @return A message accessor that can be used to access the newly created CVEnumISMFGIOpenValues message
           */
         virtual ismfgio::type::CVEnumISMFGIOpenValues& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new CVEnumISMFGIOpenValues message, returning an accessor to that message that can be used
           * to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created CVEnumISMFGIOpenValues message
           */
         virtual ismfgio::type::CVEnumISMFGIOpenValues& create(const ismfgio::type::CVEnumISMFGIOpenValues& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a CVEnumISMFGIOpenValues message accessor, along with the message that the accessor was
           * providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(ismfgio::type::CVEnumISMFGIOpenValues& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CVEnumISMFGIOpenValuesFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CVEnumISMFGIOpenValuesFactory to copy from
           */
         CVEnumISMFGIOpenValuesFactory(const CVEnumISMFGIOpenValuesFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CVEnumISMFGIOpenValuesFactory to the contents of the
           * CVEnumISMFGIOpenValuesFactory on the right hand side (rhs) of the assignment operator.CVEnumISMFGIOpenValuesFactory
           * [only available to derived classes]
           *
           * @param rhs The CVEnumISMFGIOpenValuesFactory on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this ismfgio::factory::CVEnumISMFGIOpenValuesFactory
           * @return A constant reference to this CVEnumISMFGIOpenValuesFactory.
           */
         const CVEnumISMFGIOpenValuesFactory& operator=(const CVEnumISMFGIOpenValuesFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CVEnumISMFGIOpenValuesFactory


   } // Namespace: factory
} // Namespace: ismfgio

#endif // Ismfgio__Factory__CVEnumISMFGIOpenValuesFactory_h

