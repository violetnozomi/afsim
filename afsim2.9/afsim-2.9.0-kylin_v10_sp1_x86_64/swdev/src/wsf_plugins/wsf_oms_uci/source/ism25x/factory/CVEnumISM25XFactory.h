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
#ifndef Ism25x__Factory__CVEnumISM25XFactory_h
#define Ism25x__Factory__CVEnumISM25XFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Ism25x__Type__CVEnumISM25X_h)
# include "ism25x/type/CVEnumISM25X.h"
#endif


//  
namespace ism25x {

   //  The namespace in which all factories are declared
   namespace factory {

      class CVEnumISM25XFactory;
   } // Namespace: factory
} // Namespace: ism25x


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new CVEnumISM25XFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT ism25x::factory::CVEnumISM25XFactory* ism25x_getCVEnumISM25XFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a CVEnumISM25XFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void ism25x_destroyCVEnumISM25XFactory(ism25x::factory::CVEnumISM25XFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  
namespace ism25x {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class CVEnumISM25XFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~CVEnumISM25XFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  ism25x::factory::CVEnumISM25XFactory FactoryType;
         typedef  ism25x::type::CVEnumISM25X ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT ism25x::factory::CVEnumISM25XFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return ism25x_getCVEnumISM25XFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( ism25x::factory::CVEnumISM25XFactory* factory )
         {
            ism25x_destroyCVEnumISM25XFactory( factory );
         }

         /** This method constructs a new CVEnumISM25X message, returning an accessor to that message that can be used to access
           * that message.
           *
           * @return A message accessor that can be used to access the newly created CVEnumISM25X message
           */
         virtual ism25x::type::CVEnumISM25X& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new CVEnumISM25X message, returning an accessor to that message that can be used to access
           * that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created CVEnumISM25X message
           */
         virtual ism25x::type::CVEnumISM25X& create(const ism25x::type::CVEnumISM25X& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a CVEnumISM25X message accessor, along with the message that the accessor was providing access
           * to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(ism25x::type::CVEnumISM25X& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CVEnumISM25XFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CVEnumISM25XFactory to copy from
           */
         CVEnumISM25XFactory(const CVEnumISM25XFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CVEnumISM25XFactory to the contents of the CVEnumISM25XFactory on
           * the right hand side (rhs) of the assignment operator.CVEnumISM25XFactory [only available to derived classes]
           *
           * @param rhs The CVEnumISM25XFactory on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this ism25x::factory::CVEnumISM25XFactory
           * @return A constant reference to this CVEnumISM25XFactory.
           */
         const CVEnumISM25XFactory& operator=(const CVEnumISM25XFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CVEnumISM25XFactory


   } // Namespace: factory
} // Namespace: ism25x

#endif // Ism25x__Factory__CVEnumISM25XFactory_h

