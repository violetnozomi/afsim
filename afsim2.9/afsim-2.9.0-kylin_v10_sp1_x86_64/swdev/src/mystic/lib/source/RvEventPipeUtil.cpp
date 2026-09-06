// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "RvEventPipeUtil.hpp"

#include <QString>

#include "RvEnvironment.hpp"
#include "RvEventPipeClasses.hpp"
#include "RvEventPipeClassesRegister.hpp"
#include "RvEventPipeSchema.hpp"
#include "RvMilEventPipeClasses.hpp"
#include "RvMilEventPipeClassesRegister.hpp"
#include "RvMilEventPipeSchema.hpp"
#include "UtQuaternion.hpp"

QString rv::toString(const AngleLimit& aLimit)
{
   return QString("%1:%2|%3:%4")
      .arg(aLimit.minAzimuth())
      .arg(aLimit.maxAzimuth())
      .arg(aLimit.minElevation())
      .arg(aLimit.maxElevation());
}

UtVec3d rv::toVec3(const rv::Vec3f& aVec)
{
   return UtVec3d((double)aVec.x(), (double)aVec.y(), (double)aVec.z());
}

UtVec3d rv::toVec3(const rv::Vec3d& aVec)
{
   return UtVec3d(aVec.x(), aVec.y(), aVec.z());
}

void rv::RegisterSerializer(UtPackSerializer& aSerializer)
{
   aSerializer.RegisterBuiltinTypes();
   rv::UtPack_register_all_wsf_types(aSerializer);
   rv::UtPack_register_all_wsf_mil_events_types(aSerializer);

   for (auto& ext : rvEnv.GetExtensions())
   {
      ext->RegisterSerializer(aSerializer);
   }
}

void rv::GetSchema(UtTextDocument& aDocument)
{
   // be careful, the order here matters!
   // wsf, mil, then everything else
   aDocument.Insert(0, cRvEventPipe_SCHEMA);
   aDocument.Insert(aDocument.Size() - 1, cRvMilEventPipe_SCHEMA);

   for (auto& ext : rvEnv.GetExtensions())
   {
      aDocument.Insert(aDocument.Size() - 1, ext->GetSchema());
   }
}

// some messages are immediately processed and retained in other ways
// don't bother storing these messages after processing
// One-time messages are marked as 'lasting'
bool rv::IsOneTimeMessage(const MsgBase* aMessagePtr)
{
   for (auto& ext : rvEnv.GetExtensions())
   {
      if (ext->HandlesMessage(aMessagePtr->GetMessageId()))
      {
         return ext->IsOneTimeMessage(aMessagePtr->GetMessageId());
      }
   }
   switch (aMessagePtr->GetMessageId())
   {
   case rv::MsgBehaviorTree::cMESSAGE_ID:
   case rv::MsgBookmark::cMESSAGE_ID:
   case rv::MsgComment::cMESSAGE_ID:
   case rv::MsgDrawCommand::cMESSAGE_ID:
   case rv::MsgEmitterModeDefinition::cMESSAGE_ID:
   case rv::MsgVisualPartDefinition::cMESSAGE_ID:
   case rv::MsgPlatformInfo::cMESSAGE_ID:
   case rv::MsgPlatformStatus::cMESSAGE_ID:
   case rv::MsgWeaponFired::cMESSAGE_ID:
   case rv::MsgWeaponTerminated::cMESSAGE_ID:
   case rv::MsgSetDate::cMESSAGE_ID:
   case rv::MsgExecData::cMESSAGE_ID:
   case rv::MsgPartStatus::cMESSAGE_ID:
   case rv::MsgScenarioData::cMESSAGE_ID:
   case rv::MsgSensorModeChange::cMESSAGE_ID:
   case rv::MsgWeaponModeChange::cMESSAGE_ID:
   case rv::MsgWeaponQuantityChange::cMESSAGE_ID:
   case rv::MsgResource::cMESSAGE_ID:
   case rv::MsgRouteChanged::cMESSAGE_ID:
   case rv::MsgCircularZone::cMESSAGE_ID:
   case rv::MsgEllipticalZone::cMESSAGE_ID:
   case rv::MsgSphericalZone::cMESSAGE_ID:
   case rv::MsgPolygonalZone::cMESSAGE_ID:
   case rv::MsgZoneSet::cMESSAGE_ID:
   case rv::MsgCommInfo::cMESSAGE_ID:
   case rv::MsgNetworkInfo::cMESSAGE_ID:
   case rv::MsgRouterInfo::cMESSAGE_ID:
   case rv::MsgStateMachine::cMESSAGE_ID:
      return true;
   default:
      return false;
   }
}

