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
#ifndef Uci__Factory__AirSampleActivityFactory_h
#define Uci__Factory__AirSampleActivityFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__AirSampleActivity_h)
# include "uci/type/AirSampleActivity.h"
#endif

#if !defined(Uci__Reader__AirSampleActivityReader_h)
# include "uci/reader/AirSampleActivityReader.h"
#endif

#if !defined(Uci__Writer__AirSampleActivityWriter_h)
# include "uci/writer/AirSampleActivityWriter.h"
#endif



//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class AirSampleActivityFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new AirSampleActivityFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::AirSampleActivityFactory* uci_getAirSampleActivityFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a AirSampleActivityFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyAirSampleActivityFactory(uci::factory::AirSampleActivityFactory* factory) throw(uci::base::UCIException);


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
      class AirSampleActivityFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~AirSampleActivityFactory()
         { }

         typedef  uci::factory::AirSampleActivityFactory FactoryType;
         typedef  uci::type::AirSampleActivityMT MessageType;
         typedef  uci::reader::AirSampleActivityReader ReaderType;
         typedef  uci::reader::AirSampleActivityListener ListenerType;
         typedef  uci::writer::AirSampleActivityWriter WriterType;

         static UCI_EXPORT uci::factory::AirSampleActivityFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getAirSampleActivityFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::AirSampleActivityFactory* factory )
         {
            uci_destroyAirSampleActivityFactory( factory );
         }

         /** This method constructs a new AirSampleActivityMT message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @return A message accessor that can be used to access the newly created AirSampleActivityMT message
           */
         virtual uci::type::AirSampleActivity& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new AirSampleActivityMT message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created AirSampleActivityMT message
           */
         virtual uci::type::AirSampleActivity& create(const uci::type::AirSampleActivity& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a AirSampleActivityMT message accessor, along with the message that the accessor was providing
           * access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::AirSampleActivity& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new AirSampleActivityReader that can be used to read AirSampleActivityMT messages.
           *
           * @param topic The specification of the topic the message is to be read from
           * @return A message accessor that can be used to access the newly created AirSampleActivityMT message
           */
         virtual uci::reader::AirSampleActivityReader& createReader(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a AirSampleActivityReader that was created during the invocation of the createReader() method.
           * Once the reader is destroyed, it should never be used again.
           *
           * @param reader A reference to the reader to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyReader(uci::reader::AirSampleActivityReader& reader)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new AirSampleActivityWriter that can be used to write AirSampleActivityMT messages.
           *
           * @param topic The specification of the topict the message is to be written to
           * @return A message accessor that can be used to access the newly created AirSampleActivityMT message
           */
         virtual uci::writer::AirSampleActivityWriter& createWriter(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a AirSampleActivityWriter that was created during the invocation of the createWriter() method.
           * Once the writer is destroyed, it should never be used again.
           *
           * @param writer A reference to the writer to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyWriter(uci::writer::AirSampleActivityWriter& writer)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AirSampleActivityFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AirSampleActivityFactory to copy from
           */
         AirSampleActivityFactory(const AirSampleActivityFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AirSampleActivityFactory to the contents of the
           * AirSampleActivityFactory on the right hand side (rhs) of the assignment operator.AirSampleActivityFactory [only
           * available to derived classes]
           *
           * @param rhs The AirSampleActivityFactory on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::factory::AirSampleActivityFactory
           * @return A constant reference to this AirSampleActivityFactory.
           */
         const AirSampleActivityFactory& operator=(const AirSampleActivityFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AirSampleActivityFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__AirSampleActivityFactory_h

