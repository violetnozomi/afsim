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

#include "UtEntityPart.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>

#include "UtEntity.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"

// =================================================================================================
//! Constructor.
//! @param aOwningEntityPtr The pointer to the entity to which this part is attached.
//! If 0 then SetOwningEntity() MUST be called prior to calling any of the other methods.
UtEntityPart::UtEntityPart(UtEntity* aOwningEntityPtr)
   : mOwningEntityPtr(aOwningEntityPtr)
   , mYaw(0.0)
   , mPitch(0.0)
   , mRoll(0.0)
   , mLocationWCSIsValid(false)
   , mECSToPCSTransformIsValid(false)
   , mWCSToPCSTransformIsValid(false)
{
   UtVec3d::Set(mLocationECS, 0.0);
   UtVec3d::Set(mLocationWCS, 0.0);
   UtMat3d::Identity(mECSToPCSTransform);
   UtMat3d::Identity(mWCSToPCSTransform);
}

// =================================================================================================
//! Copy constructor.
//! The copy constructor copies everything but the owning entity pointer.
//! SetOwningEntity() \b MUST be called to define the owning entity prior to
//! calling any other methods.
UtEntityPart::UtEntityPart(const UtEntityPart& aSrc)
   : mOwningEntityPtr(nullptr)
   , mYaw(aSrc.mYaw)
   , mPitch(aSrc.mPitch)
   , mRoll(aSrc.mRoll)
   , mLocationWCSIsValid(false)
   , mECSToPCSTransformIsValid(aSrc.mECSToPCSTransformIsValid)
   , mWCSToPCSTransformIsValid(false)
{
   UtVec3d::Set(mLocationECS, aSrc.mLocationECS);
   UtVec3d::Set(mLocationWCS, aSrc.mLocationWCS);
   UtMat3d::Set(mECSToPCSTransform, aSrc.mECSToPCSTransform);
   UtMat3d::Set(mWCSToPCSTransform, aSrc.mWCSToPCSTransform);
}

UtEntityPart& UtEntityPart::operator=(const UtEntityPart& aRhs)
{
   auto tmp = aRhs;
   std::swap(tmp, *this);
   return *this;
}


// =================================================================================================
//! Get the origin of the part coordinate system with respect to the host entity.
//! @return the location of the part in the ECS frame of the host entity
ut::coords::ECS UtEntityPart::GetLocation() const
{
   return mLocationECS;
}

// =================================================================================================
//! Get the origin of the part coordinate system with respect to the host entity.
//! @return the location of the part in the ECS frame of the host entity
ut::coords::ECS UtEntityPart::GetLocationECS() const
{
   return GetLocation();
}

// =================================================================================================
//! Get the origin of the part coordinate system with respect to the host entity.
//! @param aLocation is the location of the part in the ECS frame of the host entity
void UtEntityPart::GetLocation(double aLocation[3]) const
{
   UtVec3d::Set(aLocation, mLocationECS);
}

// =================================================================================================
//! Set the location of the part coordinate system with respect to the host entity.
void UtEntityPart::SetLocation(const ut::coords::ECS& aLocationECS)
{
   SetLocation(aLocationECS.GetData());
}

// =================================================================================================
//! Set the location of the part coordinate system with respect to the host entity.
void UtEntityPart::SetLocationECS(const ut::coords::ECS& aLocationECS)
{
   SetLocation(aLocationECS);
}

// =================================================================================================
//! Set the location of the part coordinate system with respect to the host entity.
void UtEntityPart::SetLocation(const double aLocation[3])
{
   UtVec3d::Set(mLocationECS, aLocation);
   mLocationWCSIsValid       = false;
   mWCSToPCSTransformIsValid = false;
}

// =================================================================================================
//! Get the orientation of the part coordinate system with respect to the host entity.
ut::coords::EulerAngles UtEntityPart::GetOrientationECS() const
{
   return GetOrientation();
}

