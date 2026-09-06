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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFPATHGUIDANCE_HPP
#define WSFPATHGUIDANCE_HPP

#include "wsf_export.h"

#include "UtInput.hpp"
class WsfMover;
#include "WsfPath.hpp"
class WsfWaypoint;

//! A guidance base class used by WsfMovers that follow WsfRoutes.
//! This is a base class that specifies guidance values that may be used to
//! direct a WsfMover in three dimensions, including from one WsfWaypoint to
//! another.  Validity flags are kept to allow the implementation to know
//! what guidance is permissible or recommended, based on the information
//! known.  A "dirty" flag is kept to indicate when any value has changed,
//! and the implementation will clear this flag after it has reacted to
//! changed guidance.  The class may be further derived to augment or fill
//! in missing data.  The mover may be constrained in its ability to simul-
//! taneously follow all the guidance given, but should comply as closely
//! as possible.  Note:  It is generally intended that target Speed, Heading,
//! and Altitude values remain persistent (stay valid), even after crossing
//! waypoints, unless overridden with new values in the WsfWaypoint.
class WSF_EXPORT WsfPathGuidance
{
public:
   //! This enum specifies integer bit-mapped "valid" flags for clients to
   //! use in deciding what guidance values can be used.
   enum
   {
      cSPEED           = 0x0001, //!< Speed value is valid
      cAXIAL_ACCEL     = 0x0002, //!< Axial Acceleration value is valid
      cCLIMB_RATE      = 0x0004, //!< Climb Rate value is valid
      cALTITUDE        = 0x0008, //!< Altitude value is valid
      cHEADING         = 0x0010, //!< Heading value is valid
      cTURN_RATE       = 0x0020, //!< Turn Rate value is valid
      cRADIAL_ACCEL    = 0x0040, //!< Radial Acceleration value is valid
      cLAT_LON         = 0x0080, //!< Latitude and Longitude values are valid
      cSEGMENT_TIME    = 0x0100, //!< Segment Time value is valid
      cSEGMENT_DIST    = 0x0200, //!< Segment Distance value is valid
      cTURN_DETERMINED = 0x8000  //!< Absolute Heading value (to an LLA point) has been calculated
   };

   WsfPathGuidance();
   virtual ~WsfPathGuidance();
   virtual WsfPathGuidance* Clone() const;

   //! Set the mover to whom guidance is being provided.
   //! @note The caller retains ownership of the mover.
   void SetMover(WsfMover* aMoverPtr) { mMoverPtr = aMoverPtr; }

   //! Get the mover to whom guidance is being provided.
   WsfMover* GetMover() const { return mMoverPtr; }

   virtual bool Initialize(double aSimTime);
   virtual bool ProcessInput(UtInput& aInput);
   virtual void Update(double aSimTime);

   void TriggerExtrapolation();

   // const (or mutable) accessors:

   //! This boolean flag indicates to the client when any guidance has been changed,
   //! otherwise, the controls are the same as previous, and need not be re-interpreted.
   bool   IsChanged() const { return mIsChanged; }
   bool   IsOnGround() const { return mIsOnGround; }
   bool   IsOnRoad() const { return mIsOnRoad; }
   bool   IsExtrapolating() const { return mSwitch == WsfPath::cSWITCH_EXTERNAL; }
   bool   IsStopping() const { return mSpeed == 0.0; }
   bool   IsInitialized() const { return mInitialized; }
   bool   IsPaused() const { return mPauseTime > 0.0; }
   double GetPauseTime() const { return mPauseTime; }

   virtual double GetAltitude() const;
   virtual double GetSpeed() const;

   double GetRawHeading() const { return mHeading; }
   double GetClimbRate() const { return mClimbRate; }
   double GetLatitude() const { return mLatitude; }
   double GetLongitude() const { return mLongitude; }
   double GetSegmentEndTime() const { return mSegEndTime; }
   double GetSegmentEndDist() const { return mSegEndDist; }
   double GetAxialAccel() const { return mAxialAccel; }
   double GetRadialAccel() const { return mRadialAccel; }
   double GetTurnRate() const { return mTurnRate; }

   WsfPath::AltRef        GetAltRef() const { return mAltRef; }
   WsfPath::HdgRef        GetHdgRef() const { return mHdgRef; }
   WsfPath::TurnDirection GetTurnDir() const { return mSetTurnDir; }
   WsfPath::Switch        GetSwitch() const { return mSwitch; }

   // non-const
   void DecrementPauseTime(double aValue) { mPauseTime -= aValue; }
   void SetPauseTime(double aValue) { mPauseTime = aValue; }
   void SetAltitude(double aAltitude, WsfPath::AltRef aAltRef);
   void SetAltitude(double aAltitude, WsfPath::AltRef aMoverDefault, WsfPath::AltRef aAltRefWP_Override);
   void SetLatLon(double aLatitude, double aLongitude);
   void SetHeading(double aHeading, WsfPath::HdgRef aHdgRef);
   void SetClimbRate(double aClimbRate);
   void SetLLA(double          aLatitude,
               double          aLongitude,
               double          aAltitude,
               WsfPath::AltRef aMoverDefault,
               WsfPath::AltRef aAltRefOverride = WsfPath::cALT_REF_MSL);
   void SetSpeed(double aSpeed);
   void SetAxialAccel(double aAxialAccel);
   void SetRadialAccel(double aRadialAccel);
   void SetTurnRate(double aTurnRate);
   void SetSegmentEndTime(double aSegmentTime);
   void SetSegmentEndDist(double aSegmentDist);
   void SetHdgRef(WsfPath::HdgRef aHdgRef);
   void SetTurnDir(WsfPath::TurnDirection aTurnDir);
   void SetSwitch(WsfPath::Switch aMoverDefault, WsfPath::Switch aWP_SwitchOverride);
   void ClearValid(int aClearMask);
   bool IsSet(int aMask);

