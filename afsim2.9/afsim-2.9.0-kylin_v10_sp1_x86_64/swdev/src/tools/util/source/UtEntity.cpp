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
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "UtEntity.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>

#include "UtDCM.hpp"
#include "UtEarth.hpp"
#include "UtEllipsoidalCentralBody.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtEntityPart.hpp"
#include "UtException.hpp"
#include "UtLog.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtVec3.hpp"

// =================================================================================================
UtEntity::UtEntity(const ut::CentralPoint& aCentralPoint)
   : mCentralPointPtr(ut::clone(aCentralPoint))
{
   // Set the initial state.
   mLLALocationIsValid      = true;
   mWCSLocationIsValid      = false;
   mInertialLocationIsValid = false;
   mNEDVelocityIsValid      = true;
   mWCSVelocityIsValid      = false;
   mInertialVelocityIsValid = false;

   mNEDAccelerationIsValid      = true;
   mWCSAccelerationIsValid      = false;
   mInertialAccelerationIsValid = false;
   mNEDOrientationIsValid       = true;
   mWCSOrientationIsValid       = false;
   mInertialOrientationIsValid  = false;

   mWCSToNEDTransformIsValid = false;
   mNEDToECSTransformIsValid = false;
   mWCSToECSTransformIsValid = false;
}

//! Return the central point as a central body.  If the central point is not a ut::CentralBody, an exception will be
//! thrown.
const ut::CentralBody& UtEntity::GetCentralBody() const
{
   return mCentralPointPtr->GetAsCentralBody();
}

void UtEntity::SetCentralPoint(const ut::CentralPoint& aCentralPoint)
{
   mCentralPointPtr = ut::clone(aCentralPoint);
}

// =================================================================================================
// POSITION ROUTINES
// =================================================================================================

// =================================================================================================
//! Get the WCS location (in meters)
//! @param aLocationWCS is the location in the WCS frame.
void UtEntity::GetLocationWCS(double aLocationWCS[3]) const
{
   if (!mWCSLocationIsValid)
   {
      UpdateWCSLocation();
   }
   UtVec3d::Set(aLocationWCS, mLocationWCS);
}

// =================================================================================================
//! Get the WCS location (in meters).
//! @return the location in the WCS frame.
ut::coords::WCS UtEntity::GetLocationWCS() const
{
   ut::coords::WCS loc;
   GetLocationWCS(loc.GetData());
   return loc;
}

// =================================================================================================
//! Set the WCS location (in meters).
void UtEntity::SetLocationWCS(const ut::coords::WCS& aLocationWCS)
{
   SetLocationWCS(aLocationWCS.GetData());
}

// =================================================================================================
//! Set the WCS location (in meters).
void UtEntity::SetLocationWCS(const UtVec3d& aLocationWCS)
{
   SetLocationWCS(aLocationWCS.GetData());
}

// =================================================================================================
//! Set the WCS location (in meters).
void UtEntity::SetLocationWCS(const double aLocationWCS[3])
{
   UtVec3d::Set(mLocationWCS, aLocationWCS);

   mCoordinateFrame         = cCF_WCS;
   mInertialLocationIsValid = false;
   mWCSLocationIsValid      = true;
   mLLALocationIsValid      = false;

   WasMoved();
}

// =================================================================================================
//! Get the Inertial location (in meters).
//! @param aLocationECI is the location in the ECI frame.
void UtEntity::GetLocationECI(double aLocationECI[3]) const
{
   if (!mInertialLocationIsValid)
   {
      UpdateInertialLocation();
   }
   UtVec3d::Set(aLocationECI, GetECI_Conversion().GetLocationECI().GetData());
}

// =================================================================================================
//! Get the Inertial location (in meters).
//! @return the location in the ECI frame.
ut::coords::ECI UtEntity::GetLocationECI() const
{
   UtVec3d loc;
   GetLocationECI(loc.GetData());
   return ut::coords::ECI(loc);
}

// =================================================================================================
//! Get the Inertial location (in meters).
//! @return the location in the ECI frame.
void UtEntity::SetLocation(const ut::coords::ECI& aLocationECI)
{
   SetLocationECI(aLocationECI.GetData());
}

// =================================================================================================
//! Get the Inertial location (in meters).
//! @return the location in the ECI frame.
void UtEntity::SetLocationECI(const UtVec3d& aLocationECI)
{
   SetLocationECI(aLocationECI.GetData());
}

// =================================================================================================
//! Get the TOD location (in meters).
//! @param aLocationTOD is the location in the TOD frame.
void UtEntity::GetLocationTOD(double aLocationTOD[3]) const
{
   if (!mInertialLocationIsValid)
   {
      UpdateInertialLocation();
   }
   UtVec3d::Set(aLocationTOD, GetECI_Conversion().GetLocationTOD().GetData());
}

// =================================================================================================
//! Get the TOD location (in meters).
//! @return the location in the TOD frame.
UtVec3d UtEntity::GetLocationTOD() const
{
   UtVec3d loc;
   GetLocationTOD(loc.GetData());
   return loc;
}

// =================================================================================================
//! Get the TOD location (in meters).
//! @return the location in the TOD frame.
void UtEntity::SetLocationTOD(const UtVec3d& aLocationTOD)
{
   SetLocationTOD(aLocationTOD.GetData());
}

// =================================================================================================
//! Get the TEME location (in meters).
//! @param aLocationTEME is the location in the TEME frame.
void UtEntity::GetLocationTEME(double aLocationTEME[3]) const
{
   if (!mInertialLocationIsValid)
   {
      UpdateInertialLocation();
   }
   UtVec3d::Set(aLocationTEME, GetECI_Conversion().GetLocationTEME().GetData());
}

// =================================================================================================
//! Get the TEME location (in meters).
//! @return the location in the TEME frame.
UtVec3d UtEntity::GetLocationTEME() const
{
   UtVec3d loc;
   GetLocationTEME(loc.GetData());
   return loc;
}

// =================================================================================================
//! Get the J2000 location (in meters).
//! @param aLocationJ2000 is the location in the J2000 frame.
void UtEntity::GetLocationJ2000(double aLocationJ2000[3]) const
{
   if (!mInertialLocationIsValid)
   {
      UpdateInertialLocation();
   }
   UtVec3d::Set(aLocationJ2000, GetECI_Conversion().GetLocationJ2000().GetData());
}

// =================================================================================================
//! Get the J2000 location (in meters).
//! @return the location in the J2000 frame.
UtVec3d UtEntity::GetLocationJ2000() const
{
   UtVec3d loc;
   GetLocationJ2000(loc.GetData());
   return loc;
}

// =================================================================================================
//! Set the Inertial location (in meters)
void UtEntity::SetLocationECI(const double aLocationECI[3])
{
   GetECI_Conversion().SetLocationECI(aLocationECI);

   mCoordinateFrame         = cCF_INERTIAL;
   mInertialLocationIsValid = true;
   mWCSLocationIsValid      = false;
   mLLALocationIsValid      = false;

   WasMoved();
}

// =================================================================================================
//! Set the TOD location (in meters)
void UtEntity::SetLocationTOD(const double aLocationTOD[3])
{
   GetECI_Conversion().SetLocationTOD(aLocationTOD);

   mCoordinateFrame         = cCF_INERTIAL;
   mInertialLocationIsValid = true;
   mWCSLocationIsValid      = false;
   mLLALocationIsValid      = false;

   WasMoved();
}

// =================================================================================================
//! Get the location in latitude/longitude/altitude.
void UtEntity::GetLocationLLA(double& aLat, double& aLon, double& aAlt) const
{
   if (!mLLALocationIsValid)
   {
      UpdateLLALocation();
   }
   aLat = mLat;
   aLon = mLon;
   aAlt = mAlt;
}

// =================================================================================================
//! Get the location in latitude/longitude/altitude.
ut::coords::LLA UtEntity::GetLocationLLA() const
{
   ut::coords::LLA out;
   GetLocationLLA(out.mLat, out.mLon, out.mAlt);
   return out;
}

// =================================================================================================
//! Set the location using latitude/longitude/altitude.
void UtEntity::SetLocationLLA(double aLat, double aLon, double aAlt)
{
   mLat = aLat;
   mLon = aLon;
   mAlt = aAlt;

   mCoordinateFrame         = cCF_LLA;
   mLLALocationIsValid      = true;
   mWCSLocationIsValid      = false;
   mInertialLocationIsValid = false;

   WasMoved();
}

// =================================================================================================
//! Set the location using latitude/longitude/altitude.
void UtEntity::SetLocation(const ut::coords::LLA& aLLA)
{
   SetLocationLLA(aLLA.mLat, aLLA.mLon, aLLA.mAlt);
}

// =================================================================================================
//! Get the altitude in meters above the ellipsoid
double UtEntity::GetAltitude() const
{
   if (!mLLALocationIsValid)
   {
      UpdateLLALocation();
   }
   return mAlt;
}

// =================================================================================================
//! Set the spatial location, movement and orientation in the Inertial coordinate system.
//! Location is in meters; velocity is in meters/sec;
//! acceleration is in meters/sec^2; orientation is Euler angles
//! (psi-theta-phi radians).  The simulation time parameter is used to
//! update the current epoch used for Inertial conversions.
void UtEntity::SetSpatialECI(const double aSimTime,
                             const double aLocationECI[3],
                             const double aVelocityECI[3],
                             const double aAccelerationECI[3],
                             const double aOrientationECI[3])
{
   SetTime(aSimTime);
   SetLocationECI(aLocationECI);
   SetVelocityECI(aVelocityECI);
   SetAccelerationECI(aAccelerationECI);
   SetOrientationECI(aOrientationECI[0], aOrientationECI[1], aOrientationECI[2]);
}

// =================================================================================================
//! Set the spatial location, movement and orientation in the Inertial coordinate system.
//! Location is in meters; velocity is in meters/sec;
//! acceleration is in meters/sec^2; orientation is Euler angles
//! (psi-theta-phi radians).  The simulation time parameter is used to
//! update the current epoch used for Inertial conversions.
void UtEntity::SetSpatialECI(const double                   aSimTime,
                             const ut::coords::ECI&         aLocationECI,
                             const ut::coords::ECI&         aVelocityECI,
                             const ut::coords::ECI&         aAccelerationECI,
                             const ut::coords::EulerAngles& aOrientationECI)
{
   double angles[3] = {aOrientationECI.mPsi, aOrientationECI.mTheta, aOrientationECI.mPhi};
   SetSpatialECI(aSimTime, aLocationECI.GetData(), aVelocityECI.GetData(), aAccelerationECI.GetData(), angles);
}

// =================================================================================================
//! Set the spatial location, movement and orientation in the Inertial coordinate system.
//! Location is in meters; velocity is in meters/sec;
//! acceleration is in meters/sec^2; orientation is Euler angles
//! (psi-theta-phi radians).  The simulation time parameter is used to
//! update the current epoch used for Inertial conversions.
void UtEntity::SetSpatialECI(const double           aSimTime,
                             const ut::coords::ECI& aLocationECI,
                             const ut::coords::ECI& aVelocityECI,
                             const ut::coords::ECI& aAccelerationECI,
                             const UtQuaternion&    aOrientationECI)
{
   SetSpatialECI(aSimTime, aLocationECI, aVelocityECI, aAccelerationECI, aOrientationECI.Get());
}

// =================================================================================================
//! Increment the current position by a delta vector (in the WCS frame) from the current position.
//! This is useful to move entities.
void UtEntity::IncrementLocationWCS(const double aLocationDeltaWCS[3])
{
   // Update the WCS position
   if (!mWCSLocationIsValid)
   {
      UpdateWCSLocation();
   }
   UtVec3d::Add(mLocationWCS, mLocationWCS, aLocationDeltaWCS);

   mCoordinateFrame         = cCF_WCS;
   mWCSLocationIsValid      = true;
   mLLALocationIsValid      = false;
   mInertialLocationIsValid = false;

   WasMoved();
}

// =================================================================================================
//! Increment the current position by a delta vector (in the WCS frame) from the current position.
//! This is useful to move entities.
void UtEntity::IncrementLocation(const ut::coords::WCS& aLocationDeltaWCS)
{
   IncrementLocationWCS(aLocationDeltaWCS.GetData());
}

