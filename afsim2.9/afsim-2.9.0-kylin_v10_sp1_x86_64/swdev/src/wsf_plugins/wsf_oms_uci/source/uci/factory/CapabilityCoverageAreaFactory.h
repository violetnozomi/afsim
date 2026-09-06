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
#ifndef Uci__Factory__CapabilityCoverageAreaFactory_h
#define Uci__Factory__CapabilityCoverageAreaFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__CapabilityCoverageArea_h)
# include "uci/type/CapabilityCoverageArea.h"
#endif

#if !defined(Uci__Reader__CapabilityCoverageAreaReader_h)
# include "uci/reader/CapabilityCoverageAreaReader.h"
#endif

#if !defined(Uci__Writer__CapabilityCoverageAreaWriter_h)
# include "uci/writer/CapabilityCoverageAreaWriter.h"
#endif



//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class CapabilityCoverageAreaFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new CapabilityCoverageAreaFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::CapabilityCoverageAreaFactory* uci_getCapabilityCoverageAreaFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a CapabilityCoverageAreaFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyCapabilityCoverageAreaFactory(uci::factory::CapabilityCoverageAreaFactory* factory) throw(uci::base::UCIException);


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
      class CapabilityCoverageAreaFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~CapabilityCoverageAreaFactory()
         { }

         typedef  uci::factory::CapabilityCoverageAreaFactory FactoryType;
         typedef  uci::type::CapabilityCoverageAreaMT MessageType;
         typedef  uci::reader::CapabilityCoverageAreaReader ReaderType;
         typedef  uci::reader::CapabilityCoverageAreaListener ListenerType;
         typedef  uci::writer::CapabilityCoverageAreaWriter WriterType;

         static UCI_EXPORT uci::factory::CapabilityCoverageAreaFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getCapabilityCoverageAreaFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::CapabilityCoverageAreaFactory* factory )
         {
            uci_destroyCapabilityCoverageAreaFactory( factory );
         }

         /** This method constructs a new CapabilityCoverageAreaMT message, returning an accessor to that message that can be used
           * to access that message.
           *
           * @return A message accessor that can be used to access the newly created CapabilityCoverageAreaMT message
           */
         virtual uci::type::CapabilityCoverageArea& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new CapabilityCoverageAreaMT message, returning an accessor to that message that can be used
           * to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created CapabilityCoverageAreaMT message
           */
         virtual uci::type::CapabilityCoverageArea& create(const uci::type::CapabilityCoverageArea& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a CapabilityCoverageAreaMT message accessor, along with the message that the accessor was
           * providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::CapabilityCoverageArea& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new CapabilityCoverageAreaReader that can be used to read CapabilityCoverageAreaMT messages.
           *
           * @param topic The specification of the topic the message is to be read from
           * @return A message accessor that can be used to access the newly created CapabilityCoverageAreaMT message
           */
         virtual uci::reader::CapabilityCoverageAreaReader& createReader(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a CapabilityCoverageAreaReader that was created during the invocation of the createReader()
           * method. Once the reader is destroyed, it should never be used again.
           *
           * @param reader A reference to the reader to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyReader(uci::reader::CapabilityCoverageAreaReader& reader)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new CapabilityCoverageAreaWriter that can be used to write CapabilityCoverageAreaMT
           * messages.
           *
           * @param topic The specification of the topict the message is to be written to
           * @return A message accessor that can be used to access the newly created CapabilityCoverageAreaMT message
           */
         virtual uci::writer::CapabilityCoverageAreaWriter& createWriter(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a CapabilityCoverageAreaWriter that was created during the invocation of the createWriter()
           * method. Once the writer is destroyed, it should never be used again.
           *
           * @param writer A reference to the writer to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyWriter(uci::writer::CapabilityCoverageAreaWriter& writer)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CapabilityCoverageAreaFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CapabilityCoverageAreaFactory to copy from
           */
         CapabilityCoverageAreaFactory(const CapabilityCoverageAreaFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CapabilityCoverageAreaFactory to the contents of the
           * CapabilityCoverageAreaFactory on the right hand side (rhs) of the assignment operator.CapabilityCoverageAreaFactory
           * [only available to derived classes]
           *
           * @param rhs The CapabilityCoverageAreaFactory on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::factory::CapabilityCoverageAreaFactory
           * @return A constant reference to this CapabilityCoverageAreaFactory.
           */
         const CapabilityCoverageAreaFactory& operator=(const CapabilityCoverageAreaFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CapabilityCoverageAreaFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__CapabilityCoverageAreaFactory_h

