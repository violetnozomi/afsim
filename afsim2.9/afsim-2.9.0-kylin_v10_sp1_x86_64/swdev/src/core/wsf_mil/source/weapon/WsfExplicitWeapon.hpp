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

#ifndef WSFEXPLICITWEAPON_HPP
#define WSFEXPLICITWEAPON_HPP

#include "wsf_mil_export.h"

#include <list>
#include <utility>

#include "UtCallback.hpp"
class UtInput;
class WsfScriptContext;
#include "WsfStringId.hpp"
#include "WsfVariable.hpp"
#include "WsfWeapon.hpp"

//! A specialization of WsfWeapon for weapons modeled as separate platforms.

class WSF_MIL_EXPORT WsfExplicitWeapon : public WsfWeapon
{
public:
   WsfExplicitWeapon(WsfScenario& aScenario);
   ~WsfExplicitWeapon() override;

   //! @name Common Framework methods.
   //@{

   WsfWeapon* Clone() const override;
   bool       Initialize(double aSimTime) override;
   bool       Initialize2(double aSimTime) override;
   bool       ProcessInput(UtInput& aInput) override;
   void       TurnOff(double aSimTime) override;

   //@}

   FireResult Fire(double aSimTime, const FireTarget& aTarget, const FireOptions& aSettings) override;

   void AbortSalvo(double aSimTime, const WsfTrackId& aTrackId) override;

   double GetQuantityRemaining() const override;

   double GetDeferredLaunchDelay() const { return mDeferredLaunchDelay; }

   //! Should the launch platform velocity should be ignored when setting the initial weapon velocity.
   bool GetIgnoreLaunchPlatformVelocity() const { return mIgnoreLaunchPlatformVelocity; }

   //! Indicate if the launch platform velocity should be ignored when setting the initial weapon velocity.
   void SetIgnoreLaunchPlatformVelocity(bool aValue) { mIgnoreLaunchPlatformVelocity = aValue; }

   virtual void GetLaunchDeltaV(double aDV[3]) const;
   virtual void SetLaunchDeltaV(const double aLaunchDeltaV[3]);

   size_t LastIndexFired() const { return mLastIndexLaunched; }

   //! Get offset for the start of the dis entity id range for this weapon type
   int GetOffsetId() const { return mIdOffset; }

   void SetOffsetId(int aOffset) { mIdOffset = aOffset; }

   virtual int ProcessDeferredLaunchEvent(double aSimTime, WsfPlatform* aWeaponPlatformPtr, WsfPlatform* aTempPlatformPtr);

   bool IsWeaponToolsGenerationMode() const { return mWeaponToolsGenMode; }
   void SetIsWeaponToolsGenerationMode(bool aValue) { mWeaponToolsGenMode = aValue; }

   void SetPlatform(WsfPlatform* aPlatformPtr) override;

   // ITAR - This functionality was refactored from the WsfMover to meet EAR99 requirements, may need moved again
   virtual bool UseDeferredLaunching() const;

   //! A class to maintain data about deferred launches.
   //! @note: None of the pointers in this class refer to data owned by this class!
   class DeferredLaunch
   {
   public:
      DeferredLaunch(WsfPlatform* aRealPlatformPtr, WsfPlatform* aTempPlatformPtr, WsfTrack* aLaunchTrackPtr);

      //! The real weapon platform.
      //! This that the one that will appear in the simulation if the deferred launch is successful.
      WsfPlatform* mRealPlatformPtr;

      //! The temporary platform used the deferred launch process.
      //! This platform is never added to the simulation.
      WsfPlatform* mTempPlatformPtr;

      //! The track supplied on the fire command.
      WsfTrack* mLaunchTrackPtr;
   };

   struct CommandChain
   {
      WsfStringId mCommandChainName;
      WsfStringId mCommanderName;
   };

protected:
   WsfExplicitWeapon(const WsfExplicitWeapon& aSrc);
   WsfExplicitWeapon& operator=(const WsfExplicitWeapon&) = delete;

   virtual bool AddWeaponToSimulation(double             aSimTime,
                                      WsfPlatform*       aWeaponPlatformPtr,
                                      const WsfTrack*    aLaunchingTrackPtr,
                                      const FireOptions& aSettings);

   void InitializeCommandChains(double aSimTime, WsfPlatform* aPlatformPtr);

   virtual void SetLaunchState(double aSimTime, WsfPlatform* aPlatformPtr, const FireOptions& aOptions);

   virtual bool WeaponPlatformUserInitialize(double aSimTime, WsfPlatform* aWeaponPlatformPtr, const WsfTrack* aTrackPtr);

   //! Called by Fire once it is determined that a weapon can be launched
   virtual WsfWeapon::FireResult
   FireP(double aSimTime, const FireTarget& aTarget, const FireOptions& aSettings, WsfPlatform* aWeaponPlatformPtr);

   WsfScriptContext* mContextPtr;

   //! The initial velocity vector relative to the launcher.
   double mLaunchDeltaV_PCS[3];

   //! The delay time for simulated deferred launching.
   //! This is a testing tool for exercising the deferred launch capability without having to use JAAM or CSIMS.
   double mDeferredLaunchDelay;

   //! The list of deferred launches that are in progress.
   //! A deferred launch is one in which the mover of the launched weapon platform wants to decide if
   //! a platform should actually be added to the simulation. See the code for more details.
   std::vector<DeferredLaunch> mDeferredLaunches;

   //! The platform index of the last platform successfully launched.
   size_t mLastIndexLaunched;

   //! The offset value from which the platform DIS entity id value will be derived
   int mIdOffset;

   std::vector<CommandChain> mCommandChains;

   //! If true, align all initial velocity along the body orientation (ignore tip-off angle).
   bool mIgnoreLaunchPlatformVelocity;

   //! Launch computer data required
   bool mRequireInterceptPoint;
   bool mRequireLoftAngle;
   bool mWeaponToolsGenMode;
};

#endif