// =================================================================================================
//! Increment the current position by a delta vector (in the WCS frame) from the current position.
//! This is useful to move entities.
void UtEntity::IncrementLocationWCS(const UtVec3d& aLocationDeltaWCS)
{
   IncrementLocationWCS(aLocationDeltaWCS.GetData());
}

// =================================================================================================
//! Increment the current position by a delta vector (in the NED frame) from the current position.
//! This is useful to move entities.
void UtEntity::IncrementLocationNED(const double aLocationDeltaNED[3])
{
   // Convert the delta vector from NED to WCS
   double positionDeltaWCS[3];
   ConvertNEDVectorToWCS(positionDeltaWCS, aLocationDeltaNED);

   // Update the WCS position
   IncrementLocationWCS(positionDeltaWCS);
}

// =================================================================================================
//! Increment the current position by a delta vector (in the NED frame) from the current position.
//! This is useful to move entities.
void UtEntity::IncrementLocation(const ut::coords::NED& aLocationDeltaNED)
{
   IncrementLocationNED(aLocationDeltaNED.GetData());
}

// =================================================================================================
//! Increment the current position by a delta vector (in the NED frame) from the current position.
//! This is useful to move entities.
void UtEntity::IncrementLocationNED(const UtVec3d& aLocationDeltaNED)
{
   IncrementLocationNED(aLocationDeltaNED.GetData());
}

// =================================================================================================
//! Sets the update simulation time in order to have valid Inertial coordinates.
void UtEntity::SetTimeP(double aTime) const
{
   mUpdateTime = aTime;

   // Invalidate the coordinate frame(s) not set last.
   if (mCoordinateFrame == cCF_INERTIAL)
   {
      mWCSLocationIsValid = false;
      mLLALocationIsValid = false;
   }
   else
   {
      mInertialLocationIsValid = false;
   }
}

// =================================================================================================
// VELOCITY ROUTINES
// =================================================================================================

// =================================================================================================
//! Get the velocity in the WCS (meters/second).
//! @param aVelocityWCS is the velocity in the WCS frame.
void UtEntity::GetVelocityWCS(double aVelocityWCS[3]) const
{
   if (!mWCSVelocityIsValid)
   {
      // Check if Inertial->WCS conversion is needed
      if (mInertialVelocityIsValid)
      {
         if (!mInertialLocationIsValid)
         {
            UpdateInertialLocation();
         }
         GetECI_Conversion().GetVelocityWCS(mVelocityWCS);
      }

      else
      {
         // Ensure the WCS->NED transformation is up-to-date
         if (!mWCSToNEDTransformIsValid)
         {
            UpdateWCSToNEDTransform();
         }
         // Rotate the vector in the NED frame back to the WCS
         UtMat3d::InverseTransform(mVelocityWCS, mWCSToNEDTransform, mVelocityNED);
      }
      mWCSVelocityIsValid = true;
   }
   UtVec3d::Set(aVelocityWCS, mVelocityWCS);
}

//! Get the velocity in the WCS frame (meters/second).
//! @return the velocity in the WCS frame.
ut::coords::WCS UtEntity::GetVelocityWCS() const
{
   UtVec3d vel;
   GetVelocityWCS(vel.GetData());
   return ut::coords::WCS(vel);
}

// =================================================================================================
//! Set the velocity in the WCS (meters/second).
void UtEntity::SetVelocityWCS(const double aVelocityWCS[3])
{
   UtVec3d::Set(mVelocityWCS, aVelocityWCS);
   mWCSVelocityIsValid      = true;
   mInertialVelocityIsValid = false;
   mNEDVelocityIsValid      = false;
}

// =================================================================================================
//! Set the velocity in the WCS (meters/second).
void UtEntity::SetVelocity(const ut::coords::WCS& aVelocityWCS)
{
   SetVelocityWCS(aVelocityWCS.GetData());
}

// =================================================================================================
//! Set the velocity in the WCS (meters/second).
void UtEntity::SetVelocityWCS(const UtVec3d& aVelocityWCS)
{
   SetVelocityWCS(aVelocityWCS.GetData());
}

// private
void UtEntity::UpdateInertialVelocity() const
{
   if (!mInertialLocationIsValid)
   {
      UpdateInertialLocation();
   }
   double velWCS[3];
   GetVelocityWCS(velWCS);
   GetECI_Conversion().SetVelocityWCS(velWCS);
   mInertialVelocityIsValid = true;
}

// =================================================================================================
//! Get the velocity in the ECI (meters/second).
//! @param aVelocityECI is the velocity in the ECI frame.
void UtEntity::GetVelocityECI(double aVelocityECI[3]) const
{
   if (!mInertialVelocityIsValid)
   {
      UpdateInertialVelocity();
   }
   UtVec3d::Set(aVelocityECI, GetECI_Conversion().GetVelocityECI().GetData());
}

//! Get the velocity in the ECI frame (meters/second).
//! @return the velocity in the ECI frame.
ut::coords::ECI UtEntity::GetVelocityECI() const
{
   UtVec3d vel;
   GetVelocityECI(vel.GetData());
   return ut::coords::ECI(vel);
}

// =================================================================================================
//! Get the velocity in the TOD (meters/second).
//! @param aVelocityTOD is the velocity in the TOD frame.
void UtEntity::GetVelocityTOD(double aVelocityTOD[3]) const
{
   if (!mInertialVelocityIsValid)
   {
      UpdateInertialVelocity();
   }
   UtVec3d::Set(aVelocityTOD, GetECI_Conversion().GetVelocityTOD().GetData());
}

//! Get the velocity in the TOD frame (meters/second).
//! @return the velocity in the TOD frame.
UtVec3d UtEntity::GetVelocityTOD() const
{
   UtVec3d vel;
   GetVelocityTOD(vel.GetData());
   return vel;
}

// =================================================================================================
//! Get the velocity in the TEME coordinate system (meters/second).
//! @param aVelocityTEME is the velocity in the TEME frame.
void UtEntity::GetVelocityTEME(double aVelocityTEME[3]) const
{
   if (!mInertialVelocityIsValid)
   {
      UpdateInertialVelocity();
   }
   UtVec3d::Set(aVelocityTEME, GetECI_Conversion().GetVelocityTEME().GetData());
}

//! Get the velocity in the TEME frame (meters/second).
//! @return the velocity in the TEME frame.
UtVec3d UtEntity::GetVelocityTEME() const
{
   UtVec3d vel;
   GetVelocityTEME(vel.GetData());
   return vel;
}

// =================================================================================================
//! Get the velocity in the J2000 coordinate system (meters/second).
//! @param aVelocityJ2000 is the velocity in the J2000 frame.
void UtEntity::GetVelocityJ2000(double aVelocityJ2000[3]) const
{
   if (!mInertialVelocityIsValid)
   {
      UpdateInertialVelocity();
   }
   UtVec3d::Set(aVelocityJ2000, GetECI_Conversion().GetVelocityJ2000().GetData());
}

//! Get the velocity in the J2000 frame (meters/second).
//! @return the velocity in the J2000 frame.
UtVec3d UtEntity::GetVelocityJ2000() const
{
   UtVec3d vel;
   GetVelocityJ2000(vel.GetData());
   return vel;
}

// =================================================================================================
//! Set the velocity in the ECI (meters/second).
void UtEntity::SetVelocityECI(const double aVelocityECI[3])
{
   GetECI_Conversion().SetVelocityECI(aVelocityECI);
   mWCSVelocityIsValid      = false;
   mInertialVelocityIsValid = true;
   mNEDVelocityIsValid      = false;
}

// =================================================================================================
//! Set the velocity in the ECI (meters/second).
void UtEntity::SetVelocity(const ut::coords::ECI& aVelocityECI)
{
   SetVelocityECI(aVelocityECI.GetData());
}

// =================================================================================================
//! Set the velocity in the ECI (meters/second).
void UtEntity::SetVelocityECI(const UtVec3d& aVelocityECI)
{
   SetVelocityECI(aVelocityECI.GetData());
}

// =================================================================================================
//! Set the velocity in the TOD (meters/second).
void UtEntity::SetVelocityTOD(const double aVelocityTOD[3])
{
   GetECI_Conversion().SetVelocityTOD(aVelocityTOD);
   mWCSVelocityIsValid      = false;
   mInertialVelocityIsValid = true;
   mNEDVelocityIsValid      = false;
}

// =================================================================================================
//! Set the velocity in the TOD (meters/second).
void UtEntity::SetVelocityTOD(const UtVec3d& aVelocityTOD)
{
   SetVelocityTOD(aVelocityTOD.GetData());
}

// =================================================================================================
//! Get the velocity in the NED frame (meters/second).
//! @param aVelocityNED is the velocity in the NED frame.
void UtEntity::GetVelocityNED(double aVelocityNED[3]) const
{
   if (!mNEDVelocityIsValid)
   {
      // Ensure the WCS->NED transformation is up-to-date
      if (!mWCSToNEDTransformIsValid)
      {
         UpdateWCSToNEDTransform();
      }
      if (!mWCSVelocityIsValid)
      {
         assert(mInertialVelocityIsValid);
         GetECI_Conversion().GetVelocityWCS(mVelocityWCS);
      }

      // Rotate the vector in the WCS to the NED frame
      UtMat3d::Transform(mVelocityNED, mWCSToNEDTransform, mVelocityWCS);
      mNEDVelocityIsValid = true;
   }
   UtVec3d::Set(aVelocityNED, mVelocityNED);
}

//! Get the velocity in the NED frame (meters/second).
//! @return the velocity in the NED frame.
ut::coords::NED UtEntity::GetVelocityNED() const
{
   UtVec3d vel;
   GetVelocityNED(vel.GetData());
   return ut::coords::NED(vel);
}

// =================================================================================================
//! Set the velocity in the NED frame (meters/second).
void UtEntity::SetVelocityNED(const double aVelocityNED[3])
{
   UtVec3d::Set(mVelocityNED, aVelocityNED);
   mNEDVelocityIsValid      = true;
   mWCSVelocityIsValid      = false;
   mInertialVelocityIsValid = false;
}

// =================================================================================================
//! Set the velocity in the NED frame (meters/second).
void UtEntity::SetVelocity(const ut::coords::NED& aVelocityNED)
{
   SetVelocityNED(aVelocityNED.GetData());
}

// =================================================================================================
//! Set the velocity in the NED frame (meters/second).
void UtEntity::SetVelocityNED(const UtVec3d& aVelocityNED)
{
   SetVelocityNED(aVelocityNED.GetData());
}

// =================================================================================================
//! Get the velocity in the ECS frame (meters/second).
//! @param aVelocityECS is the velocity in the ECS frame.
void UtEntity::GetVelocityECS(double aVelocityECS[3]) const
{
   double tmp[3];
   // This call is to mVelocityWCS is up to date, 'tmp; value is ignored.
   GetVelocityWCS(tmp);
   // Ensure the WCS->ECS transformation is up-to-date
   if (!mWCSToECSTransformIsValid)
   {
      UpdateWCSToECSTransform();
   }
   // tmp now reused to hold VelocityECS
   UtMat3d::Transform(tmp, mWCSToECSTransform, mVelocityWCS);
   UtVec3d::Set(aVelocityECS, tmp);
}

//! Get the velocity in the ECS frame (meters/second).
//! @return the velocity in the ECS frame.
ut::coords::ECS UtEntity::GetVelocityECS() const
{
   UtVec3d vel;
   GetVelocityECS(vel.GetData());
   return ut::coords::ECS(vel);
}

// =================================================================================================
//! Set the velocity in the ECS frame (meters/second).
void UtEntity::SetVelocityECS(const double aVelocityECS[3])
{
   // Ensure the WCS->ECS transformation is up-to-date
   if (!mWCSToECSTransformIsValid)
   {
      UpdateWCSToECSTransform();
   }
   double tmp[3];
   UtMat3d::InverseTransform(tmp, mWCSToECSTransform, aVelocityECS);
   SetVelocityWCS(tmp);
}

// =================================================================================================
//! Set the velocity in the ECS frame (meters/second).
void UtEntity::SetVelocity(const ut::coords::ECS& aVelocityECS)
{
   SetVelocityECS(aVelocityECS.GetData());
}

// =================================================================================================
//! Set the velocity in the ECS frame (meters/second).
void UtEntity::SetVelocityECS(const UtVec3d& aVelocityECS)
{
   SetVelocityECS(aVelocityECS.GetData());
}

