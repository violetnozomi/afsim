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
#ifndef Xs__Factory__IDFactory_h
#define Xs__Factory__IDFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Xs__Type__StringXmlSchemaPrimitives_h)
# include "xs/type/stringXmlSchemaPrimitives.h"
#endif


//  The namespace in which all XML Schema Definition data types are declared
namespace xs {

   //  The namespace in which all factories are declared
   namespace factory {

      class IDFactory;
   } // Namespace: factory
} // Namespace: xs

#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new IDFactoryFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT xs::factory::IDFactory* xs_getIDFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a IDFactoryFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void xs_destroyIDFactory(xs::factory::IDFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all XML Schema Definition data types are declared
namespace xs {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy ID. */
      class IDFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~IDFactory()
         { }

         typedef  xs::factory::IDFactory FactoryType;
         typedef  xs::ID ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT xs::factory::IDFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return xs_getIDFactory( abstractServiceBusConnection );
         }

         //  Static method to destroy a SomeType object
         static UCI_EXPORT void destroyFactory( xs::factory::IDFactory* factory )
         {
            xs_destroyIDFactory( factory );
         }

         /** This method constructs a new ID.
           *
           * @return The newly contructed ID.
           */
         virtual xs::ID& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new ID initializing the data accessed by the new ID using the data accessed by the specified
           * ID (cloning).
           *
           * @param accessor The ID that provides access to the data that is to be used to initialize the data accessed by the new
           *      created ID.
           * @return The newly contructed ID.
           */
         virtual xs::ID& create(const xs::ID& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys the specified ID. Use of the specified accessor after it has been destroyed will result in
           * undefined behavior.
           *
           * @param accessor The ID that is to be destroyed.
           */
         virtual void destroy(xs::ID& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         IDFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The IDFactory to copy from
           */
         IDFactory(const IDFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this IDFactory to the contents of the IDFactory on the right hand side
           * (rhs) of the assignment operator.IDFactory [only available to derived classes]
           *
           * @param rhs The IDFactory on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this IDFactory
           * @return A constant reference to this IDFactory.
           */
         const IDFactory& operator=(const IDFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // IDFactory


   } // Namespace: factory
} // Namespace: xs

#endif // Xs__Factory__IDFactory_h

