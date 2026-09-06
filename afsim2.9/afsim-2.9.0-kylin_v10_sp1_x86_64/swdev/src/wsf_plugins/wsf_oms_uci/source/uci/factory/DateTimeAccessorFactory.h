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
#ifndef Uci__Factory__DateTimeAccessorFactory_h
#define Uci__Factory__DateTimeAccessorFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Base__DateTimeAccessor_h)
# include "uci/base/DateTimeAccessor.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class DateTimeAccessorFactory;
   } // Namespace: factory
} // Namespace: uci

#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new DateTimeAccessorFactoryFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::DateTimeAccessorFactory* uci_getDateTimeAccessorFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a DateTimeAccessorFactoryFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyDateTimeAccessorFactory(uci::factory::DateTimeAccessorFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy DateTimeAccessor. */
      class DateTimeAccessorFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~DateTimeAccessorFactory()
         { }

         typedef  uci::factory::DateTimeAccessorFactory FactoryType;
         typedef  uci::base::DateTimeAccessor ObjectType;

         //  // Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::DateTimeAccessorFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getDateTimeAccessorFactory( abstractServiceBusConnection );
         }

         //  // Static method to destroy a SomeType object
         static UCI_EXPORT void destroyFactory( uci::factory::DateTimeAccessorFactory* factory )
         {
            uci_destroyDateTimeAccessorFactory( factory );
         }

         /** This method constructs a new DateTimeAccessor.
           *
           * @return The newly contructed DateTimeAccessor.
           */
         virtual uci::base::DateTimeAccessor& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new DateTimeAccessor initializing the data accessed by the new DateTimeAccessor using the
           * data accessed by the specified DateTimeAccessor (cloning).
           *
           * @param accessor The DateTimeAccessor that provides access to the data that is to be used to initialize the data
           *      accessed by the new created DateTimeAccessor.
           * @return The newly contructed DateTimeAccessor.
           */
         virtual uci::base::DateTimeAccessor& create(const uci::base::DateTimeAccessor& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys the specified DateTimeAccessor. Use of the specified accessor after it has been destroyed will
           * result in undefined behavior.
           *
           * @param accessor The DateTimeAccessor that is to be destroyed.
           */
         virtual void destroy(uci::base::DateTimeAccessor& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DateTimeAccessorFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DateTimeAccessorFactory to copy from
           */
         DateTimeAccessorFactory(const DateTimeAccessorFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DateTimeAccessorFactory to the contents of the
           * DateTimeAccessorFactory on the right hand side (rhs) of the assignment operator.DateTimeAccessorFactory [only
           * available to derived classes]
           *
           * @param rhs The DateTimeAccessorFactory on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this DateTimeAccessorFactory
           * @return A constant reference to this DateTimeAccessorFactory.
           */
         const DateTimeAccessorFactory& operator=(const DateTimeAccessorFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DateTimeAccessorFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__DateTimeAccessorFactory_h

