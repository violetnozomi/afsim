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

#ifndef WSFLASERDESIGNATOR_HPP
#define WSFLASERDESIGNATOR_HPP

#include "wsf_mil_export.h"

#include "UtVec3dX.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfLaserDesignations.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfTerrain.hpp"

//! A specialization of WsfSensor that implements a Laser Designator (LD).
//! An LD potentially produces a laser spot in the simulation environment.
//! The laser spot is produced and placed/updated continuously in the
//! environment at the terminus end of the emitted beam, provided that:
//! A) The LD is "On" (Emitting).
//! B) The terminus of the beam will rest on a cloud layer, if in LOS.
//! C) The terminus of the beam may not be more than "maximum_range" away
//! from the emitter (else no spot).  (Later, a propagation model may
//! be inserted for higher fidelity, but is presently a minimal approach.)
//! And then, given the fact that a Spot is currently being produced...
//! if the closest platform in proximity to the spot is closer than
//! 'epsilon' from the beam (not the spot), it is associated as being
//! the lased entity.  (This is what the sensor chances are used for in
//! this transmitter.)  Each time a spot disappears, must follow updating
//! the spot with Power set to zero to extinguish it.  The association with
//! a target platform is kept until something else closer comes along, or
//!  the association times out.

class WSF_MIL_EXPORT WsfLaserDesignator : public WsfSensor
{
public:
   WsfLaserDesignator(WsfScenario& aScenario);
   WsfLaserDesignator(const WsfLaserDesignator& aSrc);
   WsfLaserDesignator& operator=(const WsfLaserDesignator&) = delete;
   ~WsfLaserDesignator() override;

   WsfSensor* Clone() const override;
   bool       Initialize(double aSimTime) override;
   bool       ProcessInput(UtInput& aInput) override;
   void       Update(double aSimTime) override;
   void       PerformScheduledDetections(double aSimTime) override;

   void TurnOff(double aSimTime) override;
   void TurnOn(double aSimTime) override;

   void PlatformDeleted(WsfPlatform* aPlatformPtr) override;

   size_t      GetEM_XmtrCount() const override;
   WsfEM_Xmtr& GetEM_Xmtr(size_t aIndex) const override;

   int  GetLaserCode() const { return mLaserCode; }
   void SetLaserCode(int aLaserCode) { mLaserCode = aLaserCode; }

   //! A mode of the sensor.
   class LaserDesignationMode : public WsfSensorMode
   {
   public:
      LaserDesignationMode();
      LaserDesignationMode(const LaserDesignationMode& aSrc);
      LaserDesignationMode& operator=(const LaserDesignationMode&) = delete;

      WsfMode* Clone() const override;
      bool     Initialize(double aSimTime) override;
      bool     ProcessInput(UtInput& aInput) override;

      bool AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, WsfSensorResult& aResult) override;

      void Deselect(double aSimTime) override;
      void Select(double aSimTime) override;

      WsfLaserDesignator* mLD_Ptr;

      WsfEM_Antenna mAntenna;
      WsfEM_Xmtr    mXmtr;
   };

   bool   IsEmitting() const { return mIsEmitting; }
   size_t GetDesignatedEntityIndex() const;

protected:
   class Consideration
   {
   public:
      Consideration(size_t aIndex)
         : mIndex(aIndex)
         , mLastCloseTime(0.0)
         , mDist2BeamSqd(0.0)
         , mDist2SpotSqd(0.0)
      {
      }

      size_t mIndex         = 0;
      double mLastCloseTime = 0.0;
      double mDist2BeamSqd  = 0.0;
      double mDist2SpotSqd  = 0.0;
   };

   class TargetAssociation
   {
   public:
      TargetAssociation();
      ~TargetAssociation();
      void   Clear();
      size_t GetIndex() const { return mCurrentPlatIndex; }
      // Return true if an association change occurred.
      bool           Consider(double          aSimTime,
                              const UtVec3dX& aEmitterLocWCS,
                              const UtVec3dX& aLOS_UnitVectorWCS,
                              const UtVec3dX& aLaserSpotLocWCS,
                              WsfPlatform*    aTargetPtr);
      Consideration* GetPtr(size_t aIndex);
      void           Remove(size_t aIndex);
      bool           Update(double aSimTime);

      size_t                      mCurrentPlatIndex;
      double                      mTimeoutInterval;
      double                      mNearestLimitSquared;
      std::vector<Consideration*> mConsiderations;
   };

   void Extinguish(double aSimTime);

   void Lase(double aSimTime, double aFrequency, double aPower);

   WsfLaserDesignations* mLaserDesignationsPtr;

   //! The sensor-specific list of modes (not valid until Initialize is called)
   std::vector<LaserDesignationMode*> mLD_ModeList;

   int          mLaserCode;
   unsigned int mUpdateCount;

   UtVec3dX mEmitterLocWCS;
   UtVec3dX mUnitLOS_WCS;
   UtVec3dX mSpotLocationWCS;

   double mFrequency;
   double mPower;
   double mMaximumRange;
   double mTerrainFloatDistance; // shorten the beam to place above terrain.

   bool mPrivateDebug;
   bool mIsEmitting;         // This goes true first, even before placing the spot.
   bool mSpotIsPlaced;       // Spot is placed in the Environment
   bool mSpotIsExtinguished; // Power is set to zero in the Environment.

   TargetAssociation          mAssociation;
   WsfLaserDesignations::Spot mLaserSpot;
   wsf::Terrain*              mTerrain;
};

#endif
