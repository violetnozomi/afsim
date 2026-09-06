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
#include "RvP6DofDataExtension.hpp"

#include "RvEnvironment.hpp"
#include "RvP6dofEventPipeClasses.hpp"
#include "RvP6dofEventPipeClassesRegister.hpp"
#include "RvP6dofEventPipeSchema.hpp"

bool rv::P6DofDataExtension::HandlesMessage(int aMessageId) const
{
   switch (aMessageId)
   {
   case (MsgP6dofCoreData::cMESSAGE_ID):
   case (MsgP6dofKinematic::cMESSAGE_ID):
   case (MsgP6dofEngineFuel::cMESSAGE_ID):
   case (MsgP6dofAutopilot::cMESSAGE_ID):
   case (MsgP6dofAutopilotLimits::cMESSAGE_ID):
   case (MsgP6dofControlInputs::cMESSAGE_ID):
   case (MsgP6dofControlSurfaces::cMESSAGE_ID):
   case (MsgP6dofForceMoment::cMESSAGE_ID):
      return true;
   default:
      return false;
   }
}

QVariant rv::P6DofDataExtension::HandleMessage(const MsgBase* aMsg, int aColumn, int aRole) const
{
   switch (aMsg->GetMessageId())
   {
   case (MsgP6dofCoreData::cMESSAGE_ID):
      return HandleP6DofCoreData(static_cast<const MsgP6dofCoreData*>(aMsg), aColumn, aRole);
   case (MsgP6dofKinematic::cMESSAGE_ID):
      return HandleP6DofKinematic(static_cast<const MsgP6dofKinematic*>(aMsg), aColumn, aRole);
   case (MsgP6dofEngineFuel::cMESSAGE_ID):
      return HandleP6DofEngineFuel(static_cast<const MsgP6dofEngineFuel*>(aMsg), aColumn, aRole);
   case (MsgP6dofAutopilot::cMESSAGE_ID):
      return HandleP6DofAutopilot(static_cast<const MsgP6dofAutopilot*>(aMsg), aColumn, aRole);
   case (MsgP6dofAutopilotLimits::cMESSAGE_ID):
      return HandleP6DofAutopilotLimits(static_cast<const MsgP6dofAutopilotLimits*>(aMsg), aColumn, aRole);
   case (MsgP6dofControlInputs::cMESSAGE_ID):
      return HandleP6DofControlInputs(static_cast<const MsgP6dofControlInputs*>(aMsg), aColumn, aRole);
   case (MsgP6dofForceMoment::cMESSAGE_ID):
      return HandleP6DofForceMoment(static_cast<const MsgP6dofForceMoment*>(aMsg), aColumn, aRole);
   default:
      return QVariant();
   }
}

void rv::P6DofDataExtension::RegisterSerializer(UtPackSerializer& aSerializer) const
{
   UtPack_register_all_wsf_p6dof_events_types(aSerializer);
}

const char* rv::P6DofDataExtension::GetSchema() const
{
   return cRvP6dofEventPipe_SCHEMA;
}

bool rv::P6DofDataExtension::IsOneTimeMessage(int aMessageId) const
{
   return false;
}

void rv::P6DofDataExtension::AddMessage(ResultDb& aResultDb, MsgBase* aMsg, bool aAtBegin) const
{
   switch (aMsg->GetMessageId())
   {
   case MsgP6dofCoreData::cMESSAGE_ID:
      AddTypedMessage<MsgP6dofCoreData>(aMsg, aResultDb, aAtBegin);
      break;
   case MsgP6dofKinematic::cMESSAGE_ID:
      AddTypedMessage<MsgP6dofKinematic>(aMsg, aResultDb, aAtBegin);
      break;
   case MsgP6dofEngineFuel::cMESSAGE_ID:
      AddTypedMessage<MsgP6dofEngineFuel>(aMsg, aResultDb, aAtBegin);
      break;
   case MsgP6dofAutopilot::cMESSAGE_ID:
      AddTypedMessage<MsgP6dofAutopilot>(aMsg, aResultDb, aAtBegin);
      break;
   case MsgP6dofAutopilotLimits::cMESSAGE_ID:
      AddTypedMessage<MsgP6dofAutopilotLimits>(aMsg, aResultDb, aAtBegin);
      break;
   case MsgP6dofControlInputs::cMESSAGE_ID:
      AddTypedMessage<MsgP6dofControlInputs>(aMsg, aResultDb, aAtBegin);
      break;
   case MsgP6dofControlSurfaces::cMESSAGE_ID:
      AddTypedMessage<MsgP6dofControlSurfaces>(aMsg, aResultDb, aAtBegin);
      break;
   case MsgP6dofForceMoment::cMESSAGE_ID:
      AddTypedMessage<MsgP6dofForceMoment>(aMsg, aResultDb, aAtBegin);
      break;
   }
}