   bool SpeedIsSet() const { return (mValidityMap & cSPEED) ? true : false; }
   bool AltIsSet() const { return (mValidityMap & cALTITUDE) ? true : false; }
   bool LatLonIsSet() const { return (mValidityMap & cLAT_LON) ? true : false; }
   bool HeadingIsSet() const { return (mValidityMap & cHEADING) ? true : false; }
   bool AxialAccelIsSet() const { return (mValidityMap & cAXIAL_ACCEL) ? true : false; }
   bool ClimbRateIsSet() const { return (mValidityMap & cCLIMB_RATE) ? true : false; }
   bool TurnRateIsSet() const { return (mValidityMap & cTURN_RATE) ? true : false; }
   bool RadialAccelIsSet() const { return (mValidityMap & cRADIAL_ACCEL) ? true : false; }
   bool SegmentTimeIsSet() const { return (mValidityMap & cSEGMENT_TIME) ? true : false; }
   bool SegmentDistIsSet() const { return (mValidityMap & cSEGMENT_DIST) ? true : false; }

   // Convenience methods...
   bool LLA_IsSet() const { return LatLonIsSet() && AltIsSet(); }
   bool HdgAndAltSet() const { return HeadingIsSet() && AltIsSet(); }
   bool HdgAltAndSpeedSet() const { return HeadingIsSet() && AltIsSet() && SpeedIsSet(); }

   // Three chosen turn outputs, first check the flag, then use the two accessors:
   bool                   TurnIsDetermined() const { return (mValidityMap & cTURN_DETERMINED) ? true : false; }
   double                 ChosenHeading() const;
   WsfPath::TurnDirection ChosenDirection() const { return mChosenDir; }

   void PrintValidityMap(double aSimTime) const;

   void GetWPTargetInfo(double  aVelNED[3],
                        double& aPlatformHeading,
                        double& aPlatformPitch,
                        double& aPlatformRoll,
                        double& aHeadingChangeMag,
                        double& aHdgChgDirection,
                        double& aGroundSpeed,
                        double& aTotalSpeed,
                        double& aGroundDistTo);

   virtual double GetRadialAccelAtPoint(const WsfWaypoint& aPoint) const;

protected:
   virtual bool BeginExtrapolation(double aSimTime);
   virtual bool SetNewGuidance(double aSimTime);
   virtual bool ReviseGuidance(double aSimTime);
   virtual bool DetermineTheTurn(double aSimTime);

   void ClearChosenDir() { mChosenDir = WsfPath::cTURN_DIR_SHORTEST; }

   void SetAltRef(WsfPath::AltRef aMoverDefault, WsfPath::AltRef aAltRefOverride = WsfPath::cALT_REF_MSL);

   bool   mDebug;
   bool   mInitialized;
   bool   mIsChanged; // flag to indicate there is new information
   bool   mIsOnGround;
   bool   mIsOnRoad;
   bool   mKeepLevel;
   double mPauseTime; // The mover is FROZEN if this value is non-zero.

   const double* TgtWCS() const { return mTgtWCS; }

   void PrivateSetTurnDir(WsfPath::TurnDirection aTurnDir);

   std::string PrintBool(int aMask) const;
   void        SetValid(int aSetMask);

private:
   void PrivateSetAltitude(double aValue);
   void PrivateSetAltRef(WsfPath::AltRef aValue);
   void PrivateSetSwitch(WsfPath::Switch aValue);

   bool mExtrapNoted;

   int mValidityMap; // summed bit mapped values

   double mSpeed;      // m/s
   double mAxialAccel; // m/s^2

   double mDistance; // Distance to extrapolate the last waypoint.
   double mTime;     // Time to extrapolate the last waypoint.

   double mClimbRate; // m/s

   double mAltitude; // MSL or AGL altitude, meters.

   double mTurnRate;    // radial turn rate (radians/sec = 1/sec)
   double mRadialAccel; // lateral turn acceleration m/s^2

   double mHeading; // Desired heading, rad (reference is implementation defined).

   double mLatitude;  // Waypoint Location Latitude
   double mLongitude; // Waypoint Location Longitude

   double mSegEndTime; // Time to extrapolate a heading and speed before switching.
   double mSegEndDist; // Distance to extrapolate a heading and speed before switching.

   WsfPath::AltRef        mAltRef;     // Reference value for specified altitude
   WsfPath::HdgRef        mHdgRef;     // Specified heading is relative (implementation defined)
   WsfPath::TurnDirection mSetTurnDir; // Specified direction to turn when changing heading
   WsfPath::Switch        mSwitch;     // Instruction on when to switch waypoints

   WsfMover*              mMoverPtr;  // The mover for whom guidance is being provided
   WsfPath::TurnDirection mChosenDir; // Selected direction to turn when changing heading
   double                 mTgtWCS[3]; // Target WCS location, which will not change between Sets().
};

#endif