// =================================================================================================
//! Get the magnitude of the velocity (meters/second) in the coordinate frame of the entity.
//! @note The coordinate frame of the entity depends on the most recent method used to define its
//! location (i.e.: SetLocationECI, SetLocationWCS, SetLocationLLA, SetSpatialECI). The return value
//! is the speed IN THAT FRAME! Note that the ECEF and Inertial values are DIFFERENT!
//! @note Use GetSpeedECI() or GetSpeedWCS() to get the value in a specific frame.
double UtEntity::GetSpeed() const
{
   double speed = 0.0;
   if (mCoordinateFrame == cCF_INERTIAL) // Return Inertial if frame is Inertial
   {
      if (!mInertialVelocityIsValid)
      {
         double temp[3];
         GetVelocityECI(temp);
      }
      speed = GetECI_Conversion().GetVelocityECI().Magnitude();
   }
   else // Must be one of the ECEF frames (cCF_LLA or cCF_WCS)
   {
      if (mNEDVelocityIsValid)
      {
         speed = UtVec3d::Magnitude(mVelocityNED);
      }
      else // if (mWCSVelocityIsValid)
      {
         speed = UtVec3d::Magnitude(mVelocityWCS); // Return WCS if nothing defined.
      }
   }
   return speed;
}

// =================================================================================================
//! Get the magnitude of the velocity (meters/second) in the Earth-Centered Inertial frame.
double UtEntity::GetSpeedECI() const
{
   if (!mInertialVelocityIsValid)
   {
      double temp[3];
      GetVelocityECI(temp);
   }
   return GetECI_Conversion().GetVelocityECI().Magnitude();
}

// =================================================================================================
//! Get the magnitude of the velocity (meters/second) in the Earth-Centered, Earth-Fixed frame.
double UtEntity::GetSpeedWCS() const
{
   double speed = 0.0;
   if (mNEDVelocityIsValid)
   {
      speed = UtVec3d::Magnitude(mVelocityNED);
   }
   else if (mWCSVelocityIsValid)
   {
      speed = UtVec3d::Magnitude(mVelocityWCS);
   }
   else
   {
      // Neither of the values in the ECEF frames is valid.  The Inertial value *SHOULD* be valid, so
      // convert it to WCS and return its magnitude.
      double temp[3];
      GetVelocityWCS(temp);
      speed = UtVec3d::Magnitude(mVelocityWCS);
   }
   return speed;
}

// =================================================================================================
//! Get the speed (magnitude of the velocity squared; (meters/second)^2).
double UtEntity::GetSpeedSquared() const
{
   double speedSq = 0.0;
   if (mCoordinateFrame == cCF_INERTIAL)
   {
      if (!mInertialVelocityIsValid)
      {
         double temp[3];
         GetVelocityECI(temp);
      }
      speedSq = GetECI_Conversion().GetVelocityECI().MagnitudeSquared();
   }
   else // Must be one of the ECEF frames (cCF_LLA or cCF_WCS)
   {
      if (mNEDVelocityIsValid)
      {
         speedSq = UtVec3d::MagnitudeSquared(mVelocityNED);
      }
      else // if (mWCSVelocityIsValid)
      {
         speedSq = UtVec3d::MagnitudeSquared(mVelocityWCS); // Return WCS if nothing defined.
      }
   }
   return speedSq;
}

// =================================================================================================
// ACCELERATION ROUTINES
// =================================================================================================

// =================================================================================================
//! Get the acceleration in the WCS (meters/second^2).
//! @param aAccelerationWCS is the acceleration in the WCS frame.
void UtEntity::GetAccelerationWCS(double aAccelerationWCS[3]) const
{
   if (!mWCSAccelerationIsValid)
   {
      // Check if Inertial->WCS conversion is needed
      if (mInertialAccelerationIsValid)
      {
         GetECI_Conversion().GetAccelerationWCS(mAccelerationWCS);
      }
      else
      {
         // Ensure the WCS->NED transformation is up-to-date
         if (!mWCSToNEDTransformIsValid)
         {
            UpdateWCSToNEDTransform();
         }
         // Rotate the vector in the NED frame back to the WCS
         UtMat3d::InverseTransform(mAccelerationWCS, mWCSToNEDTransform, mAccelerationNED);
      }
      mWCSAccelerationIsValid = true;
   }
   UtVec3d::Set(aAccelerationWCS, mAccelerationWCS);
}

//! Get the acceleration in the WCS frame (meters/second^2).
//! @return the acceleration in the WCS frame.
ut::coords::WCS UtEntity::GetAccelerationWCS() const
{
   UtVec3d accel;
   GetAccelerationWCS(accel.GetData());
   return ut::coords::WCS(accel);
}

// =================================================================================================
//! Set the acceleration in the WCS (meters/second^2).
void UtEntity::SetAccelerationWCS(const double aAccelerationWCS[3])
{
   UtVec3d::Set(mAccelerationWCS, aAccelerationWCS);
   mWCSAccelerationIsValid      = true;
   mNEDAccelerationIsValid      = false;
   mInertialAccelerationIsValid = false;
}

// =================================================================================================
//! Set the acceleration in the WCS (meters/second^2).
void UtEntity::SetAcceleration(const ut::coords::WCS& aAccelerationWCS)
{
   SetAccelerationWCS(aAccelerationWCS.GetData());
}

// =================================================================================================
//! Set the acceleration in the WCS (meters/second^2).
void UtEntity::SetAccelerationWCS(const UtVec3d& aAccelerationWCS)
{
   SetAccelerationWCS(aAccelerationWCS.GetData());
}

// private
void UtEntity::UpdateInertialAcceleration() const
{
   if (!mInertialVelocityIsValid)
   {
      UpdateInertialVelocity();
   }
   double tmp[3];
   GetAccelerationWCS(tmp);
   GetECI_Conversion().SetAccelerationWCS(tmp);
   mInertialAccelerationIsValid = true;
}

// =================================================================================================
//! Get the acceleration in the Inertial (meters/second^2).
//! @param aAccelerationECI is the acceleration in the ECI frame.
void UtEntity::GetAccelerationECI(double aAccelerationECI[3]) const
{
   if (!mInertialAccelerationIsValid)
   {
      UpdateInertialAcceleration();
   }
   UtVec3d::Set(aAccelerationECI, GetECI_Conversion().GetAccelerationECI().GetData());
}

//! Get the acceleration in the ECI frame (meters/second^2).
//! @return the acceleration in the ECI frame.
ut::coords::ECI UtEntity::GetAccelerationECI() const
{
   ut::coords::ECI accel;
   GetAccelerationECI(accel.GetData());
   return accel;
}

// =================================================================================================
//! Set the acceleration in the Inertial (meters/second^2).
void UtEntity::SetAccelerationECI(const double aAccelerationECI[3])
{
   GetECI_Conversion().SetAccelerationECI(aAccelerationECI);
   mWCSAccelerationIsValid      = false;
   mNEDAccelerationIsValid      = false;
   mInertialAccelerationIsValid = true;
}

// =================================================================================================
//! Set the acceleration in the Inertial (meters/second^2).
void UtEntity::SetAcceleration(const ut::coords::ECI& aAccelerationECI)
{
   SetAccelerationECI(aAccelerationECI.GetData());
}

// =================================================================================================
//! Set the acceleration in the Inertial (meters/second^2).
void UtEntity::SetAccelerationECI(const UtVec3d& aAccelerationECI)
{
   SetAccelerationECI(aAccelerationECI.GetData());
}

// =================================================================================================
//! Get the acceleration in the TOD (meters/second^2).
//! @param aAccelerationTOD is the acceleration in the TOD frame.
void UtEntity::GetAccelerationTOD(double aAccelerationTOD[3]) const
{
   if (!mInertialAccelerationIsValid)
   {
      UpdateInertialAcceleration();
   }
   UtVec3d::Set(aAccelerationTOD, GetECI_Conversion().GetAccelerationTOD().GetData());
}

//! Get the acceleration in the TOD frame (meters/second^2).
//! @return the acceleration in the TOD frame.
UtVec3d UtEntity::GetAccelerationTOD() const
{
   UtVec3d accel;
   GetAccelerationTOD(accel.GetData());
   return accel;
}

// =================================================================================================
//! Set the acceleration in the TOD (meters/second^2).
void UtEntity::SetAccelerationTOD(const double aAccelerationTOD[3])
{
   GetECI_Conversion().SetAccelerationTOD(aAccelerationTOD);
   mWCSAccelerationIsValid      = false;
   mNEDAccelerationIsValid      = false;
   mInertialAccelerationIsValid = true;
}

// =================================================================================================
//! Set the acceleration in the TOD (meters/second^2).
void UtEntity::SetAccelerationTOD(const UtVec3d& aAccelerationECI)
{
   SetAccelerationTOD(aAccelerationECI.GetData());
}

// =================================================================================================
//! Get the acceleration in the NED frame (meters/second^2).
//! @param aAccelerationNED is the acceleration in the NED frame.
void UtEntity::GetAccelerationNED(double aAccelerationNED[3]) const
{
   if (!mNEDAccelerationIsValid)
   {
      if (mInertialAccelerationIsValid)
      {
         GetECI_Conversion().GetAccelerationWCS(mAccelerationWCS);
      }
      // Ensure the WCS->NED transformation is up-to-date
      if (!mWCSToNEDTransformIsValid)
      {
         UpdateWCSToNEDTransform();
      }
      // Rotate the vector in the WCS to the NED frame
      UtMat3d::Transform(mAccelerationNED, mWCSToNEDTransform, mAccelerationWCS);
      mNEDAccelerationIsValid = true;
   }
   UtVec3d::Set(aAccelerationNED, mAccelerationNED);
}

//! Get the acceleration in the NED frame (meters/second^2).
//! @return the acceleration in the NED frame.
ut::coords::NED UtEntity::GetAccelerationNED() const
{
   ut::coords::NED accel;
   GetAccelerationNED(accel.GetData());
   return accel;
}

// =================================================================================================
//! Set the acceleration in the NED frame (meters/second^2).
void UtEntity::SetAccelerationNED(const double aAccelerationNED[3])
{
   UtVec3d::Set(mAccelerationNED, aAccelerationNED);
   mNEDAccelerationIsValid      = true;
   mWCSAccelerationIsValid      = false;
   mInertialAccelerationIsValid = false;
}

// =================================================================================================
//! Set the acceleration in the NED frame (meters/second^2).
void UtEntity::SetAcceleration(const ut::coords::NED& aAccelerationNED)
{
   SetAccelerationNED(aAccelerationNED.GetData());
}

// =================================================================================================
//! Set the acceleration in the NED frame (meters/second^2).
void UtEntity::SetAccelerationNED(const UtVec3d& aAccelerationNED)
{
   SetAccelerationNED(aAccelerationNED.GetData());
}

// =================================================================================================
//! Get the acceleration in the ECS frame (meters/second^2).
//! @param aAccelerationECS is the acceleration in the ECS frame.
void UtEntity::GetAccelerationECS(double aAccelerationECS[3]) const
{
   double tmp[3];
   // This call is to insure mAccelerationWCS is up to date, 'tmp' value is ignored
   GetAccelerationWCS(tmp);
   // Ensure the WCS->ECS transformation is up-to-date
   if (!mWCSToECSTransformIsValid)
   {
      UpdateWCSToECSTransform();
   }
   // tmp now reused to hold AccelerationECS
   UtMat3d::Transform(tmp, mWCSToECSTransform, mAccelerationWCS);
   UtVec3d::Set(aAccelerationECS, tmp);
}

//! Get the acceleration in the ECS frame (meters/second^2).
//! @return the acceleration in the ECS frame.
ut::coords::ECS UtEntity::GetAccelerationECS() const
{
   ut::coords::ECS accel;
   GetAccelerationECS(accel.GetData());
   return accel;
}

// =================================================================================================
//! Set the acceleration in the NED frame (meters/second^2).
void UtEntity::SetAccelerationECS(const double aAccelerationECS[3])
{
   // Ensure the WCS->ECS transformation is up-to-date
   if (!mWCSToECSTransformIsValid)
   {
      UpdateWCSToECSTransform();
   }
   double tmp[3];
   UtMat3d::InverseTransform(tmp, mWCSToECSTransform, aAccelerationECS);
   SetAccelerationWCS(tmp);
}

// =================================================================================================
//! Set the acceleration in the NED frame (meters/second^2).
void UtEntity::SetAcceleration(const ut::coords::ECS& aAccelerationECS)
{
   SetAccelerationECS(aAccelerationECS.GetData());
}

