// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFAIRCOMBATEVENTPIPE_HPP
#define WSFAIRCOMBATEVENTPIPE_HPP

#include "WsfAirCombatEventPipeClasses.hpp" // Note: This auto-generated file must come after WsfEventPipe.hpp
#include "WsfEventPipe.hpp"
#include "WsfEventPipeClasses.hpp"
#include "WsfSA_Processor.hpp"

namespace wsf
{
namespace AirCombat
{
/**
 * This is the class that is responsible for logging all Air Combat related
 * events.
 */
class EventPipe : public WsfEventPipeLogger
{
public:
   /** Event pipe constructor. It assigns the internal simulation pointer to the
    * simulation referenced by aEventPipeInterfacePtr, assigns its internal
    * interface pointer to aEventPipeInterfacePtr, and sets cAIRCOMBAT to its
    * event ID from aEventPipeInterfacePtr.
    * @param aEventPipeInterfacePtr  the event pipe interface associated with
    *    the simulation.
    */
   explicit EventPipe(WsfEventPipeInterface* aEventPipeInterfacePtr);

   /** This registers the AIRCOMBAT option with aEventPipeExtension, adds its schema,
    * adds EventPipe::RegisterMessages to aEventPipeExtension's callback list.
    * @param aEventPipeExtension  The event pipe extension to which the events are
    *    registered.
    */
   static void RegisterEvents(WsfEventPipeExtension& aEventPipeExtension);

   /** Calls a function called UtPack_register_all_wsf_air_combat_events_types.
    * That function appears to be auto-generated. It seems to add a couple struct
    * definitions to aSerializePtr.
    * @param aSerializerPtr  The serializer to which the messages are registered.
    */
   static void RegisterMessages(UtPackSerializer* aSerializerPtr);

   /** This should be called whenever an event pipe option is changed. It adds this
    * plugin's observer callbacks to the EventPipe's internal callback list if
    * the AIRCOMBAT option was enabled, or removes them otherwise.
    * @param aNewCombinedOptions       The new set of options.
    * @param aPreviousCombinedOptions  The old set of options.
    */
   void UpdateSubscriptions(const WsfEventPipeOptions& aNewCombinedOptions,
                            const WsfEventPipeOptions& aPreviousCombinedOptions) override;

private:
   /** This creates a MsgSA_FlightKinematicsData, populates its data members with values from aPlatform and aProcessor.
    * @param aSimTime    The current time in the simulation.
    * @param aPlatform   The message details the perspective of this platform.
    * @param aProcessor  The processor that is providing the info sent in the message.
    */
   void AirCombatSendFlightKinematicsData(double aSimTime, const WsfPlatform& aPlatform, const WsfSA_Processor& aProcessor);

   /** This creates a MsgSA_FuelData, populates its data members with values from aPlatform and aProcessor.
    * @param aSimTime    The current time in the simulation.
    * @param aPlatform   The message details the perspective of this platform.
    * @param aProcessor  The processor that is providing the info sent in the message.
    */
   void AirCombatSendFuelData(double aSimTime, const WsfPlatform& aPlatform, const WsfSA_Processor& aProcessor);

   /** This creates a MsgSA_NavData, populates its data members with values from aPlatform and aProcessor.
    * @param aSimTime    The current time in the simulation.
    * @param aPlatform   The message details the perspective of this platform.
    * @param aProcessor  The processor that is providing the info sent in the message.
    */
   void AirCombatSendNavData(double aSimTime, const WsfPlatform& aPlatform, const WsfSA_Processor& aProcessor);

   /** This creates a MsgSA_FlightControlsData, populates its data members with values from aPlatform and aProcessor.
    * @param aSimTime    The current time in the simulation.
    * @param aPlatform   The message details the perspective of this platform.
    * @param aProcessor  The processor that is providing the info sent in the message.
    */
   void AirCombatSendFlightControlsData(double aSimTime, const WsfPlatform& aPlatform, const WsfSA_Processor& aProcessor);

   /** This creates a MsgSA_WeaponsData, populates its data members with values from aPlatform and aProcessor.
    * @param aSimTime    The current time in the simulation.
    * @param aPlatform   The message details the perspective of this platform.
    * @param aProcessor  The processor that is providing the info sent in the message.
    */
   void AirCombatSendWeaponsData(double aSimTime, const WsfPlatform& aPlatform, const WsfSA_Processor& aProcessor);

   /** This creates a MsgSA_TrackData, populates its data members with values from aPlatform and aProcessor.
    * @param aSimTime    The current time in the simulation.
    * @param aPlatform   The message details the perspective of this platform.
    * @param aProcessor  The processor that is providing the info sent in the message.
    */
   void AirCombatSendTrackData(double aSimTime, const WsfPlatform& aPlatform, const WsfSA_Processor& aProcessor);

