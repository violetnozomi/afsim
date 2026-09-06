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

#ifndef WKFVISUALEFFECTSDISPLAYINTERFACE_HPP
#define WKFVISUALEFFECTSDISPLAYINTERFACE_HPP

#include "wkf_common_export.h"

#include "VaCallbackHolder.hpp"
#include "VaEntity.hpp"
#include "WkfAttachmentVisualEffect.hpp"
#include "WkfViewer.hpp"
#include "WkfVisualEffects.hpp"
#include "WkfVisualEffectsPrefObject.hpp"

namespace wkf
{
class WKF_COMMON_EXPORT VisualEffectsDisplayInterface : public QObject
{
   Q_OBJECT

public:
   VisualEffectsDisplayInterface(QObject* aParent, const VisualEffectsPrefObject& aPrefObject);

   // Looks at each effect and determines whether to delete it based on lifespan.
   void Update(double aTime);

   // Attaches the given effect to the given entity.
   void AddEffect(vespa::VaEntity& aAnchorEntity, std::unique_ptr<VisualEffect> aEffect);

   // Pauses or unpauses each effect.
   void SetPaused(bool aPaused);
   bool IsPaused() { return mPaused; }

   // Removes all effects of the given type from the given platform. If aGradual is true,
   // the effect's emitter is turned off and its duration is set to 0; it will eventually
   // be deleted in Update() once all the remaining particles are dead. Otherwise,
   // the effect will be deleted immediately.
   template<class EFFECT_TYPE>
   void RemoveEffectsFromPlatform(bool aGradual, wkf::Platform& aPlatform)
   {
      bool attachedToPlatform = false;
      for (auto&& effectPairing = mEffectsMap.begin(); effectPairing != mEffectsMap.end();)
      {
         attachedToPlatform = false;
         if (dynamic_cast<wkf::Platform*>(effectPairing->first))
         {
            attachedToPlatform = true;
         }

         for (auto&& effect = effectPairing->second.begin(); effect != effectPairing->second.end();)
         {
            TrailingEffect* trailEffect = dynamic_cast<TrailingEffect*>(effect->get());
            if (trailEffect != nullptr && dynamic_cast<EFFECT_TYPE*>(trailEffect) != nullptr)
            {
               if (trailEffect->GetSourcePlatform()->GetIndex() == aPlatform.GetIndex())
               {
                  // Turn the emitter off and set the duration to 0.
                  if (aGradual && !attachedToPlatform)
                  {
                     (*effect)->KillGradually();
                     ++effect;
                  }
                  else
                  {
                     // Remove the effect immediately.
                     RemoveAttachments(*effectPairing->first, *effect->get());
                     effect = effectPairing->second.erase(effect);
                  }
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

   // Hides or reveals each effect of a given type.
   template<class EFFECT_TYPE>
   void ChangeEffectVisibility(bool aShow)
   {
      for (auto& effectPairing : mEffectsMap)
      {
         for (auto& effect : effectPairing.second)
         {
            if (dynamic_cast<EFFECT_TYPE*>(effect.get()) != nullptr)
            {
               std::vector<wkf::AttachmentVisualEffect*> attachments =
                  effectPairing.first->FindAttachmentsOfType<wkf::AttachmentVisualEffect>();
               for (auto& attachment : attachments)
               {
                  if (attachment->GetEffect() == effect.get())
                  {
                     attachment->SetStateVisibility(aShow);
                     if (!aShow)
                     {
                        effect->KillAllParticles();
                     }
                  }
               }
            }
         }
      }
   }

private:
   void CreateAttachment(vespa::VaEntity& aEntity, VisualEffect& aEffect, vespa::VaViewer& aViewer);
   void RemoveAttachments(vespa::VaEntity& aEntity, VisualEffect& aEffect);

   void MapExplosionScaleChangedCB(double aScalar);
   void MapExplosionLifetimeChangedCB(double aLifetime);
   void ViewerInitializedCB(vespa::VaViewer* aViewer);
   void PlatformPendingDeletionCB(wkf::Platform* aPlatform);
   void StandardScenarioRemovedCB();

   bool mPaused;

   std::map<vespa::VaEntity*, std::vector<std::unique_ptr<VisualEffect>>> mEffectsMap; // Maps anchor entities to a list of effects.

   const VisualEffectsPrefObject* mPrefObject;
   vespa::VaCallbackHolder        mCallbacks;
};
} // namespace wkf

#endif // !WKFVISUALEFFECTSDISPLAYINTERFACE_HPP