// =================================================================================================
//! Set the acceleration in the NED frame (meters/second^2).
void UtEntity::SetAccelerationECS(const UtVec3d& aAccelerationECS)
{
   SetAccelerationECS(aAccelerationECS.GetData());
}

// =================================================================================================
//! Get the magnitude of the acceleration (meters/second^2) in the coordinate frame of the entity.
//! (i.e.: WCS/NED for Earth-centered, Earth-fixed (ECEF), Inertial for Earth-centered inertial.
//! @note The coordinate frame of the entity depends on the most recent method used to define its
//! location (i.e.: SetLocationECI, SetLocationWCS, SetLocationLLA, SetSpatialECI). The return value
//! is the acceleration IN THAT FRAME! Note that the ECEF and Inertial values are DIFFERENT!
//! @note Use GetAccelerationECI() or GetAccelerationWCS() to get the value in a specific frame.
double UtEntity::GetAccelerationMagnitude() const
{
   double accel = 0.0;
   if (mCoordinateFrame == cCF_INERTIAL) // Return Inertial if frame is Inertial
   {
      if (!mInertialAccelerationIsValid)
      {
         double temp[3];
         GetAccelerationECI(temp);
      }
      accel = GetECI_Conversion().GetAccelerationECI().Magnitude();
   }
   else // Must be one of the ECEF frames (cCF_LLA or cCF_WCS)
   {
      if (mNEDAccelerationIsValid)
      {
         accel = UtVec3d::Magnitude(mAccelerationNED);
      }
      else // if (mWCSAccelerationIsValid)
      {
         accel = UtVec3d::Magnitude(mAccelerationWCS); // Return WCS if nothing defined.
      }
   }
   return accel;
}

// =================================================================================================
//! Get the magnitude of the acceleration (meters/second^2) in the Earth-Centered Inertial frame.
double UtEntity::GetAccelerationMagnitudeECI() const
{
   if (!mInertialAccelerationIsValid)
   {
      double temp[3];
      GetAccelerationECI(temp);
   }
   return GetECI_Conversion().GetAccelerationECI().Magnitude();
}

// =================================================================================================
//! Get the magnitude of the acceleration (meters/second^2) in the Earth-Centered, Earth-Fixed frame.
double UtEntity::GetAccelerationMagnitudeWCS() const
{
   double accel = 0.0;
   if (mNEDAccelerationIsValid)
   {
      accel = UtVec3d::Magnitude(mAccelerationNED);
   }
   else if (mWCSAccelerationIsValid)
   {
      accel = UtVec3d::Magnitude(mAccelerationWCS);
   }
   else
   {
      // Neither of the values in the ECEF frames is valid.  The Inertial value *SHOULD* be valid, so
      // convert it to WCS and return its magnitude.
      double temp[3];
      GetAccelerationWCS(temp);
      accel = UtVec3d::Magnitude(mAccelerationWCS);
   }
   return accel;
}

// =================================================================================================
// ORIENTATION ROUTINES
// =================================================================================================

// =================================================================================================
//! Get the orientation of the ECS relative to the WCS.
void UtEntity::GetOrientationWCS(double& aPsiWCS, double& aThetaWCS, double& aPhiWCS) const
{
   if (!mWCSOrientationIsValid)
   {
      // Ensure the WCS->ECS transformation is up-to-date
      if (!mWCSToECSTransformIsValid)
      {
         UpdateWCSToECSTransform();
      }
      ExtractEulerAngles(mWCSToECSTransform, mPsiWCS, mThetaWCS, mPhiWCS);
      mWCSOrientationIsValid = true;
   }
   aPsiWCS   = mPsiWCS;
   aThetaWCS = mThetaWCS;
   aPhiWCS   = mPhiWCS;
}

// =================================================================================================
//! Get the orientation of the ECS relative to the WCS.
ut::coords::EulerAngles UtEntity::GetOrientationWCS() const
{
   ut::coords::EulerAngles out;
   GetOrientationWCS(out.mPsi, out.mTheta, out.mPhi);
   return out;
}

// =================================================================================================
//! Get the orientation of the ECS relative to the WCS.
UtQuaternion UtEntity::GetOrientationWCSq() const
{
   return GetOrientationWCS();
}

// =================================================================================================
//! Get the orientation of the ECS relative to the WCS.
UtDCM UtEntity::GetDcmWCStoECS() const
{
   if (!mWCSLocationIsValid)
   {
      UpdateWCSLocation();
   }
   double a;
   double b;
   double c;
   GetOrientationWCS(a, b, c);
   return UtDCM(mWCSToECSTransform);
}

// =================================================================================================
//! Set the orientation of the ECS relative to the WCS.
void UtEntity::SetOrientationWCS(double aPsiWCS, double aThetaWCS, double aPhiWCS)
{
   mPsiWCS   = UtMath::NormalizeAngleMinusPi_Pi(aPsiWCS);
   mThetaWCS = UtMath::NormalizeAngleMinusPi_Pi(aThetaWCS);
   mPhiWCS   = UtMath::NormalizeAngleMinusPi_Pi(aPhiWCS);

   mWCSOrientationIsValid      = true;
   mNEDOrientationIsValid      = false;
   mInertialOrientationIsValid = false;

   WasRotated();
}

// =================================================================================================
//! Set the orientation of the ECS relative to the WCS.
void UtEntity::SetOrientationWCS(const ut::coords::EulerAngles& aAnglesWCS)
{
   SetOrientationWCS(aAnglesWCS.mPsi, aAnglesWCS.mTheta, aAnglesWCS.mPhi);
}

// =================================================================================================
//! Set the orientation of the ECS relative to the WCS.
void UtEntity::SetOrientationWCS(const UtQuaternion& aOrientationWCS)
{
   SetOrientationWCS(aOrientationWCS.Get());
}

// =================================================================================================
//! Get the orientation of the ECS relative to the Inertial.
void UtEntity::GetOrientationECI(double& aPsiECI, double& aThetaECI, double& aPhiECI) const
{
   if (!mInertialOrientationIsValid)
   {
      // Ensure the Inertial->ECS transformation is up-to-date
      if (!mECIToECSTransformIsValid)
      {
         UpdateECIToECSTransform();
      }
      ExtractEulerAngles(mECIToECSTransform, mPsiECI, mThetaECI, mPhiECI);
      mInertialOrientationIsValid = true;
   }

   aPsiECI   = mPsiECI;
   aThetaECI = mThetaECI;
   aPhiECI   = mPhiECI;
}

// =================================================================================================
//! Get the orientation of the ECS relative to the Inertial.
ut::coords::EulerAngles UtEntity::GetOrientationECI() const
{
   ut::coords::EulerAngles out;
   GetOrientationECI(out.mPsi, out.mTheta, out.mPhi);
   return out;
}

// =================================================================================================
//! Get the orientation of the ECS relative to the Inertial.
UtQuaternion UtEntity::GetOrientationECIq() const
{
   return GetOrientationECI();
}

// =================================================================================================
//! Set the orientation of the ECS relative to the Inertial.
void UtEntity::SetOrientationECI(double aPsiECI, double aThetaECI, double aPhiECI)
{
   mPsiECI   = UtMath::NormalizeAngleMinusPi_Pi(aPsiECI);
   mThetaECI = UtMath::NormalizeAngleMinusPi_Pi(aThetaECI);
   mPhiECI   = UtMath::NormalizeAngleMinusPi_Pi(aPhiECI);

   mInertialOrientationIsValid = true;
   mWCSOrientationIsValid      = false;
   mNEDOrientationIsValid      = false;

   WasRotated();
}

// =================================================================================================
//! Set the orientation of the ECS relative to the Inertial.
void UtEntity::SetOrientationECI(const ut::coords::EulerAngles& aAnglesECI)
{
   SetOrientationECI(aAnglesECI.mPsi, aAnglesECI.mTheta, aAnglesECI.mPhi);
}

// =================================================================================================
//! Set the orientation of the ECS relative to the Inertial.
void UtEntity::SetOrientationECI(const UtQuaternion& aOrientationWCS)
{
   SetOrientationECI(aOrientationWCS.Get());
}

// =================================================================================================
//! Get the orientation of the ECS in the NED frame.
void UtEntity::GetOrientationNED(double& aHeadingNED, double& aPitchNED, double& aRollNED) const
{
   if (!mNEDOrientationIsValid)
   {
      // Ensure the NED->ECS transformation is up-to-date
      if (!mNEDToECSTransformIsValid)
      {
         UpdateNEDToECSTransform();
      }
      ExtractEulerAngles(mNEDToECSTransform, mHeadingNED, mPitchNED, mRollNED);
      mNEDOrientationIsValid = true;
   }
   aHeadingNED = mHeadingNED;
   aPitchNED   = mPitchNED;
   aRollNED    = mRollNED;
}

// =================================================================================================
//! Get the orientation of the ECS in the NED frame.
ut::coords::EulerAngles UtEntity::GetOrientationNED() const
{
   ut::coords::EulerAngles out;
   GetOrientationNED(out.mPsi, out.mTheta, out.mPhi);
   return out;
}

// =================================================================================================
//! Get the orientation of the ECS in the NED frame.
UtQuaternion UtEntity::GetOrientationNEDq() const
{
   return GetOrientationNED();
}

// =================================================================================================
//! Get the orientation of the WCS relative to the NED.
UtDCM UtEntity::GetDcmWCStoNED() const
{
   if (!mWCSLocationIsValid)
   {
      UpdateWCSLocation();
   }
   if (!mWCSToNEDTransformIsValid)
   {
      UpdateWCSToNEDTransform();
   }

   double a;
   double b;
   double c;
   GetOrientationNED(a, b, c);
   return UtDCM(mWCSToNEDTransform);
}

// =================================================================================================
//! Set the orientation of the ECS in the NED frame.
void UtEntity::SetOrientationNED(double aHeadingNED, double aPitchNED, double aRollNED)
{
   mHeadingNED = UtMath::NormalizeAngleMinusPi_Pi(aHeadingNED);
   mPitchNED   = UtMath::NormalizeAngleMinusPi_Pi(aPitchNED);
   mRollNED    = UtMath::NormalizeAngleMinusPi_Pi(aRollNED);

   mNEDOrientationIsValid      = true;
   mWCSOrientationIsValid      = false;
   mInertialOrientationIsValid = false;
   WasRotated();
}

// =================================================================================================
//! Set the orientation of the ECS in the NED frame.
void UtEntity::SetOrientationNED(const ut::coords::EulerAngles& aAnglesNED)
{
   SetOrientationNED(aAnglesNED.mPsi, aAnglesNED.mTheta, aAnglesNED.mPhi);
}

// =================================================================================================
//! Set the orientation of the ECS in the NED frame.
void UtEntity::SetOrientationNED(const UtQuaternion& aOrientationNED)
{
   SetOrientationNED(aOrientationNED.Get());
}

// =================================================================================================
//! Given a relative location vector (in WCS) to another object,
//! compute the azimuth and elevation (in radians) of another
//! object relative to the entity coordinate system.
//! @note The input vector should have been computed with either of the GetRelativeLocationWCS().
void UtEntity::ComputeAspect(const double aRelativeLocationWCS[3], double& aRelativeAzimuth, double& aRelativeElevation) const
{
   // Transform the relative WCS position vector to a ECS location and compute the azimuth and elevation.

   double locationECS[3];
   ConvertWCSVectorToECS(locationECS, aRelativeLocationWCS);
   ComputeAzimuthAndElevation(locationECS, aRelativeAzimuth, aRelativeElevation);
}

// =================================================================================================
//! Given a relative location vector (in WCS) to another object,
//! compute the azimuth and elevation (in radians) of another
//! object relative to the entity coordinate system.
//! @note The input vector should have been computed with either of the GetRelativeLocationWCS().
ut::coords::Aspect UtEntity::ComputeAspect(ut::coords::WCS aRelativeLocationWCS) const
{
   ut::coords::Aspect out;
   ComputeAspect(aRelativeLocationWCS.GetData(), out.mAz, out.mEl);
   return out;
}

