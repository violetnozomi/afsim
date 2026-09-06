// This file was generated  on 10/18/2017 at 5:35:43 AM by the Boeing OMS CAL generation tools
// @warning  This file was automatically generated, edit at your own risk

/**
* Unclassified               U N C L A S S I F I E D               Unclassified
*
* DEVELOPMENT:
*    This document wholly developed with USG funds.
*    For additional information, contact the AFRCO.
*
* DISTRIBUTION STATEMENT D.  Distribution authorized to the Department 
*    of Defense and U.S. DoD contractors only; Critical Technology; 26 Mar 13.
*    Other requests shall be referred to the Air Force Rapid Capabilities 
*    Office, 202-404-5497 or 202-404-5498.
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
#ifndef Uci__Factory__ThreatAssessmentDetectionTypeFactory_h
#define Uci__Factory__ThreatAssessmentDetectionTypeFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__ThreatAssessmentDetectionType_h)
# include "uci/type/ThreatAssessmentDetectionType.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class ThreatAssessmentDetectionTypeFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new ThreatAssessmentDetectionTypeFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::ThreatAssessmentDetectionTypeFactory* uci_getThreatAssessmentDetectionTypeFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a ThreatAssessmentDetectionTypeFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyThreatAssessmentDetectionTypeFactory(uci::factory::ThreatAssessmentDetectionTypeFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class ThreatAssessmentDetectionTypeFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~ThreatAssessmentDetectionTypeFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  uci::factory::ThreatAssessmentDetectionTypeFactory FactoryType;
         typedef  uci::type::ThreatAssessmentDetectionType ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::ThreatAssessmentDetectionTypeFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getThreatAssessmentDetectionTypeFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::ThreatAssessmentDetectionTypeFactory* factory )
         {
            uci_destroyThreatAssessmentDetectionTypeFactory( factory );
         }
         /** This method constructs a new ThreatAssessmentDetectionType message, returning an accessor to that message that can be
           * used to access that message.
           *
           * @return A message accessor that can be used to access the newly created ThreatAssessmentDetectionType message
           */
         virtual uci::type::ThreatAssessmentDetectionType& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new ThreatAssessmentDetectionType message, returning an accessor to that message that can be
           * used to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created ThreatAssessmentDetectionType message
           */
         virtual uci::type::ThreatAssessmentDetectionType& create(const uci::type::ThreatAssessmentDetectionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a ThreatAssessmentDetectionType message accessor, along with the message that the accessor was
           * providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::ThreatAssessmentDetectionType& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ThreatAssessmentDetectionTypeFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ThreatAssessmentDetectionTypeFactory to copy from
           */
         ThreatAssessmentDetectionTypeFactory(const ThreatAssessmentDetectionTypeFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ThreatAssessmentDetectionTypeFactory to the contents of the
           * ThreatAssessmentDetectionTypeFactory on the right hand side (rhs) of the assignment
           * operator.ThreatAssessmentDetectionTypeFactory [only available to derived classes]
           *
           * @param rhs The ThreatAssessmentDetectionTypeFactory on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::factory::ThreatAssessmentDetectionTypeFactory
           * @return A constant reference to this ThreatAssessmentDetectionTypeFactory.
           */
         const ThreatAssessmentDetectionTypeFactory& operator=(const ThreatAssessmentDetectionTypeFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ThreatAssessmentDetectionTypeFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__ThreatAssessmentDetectionTypeFactory_h

