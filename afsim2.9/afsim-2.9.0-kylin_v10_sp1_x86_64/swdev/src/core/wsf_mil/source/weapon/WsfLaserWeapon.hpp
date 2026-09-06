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

#ifndef WSFLASERWEAPON_HPP
#define WSFLASERWEAPON_HPP

#include "wsf_mil_export.h"

#include <string>

#include "WsfDirectedEnergyWeapon.hpp"
#include "WsfEvent.hpp"
#include "WsfHighEnergyLaser.hpp"
#include "WsfStringId.hpp"
#include "WsfThermalSystem.hpp"
#include "WsfTrack.hpp"
class WsfWeaponEngagement;

//! This is a laser used as a weapon.  It implements a specific firing time during which the target
//! geometry is updated and the fluence is integrated.  At a user-specified update rate, the fluence
//! calculations will be updated.  If the target of the laser has a WsfIntersectProcessor on it,
//! the laser will utilize that processor to perform normal incidence and occlusion calculations
//! (this is highly recommended).  Finally, one may specify a thermal system to determine when the
//! laser can no longer fire; otherwise one may specify a number of shots and a cooldown time.
class WSF_MIL_EXPORT WsfLaserWeapon : public WsfDirectedEnergyWeapon
{
public:
   enum State
   {
      cSTATE_INVALID,
      cREADY_TO_FIRE,
      cFIRING,
      cNOT_READY_TO_FIRE, //! The weapon is out of constraints and can't fire.
      cPAUSED
   };

   WsfLaserWeapon(WsfScenario& aScenario);
   WsfLaserWeapon& operator=(const WsfLaserWeapon&) = delete;
   ~WsfLaserWeapon() override;

   WsfWeapon* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;

   bool PreInitialize(double aSimTime) override;

   bool Initialize(double aSimTime) override;

   bool Initialize2(double aSimTime) override;

   void Update(double aSimTime) override;

   void TurnOff(double aSimTime) override;

   double GetQuantityRemaining() const override;

   const char* GetScriptClassName() const override;

   //! Return the HEL object associated with energy propagation.
   WsfHighEnergyLaser* GetHEL() const { return mHEL_Ptr; }

   //! Set the HEL object associated with energy propagation.
   void SetHEL(WsfHighEnergyLaser* aHEL);

   using WsfWeapon::GetTargetIndex; // Needed to prevent SGI compiler warning

   FireResult Fire(double aSimTime, const FireTarget& aTarget, const FireOptions& aSettings) override;

   // Todo; pause/resume are not necessary; just call fire again after CeaseFire call.
   void CeaseFire(double aSimTime) override;

   //! Helper / accessor methods.
   bool IsFiring() const override;

   bool CanLase(double aSimTime);

   //! Return the current state of the laser.
   int GetState() const { return mState; }

   //! Return the peak fluence (i.e., the peak irradiance on target integrated over time).
   double GetPeakFluence() const { return mHEL_Ptr->GetPeakFluence(); }

   //! Return the edge fluence (i.e., the irradiance at the damage radius or the sigma radius,
   //! whichever is smaller.
   double GetEdgeFluence() const { return mHEL_Ptr->GetEdgeFluence(); }

   //! Return the total energy (i.e., the average irradiance on target integrated over time and area).
   double GetEnergy() const { return mHEL_Ptr->GetEnergy(); }

   //! Return the total time that the fully charged system is allowed to fire.
   //! (This value is constant.)
   double GetTotalFiringTime() const { return mTotalFiringTime; }

   //! Return the average integrated energy density for the current engagement.
   double GetEnergyDensity() const { return mHEL_Ptr->GetEnergyDensity(); }

   void   SetFiringTime(double aFiringTime);
   double GetCoolingTime() const;
   double GetCoolingTimeRemaining() const;

   double GetFiringTime() const;

   double GetTotalFiringTimeRemaining(double aSimTime) const;

   double GetFiringTimeRemaining() const;

protected:
   WsfLaserWeapon(const WsfLaserWeapon& aSrc);

   //! Shot management methods.
   virtual bool BeginShot(double aSimTime);

   virtual bool UpdateShot(double aSimTime);

   virtual bool EndShot(double aSimTime);

   //! Cooling management
   virtual void EvaluateCooling(double aSimTime);

protected:
   // The laser system's aggregated objects:
   WsfHighEnergyLaser* mHEL_Ptr;

   // The state of the laser (see states above).
   int mState;

private:
   friend class LaserEvent;

   class LaserEvent : public WsfEvent
   {
   public:
      enum EventId
      {
         cNONE,
         cDONE_COOLING,
         cUPDATE_SHOT,
         cEND_SHOT,
         cEVALUATE_COOLING // for thermal system.
      };

      LaserEvent(double aSimTime, size_t aParentPlatformIndex, WsfLaserWeapon* aWsfLaserWeaponPtr, int aShotId)
         : WsfEvent(aSimTime)
         , mWsfLaserWeaponPtr(aWsfLaserWeaponPtr)
         , mParentPlatformIndex(aParentPlatformIndex)
         , mEventId(aShotId)
         , mShotId(aWsfLaserWeaponPtr->mShotId)
         , mLastEvaluationTime(0.0)
      {
      }
      ~LaserEvent() override = default;

      EventDisposition Execute() override;

   private:
      WsfLaserWeapon* mWsfLaserWeaponPtr;
      size_t          mParentPlatformIndex;
      int             mEventId;
      int             mShotId;
      double          mLastEvaluationTime;
   };

   // The thermal management object
   //(no effect if it is not configured).
   WsfThermalSystem* mThermalSystemPtr;

   double mFiringTime;      // The time that the laser fires per shot.
   double mTotalFiringTime; // The total possible firing time of a fully-charged system
   double mLastFiringStartTime;
   double mLastFiringEndTime;
   double mFiringUpdateInterval;

   // Thermal System-Related values.
   double mCoolingUpdateInterval;
   double mPostCoolingCapacity; //(s)
   double mEfficiency;
   double mLowTempLimit;
   double mHighTempLimit;
   double mPowerDissipation;

   int         mNumberOfShots;
   int         mCurrentShotNumber;
   int         mShotId;
   double      mCoolingTime;
   double      mLastCoolingStartTime;
   double      mLastUpdateTime;
   WsfStringId mThermalSystemName;

   bool mAtThermalLimit;
   bool mCommentsEnabled;
};
#endif