// =================================================================================================
//! Get the orientation of the part coordinate system with respect to the host entity.
ut::coords::EulerAngles UtEntityPart::GetOrientation() const
{
   ut::coords::EulerAngles out;
   GetOrientation(out.mPsi, out.mTheta, out.mPhi);
   return out;
}

// =================================================================================================
//! Get the orientation of the part coordinate system with respect to the host entity.
UtQuaternion UtEntityPart::GetOrientationECSq() const
{
   return UtQuaternion(GetOrientation());
}

// =================================================================================================
//! Get the orientation of the part coordinate system with respect to the host entity.
void UtEntityPart::GetOrientation(double& aYaw, double& aPitch, double& aRoll) const
{
   aYaw   = mYaw;
   aPitch = mPitch;
   aRoll  = mRoll;
}

// =================================================================================================
//! Set the orientation of the part coordinate system with respect to the host entity.
void UtEntityPart::SetOrientation(const ut::coords::EulerAngles& aOrientation)
{
   SetOrientation(aOrientation.mPsi, aOrientation.mTheta, aOrientation.mPhi);
}

// =================================================================================================
//! Set the orientation of the part coordinate system with respect to the host entity.
void UtEntityPart::SetOrientation(const UtQuaternion& aOrientation)
{
   SetOrientation(aOrientation.Get());
}

// =================================================================================================
//! Set the orientation of the part coordinate system with respect to the host entity.
void UtEntityPart::SetOrientationECS(const ut::coords::EulerAngles& aOrientation)
{
   SetOrientation(aOrientation);
}

// =================================================================================================
//! Set the orientation of the part coordinate system with respect to the host entity.
void UtEntityPart::SetOrientationECS(const UtQuaternion& aOrientation)
{
   SetOrientation(aOrientation);
}

// =================================================================================================
//! Set the orientation of the part coordinate system with respect to the host entity.
void UtEntityPart::SetOrientation(double aYaw, double aPitch, double aRoll)
{
   mYaw                      = aYaw;
   mPitch                    = aPitch;
   mRoll                     = aRoll;
   mECSToPCSTransformIsValid = false;
   mWCSToPCSTransformIsValid = false;
}

// =================================================================================================
//! Get the orientation of the part coordinate system with respect to the world coordinate system.
ut::coords::EulerAngles UtEntityPart::GetOrientationWCS() const
{
   ut::coords::EulerAngles angles;
   GetOrientationWCS(angles.mPsi, angles.mTheta, angles.mPhi);
   return angles;
}

// =================================================================================================
//! Get the orientation of the part coordinate system with respect to the world coordinate system.
void UtEntityPart::GetOrientationWCS(double& aPsiWCS, double& aThetaWCS, double& aPhiWCS) const
{
   // Ensure that the WCS->PCS transformation is current
   if (!mWCSToPCSTransformIsValid)
   {
      UpdateWCSToPCSTransform();
   }
   UtEntity::ExtractEulerAngles(mWCSToPCSTransform, aPsiWCS, aThetaWCS, aPhiWCS);
}

// =================================================================================================
//! Get the orientation of the part coordinate system with respect to the world coordinate system.
UtQuaternion UtEntityPart::GetOrientationWCSq() const
{
   return UtQuaternion(GetOrientationWCS());
}

// =================================================================================================
//! Get the orientation of the part coordinate system with respect to the world coordinate system.
ut::coords::EulerAngles UtEntityPart::GetOrientationECI() const
{
   ut::coords::EulerAngles angles;
   GetOrientationECI(angles.mPsi, angles.mTheta, angles.mPhi);
   return angles;
}

// =================================================================================================
//! Get the orientation of the part coordinate system with respect to the world coordinate system.
void UtEntityPart::GetOrientationECI(double& aPsiECI, double& aThetaECI, double& aPhiECI) const
{
   double ECI_ToWCS_Transform[3][3];
   mOwningEntityPtr->GetECIToWCSTransform(ECI_ToWCS_Transform);
   double ECI_ToPCS_Transform[3][3];

   // Ensure that the WCS->PCS transformation is current
   if (!mWCSToPCSTransformIsValid)
   {
      UpdateWCSToPCSTransform();
   }

   // Compute the ECI->PCS transform.
   // [ ECI->PCS ] = [ WCS->PCS ] x [ ECI->WCS ]
   UtMat3d::Multiply(ECI_ToPCS_Transform, mWCSToPCSTransform, ECI_ToWCS_Transform);
   UtEntity::ExtractEulerAngles(ECI_ToPCS_Transform, aPsiECI, aThetaECI, aPhiECI);
}

