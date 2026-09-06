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

#include "WsfWaypoint.hpp"

#include <iostream>

#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtLatPos.hpp"
#include "UtLonPos.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtScriptBasicTypes.hpp"
#include "UtSphericalEarth.hpp"
#include "UtStringUtil.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfObject.hpp"
#include "WsfStringId.hpp"

namespace
{

// Reads in a value from UtInput, translating "default" to mean WsfPath::cUSE_DEFAULT
double ReadValue(UtInput& aInput, UtInput::ValueType aType)
{
   double      value = WsfPath::cUSE_PREVIOUS;
   std::string command;
   aInput.ReadValue(command);
   if (command == "default")
   {
      value = WsfPath::cUSE_DEFAULT;
   }
   else
   {
      aInput.PushBack(command);
      aInput.ReadValueOfType(value, aType);
   }
   return value;
}

} // namespace

// =================================================================================================
//! Constructor that uses lat, lon, alt, and speed inputs to create a valid waypoint object.
//! @param aLat The latitude of the waypoint.
//! @param aLon The longitude of the waypoint.
//! @param aAlt The altitude of the waypoint.
//! @param aSpeed The speed associated with the waypoint.
WsfWaypoint::WsfWaypoint(double aLat, double aLon, double aAlt, double aSpeed /* = WsfPath::cUSE_PREVIOUS*/)
   : mLat(aLat)
   , mLon(aLon)
   , mAlt(aAlt)
   , mSpeed(aSpeed)
{
}

// =================================================================================================
//! Constructor that uses a geopoint (WsfGeoPoint) and speed inputs create a valid waypoint object.
//! @param aPoint The geopoint (WsfGeoPoint) object that provides the (lat, lon, alt) of the waypoint.
//! @param aSpeed The speed associated with the waypoint.
WsfWaypoint::WsfWaypoint(const WsfGeoPoint& aPoint, double aSpeed /* = WsfPath::cUSE_PREVIOUS*/)
   : mLat(aPoint.GetLat())
   , mLon(aPoint.GetLon())
   , mAlt(aPoint.GetAlt())
   , mSpeed(aSpeed)
{
}

// =================================================================================================
//! Clone constructs a new WsfWaypoint equivalent to the (*this) object.
WsfWaypoint* WsfWaypoint::Clone() const
{
   return new WsfWaypoint(*this);
}

// =================================================================================================
//! Get the latitude, longitude, and altitude of the specified waypoint.
//! @param aLat  [output] The latitude of the given waypoint.
//! @param aLon  [output] The longitude of the given waypoint.
//! @param aAlt  [output] The altitude of the given waypoint.
void WsfWaypoint::GetLocationLLA(double& aLat, double& aLon, double& aAlt) const
{
   aLat = mLat;
   aLon = mLon;
   aAlt = mAlt;
}

void WsfWaypoint::SetLocationLLA(double aLat, double aLon, double aAlt)
{
   mLat = aLat;
   mLon = aLon;
   mAlt = aAlt;
}

// =================================================================================================
//! Get the great circle heading and distance from this waypoint to the
//! specified waypoint.
//! @param aWaypoint The reference waypoint to which the distance is computed.
//! @param aHeading  [output] The heading to the given waypoint.
//! @param aDistance [output] The distance to the given waypoint.
void WsfWaypoint::GreatCircleHeadingAndDistance(const WsfWaypoint& aWaypoint, double& aHeading, double& aDistance) const
{
   UtSphericalEarth::GreatCircleHeadingAndDistance(mLat, mLon, aWaypoint.mLat, aWaypoint.mLon, aHeading, aDistance);
}

// =================================================================================================
bool WsfWaypoint::ProcessAllInput(UtInput& aInput)
{
   bool        processed = false;
   std::string command;
   aInput.ReadCommand(command);
   while (ProcessInput(aInput))
   {
      aInput.ReadCommand(command);
      processed = true;
   }
   aInput.PushBack(command);
   return processed;
}

