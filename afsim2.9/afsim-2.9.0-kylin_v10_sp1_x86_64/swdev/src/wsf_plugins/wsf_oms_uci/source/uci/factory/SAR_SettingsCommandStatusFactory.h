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
#ifndef Uci__Factory__SAR_SettingsCommandStatusFactory_h
#define Uci__Factory__SAR_SettingsCommandStatusFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__SAR_SettingsCommandStatus_h)
# include "uci/type/SAR_SettingsCommandStatus.h"
#endif

#if !defined(Uci__Reader__SAR_SettingsCommandStatusReader_h)
# include "uci/reader/SAR_SettingsCommandStatusReader.h"
#endif

#if !defined(Uci__Writer__SAR_SettingsCommandStatusWriter_h)
# include "uci/writer/SAR_SettingsCommandStatusWriter.h"
#endif



//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class SAR_SettingsCommandStatusFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new SAR_SettingsCommandStatusFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::SAR_SettingsCommandStatusFactory* uci_getSAR_SettingsCommandStatusFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a SAR_SettingsCommandStatusFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroySAR_SettingsCommandStatusFactory(uci::factory::SAR_SettingsCommandStatusFactory* factory) throw(uci::base::UCIException);


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
      class SAR_SettingsCommandStatusFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~SAR_SettingsCommandStatusFactory()
         { }

         typedef  uci::factory::SAR_SettingsCommandStatusFactory FactoryType;
         typedef  uci::type::SAR_SettingsCommandStatusMT MessageType;
         typedef  uci::reader::SAR_SettingsCommandStatusReader ReaderType;
         typedef  uci::reader::SAR_SettingsCommandStatusListener ListenerType;
         typedef  uci::writer::SAR_SettingsCommandStatusWriter WriterType;

         static UCI_EXPORT uci::factory::SAR_SettingsCommandStatusFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getSAR_SettingsCommandStatusFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::SAR_SettingsCommandStatusFactory* factory )
         {
            uci_destroySAR_SettingsCommandStatusFactory( factory );
         }

         /** This method constructs a new SAR_SettingsCommandStatusMT message, returning an accessor to that message that can be
           * used to access that message.
           *
           * @return A message accessor that can be used to access the newly created SAR_SettingsCommandStatusMT message
           */
         virtual uci::type::SAR_SettingsCommandStatus& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new SAR_SettingsCommandStatusMT message, returning an accessor to that message that can be
           * used to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created SAR_SettingsCommandStatusMT message
           */
         virtual uci::type::SAR_SettingsCommandStatus& create(const uci::type::SAR_SettingsCommandStatus& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a SAR_SettingsCommandStatusMT message accessor, along with the message that the accessor was
           * providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::SAR_SettingsCommandStatus& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new SAR_SettingsCommandStatusReader that can be used to read SAR_SettingsCommandStatusMT
           * messages.
           *
           * @param topic The specification of the topic the message is to be read from
           * @return A message accessor that can be used to access the newly created SAR_SettingsCommandStatusMT message
           */
         virtual uci::reader::SAR_SettingsCommandStatusReader& createReader(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a SAR_SettingsCommandStatusReader that was created during the invocation of the createReader()
           * method. Once the reader is destroyed, it should never be used again.
           *
           * @param reader A reference to the reader to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyReader(uci::reader::SAR_SettingsCommandStatusReader& reader)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new SAR_SettingsCommandStatusWriter that can be used to write SAR_SettingsCommandStatusMT
           * messages.
           *
           * @param topic The specification of the topict the message is to be written to
           * @return A message accessor that can be used to access the newly created SAR_SettingsCommandStatusMT message
           */
         virtual uci::writer::SAR_SettingsCommandStatusWriter& createWriter(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a SAR_SettingsCommandStatusWriter that was created during the invocation of the createWriter()
           * method. Once the writer is destroyed, it should never be used again.
           *
           * @param writer A reference to the writer to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyWriter(uci::writer::SAR_SettingsCommandStatusWriter& writer)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SAR_SettingsCommandStatusFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SAR_SettingsCommandStatusFactory to copy from
           */
         SAR_SettingsCommandStatusFactory(const SAR_SettingsCommandStatusFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SAR_SettingsCommandStatusFactory to the contents of the
           * SAR_SettingsCommandStatusFactory on the right hand side (rhs) of the assignment
           * operator.SAR_SettingsCommandStatusFactory [only available to derived classes]
           *
           * @param rhs The SAR_SettingsCommandStatusFactory on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::factory::SAR_SettingsCommandStatusFactory
           * @return A constant reference to this SAR_SettingsCommandStatusFactory.
           */
         const SAR_SettingsCommandStatusFactory& operator=(const SAR_SettingsCommandStatusFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SAR_SettingsCommandStatusFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__SAR_SettingsCommandStatusFactory_h

