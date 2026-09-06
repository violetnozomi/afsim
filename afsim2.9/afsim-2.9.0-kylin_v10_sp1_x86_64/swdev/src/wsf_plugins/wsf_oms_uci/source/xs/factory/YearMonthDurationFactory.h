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
#ifndef Xs__Factory__YearMonthDurationFactory_h
#define Xs__Factory__YearMonthDurationFactory_h 1

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

      class YearMonthDurationFactory;
   } // Namespace: factory
} // Namespace: xs

#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new YearMonthDurationFactoryFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT xs::factory::YearMonthDurationFactory* xs_getYearMonthDurationFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a YearMonthDurationFactoryFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void xs_destroyYearMonthDurationFactory(xs::factory::YearMonthDurationFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all XML Schema Definition data types are declared
namespace xs {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy YearMonthDuration. */
      class YearMonthDurationFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~YearMonthDurationFactory()
         { }

         typedef  xs::factory::YearMonthDurationFactory FactoryType;
         typedef  xs::YearMonthDuration ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT xs::factory::YearMonthDurationFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return xs_getYearMonthDurationFactory( abstractServiceBusConnection );
         }

         //  Static method to destroy a SomeType object
         static UCI_EXPORT void destroyFactory( xs::factory::YearMonthDurationFactory* factory )
         {
            xs_destroyYearMonthDurationFactory( factory );
         }

         /** This method constructs a new YearMonthDuration.
           *
           * @return The newly contructed YearMonthDuration.
           */
         virtual xs::YearMonthDuration& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new YearMonthDuration initializing the data accessed by the new YearMonthDuration using the
           * data accessed by the specified YearMonthDuration (cloning).
           *
           * @param accessor The YearMonthDuration that provides access to the data that is to be used to initialize the data
           *      accessed by the new created YearMonthDuration.
           * @return The newly contructed YearMonthDuration.
           */
         virtual xs::YearMonthDuration& create(const xs::YearMonthDuration& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys the specified YearMonthDuration. Use of the specified accessor after it has been destroyed will
           * result in undefined behavior.
           *
           * @param accessor The YearMonthDuration that is to be destroyed.
           */
         virtual void destroy(xs::YearMonthDuration& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         YearMonthDurationFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The YearMonthDurationFactory to copy from
           */
         YearMonthDurationFactory(const YearMonthDurationFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this YearMonthDurationFactory to the contents of the
           * YearMonthDurationFactory on the right hand side (rhs) of the assignment operator.YearMonthDurationFactory [only
           * available to derived classes]
           *
           * @param rhs The YearMonthDurationFactory on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this YearMonthDurationFactory
           * @return A constant reference to this YearMonthDurationFactory.
           */
         const YearMonthDurationFactory& operator=(const YearMonthDurationFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // YearMonthDurationFactory


   } // Namespace: factory
} // Namespace: xs

#endif // Xs__Factory__YearMonthDurationFactory_h

