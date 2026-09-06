// This file was generated  on 10/18/2017 at 5:35:32 AM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Factory__EOB_TimestampsTypeFactory_h
#define Uci__Factory__EOB_TimestampsTypeFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__EOB_TimestampsType_h)
# include "uci/type/EOB_TimestampsType.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class EOB_TimestampsTypeFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new EOB_TimestampsTypeFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::EOB_TimestampsTypeFactory* uci_getEOB_TimestampsTypeFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a EOB_TimestampsTypeFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyEOB_TimestampsTypeFactory(uci::factory::EOB_TimestampsTypeFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages */
      class EOB_TimestampsTypeFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~EOB_TimestampsTypeFactory()
         { }



         //  Typedefs to support template programming in services
         typedef  uci::factory::EOB_TimestampsTypeFactory FactoryType;
         typedef  uci::type::EOB_TimestampsType ObjectType;

         //  Static method to construct a new SomeType object
         static UCI_EXPORT uci::factory::EOB_TimestampsTypeFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getEOB_TimestampsTypeFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::EOB_TimestampsTypeFactory* factory )
         {
            uci_destroyEOB_TimestampsTypeFactory( factory );
         }
         /** This method constructs a new EOB_TimestampsType message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @return A message accessor that can be used to access the newly created EOB_TimestampsType message
           */
         virtual uci::type::EOB_TimestampsType& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new EOB_TimestampsType message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created EOB_TimestampsType message
           */
         virtual uci::type::EOB_TimestampsType& create(const uci::type::EOB_TimestampsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a EOB_TimestampsType message accessor, along with the message that the accessor was providing
           * access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::EOB_TimestampsType& accessor)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EOB_TimestampsTypeFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EOB_TimestampsTypeFactory to copy from
           */
         EOB_TimestampsTypeFactory(const EOB_TimestampsTypeFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EOB_TimestampsTypeFactory to the contents of the
           * EOB_TimestampsTypeFactory on the right hand side (rhs) of the assignment operator.EOB_TimestampsTypeFactory [only
           * available to derived classes]
           *
           * @param rhs The EOB_TimestampsTypeFactory on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::factory::EOB_TimestampsTypeFactory
           * @return A constant reference to this EOB_TimestampsTypeFactory.
           */
         const EOB_TimestampsTypeFactory& operator=(const EOB_TimestampsTypeFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EOB_TimestampsTypeFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__EOB_TimestampsTypeFactory_h

