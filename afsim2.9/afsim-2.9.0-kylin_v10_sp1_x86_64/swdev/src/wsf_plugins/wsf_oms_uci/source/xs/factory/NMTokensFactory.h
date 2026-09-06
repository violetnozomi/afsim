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
#ifndef Xs__Factory__NMTokensFactory_h
#define Xs__Factory__NMTokensFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Xs__Type__ListXmlSchemaPrimitives_h)
# include "xs/type/listXmlSchemaPrimitives.h"
#endif


//  The namespace in which all XML Schema Definition data types are declared
namespace xs {

   //  The namespace in which all factories are declared
   namespace factory {

      class NMTokensFactory;
   } // Namespace: factory
} // Namespace: xs

#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new NMTokensFactoryFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT xs::factory::NMTokensFactory* xs_getNMTokensFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a NMTokensFactoryFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void xs_destroyNMTokensFactory(xs::factory::NMTokensFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all XML Schema Definition data types are declared
namespace xs {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy NMTokens. */
      class NMTokensFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~NMTokensFactory()
         { }

         typedef  xs::factory::NMTokensFactory FactoryType;
         typedef  xs::NMTokens ObjectType;

         //  // Static method to construct a new SomeType object
         static UCI_EXPORT xs::factory::NMTokensFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return xs_getNMTokensFactory( abstractServiceBusConnection );
         }

         //  // Static method to destroy a SomeType object
         static UCI_EXPORT void destroyFactory( xs::factory::NMTokensFactory* factory )
         {
            xs_destroyNMTokensFactory( factory );
         }

         /** This method constructs a new NMTokens.
           *
           * @return The newly contructed NMTokens.
           */
         virtual xs::NMTokens& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new NMTokens initializing the data accessed by the new NMTokens using the data accessed by
           * the specified NMTokens (cloning).
           *
           * @param accessor The NMTokens that provides access to the data that is to be used to initialize the data accessed by
           *      the new created NMTokens.
           * @return The newly contructed NMTokens.
           */
         virtual xs::NMTokens& create(const xs::NMTokens& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys the specified NMTokens. Use of the specified accessor after it has been destroyed will result in
           * undefined behavior.
           *
           * @param accessor The NMTokens that is to be destroyed.
           */
         virtual void destroy(xs::NMTokens& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         NMTokensFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The NMTokensFactory to copy from
           */
         NMTokensFactory(const NMTokensFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this NMTokensFactory to the contents of the NMTokensFactory on the
           * right hand side (rhs) of the assignment operator.NMTokensFactory [only available to derived classes]
           *
           * @param rhs The NMTokensFactory on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this NMTokensFactory
           * @return A constant reference to this NMTokensFactory.
           */
         const NMTokensFactory& operator=(const NMTokensFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // NMTokensFactory


   } // Namespace: factory
} // Namespace: xs

#endif // Xs__Factory__NMTokensFactory_h

