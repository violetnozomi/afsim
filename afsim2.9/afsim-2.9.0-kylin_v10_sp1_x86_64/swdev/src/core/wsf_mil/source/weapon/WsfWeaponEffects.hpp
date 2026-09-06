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

#ifndef WSFWEAPONEFFECTS_HPP
#define WSFWEAPONEFFECTS_HPP

#include "wsf_mil_export.h"

#include <vector>

class UtInput;
class UtScript;
#include "WsfCategoryList.hpp"
#include "WsfObject.hpp"
class WsfPlatform;
#include "script/WsfScriptContext.hpp"
class WsfWeapon;
class WsfWeaponEngagement;

namespace WsfPk
{
class Table;
}

//! A base class for all types of weapon effect determinations.  Produces some effect
//! on other platforms during a weapon engagement.  Note that there are two key methods
//! of the class:  ProcessEffectExplicit() and ProcessEffectImplicit(), for explicit and
//! implicit weapons, respectively.  The base class is abstract, specialized to use a
//! fixed Pk draw against the target, and will need overridden.  Allowance is made for
//! derived classes to (optionally) do a Launch Probability of Kill (Pk) (CalcLaunchPk())
//! estimation at the time of weapon launch, an Intercept Pk (CalcInterceptPk()) at time
//! of intercept, and to set/use a Circular Error Probable (CEP) value.  A virtual Pk
//! "degrade" option, CalcPkDegradation() is also available, for events during flyout
//! that may reduce the possibility of a successful intercept, as well as Defeat(), an
//! externally available trigger to force no terminal target effect.  The two Pk values
//! may be input constants, or dependent variables of the engagement conditions and
//! geometry.

class WSF_MIL_EXPORT WsfWeaponEffects : public WsfObject
{
public:
   static WsfStringId GetWeaponEffectsType(WsfPlatform& aPlatform);

   static const char* cTYPE_KIND;
   //! Value indicating no damage to a player, the weapon had no effect.
   static const double cUNHARMED;

   //! Value indicating total damage to a player
   //! (eventually causing him to be removed from the simulation).
   static const double cMORTALLY_WOUNDED;

   //! Resulting effect on a platform.  Note that this enumeration applies separately to both the
   //! intended target for the engagement, as well as incidentally damaged players (if permitted).
   enum Result
   {
      cINVALID, //!< The effect has not yet been exercised in the engagement (no detonation yet).
      cMISSED,  //!< Did not result in any damage to the intended target platform (a 'miss').
      cDAMAGED, //!< Did apply some incremental damage to the intended platform (a 'hit').
      cKILLED   //!< Did result in total damage (kill) to the intended platform (a 'hit').
   };

   WsfWeaponEffects(WsfScenario& aScenario);
   ~WsfWeaponEffects() override;

   //! Create a clone of the object.
   //!   @return A pointer to the new object.
   WsfWeaponEffects* Clone() const override = 0;

   virtual bool PreInitialize();
   bool         ProcessInput(UtInput& aInput) override;
   virtual bool Initialize(double aSimTime, const WsfWeaponEngagement* aEngagementPtr);

   UtScriptContext* GetScriptAccessibleContext() const override { return &mContextPtr->GetContext(); }

   //! @name The primary function of this object..
   //@{
   virtual void ProcessEffectExplicit(double aSimTime);
   virtual void ProcessEffectImplicit(double aSimTime);
   virtual void Terminate(double aSimTime);

   //! Force no terminal effect.
   //! Purpose is to be externally called when a catastrophic event during weapon flyout negates any
   //! future target effect (i.e. seeker gimbal limit, etc.).
   void Defeat() { mDefeated = true; }
   //@}

   //! Returns 'true' if the effect is allowed to extend beyond the intended target
   //! to damage other platforms.
   bool GetIncidentalDamageAllowed() const { return mIncidentalDamageAllowed; }

   void SetIncidentalDamageAllowed(bool aIncidentalDamageAllowed)
   {
      mIncidentalDamageAllowed = aIncidentalDamageAllowed;
   }

   //! Can this weapon effect do damage to the parent?
   //! Not thread safe
   //! \{
   bool GetParentDamageAllowed() const { return mParentDamageAllowed; }

   void SetParentDamageAllowed(bool aParentDamageAllowed) { mParentDamageAllowed = aParentDamageAllowed; }
   //! \}

