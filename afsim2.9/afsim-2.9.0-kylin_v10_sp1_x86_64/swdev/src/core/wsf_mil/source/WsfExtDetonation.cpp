// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfExtDetonation.hpp"

#include "UtLog.hpp"
#include "UtRandom.hpp"
#include "WsfExplicitWeaponEffects.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMil.hpp"
#include "WsfMilExtInterface.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackManager.hpp"
#include "WsfUtil.hpp"
#include "WsfWeaponEffects.hpp"
#include "WsfWeaponEffectsTypes.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponObserver.hpp"

namespace
{
enum ImpactCode
{
   // If you add entries, also update PerformSimpleDetonations

   cDIRECT_HIT,   //!< The weapon hit the intended target
   cDETONATED,    //!< The weapon detonated, but may or may not have killed something
   cNO_DETONATION //!< The weapon failed to detonate
};

// DIS Detonation result data - indexed by DIS result code.
// The impact code is used to guide the simple detonation logic
struct ResultCode
{
   const char* mString;
   int         mImpactCode;
} sResultCodes[] = {
   /*  0 */ {"Other", cNO_DETONATION},
   /*  1 */ {"Entity Impact", cDIRECT_HIT},
   /*  2 */ {"Entity Proximate Detonation", cDETONATED},
   /*  3 */ {"Ground Impact", cDETONATED},
   /*  4 */ {"Ground Proximate Detonation", cDETONATED},
   /*  5 */ {"Detonation", cDETONATED},
   /*  6 */ {"None or No Detonation", cNO_DETONATION},
   /*  7 */ {"HE hit, small", cDIRECT_HIT},
   /*  8 */ {"HE hit, medium", cDIRECT_HIT},
   /*  9 */ {"HE hit, large", cDIRECT_HIT},
   /* 10 */ {"Armor-piercing hit", cDIRECT_HIT},
   /* 11 */ {"Dirt blast, small", cDETONATED},
   /* 12 */ {"Dirt blast, medium", cDETONATED},
   /* 13 */ {"Dirt blast, large", cDETONATED},
   /* 14 */ {"Water blast, small", cDETONATED},
   /* 15 */ {"Water blast, medium", cDETONATED},
   /* 16 */ {"Water blast, large", cDETONATED},
   /* 17 */ {"Air hit", cDETONATED},
   /* 18 */ {"Building hit, small", cDIRECT_HIT},
   /* 19 */ {"Building hit, medium", cDIRECT_HIT},
   /* 20 */ {"Building hit, large", cDIRECT_HIT},
   /* 21 */ {"Mine-clearing line charge", cDETONATED},
   /* 22 */ {"Environment object impact", cDETONATED},
   /* 23 */ {"Environment object proximate detonation", cDETONATED},
   /* 24 */ {"Water impact", cDETONATED},
   /* 25 */ {"Air burst", cDETONATED},
   /* 26 */ {"Kill with fragment type 1", cDIRECT_HIT},
   /* 27 */ {"Kill with fragment type 2", cDIRECT_HIT},
   /* 28 */ {"Kill with fragment type 3", cDIRECT_HIT},
   /* 29 */ {"Kill with fragment type 1 after fly-out failure", cDIRECT_HIT},
   /* 30 */ {"Kill with fragment type 2 after fly-out failure", cDIRECT_HIT},
   /* 31 */ {"Miss due to fly-out failure", cNO_DETONATION},
   /* 32 */ {"Miss due to end-game failure", cNO_DETONATION},
   /* 33 */ {"Miss due to fly-out and end-game failure", cNO_DETONATION}

};
} // namespace

// =================================================================================================
WsfExtDetonation::WsfExtDetonation(WsfExtInterface* aInterfacePtr)
   : mInterfacePtr(aInterfacePtr)
   , mFiringEntityPtr(nullptr)
   , mTargetEntityPtr(nullptr)
   , mWeaponEntityPtr(nullptr)
   , mResult(cENTITY_IMPACT)
{
   mMilInterfacePtr = WsfMilExtInterface::Find(*aInterfacePtr);
}

bool WsfExtDetonation::DebugEnabled()
{
   return (mInterfacePtr->mDebugDetonationEnabled /* || (WsfDisWeaponEvents::GetDebugMask() & 1)*/);
}

