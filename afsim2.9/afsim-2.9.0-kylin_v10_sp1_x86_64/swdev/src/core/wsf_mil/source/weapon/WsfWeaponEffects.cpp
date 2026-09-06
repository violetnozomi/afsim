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

#include "WsfWeaponEffects.hpp"

#include <algorithm>
#include <iostream>
#include <memory>

#include "UtDCM.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtRandom.hpp"
#include "UtScriptContext.hpp"
#include "UtScriptData.hpp"
#include "UtVec3.hpp"
#include "UtVec3dX.hpp"
#include "WsfMil.hpp"
#include "WsfPkTable.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTrack.hpp"
#include "WsfWeapon.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponObserver.hpp"
#include "WsfWeaponPlatformExtension.hpp"
#include "script/WsfScriptContext.hpp"
const char*  WsfWeaponEffects::cTYPE_KIND        = "weapon_effect";
const double WsfWeaponEffects::cUNHARMED         = 0.0;
const double WsfWeaponEffects::cMORTALLY_WOUNDED = 1.0;

// =================================================================================================
//! Return the weapon effects type for the associated platform.
// static
WsfStringId WsfWeaponEffects::GetWeaponEffectsType(WsfPlatform& aPlatform)
{
   return WsfWeaponPlatformExtension::GetWeaponEffectsType(aPlatform);
}

// =================================================================================================
WsfWeaponEffects::WsfWeaponEffects(WsfScenario& aScenario)
   : WsfObject()
   ,
   // Note: the script execution context will be the engagement object and not the effects object
   mContextPtr(new WsfScriptContext(*aScenario.GetScriptContext(), "WsfWeaponEngagement"))
   , mInvincibleIndex(0)
   , mInvincibleCats()
   , mLaunchPk(0.0)
   , mInterceptPk(0.0)
   , mPkDegrade(1.0)
   , // un-degraded
   mPkDrawn(-1.0)
   , mCEP(0.0)
   , mDamageRadius(0.0)
   , mScenarioPtr(&aScenario)
   , mEngagementPtr(nullptr)
   , mKilledPlatformIndices()
   , mTargetResult(cINVALID)
   , mIncidentalResult(cINVALID)
   , mPkTablePtr(nullptr)
   , mPkTableNameId(nullptr)
   , mOnTargetDamagedPtr(nullptr)
   , mOnTargetKilledPtr(nullptr)
{
   mFlags = 0;
   mContextPtr->DeclareVariable("double", "DAMAGE");
   mDebugEnabled            = false;
   mDefeated                = false;
   mIncidentalDamageAllowed = false;
   mUseLaunchPk             = true;
   mRemoveWeaponPlatform    = true;
   mDrawEndgamePk           = false;
}

// =================================================================================================
//! Copy Constructor for Clone().
// protected
WsfWeaponEffects::WsfWeaponEffects(const WsfWeaponEffects& aSrc)
   : WsfObject(aSrc)
   , mContextPtr(new WsfScriptContext(*(aSrc.mContextPtr)))
   , mInvincibleIndex(aSrc.mInvincibleIndex)
   , mInvincibleCats(aSrc.mInvincibleCats)
   , mLaunchPk(aSrc.mLaunchPk)
   , mInterceptPk(aSrc.mInterceptPk)
   , mPkDegrade(aSrc.mPkDegrade)
   , mPkDrawn(aSrc.mPkDrawn)
   , mCEP(aSrc.mCEP)
   , mDamageRadius(aSrc.mDamageRadius)
   , mScenarioPtr(aSrc.mScenarioPtr)
   , mEngagementPtr(nullptr)
   , mKilledPlatformIndices()
   , mTargetResult(aSrc.mTargetResult)
   , mIncidentalResult(aSrc.mIncidentalResult)
   , mPkTablePtr(aSrc.mPkTablePtr)
   , mPkTableNameId(aSrc.mPkTableNameId)
   , mOnTargetDamagedPtr(nullptr)
   , mOnTargetKilledPtr(nullptr)
{
   mContextPtr->DeclareVariable("double", "DAMAGE");
   mFlags = aSrc.mFlags;
}

// =================================================================================================
// virtual
WsfWeaponEffects::~WsfWeaponEffects()
{
   // delete mPkTablePtr;  // Do NOT do this, as it is a const reference-only pointer.
   delete mContextPtr;
}