void rv::P6DofDataExtension::RemoveMessage(ResultDb& aResultDb, MsgBase* aMsg, bool aAtBegin) const
{
   switch (aMsg->GetMessageId())
   {
   case MsgP6dofCoreData::cMESSAGE_ID:
      RemoveTypedMessage<MsgP6dofCoreData>(aMsg, aResultDb, aAtBegin);
      break;
   case MsgP6dofKinematic::cMESSAGE_ID:
      RemoveTypedMessage<MsgP6dofKinematic>(aMsg, aResultDb, aAtBegin);
      break;
   case MsgP6dofEngineFuel::cMESSAGE_ID:
      RemoveTypedMessage<MsgP6dofEngineFuel>(aMsg, aResultDb, aAtBegin);
      break;
   case MsgP6dofAutopilot::cMESSAGE_ID:
      RemoveTypedMessage<MsgP6dofAutopilot>(aMsg, aResultDb, aAtBegin);
      break;
   case MsgP6dofAutopilotLimits::cMESSAGE_ID:
      RemoveTypedMessage<MsgP6dofAutopilotLimits>(aMsg, aResultDb, aAtBegin);
      break;
   case MsgP6dofControlInputs::cMESSAGE_ID:
      RemoveTypedMessage<MsgP6dofControlInputs>(aMsg, aResultDb, aAtBegin);
      break;
   case MsgP6dofControlSurfaces::cMESSAGE_ID:
      RemoveTypedMessage<MsgP6dofControlSurfaces>(aMsg, aResultDb, aAtBegin);
      break;
   case MsgP6dofForceMoment::cMESSAGE_ID:
      RemoveTypedMessage<MsgP6dofForceMoment>(aMsg, aResultDb, aAtBegin);
      break;
   }
}

QVariant rv::P6DofDataExtension::HandleP6DofCoreData(const MsgP6dofCoreData* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "owner";
      }
      return rvEnv.LookupPlatformVariant(aMsg->platformIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "altitude";
      }
      return aMsg->altitude_m();
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "vertical speed";
      }
      return aMsg->vertSpeed_msec();
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "KCAS";
      }
      return aMsg->KCAS();
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "KTAS";
      }
      return aMsg->KTAS();
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "mach";
      }
      return aMsg->mach();
   }
   case (rv::MessageBase::cFIELD_DATA + 5):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "heading";
      }
      return aMsg->heading_rad();
   }
   case (rv::MessageBase::cFIELD_DATA + 6):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "pitch";
      }
      return aMsg->pitch_rad();
   }
   case (rv::MessageBase::cFIELD_DATA + 7):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "roll";
      }
      return aMsg->roll_rad();
   }
   case (rv::MessageBase::cFIELD_DATA + 8):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "gLoad";
      }
      return aMsg->gLoad();
   }
   case (rv::MessageBase::cFIELD_DATA + 9):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "alpha";
      }
      return aMsg->alpha_rad();
   }
   case (rv::MessageBase::cFIELD_DATA + 10):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "gAvail";
      }
      return aMsg->gAvail();
   }
   case (rv::MessageBase::cFIELD_DATA + 11):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "speed brake angle";
      }
      return aMsg->speedbrakeAngle_rad();
   }
   }
   return QVariant();
}

