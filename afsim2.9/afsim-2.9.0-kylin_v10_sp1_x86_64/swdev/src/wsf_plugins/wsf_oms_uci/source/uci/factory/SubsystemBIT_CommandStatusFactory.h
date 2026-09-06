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
#ifndef Uci__Factory__SubsystemBIT_CommandStatusFactory_h
#define Uci__Factory__SubsystemBIT_CommandStatusFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__SubsystemBIT_CommandStatus_h)
# include "uci/type/SubsystemBIT_CommandStatus.h"
#endif

#if !defined(Uci__Reader__SubsystemBIT_CommandStatusReader_h)
# include "uci/reader/SubsystemBIT_CommandStatusReader.h"
#endif

#if !defined(Uci__Writer__SubsystemBIT_CommandStatusWriter_h)
# include "uci/writer/SubsystemBIT_CommandStatusWriter.h"
#endif



//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class SubsystemBIT_CommandStatusFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new SubsystemBIT_CommandStatusFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::SubsystemBIT_CommandStatusFactory* uci_getSubsystemBIT_CommandStatusFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a SubsystemBIT_CommandStatusFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroySubsystemBIT_CommandStatusFactory(uci::factory::SubsystemBIT_CommandStatusFactory* factory) throw(uci::base::UCIException);


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
      class SubsystemBIT_CommandStatusFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~SubsystemBIT_CommandStatusFactory()
         { }

         typedef  uci::factory::SubsystemBIT_CommandStatusFactory FactoryType;
         typedef  uci::type::SubsystemBIT_CommandStatusMT MessageType;
         typedef  uci::reader::SubsystemBIT_CommandStatusReader ReaderType;
         typedef  uci::reader::SubsystemBIT_CommandStatusListener ListenerType;
         typedef  uci::writer::SubsystemBIT_CommandStatusWriter WriterType;

         static UCI_EXPORT uci::factory::SubsystemBIT_CommandStatusFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getSubsystemBIT_CommandStatusFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::SubsystemBIT_CommandStatusFactory* factory )
         {
            uci_destroySubsystemBIT_CommandStatusFactory( factory );
         }

         /** This method constructs a new SubsystemBIT_CommandStatusMT message, returning an accessor to that message that can be
           * used to access that message.
           *
           * @return A message accessor that can be used to access the newly created SubsystemBIT_CommandStatusMT message
           */
         virtual uci::type::SubsystemBIT_CommandStatus& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new SubsystemBIT_CommandStatusMT message, returning an accessor to that message that can be
           * used to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created SubsystemBIT_CommandStatusMT message
           */
         virtual uci::type::SubsystemBIT_CommandStatus& create(const uci::type::SubsystemBIT_CommandStatus& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a SubsystemBIT_CommandStatusMT message accessor, along with the message that the accessor was
           * providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::SubsystemBIT_CommandStatus& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new SubsystemBIT_CommandStatusReader that can be used to read SubsystemBIT_CommandStatusMT
           * messages.
           *
           * @param topic The specification of the topic the message is to be read from
           * @return A message accessor that can be used to access the newly created SubsystemBIT_CommandStatusMT message
           */
         virtual uci::reader::SubsystemBIT_CommandStatusReader& createReader(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a SubsystemBIT_CommandStatusReader that was created during the invocation of the createReader()
           * method. Once the reader is destroyed, it should never be used again.
           *
           * @param reader A reference to the reader to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyReader(uci::reader::SubsystemBIT_CommandStatusReader& reader)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new SubsystemBIT_CommandStatusWriter that can be used to write SubsystemBIT_CommandStatusMT
           * messages.
           *
           * @param topic The specification of the topict the message is to be written to
           * @return A message accessor that can be used to access the newly created SubsystemBIT_CommandStatusMT message
           */
         virtual uci::writer::SubsystemBIT_CommandStatusWriter& createWriter(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a SubsystemBIT_CommandStatusWriter that was created during the invocation of the createWriter()
           * method. Once the writer is destroyed, it should never be used again.
           *
           * @param writer A reference to the writer to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyWriter(uci::writer::SubsystemBIT_CommandStatusWriter& writer)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SubsystemBIT_CommandStatusFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SubsystemBIT_CommandStatusFactory to copy from
           */
         SubsystemBIT_CommandStatusFactory(const SubsystemBIT_CommandStatusFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SubsystemBIT_CommandStatusFactory to the contents of the
           * SubsystemBIT_CommandStatusFactory on the right hand side (rhs) of the assignment
           * operator.SubsystemBIT_CommandStatusFactory [only available to derived classes]
           *
           * @param rhs The SubsystemBIT_CommandStatusFactory on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::factory::SubsystemBIT_CommandStatusFactory
           * @return A constant reference to this SubsystemBIT_CommandStatusFactory.
           */
         const SubsystemBIT_CommandStatusFactory& operator=(const SubsystemBIT_CommandStatusFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SubsystemBIT_CommandStatusFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__SubsystemBIT_CommandStatusFactory_h