// =================================================================================================
//! Method will initialize any data required by the object prior to first use.
//! (The type loader must call this method on each object after the data input stream is read and parsed.)
// virtual
bool WsfWeaponEffects::PreInitialize()
{
   // Override if needed, but derived should call base of same name,
   // to get this Pk table set done:
   if (!mPkTableNameId.IsNull())
   {
      mPkTablePtr = WsfMilExtension::Find(GetScenario())->GetPkTableManager().FindTableName(mPkTableNameId);
      if (mPkTablePtr == nullptr)
      {
         auto out = ut::log::info() << "Specified 'pk_table' not found.";
         out.AddNote() << "pk_table: " << mPkTableNameId;
         return false;
      }
   }

   return true;
}

// =================================================================================================
//! Initializes the effect at the time of weapon firing.  May be used to set the anticipated
//! launch Pk or CEP for the engagement, if needed.  Calls virtual CalcLaunchPk()
//!   @param aSimTime simulated time at the beginning of the engagement.
//!   @param aEngagementPtr The engagement that owns and controls this effect.
//!   @return 'true' if the weapon effect was initialized successfully.
// virtual
bool WsfWeaponEffects::Initialize(double aSimTime, const WsfWeaponEngagement* aEngagementPtr)
{
   bool ok = true;
   mKilledPlatformIndices.clear();
   mEngagementPtr = aEngagementPtr;

   if (mUseLaunchPk)
   {
      CalcLaunchPk(aSimTime);
   }

   if (mEngagementPtr != nullptr)
   {
      ok &= mContextPtr->Initialize(*GetSimulation(), const_cast<WsfWeaponEngagement*>(aEngagementPtr));
      if (ok)
      {
         mOnTargetDamagedPtr = mContextPtr->FindScript("on_target_damaged");
         mOnTargetKilledPtr  = mContextPtr->FindScript("on_target_killed");
         mDamageVarPtr       = &mContextPtr->GetContext().Var("DAMAGE");
      }
   }
   return ok;
}

// =================================================================================================
//! Method will read a data stream, and set data values required by the object
//! prior to first use.
// virtual
bool WsfWeaponEffects::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "excluded_category")
   {
      std::string categoryName;
      aInput.ReadValue(categoryName);
      mInvincibleCats.JoinCategory(categoryName);
   }
   else if (command == "incidental_damage_allowed")
   {
      bool value;
      aInput.ReadValue(value);
      mIncidentalDamageAllowed = value;
   }
   else if (command == "allow_incidental_damage") // NO_DOC | deprecated as of 2.9
   {
      mIncidentalDamageAllowed = true;
      auto out                 = ut::log::warning() << "'allow_incidental_damage' has been deprecated.";
      out.AddNote() << "Use 'incidental_damage_allowed true'.";
      out.AddNote() << "Location: " << aInput.GetLocation();
   }
   else if (command == "do_not_allow_incidental_damage") // NO_DOC | deprecated as of 2.9
   {
      mIncidentalDamageAllowed = false;
      auto out                 = ut::log::warning() << "'do_not_allow_incidental_damage' has been deprecated.";
      out.AddNote() << "Use 'incidental_damage_allowed false'.";
      out.AddNote() << "Location: " << aInput.GetLocation();
   }
   else if (command == "remove_weapon_platform")
   {
      bool value;
      aInput.ReadValue(value);
      mRemoveWeaponPlatform = value;
   }
   else if (command == "use_launch_pk")
   {
      SetUseLaunchPk();
   }
   else if (command == "use_intercept_pk")
   {
      SetUseInterceptPk();
   }
   else if (command == "use_pk_table")
   {
      std::string name;
      aInput.ReadValue(name);
      mPkTableNameId = WsfStringId(name);
   }
   else if (command == "launch_pk")
   {
      double value;
      aInput.ReadValue(value);
      aInput.ValueInClosedRange(value, 0.0, 1.0);
      SetLaunchPk(value);
      SetUseLaunchPk();
   }
   else if (command == "intercept_pk")
   {
      double value;
      aInput.ReadValue(value);
      aInput.ValueInClosedRange(value, 0.0, 1.0);
      SetInterceptPk(value);
      SetUseInterceptPk();
   }
   else if (command == "circular_error_probable") // NO_DOC | only used by WSF_TABULAR_LETHALITY which is not suitable for use
   {
      double value;
      aInput.ReadValueOfType(value, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(value, 0.0);
      SetCEP(value);
   }
   else if (command == "damage_radius") // NO DOC | only used by WSF_HEL_LETHALITY which has alternate input
   {
      aInput.ReadValueOfType(mDamageRadius, UtInput::cLENGTH);
      aInput.ValueGreater(mDamageRadius, 0.0);
   }
   else if (command == "debug")
   {
      mDebugEnabled = true;
   }
   else if (command == "on_target_damaged")
   {
      mOnTargetDamagedPtr = GetScriptContext()->CompileImplicitScript(aInput, "on_target_damaged", "void");
   }
   else if (command == "draw_endgame_pk")
   {
      mDrawEndgamePk = true;
   }
   else if (command == "on_target_killed")
   {
      mOnTargetKilledPtr = GetScriptContext()->CompileImplicitScript(aInput, "on_target_killed", "void");
   }
   else if (mContextPtr->ProcessInput(aInput))
   {
   }
   // This permits an in-line table definition.
   else if (WsfMilExtension::Find(GetScenario())->GetPkTableManager().ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = WsfObject::ProcessInput(aInput);
   }

   return myCommand;
}