QVariant rv::P6DofDataExtension::HandleP6DofKinematic(const MsgP6dofKinematic* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "owner";
      }
      return rvEnv.LookupPlatformVariant(aMsg->platformIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "beta";
      }
      return aMsg->beta_rad();
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "alpha dot";
      }
      return aMsg->alphaDot_rps();
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "beta dot";
      }
      return aMsg->betaDot_rps();
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "yaw rate";
      }
      return aMsg->yawRate_rps();
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "pitch rate";
      }
      return aMsg->pitchRate_rps();
   }
   case (rv::MessageBase::cFIELD_DATA + 5):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "roll rate";
      }
      return aMsg->rollRate_rps();
   }
   case (rv::MessageBase::cFIELD_DATA + 6):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "dynamic pressure";
      }
      return aMsg->dynamicPressure_psf();
   }
   case (rv::MessageBase::cFIELD_DATA + 7):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "nx";
      }
      return aMsg->nx();
   }
   case (rv::MessageBase::cFIELD_DATA + 8):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "ny";
      }
      return aMsg->ny();
   }
   case (rv::MessageBase::cFIELD_DATA + 9):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "flight path angle";
      }
      return aMsg->flightPathAngle_rad();
   }
   case (rv::MessageBase::cFIELD_DATA + 10):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "center of gravity - x";
      }
      return aMsg->centerGravity_x();
   }
   case (rv::MessageBase::cFIELD_DATA + 11):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "center of gravity - y";
      }
      return aMsg->centerGravity_y();
   }
   case (rv::MessageBase::cFIELD_DATA + 12):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "center of gravity - z";
      }
      return aMsg->centerGravity_z();
   }
   }
   return QVariant();
}

QVariant rv::P6DofDataExtension::HandleP6DofEngineFuel(const MsgP6dofEngineFuel* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "owner";
      }
      return rvEnv.LookupPlatformVariant(aMsg->platformIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "fuel flow";
      }
      return aMsg->fuelFlow_kgsec();
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "fuel internal";
      }
      return aMsg->fuelInternal_kg();
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "fuel external";
      }
      return aMsg->fuelExternal_kg();
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "fuel internal remaining";
      }
      return aMsg->fuelInternalRemaining_kg();
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "fuel external remaining";
      }
      return aMsg->fuelExternalRemaining_kg();
   }
   case (rv::MessageBase::cFIELD_DATA + 5):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "total weight";
      }
      return aMsg->totalWeight_kg();
   }
   case (rv::MessageBase::cFIELD_DATA + 6):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "afterburner on";
      }
      return aMsg->afterburner_on();
   }
   case (rv::MessageBase::cFIELD_DATA + 7):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "producing thrust";
      }
      return aMsg->producing_thrust();
   }
   case (rv::MessageBase::cFIELD_DATA + 8):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "engine operating";
      }
      return aMsg->engine_operating();
   }
   case (rv::MessageBase::cFIELD_DATA + 9):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "engine smoking";
      }
      return aMsg->engine_smoking();
   }
   case (rv::MessageBase::cFIELD_DATA + 10):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "contrailing";
      }
      return aMsg->contrailing();
   }
   case (rv::MessageBase::cFIELD_DATA + 11):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "joker fuel state";
      }
      return aMsg->jokerFuelState_kg();
   }
   case (rv::MessageBase::cFIELD_DATA + 12):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "bingo fuel state";
      }
      return aMsg->bingoFuelState_kg();
   }
   case (rv::MessageBase::cFIELD_DATA + 13):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "weight on wheels";
      }
      return aMsg->weightOnWheels();
   }
   }
   return QVariant();
}

QVariant rv::P6DofDataExtension::HandleP6DofAutopilot(const MsgP6dofAutopilot* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "owner";
      }
      return rvEnv.LookupPlatformVariant(aMsg->platformIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "active pilot";
      }
      return QString::fromStdString(aMsg->activePilot());
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "lateral mode name";
      }
      return QString::fromStdString(aMsg->lateralModeName());
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "lateral mode value";
      }
      return aMsg->lateralModeValue();
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "vertical mode name";
      }
      return QString::fromStdString(aMsg->verticalModeName());
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "vertical mode value";
      }
      return aMsg->verticalModeValue();
   }
   case (rv::MessageBase::cFIELD_DATA + 5):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "speed mode name";
      }
      return QString::fromStdString(aMsg->speedModeName());
   }
   case (rv::MessageBase::cFIELD_DATA + 6):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "speed mode value";
      }
      return aMsg->speedModeValue();
   }
   case (rv::MessageBase::cFIELD_DATA + 7):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "current maneuver";
      }
      return QString::fromStdString(aMsg->currentManeuver());
   }
   }
   return QVariant();
}

