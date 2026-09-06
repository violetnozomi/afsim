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
#ifndef Uci__Factory__StrikeCommandStatusFactory_h
#define Uci__Factory__StrikeCommandStatusFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__StrikeCommandStatus_h)
# include "uci/type/StrikeCommandStatus.h"
#endif

#if !defined(Uci__Reader__StrikeCommandStatusReader_h)
# include "uci/reader/StrikeCommandStatusReader.h"
#endif

#if !defined(Uci__Writer__StrikeCommandStatusWriter_h)
# include "uci/writer/StrikeCommandStatusWriter.h"
#endif



//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class StrikeCommandStatusFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new StrikeCommandStatusFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::StrikeCommandStatusFactory* uci_getStrikeCommandStatusFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a StrikeCommandStatusFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyStrikeCommandStatusFactory(uci::factory::StrikeCommandStatusFactory* factory) throw(uci::base::UCIException);


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
      class StrikeCommandStatusFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~StrikeCommandStatusFactory()
         { }

         typedef  uci::factory::StrikeCommandStatusFactory FactoryType;
         typedef  uci::type::StrikeCommandStatusMT MessageType;
         typedef  uci::reader::StrikeCommandStatusReader ReaderType;
         typedef  uci::reader::StrikeCommandStatusListener ListenerType;
         typedef  uci::writer::StrikeCommandStatusWriter WriterType;

         static UCI_EXPORT uci::factory::StrikeCommandStatusFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getStrikeCommandStatusFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::StrikeCommandStatusFactory* factory )
         {
            uci_destroyStrikeCommandStatusFactory( factory );
         }

         /** This method constructs a new StrikeCommandStatusMT message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @return A message accessor that can be used to access the newly created StrikeCommandStatusMT message
           */
         virtual uci::type::StrikeCommandStatus& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new StrikeCommandStatusMT message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created StrikeCommandStatusMT message
           */
         virtual uci::type::StrikeCommandStatus& create(const uci::type::StrikeCommandStatus& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a StrikeCommandStatusMT message accessor, along with the message that the accessor was providing
           * access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::StrikeCommandStatus& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new StrikeCommandStatusReader that can be used to read StrikeCommandStatusMT messages.
           *
           * @param topic The specification of the topic the message is to be read from
           * @return A message accessor that can be used to access the newly created StrikeCommandStatusMT message
           */
         virtual uci::reader::StrikeCommandStatusReader& createReader(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a StrikeCommandStatusReader that was created during the invocation of the createReader() method.
           * Once the reader is destroyed, it should never be used again.
           *
           * @param reader A reference to the reader to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyReader(uci::reader::StrikeCommandStatusReader& reader)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new StrikeCommandStatusWriter that can be used to write StrikeCommandStatusMT messages.
           *
           * @param topic The specification of the topict the message is to be written to
           * @return A message accessor that can be used to access the newly created StrikeCommandStatusMT message
           */
         virtual uci::writer::StrikeCommandStatusWriter& createWriter(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a StrikeCommandStatusWriter that was created during the invocation of the createWriter() method.
           * Once the writer is destroyed, it should never be used again.
           *
           * @param writer A reference to the writer to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyWriter(uci::writer::StrikeCommandStatusWriter& writer)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         StrikeCommandStatusFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The StrikeCommandStatusFactory to copy from
           */
         StrikeCommandStatusFactory(const StrikeCommandStatusFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this StrikeCommandStatusFactory to the contents of the
           * StrikeCommandStatusFactory on the right hand side (rhs) of the assignment operator.StrikeCommandStatusFactory [only
           * available to derived classes]
           *
           * @param rhs The StrikeCommandStatusFactory on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::factory::StrikeCommandStatusFactory
           * @return A constant reference to this StrikeCommandStatusFactory.
           */
         const StrikeCommandStatusFactory& operator=(const StrikeCommandStatusFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // StrikeCommandStatusFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__StrikeCommandStatusFactory_h