// =================================================================================================
//! Method for explicit weapon effects.
//!
//! The effect is delivered by an intermediate platform (e.g., a bomb) not residing on the platform
//! that initiated the engagement. This method applies the ApplyEffectTo()lethality transfer function
//! from the weapon platform to all target platform(s) in the simulation which are within lethal range.
//!
//! Calling heirarchy as follows:
//! - ProcessEffectExplicit()
//! - if (IsVulnerable()) ->
//! - ProcessVulnerable()
//! - ApplyEffectTo()
//! - ApplyEffectIncrement()
//! - GetSimulation()->GetObserver().WeaponHit() and WsfPlatform::SetDamageFactor().
//!
//!   @param aSimTime simulated time for the weapon effect.
// virtual
void WsfWeaponEffects::ProcessEffectExplicit(double aSimTime)
{
   if (!Defeated())
   {
      // This is an extremely remote possibility, but it HAS happened where two
      // different weapons' proximity fuses detonate against each other at the
      // same time.  In this case, the second weapon to be processed will not
      // itself exist, due to being killed by the first.  So we must test for
      // SELF existence prior to damaging another platform...

      WsfPlatform* pPtr = GetEngagement()->GetWeaponPlatform();
      if ((pPtr != nullptr) && GetSimulation()->PlatformExists(pPtr->GetIndex()))
      {
         ProcessEffectCommon(aSimTime, mEngagementPtr->GetWeaponPlatform());
      }
   }
}

// =================================================================================================
//! Method for implicit effects.
//! The effect is delivered directly by the WsfWeapon that initiated the engagement (e.g., a directed energy device).
//!
//! Calling hierarchy as follows:
//! - ProcessEffectImplicit()
//! - if (IsVulnerable())
//! - ProcessVulnerable()
//! - ApplyEffectTo()
//! - ApplyEffectIncrement()
//! - GetSimulation()->GetObserver().WeaponHit() and WsfPlatform::SetDamageFactor().
//!
//!   @param aSimTime simulated time for the weapon effect.
// virtual
void WsfWeaponEffects::ProcessEffectImplicit(double aSimTime)
{
   if (!Defeated())
   {
      // For implicit weapons that destroy their target during an update,
      // this method will get called twice by WsfWeaponEngagement::Update().
      // The first call results in the target platform being added to the
      // mKilledPlatformIndices vector.  This check prevents an erroneous
      // miss being recorded as the second call is made during engagement
      // termination (part of AFSIM-206).
      if (!IsKilledPlatform(mEngagementPtr->GetTargetPlatformIndex()))
      {
         ProcessEffectCommon(aSimTime, mEngagementPtr->GetFiringPlatform());
      }
   }
}

// =================================================================================================
//! Method to complete the effect at the end of a weapon engagement.
//! The first action is to inform the WsfObservers of WsfWeaponEngagement
//! termination.  For an explicit engagement, the next action is to remove the
//! weapon platform from the simulation.  For both implicit and explicit
//! engagements, killed players are then removed.
//!    @param aSimTime Current simulated time value.
// virtual
void WsfWeaponEffects::Terminate(double aSimTime)
{
   // Inform outside observers that a weapon has terminated.
   WsfObserver::WeaponTerminated(GetSimulation())(aSimTime, mEngagementPtr);

   // To terminate an EXPLICIT engagement, remove the weapon platform.
   WsfPlatform* wpnPtr = GetEngagement()->GetWeaponPlatform();
   if ((wpnPtr != nullptr) && (!wpnPtr->IsIndestructible()) && (!wpnPtr->IsExternallyControlled()))
   {
      // Potentially relocate the weapon to the calculated detonation location.
      double wpnLocWCS[3];
      mEngagementPtr->GetWeaponLocationWCS(wpnPtr, wpnLocWCS);
      wpnPtr->SetLocationWCS(wpnLocWCS);

      // Remove the weapon from the simulation, after applying terminal damage.
      if (mRemoveWeaponPlatform)
      {
         wpnPtr->SetDamageFactor(1.0);
         WsfMilInterface::Find(*GetSimulation())->KillPlatform(aSimTime, wpnPtr);
      }
   }

   // For implicit or explicit engagements, remove the carnage...
   for (unsigned int i = 0; i != mKilledPlatformIndices.size(); ++i)
   {
      WsfPlatform* tgtPtr = GetSimulation()->GetPlatformByIndex(mKilledPlatformIndices[i]);
      if ((tgtPtr != nullptr) && (!tgtPtr->IsIndestructible()) && // should not happen
          (!tgtPtr->IsExternallyControlled()))                    // should not happen
      {
         double tgtLocWCS[3];
         mEngagementPtr->GetTargetLocationWCS(tgtPtr, tgtLocWCS);
         tgtPtr->SetLocationWCS(tgtLocWCS);
         WsfMilInterface::Find(*GetSimulation())->KillPlatform(aSimTime, tgtPtr);
      }
   }
   mKilledPlatformIndices.clear(); // Empty the vector, ready for next time...
}