// =================================================================================================
std::string WsfExtDetonation::GetDefaultWeaponEffectName()
{
   return mMilInterfacePtr->GetExternalWeaponEffectsType();
}
//
// // =================================================================================================
// //static
// void WsfExtDetonation::Reset()
// {
//    sSimpleKillRange = 100.0;
//    sUseSimpleDetonations = true;
//    sDebugEnabled = false;
//    sExcludedPlatformTypes.clear();
//    sExcludedWeaponEffects.clear();
//    sTargetPriority.clear();
//    sPK_Table.clear();
//    mInterfacePtr->mExternalWeaponEffectsType.clear();
// }

// =================================================================================================
std::string WsfExtDetonation::GetResultName() const
{
   std::string resultName;
   if (mResult < cRESULT_CODE_COUNT)
   {
      resultName = sResultCodes[mResult].mString;
   }
   else
   {
      resultName = "None";
   }
   return resultName;
}

// =================================================================================================
void WsfExtDetonation::Execute(double aSimTime, Result& aResult)
{
   // Ignore detonation if the weapon is not external or the target is not available
   bool localWeapon = (mWeaponEntityPtr != nullptr) && !mWeaponEntityPtr->IsExternallyControlled();
   if (mWeaponEntityPtr == nullptr)
   {
      localWeapon = (mFiringEntityPtr != nullptr) && (!mFiringEntityPtr->IsExternallyControlled());
   }
   bool targetInvalid = (mTargetEntityPtr != nullptr) && mTargetEntityPtr->IsExternallyControlled();
   if (mTargetEntityPtr == nullptr)
   {
      targetInvalid = mDetonateLocationWCS.Equals(0.0);
   }
   aResult.mIgnored = (localWeapon || targetInvalid);
   if (aResult.mIgnored)
   {
      if (targetInvalid && DebugEnabled())
      {
         PrintDetonation(aSimTime, aResult);
      }
      return;
   }

   aResult.mTargetPlatformPtr = mTargetEntityPtr;

   if (mFiringEntityPtr != nullptr)
   {
      mFiringEntityPtr->Update(aSimTime);
   }
   if (mTargetEntityPtr != nullptr)
   {
      mTargetEntityPtr->Update(aSimTime);
   }
   if (mWeaponEntityPtr != nullptr)
   {
      mWeaponEntityPtr->Update(aSimTime);
   }

   // Get the location of the detonation. If one wasn't provided then use the location of the weapon platform if it exists.
   aResult.mDetonateLocationWCS = mDetonateLocationWCS;
   bool inputDetonateLocValid   = true;
   if (aResult.mDetonateLocationWCS.Equals(0.0))
   {
      inputDetonateLocValid = false;
      if (mWeaponEntityPtr != nullptr)
      {
         mWeaponEntityPtr->GetLocationWCS(aResult.mDetonateLocationWCS.GetData());
      }
   }

   // If an explicit detonation location and target was specified, use the target ECS-relative location
   // of the detonation to reconstruct the target location at detonation as perceived by the sender.
   // Note that this is approximate because we are using the current local target state to derive a WCS vector.
   // (We're assume no change in target orientation).

   bool inputTargetLocValid = false;
   if (inputDetonateLocValid && (mTargetEntityPtr != nullptr))
   {
      inputTargetLocValid   = true;
      aResult.mMissDistance = mRelativeDetonationLocationECS.Magnitude();
      UtVec3d tgtToDetLocWCS;
      mTargetEntityPtr->ConvertECSVectorToWCS(tgtToDetLocWCS.GetData(), mRelativeDetonationLocationECS.GetData());
      aResult.mTargetLocationWCS = mDetonateLocationWCS - tgtToDetLocWCS;
   }

   if (aResult.mTargetPlatformPtr == nullptr)
   {
      // Target was not specified. Try to find a suitable target.
      if (mWeaponEntityPtr != nullptr)
      {
         // Find the platform that is closest to the detonation location and not on the same team as the weapon.
         aResult.mTargetPlatformPtr = FindTargetPlatform(mWeaponEntityPtr);
      }
      if ((aResult.mTargetPlatformPtr == nullptr) && (mFiringEntityPtr != nullptr))
      {
         // Find the platform that is closest to the detonation location and not on the same team as the firing platform.
         aResult.mTargetPlatformPtr = FindTargetPlatform(mFiringEntityPtr);
      }
      if (aResult.mTargetPlatformPtr == nullptr)
      {
         // Find the platform that is closest to the detonation location, regardless of team affiliation.
         aResult.mTargetPlatformPtr = FindTargetPlatform(nullptr);
      }
   }

   if (aResult.mTargetPlatformPtr != nullptr && !inputTargetLocValid)
   {
      aResult.mTargetPlatformPtr->GetLocationWCS(aResult.mTargetLocationWCS.GetData());
      if (!aResult.mDetonateLocationWCS.Equals(0.0))
      {
         aResult.mMissDistance = (aResult.mTargetLocationWCS - mDetonateLocationWCS).Magnitude();
      }
   }

   // If a weapon platform was not provided (or is already dead) then the only way to kill players is though simple detonation.
   if (mWeaponEntityPtr == nullptr)
   {
      PerformSimpleDetonation(aSimTime, aResult, nullptr);
   }
   else
   {
      // Attempt to invoke damage though the normal weapon engagement object. This will result in weapon effects being
      // applied in a consistent manner, the issuing of event output, etc.
      //
      // If the engagement object cannot be created the we simply revert to the simple detonation logic if requested.

      bool                 excludeSimpleDetonation;
      WsfStringId          weaponEffectId    = SelectWeaponEffect(mWeaponEntityPtr, excludeSimpleDetonation);
      bool                 engagementValid   = true;
      bool                 engagementCreated = false;
      WsfWeaponEngagement* engagementPtr     = WsfWeaponEngagement::Find(*mWeaponEntityPtr);
      if (engagementPtr != nullptr)
      {
         // The weapon effect in the engagement object is replaced with the selected weapon effect.
         engagementPtr->SetWeaponEffects(weaponEffectId);

         // If a target has changed then re-target to the target in the PDU.
         if ((aResult.mTargetPlatformPtr != nullptr) &&
             (aResult.mTargetPlatformPtr->GetIndex() != engagementPtr->GetTargetPlatformIndex()))
         {
            WsfTrack targetTrack;
            WsfUtil::MakeTrack(aSimTime, targetTrack, *aResult.mTargetPlatformPtr, *mWeaponEntityPtr);
            mWeaponEntityPtr->GetTrackManager().SetCurrentTarget(aSimTime, targetTrack);
         }
      }
      else
      {
         size_t targetIndex = 0;
         if (aResult.mTargetPlatformPtr != nullptr)
         {
            targetIndex = aResult.mTargetPlatformPtr->GetIndex();
         }
         engagementPtr =
            new WsfWeaponEngagement(aSimTime, &mInterfacePtr->GetSimulation(), nullptr, WsfTrackId(), targetIndex, weaponEffectId);
         engagementPtr->SetIsExternallyControlled(true);
         engagementPtr->SetFiringPlatform(mFiringEntityPtr);
         WsfWeaponEngagement::Assign(*mWeaponEntityPtr, engagementPtr);
         engagementValid   = engagementPtr->Initialize(aSimTime, mWeaponEntityPtr);
         engagementCreated = true;
      }

      if (engagementValid)
      {
         // If provided, set the reported locations of the weapon and target.
         if (inputDetonateLocValid)
         {
            engagementPtr->SetWeaponLocationWCS(mWeaponEntityPtr, mDetonateLocationWCS.GetData());
         }
         if (inputTargetLocValid)
         {
            engagementPtr->SetTargetLocationWCS(aResult.mTargetPlatformPtr, aResult.mTargetLocationWCS.GetData());
         }
         // Map the result code into one of our three 'impact codes'. If the impact code indicates detonation did
         // not occur then we indicate the engagement was a 'dud'. For all other cases we treat it as a potential hit.
         aResult.mGeometryResult = WsfWeaponEngagement::cTARGET_IMPACT;
         if ((mResult < cRESULT_CODE_COUNT) && (sResultCodes[mResult].mImpactCode == cNO_DETONATION))
         {
            aResult.mGeometryResult = WsfWeaponEngagement::cDUD;
         }
         engagementPtr->Terminate(aSimTime, aResult.mGeometryResult);

         // Also perform the simple detonation if allowed.
         // The above may not have killed the target when it should have.
         if (!excludeSimpleDetonation)
         {
            PerformSimpleDetonation(aSimTime, aResult, engagementPtr);
         }
      }
      else
      {
         ut::log::warning() << "WsfExtDetonation unable to initialize weapon engagement.";
         PerformSimpleDetonation(aSimTime, aResult, nullptr);
      }
      if (engagementCreated)
      {
         WsfWeaponEngagement::Delete(engagementPtr);
      }
   }
   if (DebugEnabled())
   {
      PrintDetonation(aSimTime, aResult);
   }
}

