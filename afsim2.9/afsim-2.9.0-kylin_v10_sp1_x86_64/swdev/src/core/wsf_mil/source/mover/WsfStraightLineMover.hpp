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

#ifndef WSFSTRAIGHTLINEMOVER_HPP
#define WSFSTRAIGHTLINEMOVER_HPP

#include "wsf_mil_export.h"

#include "TblLookup.hpp"
class UtInput;
#include "WsfMover.hpp"

//! This class derived from WsfMover models a low-fidelity (approximately) straight-line trajectory from a
//! missile launch point to target intercept.  The simplistic treatment does not consider mass properties
//! or Newtonian dynamics when intercepting a target track, but the benefit is that minimal information is
//! required from the user.  Two options are available:  1) The lowest fidelity approach assumes a constant
//! speed from launch to missile termination.  2) Another possibility is to specify a linearly-interpolated
//! table for speed versus time-of-flight.  If desired, a lateral g maneuver limit constraint will be imposed.
//! The target velocity (extrapolated by the expected time-of-flight) is calculated, and the missile velocity
//! vector is re-oriented toward the calculated intercept point (subject to g-limits).  If the missile and
//! target velocities are constant, then the trajectory will be a straight line to intercept.  As the
//! velocities change, the trajectory will curve in real time to re-adjust the intercept point.

class WSF_MIL_EXPORT WsfStraightLineMover : public WsfMover
{
public:
   WsfStraightLineMover(WsfScenario& aScenario);
   ~WsfStraightLineMover() override = default;
   WsfStraightLineMover& operator=(const WsfStraightLineMover&) = delete;

   //! @name Common framework methods
   //@{
   WsfMover* Clone() const override;
   bool      Initialize(double aSimTime) override;
   bool      ProcessInput(UtInput& aInput) override;
   void      Update(double aSimTime) override;
   //@}

   WsfSpatialDomain GetSpatialDomain() const override { return WSF_SPATIAL_DOMAIN_AIR; }

   //! Returns the curvilinear distance traveled thus far during the trajectory.
   double DistanceTraveled() const { return mDistanceTrav; }

   //! Returns the simulated elapsed time since launch for this missile.
   double CurrentTimeOfFlight() const { return mLastUpdateTime - mLaunchTime; }

   //! Returns the current speed of the missile.
   double CurrentSpeed() const { return mCurrentSpeed; }

protected:
   // Declare default copy constructor (for Clone())
   WsfStraightLineMover(const WsfStraightLineMover& aSrc);

   void GetSpeed(double aSimTime);

private:
   enum GuidanceMode
   {
      cPURE_PURSUIT,
      cLEAD_PURSUIT,
      cPRONAV
   };

   GuidanceMode        mGuidanceMode;
   bool                mGuideToTruth; //!< true if should guide to target truth.
   TblIndVarU<double>  mIVs;          //!< Speed-versus-time table independent values.
   TblDepVar1<double>  mDVs;          //!< Speed-versus-time table dependent values.
   TblLookupLU<double> mLU;           //!< Speed-versus-time table interpolator values.

   double mUnitVelDirWCS[3]; //!< Unit vector denoting the current velocity in the WCS frame
   double mCurrentSpeed;     //!< Current speed
   double mDistanceTrav;     //!< Distance traveled during this trajectory
   double mLaunchTime;       //!< The saved time of launch
   double mAvgSpeed;         //!< If this value is specified, a constant-speed trajectory is flown
   double mMaxLatAccel;      //!< Maximum lateral acceleration
   double mProNavGain;       //!< If pronav guidance is specified this sets the pronav gain.
   double mMissileTimeConst; //!< If acceleration is constrained this can also be defined to represent missile response dynamics.
   bool mHOBSenabled;       //!< This can be used to quickly change the heading of the missile for high off-boresight
                            //!< engagements by commanding maximum accleration.
   double mHOBSangle;       //!< This is the off-boresight angle from missile where HOBS is active.
   double mHOBSdisableTime; //!< This allows the user to set a time after which HOBS logic will no longer be used.
};

#endif