std::string WsfWeaponEffects::GetResultString(WsfWeaponEffects::Result aResult)
{
   switch (aResult)
   {
   case WsfWeaponEffects::cMISSED:
      return "MISSED";
   case WsfWeaponEffects::cDAMAGED:
      return "DAMAGED";
   case WsfWeaponEffects::cKILLED:
      return "KILLED";
   default:
      return "UNKNOWN";
   }
}

// =================================================================================================
// virtual
//! This method isolates the actual application of damage to the other platform.
//! By default, the input damage is applied to the target platform, and if the
//! 'aResult' parameter is "cKILLED", the platform is marked for removal if it is local to the
//! simulation.
//! However, if "on_target_damaged" or "on_target_destroyed" scripts are defined, the user-defined
//! behavior will be executed instead of the standard behavior for that result.  This means
//! that damage will not be applied and platforms will not be removed, except through script calls.
//! @param aSimTime The current simulation time.
//! @param aOtherPlatformPtr The platform on which the damage is applied.
//! @param aDamage The damage result to be applied to the target platform.
//! @param aResult The general result (cDAMAGED or cKILLED) to be applied to the target platform.
void WsfWeaponEffects::ApplyDamageResultToOtherPlatform(double       aSimTime,
                                                        WsfPlatform* aOtherPlatformPtr,
                                                        double       aDamage,
                                                        Result       aResult)
{
   // If we have defined either of "on_target_damaged" or "on_target_destroyed", override the default behavor
   // with the custom scripted behavior.
   UtScript* scriptPtr = nullptr;
   if (aResult == WsfWeaponEffects::cDAMAGED)
   {
      scriptPtr = mOnTargetDamagedPtr;
   }
   else if (aResult == WsfWeaponEffects::cKILLED)
   {
      scriptPtr = mOnTargetKilledPtr;
   }

   if (scriptPtr != nullptr)
   {
      mDamageVarPtr->SetDouble(aDamage);
      mContextPtr->ExecuteScript(aSimTime, scriptPtr);
   }
   else
   {
      // We cannot directly damage or remove a remotely controlled player.
      // But we can inform the others (through the observer interface) that
      // we THINK we hit the external player. It will be up to the remote
      // simulation to respond accordingly.
      bool isExternal = aOtherPlatformPtr->IsExternallyControlled();
      if (!isExternal)
      {
         aOtherPlatformPtr->SetDamageFactor(aDamage);
      }

      // If the target is to be killed then mark it for deletion. It isn't deleted
      // immediately because others may need data about the target.

      if ((aResult == cKILLED) && (!isExternal))
      {
         mKilledPlatformIndices.push_back(aOtherPlatformPtr->GetIndex());
      }
   }
}

