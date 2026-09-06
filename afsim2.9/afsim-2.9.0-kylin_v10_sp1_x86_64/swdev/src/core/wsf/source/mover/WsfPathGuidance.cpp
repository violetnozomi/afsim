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

#include "WsfPathGuidance.hpp"

#include <limits>

#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfWaypoint.hpp"

// =================================================================
//! Construct a path guidance object.
WsfPathGuidance::WsfPathGuidance()
   : mDebug(false)
   , mInitialized(false)
   , mIsChanged(false)
   , mIsOnGround(false)
   , mIsOnRoad(false)
   , mKeepLevel(false)
   , mPauseTime(0.0)
   , mExtrapNoted(true)
   , mValidityMap(0)
   , mSpeed(0.0)
   , mAxialAccel(0.0)
   , mDistance(0.0)
   , mTime(0.0)
   , mClimbRate(0.0)
   , mAltitude(0.0)
   , mTurnRate(0.0)
   , mRadialAccel(0.0)
   , mHeading(0.0)
   , mLatitude(std::numeric_limits<double>::epsilon())
   , mLongitude(std::numeric_limits<double>::epsilon())
   , mSegEndTime(0.0)
   , mSegEndDist(0.0)
   , mAltRef(WsfPath::cALT_REF_MSL)
   , mHdgRef(WsfPath::cHDG_REF_NORTH)
   , mSetTurnDir(WsfPath::cTURN_DIR_SHORTEST)
   , mSwitch(WsfPath::cSWITCH_ON_PASSING)
   , mMoverPtr(nullptr)
   , mChosenDir(WsfPath::cTURN_DIR_SHORTEST)
{
   UtVec3d::Set(mTgtWCS, 0.0);
}

// =================================================================
// virtual
WsfPathGuidance::~WsfPathGuidance() {}

// =================================================================
//! Clone a path guidance object.
// virtual
WsfPathGuidance* WsfPathGuidance::Clone() const
{
   return new WsfPathGuidance(*this);
}

// =================================================================
// virtual
double WsfPathGuidance::GetAltitude() const
{
   return mAltitude;
}

// =================================================================
// virtual
double WsfPathGuidance::GetSpeed() const
{
   return mSpeed;
}

// =================================================================
//! Get the radial acceleration to be used for turning FROM the specified waypoint.
//! This method is typically overridden by a derived class to provide additional constraints
//! on the resultant value.
//! @return The radial acceleration (m/s^2)
// virtual
double WsfPathGuidance::GetRadialAccelAtPoint(const WsfWaypoint& aPoint) const
{
   double radialAccel = 0.0;
   if (aPoint.GetRadialAccel() != WsfPath::cDOUBLE_NOT_SET)
   {
      radialAccel = aPoint.GetRadialAccel();
   }
   return radialAccel;
}

// =================================================================
// virtual
bool WsfPathGuidance::Initialize(double /*aSimTime*/)
{
   mInitialized = (mMoverPtr != nullptr);
   if (mMoverPtr == nullptr)
   {
      ut::log::error() << "WsfPathGuidance has not been attached to a mover.";
   }
   return mInitialized;
}

// =================================================================
// virtual
bool WsfPathGuidance::ProcessInput(UtInput& aInput)
{
   bool myCommand = false;

   if (aInput.GetCommand() == "debug_guidance")
   {
      myCommand = true;
      mDebug    = true;
   }
   return myCommand;
}

// =================================================================
//! This virtual method may be overridden if needed to accomplish
//! the processing needed for a specific WsfMover.  Three distinct
//! guidance modes are intrinsic to the class, triggering a new
//! guidance type, begin extrapolating existing guidance, and
//! revising guidance already in-progress.
// virtual
void WsfPathGuidance::Update(double aSimTime)
{
   assert(mInitialized);

   if (IsPaused())
   {
      return;
   }

   bool responded = true;

   if (mIsChanged)
   {
      if (mDebug)
      {
         PrintValidityMap(aSimTime);
      }

      if (!mExtrapNoted)
      {
         mExtrapNoted = BeginExtrapolation(aSimTime) && // BeginExtrapolation() just tries to maintain current heading
                                                        // (based on current velocity)
                        DetermineTheTurn(aSimTime); // DetermineTheTurn() just chooses left/right turn OR shortest turn
         responded &= mExtrapNoted;
         assert(responded);
      }
      else
      {
         responded &= SetNewGuidance(aSimTime); // SetNewGuidance() just calls DetermineTheTurn(), see above
      }

      mIsChanged = false;
   }

   responded &= ReviseGuidance(aSimTime);

   if (!responded && mDebug)
   {
      auto out = ut::log::debug() << "WsfPathGuidance did not respond to a change in guidance.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << mMoverPtr->GetPlatform()->GetName();
   }

   // assert(TurnIsDetermined());
}

