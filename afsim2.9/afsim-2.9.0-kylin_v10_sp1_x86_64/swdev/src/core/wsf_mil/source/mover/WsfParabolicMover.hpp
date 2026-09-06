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

#ifndef WSFPARABOLICMOVER_HPP
#define WSFPARABOLICMOVER_HPP

#include "wsf_mil_export.h"

#include "WsfMover.hpp"

//! This class approximates a ballistic trajectory.  It may be used in three
//! different ways:  1) It can fly a simple parabolic trajectory from specified
//! initial conditions (vertical path subject only to a constant downward
//! gravitational acceleration), or 2) it may specify final conditions, and how
//! long to get there, and will fly constantly accelerated motion in order to
//! arrive at the final desired impact point, or 3) if not set in some other
//! manner, it will query its platform for the current target (if it exists),
//! and will attempt to fly there, given its initial conditions.
//!
//! NOTE:  This mover will not terminate motion on its own; terrain impact must
//! be determined separately, so the calculated final conditions (Lat, Lon, Alt)
//! are only the desired values.  The actual trajectory may terminate early, or
//! fly through the desired point and continue on.

class WSF_MIL_EXPORT WsfParabolicMover : public WsfMover
{
public:
   static WsfStringId TypeId();

   WsfParabolicMover(WsfScenario& aScenario);
   ~WsfParabolicMover() override;
   WsfParabolicMover& operator=(const WsfParabolicMover&) = delete;

   WsfMover* Clone() const override;

   bool Initialize(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;
   void Update(double aSimTime) override;

   WsfSpatialDomain GetSpatialDomain() const override { return WSF_SPATIAL_DOMAIN_AIR; }

   bool SetBallistic();
   bool SetCurrentTarget();
   bool SetImpactTOFAndLLA(double aTOF, double aLat, double aLon, double aAlt);
   bool SetImpactTOFAndLL(double aTOF, double aLat, double aLon);

protected:
   enum TrajectoryType
   {
      cUNDETERMINED,
      cSIMPLE_GRAVITY,
      cLLA_IMPACT,
      cCURRENT_TARGET
   };

   WsfParabolicMover(const WsfParabolicMover& aSrc);

   void InitShot(double aSimTime);
   void ToWCSLocation(double aFinalWCSLocation[]);
   void SetAccelerations();

private:
   double         mLastUpdate;
   double         mPlannedFlightTime;
   double         mPlannedFinalLatDeg;
   double         mPlannedFinalLonDeg;
   double         mPlannedFinalAlt;
   double         mGreatCircleDist;
   double         mStraightLineDist;
   double         mGroundSpeed; // Constant ONLY in the case of a cSIMPLE_GRAVITY trajectory
   double         mHeadingDeg;  // Constant ONLY in the case of a cSIMPLE_GRAVITY trajectory
   double         mStartFlightTime;
   double         mAccelNED[3];      // Always assumed constant, for all trajectory types (simplification)
   double         mCurVelNED[3];     // Initialized at launch, and varies during trajectory.
   double         mTotalDeltaNED[3]; // NED vector from launch point to desired impact point
   bool           mInitialized;
   bool           mInFlight;
   TrajectoryType mTrajType;
};

#endif