// =================================================================================================
//! Virtual method to apply an incremental amount of damage to a platform.
//! Should be called internally by ApplyEffectTo() after it has determined a
//! specified amount of damage.  Depending upon damage done to the platform,
//! may change the internal Result value to cMISSED, cDAMAGED, or cKILLED.
//! Will inform necessary WsfObservers of target hit or miss.
//!   @param aSimTime Current simulated time value.
//!   @param aOtherPlatformPtr Platform under consideration for damage.
//!   @param aDamageAmount A value from cUNHARMED to cMORTALLY_WOUNDED, indicating damage level.
// protected virtual
void WsfWeaponEffects::ApplyEffectIncrement(double aSimTime, WsfPlatform* aOtherPlatformPtr, double aDamageAmount)
{
   double initialDamage = aOtherPlatformPtr->GetDamageFactor();
   double incremDamage  = std::max(cUNHARMED, aDamageAmount);
   double finalDamage   = std::min(cMORTALLY_WOUNDED, initialDamage + incremDamage);

   Result newResult = cMISSED;
   if (incremDamage > cUNHARMED)
   {
      // Target was hit. See if we've killed it.

      // Checks for the platform being external/indestructible
      // Note that any external platform is also always marked as indestructible
      //   see WsfDisInterface::AddExternalPlatform()
      bool isExternal       = aOtherPlatformPtr->IsExternallyControlled();
      bool isIndestructible = aOtherPlatformPtr->IsIndestructible();

      newResult = cDAMAGED;
      if (finalDamage >= cMORTALLY_WOUNDED)
      {
         incremDamage = cMORTALLY_WOUNDED - initialDamage;
         finalDamage  = cMORTALLY_WOUNDED;
         newResult    = cKILLED;

         // If the target is marked 'indestructible' but not also 'external'
         // then simply mark the result as damaged. Don't set the result back
         // for an external entity because we want to be able to report the
         // result as KILLED. This is necessary to get the correct data in a
         // detonation PDU.
         if (isIndestructible && !isExternal)
         {
            newResult   = cDAMAGED;
            finalDamage = 0.9999999 * cMORTALLY_WOUNDED;
         }
      }

      ApplyDamageResultToOtherPlatform(aSimTime, aOtherPlatformPtr, finalDamage, newResult);
   }

   // Update the result of the engagement.
   bool isIntendedTgt = aOtherPlatformPtr->GetIndex() == mEngagementPtr->GetTargetPlatformIndex();
   SetResult(newResult, isIntendedTgt);

   // Inform the observers about the weapon hit or miss.
   // This must be done after SetResult() so the observers have the result.
   if (newResult == cMISSED)
   {
      WsfObserver::WeaponMissed(GetSimulation())(aSimTime, mEngagementPtr, aOtherPlatformPtr);
   }
   else
   {
      WsfObserver::WeaponHit(GetSimulation())(aSimTime, mEngagementPtr, aOtherPlatformPtr);
   }
}

// =================================================================================================
//! Virtual method to apply an amount of damage to an affected target platform.
//! The platform should have already passed a IsVulnerable() check prior to this
//! call.  This base implementation considers whether the targeted WsfPlatform
//! IsIndestructible() or not.  If so, no Pk is drawn, but an ever decreasing
//! amount of damage is applied to the platform, based upon the set Pk constraint.
//! If the platform is destructible, a draw is taken against either the Launch or
//! Intercept Pk value and any applied Pk degradation, and the damage result is
//! boolean (zero damage, or death).
//!   @param aSimTime Current simulated time.
//!   @param aOtherPlatformPtr Platform to (potentially) apply damage to.
// protected virtual
void WsfWeaponEffects::ApplyEffectTo(double aSimTime, WsfPlatform* aOtherPlatformPtr)
{
   double damageIncrement = 0.0;
   if (GetPkConstraint() > 0.0) // Only process non-zero Pk as zero is always a miss
   {
      // Allow a pk draw if user wants data sent to external applications
      if ((aOtherPlatformPtr->IsIndestructible()) && (!mDrawEndgamePk))
      {
         // Continually apply a logarithmically decaying amount of damage.
         mPkDrawn     = -1.0; // NOT USED
         double oldPs = 1.0 - aOtherPlatformPtr->GetDamageFactor();
         double newPs = oldPs * (1.0 - GetPkConstraint());
         // The damage increment cannot be allowed to become zero because it will be treated as a miss.
         // This could happen either because the P(k) was 1 or because P(s) actually becomes zero after
         // a large number of hits (because of the nature of computer arithmetic).
         damageIncrement = std::max(1.0E-6, oldPs - newPs);
      }
      else
      {
         // Throw the dice, and get a number between zero and one...
         // If the dice value is less than the PK, we have a kill:
         double drawnPk  = DrawUniformPk();
         damageIncrement = (drawnPk < GetPkConstraint()) ? cMORTALLY_WOUNDED : cUNHARMED;
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "WsfWeaponEffects::ApplyEffectTo() obtained a uniform random draw and the "
                                           "target platform incurred a damage increment.";
            out.AddNote() << "Draw of: " << drawnPk;
            out.AddNote() << "Damage Increment: " << damageIncrement;
         }
      }
   }
   ApplyEffectIncrement(aSimTime, aOtherPlatformPtr, damageIncrement);
}

// =================================================================================================
void WsfWeaponEffects::AddToKilledPlatforms(unsigned int aKilledIndex)
{
   mKilledPlatformIndices.push_back(aKilledIndex);
}