// =================================================================================================
// private
void WsfExtDetonation::PerformSimpleDetonation(double aSimTime, Result& aResult, WsfWeaponEngagement* aEngagementPtr)
{
   if (!mMilInterfacePtr->GetUseSimpleDetonations())
   {
      return;
   }

   int impactCode = cNO_DETONATION;
   if (mResult < cRESULT_CODE_COUNT)
   {
      impactCode = sResultCodes[mResult].mImpactCode;
   }

   if (impactCode == cNO_DETONATION)
   {
      return;
   }
   else if ((impactCode == cDETONATED) && (mTargetEntityPtr != nullptr))
   {
      // If a weapon detonates against an explicitly specified target without registering it as a hit,
      // then nothing is done.
      //
      // If a target has not been explicitly specified but one has been derived, then simple detonations
      // will be performed against the derived target.
      return;
   }

   if ((aResult.mTargetPlatformPtr != nullptr) &&
       (!aResult.mTargetPlatformPtr->IsDeleted()) && // Death is not already pending
       (!aResult.mTargetPlatformPtr->IsExternallyControlled()) &&
       (!aResult.mTargetPlatformPtr->IsExternallyMoved()) && // if the platform is externally moved then we do not have
                                                             // control to kill it
       (!aResult.mTargetPlatformPtr->IsExternalShadow()))
   {
      // If the target being considered is the same a the target specified in the PDU then range
      // will not considered a factor. If it does not match (i.e.: it is a derived target) then the
      // target must be within the simple kill range.

      double maximumKillRange = mMilInterfacePtr->GetSimpleKillRange();
      if (aResult.mTargetPlatformPtr == mTargetEntityPtr)
      {
         maximumKillRange = 1.0E+30;
      }

      // Check to see whether this engagement has a weapon-target pairing
      bool PK_Kill = true; // Assume P-K of 1 unless we have made a P-K entry and need to draw.
      WsfMilExtInterface::PK_Table::iterator iter =
         mMilInterfacePtr->GetPK_Table().find(std::make_pair(mWeaponTypeId, aResult.mTargetPlatformPtr->GetTypeId()));
      if (iter != mMilInterfacePtr->GetPK_Table().end())
      {
         double probKill     = iter->second;
         double probKillDraw = GetSimulation()->GetRandom().Uniform<double>();
         PK_Kill             = (probKillDraw <= probKill);

         auto out = ut::log::info() << "P-K draw of Weapon Type against Target.";
         out.AddNote() << "Weapon Type: " << mWeaponTypeId;
         out.AddNote() << "Target: " << aResult.mTargetPlatformPtr->GetName();
         out.AddNote() << "Pk: " << probKill;
         out.AddNote() << "Draw: " << probKillDraw;
         out.AddNote() << "Result: " << (PK_Kill ? "Killed" : "Not Killed");
      }

      if (aResult.mMissDistance <= maximumKillRange)
      {
         aResult.mGeometryResult = WsfWeaponEngagement::cTARGET_IMPACT;
         if (PK_Kill)
         {
            if (aEngagementPtr != nullptr)
            {
               WsfObserver::WeaponHit(GetSimulation())(aSimTime, aEngagementPtr, aResult.mTargetPlatformPtr);
               WsfObserver::WeaponTerminated(GetSimulation())(aSimTime, aEngagementPtr);
            }
            aResult.mTargetPlatformPtr->SetDamageFactor(1.0);
            if (!aResult.mTargetPlatformPtr->IsIndestructible())
            {
               WsfMilInterface::Find(GetSimulation())->KillPlatform(aSimTime, aResult.mTargetPlatformPtr);
            }
         }
         else if (aEngagementPtr != nullptr)
         {
            WsfObserver::WeaponMissed(GetSimulation())(aSimTime, aEngagementPtr, aResult.mTargetPlatformPtr);
         }
      }
      else
      {
         aResult.mGeometryResult = WsfWeaponEngagement::cFAR_AWAY_IN_AIR;
         if (aEngagementPtr != nullptr)
         {
            WsfObserver::WeaponMissed(GetSimulation())(aSimTime, aEngagementPtr, aResult.mTargetPlatformPtr);
         }
      }
   }
}

