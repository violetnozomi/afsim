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
#ifndef Uci__Factory__MissionPlanActivationStateFactory_h
#define Uci__Factory__MissionPlanActivationStateFactory_h 1

#if !defined(Uci__Base__AbstractServiceBusConnection_h)
# include "uci/base/AbstractServiceBusConnection.h"
#endif

#if !defined(Uci__Base__Factory_h)
# include "uci/base/Factory.h"
#endif

#if !defined(Uci__Type__MissionPlanActivationState_h)
# include "uci/type/MissionPlanActivationState.h"
#endif

#if !defined(Uci__Reader__MissionPlanActivationStateReader_h)
# include "uci/reader/MissionPlanActivationStateReader.h"
#endif

#if !defined(Uci__Writer__MissionPlanActivationStateWriter_h)
# include "uci/writer/MissionPlanActivationStateWriter.h"
#endif



//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all factories are declared
   namespace factory {

      class MissionPlanActivationStateFactory;
   } // Namespace: factory
} // Namespace: uci


#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new MissionPlanActivationStateFactory
//  
//   @returns The newly created factory
//  
UCI_EXPORT uci::factory::MissionPlanActivationStateFactory* uci_getMissionPlanActivationStateFactory(uci::base::AbstractServiceBusConnection* abstractServiceBusConnection) throw(uci::base::UCIException);


//   Destroys a MissionPlanActivationStateFactory
//  
//   @param factory The factory to be destroyed.
//  
UCI_EXPORT void uci_destroyMissionPlanActivationStateFactory(uci::factory::MissionPlanActivationStateFactory* factory) throw(uci::base::UCIException);


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
      class MissionPlanActivationStateFactory : public virtual uci::base::Factory {
      public:

         /** The destructor */
         virtual ~MissionPlanActivationStateFactory()
         { }

         typedef  uci::factory::MissionPlanActivationStateFactory FactoryType;
         typedef  uci::type::MissionPlanActivationStateMT MessageType;
         typedef  uci::reader::MissionPlanActivationStateReader ReaderType;
         typedef  uci::reader::MissionPlanActivationStateListener ListenerType;
         typedef  uci::writer::MissionPlanActivationStateWriter WriterType;

         static UCI_EXPORT uci::factory::MissionPlanActivationStateFactory* getFactory( uci::base::AbstractServiceBusConnection* abstractServiceBusConnection )
         {
            return uci_getMissionPlanActivationStateFactory( abstractServiceBusConnection );
         }

         static UCI_EXPORT void destroyFactory( uci::factory::MissionPlanActivationStateFactory* factory )
         {
            uci_destroyMissionPlanActivationStateFactory( factory );
         }

         /** This method constructs a new MissionPlanActivationStateMT message, returning an accessor to that message that can be
           * used to access that message.
           *
           * @return A message accessor that can be used to access the newly created MissionPlanActivationStateMT message
           */
         virtual uci::type::MissionPlanActivationState& create()
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new MissionPlanActivationStateMT message, returning an accessor to that message that can be
           * used to access that message.
           *
           * @param accessor A message accessor that provides access to the message that is to be used to initialize the new
           *      message (basically cloned)
           * @return A message accessor that can be used to access the newly created MissionPlanActivationStateMT message
           */
         virtual uci::type::MissionPlanActivationState& create(const uci::type::MissionPlanActivationState& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a MissionPlanActivationStateMT message accessor, along with the message that the accessor was
           * providing access to, that was created using the createMessage() method
           *
           * @param accessor A message accessor returned by the constructMessage() method.
           */
         virtual void destroy(uci::type::MissionPlanActivationState& accessor)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new MissionPlanActivationStateReader that can be used to read MissionPlanActivationStateMT
           * messages.
           *
           * @param topic The specification of the topic the message is to be read from
           * @return A message accessor that can be used to access the newly created MissionPlanActivationStateMT message
           */
         virtual uci::reader::MissionPlanActivationStateReader& createReader(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a MissionPlanActivationStateReader that was created during the invocation of the createReader()
           * method. Once the reader is destroyed, it should never be used again.
           *
           * @param reader A reference to the reader to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyReader(uci::reader::MissionPlanActivationStateReader& reader)
            throw(uci::base::UCIException) = 0;


         /** This method constructs a new MissionPlanActivationStateWriter that can be used to write MissionPlanActivationStateMT
           * messages.
           *
           * @param topic The specification of the topict the message is to be written to
           * @return A message accessor that can be used to access the newly created MissionPlanActivationStateMT message
           */
         virtual uci::writer::MissionPlanActivationStateWriter& createWriter(const std::string& topic)
            throw(uci::base::UCIException) = 0;


         /** This method destroys a MissionPlanActivationStateWriter that was created during the invocation of the createWriter()
           * method. Once the writer is destroyed, it should never be used again.
           *
           * @param writer A reference to the writer to be destroyed. Once destroyed, the reference should never be used again
           */
         virtual void destroyWriter(uci::writer::MissionPlanActivationStateWriter& writer)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MissionPlanActivationStateFactory()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MissionPlanActivationStateFactory to copy from
           */
         MissionPlanActivationStateFactory(const MissionPlanActivationStateFactory& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MissionPlanActivationStateFactory to the contents of the
           * MissionPlanActivationStateFactory on the right hand side (rhs) of the assignment
           * operator.MissionPlanActivationStateFactory [only available to derived classes]
           *
           * @param rhs The MissionPlanActivationStateFactory on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::factory::MissionPlanActivationStateFactory
           * @return A constant reference to this MissionPlanActivationStateFactory.
           */
         const MissionPlanActivationStateFactory& operator=(const MissionPlanActivationStateFactory& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MissionPlanActivationStateFactory


   } // Namespace: factory
} // Namespace: uci

#endif // Uci__Factory__MissionPlanActivationStateFactory_h