// =================================================================================================
//! Get the orientation of the part coordinate system with respect to the world coordinate system.
UtQuaternion UtEntityPart::GetOrientationECIq() const
{
   return UtQuaternion(GetOrientationECI());
}

// =================================================================================================
//! Apply an incremental rotation to the part coordinate system.
void UtEntityPart::Rotate(const ut::coords::EulerAngles& aRotation)
{
   Rotate(aRotation.mPsi, aRotation.mTheta, aRotation.mPhi);
}

// =================================================================================================
//! Apply an incremental rotation to the part coordinate system.
void UtEntityPart::Rotate(const UtQuaternion& aRotation)
{
   SetOrientationECS(aRotation * GetOrientationECSq());
}

// =================================================================================================
//! Apply an incremental rotation to the part coordinate system.
void UtEntityPart::Rotate(double aYaw, double aPitch, double aRoll)
{
   // Ensure the ECS->PCS transform is current
   if (!mECSToPCSTransformIsValid)
   {
      UpdateECSToPCSTransform();
   }

   // Compute the incremental rotation.
   double rotationTransform[3][3];
   UtEntity::ComputeRotationalTransform(aYaw, aPitch, aRoll, rotationTransform);

   // Update the current ECS->PCS transform;
   // [ ECS->PCS ] = [ rotation ] x [ ECS->PCS ]

   double tempECSToPCSTransform[3][3];
   UtMat3d::Multiply(tempECSToPCSTransform, rotationTransform, mECSToPCSTransform);
   UtMat3d::Set(mECSToPCSTransform, tempECSToPCSTransform);

   // Also recompute the WCS->PCS transform
   mWCSToPCSTransformIsValid = false;
   UpdateWCSToPCSTransform();
}

ut::coords::Aspect UtEntityPart::ComputeAspect(const ut::coords::WCS& aRelativeLocationWCS)
{
   ut::coords::Aspect out;
   ComputeAspect(aRelativeLocationWCS.GetData(), out.mAz, out.mEl);
   return out;
}

// =================================================================================================
//! Compute the aspect (azimuth and elevation) of a location with respect to this part.
//!
//! @param aRelativeLocationWCS [input] The relative WCS location of some object (relative
//!                             to this part.
//! @param aRelativeAzimuth     [output] The azimuth (relative to this part) of the
//!                             input location.
//! @param aRelativeElevation   [output] The elevation (relative to this part) of the
//!                             input location.
//!
//! @note The input relative location should probably been computed with
//! UtEntityPart::GetRelativeLocationWCS.
void UtEntityPart::ComputeAspect(const double aRelativeLocationWCS[3], double& aRelativeAzimuth, double& aRelativeElevation)
{
   // Transform the relative WCS location to a PCS location and compute the resulting azimuth and elevation.
   double locationPCS[3];
   ConvertWCSVectorToPCS(locationPCS, aRelativeLocationWCS);
   UtEntity::ComputeAzimuthAndElevation(locationPCS, aRelativeAzimuth, aRelativeElevation);
}

// =================================================================================================
//! Get the absolute spherical location of this part.
void UtEntityPart::GetLocationLLA(double& aLat, double& aLon, double& aAlt) const
{
   // Ensure the WCS location is valid (we don't need to worry about rotation)
   if (!mLocationWCSIsValid)
   {
      UpdateLocationWCS();
   }
   UtEntity::ConvertWCSToLLA(mLocationWCS, mOwningEntityPtr->GetCentralBody().GetEllipsoid(), aLat, aLon, aAlt);
}