QVariant rv::P6DofDataExtension::HandleP6DofAutopilotLimits(const MsgP6dofAutopilotLimits* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "owner";
      }
      return rvEnv.LookupPlatformVariant(aMsg->platformIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "pitch gLoad minimum";
      }
      return aMsg->pitchGLoadMin();
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "pitch gLoad maximum";
      }
      return aMsg->pitchGLoadMax();
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "alpha minimum";
      }
      return aMsg->alphaMin();
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "alpha maximum";
      }
      return aMsg->alphaMax();
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "pitch rate minimum";
      }
      return aMsg->pitchRateMin();
   }
   case (rv::MessageBase::cFIELD_DATA + 5):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "pitch rate maximum";
      }
      return aMsg->pitchRateMax();
   }
   case (rv::MessageBase::cFIELD_DATA + 6):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "vertical speed minimum";
      }
      return aMsg->verticalSpeedMin();
   }
   case (rv::MessageBase::cFIELD_DATA + 7):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "vertical speed maximum";
      }
      return aMsg->verticalSpeedMax();
   }
   case (rv::MessageBase::cFIELD_DATA + 8):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "yaw gLoad maximum";
      }
      return aMsg->yawGLoadMax();
   }
   case (rv::MessageBase::cFIELD_DATA + 9):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "beta maximum";
      }
      return aMsg->betaMax();
   }
   case (rv::MessageBase::cFIELD_DATA + 10):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "yaw rate maximum";
      }
      return aMsg->yawRateMax();
   }
   case (rv::MessageBase::cFIELD_DATA + 11):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "roll rate maximum";
      }
      return aMsg->rollRateMax();
   }
   case (rv::MessageBase::cFIELD_DATA + 12):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "bank angle maximum";
      }
      return aMsg->bankAngleMax();
   }
   case (rv::MessageBase::cFIELD_DATA + 13):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "forward acceleration minimum";
      }
      return aMsg->forwardAccelMin();
   }
   case (rv::MessageBase::cFIELD_DATA + 14):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "forward acceleration maximum";
      }
      return aMsg->forwardAccelMax();
   }
   case (rv::MessageBase::cFIELD_DATA + 15):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "taxi speed maximum";
      }
      return aMsg->taxiSpeedMax();
   }
   case (rv::MessageBase::cFIELD_DATA + 16):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "taxi yaw rate maximum";
      }
      return aMsg->taxiYawRateMax();
   }
   }
   return QVariant();
}

QVariant rv::P6DofDataExtension::HandleP6DofControlInputs(const MsgP6dofControlInputs* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "owner";
      }
      return rvEnv.LookupPlatformVariant(aMsg->platformIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "stick back";
      }
      return aMsg->stickBack();
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "stick right";
      }
      return aMsg->stickRight();
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "rudder right";
      }
      return aMsg->rudderRight();
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "throttle";
      }
      return aMsg->throttle();
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "speed brake lever";
      }
      return aMsg->speedBrakeLever();
   }
   case (rv::MessageBase::cFIELD_DATA + 5):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "flaps lever";
      }
      return aMsg->flapsLever();
   }
   case (rv::MessageBase::cFIELD_DATA + 6):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "landing gear lever";
      }
      return aMsg->landingGearLever();
   }
   case (rv::MessageBase::cFIELD_DATA + 7):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "spoilers lever";
      }
      return aMsg->spoilersLever();
   }
   case (rv::MessageBase::cFIELD_DATA + 8):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "thrust reverser lever";
      }
      return aMsg->thrustReverserLever();
   }
   }
   return QVariant();
}

QVariant rv::P6DofDataExtension::HandleP6DofForceMoment(const MsgP6dofForceMoment* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "owner";
      }
      return rvEnv.LookupPlatformVariant(aMsg->platformIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "lift force";
      }
      return aMsg->liftForce_nt();
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "drag force";
      }
      return aMsg->dragForce_nt();
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "thrust force";
      }
      return aMsg->thrustForce_nt();
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "side force";
      }
      return aMsg->sideForce_nt();
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "yaw moment";
      }
      return aMsg->yawMoment();
   }
   case (rv::MessageBase::cFIELD_DATA + 5):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "pitch moment";
      }
      return aMsg->pitchMoment();
   }
   case (rv::MessageBase::cFIELD_DATA + 6):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "roll moment";
      }
      return aMsg->rollMoment();
   }
   }
   return QVariant();
}
