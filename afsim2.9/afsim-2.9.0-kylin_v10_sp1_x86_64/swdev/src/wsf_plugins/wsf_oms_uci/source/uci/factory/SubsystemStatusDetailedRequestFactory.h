// This file was generated  on 12/5/2018 at 1:1:51 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Factory__SubsystemStatusDetailedRequestFactory_h
#define Uci__Factory__SubsystemStatusDetailedRequestFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__SubsystemStatusDetailedRequest_h)
# include "uci/type/SubsystemStatusDetailedRequest.h"
#endif

#if !defined(Uci__Reader__SubsystemStatusDetailedRequestReader_h)
# include "uci/reader/SubsystemStatusDetailedRequestReader.h"
#endif

#if !defined(Uci__Writer__SubsystemStatusDetailedRequestWriter_h)
# include "uci/writer/SubsystemStatusDetailedRequestWriter.h"
#endif



//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class SubsystemStatusDetailedRequestFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new SubsystemStatusDetailedRequestFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::SubsystemStatusDetailedRequestFactory* uci_getSubsystemStatusDetailedRequestFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a SubsystemStatusDetailedRequestFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroySubsystemStatusDetailedRequestFactory(uci::factory::SubsystemStatusDetailedRequestFactory* factory) throw(uci::base::UCIException);


#ifdef __cplusplus
}
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      /** The Factory provides the support to create and destroy messages as well as to create readers that can read these
        * messages and writers that can write these messages
        */
      class SubsystemStatusDetailedRequestFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~SubsystemStatusDetailedRequestFactory()
         { }

         typedef  uci::factory::SubsystemStatusDetailedRequestFactory FactoryType;
         typedef  uci::type::SubsystemStatusDetailedRequestMT MessageType;
         typedef  uci::reader::SubsystemStatusDetailedRequestReader ReaderType;
         typedef  uci::reader::SubsystemStatusDetailedRequestListener ListenerType;
         typedef  uci::writer::SubsystemStatusDetailedRequestWriter WriterType;

         static UCI_EXPORT uci::factory::SubsystemStatusDetailedRequestFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getSubsystemStatusDetailedRequestFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::SubsystemStatusDetailedRequestFactory* factory )
         {
            uci_destroySubsystemStatusDetailedRequestFactory( factory );
         }

         /** This method constructs a new SubsystemStatusDetailedRequestMT message, returning an accessor to that message that can
           * be used to access that message.
           *
           * @return A message accessor that can be used to access the newly created SubsystemStatusDetailedRequestMT message
           */
         virtual uci::type::SubsystemStatusDetailedRequest& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new SubsystemStatusDetailedRequestMT message, returning an accessor to that message that can
           * be used to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created SubsystemStatusDetailedRequestMT message
           */
         virtual uci::type::SubsystemStatusDetailedRequest& create(const uci::type::SubsystemStatusDetailedRequest& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a SubsystemStatusDetailedRequestMT message accessor, along with the message that the accessor
           * was providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::SubsystemStatusDetailedRequest& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new SubsystemStatusDetailedRequestReader that can be used to read
           * SubsystemStatusDetailedRequestMT messages.
           *
           * @param topic The specification of the topic the message is to be read from
           * @return A message accessor that can be used to access the newly created SubsystemStatusDetailedRequestMT message
           */
         virtual uci::reader::SubsystemStatusDetailedRequestReader& createReader(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a SubsystemStatusDetailedRequestReader that was created during the invocation of the
           * createReader() method. Once the reader is destroyed, it should never be used again.
           *
           * @param reader A reference to the reader to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyReader(uci::reader::SubsystemStatusDetailedRequestReader& reader)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new SubsystemStatusDetailedRequestWriter that can be used to write
           * SubsystemStatusDetailedRequestMT messages.
           *
           * @param topic The specification of the topict the message is to be written to
           * @return A message accessor that can be used to access the newly created SubsystemStatusDetailedRequestMT message
           */
         virtual uci::writer::SubsystemStatusDetailedRequestWriter& createWriter(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a SubsystemStatusDetailedRequestWriter that was created during the invocation of the
           * createWriter() method. Once the writer is destroyed, it should never be used again.
           *
           * @param writer A reference to the writer to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyWriter(uci::writer::SubsystemStatusDetailedRequestWriter& writer)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SubsystemStatusDetailedRequestFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SubsystemStatusDetailedRequestFactory to copy from
           */
         SubsystemStatusDetailedRequestFactory(const SubsystemStatusDetailedRequestFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SubsystemStatusDetailedRequestFactory to the contents of the
           * SubsystemStatusDetailedRequestFactory on the right hand side (rhs) of the assignment
           * operator.SubsystemStatusDetailedRequestFactory [only available to derived classes]
           *
           * @param rhs The SubsystemStatusDetailedRequestFactory on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::factory::SubsystemStatusDetailedRequestFactory
           * @return A constant reference to this SubsystemStatusDetailedRequestFactory.
           */
         const SubsystemStatusDetailedRequestFactory& operator=(const SubsystemStatusDetailedRequestFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SubsystemStatusDetailedRequestFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__SubsystemStatusDetailedRequestFactory_h