   // Deprecated form (use GetIncidentalDamageAllowed);
   // bool AllowIncidentalDamage() const                   { return mIncidentalDamageAllowed; }

   //! Current resulting effect on the intended target.
   //! The value may change during an engagement.
   Result GetTargetResult() const { return mTargetResult; }

   //! Returns the current result of the effect on the incidental (unintended) targets.
   //! The value may change during an engagement.
   Result GetIncidentalResult() const { return mIncidentalResult; }

   //! Returns the current result of the total effect on the target environment.
   //! The value may change during an engagement.
   Result GetTotalResult() const { return (mTargetResult > mIncidentalResult) ? mTargetResult : mIncidentalResult; }

   //! Uses the result enum value to return the result as a string
   //! cINVALID case defaults to "UNKNOWN" to match event output
   static std::string GetResultString(WsfWeaponEffects::Result aResult);

   //!  Method returns a boolean to indicate that the current value for Launch Pk
   //!  (in lieu of Intercept Pk) will be the target of comparison for a uniform
   //!  draw at intercept time.
   //!  @note The particular implementation may not in fact use the Launch Pk.
   bool UseLaunchPk() const { return mUseLaunchPk; }

   //! Method returns a boolean to indicate that the current value for Intercept
   //! Pk (in lieu of Launch Pk) will be the target of comparison for a uniform
   //! draw at intercept time.
   //!   @note The particular implementation may not in fact use the Intercept Pk.
   bool UseInterceptPk() const { return !mUseLaunchPk; }

   //! Return the current Pk degrade factor.
   double GetPkDegrade() const { return mPkDegrade; }

   //! Method returns the current value of Pk to be used at intercept time to determine
   //! weapon miss or kill.  This will return either the Launch Pk or Intercept PK,
   //! depending upon the results of UseLaunchPk() or UseInterceptPk() methods.
   //!   @note The particular implementation may not in fact use ANY Pk value.
   double GetPkConstraint() const { return mPkDegrade * ((mUseLaunchPk) ? mLaunchPk : mInterceptPk); }

   //! Set the ID of a platform which shall not be damaged. (Usually the launching entity.)
   void SetInvincibleIndex(unsigned int aPlatformID) { mInvincibleIndex = aPlatformID; }

   //! Get the ID of a platform which will not be damaged.
   unsigned int GetInvincibleIndex() const { return mInvincibleIndex; }

   //! Set the maximum lethal damage radius for this effect.
   void SetDamageRadius(double aDamageRadius) { mDamageRadius = aDamageRadius; }

   //! Return the maximum lethal damage radius for this effect.
   double GetDamageRadius() const { return mDamageRadius; }

   //! Get the Launch Pk.
   double GetLaunchPk() const { return mLaunchPk; }

   //! Get the Intercept Pk.
   double GetInterceptPk() const { return mInterceptPk; }

   //! Force a pk draw for an external platform target
   bool DrawEndgamePk() { return mDrawEndgamePk; }

   //! Access the value for engagement Circular Error Probable (CEP).
   virtual double GetCEP() const { return mCEP; }

   //! Set the value for engagement Circular Error Probable (CEP).
   //! (May not be used by all WsfEffects classes.)
   void SetCEP(double aValue) { mCEP = aValue; }

   //! Set the fraction by which the un-degraded Pk value should be multiplied to get the effective Pk.
   //! @param aPkDegrade The multiplier [0..1] to be applied to the un-degraded Pk to get the effective Pk.
   void SetPkDegrade(double aPkDegrade) { mPkDegrade = aPkDegrade; }

   //! Access the Pk value drawn for the (intended) target hit assessment.
   virtual double GetPkDrawn() const { return mPkDrawn; }

   //! Returns 'true' if debugging is enabled for this instance.
   bool DebugEnabled() const { return mDebugEnabled; }

   //! Enable or disable debugging for this instance
   void SetDebugEnabled(bool aDebugEnabled) { mDebugEnabled = aDebugEnabled; }

   //! Method returns the count of platforms killed during this engagement
   unsigned int KilledPlatformCount() const { return static_cast<unsigned int>(mKilledPlatformIndices.size()); }

   //! Returns 'true' if the specified WSF platform index has been killed during this engagement
   bool IsKilledPlatform(size_t aPlatformIndex) const;

   WsfPk::Table* GetPkTable() { return mPkTablePtr; }

   WsfScenario& GetScenario() { return *mScenarioPtr; }

protected:
   WsfWeaponEffects(const WsfWeaponEffects& aSrc);
   WsfWeaponEffects& operator=(const WsfWeaponEffects& aRhs);