// =================================================================================================
//! Compute both the relative location vector (in ECI) to another
//! object, and the azimuth and elevation (in radians) of that
//! object relative to the entity coordinate system.
//! This function calls GetRelativeLocationECI() to get the
//! relative location of the other entity.
void UtEntity::ComputeAspectECI(double          aSimTime,
                                const UtEntity* aOtherEntityPtr,
                                double          aRelativeLocationECI[3],
                                double&         aRelativeAzimuth,
                                double&         aRelativeElevation) const
{
   // Get the relative location of the other object in ECI
   GetRelativeLocationECI(aSimTime, aOtherEntityPtr, aRelativeLocationECI);

   double aRelativeLocationWCS[3];
   GetECI_Conversion().ConvertECI_ToWCS(aRelativeLocationECI, aRelativeLocationWCS);

   ComputeAspect(aRelativeLocationWCS, aRelativeAzimuth, aRelativeElevation);
}

// =================================================================================================
//! Compute both the relative location vector (in ECI) to another
//! object, and the azimuth and elevation (in radians) of that
//! object relative to the entity coordinate system.
//! This function calls GetRelativeLocationECI() to get the
//! relative location of the other entity.
ut::coords::Aspect UtEntity::ComputeAspectECI(double aSimTime, const UtEntity* aOtherEntityPtr) const
{
   ut::coords::Aspect out;
   double             dummy[3];

   ComputeAspectECI(aSimTime, aOtherEntityPtr, dummy, out.mAz, out.mEl);

   return out;
}

// =================================================================================================
// RELATIVE GEOMETRY ROUTINES
// =================================================================================================

// =================================================================================================
//! Compute the relative WCS location of another object given its relative spherical coordinates.
//! @param aRelativeAzimuth     [input]  The azimuth of the other object with respect to this object (radians).
//! @param aRelativeElevation   [input]  The elevation of the other object with respect to this object (radians).
//! @param aDistance            [output] The distance from this object to the other object (meters).
//! @param aRelativeLocationWCS [output] The relative WCS location of the other object.
void UtEntity::GetRelativeLocationWCS(double aRelativeAzimuth,
                                      double aRelativeElevation,
                                      double aDistance,
                                      double aRelativeLocationWCS[3]) const
{
   // Compute the XYZ components in the ECS
   double z  = -aDistance * sin(aRelativeElevation);
   double xy = sqrt((aDistance * aDistance) - (z * z));
   double x  = xy * cos(aRelativeAzimuth);
   double y  = xy * sin(aRelativeAzimuth);

   // Convert the ECS vector into a relative WCS vector.
   double relativeLocationECS[3] = {x, y, z};
   ConvertECSVectorToWCS(aRelativeLocationWCS, relativeLocationECS);
}

// =================================================================================================
//! Compute the relative WCS location of another object given its relative spherical coordinates.
//! @param aRelativeAzimuth     [input]  The azimuth of the other object with respect to this object (radians).
//! @param aRelativeElevation   [input]  The elevation of the other object with respect to this object (radians).
//! @param aDistance            [output] The distance from this object to the other object (meters).
//! @param aRelativeLocationWCS [output] The relative WCS location of the other object.
ut::coords::WCS UtEntity::GetRelativeLocationWCS(ut::coords::Aspect aAspect, double aDistance) const
{
   ut::coords::WCS out;
   GetRelativeLocationWCS(aAspect.mAz, aAspect.mEl, aDistance, out.GetData());
   return out;
}

// =================================================================================================
//! Return the WCS vector that points from this location to another location.
//!
//! This routine computes the WCS vector that points from this location to some other
//! location of interest.
//!
//! @param aOtherLocationWCS [input]  The WCS position vector of the other location.
//! @param aLocationWCS      [output] The WCS vector that points from this location to the
//!                                   other location.
void UtEntity::GetRelativeLocationWCS(const double aOtherLocationWCS[3], double aLocationWCS[3]) const
{
   if (!mWCSLocationIsValid)
   {
      UpdateWCSLocation();
   }
   UtVec3d::Subtract(aLocationWCS, aOtherLocationWCS, mLocationWCS);
}

// =================================================================================================
//! Return the WCS vector that points from this location to another location.
//!
//! This routine computes the WCS vector that points from this location to some other
//! location of interest.
//!
//! @param aOtherLocationWCS [input]  The WCS position vector of the other location.
//! @param aLocationWCS      [output] The WCS vector that points from this location to the
//!                                   other location.
ut::coords::WCS UtEntity::GetRelativeLocationWCS(const ut::coords::WCS& aOtherLocationWCS) const
{
   ut::coords::WCS out;
   GetRelativeLocationWCS(aOtherLocationWCS.GetData(), out.GetData());
   return out;
}

// =================================================================================================
//! Return the WCS position vector of another entity relative to this entity.
void UtEntity::GetRelativeLocationWCS(const UtEntity* aOtherEntityPtr, double aLocationWCS[3]) const
{
   if (!mWCSLocationIsValid)
   {
      UpdateWCSLocation();
   }
   if (!aOtherEntityPtr->mWCSLocationIsValid)
   {
      aOtherEntityPtr->UpdateWCSLocation();
   }
   UtVec3d::Subtract(aLocationWCS, aOtherEntityPtr->mLocationWCS, mLocationWCS);
}

// =================================================================================================
//! Return the WCS position vector of another entity relative to this entity.
ut::coords::WCS UtEntity::GetRelativeLocationWCS(const UtEntity* aOtherEntityPtr) const
{
   ut::coords::WCS out;
   GetRelativeLocationWCS(aOtherEntityPtr, out.GetData());
   return out;
}

// =================================================================================================
//! Return the ECI position vector of another entity relative to this entity.
void UtEntity::GetRelativeLocationECI(double aSimTime, const UtEntity* aOtherEntityPtr, double aLocationECI[3]) const
{
   if (!mInertialLocationIsValid)
   {
      UpdateInertialLocation(aSimTime);
   }
   if (!aOtherEntityPtr->mInertialLocationIsValid)
   {
      aOtherEntityPtr->UpdateInertialLocation(aSimTime);
   }
   UtVec3d::Subtract(aLocationECI,
                     aOtherEntityPtr->GetECI_Conversion().GetLocationECI().GetData(),
                     GetECI_Conversion().GetLocationECI().GetData());
}

// =================================================================================================
//! Return the ECI position vector of another entity relative to this entity.
ut::coords::ECI UtEntity::GetRelativeLocationECI(double aSimTime, const UtEntity* aOtherEntityPtr) const
{
   ut::coords::ECI out;
   GetRelativeLocationECI(aSimTime, aOtherEntityPtr, out.GetData());
   return out;
}

// =================================================================================================
//! Return the NED position vector of another entity relative to this entity.
void UtEntity::GetRelativeLocationNED(const UtEntity* aOtherEntityPtr, double aLocationNED[3]) const
{
   // Ensure the WCS->NED transformation is up-to-date
   if (!mWCSToNEDTransformIsValid)
   {
      UpdateWCSToNEDTransform();
   }

   double positionWCS[3];
   GetRelativeLocationWCS(aOtherEntityPtr, positionWCS);
   UtMat3d::Transform(aLocationNED, mWCSToNEDTransform, positionWCS);
}

// =================================================================================================
//! Return the NED position vector of another entity relative to this entity.
ut::coords::NED UtEntity::GetRelativeLocationNED(const UtEntity* aOtherEntityPtr) const
{
   ut::coords::NED out;
   GetRelativeLocationNED(aOtherEntityPtr, out.GetData());
   return out;
}

// =================================================================================================
//! Return the ECS position vector of another entity relative to this entity.
void UtEntity::GetRelativeLocationECS(const UtEntity* aOtherEntityPtr, double aLocationECS[3]) const
{
   if (!mWCSToECSTransformIsValid)
   {
      UpdateWCSToECSTransform();
   }
   double positionWCS[3];
   GetRelativeLocationWCS(aOtherEntityPtr, positionWCS);
   UtMat3d::Transform(aLocationECS, mWCSToECSTransform, positionWCS);
}

// =================================================================================================
//! Return the ECS position vector of another entity relative to this entity.
ut::coords::ECS UtEntity::GetRelativeLocationECS(const UtEntity* aOtherEntityPtr) const
{
   ut::coords::ECS out;
   GetRelativeLocationECS(aOtherEntityPtr, out.GetData());
   return out;
}

// =================================================================================================
//! Given a position in the ECS of this entity, return the equivalent WCS coordinates.
//! (This is invoked from UtEntityPart to get the WCS position of the PCS origin).
void UtEntity::ConvertECSToWCS(const double aLocationECS[3], double aLocationWCS[3]) const
{
   if (!mWCSToECSTransformIsValid)
   {
      UpdateWCSToECSTransform();
   }
   UtMat3d::InverseTransform(aLocationWCS, mWCSToECSTransform, aLocationECS);
   UtVec3d::Add(aLocationWCS, aLocationWCS, mLocationWCS);
}

// =================================================================================================
//! Given a position in the ECS of this entity, return the equivalent WCS coordinates.
//! (This is invoked from UtEntityPart to get the WCS position of the PCS origin).
ut::coords::WCS UtEntity::ConvertECSToWCS(const ut::coords::ECS& aLocationECS) const
{
   ut::coords::WCS out;
   ConvertECSToWCS(aLocationECS.GetData(), out.GetData());
   return out;
}

// =================================================================================================
//! Given a position in NED coordinates (relative to this entity), return the WCS coordinates of the position.
void UtEntity::ConvertNEDToWCS(const double aLocationNED[3], double aLocationWCS[3]) const
{
   // Ensure the WCS->NED transformation is up-to-date
   if (!mWCSToNEDTransformIsValid)
   {
      UpdateWCSToNEDTransform();
   }
   // WCS location is not required to compute the above transform (it is computed using LLA), but it is below, so
   if (!mWCSLocationIsValid)
   {
      UpdateWCSLocation();
   }

   ut::EllipsoidalCentralBody::ConvertLocalToECEF(mLocationWCS,
                                                  mWCSToNEDTransform,
                                                  aLocationNED,  // Inputs
                                                  aLocationWCS); // Output
}

// =================================================================================================
//! Given a position in NED coordinates (relative to this entity), return the WCS coordinates of the position.
ut::coords::WCS UtEntity::ConvertNEDToWCS(const ut::coords::NED& aLocationNED) const
{
   ut::coords::WCS out;
   ConvertNEDToWCS(aLocationNED.GetData(), out.GetData());
   return out;
}

// =================================================================================================
//! Given a position in NED coordinates (relative to this entity), return the ECI coordinates of the position.
void UtEntity::ConvertNEDToECI(const double aLocationNED[3], double aLocationECI[3]) const
{
   double locationWCS[3];
   ConvertNEDToWCS(aLocationNED, locationWCS);
   GetECI_Conversion().ConvertWCS_ToECI(locationWCS, aLocationECI);
}

// =================================================================================================
//! Given a position in NED coordinates (relative to this entity), return the ECI coordinates of the position.
ut::coords::ECI UtEntity::ConvertNEDToECI(const ut::coords::NED& aLocationNED) const
{
   ut::coords::ECI out;
   ConvertNEDToECI(aLocationNED.GetData(), out.GetData());
   return out;
}

// =================================================================================================
//! Given a position in WCS coordinates (relative to this entity), return the ECI coordinates of the position.
void UtEntity::ConvertWCSToECI(const double aLocationWCS[3], double aLocationECI[3]) const
{
   GetECI_Conversion().ConvertWCS_ToECI(aLocationWCS, aLocationECI);
}

// =================================================================================================
//! Given a position in WCS coordinates (relative to this entity), return the ECI coordinates of the position.
ut::coords::ECI UtEntity::ConvertWCSToECI(const ut::coords::WCS& aLocationWCS) const
{
   ut::coords::ECI out;
   ConvertWCSToECI(aLocationWCS.GetData(), out.GetData());
   return out;
}

// =================================================================================================
//! Given a position in ECI coordinates (relative to this entity), return the WCS coordinates of the position.
void UtEntity::ConvertECIToWCS(const double aLocationECI[3], double aLocationWCS[3]) const
{
   GetECI_Conversion().ConvertECI_ToWCS(aLocationECI, aLocationWCS);
}

// =================================================================================================
//! Given a position in ECI coordinates (relative to this entity), return the WCS coordinates of the position.
ut::coords::WCS UtEntity::ConvertECIToWCS(const ut::coords::ECI& aLocationECI) const
{
   ut::coords::WCS out;
   ConvertECIToWCS(aLocationECI.GetData(), out.GetData());
   return out;
}

