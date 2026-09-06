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
#ifndef Ismrelto__Factory__CVEnumISMRelToValuesFactory_h
#define Ismrelto__Factory__CVEnumISMRelToValuesFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Ismrelto__Type__CVEnumISMRelToValues_h)
# include "ismrelto/type/CVEnumISMRelToValues.h"
#endif


//  
namespace ismrelto {

   //  The namespace in which all factories are declared
   namespace factory {

      class CVEnumISMRelToValuesFactory;
   } // Namespace: factory
} // Namespace: ismrelto


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new CVEnumISMRelToValuesFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT ismrelto::factory::CVEnumISMRelToValuesFactory* ismrelto_getCVEnumISMRelToValuesFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a CVEnumISMRelToValuesFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void ismrelto_destroyCVEnumISMRelToValuesFactory(ismrelto::factory::CVEnumISMRelToValuesFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  
namespace ismrelto {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class CVEnumISMRelToValuesFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~CVEnumISMRelToValuesFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  ismrelto::factory::CVEnumISMRelToValuesFactory FactoryType;
         typedef  ismrelto::type::CVEnumISMRelToValues ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT ismrelto::factory::CVEnumISMRelToValuesFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return ismrelto_getCVEnumISMRelToValuesFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( ismrelto::factory::CVEnumISMRelToValuesFactory* factory )
         {
            ismrelto_destroyCVEnumISMRelToValuesFactory( factory );
         }

         /** This method constructs a new CVEnumISMRelToValues message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @return A message accessor that can be used to access the newly created CVEnumISMRelToValues message
           */
         virtual ismrelto::type::CVEnumISMRelToValues& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new CVEnumISMRelToValues message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created CVEnumISMRelToValues message
           */
         virtual ismrelto::type::CVEnumISMRelToValues& create(const ismrelto::type::CVEnumISMRelToValues& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a CVEnumISMRelToValues message accessor, along with the message that the accessor was providing
           * access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(ismrelto::type::CVEnumISMRelToValues& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CVEnumISMRelToValuesFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CVEnumISMRelToValuesFactory to copy from
           */
         CVEnumISMRelToValuesFactory(const CVEnumISMRelToValuesFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CVEnumISMRelToValuesFactory to the contents of the
           * CVEnumISMRelToValuesFactory on the right hand side (rhs) of the assignment operator.CVEnumISMRelToValuesFactory [only
           * available to derived classes]
           *
           * @param rhs The CVEnumISMRelToValuesFactory on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this ismrelto::factory::CVEnumISMRelToValuesFactory
           * @return A constant reference to this CVEnumISMRelToValuesFactory.
           */
         const CVEnumISMRelToValuesFactory& operator=(const CVEnumISMRelToValuesFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CVEnumISMRelToValuesFactory


   } // Namespace: factory
} // Namespace: ismrelto

#endif // Ismrelto__Factory__CVEnumISMRelToValuesFactory_h