   virtual void ApplyEffectIncrement(double aSimTime, WsfPlatform* aOtherPlatformPtr, double aDamageAmount);
   virtual void ApplyEffectTo(double aSimTime, WsfPlatform* aOtherPlatformPtr);
   virtual void ApplyDamageResultToOtherPlatform(double aSimTime, WsfPlatform* aOtherPlatformPtr, double aDamage, Result aResult);

   virtual void CalcLaunchPk(double aSimTime);
   virtual void CalcPkDegradation(double aSimTime, WsfPlatform* aTargetPtr);
   virtual void CalcInterceptPk(double aSimTime, WsfPlatform* aTargetPtr);
   virtual void CalcPkParameters(WsfPlatform* aTargetPtr,
                                 double&      aAzimuth,
                                 double&      aElevation,
                                 double&      aMissileSpeed,
                                 double&      aTargetSpeed);

   bool IsIntendedTargetValid();
   void AddToKilledPlatforms(unsigned int aKilledIndex);

   double       DrawUniformPk();
   virtual bool IsVulnerable(WsfPlatform* aOtherPlatformPtr);
   void         ProcessEffectCommon(double aSimTime, WsfPlatform* aRefPlatformPtr);

   //! Return a flag to indicate that this effect has been annulled by outside influences.
   bool Defeated() const { return mDefeated; }

   //! Return the Engagement that this effect was instantiated to support.
   const WsfWeaponEngagement* GetEngagement() const { return mEngagementPtr; }

   //! Indicate that you wish to use the Launch Pk value (in lieu of the Intercept Pk value).
   void SetUseLaunchPk() { mUseLaunchPk = true; }

   //! Indicate that you wish to use the Intercept Pk value (in lieu of the Launch Pk value).
   void SetUseInterceptPk() { mUseLaunchPk = false; }

   //! Set the (assumedly fixed) Launch Pk value.
   //! @note Some derived-class virtual methods (if used) are free to overwrite this value.
   void SetLaunchPk(double aValue) { mLaunchPk = aValue; }

   //! Set the (assumedly fixed) Intercept Pk value.
   //! @note Some derived-class virtual methods (if used) are free to overwrite this value.
   void SetInterceptPk(double aValue) { mInterceptPk = aValue; }

   //! Set the Pk value used in the intended target hit assessment.
   void SetPkDrawn(double aValue) { mPkDrawn = aValue; }

   void SetIncidentalResult(const WsfWeaponEffects::Result& aResult);
   void SetTargetResult(const WsfWeaponEffects::Result& aResult);
   void SetResult(const WsfWeaponEffects::Result& aResult, bool aIsIntended);

   //! Method returns an index of one of the platforms killed during this engagement
   size_t KilledPlatform(size_t aIndex) { return mKilledPlatformIndices[aIndex]; }

   const WsfWeaponEngagement* GetEngagementPtr() const { return mEngagementPtr; }

   WsfSimulation* GetSimulation() const;

   WsfScriptContext* GetScriptContext() const { return mContextPtr; }

private:
   WsfScriptContext* mContextPtr;

   unsigned int    mInvincibleIndex;
   WsfCategoryList mInvincibleCats;

   union
   {
      struct
      {
         bool mDebugEnabled : 1;
         bool mDefeated : 1;
         bool mIncidentalDamageAllowed : 1;
         bool mParentDamageAllowed : 1;
         bool mUseLaunchPk : 1; // if false, use intercept Pk
         bool mRemoveWeaponPlatform : 1;
         bool mDrawEndgamePk : 1; // Force a pk draw for an external platform
      };
      unsigned short mFlags;
   };

   double                     mLaunchPk;
   double                     mInterceptPk;
   double                     mPkDegrade;
   double                     mPkDrawn;
   double                     mCEP;
   double                     mDamageRadius;
   WsfScenario*               mScenarioPtr;
   const WsfWeaponEngagement* mEngagementPtr;
   std::vector<size_t>        mKilledPlatformIndices;
   Result                     mTargetResult;
   Result                     mIncidentalResult;

   WsfPk::Table* mPkTablePtr;
   WsfStringId   mPkTableNameId;

   UtScript* mOnTargetDamagedPtr;
   UtScript* mOnTargetKilledPtr;

   UtScriptData* mDamageVarPtr;
};

#endif