// =================================================================================================
//! Given a position in NED coordinates (relative to this entity), return the LLA coordinates of the position.
void UtEntity::ConvertNEDToLLA(const double aLocationNED[3], double& aLat, double& aLon, double& aAlt) const
{
   // Ensure the WCS->NED transformation is up-to-date
   if (!mWCSToNEDTransformIsValid)
   {
      // This will also guarantee that mWCSLocationIsValid = true;
      UpdateWCSToNEDTransform();
   }
   // WCS location is not required to compute the above transform (it is computed using LLA), but it is below, so
   if (!mWCSLocationIsValid)
   {
      UpdateWCSLocation();
   }

   ut::EllipsoidalCentralBody::ConvertLocalToLLA(mLocationWCS,
                                                 mWCSToNEDTransform,
                                                 aLocationNED,
                                                 GetCentralBody().GetEllipsoid(), // Inputs
                                                 aLat,
                                                 aLon,
                                                 aAlt); // Outputs
}

// =================================================================================================
//! Given a position in NED coordinates (relative to this entity), return the LLA coordinates of the position.
ut::coords::LLA UtEntity::ConvertNEDToLLA(const ut::coords::NED& aLocationNED) const
{
   ut::coords::LLA out;
   ConvertNEDToLLA(aLocationNED.GetData(), out.mLat, out.mLon, out.mAlt);
   return out;
}

// =================================================================================================
//! Given a position in LLA coordinates return the NED coordinates of the position (relative to this entity).
void UtEntity::ConvertLLAToNED(double aLat, double aLon, double aAlt, double aLocationNED[3]) const
{
   // Ensure the WCS->NED transformation is up-to-date
   if (!mWCSToNEDTransformIsValid)
   {
      UpdateWCSToNEDTransform();
   }
   // WCS location is not required to compute the above transform (it is computed using LLA), but it is below, so
   if (!mWCSLocationIsValid)
   {
      UpdateWCSLocation();
   }

   ut::EllipsoidalCentralBody::ConvertLLAToLocal(mLocationWCS,
                                                 mWCSToNEDTransform,
                                                 aLat,
                                                 aLon,
                                                 aAlt,
                                                 GetCentralBody().GetEllipsoid(), // Inputs
                                                 aLocationNED);                   // Output
}

// =================================================================================================
//! Given a position in LLA coordinates return the NED coordinates of the position (relative to this entity).
ut::coords::NED UtEntity::ConvertLLAToNED(const ut::coords::LLA& aLLA) const
{
   ut::coords::NED out;
   ConvertLLAToNED(aLLA.mLat, aLLA.mLon, aLLA.mAlt, out.GetData());
   return out;
}

// =================================================================================================
//! Given a position in WCS coordinates, return the NED coordinates relative to this entity.
void UtEntity::ConvertWCSToNED(const double aLocationWCS[3], double aLocationNED[3]) const
{
   // Ensure the WCS->NED transformation is up-to-date
   if (!mWCSToNEDTransformIsValid)
   {
      // This will also guarantee that mWCSLocationIsValid = true;
      UpdateWCSToNEDTransform();
   }
   // WCS location is not required to compute the above transform (it is computed using LLA), but it is below, so
   if (!mWCSLocationIsValid)
   {
      UpdateWCSLocation();
   }

   ut::EllipsoidalCentralBody::ConvertECEFToLocal(mLocationWCS,
                                                  mWCSToNEDTransform,
                                                  aLocationWCS,  // Inputs
                                                  aLocationNED); // Output
}

// =================================================================================================
//! Given a position in WCS coordinates, return the NED coordinates relative to this entity.
ut::coords::NED UtEntity::ConvertWCSToNED(const ut::coords::WCS& aLocationWCS) const
{
   ut::coords::NED out;
   ConvertWCSToNED(aLocationWCS.GetData(), out.GetData());
   return out;
}

// =================================================================================================
//! Given a position in WCS coordinates, return the NED coordinates relative to this entity.
void UtEntity::ConvertECIToNED(const double aLocationECI[3], double aLocationNED[3]) const
{
   double locationWCS[3];
   GetECI_Conversion().ConvertECI_ToWCS(aLocationECI, locationWCS);
   ConvertWCSToNED(locationWCS, aLocationNED);
}

// =================================================================================================
//! Given a position in WCS coordinates, return the NED coordinates relative to this entity.
ut::coords::NED UtEntity::ConvertECIToNED(const ut::coords::ECI& aLocationECI) const
{
   ut::coords::NED out;
   ConvertECIToNED(aLocationECI.GetData(), out.GetData());
   return out;
}

// =================================================================================================
//! Given a position in WCS coordinates, return the ECS coordinates relative to this entity.
void UtEntity::ConvertWCSToECS(const double aLocationWCS[3], double aLocationECS[3]) const
{
   if (!mWCSLocationIsValid)
   {
      UpdateWCSLocation();
   }
   if (!mWCSToECSTransformIsValid)
   {
      UpdateWCSToECSTransform();
   }
   double tempWCS[3];
   UtVec3d::Subtract(tempWCS, aLocationWCS, mLocationWCS);
   UtMat3d::Transform(aLocationECS, mWCSToECSTransform, tempWCS);
}

// =================================================================================================
//! Given a position in WCS coordinates, return the ECS coordinates relative to this entity.
ut::coords::ECS UtEntity::ConvertWCSToECS(const ut::coords::WCS& aLocationWCS) const
{
   ut::coords::ECS out;
   ConvertWCSToECS(aLocationWCS.GetData(), out.GetData());
   return out;
}

// =================================================================================================
//! Transform a vector in the WCS frame to the NED frame.
//! This can be used to transform any vector (velocity, acceleration, etc.) from the WCS frame to the NED frame.
void UtEntity::ConvertWCSVectorToNED(double aVectorNED[3], const double aVectorWCS[3]) const
{
   if (!mWCSLocationIsValid)
   {
      UpdateWCSLocation();
   }

   // Ensure the WCS->NED transformation is up-to-date
   if (!mWCSToNEDTransformIsValid)
   {
      UpdateWCSToNEDTransform();
   }
   // Rotate the vector in the WCS to the NED frame
   UtMat3d::Transform(aVectorNED, mWCSToNEDTransform, aVectorWCS);
}

// =================================================================================================
//! Transform a vector in the WCS frame to the NED frame.
//! This can be used to transform any vector (velocity, acceleration, etc.) from the WCS frame to the NED frame.
ut::coords::NED UtEntity::ConvertWCSVectorToNED(const ut::coords::WCS& aVectorWCS) const
{
   ut::coords::NED out;
   ConvertWCSVectorToNED(out.GetData(), aVectorWCS.GetData());
   return out;
}

// =================================================================================================
//! Transform a vector in the NED frame to the WCS frame.
//! This can be used to transform any vector (velocity, acceleration, etc.) from the NED frame to the WCS frame.
void UtEntity::ConvertNEDVectorToWCS(double aVectorWCS[3], const double aVectorNED[3]) const
{
   // Ensure the WCS->NED transformation is up-to-date
   if (!mWCSToNEDTransformIsValid)
   {
      UpdateWCSToNEDTransform();
   }
   // Rotate the vector in the NED to the WCS frame
   UtMat3d::InverseTransform(aVectorWCS, mWCSToNEDTransform, aVectorNED);
}

// =================================================================================================
//! Transform a vector in the NED frame to the WCS frame.
//! This can be used to transform any vector (velocity, acceleration, etc.) from the NED frame to the WCS frame.
ut::coords::WCS UtEntity::ConvertNEDVectorToWCS(const ut::coords::NED& aVectorNED) const
{
   ut::coords::WCS out;
   ConvertNEDVectorToWCS(out.GetData(), aVectorNED.GetData());
   return out;
}

// =================================================================================================
//! Transform a vector in the ECS frame to the WCS frame.
//! This can be used to transform any vector (velocity, acceleration, etc.) from the ECS frame to the WCS frame.
void UtEntity::ConvertECSVectorToWCS(double aVectorWCS[3], const double aVectorECS[3]) const
{
   // Ensure the WCS->ECS transformation is up-to-date
   if (!mWCSToECSTransformIsValid)
   {
      UpdateWCSToECSTransform();
   }
   // Rotate the vector in the ECS to the WCS frame
   UtMat3d::InverseTransform(aVectorWCS, mWCSToECSTransform, aVectorECS);
}

// =================================================================================================
//! Transform a vector in the ECS frame to the WCS frame.
//! This can be used to transform any vector (velocity, acceleration, etc.) from the ECS frame to the WCS frame.
ut::coords::WCS UtEntity::ConvertECSVectorToWCS(const ut::coords::ECS& aVectorECS) const
{
   ut::coords::WCS out;
   ConvertECSVectorToWCS(out.GetData(), aVectorECS.GetData());
   return out;
}

// =================================================================================================
//! Transform a vector in the WCS frame to the ECS frame.
//! This can be used to transform any vector (velocity, acceleration, etc.) from the WCS frame to the ECS frame.
void UtEntity::ConvertWCSVectorToECS(double aVectorECS[3], const double aVectorWCS[3]) const
{
   // Ensure the WCS->ECS transformation is up-to-date
   if (!mWCSToECSTransformIsValid)
   {
      UpdateWCSToECSTransform();
   }
   // Rotate the vector in the WCS to the ECS frame
   UtMat3d::Transform(aVectorECS, mWCSToECSTransform, aVectorWCS);
}

// =================================================================================================
//! Transform a vector in the WCS frame to the ECS frame.
//! This can be used to transform any vector (velocity, acceleration, etc.) from the WCS frame to the ECS frame.
ut::coords::ECS UtEntity::ConvertWCSVectorToECS(const ut::coords::WCS& aVectorWCS) const
{
   ut::coords::ECS out;
   ConvertWCSVectorToECS(out.GetData(), aVectorWCS.GetData());
   return out;
}

// =================================================================================================
//! Convert velocity from ECI to WCS.
void UtEntity::ConvertVelocityECIToWCS(const double aLocationECI[3], const double aVelECI[3], double aVelWCS[3]) const
{
   GetECI_Conversion().ConvertVelocityECI_ToWCS(aLocationECI, aVelECI, aVelWCS);
}

// =================================================================================================
//! Convert velocity from ECI to WCS.
ut::coords::WCS UtEntity::ConvertVelocityECIToWCS(const ut::coords::ECI& aLocationECI, const ut::coords::ECI& aVelECI) const
{
   ut::coords::WCS out;
   ConvertVelocityECIToWCS(aLocationECI.GetData(), aVelECI.GetData(), out.GetData());
   return out;
}

// =================================================================================================
//! Convert velocity from WCS to ECI.
void UtEntity::ConvertVelocityWCSToECI(const double aLocationWCS[3], const double aVelWCS[3], double aVelECI[3]) const
{
   GetECI_Conversion().ConvertVelocityWCS_ToECI(aLocationWCS, aVelWCS, aVelECI);
}

// =================================================================================================
//! Convert velocity from WCS to ECI.
ut::coords::ECI UtEntity::ConvertVelocityWCSToECI(const ut::coords::WCS& aLocationWCS, const ut::coords::WCS& aVelWCS) const
{
   ut::coords::ECI out;
   ConvertVelocityECIToWCS(aLocationWCS.GetData(), aVelWCS.GetData(), out.GetData());
   return out;
}

// =================================================================================================
// Static conversion methods.
// =================================================================================================

// =================================================================================================
//! Convert from spherical to WCS coordinates without having to create an entity.
// static
void UtEntity::ConvertLLAToWCS(double                          aLat,
                               double                          aLon,
                               double                          aAlt,
                               const ut::CentralBodyEllipsoid& aEllipsoid,
                               double                          aLocationWCS[3])
{
   ut::EllipsoidalCentralBody::ConvertLLAToECEF(aLat, aLon, aAlt, aEllipsoid, aLocationWCS);
}

// =================================================================================================
//! Convert from WCS to spherical coordinates without having to create an entity.
// static
void UtEntity::ConvertWCSToLLA(const double                    aLocationWCS[3],
                               const ut::CentralBodyEllipsoid& aEllipsoid,
                               double&                         aLat,
                               double&                         aLon,
                               double&                         aAlt)
{
   ut::EllipsoidalCentralBody::ConvertECEFToLLA(aLocationWCS, aEllipsoid, aLat, aLon, aAlt);
}

