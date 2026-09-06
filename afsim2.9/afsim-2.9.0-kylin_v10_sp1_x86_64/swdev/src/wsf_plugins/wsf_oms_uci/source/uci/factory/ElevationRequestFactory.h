// This file was generated  on 12/5/2018 at 1:1:50 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Factory__ElevationRequestFactory_h
#define Uci__Factory__ElevationRequestFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__ElevationRequest_h)
# include "uci/type/ElevationRequest.h"
#endif

#if !defined(Uci__Reader__ElevationRequestReader_h)
# include "uci/reader/ElevationRequestReader.h"
#endif

#if !defined(Uci__Writer__ElevationRequestWriter_h)
# include "uci/writer/ElevationRequestWriter.h"
#endif



//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class ElevationRequestFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new ElevationRequestFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::ElevationRequestFactory* uci_getElevationRequestFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a ElevationRequestFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyElevationRequestFactory(uci::factory::ElevationRequestFactory* factory) throw(uci::base::UCIException);


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
      class ElevationRequestFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~ElevationRequestFactory()
         { }

         typedef  uci::factory::ElevationRequestFactory FactoryType;
         typedef  uci::type::ElevationRequestMT MessageType;
         typedef  uci::reader::ElevationRequestReader ReaderType;
         typedef  uci::reader::ElevationRequestListener ListenerType;
         typedef  uci::writer::ElevationRequestWriter WriterType;

         static UCI_EXPORT uci::factory::ElevationRequestFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getElevationRequestFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::ElevationRequestFactory* factory )
         {
            uci_destroyElevationRequestFactory( factory );
         }

         /** This method constructs a new ElevationRequestMT message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @return A message accessor that can be used to access the newly created ElevationRequestMT message
           */
         virtual uci::type::ElevationRequest& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new ElevationRequestMT message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created ElevationRequestMT message
           */
         virtual uci::type::ElevationRequest& create(const uci::type::ElevationRequest& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a ElevationRequestMT message accessor, along with the message that the accessor was providing
           * access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::ElevationRequest& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new ElevationRequestReader that can be used to read ElevationRequestMT messages.
           *
           * @param topic The specification of the topic the message is to be read from
           * @return A message accessor that can be used to access the newly created ElevationRequestMT message
           */
         virtual uci::reader::ElevationRequestReader& createReader(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a ElevationRequestReader that was created during the invocation of the createReader() method.
           * Once the reader is destroyed, it should never be used again.
           *
           * @param reader A reference to the reader to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyReader(uci::reader::ElevationRequestReader& reader)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new ElevationRequestWriter that can be used to write ElevationRequestMT messages.
           *
           * @param topic The specification of the topict the message is to be written to
           * @return A message accessor that can be used to access the newly created ElevationRequestMT message
           */
         virtual uci::writer::ElevationRequestWriter& createWriter(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a ElevationRequestWriter that was created during the invocation of the createWriter() method.
           * Once the writer is destroyed, it should never be used again.
           *
           * @param writer A reference to the writer to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyWriter(uci::writer::ElevationRequestWriter& writer)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ElevationRequestFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ElevationRequestFactory to copy from
           */
         ElevationRequestFactory(const ElevationRequestFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ElevationRequestFactory to the contents of the
           * ElevationRequestFactory on the right hand side (rhs) of the assignment operator.ElevationRequestFactory [only
           * available to derived classes]
           *
           * @param rhs The ElevationRequestFactory on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::factory::ElevationRequestFactory
           * @return A constant reference to this ElevationRequestFactory.
           */
         const ElevationRequestFactory& operator=(const ElevationRequestFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ElevationRequestFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__ElevationRequestFactory_h