// =================================================================================================
//! Select the weapon effect to be used for the current engagement.
// private
WsfStringId WsfExtDetonation::SelectWeaponEffect(WsfPlatform* aWeaponPlatformPtr, bool& aExcludeSimpleDetonation)
{
   // Create the weapon effects object to be used for simple detonations
   // or for the case when the weapon does not have an associated effect.

   WsfStringId weaponEffectsType = WsfWeaponEffects::GetWeaponEffectsType(*aWeaponPlatformPtr);
   if (!weaponEffectsType)
   {
      // No weapon effect provided. Use the default weapon effect.
      weaponEffectsType = GetDefaultWeaponEffectName();
   }

   // If simple detonations are allowed, determine if the weapon platform or the weapon effect is
   // excluded from simple detonation processing. If simple detonations are not
   aExcludeSimpleDetonation = true;
   if (mMilInterfacePtr->GetUseSimpleDetonations())
   {
      bool isExcluded = false;

      // Determine if the weapon_effect is excluded from simple detonations
      if (mMilInterfacePtr->GetExcludedDetonationWeaponEffects().find(weaponEffectsType) !=
          mMilInterfacePtr->GetExcludedDetonationWeaponEffects().end())
      {
         isExcluded = true;
      }

      // Check if any member of type weapon platform_type hierarchy is excluded from simple detonations
      if (!isExcluded)
      {
         const WsfObject::TypeList& typeList = aWeaponPlatformPtr->GetTypeList();
         for (size_t i = 0; i < typeList.size(); ++i)
         {
            if (mMilInterfacePtr->GetExcludedDetonationPlatformTypes().find(typeList[i]) !=
                mMilInterfacePtr->GetExcludedDetonationPlatformTypes().end())
            {
               isExcluded = true;
               break;
            }
         }
      }

      // If not excluded, then use the default weapon effect and allow simple detonation to be employed.
      if (!isExcluded)
      {
         weaponEffectsType        = GetDefaultWeaponEffectName();
         aExcludeSimpleDetonation = false;
      }
   }
   return weaponEffectsType;
}