// =================================================================================================
//! Read in the configuration information for a single waypoint.
//! For more details, see WsfRoute, and also the WSF User Guide.
//! @param aInput A generic input.
//! @note A waypoint is defined as the following:\n
//!    waypoint ::= |latitude| |longitude|\n
//!                 [altitude |length|]\n
//!                 [speed |speed|]\n
//!                 [climb_rate |speed|]\n
//!                 [linear_acceleration |acceleration|]\n
//!                 [radial_acceleration |acceleration|]\n
//!                 [pause_time |time|]\n
//!                 [node_id |unsigned int|]\n
//!   where |latitude|, |longitude|, |length|, |speed|,\n
//!   |acceleration|, and |time| are defined in UtInput.\n
//!
//!  @note The fields are read in until a field not listed is
//!   reached.
//!  @returns 'true', if any identified field is read; 'false' otherwise.
bool WsfWaypoint::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "altitude")
   {
      aInput.ReadValueOfType(mAlt, UtInput::cLENGTH);
   }
   else if (command == "depth")
   {
      // Reference for underwater waypoints must be "AGL", >= 0.
      aInput.ReadValueOfType(mAlt, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mAlt, 0.0);
      mAlt    = -mAlt;
      mAltRef = WsfPath::cALT_REF_AGL;
   }
   else if (command == "agl")
   {
      mAltRef = WsfPath::cALT_REF_AGL;
   }
   else if (command == "msl")
   {
      mAltRef = WsfPath::cALT_REF_MSL;
   }
   else if (command == "speed")
   {
      aInput.ReadValueOfType(mSpeed, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mSpeed, 0.0);
   }
   else if ((command == "climb_rate") || (command == "dive_rate"))
   {
      mClimbRate = ReadValue(aInput, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mClimbRate, WsfPath::cDOUBLE_NOT_SET);
   }
   else if (command == "maximum_flight_path_angle")
   {
      mMaximumFlightPathAngle = ReadValue(aInput, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(mMaximumFlightPathAngle, WsfPath::cDOUBLE_NOT_SET);
   }
   else if (command == "linear_acceleration")
   {
      mLinearAccel = ReadValue(aInput, UtInput::cACCELERATION);
      aInput.ValueGreaterOrEqual(mLinearAccel, WsfPath::cDOUBLE_NOT_SET);
   }
   else if (command == "radial_acceleration")
   {
      mRadialAccel = ReadValue(aInput, UtInput::cACCELERATION);
      aInput.ValueGreaterOrEqual(mRadialAccel, WsfPath::cDOUBLE_NOT_SET);
   }
   else if (command == "bank_angle_limit")
   {
      double bankLimit;
      aInput.ReadValueOfType(bankLimit, UtInput::cANGLE);
      aInput.ValueInClosedRange(bankLimit, 0.0, 89.0 * UtMath::cRAD_PER_DEG);
      mRadialAccel = tan(bankLimit) * UtEarth::cACCEL_OF_GRAVITY;
   }
   else if (command == "turn_g_limit")
   {
      double gLimit;
      aInput.ReadValueOfType(gLimit, UtInput::cACCELERATION);
      aInput.ValueGreater(gLimit, UtEarth::cACCEL_OF_GRAVITY);
      mRadialAccel = sqrt(gLimit * gLimit - UtEarth::cACCEL_OF_GRAVITY * UtEarth::cACCEL_OF_GRAVITY);
   }
   else if (command == "pause_time")
   {
      aInput.ReadValueOfType(mPauseTime, UtInput::cTIME);
      aInput.ValueGreater(mPauseTime, 0.0);
   }
   else if (command == "extrapolate")
   {
      mEndOfPathOption = WsfPath::cEP_EXTRAPOLATE;
   }
   else if (command == "stop")
   {
      mEndOfPathOption = WsfPath::cEP_STOP;
   }
   else if (command == "remove")
   {
      mEndOfPathOption = WsfPath::cEP_REMOVE;
   }
   else if (command == "distance")
   {
      if (0 == (mPointType & cLOCATION_MASK))
      {
         aInput.ReadValueOfType(mDistanceOrTime, UtInput::cLENGTH);
         aInput.ValueGreater(mDistanceOrTime, 0.0);
         mPointType &= ~cDURATION_MASK;
         mPointType |= cDISTANCE;
         mSwitch = WsfPath::cSWITCH_ON_DISTANCE;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Cannot use 'distance' in this context");
      }
   }
   else if (command == "time")
   {
      if (0 == (mPointType & cLOCATION_MASK))
      {
         aInput.ReadValueOfType(mDistanceOrTime, UtInput::cTIME);
         aInput.ValueGreater(mDistanceOrTime, 0.0);
         mPointType &= ~cDURATION_MASK;
         mPointType |= cTIME;
         mSwitch = WsfPath::cSWITCH_ON_TIME;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Cannot use 'time' in this context");
      }
   }
   else if (command == "time_to_point")
   {
      if (mPointType & cLOCATION_MASK)
      {
         mPointType |= cTIME_TO_POINT;
         aInput.ReadValueOfType(mDistanceOrTime, UtInput::cTIME);
         aInput.ValueGreater(mDistanceOrTime, 0.0);
      }
      else
      {
         throw UtInput::BadValue(aInput, "Cannot use 'time_to_point' in this context");
      }
   }
   else if (command == "turn")
   {
      std::string turnDirection;
      aInput.ReadValue(turnDirection);
      if (turnDirection == "left")
      {
         mTurnDirection = WsfPath::cTURN_DIR_LEFT;
      }
      else if (turnDirection == "right")
      {
         mTurnDirection = WsfPath::cTURN_DIR_RIGHT;
      }
      else if (turnDirection == "shortest")
      {
         mTurnDirection = WsfPath::cTURN_DIR_SHORTEST;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Bad value for 'turn': " + turnDirection);
      }
   }
   else if (command == "switch_on_approach")
   {
      if (mPointType & cLOCATION_MASK)
      {
         mSwitch = WsfPath::cSWITCH_ON_APPROACH;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Cannot use 'switch_on_approach' in this context.");
      }
   }
   else if (command == "switch_on_passing")
   {
      if (mPointType & cLOCATION_MASK)
      {
         mSwitch = WsfPath::cSWITCH_ON_PASSING;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Cannot use 'switch_on_passing' in this context.");
      }
   }
   else if (command == "node_id")
   {
      aInput.ReadValue(mNodeId);
   }
   else if (command == "heading")
   {
      if (mPointType & cLATITUDE_AND_LONGITUDE)
      {
         aInput.ReadValueOfType(mHeading, UtInput::cANGLE);
         aInput.ValueGreaterOrEqual(mHeading, -UtMath::cTWO_PI);
         aInput.ValueLessOrEqual(mHeading, UtMath::cTWO_PI);
         UtMath::NormalizeAngle0_TwoPi(mHeading);
      }
      else
      {
         throw UtInput::BadValue(aInput, "Cannot use 'heading' in this context");
      }
   }
   /*else if (command == "relative_heading")
   {
      if (mPointType == cLATITUDE_AND_LONGITUDE)
      {
         aInput.ReadValueOfType(mHeading, UtInput::cANGLE);
         aInput.ValueGreaterOrEqual(mHeading, -UtMath::cTWO_PI);
         aInput.ValueLessOrEqual(mHeading, UtMath::cTWO_PI);
         UtMath::NormalizeAngle0_TwoPi(mHeading);
         mPointType = cRELATIVE_HEADING;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Cannot use 'relative_heading' in this context");
      }
   }*/
   else if ((command == "action") || (command == "execute"))
   {
      aInput.ReadValue(mScriptId);
   }
   else if (command == "mode")
   {
      aInput.ReadValue(mNewModeId);
   }
   else if (WsfAuxDataEnabled::ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
void WsfWaypoint::Print(ut::log::MessageStream& aStream) const
{
   if (mLabelId != 0)
   {
      aStream.AddNote() << "Label: " << mLabelId;
   }

   if (mPointType & cLATITUDE_AND_LONGITUDE)
   {
      UtLatPos lat(mLat);
      UtLonPos lon(mLon);
      aStream.AddNote() << "Lat: " << lat;
      aStream.AddNote() << "Lon: " << lon;
   }
   else if (mPointType & cHEADING_MASK)
   {
      if (mPointType & cRELATIVE_HEADING)
      {
         if (mHeading >= 0)
         {
            aStream.AddNote() << "Turn Right: " << mHeading * UtMath::cDEG_PER_RAD << " deg";
         }
         else
         {
            aStream.AddNote() << "Turn Left: " << -mHeading * UtMath::cDEG_PER_RAD << " deg";
         }
      }
      else if (mPointType & cABSOLUTE_HEADING)
      {
         aStream.AddNote() << "Turn to Heading: " << mHeading * UtMath::cDEG_PER_RAD << " deg";
      }
   }

   if (mPointType & cLOCATION_MASK)
   {
      if (mHeading != WsfPath::cDOUBLE_NOT_SET)
      {
         aStream.AddNote() << "Heading: " << mHeading * UtMath::cDEG_PER_RAD << " deg";
      }
   }

   if (mAlt != WsfPath::cUSE_PREVIOUS)
   {
      auto altNote = aStream.AddNote() << "Altitude: " << mAlt << " m";
      if (mAltRef == WsfPath::cALT_REF_AGL)
      {
         altNote << " agl";
      }
   }
   if (mSpeed != WsfPath::cUSE_PREVIOUS)
   {
      aStream.AddNote() << "Speed: " << mSpeed << " m/s";
   }

   if (mClimbRate != WsfPath::cUSE_PREVIOUS)
   {
      aStream.AddNote() << "Climb Rate: " << mClimbRate << " m/s";
   }

   if (mLinearAccel != WsfPath::cUSE_PREVIOUS)
   {
      aStream.AddNote() << "Linear Accel: " << mLinearAccel << " m/s2";
   }

   if (mRadialAccel != WsfPath::cUSE_PREVIOUS)
   {
      aStream.AddNote() << "Radial Accel: " << mRadialAccel << " m/s2";
   }
   if (mPauseTime != 0.0F)
   {
      aStream.AddNote() << "Pause Time: " << mPauseTime << " s";
   }

   switch (mEndOfPathOption)
   {
   case WsfPath::cEP_EXTRAPOLATE:
      aStream.AddNote() << "End of Path: Extrapolate";
      break;
   case WsfPath::cEP_STOP:
      aStream.AddNote() << "End of Path: Stop";
      break;
   case WsfPath::cEP_REMOVE:
      aStream.AddNote() << "End of Path: Remove";
      break;
   default:
      // mover default
      break;
   }

   if (mPointType & cDISTANCE)
   {
      aStream.AddNote() << "Distance: " << mDistanceOrTime << " m";
   }
   if (mPointType & cTIME)
   {
      aStream.AddNote() << "Time: " << mDistanceOrTime << " s";
   }

   if (mTurnDirection == WsfPath::cTURN_DIR_LEFT)
   {
      aStream.AddNote() << "Turn Direction: Left";
   }
   else if (mTurnDirection == WsfPath::cTURN_DIR_RIGHT)
   {
      aStream.AddNote() << "Turn Direction: Right";
   }

   if (mSwitch == WsfPath::cSWITCH_ON_APPROACH)
   {
      aStream.AddNote() << "Switch on Approach";
   }

   if (mNodeId != 0)
   {
      aStream.AddNote() << "Node ID: " << mNodeId;
   }

   if (mScriptId != 0)
   {
      aStream.AddNote() << "Execute Script: " << mScriptId;
   }

   if (mNewModeId != 0)
   {
      aStream.AddNote() << "Mode: " << mNewModeId;
   }

   if (mGotoId != 0)
   {
      aStream.AddNote() << "Goto: " << mGotoId;
   }
}

// =================================================================================================
//! Sets the waypoint's current callback
//! @param aCallbackPtr A pointer to the callback object.
void WsfWaypoint::SetCallback(WsfObject* aCallbackPtr)
{
   mCallbackPtr = std::unique_ptr<WsfObject>(aCallbackPtr);
}

// =================================================================================================
//! Returns a pointer to the waypoint's current callback
//! @returns a pointer to the callback object.
WsfObject* WsfWaypoint::GetCallback() const
{
   return mCallbackPtr.get();
}

// =================================================================================================
//! Set a heading for this waypoint, and extrapolate the velocity in that direction.
//! @param aHeading Compass heading in radians.
//! @note The waypoint must already have a specified latitude/longitude, altitude, and velocity.
//! @returns 'void', and will do nothing if required values are not set.
void WsfWaypoint::ExtrapolateAlongHeading(double aHeading)
{
   if (mPointType & cLATITUDE_AND_LONGITUDE)
   {
      mHeading         = aHeading;
      mEndOfPathOption = WsfPath::cEP_EXTRAPOLATE;
   }
}
