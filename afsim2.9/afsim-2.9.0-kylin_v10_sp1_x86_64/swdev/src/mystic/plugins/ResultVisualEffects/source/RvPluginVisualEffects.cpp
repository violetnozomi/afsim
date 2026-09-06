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

#include "RvPluginVisualEffects.hpp"

#include "RvEnvironment.hpp"
#include "RvEventPipeClasses.hpp"
#include "RvInteractionDb.hpp"
#include "RvResultData.hpp"
#include "RvResultPlatform.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "VaCamera.hpp"
#include "VaCameraMotionTethered.hpp"
#include "VaModelDatabase.hpp"
#include "VaTimeManager.hpp"
#include "VaViewerManager.hpp"
#include "WkfDefines.hpp"
#include "WkfScenario.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"

namespace
{
// Returns a vector of available engine data for the given platform.
// Prioritizes finding the engine coordinates for the model used in immersive views,
// then checks the model used in the Map Display.
std::vector<osg::Vec4> GetEngineData(wkf::Platform& aPlatform)
{
   const vespa::VaModelDefinition* modelDef = vaEnv.GetModelDatabase()->GetModelDefinition(aPlatform.GetIcon());
   if (modelDef != nullptr)
   {
      for (const auto& view : vaEnv.GetViewerManager()->GetViewers())
      {
         UtVariant   v    = view->GetData("type");
         std::string type = v.GetString();
         if (type == "immersive")
         {
            return modelDef->GetEngines(vaEnv.GetModelDatabase()->GetViewerModelSet(*view));
         }
      }

      if (vaEnv.GetStandardViewer() != nullptr)
      {
         return modelDef->GetEngines(vaEnv.GetModelDatabase()->GetViewerModelSet(*vaEnv.GetStandardViewer()));
      }
   }

   return std::vector<osg::Vec4>();
}
} // namespace

WKF_PLUGIN_DEFINE_SYMBOLS(RvVisualEffects::Plugin,
                          "Visual Effects",
                          "The Visual Effects plugin adds particle effects to the world.",
                          "mystic")

RvVisualEffects::Plugin::Plugin(const QString& aName, const size_t aUniqueId)
   : rv::Plugin(aName, aUniqueId)
   , mPrefWidgetPtr(new wkf::VisualEffectsPrefWidget)
   , mPrevReadTime(0)
   , mVisibilityNeedsUpdate(false)
{
   mPrefObjectPtr       = mPrefWidgetPtr->GetPreferenceObject();
   mDisplayInterfacePtr = new wkf::VisualEffectsDisplayInterface(this, *mPrefObjectPtr);
}

void RvVisualEffects::Plugin::GuiUpdate()
{
   mDisplayInterfacePtr->Update(vaEnv.GetTime()->GetWallClockTime());
}

QList<wkf::PrefWidget*> RvVisualEffects::Plugin::GetPreferencesWidgets() const
{
   QList<wkf::PrefWidget*> prefs;
   prefs << mPrefWidgetPtr;
   return prefs;
}

