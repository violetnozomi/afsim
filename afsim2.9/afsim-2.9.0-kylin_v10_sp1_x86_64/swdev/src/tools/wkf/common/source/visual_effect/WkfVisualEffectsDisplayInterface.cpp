// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfVisualEffectsDisplayInterface.hpp"

#include "VaModelDatabase.hpp"
#include "VaObserver.hpp"
#include "VaUtils.hpp"
#include "VaViewerManager.hpp"
#include "WkfObserver.hpp"
#include "WkfScenario.hpp"

wkf::VisualEffectsDisplayInterface::VisualEffectsDisplayInterface(QObject*                            aParent,
                                                                  const wkf::VisualEffectsPrefObject& aPrefObject)
   : QObject(aParent)
   , mPaused(false)
   , mPrefObject(&aPrefObject)
{
   mCallbacks.Add(
      wkf::Observer::StandardScenarioRemoved.Connect([this](Scenario* aScenario) { StandardScenarioRemovedCB(); }));
   mCallbacks.Add(wkf::Observer::PlatformPendingDeletion.Connect([this](Platform* aPlatform)
                                                                 { PlatformPendingDeletionCB(aPlatform); }));
   mCallbacks.Add(
      vespa::VaObserver::ViewerInitialized.Connect([this](vespa::VaViewer* aViewer) { ViewerInitializedCB(aViewer); }));

   connect(mPrefObject,
           &VisualEffectsPrefObject::ShowExplosionsChanged,
           [this](bool aShow)
           {
              ChangeEffectVisibility<ExplosionEffect>(aShow);
              ChangeEffectVisibility<ExplosionDebrisEffect>(aShow);
              ChangeEffectVisibility<IconifiedExplosionEffect>(aShow);
           });
   connect(mPrefObject,
           &VisualEffectsPrefObject::ShowTrailingEffectsChanged,
           [this](bool aShow) { ChangeEffectVisibility<TrailingEffect>(aShow); });
   connect(mPrefObject,
           &VisualEffectsPrefObject::MapExplosionScaleChanged,
           [this](double aScalar) { MapExplosionScaleChangedCB(aScalar); });
   connect(mPrefObject,
           &VisualEffectsPrefObject::MapExplosionLifetimeChanged,
           [this](double aLifetime) { MapExplosionLifetimeChangedCB(aLifetime); });
}

void wkf::VisualEffectsDisplayInterface::Update(double aTime)
{
   for (auto&& effectPairing = mEffectsMap.begin(); effectPairing != mEffectsMap.end();)
   {
      for (auto&& effect = effectPairing->second.begin(); effect != effectPairing->second.end();)
      {
         if (!(*effect)->IsEndless())
         {
            // Kill the effect if it has outlived its duration.
            if (aTime > ((*effect)->GetCreationTime() + (*effect)->GetDuration()))
            {
               (*effect)->KillGradually();
            }

            // Remove the effect if it's dead.
            if ((*effect)->IsDead())
            {
               // First, remove any attachments that use this effect.
               RemoveAttachments(*effectPairing->first, *effect->get());

               // Next, remove the effect from the map.
               effect = effectPairing->second.erase(effect);
            }
            else
            {
               ++effect;
            }
         }
         else
         {
            ++effect;
         }
      }

      // Finally, if this entity is not a platform and it doesn't have any effects,
      // get rid of it.
      if (effectPairing->second.empty())
      {
         bool attachedToPlatform = false;
         if (dynamic_cast<wkf::Platform*>(effectPairing->first))
         {
            attachedToPlatform = true;
         }

         if (!attachedToPlatform)
         {
            vaEnv.DeleteEntity(effectPairing->first);
         }
         effectPairing = mEffectsMap.erase(effectPairing);
      }
      else
      {
         ++effectPairing;
      }
   }
}

void wkf::VisualEffectsDisplayInterface::AddEffect(vespa::VaEntity& aEntity, std::unique_ptr<VisualEffect> aEffect)
{
   // First, add the effect to the map.
   auto effectPairing = mEffectsMap.find(&aEntity);
   if (effectPairing != mEffectsMap.end())
   {
      effectPairing->second.emplace_back(std::move(aEffect));
   }
   else
   {
      std::vector<std::unique_ptr<VisualEffect>> effectList;
      effectList.emplace_back(std::move(aEffect));
      mEffectsMap.emplace(&aEntity, std::move(effectList));
   }

   // Next, create attachments for the new effect in each viewer
   // it wants to be displayed in.
   VisualEffect* effect = mEffectsMap.at(&aEntity).back().get();
   switch (effect->GetDisplayOption())
   {
   case VisualEffect::ViewerDisplayOption::cMAP_VIEW:
   {
      if (vaEnv.GetStandardViewer() != nullptr)
      {
         CreateAttachment(aEntity, *effect, *vaEnv.GetStandardViewer());
      }
      break;
   }
   case VisualEffect::ViewerDisplayOption::cIMMERSIVE_VIEWS:
   {
      for (auto& viewer : vaEnv.GetViewerManager()->GetViewers())
      {
         UtVariant   v    = viewer->GetData("type");
         std::string type = v.GetString();
         if (type == "immersive")
         {
            CreateAttachment(aEntity, *effect, *viewer);
         }
      }
      break;
   }
   case VisualEffect::ViewerDisplayOption::cALL_VIEWS:
   {
      for (auto& viewer : vaEnv.GetViewerManager()->GetViewers())
      {
         CreateAttachment(aEntity, *effect, *viewer);
      }
      break;
   }
   default:
   {
      break;
   }
   }
}