// =================================================================================================
bool WsfWeaponEffects::IsKilledPlatform(size_t aPlatformIndex) const
{
   auto iter = std::find_if(mKilledPlatformIndices.begin(),
                            mKilledPlatformIndices.end(),
                            [aPlatformIndex](size_t entry) { return entry == aPlatformIndex; });
   if (iter != mKilledPlatformIndices.end())
   {
      return true;
   }
   return false;
}

// =================================================================================================
//! Method to calculate the Launch Pk, in response to engagement
//! conditions during launch.  Will be called during Initialize().
//! @param aSimTime simulated time for the weapon effect.
// protected virtual
void WsfWeaponEffects::CalcLaunchPk(double /*aSimTime*/)
{
   // Not needed at the base level.  Override if needed.
   if (mPkTablePtr != nullptr)
   {
      double       azimuth;
      double       elevation;
      double       missileSpeed;
      double       targetSpeed;
      WsfPlatform* tgtPtr     = GetEngagement()->GetTargetPlatform();
      WsfStringId  targetType = tgtPtr->GetTypeId();
      CalcPkParameters(tgtPtr, azimuth, elevation, missileSpeed, targetSpeed);
      mLaunchPk = mPkTablePtr->LookUp(targetType, azimuth, elevation, missileSpeed, targetSpeed);
   }
}

// =================================================================================================
//! Virtual method to degrade the selected Pk by some factor, in response to
//! adverse engagement conditions.  The base class implementation is null,
//! but derived implementations may choose to degrade the (launch OR intercept)
//! Pk for each individual target considered.
//!   @param aSimTime   simulated time for the weapon effect.
//!   @param aTargetPtr Target under consideration for damage.
// protected virtual
void WsfWeaponEffects::CalcPkDegradation(double aSimTime, WsfPlatform* aTargetPtr) {}

// =================================================================================================
//! Method to calculate the Intercept Pk, in response to engagement conditions at intercept.
//! Will be called at the beginning of ProcessEffectExplicit().
//!   @param aSimTime   simulated time for the weapon effect.
//!   @param aTargetPtr Target under consideration for damage.
// protected virtual
void WsfWeaponEffects::CalcInterceptPk(double aSimTime, WsfPlatform* aTargetPtr) {}

// =================================================================================================
//! This protected method assures that a specified target index does exist
// protected
bool WsfWeaponEffects::IsIntendedTargetValid()
{
   return mEngagementPtr->GetTargetPlatform() != nullptr;
}

// =================================================================================================
//! Draw a uniform value for Pk determination.
// protected
double WsfWeaponEffects::DrawUniformPk()
{
   SetPkDrawn(GetSimulation()->GetRandom().Uniform<double>());
   return mPkDrawn;
}

// =================================================================================================
//! Virtual method returning a boolean to indicate whether a target platform
//! should be subjected to this weapon effect.  The base class implementation
//! enforces the 'allow_incidental_damage' flag to be honored, as well as
//! a weapon not targeting itself, a specified "invincible" platform, or a
//! target belonging to a specified category.  Derived implementations
//! may ignore these restrictions, or impose greater ones on top of the
//! basic ones.
//!   @param aOtherPlatformPtr Target under consideration.
//!   @return A flag indicating 'prosecute' or 'do not prosecute'.
// protected virtual
bool WsfWeaponEffects::IsVulnerable(WsfPlatform* aOtherPlatformPtr)
{
   // Enforce that we cannot target ourself, a specified invincible platform
   // (i.e. the launching aircraft), someone who's already dead, or any of a
   // category of invincible platform types.

   bool isMe       = aOtherPlatformPtr == GetEngagement()->GetWeaponPlatform();
   bool isMyParent = aOtherPlatformPtr == GetEngagement()->GetFiringPlatform();

   bool isVulnerable = true;
   if ((isMe) || (isMyParent && !mParentDamageAllowed) ||
       (aOtherPlatformPtr->IsDeleted()) ||                            // Death is pending... don't kill it again...
       (aOtherPlatformPtr->GetDamageFactor() >= cMORTALLY_WOUNDED) || // ...may not be needed based on IsDeleted check
       (aOtherPlatformPtr->IsExternalShadow()) || (aOtherPlatformPtr->GetIndex() == mInvincibleIndex) ||
       (aOtherPlatformPtr->GetCategories().Intersects(mInvincibleCats)))
   {
      isVulnerable = false;
   }
   return isVulnerable;
}

