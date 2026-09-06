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
#ifndef Uci__Factory__ProductDownloadRequestFactory_h
#define Uci__Factory__ProductDownloadRequestFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__ProductDownloadRequest_h)
# include "uci/type/ProductDownloadRequest.h"
#endif

#if !defined(Uci__Reader__ProductDownloadRequestReader_h)
# include "uci/reader/ProductDownloadRequestReader.h"
#endif

#if !defined(Uci__Writer__ProductDownloadRequestWriter_h)
# include "uci/writer/ProductDownloadRequestWriter.h"
#endif



//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class ProductDownloadRequestFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new ProductDownloadRequestFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::ProductDownloadRequestFactory* uci_getProductDownloadRequestFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a ProductDownloadRequestFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyProductDownloadRequestFactory(uci::factory::ProductDownloadRequestFactory* factory) throw(uci::base::UCIException);


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
      class ProductDownloadRequestFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~ProductDownloadRequestFactory()
         { }

         typedef  uci::factory::ProductDownloadRequestFactory FactoryType;
         typedef  uci::type::ProductDownloadRequestMT MessageType;
         typedef  uci::reader::ProductDownloadRequestReader ReaderType;
         typedef  uci::reader::ProductDownloadRequestListener ListenerType;
         typedef  uci::writer::ProductDownloadRequestWriter WriterType;

         static UCI_EXPORT uci::factory::ProductDownloadRequestFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getProductDownloadRequestFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::ProductDownloadRequestFactory* factory )
         {
            uci_destroyProductDownloadRequestFactory( factory );
         }

         /** This method constructs a new ProductDownloadRequestMT message, returning an accessor to that message that can be used
           * to access that message.
           *
           * @return A message accessor that can be used to access the newly created ProductDownloadRequestMT message
           */
         virtual uci::type::ProductDownloadRequest& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new ProductDownloadRequestMT message, returning an accessor to that message that can be used
           * to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created ProductDownloadRequestMT message
           */
         virtual uci::type::ProductDownloadRequest& create(const uci::type::ProductDownloadRequest& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a ProductDownloadRequestMT message accessor, along with the message that the accessor was
           * providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::ProductDownloadRequest& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new ProductDownloadRequestReader that can be used to read ProductDownloadRequestMT messages.
           *
           * @param topic The specification of the topic the message is to be read from
           * @return A message accessor that can be used to access the newly created ProductDownloadRequestMT message
           */
         virtual uci::reader::ProductDownloadRequestReader& createReader(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a ProductDownloadRequestReader that was created during the invocation of the createReader()
           * method. Once the reader is destroyed, it should never be used again.
           *
           * @param reader A reference to the reader to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyReader(uci::reader::ProductDownloadRequestReader& reader)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new ProductDownloadRequestWriter that can be used to write ProductDownloadRequestMT
           * messages.
           *
           * @param topic The specification of the topict the message is to be written to
           * @return A message accessor that can be used to access the newly created ProductDownloadRequestMT message
           */
         virtual uci::writer::ProductDownloadRequestWriter& createWriter(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a ProductDownloadRequestWriter that was created during the invocation of the createWriter()
           * method. Once the writer is destroyed, it should never be used again.
           *
           * @param writer A reference to the writer to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyWriter(uci::writer::ProductDownloadRequestWriter& writer)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductDownloadRequestFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductDownloadRequestFactory to copy from
           */
         ProductDownloadRequestFactory(const ProductDownloadRequestFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductDownloadRequestFactory to the contents of the
           * ProductDownloadRequestFactory on the right hand side (rhs) of the assignment operator.ProductDownloadRequestFactory
           * [only available to derived classes]
           *
           * @param rhs The ProductDownloadRequestFactory on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::factory::ProductDownloadRequestFactory
           * @return A constant reference to this ProductDownloadRequestFactory.
           */
         const ProductDownloadRequestFactory& operator=(const ProductDownloadRequestFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductDownloadRequestFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__ProductDownloadRequestFactory_h

