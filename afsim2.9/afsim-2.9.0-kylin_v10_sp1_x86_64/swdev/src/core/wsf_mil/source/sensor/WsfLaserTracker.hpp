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

#ifndef WSFLASERTRACKER_HPP
#define WSFLASERTRACKER_HPP

#include "wsf_mil_export.h"

#include <vector>

#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorResult.hpp"
#include "WsfTerrain.hpp"
#include "WsfTrackId.hpp"
class WsfLaserDesignations;

//! A specialization of WsfSensor that implements a Laser Tracker (LT).
//! This LT will obey commanded cueing angles until a track is established,
//! afterwards will slave itself to null the track LOS to center of FOV,
//! ignoring cueing commands.
//!
//! A Laser Tracker produces and updates a track if and only if:
//! A) The tracker is On.
//! B) A laser spot is found that matches all below:
//!       i.)   Is a Frequency match.
//!       ii.)  Is pulsing the desired (integer) Code.
//!       iii.) Is of power greater than zero.
//!       iv.)  Is within fixed maximum detection range.
//!       v.)   Is not occulted by passing through a cloud layer.
//!       vi.)  Is within the field of view.
//!       vii.) Is not occulted by terrain line of sight.
//!
//!  There may or may not be a target platform associated with
//!  the spot designation.  If the spot is claimed (by the producer) to
//!  be associated with a particular platform in the simulation, ignore
//!  the reported WCS location, and instead track upon the ECS-relative
//!  offset of the platform (this is done to minimize the effects of
//!  DIS transport latency).

class WSF_MIL_EXPORT WsfLaserTracker : public WsfSensor
{
public:
   WsfLaserTracker(WsfScenario& aScenario);
   WsfLaserTracker(const WsfLaserTracker& aSrc);
   WsfLaserTracker& operator=(const WsfLaserTracker&) = delete;
   ~WsfLaserTracker() override;

   WsfSensor* Clone() const override;
   bool       Initialize(double aSimTime) override;
   bool       ProcessInput(UtInput& aInput) override;
   void       Update(double aSimTime) override;
   void       TurnOn(double aSimTime) override;
   void       TurnOff(double aSimTime) override;

   int  GetLaserCode() const { return mLaserCode; }
   void SetLaserCode(int aLaserCode) { mLaserCode = aLaserCode; }

   enum LossReason
   {
      cDETECTED         = 0,
      cNO_SPOTS         = 1,
      cZERO_POWER       = 2,
      cOUT_OF_RANGE     = 3,
      cCLOUD_OCCULTED   = 4,
      cOUT_OF_FOV       = 5,
      cTERRAIN_OCCULTED = 6
   };

   //! A mode of the sensor.
   class LaserTrackMode : public WsfSensorMode
   {
   public:
      LaserTrackMode();
      LaserTrackMode(const LaserTrackMode& aSrc);
      LaserTrackMode& operator=(const LaserTrackMode&) = delete;

      WsfMode* Clone() const override;
      bool     Initialize(double aSimTime) override;
      bool     ProcessInput(UtInput& aInput) override;

      bool AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, WsfSensorResult& aResult) override;

      double GetMaximumRange() const { return mMaximumRange; }

      void Deselect(double aSimTime) override;
      void Select(double aSimTime) override;

      WsfEM_Antenna    mAntenna;
      WsfEM_Rcvr       mRcvr;
      WsfLaserTracker* mLT_Ptr;
   };

   size_t      GetEM_RcvrCount() const override;
   WsfEM_Rcvr& GetEM_Rcvr(size_t aIndex) const override;

   void SetCurrentLT_Mode(LaserTrackMode* aModePtr);
   bool IsTracking() const { return mIsTracking; }
   // const WsfLaserDesignations::Spot& TrackedSpot() { return mTrackedSpot; }
   WsfSensorResult& GetLocalResult() { return mLocalResult; }

protected:
   std::string GetReason();

   WsfLaserDesignations* mLaserDesignationsPtr;
   //! The sensor-specific list of modes (not valid until Initialize is called)
   std::vector<LaserTrackMode*> mTrackModeList;
   int                          mLaserCode;
   LaserTrackMode*              mCurrentLT_ModePtr;
   unsigned int                 mTrackModeIndex;
   unsigned int                 mUpdateCount;
   size_t                       mCorrelatedToTgtIndex;
   unsigned int                 mLastSpotCount;
   double                       mWavelength;
   double                       mMaximumDetectionRangeSquared;
   bool                         mPrivateDebug;
   bool                         mIsTracking;
   WsfTrackId                   mLocalTrackId;
   WsfSensorResult              mLocalResult;
   wsf::Terrain*                mTerrain;
   LossReason                   mLastLossReason;
   LossReason                   mLossReason;
};

#endif
