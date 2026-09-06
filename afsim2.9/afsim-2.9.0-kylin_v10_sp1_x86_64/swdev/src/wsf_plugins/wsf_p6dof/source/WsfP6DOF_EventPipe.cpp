// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfP6DOF_EventPipe.hpp"

#include "UtMemory.hpp"
#include "UtOrbitalElements.hpp"
#include "UtPackReflect.hpp"
#include "WsfEventPipeClasses.hpp"
#include "WsfP6DOF_Mover.hpp"
#include "WsfP6DOF_Observer.hpp"
#include "WsfP6dofEventPipeClasses.hpp"
#include "WsfP6dofEventPipeClassesRegister.hpp"
#include "WsfP6dofEventPipeSchema.hpp"
#include "WsfSimulation.hpp"
#include "maneuvers/WsfManeuver.hpp"

/** Register the event pipe events for P6dof data. */
void wsf::p6dof::EventPipe::RegisterEvents(WsfEventPipeExtension& aEventPipeExtension)
{
   aEventPipeExtension.AddSchema(cWsfP6dofEventPipe_SCHEMA);
   aEventPipeExtension.RegisterEventOption("P6DOF", false);

   // Register the callback that gets invoked by WsfEventPipeInterface::AddedToSimulation to register
   // our message types with the serializer.
   aEventPipeExtension.AddCallback(aEventPipeExtension.RegisterExtensionMessages.Connect(&EventPipe::RegisterMessages));
}

/** Register the messages with the serializer. */
void wsf::p6dof::EventPipe::RegisterMessages(UtPackSerializer* aSerializerPtr)
{
   WsfEventPipe::UtPack_register_all_wsf_p6dof_events_types(*aSerializerPtr);
}

wsf::p6dof::EventPipe::EventPipe(WsfEventPipeInterface* aEventPipeInterfacePtr)
   : mSimulationPtr(&aEventPipeInterfacePtr->GetSimulation())
   , mEventPipePtr(aEventPipeInterfacePtr)
{
   cP6DOF = aEventPipeInterfacePtr->GetEventId("P6DOF");
}

/** Update callback subscriptions. */
void wsf::p6dof::EventPipe::UpdateSubscriptions(const WsfEventPipeOptions& aNewCombinedOptions,
                                                const WsfEventPipeOptions& aPreviousCombinedOptions)
{
   if (aNewCombinedOptions.IsOptionEnabled(cP6DOF))
   {
      if (!aPreviousCombinedOptions.IsOptionEnabled(cP6DOF))
      {
         mP6dofCallback += WsfObserver::P6DofSendMessage(mSimulationPtr).Connect(&EventPipe::P6DofSendMessage, this);
      }
   }
   else if (aPreviousCombinedOptions.IsOptionEnabled(cP6DOF))
   {
      mP6dofCallback.Clear();
   }
}