ut::coords::LLA UtEntityPart::GetLocationLLA() const
{
   ut::coords::LLA lla;
   GetLocationLLA(lla.mLat, lla.mLon, lla.mAlt);
   return lla;
}

// =================================================================================================
//! Get the absolute ECI location of this part.
//! @param aLocationECI is the ECI location of this part.
void UtEntityPart::GetLocationECI(double aLocationECI[3]) const
{
   // Ensure the WCS location is valid
   if (!mLocationWCSIsValid)
   {
      UpdateLocationWCS();
   }
   GetOwningEntity()->ConvertWCSToECI(mLocationWCS, aLocationECI);
}

// =================================================================================================
//! Get the absolute ECI location of this part.
//! @return the ECI location of this part.
ut::coords::ECI UtEntityPart::GetLocationECI() const
{
   UtVec3d loc;
   GetLocationECI(loc.GetData());
   return ut::coords::ECI(loc);
}

// =================================================================================================
//! Get the absolute WCS location of this part.
//! @param aLocationWCS is the WCS location of this part.
void UtEntityPart::GetLocationWCS(double aLocationWCS[3]) const
{
   // Ensure the WCS location is valid (we don't need to worry about rotation)
   if (!mLocationWCSIsValid)
   {
      UpdateLocationWCS();
   }
   UtVec3d::Set(aLocationWCS, mLocationWCS);
}

// =================================================================================================
//! Get the absolute WCS location of this part.
//! @return the WCS location of this part.
ut::coords::WCS UtEntityPart::GetLocationWCS() const
{
   UtVec3d loc;
   GetLocationWCS(loc.GetData());
   return ut::coords::WCS(loc);
}

// =================================================================================================
//! Given the relative WCS location of another object relative to this
//! part, compute the absolute WCS location of the other object.
void UtEntityPart::GetLocationWCS(const double aRelativeLocationWCS[3], double aLocationWCS[3])
{
   // Ensure the WCS location is valid (we don't need to worry about rotation)
   if (!mLocationWCSIsValid)
   {
      UpdateLocationWCS();
   }
   UtVec3d::Add(aLocationWCS, mLocationWCS, aRelativeLocationWCS);
}

// =================================================================================================
//! Compute the relative WCS location of another object given its relative spherical coordinates.
ut::coords::WCS UtEntityPart::GetRelativeLocationWCS(const ut::coords::Aspect& aAspect, double aDistance)
{
   ut::coords::WCS out;
   GetRelativeLocationWCS(aAspect.mAz, aAspect.mEl, aDistance, out.GetData());
   return out;
}

// =================================================================================================
//! Compute the relative WCS location of another object given its relative spherical coordinates.
//! @param aRelativeAzimuth     [input]  The azimuth of the other object with respect to this object (radians).
//! @param aRelativeElevation   [input]  The elevation of the other object with respect to this object (radians).
//! @param aDistance            [output] The distance from this object to the other object (meters).
//! @param aRelativeLocationWCS [output] The relative WCS location of the other object.
void UtEntityPart::GetRelativeLocationWCS(double aRelativeAzimuth,
                                          double aRelativeElevation,
                                          double aDistance,
                                          double aRelativeLocationWCS[3])
{
   // Ensure that the WCS->PCS transformation is current
   if (!mWCSToPCSTransformIsValid)
   {
      UpdateWCSToPCSTransform();
   }

   // Compute the XYZ components in the PCS
   double partZ  = -aDistance * sin(aRelativeElevation);
   double partXY = sqrt((aDistance * aDistance) - (partZ * partZ));
   double partX  = partXY * cos(aRelativeAzimuth);
   double partY  = partXY * sin(aRelativeAzimuth);

   // Convert the PCS vector into a relative WCS vector.
   double relativeLocationPCS[3] = {partX, partY, partZ};
   UtMat3d::InverseTransform(aRelativeLocationWCS, mWCSToPCSTransform, relativeLocationPCS);
}

