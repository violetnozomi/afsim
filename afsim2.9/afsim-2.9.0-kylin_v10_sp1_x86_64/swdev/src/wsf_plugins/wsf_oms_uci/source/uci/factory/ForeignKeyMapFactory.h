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
#ifndef Uci__Factory__ForeignKeyMapFactory_h
#define Uci__Factory__ForeignKeyMapFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__ForeignKeyMap_h)
# include "uci/type/ForeignKeyMap.h"
#endif

#if !defined(Uci__Reader__ForeignKeyMapReader_h)
# include "uci/reader/ForeignKeyMapReader.h"
#endif

#if !defined(Uci__Writer__ForeignKeyMapWriter_h)
# include "uci/writer/ForeignKeyMapWriter.h"
#endif



//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class ForeignKeyMapFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new ForeignKeyMapFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::ForeignKeyMapFactory* uci_getForeignKeyMapFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a ForeignKeyMapFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyForeignKeyMapFactory(uci::factory::ForeignKeyMapFactory* factory) throw(uci::base::UCIException);


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
      class ForeignKeyMapFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~ForeignKeyMapFactory()
         { }

         typedef  uci::factory::ForeignKeyMapFactory FactoryType;
         typedef  uci::type::ForeignKeyMapMT MessageType;
         typedef  uci::reader::ForeignKeyMapReader ReaderType;
         typedef  uci::reader::ForeignKeyMapListener ListenerType;
         typedef  uci::writer::ForeignKeyMapWriter WriterType;

         static UCI_EXPORT uci::factory::ForeignKeyMapFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getForeignKeyMapFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::ForeignKeyMapFactory* factory )
         {
            uci_destroyForeignKeyMapFactory( factory );
         }

         /** This method constructs a new ForeignKeyMapMT message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @return A message accessor that can be used to access the newly created ForeignKeyMapMT message
           */
         virtual uci::type::ForeignKeyMap& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new ForeignKeyMapMT message, returning an accessor to that message that can be used to
           * access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created ForeignKeyMapMT message
           */
         virtual uci::type::ForeignKeyMap& create(const uci::type::ForeignKeyMap& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a ForeignKeyMapMT message accessor, along with the message that the accessor was providing
           * access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::ForeignKeyMap& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new ForeignKeyMapReader that can be used to read ForeignKeyMapMT messages.
           *
           * @param topic The specification of the topic the message is to be read from
           * @return A message accessor that can be used to access the newly created ForeignKeyMapMT message
           */
         virtual uci::reader::ForeignKeyMapReader& createReader(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a ForeignKeyMapReader that was created during the invocation of the createReader() method. Once
           * the reader is destroyed, it should never be used again.
           *
           * @param reader A reference to the reader to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyReader(uci::reader::ForeignKeyMapReader& reader)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new ForeignKeyMapWriter that can be used to write ForeignKeyMapMT messages.
           *
           * @param topic The specification of the topict the message is to be written to
           * @return A message accessor that can be used to access the newly created ForeignKeyMapMT message
           */
         virtual uci::writer::ForeignKeyMapWriter& createWriter(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a ForeignKeyMapWriter that was created during the invocation of the createWriter() method. Once
           * the writer is destroyed, it should never be used again.
           *
           * @param writer A reference to the writer to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyWriter(uci::writer::ForeignKeyMapWriter& writer)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ForeignKeyMapFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ForeignKeyMapFactory to copy from
           */
         ForeignKeyMapFactory(const ForeignKeyMapFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ForeignKeyMapFactory to the contents of the ForeignKeyMapFactory
           * on the right hand side (rhs) of the assignment operator.ForeignKeyMapFactory [only available to derived classes]
           *
           * @param rhs The ForeignKeyMapFactory on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::factory::ForeignKeyMapFactory
           * @return A constant reference to this ForeignKeyMapFactory.
           */
         const ForeignKeyMapFactory& operator=(const ForeignKeyMapFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ForeignKeyMapFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__ForeignKeyMapFactory_h