void RvVisualEffects::Plugin::AddEffectsBasedOnData(wkf::Platform*        aSourcePlatformPtr,
                                                    const AppearanceData& aPrevData,
                                                    const AppearanceData& aNewData)
{
   switch (aSourcePlatformPtr->GetSpatialDomain())
   {
   case wkf::SpatialDomain::SPATIAL_DOMAIN_AIR:
   {
      switch (aNewData.mAppearance.GetTrailingEffects())
      {
      case 1:
      {
         if (aPrevData.mAppearance.GetTrailingEffects() != 1)
         {
            CreateTrailingEffectOfType<wkf::ContrailEffect>(aSourcePlatformPtr, aNewData);
         }
         break;
      }
      case 2:
      {
         if (aPrevData.mAppearance.GetTrailingEffects() != 2)
         {
            CreateTrailingEffectOfType<wkf::RocketSmokeEffect>(aSourcePlatformPtr, aNewData);
         }
         break;
      }
      case 3:
      {
         if (aPrevData.mAppearance.GetTrailingEffects() != 3)
         {
            CreateTrailingEffectOfType<wkf::DamageSmokeEffect>(aSourcePlatformPtr, aNewData);
         }
         break;
      }
      }
   }
   default:
   {
      break;
   }
   }

   switch (aSourcePlatformPtr->GetSpatialDomain())
   {
   case wkf::SpatialDomain::SPATIAL_DOMAIN_AIR:
   {
      switch (aNewData.mAppearance.GetSmoke())
      {
      case 1:
      {
         if (aPrevData.mAppearance.GetSmoke() != 1 && aPrevData.mAppearance.GetSmoke() != 3)
         {
            CreateTrailingEffectOfType<wkf::SmokePlumeEffect>(aSourcePlatformPtr, aNewData);
         }
         break;
      }
      case 2:
      {
         if (aPrevData.mAppearance.GetSmoke() != 2 && aPrevData.mAppearance.GetSmoke() != 3)
         {
            CreateTrailingEffectOfType<wkf::EngineSmokeEffect>(aSourcePlatformPtr, aNewData);
         }
         break;
      }
      case 3:
      {
         if (aPrevData.mAppearance.GetSmoke() != 1 && aPrevData.mAppearance.GetSmoke() != 3)
         {
            CreateTrailingEffectOfType<wkf::EngineSmokeEffect>(aSourcePlatformPtr, aNewData);
            CreateTrailingEffectOfType<wkf::SmokePlumeEffect>(aSourcePlatformPtr, aNewData);
         }
         break;
      }
      }
   }
   default:
   {
      break;
   }
   }

   switch (aSourcePlatformPtr->GetSpatialDomain())
   {
   case wkf::SpatialDomain::SPATIAL_DOMAIN_AIR:
   {
      if (aNewData.mAppearance.GetFlaming() == 1 && aPrevData.mAppearance.GetFlaming() != 1)
      {
         CreateTrailingEffectOfType<wkf::FireEffect>(aSourcePlatformPtr, aNewData);
      }
   }
   default:
   {
      break;
   }
   }

   switch (aSourcePlatformPtr->GetSpatialDomain())
   {
   case wkf::SpatialDomain::SPATIAL_DOMAIN_AIR:
   {
      if (aNewData.mAppearance.GetAfterburner() == 1 && aPrevData.mAppearance.GetAfterburner() != 1)
      {
         CreateTrailingEffectOfType<wkf::AfterburnerEffect>(aSourcePlatformPtr, aNewData);
      }
   }
   default:
   {
      break;
   }
   }

   // TODO: Launch flash cannot be properly implemented until WsfEntityKind exists.

   switch (aSourcePlatformPtr->GetSpatialDomain())
   {
   case wkf::SpatialDomain::SPATIAL_DOMAIN_AIR:
   {
      if (aNewData.mAppearance.GetPowerplant() == 1 && aPrevData.mAppearance.GetPowerplant() != 1)
      {
         CreateTrailingEffectOfType<wkf::EngineOperatingEffect>(aSourcePlatformPtr, aNewData);
      }
   }
   default:
   {
      break;
   }
   }
}