void wkf::VisualEffectsDisplayInterface::SetPaused(bool aPaused)
{
   for (auto& effectPairing : mEffectsMap)
   {
      for (auto& effect : effectPairing.second)
      {
         effect->SetPaused(aPaused);
      }
   }

   mPaused = aPaused;
}

void wkf::VisualEffectsDisplayInterface::CreateAttachment(vespa::VaEntity&   aEntity,
                                                          wkf::VisualEffect& aEffect,
                                                          vespa::VaViewer&   aViewer)
{
   auto attachment =
      vespa::make_attachment<wkf::AttachmentVisualEffect>(aEntity, &aViewer, aEffect, aEntity.GetDescription());
   attachment->Initialize();

   // Determine whether to show or to hide the effect based on preferences.
   bool showEffect = true;
   if (dynamic_cast<wkf::TrailingEffect*>(&aEffect) != nullptr)
   {
      showEffect = mPrefObject->GetPreferences().mShowTrailingEffects;
   }
   else if (dynamic_cast<wkf::ExplosionEffect*>(&aEffect) != nullptr ||
            dynamic_cast<wkf::ExplosionDebrisEffect*>(&aEffect) != nullptr ||
            dynamic_cast<wkf::IconifiedExplosionEffect*>(&aEffect) != nullptr)
   {
      showEffect = mPrefObject->GetPreferences().mShowExplosions;
   }
   attachment->SetStateVisibility(showEffect);
}

void wkf::VisualEffectsDisplayInterface::RemoveAttachments(vespa::VaEntity& aEntity, wkf::VisualEffect& aEffect)
{
   std::vector<wkf::AttachmentVisualEffect*> attachments = aEntity.FindAttachmentsOfType<wkf::AttachmentVisualEffect>();
   for (auto& attachment : attachments)
   {
      if (attachment->GetEffect() == &aEffect)
      {
         aEntity.RemoveAttachment(attachment->GetUniqueId());
      }
   }
}

void wkf::VisualEffectsDisplayInterface::MapExplosionScaleChangedCB(double aScalar)
{
   for (auto& effectPairing : mEffectsMap)
   {
      for (auto& effect : effectPairing.second)
      {
         if (dynamic_cast<IconifiedExplosionEffect*>(effect.get()) != nullptr)
         {
            effect->ScaleParticles(aScalar);
         }
      }
   }
}

void wkf::VisualEffectsDisplayInterface::MapExplosionLifetimeChangedCB(double aScalar)
{
   for (auto& effectPairing : mEffectsMap)
   {
      for (auto& effect : effectPairing.second)
      {
         if (dynamic_cast<IconifiedExplosionEffect*>(effect.get()) != nullptr)
         {
            effect->SetParticleLifetime(aScalar);
         }
      }
   }
}

void wkf::VisualEffectsDisplayInterface::ViewerInitializedCB(vespa::VaViewer* aViewer)
{
   for (auto& effectPairing : mEffectsMap)
   {
      for (auto& effect : effectPairing.second)
      {
         switch (effect->GetDisplayOption())
         {
         case VisualEffect::ViewerDisplayOption::cMAP_VIEW:
         {
            if (aViewer == vaEnv.GetStandardViewer())
            {
               CreateAttachment(*effectPairing.first, *effect.get(), *aViewer);
            }
            break;
         }
         case VisualEffect::ViewerDisplayOption::cIMMERSIVE_VIEWS:
         {
            UtVariant   v    = aViewer->GetData("type");
            std::string type = v.GetString();
            if (type == "immersive")
            {
               CreateAttachment(*effectPairing.first, *effect.get(), *aViewer);
            }
            break;
         }
         case VisualEffect::ViewerDisplayOption::cALL_VIEWS:
         {
            CreateAttachment(*effectPairing.first, *effect.get(), *aViewer);
            break;
         }
         }
      }
   }
}

void wkf::VisualEffectsDisplayInterface::PlatformPendingDeletionCB(wkf::Platform* aPlatform)
{
   RemoveEffectsFromPlatform<wkf::TrailingEffect>(true, *aPlatform);
}

void wkf::VisualEffectsDisplayInterface::StandardScenarioRemovedCB()
{
   mEffectsMap.clear();
}