// =================================================================
// virtual
bool WsfPathGuidance::BeginExtrapolation(double aSimTime)
{
   if (LatLonIsSet())
   {
      ClearValid(cLAT_LON);

      // Set the heading to continue the current heading.

      double heading;
      double notUsed;
      GetMover()->GetPlatform()->GetOrientationNED(heading, notUsed, notUsed);

      double velNED[3];
      GetMover()->GetPlatform()->GetVelocityNED(velNED);
      velNED[2]    = 0.0; // Only consider horizontal speed
      double speed = UtVec3d::Normalize(velNED);
      if (speed > 0.0)
      {
         heading = atan2(velNED[1], velNED[0]);
      }
      SetHeading(heading, WsfPath::cHDG_REF_NORTH);
   }
   mSwitch = WsfPath::cSWITCH_EXTERNAL;
   return true;
}

// =================================================================
// virtual
bool WsfPathGuidance::SetNewGuidance(double aSimTime)
{
   bool success = true;

   // There is a certain PRIORITY to the guidance requests, and we
   // fill the higher priority ones first, ignoring the lower:
   if (!TurnIsDetermined() && HeadingIsSet())
   {
      success &= DetermineTheTurn(aSimTime);
   }

   return success;
}

// =================================================================
// virtual
bool WsfPathGuidance::ReviseGuidance(double aSimTime)
{
   return true;
}

// =================================================================
void WsfPathGuidance::ClearValid(int aMask)
{
   mValidityMap &= ~aMask;
}

// =================================================================
bool WsfPathGuidance::IsSet(int aMask)
{
   return ((mValidityMap & aMask) == aMask);
}

// =================================================================
void WsfPathGuidance::TriggerExtrapolation()
{
   mIsChanged   = true;
   mExtrapNoted = false;
}

// =================================================================
std::string WsfPathGuidance::PrintBool(int aMask) const
{
   if (mValidityMap & aMask)
   {
      return "TRUE";
   }
   else
   {
      return "FALSE";
   }
}

// =================================================================
void WsfPathGuidance::SetValid(int aSetMask)
{
   if (!(mValidityMap & aSetMask))
   {
      mValidityMap |= aSetMask;
      mIsChanged = true;
   }
   // else these bits are already set, do nothing
}

// =================================================================
void WsfPathGuidance::PrivateSetAltitude(double aValue)
{
   mIsChanged |= (mAltitude != aValue);
   mAltitude = aValue;
   SetValid(cALTITUDE);
}

// =================================================================
void WsfPathGuidance::SetAltitude(double aAltitude, WsfPath::AltRef aAltRef)
{
   PrivateSetAltitude(aAltitude);
   PrivateSetAltRef(aAltRef);
}

// =================================================================
void WsfPathGuidance::SetAltitude(double aAltitude, WsfPath::AltRef aMoverDefault, WsfPath::AltRef aAltRefWP_Override)
{
   PrivateSetAltitude(aAltitude);
   SetAltRef(aMoverDefault, aAltRefWP_Override);
}

// =================================================================
void WsfPathGuidance::SetLatLon(double aLatitude, double aLongitude)
{
   bool internalChange = false;
   internalChange |= (mLatitude != aLatitude);
   internalChange |= (mLongitude != aLongitude);
   if (internalChange || !LatLonIsSet())
   {
      mIsChanged = true;
      mLatitude  = aLatitude;
      mLongitude = aLongitude;
      ClearValid(cSEGMENT_DIST);
      ClearValid(cSEGMENT_TIME);
      SetValid(cLAT_LON);
      ClearValid(cHEADING);

      UtEntity::ConvertLLAToWCS(mLatitude, mLongitude, 0.0, mTgtWCS);

      // (JAJ) The code for setting the heading used to have a note about using it for possible
      // future extrapolation. That was an improper use because the extrapolation needs to use
      // the heading at the time the extrapolation was encountered.
      //
      // We still, however, have to set some heading so the turn to the waypoint will be determined
      // (if one is indeed required). Failure to do so will cause a assertion to fail.

      double heading = 0.0;
      if (mInitialized)
      {
         // Assume current heading if cannot be determined.
         double notUsed;
         GetMover()->GetPlatform()->GetOrientationNED(heading, notUsed, notUsed);

         double startLat, startLon;
         GetMover()->GetPlatform()->GetLocationLLA(startLat, startLon, notUsed);

         // Since the compass heading changes over a great circle route, we will set the heading
         // to what it would be when it reaches the target (and not what it is at the starting
         // point.  To do this, define the segment in the REVERSE direction, and add 180 degrees.
         double hdgDeg;
         double distance;
         UtSphericalEarth::GreatCircleHeadingAndDistance(mLatitude, mLongitude, startLat, startLon, hdgDeg, distance);
         if (distance > 1.0) // Points are not coincident
         {
            heading = hdgDeg * UtMath::cRAD_PER_DEG + UtMath::cPI;
         }
      }
      // SetHeading(heading, cHDG_REF_NORTH);   //this is not necessary for flying to a target point
   }
}

