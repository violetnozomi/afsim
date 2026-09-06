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
#ifndef Ismsar__Factory__CVEnumISMSARFactory_h
#define Ismsar__Factory__CVEnumISMSARFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Ismsar__Type__CVEnumISMSAR_h)
# include "ismsar/type/CVEnumISMSAR.h"
#endif


//  
namespace ismsar {

   //  The namespace in which all factories are declared
   namespace factory {

      class CVEnumISMSARFactory;
   } // Namespace: factory
} // Namespace: ismsar


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new CVEnumISMSARFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT ismsar::factory::CVEnumISMSARFactory* ismsar_getCVEnumISMSARFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a CVEnumISMSARFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void ismsar_destroyCVEnumISMSARFactory(ismsar::factory::CVEnumISMSARFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  
namespace ismsar {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class CVEnumISMSARFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~CVEnumISMSARFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  ismsar::factory::CVEnumISMSARFactory FactoryType;
         typedef  ismsar::type::CVEnumISMSAR ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT ismsar::factory::CVEnumISMSARFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return ismsar_getCVEnumISMSARFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( ismsar::factory::CVEnumISMSARFactory* factory )
         {
            ismsar_destroyCVEnumISMSARFactory( factory );
         }

         /** This method constructs a new CVEnumISMSAR message, returning an accessor to that message that can be used to access
           * that message.
           *
           * @return A message accessor that can be used to access the newly created CVEnumISMSAR message
           */
         virtual ismsar::type::CVEnumISMSAR& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new CVEnumISMSAR message, returning an accessor to that message that can be used to access
           * that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created CVEnumISMSAR message
           */
         virtual ismsar::type::CVEnumISMSAR& create(const ismsar::type::CVEnumISMSAR& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a CVEnumISMSAR message accessor, along with the message that the accessor was providing access
           * to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(ismsar::type::CVEnumISMSAR& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CVEnumISMSARFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CVEnumISMSARFactory to copy from
           */
         CVEnumISMSARFactory(const CVEnumISMSARFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CVEnumISMSARFactory to the contents of the CVEnumISMSARFactory on
           * the right hand side (rhs) of the assignment operator.CVEnumISMSARFactory [only available to derived classes]
           *
           * @param rhs The CVEnumISMSARFactory on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this ismsar::factory::CVEnumISMSARFactory
           * @return A constant reference to this CVEnumISMSARFactory.
           */
         const CVEnumISMSARFactory& operator=(const CVEnumISMSARFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CVEnumISMSARFactory


   } // Namespace: factory
} // Namespace: ismsar

#endif // Ismsar__Factory__CVEnumISMSARFactory_h