void RvVisualEffects::Plugin::RemoveEffectsBasedOnData(wkf::Platform*        aSourcePlatformPtr,
                                                       const AppearanceData& aPrevData,
                                                       const AppearanceData& aNewData,
                                                       bool                  aGradual)
{
   switch (aSourcePlatformPtr->GetSpatialDomain())
   {
   case wkf::SpatialDomain::SPATIAL_DOMAIN_AIR:
   {
      switch (aPrevData.mAppearance.GetTrailingEffects())
      {
      case 1:
      {
         if (aNewData.mAppearance.GetTrailingEffects() != 1)
         {
            mDisplayInterfacePtr->RemoveEffectsFromPlatform<wkf::ContrailEffect>(aGradual, *aSourcePlatformPtr);
         }
         break;
      }
      case 2:
      {
         if (aNewData.mAppearance.GetTrailingEffects() != 2)
         {
            mDisplayInterfacePtr->RemoveEffectsFromPlatform<wkf::RocketSmokeEffect>(aGradual, *aSourcePlatformPtr);
         }
         break;
      }
      case 3:
      {
         if (aNewData.mAppearance.GetTrailingEffects() != 3)
         {
            mDisplayInterfacePtr->RemoveEffectsFromPlatform<wkf::DamageSmokeEffect>(aGradual, *aSourcePlatformPtr);
         }
         break;
      }
      }
   }
   default:
   {
      break;
   }
   }

   switch (aSourcePlatformPtr->GetSpatialDomain())
   {
   case wkf::SpatialDomain::SPATIAL_DOMAIN_AIR:
   {
      switch (aPrevData.mAppearance.GetSmoke())
      {
      case 1:
      {
         if (aNewData.mAppearance.GetSmoke() == 3 || aNewData.mAppearance.GetSmoke() == 2)
         {
            mDisplayInterfacePtr->RemoveEffectsFromPlatform<wkf::SmokePlumeEffect>(aGradual, *aSourcePlatformPtr);
         }
         break;
      }
      case 2:
      {
         if (aNewData.mAppearance.GetSmoke() == 3 || aNewData.mAppearance.GetSmoke() == 1)
         {
            mDisplayInterfacePtr->RemoveEffectsFromPlatform<wkf::EngineSmokeEffect>(aGradual, *aSourcePlatformPtr);
         }
         break;
      }
      case 3:
         break;
      }
   }
   default:
   {
      break;
   }
   }

   switch (aSourcePlatformPtr->GetSpatialDomain())
   {
   case wkf::SpatialDomain::SPATIAL_DOMAIN_AIR:
   {
      if (aNewData.mAppearance.GetFlaming() != 1 && aPrevData.mAppearance.GetFlaming() == 1)
      {
         mDisplayInterfacePtr->RemoveEffectsFromPlatform<wkf::FireEffect>(aGradual, *aSourcePlatformPtr);
      }
   }
   default:
   {
      break;
   }
   }

   switch (aSourcePlatformPtr->GetSpatialDomain())
   {
   case wkf::SpatialDomain::SPATIAL_DOMAIN_AIR:
   {
      if (aNewData.mAppearance.GetAfterburner() != 1 && aPrevData.mAppearance.GetAfterburner() == 1)
      {
         mDisplayInterfacePtr->RemoveEffectsFromPlatform<wkf::AfterburnerEffect>(aGradual, *aSourcePlatformPtr);
      }
   }
   default:
   {
      break;
   }
   }

   // TODO: Launch flash cannot be properly implemented until WsfEntityKind exists.

   switch (aSourcePlatformPtr->GetSpatialDomain())
   {
   case wkf::SpatialDomain::SPATIAL_DOMAIN_AIR:
   {
      if (aNewData.mAppearance.GetPowerplant() != 1 && aPrevData.mAppearance.GetPowerplant() == 1)
      {
         mDisplayInterfacePtr->RemoveEffectsFromPlatform<wkf::EngineOperatingEffect>(aGradual, *aSourcePlatformPtr);
      }
   }
   default:
   {
      break;
   }
   }
}

