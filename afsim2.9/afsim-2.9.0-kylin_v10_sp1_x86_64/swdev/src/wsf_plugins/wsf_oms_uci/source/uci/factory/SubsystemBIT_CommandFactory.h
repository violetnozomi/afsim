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
#ifndef Uci__Factory__SubsystemBIT_CommandFactory_h
#define Uci__Factory__SubsystemBIT_CommandFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__SubsystemBIT_Command_h)
# include "uci/type/SubsystemBIT_Command.h"
#endif

#if !defined(Uci__Reader__SubsystemBIT_CommandReader_h)
# include "uci/reader/SubsystemBIT_CommandReader.h"
#endif

#if !defined(Uci__Writer__SubsystemBIT_CommandWriter_h)
# include "uci/writer/SubsystemBIT_CommandWriter.h"
#endif



//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class SubsystemBIT_CommandFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new SubsystemBIT_CommandFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::SubsystemBIT_CommandFactory* uci_getSubsystemBIT_CommandFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a SubsystemBIT_CommandFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroySubsystemBIT_CommandFactory(uci::factory::SubsystemBIT_CommandFactory* factory) throw(uci::base::UCIException);


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
      class SubsystemBIT_CommandFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~SubsystemBIT_CommandFactory()
         { }

         typedef  uci::factory::SubsystemBIT_CommandFactory FactoryType;
         typedef  uci::type::SubsystemBIT_CommandMT MessageType;
         typedef  uci::reader::SubsystemBIT_CommandReader ReaderType;
         typedef  uci::reader::SubsystemBIT_CommandListener ListenerType;
         typedef  uci::writer::SubsystemBIT_CommandWriter WriterType;

         static UCI_EXPORT uci::factory::SubsystemBIT_CommandFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getSubsystemBIT_CommandFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::SubsystemBIT_CommandFactory* factory )
         {
            uci_destroySubsystemBIT_CommandFactory( factory );
         }

         /** This method constructs a new SubsystemBIT_CommandMT message, returning an accessor to that message that can be used
           * to access that message.
           *
           * @return A message accessor that can be used to access the newly created SubsystemBIT_CommandMT message
           */
         virtual uci::type::SubsystemBIT_Command& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new SubsystemBIT_CommandMT message, returning an accessor to that message that can be used
           * to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created SubsystemBIT_CommandMT message
           */
         virtual uci::type::SubsystemBIT_Command& create(const uci::type::SubsystemBIT_Command& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a SubsystemBIT_CommandMT message accessor, along with the message that the accessor was
           * providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::SubsystemBIT_Command& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new SubsystemBIT_CommandReader that can be used to read SubsystemBIT_CommandMT messages.
           *
           * @param topic The specification of the topic the message is to be read from
           * @return A message accessor that can be used to access the newly created SubsystemBIT_CommandMT message
           */
         virtual uci::reader::SubsystemBIT_CommandReader& createReader(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a SubsystemBIT_CommandReader that was created during the invocation of the createReader()
           * method. Once the reader is destroyed, it should never be used again.
           *
           * @param reader A reference to the reader to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyReader(uci::reader::SubsystemBIT_CommandReader& reader)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new SubsystemBIT_CommandWriter that can be used to write SubsystemBIT_CommandMT messages.
           *
           * @param topic The specification of the topict the message is to be written to
           * @return A message accessor that can be used to access the newly created SubsystemBIT_CommandMT message
           */
         virtual uci::writer::SubsystemBIT_CommandWriter& createWriter(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a SubsystemBIT_CommandWriter that was created during the invocation of the createWriter()
           * method. Once the writer is destroyed, it should never be used again.
           *
           * @param writer A reference to the writer to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyWriter(uci::writer::SubsystemBIT_CommandWriter& writer)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SubsystemBIT_CommandFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SubsystemBIT_CommandFactory to copy from
           */
         SubsystemBIT_CommandFactory(const SubsystemBIT_CommandFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SubsystemBIT_CommandFactory to the contents of the
           * SubsystemBIT_CommandFactory on the right hand side (rhs) of the assignment operator.SubsystemBIT_CommandFactory [only
           * available to derived classes]
           *
           * @param rhs The SubsystemBIT_CommandFactory on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::factory::SubsystemBIT_CommandFactory
           * @return A constant reference to this SubsystemBIT_CommandFactory.
           */
         const SubsystemBIT_CommandFactory& operator=(const SubsystemBIT_CommandFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SubsystemBIT_CommandFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__SubsystemBIT_CommandFactory_h

