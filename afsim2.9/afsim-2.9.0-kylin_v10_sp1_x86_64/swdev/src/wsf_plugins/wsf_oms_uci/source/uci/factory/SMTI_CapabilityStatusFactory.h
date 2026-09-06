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
#ifndef Uci__Factory__SMTI_CapabilityStatusFactory_h
#define Uci__Factory__SMTI_CapabilityStatusFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__SMTI_CapabilityStatus_h)
# include "uci/type/SMTI_CapabilityStatus.h"
#endif

#if !defined(Uci__Reader__SMTI_CapabilityStatusReader_h)
# include "uci/reader/SMTI_CapabilityStatusReader.h"
#endif

#if !defined(Uci__Writer__SMTI_CapabilityStatusWriter_h)
# include "uci/writer/SMTI_CapabilityStatusWriter.h"
#endif



//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class SMTI_CapabilityStatusFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new SMTI_CapabilityStatusFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::SMTI_CapabilityStatusFactory* uci_getSMTI_CapabilityStatusFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a SMTI_CapabilityStatusFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroySMTI_CapabilityStatusFactory(uci::factory::SMTI_CapabilityStatusFactory* factory) throw(uci::base::UCIException);


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
      class SMTI_CapabilityStatusFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~SMTI_CapabilityStatusFactory()
         { }

         typedef  uci::factory::SMTI_CapabilityStatusFactory FactoryType;
         typedef  uci::type::SMTI_CapabilityStatusMT MessageType;
         typedef  uci::reader::SMTI_CapabilityStatusReader ReaderType;
         typedef  uci::reader::SMTI_CapabilityStatusListener ListenerType;
         typedef  uci::writer::SMTI_CapabilityStatusWriter WriterType;

         static UCI_EXPORT uci::factory::SMTI_CapabilityStatusFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getSMTI_CapabilityStatusFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::SMTI_CapabilityStatusFactory* factory )
         {
            uci_destroySMTI_CapabilityStatusFactory( factory );
         }

         /** This method constructs a new SMTI_CapabilityStatusMT message, returning an accessor to that message that can be used
           * to access that message.
           *
           * @return A message accessor that can be used to access the newly created SMTI_CapabilityStatusMT message
           */
         virtual uci::type::SMTI_CapabilityStatus& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new SMTI_CapabilityStatusMT message, returning an accessor to that message that can be used
           * to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created SMTI_CapabilityStatusMT message
           */
         virtual uci::type::SMTI_CapabilityStatus& create(const uci::type::SMTI_CapabilityStatus& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a SMTI_CapabilityStatusMT message accessor, along with the message that the accessor was
           * providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::SMTI_CapabilityStatus& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new SMTI_CapabilityStatusReader that can be used to read SMTI_CapabilityStatusMT messages.
           *
           * @param topic The specification of the topic the message is to be read from
           * @return A message accessor that can be used to access the newly created SMTI_CapabilityStatusMT message
           */
         virtual uci::reader::SMTI_CapabilityStatusReader& createReader(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a SMTI_CapabilityStatusReader that was created during the invocation of the createReader()
           * method. Once the reader is destroyed, it should never be used again.
           *
           * @param reader A reference to the reader to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyReader(uci::reader::SMTI_CapabilityStatusReader& reader)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new SMTI_CapabilityStatusWriter that can be used to write SMTI_CapabilityStatusMT messages.
           *
           * @param topic The specification of the topict the message is to be written to
           * @return A message accessor that can be used to access the newly created SMTI_CapabilityStatusMT message
           */
         virtual uci::writer::SMTI_CapabilityStatusWriter& createWriter(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a SMTI_CapabilityStatusWriter that was created during the invocation of the createWriter()
           * method. Once the writer is destroyed, it should never be used again.
           *
           * @param writer A reference to the writer to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyWriter(uci::writer::SMTI_CapabilityStatusWriter& writer)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SMTI_CapabilityStatusFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SMTI_CapabilityStatusFactory to copy from
           */
         SMTI_CapabilityStatusFactory(const SMTI_CapabilityStatusFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SMTI_CapabilityStatusFactory to the contents of the
           * SMTI_CapabilityStatusFactory on the right hand side (rhs) of the assignment operator.SMTI_CapabilityStatusFactory
           * [only available to derived classes]
           *
           * @param rhs The SMTI_CapabilityStatusFactory on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::factory::SMTI_CapabilityStatusFactory
           * @return A constant reference to this SMTI_CapabilityStatusFactory.
           */
         const SMTI_CapabilityStatusFactory& operator=(const SMTI_CapabilityStatusFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SMTI_CapabilityStatusFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__SMTI_CapabilityStatusFactory_h

