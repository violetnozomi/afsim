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
#include "RvEventMarkerPlugin.hpp"

#include "RvEnvironment.hpp"
#include "RvInteractionDb.hpp"
#include "RvResultData.hpp"
#include "RvResultPlatform.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtVec3.hpp"
#include "VaAttachmentModel.hpp"
#include "WkfPlatform.hpp"
#include "WkfScenario.hpp"
#include "WkfVtkEnvironment.hpp"
#include "event_marker/WkfEventMarker.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(RvEventMarker::Plugin, "Event Markers", "Displays markers at event sites", "mystic")

RvEventMarker::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::PluginT<wkf::EventMarkerPluginBase>(aPluginName, aUniqueId)
   , mLastTime(0.0f)
{
}

void RvEventMarker::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   double time = aData.GetSimTime();
   if (aData.GetDb() != nullptr)
   {
      GetDisplayInterface()->CleanUp(time);
      auto* scenario = vaEnv.GetStandardScenario();
      if (time > mLastTime && scenario != nullptr)
      {
         double startTime = std::max<double>(mLastTime, time - GetPrefObject()->GetTimeout());
         for (auto& cevent : mCustomEvents)
         {
            if (GetPrefObject()->GetShowEvent(cevent.first))
            // if the pref object indicates this custom event should be shown
            {
               if (aData.GetDb()->HasMessagesOfType(cevent.second->GetEventId()))
               {
                  const rv::MessageBaseArray& mb = aData.GetDb()->LockMessagesOfType(cevent.second->GetEventId());
                  UtCircularBufferIteratorConst<rv::MsgBase*> it = mb.FindFirstIteratorAfter(startTime);

                  while ((it != mb.end()) && ((*it)->simTime() < time))
                  {
                     if (cevent.second->EventFilter(*it))
                     {
                        cevent.second->GetInstanceLabel(*it);
                        // is the platform name the approriate thing to get here?
                        GetDisplayInterface()->AddMarker(wkf::make_entity<wkf::PlatformEventMarker>(
                                                            (*it)->simTime(),
                                                            cevent.first,
                                                            wkf::PlatformEventInfo{
                                                               cevent.second->GetInstanceLabel(*it).toStdString()}),
                                                         cevent.second->GetPosition(*it));
                     }

                     ++it;
                  }
                  aData.GetDb()->UnlockMessagesOfType(cevent.second->GetEventId());
               }
            }
         }
         if (GetPrefObject()->GetShowEvent("DAMAGED") || GetPrefObject()->GetShowEvent("REMOVED"))
         {
            if (aData.GetDb()->HasMessagesOfType(rv::MsgPlatformStatus::cMESSAGE_ID))
            {
               // pull MsgPlatformStatus between mLastTime and time
               const rv::MessageBaseArray& mb = aData.GetDb()->LockMessagesOfType(rv::MsgPlatformStatus::cMESSAGE_ID);
               UtCircularBufferIteratorConst<rv::MsgBase*> it = mb.FindFirstIteratorAfter(startTime);

               while ((it != mb.end()) && ((*it)->simTime() < time))
               {
                  rv::MsgPlatformStatus* msg = dynamic_cast<rv::MsgPlatformStatus*>(*it);
                  if (msg->broken() && GetPrefObject()->GetShowEvent("DAMAGED"))
                  {
                     std::string owner = scenario->FindPlatformByIndex(msg->platformIndex())->GetName();
                     GetDisplayInterface()->AddMarker(
                        wkf::make_entity<wkf::PlatformEventMarker>(msg->simTime(), "DAMAGED", wkf::PlatformEventInfo{owner}),
                        GetPositionAtTime(msg->simTime(), msg->platformIndex(), aData.GetDb()));
                  }
                  else if (msg->removed() && GetPrefObject()->GetShowEvent("REMOVED"))
                  {
                     std::string owner = scenario->FindPlatformByIndex(msg->platformIndex())->GetName();
                     GetDisplayInterface()->AddMarker(
                        wkf::make_entity<wkf::PlatformEventMarker>(msg->simTime(), "REMOVED", wkf::PlatformEventInfo{owner}),
                        GetPositionAtTime(msg->simTime(), msg->platformIndex(), aData.GetDb()));
                  }

                  ++it;
               }
               aData.GetDb()->UnlockMessagesOfType(rv::MsgPlatformStatus::cMESSAGE_ID);
            }
         }
         if (GetPrefObject()->GetShowEvent("WEAPON_HIT") || GetPrefObject()->GetShowEvent("WEAPON_MISSED"))
         {
            if (aData.GetDb()->HasMessagesOfType(rv::MsgWeaponTerminated::cMESSAGE_ID))
            {
               const rv::MessageBaseArray& mb = aData.GetDb()->LockMessagesOfType(rv::MsgWeaponTerminated::cMESSAGE_ID);
               UtCircularBufferIteratorConst<rv::MsgBase*> it = mb.FindFirstIteratorAfter(startTime);
               if ((it != mb.end()) && ((*it)->simTime() < time))
               {
                  while ((it != mb.end()) && ((*it)->simTime() < time))
                  {
                     rv::MsgWeaponTerminated* msg = dynamic_cast<rv::MsgWeaponTerminated*>(*it);

                     wkf::WeaponEventInfo info;

                     wkf::Platform* weaponPtr = scenario->FindPlatformByIndex(msg->weaponPlatformIndex());
                     if (weaponPtr != nullptr)
                     {
                        info.mWeaponName  = weaponPtr->GetName();
                        info.mWeaponSpeed = weaponPtr->GetVelocityWCS().Magnitude();
                     }

                     wkf::Platform* targetPtr = scenario->FindPlatformByIndex(msg->targetPlatformIndex());
                     if (targetPtr != nullptr)
                     {
                        info.mTargetName  = targetPtr->GetName();
                        info.mTargetSpeed = targetPtr->GetVelocityWCS().Magnitude();
                     }

                     wkf::Platform* firingPtr = scenario->FindPlatformByIndex(msg->firingPlatformIndex());
                     if (firingPtr != nullptr)
                     {
                        info.mFiringName = firingPtr->GetName();
                     }

                     if (msg->missDistanceValid())
                     {
                        info.mMissDistance = msg->missDistance();
                     }
                     if (msg->interceptPkValid())
                     {
                        info.mInterceptPk = msg->interceptPk();
                     }
                     if (msg->pkDrawnValid())
                     {
                        info.mPkDrawn = msg->pkDrawn();
                     }
                     std::string type = "";

                     switch (msg->geometryResult())
                     {
                     case (rv::MsgWeaponTerminated_GeometryResult::target_impact):
                        info.mResult = "target impact";
                        type         = "WEAPON_HIT";
                        break;
                     case (rv::MsgWeaponTerminated_GeometryResult::target_proximity_above_ground):
                        info.mResult = "target proximity above ground";
                        type         = "WEAPON_HIT";
                        break;
                     case (rv::MsgWeaponTerminated_GeometryResult::target_proximity_air_burst):
                        info.mResult = "target proximity air burst";
                        type         = "WEAPON_HIT";
                        break;
                     case (rv::MsgWeaponTerminated_GeometryResult::target_proximity_ground_impact):
                        info.mResult = "target proximity ground impact";
                        type         = "WEAPON_HIT";
                        break;
                     case (rv::MsgWeaponTerminated_GeometryResult::dud):
                        info.mResult = "dud";
                        type         = "WEAPON_MISSED";
                        break;
                     case (rv::MsgWeaponTerminated_GeometryResult::far_away_in_air):
                        info.mResult = "far away in air";
                        type         = "WEAPON_MISSED";
                        break;
                     case (rv::MsgWeaponTerminated_GeometryResult::far_away_above_ground):
                        info.mResult = "far away above ground";
                        type         = "WEAPON_MISSED";
                        break;
                     case (rv::MsgWeaponTerminated_GeometryResult::far_away_ground_impact):
                        info.mResult = "far away ground impact";
                        type         = "WEAPON_MISSED";
                        break;
                     case (rv::MsgWeaponTerminated_GeometryResult::part_disabled_or_destroyed):
                        info.mResult = "part disabled or destroyed";
                        type         = "WEAPON_MISSED";
                        break;
                     default:
                        break;
                     }

                     if (!type.empty() && GetPrefObject()->GetShowEvent(type))
                     {
                        GetDisplayInterface()->AddMarker(
                           wkf::make_entity<wkf::WeaponEventMarker>(msg->simTime(), type, info),
                           GetPositionAtTime(msg->simTime(), msg->weaponPlatformIndex(), aData.GetDb()));
                     }

                     ++it;
                  }
               }
               aData.GetDb()->UnlockMessagesOfType(rv::MsgWeaponTerminated::cMESSAGE_ID);
               // pull MsgWeaponTerminated between mLastTime and time
            }
         }
      }
   }
   mLastTime = time;
}