// =================================================================================================
//! Compute the relative location of another WCS location.
//! The magnitude of this vector is the distance to the location.
ut::coords::WCS UtEntityPart::GetRelativeLocationWCS(const ut::coords::WCS& aOtherLocationWCS) const
{
   ut::coords::WCS out;
   GetRelativeLocationWCS(aOtherLocationWCS.GetData(), out.GetData());
   return out;
}

// =================================================================================================
//! Compute the relative location of another WCS location.
//! The magnitude of this vector is the distance to the location.
void UtEntityPart::GetRelativeLocationWCS(const double aOtherLocationWCS[3], double aRelativeLocationWCS[3]) const
{
   // Ensure the WCS location is valid (we don't need to worry about rotation)
   if (!mLocationWCSIsValid)
   {
      UpdateLocationWCS();
   }
   // Compute the WCS vector from the origin of the PCS to the other location
   UtVec3d::Subtract(aRelativeLocationWCS, aOtherLocationWCS, mLocationWCS);
}

// =================================================================================================
//! Compute the relative location of another entity.
//! The magnitude of this vector is the distance to the entity.
ut::coords::WCS UtEntityPart::GetRelativeLocationWCS(const UtEntity* aOtherEntityPtr) const
{
   ut::coords::WCS out;
   GetRelativeLocationWCS(aOtherEntityPtr, out.GetData());
   return out;
}

// =================================================================================================
//! Compute the relative location of another entity.
//! The magnitude of this vector is the distance to the entity.
void UtEntityPart::GetRelativeLocationWCS(const UtEntity* aOtherEntityPtr, double aRelativeLocationWCS[3]) const
{
   // Ensure the WCS location is valid (we don't need to worry about rotation)
   if (!mLocationWCSIsValid)
   {
      UpdateLocationWCS();
   }

   // Compute the WCS vector from the origin of the PCS to the other entity
   double otherLocationWCS[3];
   aOtherEntityPtr->GetLocationWCS(otherLocationWCS);

   UtVec3d::Subtract(aRelativeLocationWCS, otherLocationWCS, mLocationWCS);
}

// =================================================================================================
//! Compute the relative location of another entity part.
//! The magnitude of this vector is the distance to the entity part.
ut::coords::WCS UtEntityPart::GetRelativeLocationWCS(const UtEntityPart* aOtherEntityPtr) const
{
   ut::coords::WCS out;
   GetRelativeLocationWCS(aOtherEntityPtr, out.GetData());
   return out;
}

// =================================================================================================
//! Compute the relative location of another entity part.
//! The magnitude of this vector is the distance to the entity part.
void UtEntityPart::GetRelativeLocationWCS(const UtEntityPart* aOtherPartPtr, double aRelativeLocationWCS[3]) const
{
   // Ensure the WCS location is valid (we don't need to worry about rotation)
   if (!mLocationWCSIsValid)
   {
      UpdateLocationWCS();
   }

   // Compute the WCS vector from the origin of the PCS to the other entity
   double otherLocationWCS[3];
   aOtherPartPtr->GetLocationWCS(otherLocationWCS);

   UtVec3d::Subtract(aRelativeLocationWCS, otherLocationWCS, mLocationWCS);
}

// =================================================================================================
//! Transform a vector in the PCS frame to the WCS frame.
//!
//! This can be used to transform any vector (velocity, acceleration, etc.)
//! from the PCS frame to the WCS frame.
ut::coords::WCS UtEntityPart::ConvertPCSVectorToWCS(const ut::coords::PCS& aVectorPCS) const
{
   ut::coords::WCS out;
   ConvertPCSVectorToWCS(out.GetData(), aVectorPCS.GetData());
   return out;
}

// =================================================================================================
//! Transform a vector in the PCS frame to the WCS frame.
//!
//! This can be used to transform any vector (velocity, acceleration, etc.)
//! from the PCS frame to the WCS frame.
void UtEntityPart::ConvertPCSVectorToWCS(double aVectorWCS[3], const double aVectorPCS[3]) const
{
   // Ensure the WCS->PCS transformation is up-to-date
   if (!mWCSToPCSTransformIsValid)
   {
      UpdateWCSToPCSTransform();
   }
   // Rotate the vector in the PCS to the WCS frame
   UtMat3d::InverseTransform(aVectorWCS, mWCSToPCSTransform, aVectorPCS);
}

