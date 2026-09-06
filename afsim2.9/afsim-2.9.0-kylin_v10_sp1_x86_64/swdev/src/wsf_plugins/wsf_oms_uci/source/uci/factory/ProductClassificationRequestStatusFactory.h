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
#ifndef Uci__Factory__ProductClassificationRequestStatusFactory_h
#define Uci__Factory__ProductClassificationRequestStatusFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__ProductClassificationRequestStatus_h)
# include "uci/type/ProductClassificationRequestStatus.h"
#endif

#if !defined(Uci__Reader__ProductClassificationRequestStatusReader_h)
# include "uci/reader/ProductClassificationRequestStatusReader.h"
#endif

#if !defined(Uci__Writer__ProductClassificationRequestStatusWriter_h)
# include "uci/writer/ProductClassificationRequestStatusWriter.h"
#endif



//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class ProductClassificationRequestStatusFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new ProductClassificationRequestStatusFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::ProductClassificationRequestStatusFactory* uci_getProductClassificationRequestStatusFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a ProductClassificationRequestStatusFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyProductClassificationRequestStatusFactory(uci::factory::ProductClassificationRequestStatusFactory* factory) throw(uci::base::UCIException);


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
      class ProductClassificationRequestStatusFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~ProductClassificationRequestStatusFactory()
         { }

         typedef  uci::factory::ProductClassificationRequestStatusFactory FactoryType;
         typedef  uci::type::ProductClassificationRequestStatusMT MessageType;
         typedef  uci::reader::ProductClassificationRequestStatusReader ReaderType;
         typedef  uci::reader::ProductClassificationRequestStatusListener ListenerType;
         typedef  uci::writer::ProductClassificationRequestStatusWriter WriterType;

         static UCI_EXPORT uci::factory::ProductClassificationRequestStatusFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getProductClassificationRequestStatusFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::ProductClassificationRequestStatusFactory* factory )
         {
            uci_destroyProductClassificationRequestStatusFactory( factory );
         }

         /** This method constructs a new ProductClassificationRequestStatusMT message, returning an accessor to that message that
           * can be used to access that message.
           *
           * @return A message accessor that can be used to access the newly created ProductClassificationRequestStatusMT message
           */
         virtual uci::type::ProductClassificationRequestStatus& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new ProductClassificationRequestStatusMT message, returning an accessor to that message that
           * can be used to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created ProductClassificationRequestStatusMT message
           */
         virtual uci::type::ProductClassificationRequestStatus& create(const uci::type::ProductClassificationRequestStatus& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a ProductClassificationRequestStatusMT message accessor, along with the message that the
           * accessor was providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::ProductClassificationRequestStatus& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new ProductClassificationRequestStatusReader that can be used to read
           * ProductClassificationRequestStatusMT messages.
           *
           * @param topic The specification of the topic the message is to be read from
           * @return A message accessor that can be used to access the newly created ProductClassificationRequestStatusMT message
           */
         virtual uci::reader::ProductClassificationRequestStatusReader& createReader(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a ProductClassificationRequestStatusReader that was created during the invocation of the
           * createReader() method. Once the reader is destroyed, it should never be used again.
           *
           * @param reader A reference to the reader to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyReader(uci::reader::ProductClassificationRequestStatusReader& reader)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new ProductClassificationRequestStatusWriter that can be used to write
           * ProductClassificationRequestStatusMT messages.
           *
           * @param topic The specification of the topict the message is to be written to
           * @return A message accessor that can be used to access the newly created ProductClassificationRequestStatusMT message
           */
         virtual uci::writer::ProductClassificationRequestStatusWriter& createWriter(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a ProductClassificationRequestStatusWriter that was created during the invocation of the
           * createWriter() method. Once the writer is destroyed, it should never be used again.
           *
           * @param writer A reference to the writer to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyWriter(uci::writer::ProductClassificationRequestStatusWriter& writer)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductClassificationRequestStatusFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductClassificationRequestStatusFactory to copy from
           */
         ProductClassificationRequestStatusFactory(const ProductClassificationRequestStatusFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductClassificationRequestStatusFactory to the contents of the
           * ProductClassificationRequestStatusFactory on the right hand side (rhs) of the assignment
           * operator.ProductClassificationRequestStatusFactory [only available to derived classes]
           *
           * @param rhs The ProductClassificationRequestStatusFactory on the right hand side (rhs) of the assignment operator
           *      whose contents are used to set the contents of this uci::factory::ProductClassificationRequestStatusFactory
           * @return A constant reference to this ProductClassificationRequestStatusFactory.
           */
         const ProductClassificationRequestStatusFactory& operator=(const ProductClassificationRequestStatusFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductClassificationRequestStatusFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__ProductClassificationRequestStatusFactory_h