// =================================================================================================
//! Common driver code for base class explicit and implicit processing.
//!
//! @param aSimTime simulated time for the weapon effect.
//! @param aRefPlatformPtr The 'reference' platform. This will be the weapon platform (missile, bomb, etc.) for an
//!                        explicit weapon and the firing platform for an implicit weapon.
// protected
void WsfWeaponEffects::ProcessEffectCommon(double aSimTime, WsfPlatform* aRefPlatformPtr)
{
   UtScript*          engScriptPtr = nullptr;
   UtScriptData       engScriptRetVal;
   UtScriptDataPacker engScriptArgs{mScenarioPtr->GetScriptTypes()};

   UtScript*          wpnScriptPtr = nullptr;
   UtScriptData       wpnScriptRetVal;
   UtScriptDataPacker wpnScriptArgs{mScenarioPtr->GetScriptTypes()};

   if (mEngagementPtr != nullptr)
   {
      UtScript* tempScriptPtr = nullptr;

      // Execute the 'on_weapon_detonation' script if it exists.
      tempScriptPtr = mContextPtr->FindScript("on_weapon_detonation");
      if ((tempScriptPtr != nullptr) && mContextPtr->ValidateScript(tempScriptPtr, "void", ""))
      {
         UtScriptData     tempScriptRetVal;
         UtScriptDataList tempScriptArgs;
         mContextPtr->ExecuteScript(aSimTime, tempScriptPtr, tempScriptRetVal, tempScriptArgs);
      }

      // Determine if there is an 'on_weapon_target_engagement' script associated with the engagement object
      // NOTE: This is different than the one that may exist on the weapon platform. This version has one arguments.

      tempScriptPtr = mContextPtr->FindScript("on_weapon_target_engagement");
      if ((tempScriptPtr != nullptr) && mContextPtr->ValidateScript(tempScriptPtr, "void", "WsfPlatform"))
      {
         engScriptPtr = tempScriptPtr;
         // Pointer to target platform is filled in later.
         engScriptArgs.Push(nullptr, "WsfPlatform");
      }

      // Determine if there is an 'on_weapon_target_engagement' script associated with the reference platform.

      if (aRefPlatformPtr != nullptr)
      {
         WsfScriptContext& context = aRefPlatformPtr->GetScriptContext();
         tempScriptPtr             = context.FindScript("on_weapon_target_engagement");
         if (tempScriptPtr != nullptr && context.ValidateScript(tempScriptPtr, "void", "WsfWeaponEngagement, WsfPlatform"))
         {
            wpnScriptPtr = tempScriptPtr;
            wpnScriptArgs << mEngagementPtr;
            // Pointer to target platform is filled in later.
            wpnScriptArgs.Push(nullptr, "WsfPlatform");
         }
      }
   }

   // Logical preconditions ensure this target is always non-null
   auto processEffectOnTarget = [&](WsfPlatform* aTargetPtr)
   {
      if (IsVulnerable(aTargetPtr))
      {
         // The intercept Pk is calculated prior to calling any script because the script may want it.
         if (UseInterceptPk())
         {
            CalcInterceptPk(aSimTime, aTargetPtr);
         }

         // TODO: In the case of weapon where the weapon_effect is called repeatedly over time, the effect would
         // accumulate. The Set{Target|Implicit}Result methods only updated the status if the new status was more
         // damaging than the old. This doesn't work for incidental damage as it now stands, however, because there
         // is only one incidental status for ALL incidental platforms. As it was, once a platform was damaged
         // incidentally, the result ALWAYS indicated damaged from that point on, even if it missed a target.
         //
         // So, for now, the incidental status must be reset for each assessment.

         mIncidentalResult = cMISSED;

         mPkDrawn   = -1.0; // Pk has not yet been drawn.
         mPkDegrade = 1.0;  // Undegraded
         CalcPkDegradation(aSimTime, aTargetPtr);
         if (engScriptPtr != nullptr)
         {
            engScriptArgs.mDataList[0].SetPointer(UtScriptRef::Ref(aTargetPtr, mScenarioPtr->GetScriptTypes()));
            mContextPtr->ExecuteScript(aSimTime, engScriptPtr, engScriptRetVal, engScriptArgs);
         }
         if (wpnScriptPtr != nullptr)
         {
            wpnScriptArgs.mDataList[1].SetPointer(UtScriptRef::Ref(aTargetPtr, mScenarioPtr->GetScriptTypes()));
            aRefPlatformPtr->GetScriptContext().ExecuteScript(aSimTime, wpnScriptPtr, wpnScriptRetVal, wpnScriptArgs);
         }
         ApplyEffectTo(aSimTime, aTargetPtr);
      }
      else if (aTargetPtr == mEngagementPtr->GetTargetPlatform())
      { // If the intended target was not vulnerable, notify observers of the weapon miss.
         WsfObserver::WeaponMissed(GetSimulation())(aSimTime, mEngagementPtr, aTargetPtr);
      }
   };

   // If incidental damage is allowed then we need process all platforms,
   // otherwise we process only the intended target (if it is still valid)
   if (!mIncidentalDamageAllowed)
   {
      if (IsIntendedTargetValid())
      {
         processEffectOnTarget(mEngagementPtr->GetTargetPlatform());
      }
   }
   else
   {
      for (size_t entryNumber = 0; entryNumber < GetSimulation()->GetPlatformCount(); ++entryNumber)
      {
         auto tgtPtr = GetSimulation()->GetPlatformEntry(entryNumber);
         processEffectOnTarget(tgtPtr);
      }
   }
}