   /** This creates a MsgSA_EngagementSummaryData, populates its data members with
    * values from aPlatform and aProcessor. Then, it 'sends' the message (which
    * I assume ultimately means writing it to the .aer file).
    * @param aSimTime    The current time in the simulation.
    * @param aPlatform   The message details the perspective of this platform.
    * @param aProcessor  The processor that is providing the info sent in the message.
    * @param aMsgType    The message type to compare with "MsgAirCombatEngagementData"
    *    to see if it's the right type.
    */
   void AirCombatSendEngagementSummaryData(double aSimTime, const WsfPlatform& aPlatform, const WsfSA_Processor& aProcessor);

   /** This creates a MsgSA_PerceivedAssetsData, populates its data members with values from aPlatform and aProcessor.
    * @param aSimTime    The current time in the simulation.
    * @param aPlatform   The message details the perspective of this platform.
    * @param aProcessor  The processor that is providing the info sent in the message.
    */
   void AirCombatSendPerceivedAssetsData(double aSimTime, const WsfPlatform& aPlatform, const WsfSA_Processor& aProcessor);

   /** This creates a MsgSA_PerceivedBogiesAndBanditsData and a MsgSA_GroupsData, and populates data members with values
    * from aPlatform and aProcessor.
    * @param aSimTime    The current time in the simulation.
    * @param aPlatform   The message details the perspective of this platform.
    * @param aProcessor  The processor that is providing the info sent in the message.
    */
   void AirCombatSendPerceivedItemsData(double aSimTime, const WsfPlatform& aPlatform, const WsfSA_Processor& aProcessor);

   /** This creates a MsgSA_PerceivedBogiesAndBanditsData, populates its data members with values from aPlatform and
    * aProcessor.
    * @param aSimTime    The current time in the simulation.
    * @param aPlatform   The message details the perspective of this platform.
    * @param aProcessor  The processor that is providing the info sent in the message.
    */
   void AirCombatSendPerceivedBogiesAndBanditsData(double                 aSimTime,
                                                   const WsfPlatform&     aPlatform,
                                                   const WsfSA_Processor& aProcessor);

   /** This creates a MsgSA_GroupsData, populates its data members with values from aPlatform and aProcessor.
    * @param aSimTime    The current time in the simulation.
    * @param aPlatform   The message details the perspective of this platform.
    * @param aProcessor  The processor that is providing the info sent in the message.
    */
   void AirCombatSendGroupsData(double aSimTime, const WsfPlatform& aPlatform, const WsfSA_Processor& aProcessor);

   /** This creates a MsgSA_PrioritizedThreatsAndTargetsData, populates its data members with values from aPlatform and
    * aProcessor.
    * @param aSimTime    The current time in the simulation.
    * @param aPlatform   The message details the perspective of this platform.
    * @param aProcessor  The processor that is providing the info sent in the message.
    */
   void AirCombatSendPrioritizedThreatsAndTargetsData(double                 aSimTime,
                                                      const WsfPlatform&     aPlatform,
                                                      const WsfSA_Processor& aProcessor);

   /** Pass-through to the event pipe interface's Send method.
    * @param aSimTime     The current time in the simulation.
    * @param aMessagePtr  The message to send.
    */
   void Send(double aSimTime, std::unique_ptr<WsfEventPipe::MsgBase> aMessagePtr)
   {
      mEventPipePtr->SendImmediate(aSimTime, false, std::move(aMessagePtr));
   }

   /** Helper function for populating the values in an entity.
    * @param aMessageEntity    The event pipe entity to populate.
    * @param aPerceivedEntity  The source entity for the data.
    */
   void PopulateEntity(WsfEventPipe::SA_EntityPerception& aMessageEntity, const WsfSA_EntityPerception* aPerceivedEntity);

   /** A pointer to the associated simulation.*/
   WsfSimulation* mSimulationPtr;

   /** A pointer to the WsfEventPipeInterface. Note the distinction from AirCombat::EventPipeInterface. */
   WsfEventPipeInterface* mEventPipePtr;

   /** A list of callbacks. It should either be empty, or include EventPipe::AirCombatSendEngagementSummaryData
    * and EventPipe::AirCombatAddPlatform. */
   UtCallbackHolder mCallbacks;

   /** An index for the AIRCOMBAT option.*/
   int cAIRCOMBAT;
};

/** Note that the interface is a WsfSimulationExtension. It is not a WsfEventPipeInterface. */
class EventPipeInterface : public WsfSimulationExtension
{
public:
   /**
    * The EventPipeInterface constructor. All it does is set its internal pointer
    * to an EventPipe to nullptr.
    */
   EventPipeInterface();
   ~EventPipeInterface() override = default;

   /** Get the EventPipeInterface associated with aSimulation.*/
   static EventPipeInterface& Get(const WsfSimulation& aSimulation);

   /**
    * Called when the extension is added to the simulation. This creates the
    * AirCombat::EventPipe and adds it as a logger to a WsfEventPipeInterface
    * found in the simulation.
    */
   void AddedToSimulation() override;

private:
   /** A pointer to the associated AirCombat::EventPipe.*/
   std::unique_ptr<EventPipe> mEventPipePtr;
};
} // namespace AirCombat
} // namespace wsf

#endif // !WSFAIRCOMBATEVENTPIPE_HPP