// =================================================================================================
//! Convert from spherical to WCS coordinates without having to create an entity.
// static
void UtEntity::ConvertLLAToWCS(double aLat, double aLon, double aAlt, double aLocationWCS[3])
{
   UtEllipsoidalEarth::ConvertLLAToECEF(aLat, aLon, aAlt, aLocationWCS);
}

// =================================================================================================
//! Convert from WCS to spherical coordinates without having to create an entity.
// static
void UtEntity::ConvertWCSToLLA(const double aLocationWCS[3], double& aLat, double& aLon, double& aAlt)
{
   UtEllipsoidalEarth::ConvertECEFToLLA(aLocationWCS, aLat, aLon, aAlt);
}

// =================================================================================================
// UTILITY ROUTINES
// =================================================================================================

// =================================================================================================
//! Given a location in a local frame, compute its azimuth and elevation from the origin in that frame.
//! - Azimuth is defined to be the angle between the X axis and the projection of the location vector
//! onto the X-Y plane.
//! - Elevation is defined to be the angle of the location vector above the X-Y plane.
//! @param aLocation  The location whose azimuth and elevation are desired.
//! @param aAzimuth   [output] The azimuth of the location [-pi, pi].
//! @param aElevation [output] The elevation of the location [-pi/2, pi/2].
//! @note The standard convention of a local frame is assumed (X forward, Y right, Z down).
// static
void UtEntity::ComputeAzimuthAndElevation(const double aLocation[3], double& aAzimuth, double& aElevation)
{
   double x  = aLocation[0];
   double y  = aLocation[1];
   double z  = aLocation[2];
   aAzimuth  = atan2(y, x);
   double xy = sqrt((x * x) + (y * y));
   if (xy != 0.0)
   {
      aElevation = -atan2(z, xy);
   }
   else
   {
      aElevation = UtMath::cPI_OVER_2;
      if (z > 0.0)
      {
         aElevation = -UtMath::cPI_OVER_2;
      }
   }
}

// =================================================================================================
//! Compute a rotational transformation matrix.
//!
//! Given the Euler angles psi, theta and phi defined according
//! to the DIS convention, compute the rotational transformation.  The DIS
//! convention is:
//!
//! \li First    rotate about Z by Psi   giving X'Y'Z'
//! \li Next     rotate about Y by Theta giving X'Y'Z'
//! \li Finally, rotate about X by Phi   giving the entity coordinate system.
//!
//! If the angles are the heading, pitch and roll in the NED frame then this
//! will generate the NED->ECS transform.
//!
//! If the angles are the DIS psi, theta and phi then this will generate the
//! WCS->ECS transform.
//!
//! The input values must be in radians in the following ranges:
//! \li -PI \<= Psi \<= PI
//! \li -PI/2 \<= Theta \<= PI/2
//! \li -PI/2 \<= Phi \<= PI/2
// static
void UtEntity::ComputeRotationalTransform(double aPsi,   // [-pi, pi]
                                          double aTheta, // [-pi/2, pi/2]
                                          double aPhi,   // [-pi/2, pi/2]
                                          double aTransform[3][3])
{
   // This matrix is taken directly from:
   //
   // IEEE Standard for Distributed Interactive Simulation - Application Protocols
   // IEEE Std 1278.1-1995, Annex B, section B.1.6.1.4

   double sinPsi   = sin(aPsi);
   double cosPsi   = cos(aPsi);
   double sinTheta = sin(aTheta);
   double cosTheta = cos(aTheta);
   double sinPhi   = sin(aPhi);
   double cosPhi   = cos(aPhi);
   double temp;

   aTransform[0][0] = cosTheta * cosPsi;
   aTransform[0][1] = cosTheta * sinPsi;
   aTransform[0][2] = -sinTheta;

   temp             = sinPhi * sinTheta;
   aTransform[1][0] = temp * cosPsi - cosPhi * sinPsi;
   aTransform[1][1] = temp * sinPsi + cosPhi * cosPsi;
   aTransform[1][2] = sinPhi * cosTheta;

   temp             = cosPhi * sinTheta;
   aTransform[2][0] = temp * cosPsi + sinPhi * sinPsi;
   aTransform[2][1] = temp * sinPsi - sinPhi * cosPsi;
   aTransform[2][2] = cosPhi * cosTheta;
}

// =================================================================================================
//! Compute the coordinated transformation matrix from WCS to another local frame.
//! @param aWCSToLocalTransform [output] The transformation matrix from WCS to the desired local frame.
//! @param aECSToLocalTransform [input]  The transformation matrix from the desired local frame to ECS.
//! This is typically used to compute a WCS->PCS transformation for UtEntityPart, but can be used by
//! anyone who needs a local frame.
void UtEntity::ComputeWCSToLocalTransform(double aWCSToLocalTransform[3][3], const double aECSToLocalTransform[3][3]) const
{
   // [ WCS->Local ] = [ ECS->Local ] x [ WCS->ECS ]

   if (!mWCSToECSTransformIsValid)
   {
      UpdateWCSToECSTransform();
   }
   UtMat3d::Multiply(aWCSToLocalTransform, aECSToLocalTransform, mWCSToECSTransform);
}

// =================================================================================================
//! Extract the rotation angles from a rotational transformation matrix.
//!
//! Extract the Euler angles from a rotational transformation matrix that
//! was computed by ComputeRotationalTransform(),
//!
//! If the matrix is the NED->ECS transformation then the return angles
//! will be the heading, pitch and roll angles of the body relative to
//! the local NED frame.
//!
//! If the matrix is the WCS->ECS transformation then the returned angles
//! will be the angles needed for the orientation in the DIS PDU.
//!
//! The output values will be in radians in with the following ranges:
//! \li -PI \<= Psi \<= PI
//! \li -PI/2 \<= Theta \<= PI/2
//! \li -PI \<= Phi \<= PI
// static
void UtEntity::ExtractEulerAngles(const double aTransform[3][3],
                                  double&      aPsi,   // [-pi, pi]
                                  double&      aTheta, // [-pi/2, pi/2]
                                  double&      aPhi)        // [-pi, pi]
{
   // This algorithm is taken directly from:
   //
   // IEEE Standard for Distributed Interactive Simulation - Application Protocols
   // IEEE Std 1278.1-1995, Annex B, section B.1.6.1.5
   //
   // if aTransform[0][2] is exactly 1 or -1, the resulting value of theta will be
   // be -pi/2 or pi/2 respectively. Taking the cos(theta) would then result in a
   // value of zero and eventually a divide by zero. A more insidious problem arises,
   // however. Taking cos(asin(pi/2)) mathematically equals zero, but both GNU g++
   // Microsoft Visual C++ both return something that is not zero (about 1E-17).
   // When this is subsequently used in the recovery of the angles, one can get some
   // pretty violent fluctuations.
   //
   // If theta is +/- pi/2, we really can't solve for anything because psi and phi
   // cannot be uniquely determined. Take the matrix definition in B.1.6.1.4, and
   // set cos(theta)=0. Look at what's left, and you'll see there is no what to solve
   // for phi and psi unless you assume one of them. In this case we'll assume phi
   // (roll) is zero and solve for psi (yaw).

   // (JAJ) A tolerance was added because of numerical issues where compound transforms
   //       were involved. Recovering the NED angles during vertical rocket boosts was
   //       resulting in wild fluctuations.
   static const double cALMOST_ONE = 0.99999999999999;
   if ((aTransform[0][2] >= cALMOST_ONE) || (aTransform[0][2] <= -cALMOST_ONE))
   {
      aPsi = acos(std::min(std::max(aTransform[1][1], -1.0), 1.0)); // [0, PI]
      if (aTransform[1][0] > 0.0)
      {
         aPsi = -aPsi;
      }
      aTheta = UtMath::cPI_OVER_2; // theta has the opposite sign of aTransform[0][2]
      if (aTransform[0][2] >= 1.0)
      {
         aTheta = -UtMath::cPI_OVER_2;
      }
      aPhi = 0.0;
   }
   else
   {
      aTheta          = asin(-aTransform[0][2]);
      double cosTheta = cos(aTheta);
      aPsi            = acos(std::min(std::max(aTransform[0][0] / cosTheta, -1.0), 1.0));
      if (aTransform[0][1] < 0.0)
      {
         aPsi = -aPsi;
      }
      aPhi = acos(std::min(std::max(aTransform[2][2] / cosTheta, -1.0), 1.0));
      if (aTransform[1][2] < 0.0)
      {
         aPhi = -aPhi;
      }
   }
}

// =================================================================================================
void UtEntity::GetNEDToECSTransform(double aNEDToECSTransform[3][3]) const
{
   // Ensure the ECS->NED transformation is up-to-date
   if (!mNEDToECSTransformIsValid)
   {
      UpdateNEDToECSTransform();
   }
   UtMat3d::Set(aNEDToECSTransform, mNEDToECSTransform);
}

// =================================================================================================
void UtEntity::GetWCSToNEDTransform(double aWCSToNEDTransform[3][3]) const
{
   // Ensure the WCS->NED transformation is up-to-date
   if (!mWCSToNEDTransformIsValid)
   {
      UpdateWCSToNEDTransform();
   }
   UtMat3d::Set(aWCSToNEDTransform, mWCSToNEDTransform);
}

// =================================================================================================
void UtEntity::GetWCSToECSTransform(double aWCSToECSTransform[3][3]) const
{
   // Ensure the WCS->ECS transformation is up-to-date
   if (!mWCSToECSTransformIsValid)
   {
      UpdateWCSToECSTransform();
   }
   UtMat3d::Set(aWCSToECSTransform, mWCSToECSTransform);
}

// =================================================================================================
void UtEntity::GetECIToWCSTransform(double aECIToWCSTransform[3][3]) const
{
   GetECI_Conversion().GetECI_ToWCS_Transform(aECIToWCSTransform);
}

// =================================================================================================
// PRIVATE ROUTINES
// =================================================================================================

// =================================================================================================
// private
void UtEntity::WasMoved()
{
   mWCSToNEDTransformIsValid = false; // Obviously True

   // The next two flags are tougher to figure out... if NED Orientation is now valid,
   // and is assumed held constant during the translation movement, then the NED-To-ECS
   // transform might actually stay valid after the move.  But if the WCS orientation is
   // the "native" frame at this time, and its orientation is held constant, then the
   // WCS-To-ECS transform may stay valid.  Since we cannot be sure of either assumption,
   // conservatism dictates that we invalidate both transforms, and just re-calculate
   // them if needed.
   mNEDToECSTransformIsValid = false;
   mWCSToECSTransformIsValid = false;
   mECIToECSTransformIsValid = false;
}

// =================================================================================================
// private
void UtEntity::WasRotated()
{
   mNEDToECSTransformIsValid = false;
   mWCSToECSTransformIsValid = false;
   mECIToECSTransformIsValid = false;
}

// =================================================================================================
//! Get the Inertial conversion object.  You MUST call this method, instead of directly accessing the object.
UtECI_Conversion& UtEntity::GetECI_Conversion() const
{
   if (mInertialConversionPtr == nullptr)
   {
      mInertialConversionPtr = ut::make_unique<UtECI_Conversion>(mInertialRefEpoch, *mCentralPointPtr);
      mInertialConversionPtr->SetTime(mUpdateTime);
      mInertialUpdateTime = mUpdateTime;

      if (mWCSLocationIsValid || mLLALocationIsValid)
      {
         double locWCS[3];
         GetLocationWCS(locWCS);
         mInertialConversionPtr->SetLocationWCS(locWCS);

         // Position must be valid in order to convert to Inertial velocity
         if (mWCSVelocityIsValid || mNEDVelocityIsValid)
         {
            double velWCS[3];
            GetVelocityWCS(velWCS);
            mInertialConversionPtr->SetVelocityWCS(velWCS);

            // Position and velocity must valid in order to convert to Inertial acceleration.
            if (mWCSAccelerationIsValid || mNEDAccelerationIsValid)
            {
               double accWCS[3];
               GetAccelerationWCS(accWCS);
               mInertialConversionPtr->SetAccelerationWCS(accWCS);
            }
         }
      }
   }
   else if (mInertialUpdateTime != mUpdateTime)
   {
      if (mNutationUpdateInterval != mInertialConversionPtr->GetNutationUpdateInterval())
      {
         mInertialConversionPtr->SetNutationUpdateInterval(mNutationUpdateInterval);
      }

      mInertialConversionPtr->SetTime(mUpdateTime);
      mInertialUpdateTime = mUpdateTime;
   }

   return *mInertialConversionPtr;
}

