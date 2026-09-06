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
#ifndef Uci__Factory__PO_CommandStatusFactory_h
#define Uci__Factory__PO_CommandStatusFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__PO_CommandStatus_h)
# include "uci/type/PO_CommandStatus.h"
#endif

#if !defined(Uci__Reader__PO_CommandStatusReader_h)
# include "uci/reader/PO_CommandStatusReader.h"
#endif

#if !defined(Uci__Writer__PO_CommandStatusWriter_h)
# include "uci/writer/PO_CommandStatusWriter.h"
#endif



//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class PO_CommandStatusFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new PO_CommandStatusFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::PO_CommandStatusFactory* uci_getPO_CommandStatusFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a PO_CommandStatusFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyPO_CommandStatusFactory(uci::factory::PO_CommandStatusFactory* factory) throw(uci::base::UCIException);


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
      class PO_CommandStatusFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~PO_CommandStatusFactory()
         { }

         typedef  uci::factory::PO_CommandStatusFactory FactoryType;
         typedef  uci::type::PO_CommandStatusMT MessageType;
         typedef  uci::reader::PO_CommandStatusReader ReaderType;
         typedef  uci::reader::PO_CommandStatusListener ListenerType;
         typedef  uci::writer::PO_CommandStatusWriter WriterType;

         static UCI_EXPORT uci::factory::PO_CommandStatusFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getPO_CommandStatusFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::PO_CommandStatusFactory* factory )
         {
            uci_destroyPO_CommandStatusFactory( factory );
         }

         /** This method constructs a new PO_CommandStatusMT message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @return A message accessor that can be used to access the newly created PO_CommandStatusMT message
           */
         virtual uci::type::PO_CommandStatus& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new PO_CommandStatusMT message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created PO_CommandStatusMT message
           */
         virtual uci::type::PO_CommandStatus& create(const uci::type::PO_CommandStatus& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a PO_CommandStatusMT message accessor, along with the message that the accessor was providing
           * access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::PO_CommandStatus& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new PO_CommandStatusReader that can be used to read PO_CommandStatusMT messages.
           *
           * @param topic The specification of the topic the message is to be read from
           * @return A message accessor that can be used to access the newly created PO_CommandStatusMT message
           */
         virtual uci::reader::PO_CommandStatusReader& createReader(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a PO_CommandStatusReader that was created during the invocation of the createReader() method.
           * Once the reader is destroyed, it should never be used again.
           *
           * @param reader A reference to the reader to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyReader(uci::reader::PO_CommandStatusReader& reader)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new PO_CommandStatusWriter that can be used to write PO_CommandStatusMT messages.
           *
           * @param topic The specification of the topict the message is to be written to
           * @return A message accessor that can be used to access the newly created PO_CommandStatusMT message
           */
         virtual uci::writer::PO_CommandStatusWriter& createWriter(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a PO_CommandStatusWriter that was created during the invocation of the createWriter() method.
           * Once the writer is destroyed, it should never be used again.
           *
           * @param writer A reference to the writer to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyWriter(uci::writer::PO_CommandStatusWriter& writer)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PO_CommandStatusFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PO_CommandStatusFactory to copy from
           */
         PO_CommandStatusFactory(const PO_CommandStatusFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PO_CommandStatusFactory to the contents of the
           * PO_CommandStatusFactory on the right hand side (rhs) of the assignment operator.PO_CommandStatusFactory [only
           * available to derived classes]
           *
           * @param rhs The PO_CommandStatusFactory on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::factory::PO_CommandStatusFactory
           * @return A constant reference to this PO_CommandStatusFactory.
           */
         const PO_CommandStatusFactory& operator=(const PO_CommandStatusFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PO_CommandStatusFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__PO_CommandStatusFactory_h

