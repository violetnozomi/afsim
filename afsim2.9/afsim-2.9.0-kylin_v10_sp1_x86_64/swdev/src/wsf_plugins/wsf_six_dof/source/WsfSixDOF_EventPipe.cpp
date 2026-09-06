// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSixDOF_EventPipe.hpp"

#include "UtMemory.hpp"
#include "UtOrbitalElements.hpp"
#include "UtPackReflect.hpp"
#include "WsfEventPipeClasses.hpp"
#include "WsfPointMassSixDOF_Mover.hpp"
#include "WsfRigidBodySixDOF_Mover.hpp"
#include "WsfSimulation.hpp"
#include "WsfSixDOF_EventPipeClasses.hpp"
#include "WsfSixDOF_EventPipeClassesRegister.hpp"
#include "WsfSixDOF_EventPipeSchema.hpp"
#include "WsfSixDOF_Observer.hpp"
#include "maneuvers/WsfSixDOF_Maneuver.hpp"

/** Register the event pipe events for SixDOF data. */
void wsf::six_dof::EventPipe::RegisterEvents(WsfEventPipeExtension& aEventPipeExtension)
{
   aEventPipeExtension.AddSchema(cWsfSixDOF_EventPipe_SCHEMA);
   aEventPipeExtension.RegisterEventOption("SixDOF", false);

   // Register the callback that gets invoked by WsfEventPipeInterface::AddedToSimulation to register
   // our message types with the serializer.
   aEventPipeExtension.AddCallback(aEventPipeExtension.RegisterExtensionMessages.Connect(&EventPipe::RegisterMessages));
}

/** Register the messages with the serializer. */
void wsf::six_dof::EventPipe::RegisterMessages(UtPackSerializer* aSerializerPtr)
{
   WsfEventPipe::UtPack_register_all_wsf_six_dof_events_types(*aSerializerPtr);
}

wsf::six_dof::EventPipe::EventPipe(WsfEventPipeInterface* aEventPipeInterfacePtr)
   : mSimulationPtr(&aEventPipeInterfacePtr->GetSimulation())
   , mEventPipePtr(aEventPipeInterfacePtr)
{
   cSixDOF = aEventPipeInterfacePtr->GetEventId("SixDOF");
}

/** Update callback subscriptions. */
void wsf::six_dof::EventPipe::UpdateSubscriptions(const WsfEventPipeOptions& aNewCombinedOptions,
                                                  const WsfEventPipeOptions& aPreviousCombinedOptions)
{
   if (aNewCombinedOptions.IsOptionEnabled(cSixDOF))
   {
      if (!aPreviousCombinedOptions.IsOptionEnabled(cSixDOF))
      {
         mSixDOF_Callback += WsfObserver::SixDOF_SendMessage(mSimulationPtr).Connect(&EventPipe::SixDOF_SendMessage, this);
      }
   }
   else if (aPreviousCombinedOptions.IsOptionEnabled(cSixDOF))
   {
      mSixDOF_Callback.Clear();
   }
}