// =================================================================================================
//! Set the enumerated end result effect upon unintended target(s).
//! One detonation may have an effect upon more than one target
//! platform.  This method assures capturing the most damaging
//! result, and will save result cKILLED, ignoring a later cDAMAGED
//! or cMISSED.
//! @param aResult internally reported effect to save as final.
// private
void WsfWeaponEffects::SetIncidentalResult(const WsfWeaponEffects::Result& aResult)
{
   if (aResult > mIncidentalResult)
   {
      mIncidentalResult = aResult;
   }
}

// =================================================================================================
//! Set the most grave enumerated end result upon the intended target.
//! One employment of a weapon may have more than one effect upon a
//! target platform, especially an implicit directed energy weapon,
//! if the beam were to impinge repeatedly on the target.  This method
//! assures capturing the most damaging result, and will save result
//! cKILLED, ignoring a later cMISSED.
//! @param aResult the reported effect to save as a final result.
// private
void WsfWeaponEffects::SetTargetResult(const WsfWeaponEffects::Result& aResult)
{
   if (aResult > mTargetResult)
   {
      mTargetResult = aResult;
   }
}

// =================================================================================================
//! Set the enumerated end result effect upon affected platform(s).
//! @param aResult internally reported effect to save as final.
//! @param aIsIntended 'true' if aResult was to the intended target.
// private
void WsfWeaponEffects::SetResult(const WsfWeaponEffects::Result& aResult, bool aIsIntended)
{
   if ((mIncidentalResult == cINVALID) && (mTargetResult == cINVALID))
   {
      // This is the first time any effect was recorded for this engagement.
      // So overwrite cINVALID with cMISSED.  The cMISSED values may be
      // overwritten later with even more severe results.
      mIncidentalResult = cMISSED;
      mTargetResult     = cMISSED;
   }

   if (mDefeated)
   {
      return;
   }

   if (aIsIntended)
   {
      SetTargetResult(aResult);
   }
   else
   {
      SetIncidentalResult(aResult);
   }
}

WsfSimulation* WsfWeaponEffects::GetSimulation() const
{
   return mEngagementPtr->GetSimulation();
}

// =================================================================================================
// virtual protected
//! Capture the geometry values needed to do a Pk table lookup.
//! @param aTargetPtr Pointer to the target to be evaluated for geometry.
//! @param aAzimuth Missile-body-relative azimuth to the target.
//! @param aElevation Missile-body-relative elevation to the target.
//! @param aMissileSpeed Missile speed.
//! @param aTargetSpeed Target speed.
void WsfWeaponEffects::CalcPkParameters(WsfPlatform* aTargetPtr,
                                        double&      aAzimuth,
                                        double&      aElevation,
                                        double&      aMissileSpeed,
                                        double&      aTargetSpeed)
{
   // In this base class, the assumption is that this is an implicit rather than
   // explicit engagement, and so the Pk determination is done at trigger pull,
   // and the geometry used is the target track, relative to the _launching_
   // platform (implementation will be overridden for explicit weapon engagements).
   UtVec3dX mslLocWCS;
   UtVec3dX tgtLocWCS;
   GetEngagement()->GetTargetPlatform()->GetLocationWCS(tgtLocWCS.GetData());
   GetEngagement()->GetFiringPlatform()->GetLocationWCS(mslLocWCS.GetData());
   UtDCM    mslWCStoECS = GetEngagement()->GetFiringPlatform()->GetDcmWCStoECS();
   UtVec3dX tgtLOS_WCS  = tgtLocWCS - mslLocWCS;
   UtVec3dX tgtLOS_ECS  = tgtLOS_WCS * mslWCStoECS;
   aAzimuth             = tgtLOS_ECS.Azimuth();
   aElevation           = tgtLOS_ECS.Elevation();
   // LAUNCHING PLATFORM VELOCITY !!!
   aMissileSpeed = GetEngagement()->GetFiringPlatform()->GetSpeed();
   aTargetSpeed  = GetEngagement()->GetTargetPlatform()->GetSpeed();
}
