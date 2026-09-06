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
#ifndef Uci__Factory__CommTerminalCommandFactory_h
#define Uci__Factory__CommTerminalCommandFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__CommTerminalCommand_h)
# include "uci/type/CommTerminalCommand.h"
#endif

#if !defined(Uci__Reader__CommTerminalCommandReader_h)
# include "uci/reader/CommTerminalCommandReader.h"
#endif

#if !defined(Uci__Writer__CommTerminalCommandWriter_h)
# include "uci/writer/CommTerminalCommandWriter.h"
#endif



//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class CommTerminalCommandFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new CommTerminalCommandFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::CommTerminalCommandFactory* uci_getCommTerminalCommandFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a CommTerminalCommandFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyCommTerminalCommandFactory(uci::factory::CommTerminalCommandFactory* factory) throw(uci::base::UCIException);


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
      class CommTerminalCommandFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~CommTerminalCommandFactory()
         { }

         typedef  uci::factory::CommTerminalCommandFactory FactoryType;
         typedef  uci::type::CommTerminalCommandMT MessageType;
         typedef  uci::reader::CommTerminalCommandReader ReaderType;
         typedef  uci::reader::CommTerminalCommandListener ListenerType;
         typedef  uci::writer::CommTerminalCommandWriter WriterType;

         static UCI_EXPORT uci::factory::CommTerminalCommandFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getCommTerminalCommandFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::CommTerminalCommandFactory* factory )
         {
            uci_destroyCommTerminalCommandFactory( factory );
         }

         /** This method constructs a new CommTerminalCommandMT message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @return A message accessor that can be used to access the newly created CommTerminalCommandMT message
           */
         virtual uci::type::CommTerminalCommand& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new CommTerminalCommandMT message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created CommTerminalCommandMT message
           */
         virtual uci::type::CommTerminalCommand& create(const uci::type::CommTerminalCommand& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a CommTerminalCommandMT message accessor, along with the message that the accessor was providing
           * access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::CommTerminalCommand& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new CommTerminalCommandReader that can be used to read CommTerminalCommandMT messages.
           *
           * @param topic The specification of the topic the message is to be read from
           * @return A message accessor that can be used to access the newly created CommTerminalCommandMT message
           */
         virtual uci::reader::CommTerminalCommandReader& createReader(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a CommTerminalCommandReader that was created during the invocation of the createReader() method.
           * Once the reader is destroyed, it should never be used again.
           *
           * @param reader A reference to the reader to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyReader(uci::reader::CommTerminalCommandReader& reader)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new CommTerminalCommandWriter that can be used to write CommTerminalCommandMT messages.
           *
           * @param topic The specification of the topict the message is to be written to
           * @return A message accessor that can be used to access the newly created CommTerminalCommandMT message
           */
         virtual uci::writer::CommTerminalCommandWriter& createWriter(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a CommTerminalCommandWriter that was created during the invocation of the createWriter() method.
           * Once the writer is destroyed, it should never be used again.
           *
           * @param writer A reference to the writer to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyWriter(uci::writer::CommTerminalCommandWriter& writer)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommTerminalCommandFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommTerminalCommandFactory to copy from
           */
         CommTerminalCommandFactory(const CommTerminalCommandFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommTerminalCommandFactory to the contents of the
           * CommTerminalCommandFactory on the right hand side (rhs) of the assignment operator.CommTerminalCommandFactory [only
           * available to derived classes]
           *
           * @param rhs The CommTerminalCommandFactory on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::factory::CommTerminalCommandFactory
           * @return A constant reference to this CommTerminalCommandFactory.
           */
         const CommTerminalCommandFactory& operator=(const CommTerminalCommandFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommTerminalCommandFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__CommTerminalCommandFactory_h