// =================================================================================================
//! Transform a vector in the WCS frame to the PCS frame.
//!
//! This can be used to transform any vector (velocity, acceleration, etc.)
//! from the WCS frame to the PCS frame.
ut::coords::PCS UtEntityPart::ConvertWCSVectorToPCS(const ut::coords::WCS& aVectorWCS) const
{
   ut::coords::PCS out;
   ConvertWCSVectorToPCS(out.GetData(), aVectorWCS.GetData());
   return out;
}

// =================================================================================================
//! Transform a vector in the WCS frame to the PCS frame.
//!
//! This can be used to transform any vector (velocity, acceleration, etc.)
//! from the WCS frame to the PCS frame.
void UtEntityPart::ConvertWCSVectorToPCS(double aVectorPCS[3], const double aVectorWCS[3]) const
{
   // Ensure the WCS->PCS transformation is up-to-date
   if (!mWCSToPCSTransformIsValid)
   {
      UpdateWCSToPCSTransform();
   }
   // Rotate the vector in the WCS to the PCS frame
   UtMat3d::Transform(aVectorPCS, mWCSToPCSTransform, aVectorWCS);
}

// =================================================================================================
//! Transform a vector in the ECS frame to the PCS frame.
//!
//! This can be used to transform any vector (velocity, acceleration, etc.)
//! from the ECS frame to the PCS frame.
void UtEntityPart::ConvertECSVectorToPCS(double aVectorPCS[3], const double aVectorECS[3]) const
{
   // Ensure the ECS->PCS transformation is up-to-date
   if (!mECSToPCSTransformIsValid)
   {
      UpdateECSToPCSTransform();
   }
   // Rotate the vector in the ECS to the PCS frame
   UtMat3d::Transform(aVectorPCS, mECSToPCSTransform, aVectorECS);
}

// =================================================================================================
//! Transform a vector in the PCS frame to the ECS frame.
//!
//! This can be used to transform any vector (velocity, acceleration, etc.)
//! from the PCS frame to the ECS frame.
void UtEntityPart::ConvertPCSVectorToECS(double aVectorECS[3], const double aVectorPCS[3]) const
{
   // Ensure the ECS->PCS transformation is up-to-date
   if (!mECSToPCSTransformIsValid)
   {
      UpdateECSToPCSTransform();
   }
   // Rotate the vector in the PCS to the ECS frame
   UtMat3d::InverseTransform(aVectorECS, mECSToPCSTransform, aVectorPCS);
}

// =================================================================================================
//! Transform a vector in the ECS frame to the PCS frame.
//!
//! This can be used to transform any vector (velocity, acceleration, etc.)
//! from the ECS frame to the PCS frame.
ut::coords::PCS UtEntityPart::ConvertECSVectorToPCS(const ut::coords::ECS& aVectorECS) const
{
   ut::coords::PCS out;
   ConvertECSVectorToPCS(out.GetData(), aVectorECS.GetData());
   return out;
}

// =================================================================================================
//! Transform a vector in the PCS frame to the ECS frame.
//!
//! This can be used to transform any vector (velocity, acceleration, etc.)
//! from the PCS frame to the ECS frame.
ut::coords::ECS UtEntityPart::ConvertPCSVectorToECS(const ut::coords::PCS& aVectorPCS) const
{
   ut::coords::ECS out;
   ConvertPCSVectorToECS(out.GetData(), aVectorPCS.GetData());
   return out;
}

ut::coords::WCS UtEntityPart::ConvertPCSToWCS(const ut::coords::PCS& aLocationPCS) const
{
   ut::coords::WCS out;
   ConvertPCSVectorToWCS(out.GetData(), aLocationPCS.GetData());
   out += GetLocationWCS();
   return out;
}

