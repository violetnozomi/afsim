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
#ifndef Ismpoctyp__Factory__CVEnumISMPocTypeValuesFactory_h
#define Ismpoctyp__Factory__CVEnumISMPocTypeValuesFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Ismpoctyp__Type__CVEnumISMPocTypeValues_h)
# include "ismpoctyp/type/CVEnumISMPocTypeValues.h"
#endif


//  
namespace ismpoctyp {

   //  The namespace in which all factories are declared
   namespace factory {

      class CVEnumISMPocTypeValuesFactory;
   } // Namespace: factory
} // Namespace: ismpoctyp


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new CVEnumISMPocTypeValuesFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT ismpoctyp::factory::CVEnumISMPocTypeValuesFactory* ismpoctyp_getCVEnumISMPocTypeValuesFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a CVEnumISMPocTypeValuesFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void ismpoctyp_destroyCVEnumISMPocTypeValuesFactory(ismpoctyp::factory::CVEnumISMPocTypeValuesFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  
namespace ismpoctyp {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class CVEnumISMPocTypeValuesFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~CVEnumISMPocTypeValuesFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  ismpoctyp::factory::CVEnumISMPocTypeValuesFactory FactoryType;
         typedef  ismpoctyp::type::CVEnumISMPocTypeValues ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT ismpoctyp::factory::CVEnumISMPocTypeValuesFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return ismpoctyp_getCVEnumISMPocTypeValuesFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( ismpoctyp::factory::CVEnumISMPocTypeValuesFactory* factory )
         {
            ismpoctyp_destroyCVEnumISMPocTypeValuesFactory( factory );
         }

         /** This method constructs a new CVEnumISMPocTypeValues message, returning an accessor to that message that can be used
           * to access that message.
           *
           * @return A message accessor that can be used to access the newly created CVEnumISMPocTypeValues message
           */
         virtual ismpoctyp::type::CVEnumISMPocTypeValues& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new CVEnumISMPocTypeValues message, returning an accessor to that message that can be used
           * to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created CVEnumISMPocTypeValues message
           */
         virtual ismpoctyp::type::CVEnumISMPocTypeValues& create(const ismpoctyp::type::CVEnumISMPocTypeValues& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a CVEnumISMPocTypeValues message accessor, along with the message that the accessor was
           * providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(ismpoctyp::type::CVEnumISMPocTypeValues& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CVEnumISMPocTypeValuesFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CVEnumISMPocTypeValuesFactory to copy from
           */
         CVEnumISMPocTypeValuesFactory(const CVEnumISMPocTypeValuesFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CVEnumISMPocTypeValuesFactory to the contents of the
           * CVEnumISMPocTypeValuesFactory on the right hand side (rhs) of the assignment operator.CVEnumISMPocTypeValuesFactory
           * [only available to derived classes]
           *
           * @param rhs The CVEnumISMPocTypeValuesFactory on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this ismpoctyp::factory::CVEnumISMPocTypeValuesFactory
           * @return A constant reference to this CVEnumISMPocTypeValuesFactory.
           */
         const CVEnumISMPocTypeValuesFactory& operator=(const CVEnumISMPocTypeValuesFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CVEnumISMPocTypeValuesFactory


   } // Namespace: factory
} // Namespace: ismpoctyp

#endif // Ismpoctyp__Factory__CVEnumISMPocTypeValuesFactory_h

