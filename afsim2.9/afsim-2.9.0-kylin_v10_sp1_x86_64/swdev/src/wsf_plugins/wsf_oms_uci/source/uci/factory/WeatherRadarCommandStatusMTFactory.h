// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Factory__WeatherRadarCommandStatusMTFactory_h
#define Uci__Factory__WeatherRadarCommandStatusMTFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__WeatherRadarCommandStatusMT_h)
# include "uci/type/WeatherRadarCommandStatusMT.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class WeatherRadarCommandStatusMTFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new WeatherRadarCommandStatusMTFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::WeatherRadarCommandStatusMTFactory* uci_getWeatherRadarCommandStatusMTFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a WeatherRadarCommandStatusMTFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyWeatherRadarCommandStatusMTFactory(uci::factory::WeatherRadarCommandStatusMTFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class WeatherRadarCommandStatusMTFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~WeatherRadarCommandStatusMTFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  uci::factory::WeatherRadarCommandStatusMTFactory FactoryType;
         typedef  uci::type::WeatherRadarCommandStatusMT ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::WeatherRadarCommandStatusMTFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getWeatherRadarCommandStatusMTFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::WeatherRadarCommandStatusMTFactory* factory )
         {
            uci_destroyWeatherRadarCommandStatusMTFactory( factory );
         }

         /** This method constructs a new WeatherRadarCommandStatusMT message, returning an accessor to that message that can be
           * used to access that message.
           *
           * @return A message accessor that can be used to access the newly created WeatherRadarCommandStatusMT message
           */
         virtual uci::type::WeatherRadarCommandStatusMT& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new WeatherRadarCommandStatusMT message, returning an accessor to that message that can be
           * used to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created WeatherRadarCommandStatusMT message
           */
         virtual uci::type::WeatherRadarCommandStatusMT& create(const uci::type::WeatherRadarCommandStatusMT& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a WeatherRadarCommandStatusMT message accessor, along with the message that the accessor was
           * providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::WeatherRadarCommandStatusMT& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         WeatherRadarCommandStatusMTFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The WeatherRadarCommandStatusMTFactory to copy from
           */
         WeatherRadarCommandStatusMTFactory(const WeatherRadarCommandStatusMTFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this WeatherRadarCommandStatusMTFactory to the contents of the
           * WeatherRadarCommandStatusMTFactory on the right hand side (rhs) of the assignment
           * operator.WeatherRadarCommandStatusMTFactory [only available to derived classes]
           *
           * @param rhs The WeatherRadarCommandStatusMTFactory on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::factory::WeatherRadarCommandStatusMTFactory
           * @return A constant reference to this WeatherRadarCommandStatusMTFactory.
           */
         const WeatherRadarCommandStatusMTFactory& operator=(const WeatherRadarCommandStatusMTFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // WeatherRadarCommandStatusMTFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__WeatherRadarCommandStatusMTFactory_h