void RvEventMarker::Plugin::ClearScenario(bool aFullReset)
{
   // clear markers
   GetDisplayInterface()->DeleteEventMarkers();
   mLastTime = 0.0;
}

vespa::VaPosition RvEventMarker::Plugin::GetPositionAtTime(double aTime, int aPlatformIndex, rv::ResultDb* aDatabase)
{
   rv::ResultPlatform* plat = aDatabase->FindPlatform(aPlatformIndex);
   if (plat)
   {
      auto msg = plat->FindFirstBefore<rv::MsgEntityState>(aTime);

      if (msg != nullptr)
      {
         double xyz[3];
         double dtime = aTime - msg->simTime();
         xyz[0]       = msg->state().locationWCS().x() + dtime * msg->state().velocityWCS().x() +
                  0.5 * dtime * dtime * msg->state().accelerationWCS().x();
         xyz[1] = msg->state().locationWCS().y() + dtime * msg->state().velocityWCS().y() +
                  0.5 * dtime * dtime * msg->state().accelerationWCS().y();
         xyz[2] = msg->state().locationWCS().z() + dtime * msg->state().velocityWCS().z() +
                  0.5 * dtime * dtime * msg->state().accelerationWCS().z();

         double lla[3];
         UtEllipsoidalEarth::ConvertECEFToLLA(xyz, lla[0], lla[1], lla[2]);
         return vespa::VaPosition(lla[0], lla[1], lla[2], xyz[0], xyz[1], xyz[2]);
      }
   }
   return vespa::VaPosition(0, 0, 0);
}

void RvEventMarker::Plugin::PluginsLoaded()
{
   for (auto& ext : rvEnv.GetExtensions())
   {
      for (auto& event : ext->GetEventList())
      {
         auto po = wkfEnv.GetPreferenceObject<wkf::EventMarkerPrefObject>();
         po->RegisterEvent(event->GetName(), event->GetColor(nullptr));
         mCustomEvents[event->GetName()] = std::move(event);
      }
   }
}