template<class EFFECT_TYPE>
void RvVisualEffects::Plugin::CreateTrailingEffectOfType(wkf::Platform* aSourcePlatformPtr, const AppearanceData& aData)
{
   static_assert(std::is_base_of<wkf::TrailingEffect, EFFECT_TYPE>::value,
                 "Template argument to CreateTrailingEffectOfType in RvPluginVisualEffects must be a subclass of "
                 "wkf::TrailingEffect!");

   auto effect = ut::make_unique<EFFECT_TYPE>();
   if (effect->IsFromEngine())
   {
      std::vector<osg::Vec4> engineData = GetEngineData(*aSourcePlatformPtr);
      if (engineData.empty())
      {
         engineData.emplace_back(osg::Vec4(0, 0, 0, -1));
      }

      for (const auto& engine : engineData)
      {
         effect = ut::make_unique<EFFECT_TYPE>();
         effect->Initialize();
         effect->SetCreationTime(vaEnv.GetTime()->GetWallClockTime());
         effect->SetOffset(osg::Vec3(engine.x(), engine.y(), engine.z()));
         effect->SetSourcePlatform(*aSourcePlatformPtr);

         if (engine.w() >= 0)
         {
            effect->SetInitialParticleSize(engine.w());
         }

         if (effect->IsShort())
         {
            mDisplayInterfacePtr->AddEffect(*aSourcePlatformPtr, std::move(effect));
         }
         else
         {
            wkf::Scenario* scenario = vaEnv.GetStandardScenario();
            if (scenario != nullptr)
            {
               auto entity = new vespa::VaEntity("anchor");

               auto* data = rvEnv.GetData();
               if (data != nullptr)
               {
                  rv::ResultPlatform* plat = data->FindPlatform(aSourcePlatformPtr->GetIndex());
                  if (plat != nullptr)
                  {
                     auto stateMsg = plat->FindFirstAfter<rv::MsgEntityState>(data->GetSimTime());
                     if (stateMsg != nullptr)
                     {
                        rv::Vec3d location = stateMsg->state().locationWCS();
                        double    ecef[3]  = {location.x(), location.y(), location.z()};
                        double    lat, lon, alt;
                        UtEllipsoidalEarth::ConvertECEFToLLA(ecef, lat, lon, alt);

                        entity->SetPosition(vespa::VaPosition(lat, lon, alt));
                     }
                  }
               }

               if (scenario->AddEntity(entity))
               {
                  mDisplayInterfacePtr->AddEffect(*entity, std::move(effect));
               }
               else
               {
                  delete entity;
               }
            }
         }
      }
   }
   else
   {
      effect->Initialize();
      effect->SetCreationTime(vaEnv.GetTime()->GetWallClockTime());
      effect->SetSourcePlatform(*aSourcePlatformPtr);
      if (effect->IsShort())
      {
         mDisplayInterfacePtr->AddEffect(*aSourcePlatformPtr, std::move(effect));
      }
      else
      {
         wkf::Scenario* scenario = vaEnv.GetStandardScenario();
         if (scenario != nullptr)
         {
            auto entity = new vespa::VaEntity("anchor");

            auto* data = rvEnv.GetData();
            if (data != nullptr)
            {
               rv::ResultPlatform* plat = data->FindPlatform(aSourcePlatformPtr->GetIndex());
               if (plat != nullptr)
               {
                  auto stateMsg = plat->FindFirstAfter<rv::MsgEntityState>(data->GetSimTime());
                  if (stateMsg != nullptr)
                  {
                     rv::Vec3d location = stateMsg->state().locationWCS();
                     double    ecef[3]  = {location.x(), location.y(), location.z()};
                     double    lat, lon, alt;
                     UtEllipsoidalEarth::ConvertECEFToLLA(ecef, lat, lon, alt);

                     entity->SetPosition(vespa::VaPosition(lat, lon, alt));
                  }
               }
            }

            if (scenario->AddEntity(entity))
            {
               mDisplayInterfacePtr->AddEffect(*entity, std::move(effect));
            }
            else
            {
               delete entity;
            }
         }
      }
   }
}

