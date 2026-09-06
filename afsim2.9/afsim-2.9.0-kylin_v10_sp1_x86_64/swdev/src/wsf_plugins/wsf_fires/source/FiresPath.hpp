// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef FIRESPATH_HPP
#define FIRESPATH_HPP

#include <map>

#include "FiresTable.hpp"
#include "UtEntity.hpp"
class UtInput;
#include "WsfGeoPoint.hpp"
#include "WsfMover.hpp"
#include "WsfStringId.hpp"

namespace Fires
{
class FiresTables;

//! FiresPath implements the ballistic path used by WsfFiresMover.
//! See AFSIM technical documentation for details.
class FiresPath
{
public:
   enum PropagationType
   {
      cUNDEFINED     = 0,
      cMAX_ORD_TOF   = 1, //! Lookup Max ord and tof based on impact range
      cELEVATION_TOF = 2, //! Lookup Elevation and tof based on impact range
      cSIMPLE        = 3, //! whether to use a very simple parabolic trajectory,
                          //! computed by using a reduced value of the gravitational constant
                          //! to approximate drag
      cNUM_TYPES = 4
   };

   FiresPath(WsfSimulation& aSimulation,
             UtStringId     aType,
             WsfGeoPoint&   aInitialLocation,
             double&        aTargetRange,
             double&        aTargetBearing,
             double         aAzimuthError   = 0.0,
             double         aElevationError = 0.0);

   explicit FiresPath(WsfScenario& aScenario);
   explicit FiresPath(const FiresPath& aSrc);
   virtual ~FiresPath() = default;

   FiresPath& operator=(const FiresPath& aSrc) = delete;

   //! Set the ground impact range at which the munition will impact.
   void SetImpactRange(double aRange) { mImpactRange = aRange; }

   //! Set the initial bearing along which the trajectory will move.
   void SetInitialBearing(double aBearing) { mInitialBearing = aBearing; }

   //! Set the initial error in elevation angle (used for dispersion of fires)
   //! @note This is not the expected value of the error but the actual error.
   void SetAzimuthError(double aAzimuthError) { mBearingError = aAzimuthError; }

   //! Set the initial error in elevation angle (used for dispersion of fires)
   //! @note This is not the expected value of the error but the actual error.
   void SetElevationError(double aElevationError) { mElevationError = aElevationError; }

   virtual bool Initialize(double aSimTime);

   double GetImpactTime() const { return mImpactTime; }

   void GetState(double aSimTime, double aLocWCS[3], double aVelWCS[3]);

protected:
protected:
   void UpdateState(double aSimTime);

   bool PerformTrajectoryLookup();

   void ComputeTargetRangeBearingDeltaAlt();

   bool ComputeTrajectoryParameters();

   void ComputeTimeConstantFromMaxOrd(double aDeltaAlt);

   void ComputeTimeConstantFromElevationAngle(double aDeltaAlt);

   void ComputeInitialVelocity(double aDeltaAlt);

   PropagationType mPropagationType;
   PropagationType mSimplePropagationLookupType;
   WsfStringId     mFiresType;
   FiresTables*    mFiresTablesPtr;
   WsfSimulation*  mSimulationPtr;
   bool            mRemoveOnImpact;
   double          mImpactRange; //! This is the independent variable (input) in the table lookup

   double mMaxHeight;      //! Dependent variable of table lookup for max. ord lookups.
   double mElevationAngle; //! Dependent variable of table lookup for elevation lookups.

   double      mImpactTime; //! Dependent variable of table lookup
   WsfGeoPoint mInitialLocation;
   WsfGeoPoint mImpactLocation;
   bool        mHasImpactLocation; //! If the impact location is specified, range will be computed based on
                                   //! the initial location and impact location
   bool     mConstrainPropagation;
   double   mInitialBearing;
   double   mBearingError;   //! Azimuth error for simulating dispersion of fires
   double   mElevationError; //! Elevation error for simulating dispersion of fires
   double   mDeltaAlt;       //! Difference between initial altitude and target alt
   double   mSinB;           //! Sine of initial bearing
   double   mCosB;           //! Cosine of initial bearing
   UtEntity mGeom;

   double mLocWCS[3];
   double mVelWCS[3];
   double mVelNED[3];
   double mAccNED[3];
   double mRoll;
   double mPitch;
   double mHeading;

   double mSimpleG; //! The reduced value of the gravitational constant, used with simple propagation
   double v0x;      //! The initial velocity parallel to the ground (vector connecting the initial and impact location)
   double v0z; //! The initial velocity perpendicular to the ground (vector connecting the initial and impact location)
   double mTimeConstant; //! The approximation of the ballistic coefficient (the higher the time constant, the less drag)
   double mInitialTime;  //! The sim time the round is fired
   bool   mDebug;
};
}; // namespace Fires

#endif