// only certain messages are useful for plotting / event markers, these are those
bool rv::IsEventMessage(const MsgBase* aMessagePtr)
{
   for (auto& ext : rvEnv.GetExtensions())
   {
      if (ext->HandlesMessage(aMessagePtr->GetMessageId()))
      {
         return ext->IsEventMessage(aMessagePtr->GetMessageId());
      }
   }
   switch (aMessagePtr->GetMessageId())
   {
   case rv::MsgPlatformStatus::cMESSAGE_ID:
   case rv::MsgEntityState::cMESSAGE_ID:
   case rv::MsgComment::cMESSAGE_ID:
   case rv::MsgDetectAttempt::cMESSAGE_ID:
   case rv::MsgLocalTrackUpdate::cMESSAGE_ID:
   case rv::MsgWeaponFired::cMESSAGE_ID:
   case rv::MsgWeaponTerminated::cMESSAGE_ID:
   case rv::MsgMessageTransmitted::cMESSAGE_ID:
   case rv::MsgMessageReceived::cMESSAGE_ID:
   case rv::MsgMessageHop::cMESSAGE_ID:
   case rv::MsgSensorDetectionChange::cMESSAGE_ID:
   case rv::MsgTaskUpdate::cMESSAGE_ID:
   case rv::MsgSensorTrackCreated::cMESSAGE_ID:
   case rv::MsgLocalTrackCreated::cMESSAGE_ID:
   case rv::MsgLocalTrackCorrelation::cMESSAGE_ID:
   case rv::MsgLocalTrackDecorrelation::cMESSAGE_ID:
   case rv::MsgPlatformAppearanceChanged::cMESSAGE_ID:
   case rv::MsgJammingRequestInitiated::cMESSAGE_ID:
   case rv::MsgJammingRequestCanceled::cMESSAGE_ID:
   case rv::MsgBookmark::cMESSAGE_ID:
      return true;
   default:
      return false;
   }
}

void rv::BuildUtEntity(float aSimTime, const MsgEntityState& aEntityState, UtEntity& aEntity)
{
   double dtime   = aSimTime - aEntityState.simTime();
   double lwcs[3] = {aEntityState.state().locationWCS().x() + dtime * aEntityState.state().velocityWCS().x() +
                        0.5 * dtime * dtime * aEntityState.state().accelerationWCS().x(),
                     aEntityState.state().locationWCS().y() + dtime * aEntityState.state().velocityWCS().y() +
                        0.5 * dtime * dtime * aEntityState.state().accelerationWCS().y(),
                     aEntityState.state().locationWCS().z() + dtime * aEntityState.state().velocityWCS().z() +
                        0.5 * dtime * dtime * aEntityState.state().accelerationWCS().z()};
   aEntity.SetLocationWCS(lwcs);
   double vwcs[3] = {aEntityState.state().velocityWCS().x() + dtime * aEntityState.state().accelerationWCS().x(),
                     aEntityState.state().velocityWCS().y() + dtime * aEntityState.state().accelerationWCS().y(),
                     aEntityState.state().velocityWCS().z() + dtime * aEntityState.state().accelerationWCS().z()};
   aEntity.SetVelocityWCS(vwcs);
   aEntity.SetOrientationWCS(aEntityState.state().orientationWCS().x(),
                             aEntityState.state().orientationWCS().y(),
                             aEntityState.state().orientationWCS().z());
}

void RV_EXPORT rv::GetCombinedOrientation(const MsgPartArticulationStatus& aMsg, double& aAz, double& aEl, double& aRoll)
{
   UtQuaternion slew;
   slew.Set(aMsg.slewECS().x(), aMsg.slewECS().y(), aMsg.slewECS().z());
   UtQuaternion cue;
   cue.Set(aMsg.cuePCS().x(), aMsg.cuePCS().y(), aMsg.cuePCS().z());
   UtQuaternion final = cue * slew;
   final.Get(aAz, aEl, aRoll);
}
