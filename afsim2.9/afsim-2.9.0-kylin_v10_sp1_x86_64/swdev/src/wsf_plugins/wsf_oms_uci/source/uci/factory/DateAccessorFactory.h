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
#ifndef Uci__Factory__DateAccessorFactory_h
#define Uci__Factory__DateAccessorFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Base__DateAccessor_h)
# include "uci/base/DateAccessor.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class DateAccessorFactory;
   } // Namespace: factory
} // Namespace: uci

#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new DateAccessorFactoryFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::DateAccessorFactory* uci_getDateAccessorFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a DateAccessorFactoryFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyDateAccessorFactory(uci::factory::DateAccessorFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy DateAccessor. */
      class DateAccessorFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~DateAccessorFactory()
         { }

         typedef  uci::factory::DateAccessorFactory FactoryType;
         typedef  uci::base::DateAccessor ObjectType;

         //  // Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::DateAccessorFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getDateAccessorFactory( abstractServiceBusConnection );
         }

         //  // Static method to destroy a SomeType object
         static UCI_EXPORT void destroyFactory( uci::factory::DateAccessorFactory* factory )
         {
            uci_destroyDateAccessorFactory( factory );
         }

         /** This method constructs a new DateAccessor.
           *
           * @return The newly contructed DateAccessor.
           */
         virtual uci::base::DateAccessor& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new DateAccessor initializing the data accessed by the new DateAccessor using the data
           * accessed by the specified DateAccessor (cloning).
           *
           * @param accessor The DateAccessor that provides access to the data that is to be used to initialize the data accessed
           *      by the new created DateAccessor.
           * @return The newly contructed DateAccessor.
           */
         virtual uci::base::DateAccessor& create(const uci::base::DateAccessor& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys the specified DateAccessor. Use of the specified accessor after it has been destroyed will
           * result in undefined behavior.
           *
           * @param accessor The DateAccessor that is to be destroyed.
           */
         virtual void destroy(uci::base::DateAccessor& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DateAccessorFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DateAccessorFactory to copy from
           */
         DateAccessorFactory(const DateAccessorFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DateAccessorFactory to the contents of the DateAccessorFactory on
           * the right hand side (rhs) of the assignment operator.DateAccessorFactory [only available to derived classes]
           *
           * @param rhs The DateAccessorFactory on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this DateAccessorFactory
           * @return A constant reference to this DateAccessorFactory.
           */
         const DateAccessorFactory& operator=(const DateAccessorFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DateAccessorFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__DateAccessorFactory_h