void RvVisualEffects::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   if (aData.GetDb() != nullptr)
   {
      if (mDisplayInterfacePtr->IsPaused())
      {
         mDisplayInterfacePtr->SetPaused(false);
      }
      double simTime = aData.GetSimTime();

      // When rewinding, remove/replace trailing effects as necessary, then hide all effects until time moves forward again.
      if (simTime < mPrevReadTime)
      {
         AppearanceData blankData;
         blankData.mTime       = 0;
         blankData.mAppearance = DisEntityAppearance();

         // First, remove any trailing effects if the current time is before those effects were created.
         for (auto& appearanceList : mAppearanceMap)
         {
            // Don't bother trying to remove effects if there's only one appearance entry.
            if (appearanceList.second.size() > 1)
            {
               auto* scenarioPtr = vaEnv.GetStandardScenario();
               if (scenarioPtr)
               {
                  wkf::Platform* platform = scenarioPtr->FindPlatformByIndex(appearanceList.first);
                  if (platform != nullptr)
                  {
                     // The appearance data in the back of the list will always be the platform's most recent appearance.
                     bool appearanceChanged = false;
                     while (appearanceList.second.back().mTime > simTime)
                     {
                        RemoveEffectsBasedOnData(platform, appearanceList.second.back(), blankData, false);
                        appearanceList.second.pop_back();
                        appearanceChanged = true;

                        if (appearanceList.second.empty())
                        {
                           break;
                        }
                     }

                     // If the appearance changed while we were rewinding, and there's still a valid
                     // appearance entry, create new effects based on that appearance.
                     if (appearanceChanged && !appearanceList.second.empty())
                     {
                        AddEffectsBasedOnData(platform, blankData, appearanceList.second.back());
                     }
                  }
               }
            }
         }

         // Lastly, hide all effects.
         mDisplayInterfacePtr->ChangeEffectVisibility<wkf::VisualEffect>(false);
         mVisibilityNeedsUpdate = true;
      }
      else
      {
         if (mVisibilityNeedsUpdate)
         {
            mDisplayInterfacePtr->ChangeEffectVisibility<wkf::ExplosionEffect>(
               mPrefObjectPtr->GetPreferences().mShowExplosions);
            mDisplayInterfacePtr->ChangeEffectVisibility<wkf::ExplosionDebrisEffect>(
               mPrefObjectPtr->GetPreferences().mShowExplosions);
            mDisplayInterfacePtr->ChangeEffectVisibility<wkf::IconifiedExplosionEffect>(
               mPrefObjectPtr->GetPreferences().mShowExplosions);
            mDisplayInterfacePtr->ChangeEffectVisibility<wkf::TrailingEffect>(
               mPrefObjectPtr->GetPreferences().mShowTrailingEffects);
            mVisibilityNeedsUpdate = false;
         }

         if (aData.GetDb()->HasMessagesOfType(rv::MsgPlatformAppearanceChanged::cMESSAGE_ID))
         {
            const rv::MessageBaseArray& mb =
               aData.GetDb()->LockMessagesOfType(rv::MsgPlatformAppearanceChanged::cMESSAGE_ID);
            UtCircularBufferIteratorConst<rv::MsgBase*> it = mb.FindFirstIteratorAfter(mPrevReadTime);

            auto* scenarioPtr = vaEnv.GetStandardScenario();
            if (scenarioPtr)
            {
               while ((it != mb.end()) && ((*it)->simTime() < simTime))
               {
                  rv::MsgPlatformAppearanceChanged* msg      = dynamic_cast<rv::MsgPlatformAppearanceChanged*>(*it);
                  wkf::Platform*                    platform = scenarioPtr->FindPlatformByIndex(msg->ownerIndex());
                  if (platform != nullptr)
                  {
                     AppearanceData newData;
                     newData.mTime       = simTime;
                     newData.mAppearance = msg->appearance();

                     AppearanceData prevData;
                     prevData.mTime       = 0;
                     prevData.mAppearance = DisEntityAppearance();

                     auto appearances = mAppearanceMap.find(platform->GetIndex());
                     if (appearances != mAppearanceMap.end())
                     {
                        if (!appearances->second.empty())
                        {
                           prevData = appearances->second.back();
                        }
                     }

                     RemoveEffectsBasedOnData(platform, prevData, newData, true);
                     AddEffectsBasedOnData(platform, prevData, newData);

                     if (mAppearanceMap.find(platform->GetIndex()) != mAppearanceMap.end())
                     {
                        mAppearanceMap.at(platform->GetIndex()).emplace_back(newData);
                     }
                     else
                     {
                        std::vector<AppearanceData> appearanceList;
                        appearanceList.emplace_back(newData);
                        mAppearanceMap.emplace(platform->GetIndex(), appearanceList);
                     }
                  }

                  ++it;
               }
            }
            aData.GetDb()->UnlockMessagesOfType(rv::MsgPlatformAppearanceChanged::cMESSAGE_ID);
         }

         if (aData.GetDb()->HasMessagesOfType(rv::MsgWeaponTerminated::cMESSAGE_ID) &&
             mPrefObjectPtr->GetPreferences().mShowExplosions)
         {
            const rv::MessageBaseArray& mb = aData.GetDb()->LockMessagesOfType(rv::MsgWeaponTerminated::cMESSAGE_ID);
            UtCircularBufferIteratorConst<rv::MsgBase*> it = mb.FindFirstIteratorAfter(mPrevReadTime);

            auto* scenarioPtr = vaEnv.GetStandardScenario();
            if (scenarioPtr)
            {
               while ((it != mb.end()) && ((*it)->simTime() < simTime))
               {
                  rv::MsgWeaponTerminated* msg  = dynamic_cast<rv::MsgWeaponTerminated*>(*it);
                  std::string              desc = "";
                  vespa::VaPosition        position;

                  // If engagement resulted in an explosion, attempt to find explosion position and generate description.
                  if (msg->geometryResult() != rv::MsgWeaponTerminated_GeometryResult::in_progress &&
                      msg->geometryResult() != rv::MsgWeaponTerminated_GeometryResult::dud &&
                      msg->geometryResult() != rv::MsgWeaponTerminated_GeometryResult::part_disabled_or_destroyed)
                  {
                     // Any result can draw explosion at weapon.
                     if (msg->weaponPlatformIndexValid())
                     {
                        position = GetPositionAtTime(msg->simTime(), msg->weaponPlatformIndex(), aData.GetDb());
                        std::string weapon = scenarioPtr->FindPlatformByIndex(msg->weaponPlatformIndex())->GetName();
                        desc               = std::to_string(msg->simTime()) + ": " + weapon + " detonated";
                        if (msg->geometryResult() == rv::MsgWeaponTerminated_GeometryResult::far_away_in_air ||
                            msg->geometryResult() == rv::MsgWeaponTerminated_GeometryResult::far_away_above_ground ||
                            msg->geometryResult() == rv::MsgWeaponTerminated_GeometryResult::far_away_ground_impact)
                        {
                           desc += " and missed";
                        }
                        else if (msg->targetPlatformIndexValid())
                        {
                           std::string target = scenarioPtr->FindPlatformByIndex(msg->targetPlatformIndex())->GetName();
                           desc += " and hit " + target;
                        }
                     }
                     // Close explosions can be drawn at target if no weapon platform.
                     else if (msg->targetPlatformIndexValid() &&
                              ((msg->geometryResult() == rv::MsgWeaponTerminated_GeometryResult::target_impact) ||
                               (msg->geometryResult() ==
                                rv::MsgWeaponTerminated_GeometryResult::target_proximity_above_ground) ||
                               (msg->geometryResult() == rv::MsgWeaponTerminated_GeometryResult::target_proximity_air_burst) ||
                               (msg->geometryResult() ==
                                rv::MsgWeaponTerminated_GeometryResult::target_proximity_ground_impact)))
                     {
                        position = GetPositionAtTime(msg->simTime(), msg->targetPlatformIndex(), aData.GetDb());
                        std::string target = scenarioPtr->FindPlatformByIndex(msg->targetPlatformIndex())->GetName();
                        desc               = std::to_string(msg->simTime()) + ": " + target + " was hit by a weapon";
                     }
                  }

                  if (desc != "")
                  {
                     auto explosionEntity = new vespa::VaEntity(desc);
                     explosionEntity->SetPosition(position);
                     if (scenarioPtr->AddEntity(explosionEntity))
                     {
                        auto explosion = ut::make_unique<wkf::ExplosionEffect>();
                        explosion->Initialize();
                        explosion->SetCreationTime(vaEnv.GetTime()->GetWallClockTime());

                        mDisplayInterfacePtr->AddEffect(*explosionEntity, std::move(explosion));
                     }
                     else
                     {
                        delete explosionEntity;
                     }

                     auto debrisEntity = new vespa::VaEntity(desc);
                     debrisEntity->SetPosition(position);
                     if (scenarioPtr->AddEntity(debrisEntity))
                     {
                        auto debris = ut::make_unique<wkf::ExplosionDebrisEffect>();
                        debris->Initialize();
                        debris->SetCreationTime(vaEnv.GetTime()->GetWallClockTime());

                        mDisplayInterfacePtr->AddEffect(*debrisEntity, std::move(debris));
                     }
                     else
                     {
                        delete debrisEntity;
                     }

                     auto iconifiedEntity = new vespa::VaEntity(desc);
                     iconifiedEntity->SetPosition(position);
                     if (scenarioPtr->AddEntity(iconifiedEntity))
                     {
                        auto iconifiedExplosion = ut::make_unique<wkf::IconifiedExplosionEffect>();
                        iconifiedExplosion->Initialize();
                        iconifiedExplosion->SetCreationTime(vaEnv.GetTime()->GetWallClockTime());

                        mDisplayInterfacePtr->AddEffect(*iconifiedEntity, std::move(iconifiedExplosion));
                     }
                     else
                     {
                        delete iconifiedEntity;
                     }
                  }

                  ++it;
               }
            }
            aData.GetDb()->UnlockMessagesOfType(rv::MsgWeaponTerminated::cMESSAGE_ID);
         }

         if (aData.GetDb()->HasMessagesOfType(rv::MsgPlatformStatus::cMESSAGE_ID))
         {
            const rv::MessageBaseArray& mb = aData.GetDb()->LockMessagesOfType(rv::MsgPlatformStatus::cMESSAGE_ID);
            UtCircularBufferIteratorConst<rv::MsgBase*> it = mb.FindFirstIteratorAfter(mPrevReadTime);

            while ((it != mb.end()) && ((*it)->simTime() < simTime))
            {
               auto* msg = dynamic_cast<rv::MsgPlatformStatus*>(*it);
               if (msg->removed())
               {
                  auto* scenarioPtr = vaEnv.GetStandardScenario();
                  if (scenarioPtr)
                  {
                     auto* platform = scenarioPtr->FindPlatformByIndex(msg->platformIndex());
                     if (platform != nullptr)
                     {
                        mDisplayInterfacePtr->RemoveEffectsFromPlatform<wkf::TrailingEffect>(true, *platform);
                     }
                  }
               }
               ++it;
            }
            aData.GetDb()->UnlockMessagesOfType(rv::MsgPlatformStatus::cMESSAGE_ID);
         }
      }

      mPrevReadTime = simTime;
   }
}

void RvVisualEffects::Plugin::RegularRead(const rv::ResultData& aData)
{
   if (!mDisplayInterfacePtr->IsPaused())
   {
      mDisplayInterfacePtr->SetPaused(true);
      mVisibilityNeedsUpdate = true;
   }
}

void RvVisualEffects::Plugin::PlatformAddedRead(double aSimTime, rv::ResultDb& aData, const rv::MsgPlatformInfo& aPlatform)
{
   AppearanceData blankData;
   blankData.mTime       = 0;
   blankData.mAppearance = DisEntityAppearance();

   std::vector<AppearanceData> appearanceList;
   appearanceList.emplace_back(blankData);
   mAppearanceMap.emplace(aPlatform.platformIndex(), appearanceList);
}

vespa::VaPosition RvVisualEffects::Plugin::GetPositionAtTime(double aTime, int aPlatformIndex, rv::ResultDb* aDatabase)
{
   rv::ResultPlatform* plat = aDatabase->FindPlatform(aPlatformIndex);
   auto                msg  = plat->FindFirstBefore<rv::MsgEntityState>(aTime);

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
   return vespa::VaPosition(0.0, 0.0, 0.0);
}