ut::coords::ECS UtEntityPart::ConvertPCSToECS(const ut::coords::PCS& aLocationPCS) const
{
   ut::coords::ECS out;
   ConvertPCSVectorToECS(out.GetData(), aLocationPCS.GetData());
   out += ut::coords::ECS(mLocationECS);
   return out;
}

ut::coords::PCS UtEntityPart::ConvertWCSToPCS(const ut::coords::WCS& aLocationWCS) const
{
   ut::coords::PCS out;
   ConvertWCSVectorToPCS(out.GetData(), (aLocationWCS - GetLocationWCS()).GetData());
   return out;
}

ut::coords::PCS UtEntityPart::ConvertECSToPCS(const ut::coords::ECS& aLocationECS) const
{
   ut::coords::PCS out;
   ConvertECSVectorToPCS(out.GetData(), (aLocationECS - GetLocation()).GetData());
   return out;
}

// =================================================================================================
//! Compute a new coordinate transformation that is a rotation of the current frame.
void UtEntityPart::ComputeRotationalTransform(double aYaw, double aPitch, double aRoll, double aWCS_ToLocalTransform[3][3])
{
   // Ensure the WCS->PCS transform is current
   if (!mWCSToPCSTransformIsValid)
   {
      UpdateWCSToPCSTransform();
   }

   // Compute the PCS->Local transform
   double pcsToLocalTransform[3][3];
   UtEntity::ComputeRotationalTransform(aYaw, aPitch, aRoll, pcsToLocalTransform);

   // Compute the WCS->Local transform.
   // [ WCS->Local ] = [ PCS->Local ] x [ WCS->PCS ]
   UtMat3d::Multiply(aWCS_ToLocalTransform, pcsToLocalTransform, mWCSToPCSTransform);
}

// =================================================================================================
//! Get the ECS->PCS coordinate transformation matrix.
// virtual
void UtEntityPart::GetECSToPCSTransform(double aECSToPCSTransform[3][3]) const
{
   // Ensure the ECS->PCS transformation is up-to-date
   if (!mECSToPCSTransformIsValid)
   {
      UpdateECSToPCSTransform();
   }
   UtMat3d::Set(aECSToPCSTransform, mECSToPCSTransform);
}

// =================================================================================================
//! Get the ECS->PCS coordinate transformation matrix.
// virtual
void UtEntityPart::GetWCSToPCSTransform(double aWCSToPCSTransform[3][3]) const
{
   // Ensure the WCS->PCS transform is current
   if (!mWCSToPCSTransformIsValid)
   {
      UpdateWCSToPCSTransform();
   }
   UtMat3d::Set(aWCSToPCSTransform, mWCSToPCSTransform);
}

// =================================================================================================
//! Recompute the WCS location of the PCS origin
// private
void UtEntityPart::UpdateLocationWCS() const
{
   assert(mOwningEntityPtr != nullptr);

   // Compute the location of the origin of the PCS within the WCS
   mOwningEntityPtr->ConvertECSToWCS(mLocationECS, mLocationWCS);
   mLocationWCSIsValid = true;
}

// =================================================================================================
//! Recompute the ECS->PCS rotation transformation.
// private
void UtEntityPart::UpdateECSToPCSTransform() const
{
   UtEntity::ComputeRotationalTransform(mYaw, mPitch, mRoll, mECSToPCSTransform);
   mECSToPCSTransformIsValid = true;
}

// =================================================================================================
//! Recompute the WCS->PCS rotation transform.
// private
void UtEntityPart::UpdateWCSToPCSTransform() const
{
   // Compute the ECS->PCS transform.
   if (!mECSToPCSTransformIsValid)
   {
      UpdateECSToPCSTransform();
   }

   // [ WCS->PCS ] = [ ECS->PCS ] x [ WCS->ECS ]
   mOwningEntityPtr->ComputeWCSToLocalTransform(mWCSToPCSTransform, mECSToPCSTransform);
   mWCSToPCSTransformIsValid = true;
}