/** Callback to send a p6dof message through the event pipe. */
void wsf::p6dof::EventPipe::P6DofSendMessage(double             aSimTime,
                                             const WsfPlatform& aPlatform,
                                             WsfP6DOF_Mover&    aMover,
                                             const std::string& aMsgType)
{
   if (aMsgType == "MsgP6dofCoreData")
   {
      auto coreDataMsg = ut::make_unique<WsfEventPipe::MsgP6dofCoreData>();
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
      coreDataMsg->gAvail(aMover.GetP6DofVehicle()->MaxPotentialManeuverGLoad());

      P6DofVehicle* vehicle = aMover.GetP6DofVehicle();
      if (vehicle != nullptr)
      {
         P6DofFlightControlSystem* flightControlSystem = vehicle->GetFlightControls();
         if (flightControlSystem != nullptr)
         {
            size_t speedBrakesHandle = flightControlSystem->GetControlSurfaceHandleContainingString("SpeedBrake");
            coreDataMsg->speedbrakeAngle_rad(flightControlSystem->GetControlSurfaceAngle_deg(speedBrakesHandle) *
                                             UtMath::cRAD_PER_DEG);
         }
      }

      Send(aSimTime, aPlatform, std::move(coreDataMsg));
   }
   else if (aMsgType == "MsgP6dofKinematic")
   {
      auto kinematicMsg = ut::make_unique<WsfEventPipe::MsgP6dofKinematic>();
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
      kinematicMsg->centerGravity_x(aMover.GetCg().X());
      kinematicMsg->centerGravity_y(aMover.GetCg().Y());
      kinematicMsg->centerGravity_z(aMover.GetCg().Z());
      Send(aSimTime, aPlatform, std::move(kinematicMsg));
   }
   else if (aMsgType == "MsgP6dofEngineFuel")
   {
      auto engineFuelMsg = ut::make_unique<WsfEventPipe::MsgP6dofEngineFuel>();
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
      engineFuelMsg->weightOnWheels(aMover.GetWeightOnWheels());
      Send(aSimTime, aPlatform, std::move(engineFuelMsg));
   }
   else if (aMsgType == "MsgP6dofAutopilot")
   {
      auto autopilotMsg = ut::make_unique<WsfEventPipe::MsgP6dofAutopilot>();
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
   else if (aMsgType == "MsgP6dofAutopilotLimits")
   {
      auto autopilotLimitsMsg = ut::make_unique<WsfEventPipe::MsgP6dofAutopilotLimits>();
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
      autopilotLimitsMsg->taxiSpeedMax(aMover.GetTaxiSpeedMax() * UtMath::cM_PER_FT);
      autopilotLimitsMsg->taxiYawRateMax(aMover.GetTaxiYawRateMax() * UtMath::cRAD_PER_DEG);
      Send(aSimTime, aPlatform, std::move(autopilotLimitsMsg));
   }
   else if (aMsgType == "MsgP6dofControlInputs")
   {
      auto controlInputsMsg = ut::make_unique<WsfEventPipe::MsgP6dofControlInputs>();
      controlInputsMsg->platformIndex(WsfEventPipeInterface::Platform(aPlatform));
      controlInputsMsg->stickBack(aMover.GetStickBackPosition());
      controlInputsMsg->stickRight(aMover.GetStickRightPosition());
      controlInputsMsg->rudderRight(aMover.GetRudderRightPosition());
      P6DofPilotObject* pilotObj = aMover.GetP6DofVehicle()->GetPilotObject();
      if (pilotObj != nullptr)
      {
         controlInputsMsg->throttle(pilotObj->GetThrottleControllerPosition());
      }
      controlInputsMsg->speedBrakeLever(aMover.GetSpeedBrakeControlPosition());
      controlInputsMsg->flapsLever(aMover.GetFlapsLeverPosition());
      controlInputsMsg->landingGearLever(aMover.GetLandingGearLeverPosition());
      controlInputsMsg->spoilersLever(aMover.GetSpoilersLeverPosition());
      controlInputsMsg->thrustReverserLever(aMover.GetThrustReverserControlPosition());
      Send(aSimTime, aPlatform, std::move(controlInputsMsg));
   }
   else if (aMsgType == "MsgP6dofControlSurfaces")
   {
      auto controlSurfacesMsg = ut::make_unique<WsfEventPipe::MsgP6dofControlSurfaces>();
      controlSurfacesMsg->platformIndex(WsfEventPipeInterface::Platform(aPlatform));

      WsfEventPipe::StringList sNames;
      WsfEventPipe::DoubleList sAngles;
      for (const std::string& name : aMover.GetListOfControlSurfaceNames())
      {
         sNames.push_back(name);
         sAngles.push_back(aMover.GetAngleOfControlSurface(name) * UtMath::cRAD_PER_DEG);
      }
      controlSurfacesMsg->surfaceNames(sNames);
      controlSurfacesMsg->surfaceAngles(sAngles);
      Send(aSimTime, aPlatform, std::move(controlSurfacesMsg));
   }
   else if (aMsgType == "MsgP6dofForceMoment")
   {
      auto forceMomentMsg = ut::make_unique<WsfEventPipe::MsgP6dofForceMoment>();
      forceMomentMsg->platformIndex(WsfEventPipeInterface::Platform(aPlatform));
      forceMomentMsg->liftForce_nt(aMover.GetLift_nt());
      forceMomentMsg->dragForce_nt(aMover.GetDrag_nt());
      forceMomentMsg->thrustForce_nt(aMover.GetThrustMagnitude_nt());
      forceMomentMsg->sideForce_nt(aMover.GetSideForce_nt());
      forceMomentMsg->yawMoment(aMover.GetYawMoment_nm());
      forceMomentMsg->pitchMoment(aMover.GetPitchMoment_nm());
      forceMomentMsg->rollMoment(aMover.GetRollMoment_nm());
      Send(aSimTime, aPlatform, std::move(forceMomentMsg));
   }
}

/** Return a reference to the simulation extension. */
wsf::p6dof::EventPipeInterface& wsf::p6dof::EventPipeInterface::Get(const WsfSimulation& aSimulation)
{
   WsfSimulationExtension* extensionPtr = aSimulation.FindExtension("wsf_p6dof_eventpipe");
   return static_cast<EventPipeInterface&>(*extensionPtr);
}

wsf::p6dof::EventPipeInterface::EventPipeInterface()
   : mEventPipePtr(nullptr)
{
}

/** Calls to make when this interface is added to the simulation. */
void wsf::p6dof::EventPipeInterface::AddedToSimulation()
{
   // If the event_pipe extension is available, hook in the handlers for our events.
   WsfEventPipeInterface* eventPipePtr = WsfEventPipeInterface::Find(GetSimulation());
   if (eventPipePtr != nullptr)
   {
      mEventPipePtr = ut::make_unique<EventPipe>(eventPipePtr);
      eventPipePtr->AddLogger(mEventPipePtr.get());
   }
}
