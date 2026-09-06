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

#include "effects/WsfCyberWeaponsEffect.hpp"

#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackManager.hpp"

namespace wsf
{

namespace cyber
{

// =================================================================================================
WeaponsEffect::WeaponsEffect(EffectType aType)
   : Effect()
   , mTargetTrack(nullptr)
   , mPreviousTargetTrack()
   , mType(aType)
{
}

// =================================================================================================
WeaponsEffect::WeaponsEffect(const WeaponsEffect& aSrc, const AttackParameters& aParameters)
   : Effect(aSrc)
   , mTargetTrack(aSrc.mTargetTrack)
   , mPreviousTargetTrack(aSrc.mPreviousTargetTrack)
   , mType(aSrc.mType)
{
   // This type only expects a single track object. Any additional parameters for this
   // effect type is an error.
   auto entryVecPtr = aParameters.GetEffectEntries(GetType());
   if (!entryVecPtr || entryVecPtr->size() != RequiredInputCount())
   {
      throw UtException("Missing or invalid user provided parameters for WsfCyberWeaponsEffect");
   }

   entryVecPtr->front().GetParameter(GetType(), mTargetTrack);
}

// =================================================================================================
Effect* WeaponsEffect::Clone() const
{
   return new WeaponsEffect(*this);
}

// =================================================================================================
Effect* WeaponsEffect::Clone(const AttackParameters& aParameters) const
{
   return new WeaponsEffect(*this, aParameters);
}

// =================================================================================================
bool WeaponsEffect::Initialize(Engagement&)
{
   return true;
}

// =================================================================================================
bool WeaponsEffect::ProcessInput(UtInput&)
{
   return false;
}

// =================================================================================================
void WeaponsEffect::Attack(double aSimTime, Engagement& aEngagement)
{
   if (mType == cRETARGET)
   {
      const auto&  sim     = aEngagement.GetSimulation();
      WsfPlatform* victim  = sim.GetPlatformByName(aEngagement.GetVictim());
      mPreviousTargetTrack = victim->GetTrackManager().GetCurrentTargetTrackId();

      victim->GetTrackManager().SetCurrentTarget(aSimTime, *mTargetTrack);
   }
   else if (mType == cUNTARGETED)
   {
      const auto&  sim     = aEngagement.GetSimulation();
      WsfPlatform* victim  = sim.GetPlatformByName(aEngagement.GetVictim());
      mPreviousTargetTrack = victim->GetTrackManager().GetCurrentTargetTrackId();

      victim->GetTrackManager().ClearCurrentTarget();
   }
   else
   {
      // Incorrect type
      throw UtException("Error, incorrect typing of WsfCyberWeapons during Attack call");
   }
}

// =================================================================================================
void WeaponsEffect::Restore(double aSimTime, Engagement& aEngagement)
{
   if (mType == cRETARGET)
   {
      const auto&  sim    = aEngagement.GetSimulation();
      WsfPlatform* victim = sim.GetPlatformByName(aEngagement.GetVictim());
      if (!mPreviousTargetTrack.IsNull())
      {
         WsfTrack* prevTrack = victim->GetTrackManager().FindTrack(mPreviousTargetTrack);
         if (prevTrack)
         {
            victim->GetTrackManager().SetCurrentTarget(aSimTime, *prevTrack);
         }
         else
         {
            victim->GetTrackManager().ClearCurrentTarget();
         }
      }
      else
      {
         victim->GetTrackManager().ClearCurrentTarget();
      }
   }
   else if (mType == cUNTARGETED)
   {
      const auto&  sim    = aEngagement.GetSimulation();
      WsfPlatform* victim = sim.GetPlatformByName(aEngagement.GetVictim());
      if (!mPreviousTargetTrack.IsNull())
      {
         WsfTrack* prevTrack = victim->GetTrackManager().FindTrack(mPreviousTargetTrack);
         if (prevTrack)
         {
            victim->GetTrackManager().SetCurrentTarget(aSimTime, *prevTrack);
         }
      }
   }
   else
   {
      // Incorrect type
      throw UtException("Error, incorrect typing of WsfCyberWeapon during Restore call");
   }
}

// =================================================================================================
Effect::InputRequirement WeaponsEffect::RequiredInput() const
{
   if (mType == cRETARGET)
   {
      return InputRequirement::cREQUIRED;
   }
   else if (mType == cUNTARGETED)
   {
      return InputRequirement::cNOT_ALLOWED;
   }
   throw UtException("Error, incorrect typing of WsfCyberWeapon");
}

// =================================================================================================
size_t WeaponsEffect::RequiredInputCount() const
{
   if (mType == cRETARGET)
   {
      return 1U;
   }
   else if (mType == cUNTARGETED)
   {
      return 0U;
   }
   throw UtException("Error, incorrect typing of WsfCyberWeapon");
}

} // namespace cyber
} // namespace wsf
