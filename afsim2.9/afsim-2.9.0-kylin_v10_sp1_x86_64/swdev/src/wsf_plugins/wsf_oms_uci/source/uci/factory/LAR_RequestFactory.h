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
#ifndef Uci__Factory__LAR_RequestFactory_h
#define Uci__Factory__LAR_RequestFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__LAR_Request_h)
# include "uci/type/LAR_Request.h"
#endif

#if !defined(Uci__Reader__LAR_RequestReader_h)
# include "uci/reader/LAR_RequestReader.h"
#endif

#if !defined(Uci__Writer__LAR_RequestWriter_h)
# include "uci/writer/LAR_RequestWriter.h"
#endif



//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class LAR_RequestFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new LAR_RequestFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::LAR_RequestFactory* uci_getLAR_RequestFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a LAR_RequestFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyLAR_RequestFactory(uci::factory::LAR_RequestFactory* factory) throw(uci::base::UCIException);


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
      class LAR_RequestFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~LAR_RequestFactory()
         { }

         typedef  uci::factory::LAR_RequestFactory FactoryType;
         typedef  uci::type::LAR_RequestMT MessageType;
         typedef  uci::reader::LAR_RequestReader ReaderType;
         typedef  uci::reader::LAR_RequestListener ListenerType;
         typedef  uci::writer::LAR_RequestWriter WriterType;

         static UCI_EXPORT uci::factory::LAR_RequestFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getLAR_RequestFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::LAR_RequestFactory* factory )
         {
            uci_destroyLAR_RequestFactory( factory );
         }

         /** This method constructs a new LAR_RequestMT message, returning an accessor to that message that can be used to access
           * that message.
           *
           * @return A message accessor that can be used to access the newly created LAR_RequestMT message
           */
         virtual uci::type::LAR_Request& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new LAR_RequestMT message, returning an accessor to that message that can be used to access
           * that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created LAR_RequestMT message
           */
         virtual uci::type::LAR_Request& create(const uci::type::LAR_Request& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a LAR_RequestMT message accessor, along with the message that the accessor was providing access
           * to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::LAR_Request& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new LAR_RequestReader that can be used to read LAR_RequestMT messages.
           *
           * @param topic The specification of the topic the message is to be read from
           * @return A message accessor that can be used to access the newly created LAR_RequestMT message
           */
         virtual uci::reader::LAR_RequestReader& createReader(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a LAR_RequestReader that was created during the invocation of the createReader() method. Once
           * the reader is destroyed, it should never be used again.
           *
           * @param reader A reference to the reader to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyReader(uci::reader::LAR_RequestReader& reader)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new LAR_RequestWriter that can be used to write LAR_RequestMT messages.
           *
           * @param topic The specification of the topict the message is to be written to
           * @return A message accessor that can be used to access the newly created LAR_RequestMT message
           */
         virtual uci::writer::LAR_RequestWriter& createWriter(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a LAR_RequestWriter that was created during the invocation of the createWriter() method. Once
           * the writer is destroyed, it should never be used again.
           *
           * @param writer A reference to the writer to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyWriter(uci::writer::LAR_RequestWriter& writer)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         LAR_RequestFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The LAR_RequestFactory to copy from
           */
         LAR_RequestFactory(const LAR_RequestFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this LAR_RequestFactory to the contents of the LAR_RequestFactory on
           * the right hand side (rhs) of the assignment operator.LAR_RequestFactory [only available to derived classes]
           *
           * @param rhs The LAR_RequestFactory on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::factory::LAR_RequestFactory
           * @return A constant reference to this LAR_RequestFactory.
           */
         const LAR_RequestFactory& operator=(const LAR_RequestFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // LAR_RequestFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__LAR_RequestFactory_h