// =================================================================
//! Set the heading and basis for that heading value.
//! @param aHeading - Desired heading, in radians.
//! @param aHdgRef  - Either cHDG_REF_NORTH, or cHDG_REF_RELATIVE
void WsfPathGuidance::SetHeading(double aHeading, WsfPath::HdgRef aHdgRef)
{
   aHeading = UtMath::NormalizeAngleMinusPi_Pi(aHeading);
   mIsChanged |= (mHeading != aHeading);
   mHeading = aHeading;

   ClearValid(cSEGMENT_DIST);
   ClearValid(cSEGMENT_TIME);
   ClearValid(cLAT_LON);
   SetValid(cHEADING);

   SetHdgRef(aHdgRef);

   if (mIsChanged)
   {
      ClearValid(cTURN_DETERMINED);
   }
}

// =================================================================
//! Set the basis for a heading value.
//! @param aHdgRef  - Either cHDG_REF_NORTH, or cHDG_REF_RELATIVE
void WsfPathGuidance::SetHdgRef(WsfPath::HdgRef aHdgRef)
{
   mIsChanged |= (mHdgRef != aHdgRef);
   mHdgRef = aHdgRef;
}

// =================================================================
//! Set a desired climb rate.
//! @param aClimbRate  - Desired climb rate, m/s
void WsfPathGuidance::SetClimbRate(double aClimbRate)
{
   mIsChanged |= (mClimbRate != aClimbRate);
   mClimbRate = aClimbRate;
   SetValid(cCLIMB_RATE);
}

// =================================================================
//! Set the distance basis for a path segment.
//! @param aSegmentDist - Desired distance to travel before waypoint switch, m.
void WsfPathGuidance::SetSegmentEndDist(double aSegmentDist)
{
   mIsChanged |= (mSegEndDist != aSegmentDist);
   mSegEndDist = aSegmentDist;

   SetValid(cSEGMENT_DIST);
   ClearValid(cSEGMENT_TIME);
   ClearValid(cLAT_LON);
   ClearValid(cHEADING);

   PrivateSetSwitch(WsfPath::cSWITCH_ON_DISTANCE);
}

// =================================================================
//! Set the time basis for a path segment.
//! @param aSegmentTime - Desired time to travel before waypoint switch, sec.
void WsfPathGuidance::SetSegmentEndTime(double aSegmentTime)
{
   mIsChanged |= (mSegEndTime != aSegmentTime);
   mSegEndTime = aSegmentTime;
   ClearValid(cSEGMENT_DIST);
   SetValid(cSEGMENT_TIME);
   ClearValid(cLAT_LON);
   ClearValid(cHEADING);
   PrivateSetSwitch(WsfPath::cSWITCH_ON_TIME);
}

// =================================================================
//! Set a target destination location.
//! @param aLatitude  - Desired latitude, deg.
//! @param aLongitude - Desired longitude, deg.
//! @param aAltitude  - Desired altitude, m.
//! @param aMoverDefault - Desired altitude reference,
//!                         cALT_REF_MOVER_DEFAULT, cALT_REF_MSL, cALT_REF_AGLdeg.
//! @param aAltRefOverride - Desired altitude reference (as above).
void WsfPathGuidance::SetLLA(double          aLatitude,
                             double          aLongitude,
                             double          aAltitude,
                             WsfPath::AltRef aMoverDefault,
                             WsfPath::AltRef aAltRefOverride)
{
   SetLatLon(aLatitude, aLongitude);
   if (aAltitude != -1.0)
   {
      // (See comments for virtual WsfMover::GoToLocation() method.)
      // If altitude is specified as -1.0, do not use it...
      SetAltitude(aAltitude, aMoverDefault, aAltRefOverride);
   }
}

// =================================================================
void WsfPathGuidance::SetSpeed(double aSpeed)
{
   mIsChanged |= (mSpeed != aSpeed);
   mSpeed = aSpeed;
   SetValid(cSPEED);
}

// =================================================================
void WsfPathGuidance::SetAxialAccel(double aAxialAccel)
{
   mIsChanged |= (mAxialAccel != aAxialAccel);
   mAxialAccel = aAxialAccel;
   SetValid(cAXIAL_ACCEL);
}