// =================================================================================================
//! Attempt to find a suitable target.
// private
WsfPlatform* WsfExtDetonation::FindTargetPlatform(WsfPlatform* aWeaponPtr)
{
   double         bestTargetPriority = -1.0E+30;
   WsfPlatform*   bestTargetPtr      = nullptr;
   WsfSimulation& sim                = *GetSimulation();
   for (unsigned int entry = 0; entry < sim.GetPlatformCount(); ++entry)
   {
      WsfPlatform* targetPtr = sim.GetPlatformEntry(entry);
      if (targetPtr == nullptr)
      {
         continue;
      }

      // Don't consider either the shooter or the weapon itself.
      if ((targetPtr == mFiringEntityPtr) || (targetPtr == mWeaponEntityPtr))
      {
         continue;
      }

      // If a weapon platform is supplied then we will only choose targets that are not on the same team.
      bool onOppositeSides = true;
      if (aWeaponPtr != nullptr)
      {
         onOppositeSides = (targetPtr->GetSideId() != aWeaponPtr->GetSideId());
      }

      if ((targetPtr != aWeaponPtr) && (!targetPtr->IsExternallyControlled()) && onOppositeSides)
      {
         UtVec3d targetLocWCS;
         targetPtr->GetLocationWCS(targetLocWCS.GetData());
         // TODO - should extrapolate target location???
         UtVec3d missOffsetVecWCS = missOffsetVecWCS = targetLocWCS - mDetonateLocationWCS;
         double  missDistance                        = missOffsetVecWCS.Magnitude();
         // The default priority is '-distance' (closer targets will have higher priority)
         double targetPriority = -missDistance;

         // If there is a 'target_priority' entry then the priority will be boosted by that amount.
         const WsfObject::TypeList& types = targetPtr->GetTypeList();
         for (size_t i = 0; i < types.size(); ++i)
         {
            const std::map<WsfStringId, double>::const_iterator tpi = mInterfacePtr->mTargetPriority.find(types[i]);
            if (tpi != mInterfacePtr->mTargetPriority.end())
            {
               targetPriority += (*tpi).second;
               break;
            }
         }

         if (targetPriority >= bestTargetPriority)
         {
            if (DebugEnabled())
            {
               auto out = ut::log::debug() << "Now selected Target.";
               out.AddNote() << "Target: " << targetPtr->GetName();
               out.AddNote() << "Distance: " << missDistance;
               out.AddNote() << "Priority: " << targetPriority;
            }
            bestTargetPtr      = targetPtr;
            bestTargetPriority = targetPriority;
         }
      }
   }
   return bestTargetPtr;
}

