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
#ifndef Ismpoctyp__Factory__CVEnumISMPocTypeFactory_h
#define Ismpoctyp__Factory__CVEnumISMPocTypeFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Ismpoctyp__Type__CVEnumISMPocType_h)
# include "ismpoctyp/type/CVEnumISMPocType.h"
#endif


//  
namespace ismpoctyp {

   //  The namespace in which all factories are declared
   namespace factory {

      class CVEnumISMPocTypeFactory;
   } // Namespace: factory
} // Namespace: ismpoctyp


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new CVEnumISMPocTypeFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT ismpoctyp::factory::CVEnumISMPocTypeFactory* ismpoctyp_getCVEnumISMPocTypeFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a CVEnumISMPocTypeFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void ismpoctyp_destroyCVEnumISMPocTypeFactory(ismpoctyp::factory::CVEnumISMPocTypeFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  
namespace ismpoctyp {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class CVEnumISMPocTypeFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~CVEnumISMPocTypeFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  ismpoctyp::factory::CVEnumISMPocTypeFactory FactoryType;
         typedef  ismpoctyp::type::CVEnumISMPocType ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT ismpoctyp::factory::CVEnumISMPocTypeFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return ismpoctyp_getCVEnumISMPocTypeFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( ismpoctyp::factory::CVEnumISMPocTypeFactory* factory )
         {
            ismpoctyp_destroyCVEnumISMPocTypeFactory( factory );
         }

         /** This method constructs a new CVEnumISMPocType message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @return A message accessor that can be used to access the newly created CVEnumISMPocType message
           */
         virtual ismpoctyp::type::CVEnumISMPocType& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new CVEnumISMPocType message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created CVEnumISMPocType message
           */
         virtual ismpoctyp::type::CVEnumISMPocType& create(const ismpoctyp::type::CVEnumISMPocType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a CVEnumISMPocType message accessor, along with the message that the accessor was providing
           * access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(ismpoctyp::type::CVEnumISMPocType& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CVEnumISMPocTypeFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CVEnumISMPocTypeFactory to copy from
           */
         CVEnumISMPocTypeFactory(const CVEnumISMPocTypeFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CVEnumISMPocTypeFactory to the contents of the
           * CVEnumISMPocTypeFactory on the right hand side (rhs) of the assignment operator.CVEnumISMPocTypeFactory [only
           * available to derived classes]
           *
           * @param rhs The CVEnumISMPocTypeFactory on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this ismpoctyp::factory::CVEnumISMPocTypeFactory
           * @return A constant reference to this CVEnumISMPocTypeFactory.
           */
         const CVEnumISMPocTypeFactory& operator=(const CVEnumISMPocTypeFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CVEnumISMPocTypeFactory


   } // Namespace: factory
} // Namespace: ismpoctyp

#endif // Ismpoctyp__Factory__CVEnumISMPocTypeFactory_h

