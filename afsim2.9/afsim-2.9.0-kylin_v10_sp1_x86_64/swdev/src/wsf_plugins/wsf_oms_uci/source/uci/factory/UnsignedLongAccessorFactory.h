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
#ifndef Uci__Factory__UnsignedLongAccessorFactory_h
#define Uci__Factory__UnsignedLongAccessorFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Base__UnsignedLongAccessor_h)
# include "uci/base/UnsignedLongAccessor.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class UnsignedLongAccessorFactory;
   } // Namespace: factory
} // Namespace: uci

#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new UnsignedLongAccessorFactoryFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::UnsignedLongAccessorFactory* uci_getUnsignedLongAccessorFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a UnsignedLongAccessorFactoryFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyUnsignedLongAccessorFactory(uci::factory::UnsignedLongAccessorFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy UnsignedLongAccessor. */
      class UnsignedLongAccessorFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~UnsignedLongAccessorFactory()
         { }

         typedef  uci::factory::UnsignedLongAccessorFactory FactoryType;
         typedef  uci::base::UnsignedLongAccessor ObjectType;

         //  // Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::UnsignedLongAccessorFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getUnsignedLongAccessorFactory( abstractServiceBusConnection );
         }

         //  // Static method to destroy a SomeType object
         static UCI_EXPORT void destroyFactory( uci::factory::UnsignedLongAccessorFactory* factory )
         {
            uci_destroyUnsignedLongAccessorFactory( factory );
         }

         /** This method constructs a new UnsignedLongAccessor.
           *
           * @return The newly contructed UnsignedLongAccessor.
           */
         virtual uci::base::UnsignedLongAccessor& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new UnsignedLongAccessor initializing the data accessed by the new UnsignedLongAccessor
           * using the data accessed by the specified UnsignedLongAccessor (cloning).
           *
           * @param accessor The UnsignedLongAccessor that provides access to the data that is to be used to initialize the data
           *      accessed by the new created UnsignedLongAccessor.
           * @return The newly contructed UnsignedLongAccessor.
           */
         virtual uci::base::UnsignedLongAccessor& create(const uci::base::UnsignedLongAccessor& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys the specified UnsignedLongAccessor. Use of the specified accessor after it has been destroyed
           * will result in undefined behavior.
           *
           * @param accessor The UnsignedLongAccessor that is to be destroyed.
           */
         virtual void destroy(uci::base::UnsignedLongAccessor& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         UnsignedLongAccessorFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The UnsignedLongAccessorFactory to copy from
           */
         UnsignedLongAccessorFactory(const UnsignedLongAccessorFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this UnsignedLongAccessorFactory to the contents of the
           * UnsignedLongAccessorFactory on the right hand side (rhs) of the assignment operator.UnsignedLongAccessorFactory [only
           * available to derived classes]
           *
           * @param rhs The UnsignedLongAccessorFactory on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this UnsignedLongAccessorFactory
           * @return A constant reference to this UnsignedLongAccessorFactory.
           */
         const UnsignedLongAccessorFactory& operator=(const UnsignedLongAccessorFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // UnsignedLongAccessorFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__UnsignedLongAccessorFactory_h