/** Callback to send a common_air message through the event pipe. */
void wsf::six_dof::EventPipe::SixDOF_SendMessage(double             aSimTime,
                                                 const WsfPlatform& aPlatform,
                                                 Mover&             aMover,
                                                 const std::string& aMsgType)
{
   if (aMsgType == "MsgSixDOF_CoreData")
   {
      auto coreDataMsg = ut::make_unique<WsfEventPipe::MsgSixDOF_CoreData>();
      coreDataMsg->platformIndex(WsfEventPipeInterface::Platform(aPlatform));
      coreDataMsg->altitude_m(aMover.GetAlt_m());
      coreDataMsg->vertSpeed_msec(aMover.GetVerticalSpeed());
      coreDataMsg->KCAS(aMover.GetSpeed_KCAS());
      coreDataMsg->KTAS(aMover.GetSpeed_KTAS());
      coreDataMsg->mach(aMover.GetSpeed_Mach());
      coreDataMsg->heading_rad(aMover.GetHeading_rad());
      coreDataMsg->pitch_rad(aMover.GetPitch_rad());
      coreDataMsg->roll_rad(aMover.GetRoll_rad());
      coreDataMsg->gLoad(aMover.GetGLoad());
      coreDataMsg->alpha_rad(aMover.GetAlpha_rad());
      coreDataMsg->gAvail(aMover.GetPotentialManeuverGLoadMax());

      Send(aSimTime, aPlatform, std::move(coreDataMsg));
   }
   else if (aMsgType == "MsgSixDOF_Kinematic")
   {
      auto kinematicMsg = ut::make_unique<WsfEventPipe::MsgSixDOF_Kinematic>();
      kinematicMsg->platformIndex(WsfEventPipeInterface::Platform(aPlatform));
      kinematicMsg->beta_rad(aMover.GetBeta_rad());
      kinematicMsg->alphaDot_rps(aMover.GetAlphaDot_rps());
      kinematicMsg->betaDot_rps(aMover.GetBetaDot_rps());
      kinematicMsg->yawRate_rps(aMover.GetYawRate());
      kinematicMsg->pitchRate_rps(aMover.GetPitchRate());
      kinematicMsg->rollRate_rps(aMover.GetRollRate());
      kinematicMsg->dynamicPressure_psf(aMover.GetDynamicPressure_psf());
      kinematicMsg->nx(aMover.GetNx_g());
      kinematicMsg->ny(aMover.GetNy_g());
      kinematicMsg->flightPathAngle_rad(aMover.GetFlightPathAngle_rad());
      Send(aSimTime, aPlatform, std::move(kinematicMsg));
   }
   else if (aMsgType == "MsgSixDOF_EngineFuel")
   {
      auto engineFuelMsg = ut::make_unique<WsfEventPipe::MsgSixDOF_EngineFuel>();
      engineFuelMsg->platformIndex(WsfEventPipeInterface::Platform(aPlatform));
      engineFuelMsg->fuelFlow_kgsec(aMover.GetTotalFuelFlow());
      engineFuelMsg->fuelInternal_kg(aMover.GetInternalFuelCapacity());
      engineFuelMsg->fuelExternal_kg(aMover.GetExternalFuelCapacity());
      engineFuelMsg->fuelInternalRemaining_kg(aMover.GetInternalFuelRemaining());
      engineFuelMsg->fuelExternalRemaining_kg(aMover.GetExternalFuelRemaining());
      engineFuelMsg->totalWeight_kg(aMover.GetTotalWeight_kg());
      engineFuelMsg->afterburner_on(aMover.GetAfterburnerOn());
      engineFuelMsg->producing_thrust(aMover.IsProducingThrust());
      engineFuelMsg->engine_operating(aMover.GetEngineIsOperating());
      engineFuelMsg->engine_smoking(aMover.GetEngineIsSmoking());
      engineFuelMsg->contrailing(aMover.GetContrailTrailingEffect());
      engineFuelMsg->jokerFuelState_kg(aMover.GetJokerFuelState());
      engineFuelMsg->bingoFuelState_kg(aMover.GetBingoFuelState());
      Send(aSimTime, aPlatform, std::move(engineFuelMsg));
   }
   else if (aMsgType == "MsgSixDOF_Autopilot")
   {
      auto autopilotMsg = ut::make_unique<WsfEventPipe::MsgSixDOF_Autopilot>();
      autopilotMsg->platformIndex(WsfEventPipeInterface::Platform(aPlatform));
      autopilotMsg->activePilot(aMover.GetActivePilot());
      autopilotMsg->lateralModeName(aMover.GetAutopilotLateralMode());
      autopilotMsg->lateralModeValue(aMover.GetAutopilotLateralModeValue());
      autopilotMsg->verticalModeName(aMover.GetAutopilotVerticalMode());
      autopilotMsg->verticalModeValue(aMover.GetAutopilotVerticalModeValue());
      autopilotMsg->speedModeName(aMover.GetAutopilotSpeedMode());
      autopilotMsg->speedModeValue(aMover.GetAutopilotSpeedModeValue());

      if (aMover.GetCurrentManeuver() != nullptr)
      {
         autopilotMsg->currentManeuver(aMover.GetCurrentManeuver()->Type());
      }
      else
      {
         autopilotMsg->currentManeuver(std::string());
      }

      Send(aSimTime, aPlatform, std::move(autopilotMsg));
   }
   else if (aMsgType == "MsgSixDOF_AutopilotLimits")
   {
      auto autopilotLimitsMsg = ut::make_unique<WsfEventPipe::MsgSixDOF_AutopilotLimits>();
      autopilotLimitsMsg->platformIndex(WsfEventPipeInterface::Platform(aPlatform));
      autopilotLimitsMsg->pitchGLoadMin(aMover.GetPitchGLoadMin());
      autopilotLimitsMsg->pitchGLoadMax(aMover.GetPitchGLoadMax());
      autopilotLimitsMsg->alphaMin(aMover.GetAlphaMin() * UtMath::cRAD_PER_DEG);
      autopilotLimitsMsg->alphaMax(aMover.GetAlphaMax() * UtMath::cRAD_PER_DEG);
      autopilotLimitsMsg->pitchRateMin(aMover.GetPitchRateMin() * UtMath::cRAD_PER_DEG);
      autopilotLimitsMsg->pitchRateMax(aMover.GetPitchRateMax() * UtMath::cRAD_PER_DEG);
      autopilotLimitsMsg->verticalSpeedMin(aMover.GetVerticalSpeedMin());
      autopilotLimitsMsg->verticalSpeedMax(aMover.GetVerticalSpeedMax());
      autopilotLimitsMsg->yawGLoadMax(aMover.GetYawGLoadMax());
      autopilotLimitsMsg->betaMax(aMover.GetBetaMax() * UtMath::cRAD_PER_DEG);
      autopilotLimitsMsg->yawRateMax(aMover.GetYawRateMax() * UtMath::cRAD_PER_DEG);
      autopilotLimitsMsg->rollRateMax(aMover.GetRollRateMax() * UtMath::cRAD_PER_DEG);
      autopilotLimitsMsg->bankAngleMax(aMover.GetBankAngleMax() * UtMath::cRAD_PER_DEG);
      autopilotLimitsMsg->forwardAccelMin(aMover.GetForwardAccelMin());
      autopilotLimitsMsg->forwardAccelMax(aMover.GetForwardAccelMax());
      Send(aSimTime, aPlatform, std::move(autopilotLimitsMsg));
   }
   else if (aMsgType == "MsgSixDOF_ControlInputs")
   {
      auto controlInputsMsg = ut::make_unique<WsfEventPipe::MsgSixDOF_ControlInputs>();
      controlInputsMsg->platformIndex(WsfEventPipeInterface::Platform(aPlatform));
      controlInputsMsg->stickBack(aMover.GetStickBackPosition());
      controlInputsMsg->stickRight(aMover.GetStickRightPosition());
      controlInputsMsg->rudderRight(aMover.GetRudderRightPosition());
      controlInputsMsg->throttle(aMover.GetThrottleMilitaryPosition() + aMover.GetThrottleAfterburnerPosition());
      controlInputsMsg->speedBrakeLever(aMover.GetSpeedBrakeControlPosition());
      controlInputsMsg->flapsLever(aMover.GetFlapsLeverPosition());
      controlInputsMsg->landingGearLever(aMover.GetLandingGearLeverPosition());
      controlInputsMsg->spoilersLever(aMover.GetSpoilersLeverPosition());
      controlInputsMsg->thrustReverserLever(aMover.GetThrustReverserControlPosition());
      Send(aSimTime, aPlatform, std::move(controlInputsMsg));
   }
   else if (aMsgType == "MsgSixDOF_ForceMoment")
   {
      auto forceMomentMsg = ut::make_unique<WsfEventPipe::MsgSixDOF_ForceMoment>();
      forceMomentMsg->platformIndex(WsfEventPipeInterface::Platform(aPlatform));
      forceMomentMsg->liftForce_nt(aMover.GetLift_nt());
      forceMomentMsg->dragForce_nt(aMover.GetDrag_nt());
      forceMomentMsg->thrustForce_nt(aMover.GetThrustMagnitude_nt());
      forceMomentMsg->sideForce_nt(aMover.GetSideForce_nt());

      auto rigidBodyMover = dynamic_cast<RigidBodyMover*>(&aMover);
      if (rigidBodyMover)
      {
         forceMomentMsg->yawMoment(rigidBodyMover->GetYawMoment_nm());
         forceMomentMsg->pitchMoment(rigidBodyMover->GetPitchMoment_nm());
         forceMomentMsg->rollMoment(rigidBodyMover->GetRollMoment_nm());
      }

      Send(aSimTime, aPlatform, std::move(forceMomentMsg));
   }
}

/** Return a reference to the simulation extension. */
wsf::six_dof::EventPipeInterface& wsf::six_dof::EventPipeInterface::Get(const WsfSimulation& aSimulation)
{
   WsfSimulationExtension* extensionPtr = aSimulation.FindExtension("wsf_six_dof_eventpipe");
   return static_cast<EventPipeInterface&>(*extensionPtr);
}

wsf::six_dof::EventPipeInterface::EventPipeInterface()
   : mEventPipePtr(nullptr)
{
}

/** Calls to make when this interface is added to the simulation. */
void wsf::six_dof::EventPipeInterface::AddedToSimulation()
{
   // If the event_pipe extension is available, hook in the handlers for our events.
   WsfEventPipeInterface* eventPipePtr = WsfEventPipeInterface::Find(GetSimulation());
   if (eventPipePtr != nullptr)
   {
      mEventPipePtr = ut::make_unique<EventPipe>(eventPipePtr);
      eventPipePtr->AddLogger(mEventPipePtr.get());
   }
}