// =================================================================================================
// private
void WsfExtDetonation::PrintDetonation(double aSimTime, Result& aResult)
{
   auto out = ut::log::info() << "Detonation PDU received.";
   out.AddNote() << "T = " << aSimTime;
   if (mFiringEntityPtr != nullptr)
   {
      out.AddNote() << "Firing Entity: " << mFiringEntityPtr->GetName()
                    << (mFiringEntityPtr->IsExternallyControlled() ? ", External" : ", Internal");
   }
   if (mTargetEntityPtr != nullptr)
   {
      out << "Target Entity: " << mTargetEntityPtr->GetName()
          << (mTargetEntityPtr->IsExternallyControlled() ? ", External" : ", Internal");
   }
   else if (aResult.mTargetPlatformPtr != nullptr)
   {
      out.AddNote() << "Target Entity: " << aResult.mTargetPlatformPtr->GetName()
                    << (aResult.mTargetPlatformPtr->IsExternallyControlled() ? ", External" : ", Internal")
                    << " (Derived)";
   }
   if (mWeaponEntityPtr != nullptr)
   {
      out.AddNote() << "Weapon Entity: " << mWeaponEntityPtr->GetName()
                    << (mWeaponEntityPtr->IsExternallyControlled() ? ", External" : ", Internal");
   }
   if (!aResult.mIgnored)
   {
      if (aResult.mTargetPlatformPtr != nullptr)
      {
         // WsfUtils Print refactor needs to happen to make message atomic.(Issue #1991 )
         auto printLLA = [&out](const char* aLabel, const double* aDataWCS)
         {
            double lat, lon, alt;
            UtEntity::ConvertWCSToLLA(aDataWCS, lat, lon, alt);
            auto note = out.AddNote() << aLabel << ":";
            note.AddNote() << "Lat: " << lat;
            note.AddNote() << "Lon: " << lon;
            note.AddNote() << "Alt: " << alt * UtMath::cFT_PER_M << " ft";
         };

         printLLA("Detonation", aResult.mDetonateLocationWCS.GetData());
         printLLA("Target", aResult.mDetonateLocationWCS.GetData());

         out.AddNote() << "Miss Distance: " << aResult.mMissDistance << " m";
      }
      else
      {
         out.AddNote() << "Unable to derive target.";
      }
   }
   else
   {
      out.AddNote() << "Ignoring detonation.";
   }
   out.AddNote() << "Result: " << mResult << " (" << GetResultName() << ")";
   out.AddNote() << "Geometry Result: "
                 << ((aResult.mGeometryResult == WsfWeaponEngagement::cTARGET_IMPACT) ? "HIT" : "MISS");
}

//! Set the weapon platform pointer
void WsfExtDetonation::SetWeaponEntity(WsfPlatform* aPlatformPtr)
{
   mWeaponEntityPtr = aPlatformPtr;
   mWeaponTypeId    = mWeaponEntityPtr->GetTypeId();
}

WsfSimulation* WsfExtDetonation::GetSimulation() const
{
   return &mInterfacePtr->GetSimulation();
}