// =================================================================================================
//! Compute the LLA position that corresponds to a WCS position.
// private
void UtEntity::UpdateLLALocation() const
{
   assert(mWCSLocationIsValid || mInertialLocationIsValid);
   if (!mWCSLocationIsValid)
   {
      // Inertial location should be valid.
      GetECI_Conversion().GetLocationWCS(mLocationWCS);
      mWCSLocationIsValid = true;
   }
   ut::EllipsoidalCentralBody::ConvertECEFToLLA(mLocationWCS, GetCentralBody().GetEllipsoid(), mLat, mLon, mAlt);

   mLLALocationIsValid = true;
}

// =================================================================================================
// private
void UtEntity::UpdateNEDToECSTransform() const
{
   // Ensure the WCS->NED transformation is up-to-date
   if (!mWCSToNEDTransformIsValid)
   {
      UpdateWCSToNEDTransform();
   }
   if (!mWCSOrientationIsValid)
   {
      GetOrientationWCS(mPsiWCS, mThetaWCS, mPhiWCS);
   }
   // Compute the WCS->ECS transformation
   ComputeRotationalTransform(mPsiWCS, mThetaWCS, mPhiWCS, mWCSToECSTransform);

   //                            -1               T
   // [ NED->WCS ] = [ WCS->NED ]   = [ WCS->NED ]   (The inverse is the transpose)

   double nedToWCSTransform[3][3];
   UtMat3d::Transpose(nedToWCSTransform, mWCSToNEDTransform);

   // [ NED->ECS ] x [ WCS->NED ] = [ WCS->ECS ]
   //                                    -1
   // Multiply both sides by [ WCS->NED ]
   //                                           -1                              -1
   // [ NED->ECS ] x [ WCS->NED ] x [ WCS->NED ]   = [ WCS->ECS ] x [ WCS->NED ]
   //                                                                           -1
   // [ NED->ECS ]                                 = [ WCS->ECS ] x [ WCS->NED ]
   //
   //                                              = [ WCS->ECS ] x [ NED->WCS ]

   UtMat3d::Multiply(mNEDToECSTransform, mWCSToECSTransform, nedToWCSTransform);
}

// =================================================================================================
//! If LLA or inertial position is valid, compute the corresponding WCS position.
// private
void UtEntity::UpdateWCSLocation() const
{
   // There are only three ways to specify a global location: LLA, WCS, or Inertial.
   // Since we want to update WCS here, then it is assumed that LLA or Inertial has to be valid...
   assert(mLLALocationIsValid || mInertialLocationIsValid);

   // Preferentially covert from an inertial location as it is more accurate than from LLA.
   if (mInertialLocationIsValid)
   {
      GetECI_Conversion().GetLocationWCS(mLocationWCS);
   }
   else // LLA location is valid
   {
      // We do not need to update the wcs->ned transform here as it is checked in other places.
      ut::EllipsoidalCentralBody::ConvertLLAToECEF(mLat, mLon, mAlt, GetCentralBody().GetEllipsoid(), mLocationWCS);
   }

   mWCSLocationIsValid = true;
}

// =================================================================================================
//! Compute the WCS->ECS transform.
// private
void UtEntity::UpdateWCSToECSTransform() const
{
   if (!mWCSLocationIsValid)
   {
      UpdateWCSLocation();
   }

   if (mWCSOrientationIsValid)
   {
      // Compute the WCS->ECS transformation directly
      ComputeRotationalTransform(mPsiWCS, mThetaWCS, mPhiWCS, mWCSToECSTransform);
   }
   else if (mInertialOrientationIsValid)
   {
      double WCS_ToECI_Transform[3][3];
      GetECI_Conversion().GetWCS_ToECI_Transform(WCS_ToECI_Transform);

      if (!mECIToECSTransformIsValid)
      {
         // Compute the Inertial->ECS transformation directly
         ComputeRotationalTransform(mPsiECI, mThetaECI, mPhiECI, mECIToECSTransform);
         mECIToECSTransformIsValid = true;
      }

      UtMat3d::Multiply(mWCSToECSTransform, mECIToECSTransform, WCS_ToECI_Transform);
   }
   else
   {
      assert(mNEDOrientationIsValid);

      // Make sure the WCS->NED transformation is up-to-date.

      if (!mWCSToNEDTransformIsValid)
      {
         UpdateWCSToNEDTransform();
      }

      // Make sure the NED->ECS transformation is up-to-date.

      if (!mNEDToECSTransformIsValid)
      {
         ComputeRotationalTransform(mHeadingNED, mPitchNED, mRollNED, mNEDToECSTransform);
         mNEDToECSTransformIsValid = true;
      }

      // Form the composite WCS->ECS transformation matrix.

      // [ WCS->ECS ] = [ NED->ECS ] x [ WCS->NED ]

      UtMat3d::Multiply(mWCSToECSTransform, mNEDToECSTransform, mWCSToNEDTransform);
   }

   mWCSToECSTransformIsValid = true;
}

// =================================================================================================
//! Compute the WCS->ECS transform.
// private
void UtEntity::UpdateECIToECSTransform() const
{
   if (!mInertialLocationIsValid)
   {
      UpdateInertialLocation();
   }

   if (mInertialOrientationIsValid)
   {
      // Compute the Inertial->ECS transformation directly
      ComputeRotationalTransform(mPsiECI, mThetaECI, mPhiECI, mECIToECSTransform);
   }
   else if (mWCSOrientationIsValid)
   {
      double ECI_ToWCS_Transform[3][3];
      GetECI_Conversion().GetECI_ToWCS_Transform(ECI_ToWCS_Transform);

      if (!mWCSToECSTransformIsValid)
      {
         // Compute the WCS->ECS transformation directly
         ComputeRotationalTransform(mPsiWCS, mThetaWCS, mPhiWCS, mWCSToECSTransform);
         mWCSToECSTransformIsValid = true;
      }

      UtMat3d::Multiply(mECIToECSTransform, mWCSToECSTransform, ECI_ToWCS_Transform);
   }
   else
   {
      assert(mNEDOrientationIsValid);

      // Make sure the WCS->NED transformation is up-to-date.

      if (!mWCSToNEDTransformIsValid)
      {
         UpdateWCSToNEDTransform();
      }

      // Make sure the NED->ECS transformation is up-to-date.

      if (!mNEDToECSTransformIsValid)
      {
         ComputeRotationalTransform(mHeadingNED, mPitchNED, mRollNED, mNEDToECSTransform);
         mNEDToECSTransformIsValid = true;
      }

      // Form the composite ECI->ECS transformation matrix:
      // [ ECI->ECS ] = [ NED->ECS ] x [ WCS->NED ] x [ECI->WCS]

      double ECI_ToWCS_Transform[3][3];
      GetECI_Conversion().GetECI_ToWCS_Transform(ECI_ToWCS_Transform);
      double ECI_ToNED_Transform[3][3];

      // [ ECI->NED ] = [ WCS->NED ] x [ECI->WCS]
      UtMat3d::Multiply(ECI_ToNED_Transform, mWCSToNEDTransform, ECI_ToWCS_Transform);

      // [ ECI->ECS ] = [ NED->ECS ] x [ ECI-NED]
      UtMat3d::Multiply(mECIToECSTransform, mNEDToECSTransform, ECI_ToNED_Transform);
   }

   mECIToECSTransformIsValid = true;
}

// =================================================================================================
//! Compute the WCS->NED transform
// private
void UtEntity::UpdateWCSToNEDTransform() const
{
   // LLA is required to compute the WCS->NED transformation.  If the last position
   // position update was via SetLocationWCS or SetSpatialECI (which invalidates the
   // LLA position), we must first get the updated LLA.

   if (!mLLALocationIsValid) // either WCS or Inertial is valid
   {
      UpdateLLALocation(); // calls UpdateWCSLocation() if necessary
   }

   // Use the ComputeNEDTransform procedure that does not compute WCS.
   ut::EllipsoidalCentralBody::ComputeNEDTransform(mLat, mLon, mAlt, mWCSToNEDTransform);
   mWCSToNEDTransformIsValid = true;
}

// =================================================================================================
//! Update the Inertial location of an entity, assuming either WCS or LLA is correct and we have the correct
//! time since the start of the simulation.
// private
void UtEntity::UpdateInertialLocation(double aSimTime) const
{
   SetTimeP(aSimTime);
   UpdateInertialLocation();
}

// =================================================================================================
//! Update the Inertial location of an entity, assuming either WCS or LLA is correct and we already have the
//! correct time.
// private
void UtEntity::UpdateInertialLocation() const
{
   assert(mWCSLocationIsValid || mLLALocationIsValid);
   if (!mWCSLocationIsValid)
   {
      UpdateWCSLocation();
   }
   GetECI_Conversion().SetLocationWCS(mLocationWCS);
   mInertialLocationIsValid = true;
}

// =================================================================================================
//! Set the reference epoch associated with the creation time of the entity.
//! @param The epoch corresponding with the creation time of the entity.
//! @note This method must be called before any Inertial coordinate or coordinate conversion methods are called.
void UtEntity::SetECI_ReferenceEpoch(const UtCalendar& aEpoch)
{
   mInertialRefEpoch = aEpoch;
   mCentralPointPtr->Initialize(aEpoch);
}

// =================================================================================================
//! Set the polar offset angles associated with small changes in the earth's rotation axis with
//! respect to the crust.  If set, these are used in WCS<->Inertial conversions.
//! @param aOffsetAngleX The x polar offset angle.
//! @param aOffsetAngleY The y polar offset angle.
void UtEntity::SetPolarOffsetAngles(double aOffsetAngleX, double aOffsetAngleY) const
{
   if (mCentralPointPtr->IsCentralBody())
   {
      mCentralPointPtr->GetAsCentralBody().SetPolarOffsetAngles(aOffsetAngleX, aOffsetAngleY);
   }
}

// =================================================================================================
//! Set the interval at which nutation updates are made in WCS<->Inertial calculations.
//! @param aNutationUpdateInterval The interval at which nutation updates are performed.
//! @note The higher this value, the more error will be present in WCS<->Inertial conversions.
void UtEntity::SetNutationUpdateInterval(double aNutationUpdateInterval)
{
   mNutationUpdateInterval = aNutationUpdateInterval;
}

// =================================================================================================
//! Deprecated; see SetNutationUpdateInterval.
void UtEntity::SetPrecessionNutationUpdateInterval(double aNutationUpdateInterval)
{
   SetNutationUpdateInterval(aNutationUpdateInterval);
}

// =================================================================================================
//! Compute the relative bearing of a target point from an entity.
//! @param aTargetLocWCS The WCS location of the target point.
//! @returns The relative bearing in the range [-pi, pi].
double UtEntity::RelativeBearing(const double aTargetLocWCS[3]) const
{
   // Get the true bearing.
   double trueBearing = TrueBearing(aTargetLocWCS);

   // Adjust by the heading of the entity.
   double heading;
   double pitch;
   double roll;
   GetOrientationNED(heading, pitch, roll);

   double bearing = trueBearing - heading;
   return UtMath::NormalizeAngleMinusPi_Pi(bearing);
}

double UtEntity::RelativeBearing(const ut::coords::WCS& aTargetLocWCS) const
{
   return RelativeBearing(aTargetLocWCS.GetData());
}

double UtEntity::RelativeBearing(const UtVec3d& aTargetLocWCS) const
{
   return RelativeBearing(aTargetLocWCS.GetData());
}

// =================================================================================================
//! Compute the true bearing of a target point from an entity.
//! @param aTargetLocWCS The WCS location of the target point.
//! @returns The true bearing in the range [-pi, pi].
double UtEntity::TrueBearing(const double aTargetLocWCS[3]) const
{
   double targetLocNED[3];
   ConvertWCSToNED(aTargetLocWCS, targetLocNED);
   return atan2(targetLocNED[1], targetLocNED[0]);
}

double UtEntity::TrueBearing(const ut::coords::WCS& aTargetLocWCS) const
{
   return TrueBearing(aTargetLocWCS.GetData());
}

double UtEntity::TrueBearing(const UtVec3d& aTargetLocWCS) const
{
   return TrueBearing(aTargetLocWCS.GetData());
}