// =================================================================
void WsfPathGuidance::SetRadialAccel(double aRadialAccel)
{
   mIsChanged |= (mRadialAccel != aRadialAccel);
   mRadialAccel = aRadialAccel;
   SetValid(cRADIAL_ACCEL);
}

// =================================================================
void WsfPathGuidance::PrivateSetAltRef(WsfPath::AltRef aValue)
{
   mIsChanged |= (mAltRef != aValue);
   mAltRef = aValue;
}

// =================================================================
void WsfPathGuidance::SetAltRef(WsfPath::AltRef aMoverDefault, WsfPath::AltRef aAltRefOverride)
{
   if (aAltRefOverride != WsfPath::cALT_REF_MOVER_DEFAULT)
   {
      PrivateSetAltRef(aAltRefOverride);
   }
   else
   {
      PrivateSetAltRef(aMoverDefault);
   }
}

// =================================================================
void WsfPathGuidance::PrivateSetTurnDir(WsfPath::TurnDirection aTurnDir)
{
   mSetTurnDir = aTurnDir;
}

// =================================================================
void WsfPathGuidance::SetTurnDir(WsfPath::TurnDirection aTurnDir)
{
   mIsChanged |= (mSetTurnDir != aTurnDir);
   mSetTurnDir = aTurnDir;
}

// =================================================================
void WsfPathGuidance::SetSwitch(WsfPath::Switch aMoverDefault, WsfPath::Switch aWP_SwitchOverride)
{
   if (aWP_SwitchOverride != WsfPath::cSWITCH_MOVER_DEFAULT)
   {
      PrivateSetSwitch(aWP_SwitchOverride);
   }
   else
   {
      PrivateSetSwitch(aMoverDefault);
   }
   if (mSwitch != WsfPath::cSWITCH_EXTERNAL)
   {
      mExtrapNoted = true;
   }
}

// =================================================================
void WsfPathGuidance::PrivateSetSwitch(WsfPath::Switch aValue)
{
   mIsChanged |= (mSwitch != aValue);
   mSwitch = aValue;
}

// =================================================================
void WsfPathGuidance::PrintValidityMap(double aSimTime) const
{
   auto out = ut::log::info() << "WsfPathGuidance Validity:";
   out.AddNote() << "T = " << aSimTime;
   out.AddNote() << "cSPEED        = " << PrintBool(cSPEED);
   out.AddNote() << "cAXIAL_ACCEL  = " << PrintBool(cAXIAL_ACCEL);
   out.AddNote() << "cCLIMB_RATE   = " << PrintBool(cCLIMB_RATE);
   out.AddNote() << "cALTITUDE     = " << PrintBool(cALTITUDE);
   out.AddNote() << "cHEADING      = " << PrintBool(cHEADING);
   out.AddNote() << "cTURN_RATE    = " << PrintBool(cTURN_RATE);
   out.AddNote() << "cRADIAL_ACCEL = " << PrintBool(cRADIAL_ACCEL);
   out.AddNote() << "cLAT_LON      = " << PrintBool(cLAT_LON);
   out.AddNote() << "cSEGMENT_TIME = " << PrintBool(cSEGMENT_TIME);
   out.AddNote() << "cSEGMENT_DIST = " << PrintBool(cSEGMENT_DIST);
}

// =================================================================
double WsfPathGuidance::ChosenHeading() const
{
   double chosenHeading = mHeading;
   if (mHdgRef == WsfPath::cHDG_REF_RELATIVE)
   {
      double currentHdg, pitchNotUsed, rollNotUsed;
      GetMover()->GetPlatform()->GetOrientationNED(currentHdg, pitchNotUsed, rollNotUsed);
      chosenHeading = currentHdg + mHeading;
      chosenHeading = UtMath::NormalizeAngleMinusPi_Pi(chosenHeading);
   }
   return chosenHeading;
}

// =================================================================
// virtual
bool WsfPathGuidance::DetermineTheTurn(double /*aSimTime*/)
{
   if (!TurnIsDetermined() && HeadingIsSet())
   {
      double currentHdg, pitchNotUsed, rollNotUsed;
      GetMover()->GetPlatform()->GetOrientationNED(currentHdg, pitchNotUsed, rollNotUsed);
      currentHdg         = UtMath::NormalizeAngleMinusPi_Pi(currentHdg);
      double tgtHdgNorth = ChosenHeading();
      double delta       = UtMath::NormalizeAngleMinusPi_Pi(tgtHdgNorth - currentHdg);
      mChosenDir         = mSetTurnDir;
      if (fabs(delta) < WsfPath::cHEADING_FUZZ)
      {
         mChosenDir = WsfPath::cTURN_DIR_SHORTEST;
      }
      SetValid(cTURN_DETERMINED);
   }

   // Currently, there is no failure path for this method, return success...
   return true;
}
