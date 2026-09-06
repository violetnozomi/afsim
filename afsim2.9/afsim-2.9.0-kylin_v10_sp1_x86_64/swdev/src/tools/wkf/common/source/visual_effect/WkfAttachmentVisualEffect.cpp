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

#include "WkfAttachmentVisualEffect.hpp"

#include "UtoShaders.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"

wkf::AttachmentVisualEffect::AttachmentVisualEffect(vespa::VaEntity&   aEntity,
                                                    vespa::VaViewer*   aViewerPtr,
                                                    wkf::VisualEffect& aEffect,
                                                    const std::string& aDesc)
   : VaAttachment(aEntity, aViewerPtr, "effect")
   , mDesc(aDesc)
   , mEffect(&aEffect)
   , mEffectShapePtr(nullptr)
{
   SetType<AttachmentVisualEffect>();
   UtoRawShape effectShape;
   mEffectShapePtr = dynamic_cast<UtoRawShape*>(AddShapeToParent("effect " + std::to_string(GetUniqueId()), effectShape));

   // Attach the effect to this shape.
   if (mEffectShapePtr != nullptr)
   {
      mEffect->Attach(mEffectShapePtr->GetTree(), aEntity);
   }

   // Trailing effects are positioned relative to the world at the platform's location
   // in order to center their particle placers according to the platform's ECEF coordinates.
   if (dynamic_cast<wkf::TrailingEffect*>(mEffect) != nullptr)
   {
      mEffectShapePtr->SetReferenceFrame(UtoShape::cFRAME_WORLD_AT_ENTITY);
   }
   else
   {
      mEffectShapePtr->SetReferenceFrame(UtoShape::cFRAME_ENTITY);
      mEffectShapePtr->SetShapeFlags(UtoShape::BILLBOARD, UtoShape::BILLBOARD);
   }

   // Add the particle_effect shader.
   std::set<std::string> defset;
   osg::Program* particle_shader = UtoShaders::UseProgram("particle_effect.vert", "particle_effect.frag", defset);
   mEffectShapePtr->GetRoot()->getOrCreateStateSet()->setAttribute(particle_shader, osg::StateAttribute::ON);

   mCallbacks.Add(vespa::VaObserver::EntityMoved.Connect(
      [this](vespa::VaEntity* aEntity, bool aMoveInTime)
      {
         wkf::TrailingEffect* effect = dynamic_cast<wkf::TrailingEffect*>(mEffect);
         if (effect != nullptr)
         {
            wkf::Platform* platform = effect->GetSourcePlatform();
            if (platform != nullptr)
            {
               if (aEntity->GetUniqueId() == platform->GetUniqueId())
               {
                  mEffect->Update(vaEnv.GetDisplayedTime());
               }
            }
         }
      }));
}
