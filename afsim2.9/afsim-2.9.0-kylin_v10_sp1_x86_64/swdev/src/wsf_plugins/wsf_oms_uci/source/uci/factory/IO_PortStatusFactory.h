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
#ifndef Uci__Factory__IO_PortStatusFactory_h
#define Uci__Factory__IO_PortStatusFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__IO_PortStatus_h)
# include "uci/type/IO_PortStatus.h"
#endif

#if !defined(Uci__Reader__IO_PortStatusReader_h)
# include "uci/reader/IO_PortStatusReader.h"
#endif

#if !defined(Uci__Writer__IO_PortStatusWriter_h)
# include "uci/writer/IO_PortStatusWriter.h"
#endif



//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class IO_PortStatusFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new IO_PortStatusFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::IO_PortStatusFactory* uci_getIO_PortStatusFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a IO_PortStatusFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyIO_PortStatusFactory(uci::factory::IO_PortStatusFactory* factory) throw(uci::base::UCIException);


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
      class IO_PortStatusFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~IO_PortStatusFactory()
         { }

         typedef  uci::factory::IO_PortStatusFactory FactoryType;
         typedef  uci::type::IO_PortStatusMT MessageType;
         typedef  uci::reader::IO_PortStatusReader ReaderType;
         typedef  uci::reader::IO_PortStatusListener ListenerType;
         typedef  uci::writer::IO_PortStatusWriter WriterType;

         static UCI_EXPORT uci::factory::IO_PortStatusFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getIO_PortStatusFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::IO_PortStatusFactory* factory )
         {
            uci_destroyIO_PortStatusFactory( factory );
         }

         /** This method constructs a new IO_PortStatusMT message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @return A message accessor that can be used to access the newly created IO_PortStatusMT message
           */
         virtual uci::type::IO_PortStatus& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new IO_PortStatusMT message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created IO_PortStatusMT message
           */
         virtual uci::type::IO_PortStatus& create(const uci::type::IO_PortStatus& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a IO_PortStatusMT message accessor, along with the message that the accessor was providing
           * access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::IO_PortStatus& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new IO_PortStatusReader that can be used to read IO_PortStatusMT messages.
           *
           * @param topic The specification of the topic the message is to be read from
           * @return A message accessor that can be used to access the newly created IO_PortStatusMT message
           */
         virtual uci::reader::IO_PortStatusReader& createReader(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a IO_PortStatusReader that was created during the invocation of the createReader() method. Once
           * the reader is destroyed, it should never be used again.
           *
           * @param reader A reference to the reader to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyReader(uci::reader::IO_PortStatusReader& reader)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new IO_PortStatusWriter that can be used to write IO_PortStatusMT messages.
           *
           * @param topic The specification of the topict the message is to be written to
           * @return A message accessor that can be used to access the newly created IO_PortStatusMT message
           */
         virtual uci::writer::IO_PortStatusWriter& createWriter(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a IO_PortStatusWriter that was created during the invocation of the createWriter() method. Once
           * the writer is destroyed, it should never be used again.
           *
           * @param writer A reference to the writer to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyWriter(uci::writer::IO_PortStatusWriter& writer)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         IO_PortStatusFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The IO_PortStatusFactory to copy from
           */
         IO_PortStatusFactory(const IO_PortStatusFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this IO_PortStatusFactory to the contents of the IO_PortStatusFactory
           * on the right hand side (rhs) of the assignment operator.IO_PortStatusFactory [only available to derived classes]
           *
           * @param rhs The IO_PortStatusFactory on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::factory::IO_PortStatusFactory
           * @return A constant reference to this IO_PortStatusFactory.
           */
         const IO_PortStatusFactory& operator=(const IO_PortStatusFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // IO_PortStatusFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__IO_PortStatusFactory_h

