// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSpaceOrientation.hpp"

#include <sstream>

#include "UtEllipsoidalEarth.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMat3.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "UtSun.hpp"
#include "WsfAttitudeController.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfSpaceMoverBase.hpp"
#include "WsfTrackList.hpp"

//! Compute the Yaw, Pitch an Roll angles from a set of body frame vectors expressed in ECI frame.
//! The input frame must be a right-handed orthonormal frame expressed in the ECI frame. The input vectors
//! correspond to the usual ECS vectors.
namespace
{
void CalculateFromBodyFrame(const UtVec3d& aXe, const UtVec3d& aYe, const UtVec3d& aZe, double& aPsi, double& aTheta, double& aPhi)
{
   aTheta = -asin(aXe[2]);
   aPsi   = atan2(aXe[1], aXe[0]);
   aPhi   = atan2(aYe[2], aZe[2]);
}
} // namespace

namespace wsf
{
namespace space
{

const char* Orientation::GetScriptClassName() const
{
   return "WsfSpaceOrientation";
}

//! Calculate the direction cosines associated with the rotation of a platform part to point to a given orientation.
//! The provided x-axis orientation corresponds with a vector pointing along the
//! PCS x-axis to a target (e.g., another platform, nadir, the sun, etc.), referenced in ECI coordinates.  A second
//! vector provides a PCS z-axis constraint, such that the PCS z-axis will lie in the same plane as the PCS x-axis and
//! the constraint vector.
//! @param aX_AxisOrientationECI The direction along which the PCS x-axis is to point.
//! @param aZ_AxisConstraintECI The direction that, along with the PCS x-axis, defines the plane within which the PCS
//! z-axis is constrained to point.
//! @param aX_DC The x direction cosine vector.
//! @param aY_DC The y direction cosine vector.
//! @param aZ_DC The z direction cosine vector.
void Orientation::CalculatePCS_DirectionCosines(const UtVec3d& aX_AxisOrientationECI,
                                                const UtVec3d& aZ_AxisConstraintECI,
                                                UtVec3d&       aX_DC,
                                                UtVec3d&       aY_DC,
                                                UtVec3d&       aZ_DC)
{
   aX_DC = aX_AxisOrientationECI;
   aZ_DC = aZ_AxisConstraintECI;

   aX_DC.Normalize();
   aZ_DC.Normalize();

   // The Y axis is Z' x X
   aY_DC.CrossProduct(aZ_DC, aX_DC);
   aY_DC.Normalize();

   // Z is X x Y
   aZ_DC.CrossProduct(aX_DC, aY_DC);
}

//! Calculate a spacecraft orientation such that the ECS X-axis is aligned with the aX_AxisOrientation vector,
//! and the ECS Z-axis constrained by the aZ_AxisConstraint vector.
//! @param aX_AxisOrientationECI The direction along which the ECS x-axis is to point.
//! @param aZ_AxisConstraintECI The direction that, along with the ECS x-axis, defines the plane within which the ECS
//! z-axis is constrained to point.
//! @param aPsi The ECI-referenced yaw of the platform.
//! @param aTheta The ECI-referenced pitch of the platform.
//! @param aPhi The ECI-referenced roll of the platform.
void Orientation::CalculateX_Aligned(const UtVec3d& aX_AxisOrientationECI,
                                     const UtVec3d& aZ_AxisConstraintECI,
                                     double&        aPsi,
                                     double&        aTheta,
                                     double&        aPhi)
{
   UtVec3d xP, yP, zP;
   Orientation::CalculatePCS_DirectionCosines(aX_AxisOrientationECI, aZ_AxisConstraintECI, xP, yP, zP);
   CalculateFromBodyFrame(xP, yP, zP, aPsi, aTheta, aPhi);
}

//! Calculate a spacecraft orientation such that the ECS X-axis is aligned with the aX_AxisOrientation vector,
//! and the ECS Z-axis constrained by the aZ_AxisConstraint vector.
//! @param aZ_AxisOrientationECI The direction along which the ECS z-axis is to point.
//! @param aX_AxisConstraintECI The direction that, along with the ECS z-axis, defines the plane within which the ECS
//! x-axis is constrained to point.
//! @param aPsi The ECI-referenced yaw of the platform.
//! @param aTheta The ECI-referenced pitch of the platform.
//! @param aPhi The ECI-referenced roll of the platform.
void Orientation::CalculateZ_Aligned(const UtVec3d& aZ_AxisOrientationECI,
                                     const UtVec3d& aX_AxisConstraintECI,
                                     double&        aPsi,
                                     double&        aTheta,
                                     double&        aPhi)
{
   UtVec3d xP, yP, zP;
   Orientation::CalculatePCS_DirectionCosines(aZ_AxisOrientationECI, aX_AxisConstraintECI, xP, yP, zP);
   UtVec3d zPCopy(zP);
   // Change the direction cosines to account for the ECS-PCS rotation.
   zP = xP;
   xP = zPCopy;
   yP = -yP;
   CalculateFromBodyFrame(xP, yP, zP, aPsi, aTheta, aPhi);
}

//! Calculate a generalized ECI-referenced orientation.  Provided yaw, pitch, and roll correspond with an orientation
//! with respect to the platform's ECS (e.g., of a WsfArticulatedPart, defining a PCS).  The provided x-axis orientation
//! corresponds with a vector pointing along the PCS x-axis to a target (e.g., another platform, nadir, the sun, etc.),
//! referenced in ECI coordinates.  A second vector provides a PCS z-axis constraint, such that the z-axis will lie in
//! the same plane as the PCS x-axis and the constraint vector.
//! @param aYawECS The yaw angle of the PCS with respect to the ECS.
//! @param aPitchECS The pitch angle of the PCS with respect to the ECS.
//! @param aRollECS The roll angle of the PCS with respect to the ECS.
//! @param aX_AxisOrientationECI The direction along which the PCS x-axis is to point.
//! @param aZ_AxisConstraintECI The direction that, along with the PCS x-axis, defines the plane within which the PCS
//! z-axis is constrained to point.
//! @param aPsi The ECI-referenced yaw of the platform.
//! @param aTheta The ECI-referenced pitch of the platform.
//! @param aPhi The ECI-referenced roll of the platform.
void Orientation::CalculateGeneral(double         aYawECS,
                                   double         aPitchECS,
                                   double         aRollECS,
                                   const UtVec3d& aX_AxisOrientationECI,
                                   const UtVec3d& aZ_AxisConstraintECI,
                                   double&        aPsi,
                                   double&        aTheta,
                                   double&        aPhi)
{
   double Tecs_pcs[3][3];
   UtEntity::ComputeRotationalTransform(aYawECS, aPitchECS, aRollECS, Tecs_pcs);
   UtVec3d xP, yP, zP;
   CalculatePCS_DirectionCosines(aX_AxisOrientationECI, aZ_AxisConstraintECI, xP, yP, zP);
   double Tpcs_eci[3][3] = {
      {xP[0], yP[0], zP[0]},
      {xP[1], yP[1], zP[1]},
      {xP[2], yP[2], zP[2]},
   };
   double Tecs_eci[3][3];
   UtMat3d::Multiply(Tecs_eci, Tpcs_eci, Tecs_pcs);
   for (auto i = 0; i < 3; ++i)
   {
      xP[i] = Tecs_eci[i][0];
      yP[i] = Tecs_eci[i][1];
      zP[i] = Tecs_eci[i][2];
   }
   CalculateFromBodyFrame(xP, yP, zP, aPsi, aTheta, aPhi);
}

//! Calculate a spacecraft orientation such that either the ECS X-axis or Z-axis is aligned with the
//! aPointingAxisOrientationECI vector, and the ECS Z-axis constrained by the aConstraintAxisOrientationECI vector.
//! @param aPointingAxisOrientationECI The direction along which the ECS "aligned" axis is to point.
//! @param aConstraintAxisOrientationECI The direction that, along with the ECS "aligned" axis, defines the plane within
//! which the ECS z-axis is constrained to point.
//! @param aPsi The ECI-referenced yaw of the platform.
//! @param aTheta The ECI-referenced pitch of the platform.
//! @param aPhi The ECI-referenced roll of the platform.
void Orientation::CalculateAligned(const UtVec3d& aPointingAxisOrientationECI,
                                   const UtVec3d& aConstraintAxisOrientationECI,
                                   double&        aPsi,
                                   double&        aTheta,
                                   double&        aPhi) const
{
   if (mDefaultX_Aligned)
   {
      CalculateX_Aligned(aPointingAxisOrientationECI, aConstraintAxisOrientationECI, aPsi, aTheta, aPhi);
   }
   else
   {
      CalculateZ_Aligned(aPointingAxisOrientationECI, aConstraintAxisOrientationECI, aPsi, aTheta, aPhi);
   }
}

// private
UtVec3d Orientation::Solar() const
{
   UtVec3d solarVec;
   UtSun::GetSunLocationECI(mControllerPtr->GetMover()->GetCurrentTime(), solarVec.GetData());
   return solarVec;
}

// private
UtVec3d Orientation::VelocityECI() const
{
   UtVec3d velocityECI;
   mControllerPtr->GetMover()->GetPlatform()->GetVelocityECI(velocityECI.GetData());
   return velocityECI;
}

// private
UtVec3d Orientation::VelocityECEF() const
{
   UtVec3d velocityECEF;
   mControllerPtr->GetMover()->GetPlatform()->GetVelocityWCS(velocityECEF.GetData());
   UtVec3d velocityECEF_ECI;
   double  earthAngle = mControllerPtr->GetMover()->GetPropagator().GetCurrentTime().GetEarthAngle();
   double  sinPhi     = sin(earthAngle);
   double  cosPhi     = cos(earthAngle);

   velocityECEF_ECI[0] = (cosPhi * velocityECEF[0]) - (sinPhi * velocityECEF[1]);

   velocityECEF_ECI[1] = (sinPhi * velocityECEF[0]) + (cosPhi * velocityECEF[1]);

   velocityECEF_ECI[2] = velocityECEF[2];
   return velocityECEF_ECI;
}

// private
UtVec3d Orientation::Nadir() const
{
   UtVec3d nadir;
   mControllerPtr->GetMover()->GetPlatform()->GetLocationECI(nadir.GetData());
   nadir.Multiply(-1.0);
   return nadir;
}

//! Factory method to create the various orientation types.
//! @param aOrientation The orientation type to be created.
//! @return If a recognized orientation name is specified,
//! returns the new orientation type (unique_ptr); otherwise nullptr.
std::unique_ptr<Orientation> Orientation::Factory(const std::string& aOrientation)
{
   std::unique_ptr<Orientation> orientationPtr = nullptr;
   if (aOrientation == OrientationNadirECI_Velocity::GetTypeName())
   {
      orientationPtr = ut::make_unique<OrientationNadirECI_Velocity>();
   }
   else if (aOrientation == OrientationSolarNadir::GetTypeName())
   {
      orientationPtr = ut::make_unique<OrientationSolarNadir>();
   }
   else if (aOrientation == OrientationNadirSolar::GetTypeName())
   {
      orientationPtr = ut::make_unique<OrientationNadirSolar>();
   }
   else if (aOrientation == OrientationNadirECEF_Velocity::GetTypeName())
   {
      orientationPtr = ut::make_unique<OrientationNadirECEF_Velocity>();
   }
   else if (aOrientation == OrientationECI_VelocityNadir::GetTypeName())
   {
      orientationPtr = ut::make_unique<OrientationECI_VelocityNadir>();
   }
   else if (aOrientation == OrientationECI_VelocitySolar::GetTypeName())
   {
      orientationPtr = ut::make_unique<OrientationECI_VelocitySolar>();
   }
   else if (aOrientation == OrientationEntitySolar::GetTypeName())
   {
      orientationPtr = ut::make_unique<OrientationEntitySolar>();
   }
   else if (aOrientation == OrientationEntityNadir::GetTypeName())
   {
      orientationPtr = ut::make_unique<OrientationEntityNadir>();
   }
   else if (aOrientation == OrientationEntityOrbitPlane::GetTypeName())
   {
      orientationPtr = ut::make_unique<OrientationEntityOrbitPlane>();
   }
   else if (aOrientation == OrientationPointOrbitPlane::GetTypeName())
   {
      orientationPtr = ut::make_unique<OrientationPointOrbitPlane>();
   }
   else if (aOrientation == OrientationNone::GetTypeName())
   {
      orientationPtr = ut::make_unique<OrientationNone>();
   }

   return orientationPtr;
}

//! Compute the orbit plane constraint vector, lying in the orbit plane and
//! perpendicular to the aAlignVector.
//! aAlignVector The vector used as the basis for computing the plane constraint vector.
UtVec3d Orientation::OrbitPlaneConstraint(const UtVec3d& aAlignVector) const
{
   UtVec3d r;
   mControllerPtr->GetMover()->GetPlatform()->GetLocationECI(r.GetData());
   UtVec3d v;
   mControllerPtr->GetMover()->GetPlatform()->GetVelocityECI(v.GetData());
   UtVec3d r_x_v;
   r_x_v.CrossProduct(r, v);
   UtVec3d orbitPlaneConstraint;
   orbitPlaneConstraint.CrossProduct(r_x_v, aAlignVector);
   if (orbitPlaneConstraint.MagnitudeSquared() == 0.0) // unlikely special case
   {
      orbitPlaneConstraint = v; // in-plane vector.
   }
   orbitPlaneConstraint.Normalize();
   return orbitPlaneConstraint;
}

//! Generic exception handler for exceptions encountered in derived classes.
void Orientation::HandleException(const UtException& aException) const
{
   // Issue a warning message once.
   if (mIssueErrorMessage)
   {
      auto logger = ut::log::warning() << "Space Orientation";
      logger.AddNote() << aException.what() << " for space orientation type " << GetType() << " on platform "
                       << mControllerPtr->GetMover()->GetPlatform()->GetName() << ".";
      mIssueErrorMessage = false;
   }
}

OrientationEntity::OrientationEntity()
   : Orientation(true)
{
}

bool OrientationEntity::ProcessInput(UtInput& aInput)
{
   aInput.ReadValue(mEntityName);
   return true;
}

void OrientationEntity::SetEntityName(const std::string& aEntityName)
{
   mEntityName    = aEntityName;
   mOrientToTrack = false;
}

void OrientationEntity::SetTrackId(const WsfTrackId& aTrackId)
{
   mTrackId       = aTrackId;
   mOrientToTrack = true;
}

bool OrientationEntity::IsValid() const
{
   // If there is no controller, we have no way to access the simulation and are in a verify step.
   // In this case there is no way to tell if the external entity exists (return true).
   bool isValid = true;
   if (mControllerPtr != nullptr)
   {
      isValid = (GetTargetEntity() != nullptr);
   }

   return isValid;
}

WsfPlatform* OrientationEntity::GetTargetEntity() const
{
   return mControllerPtr->GetMover()->GetSimulation()->GetPlatformByName(mEntityName);
}

//! @return the unit vector pointing from the platform to the Entity.
UtVec3d OrientationEntity::TargetVector() const
{
   UtVec3d rsoLocECI;
   auto    platformPtr = mControllerPtr->GetMover()->GetPlatform();

   if (mOrientToTrack)
   {
      WsfLocalTrack* trackPtr = platformPtr->GetMasterTrackList().FindTrack(mTrackId);
      if (trackPtr == nullptr)
      {
         std::ostringstream ss;
         ss << mTrackId;
         throw TrackNotFoundException(ss.str());
      }
      if (!trackPtr->LocationValid())
      {
         std::ostringstream ss;
         ss << mTrackId;
         throw TrackLocationInvalidException(ss.str());
      }
      UtVec3d rsoLocWCS;
      trackPtr->GetExtrapolatedLocationWCS(platformPtr->GetSimTime(), rsoLocWCS.GetData());
      platformPtr->ConvertWCSToECI(rsoLocWCS.GetData(), rsoLocECI.GetData());
   }
   else
   {
      WsfPlatform* rsoPtr = GetTargetEntity();
      if (rsoPtr == nullptr)
      {
         throw EntityNotFoundException(mEntityName);
      }
      rsoPtr->GetLocationECI(rsoLocECI.GetData());
   }
   UtVec3d myLocECI;
   platformPtr->GetLocationECI(myLocECI.GetData());
   UtVec3d targetVec{rsoLocECI.Subtract(myLocECI)};
   targetVec.Normalize();
   return targetVec;
}

//! Read the name of the GeoPoint to access on the platform.
bool OrientationGeoPoint::ProcessInput(UtInput& aInput)
{
   aInput.ReadValue(mGeoPointName);
   return true;
}

bool OrientationGeoPoint::IsValid() const
{
   // If there is no controller, we have no way to access the simulation and are in a verify step.
   // In this case there is no way to tell if the geo-point exists (return true).
   bool isValid = true;
   if (mControllerPtr != nullptr)
   {
      isValid = (mControllerPtr->GetMover()->GetPlatform()->GetComponent<WsfGeoPoint>(mGeoPointName) != nullptr);
   }
   return isValid;
}

void OrientationGeoPoint::SetGeoPoint(const WsfGeoPoint& aGeoPoint)
{
   mGeoPointPtr.reset(aGeoPoint.Clone());
   mGeoPointName = aGeoPoint.GetName();
}

//! @return The unit vector from the platform to the GeoPoint.
UtVec3d OrientationGeoPoint::TargetVector() const
{
   WsfPlatform* platformPtr = mControllerPtr->GetMover()->GetPlatform();

   if (mGeoPointPtr == nullptr)
   {
      WsfGeoPoint* targetPtr = platformPtr->GetComponent<WsfGeoPoint>(mGeoPointName);
      if (targetPtr != nullptr)
      {
         mGeoPointPtr.reset(targetPtr->Clone());
      }
      else
      {
         throw GeoPointNotFoundException(mGeoPointName);
      }
   }

   UtVec3d targetLocWCS;
   mGeoPointPtr->GetLocationWCS(targetLocWCS.GetData());

   UtVec3d targetLocECI;
   platformPtr->ConvertWCSToECI(targetLocWCS.GetData(), targetLocECI.GetData());
   UtVec3d myLocECI;
   platformPtr->GetLocationECI(myLocECI.GetData());
   UtVec3d targetVec;
   targetVec.Subtract(targetLocECI, myLocECI);
   targetVec.Normalize();
   return targetVec;
}

bool OrientationEntitySolar::Calculate(double& aPsi, double& aTheta, double& aPhi) const
{
   try
   {
      CalculateAligned(TargetVector(), Solar(), aPsi, aTheta, aPhi);
      return true;
   }
   catch (const UtException& aException)
   {
      HandleException(aException);
      return false;
   }
}

bool OrientationPointOrbitPlane::Calculate(double& aPsi, double& aTheta, double& aPhi) const
{
   UtVec3d targetVector;
   try
   {
      targetVector = TargetVector();
   }
   catch (const UtException& aException)
   {
      HandleException(aException);
      return false;
   }
   CalculateAligned(targetVector, OrbitPlaneConstraint(targetVector), aPsi, aTheta, aPhi);
   return true;
}

//*****************************************************************************
// OrientationECI_VelocitySolar
OrientationECI_VelocitySolar::OrientationECI_VelocitySolar()
   : Orientation(true)
{
   SetType(GetTypeName());
}
//*****************************************************************************

//*****************************************************************************
// OrientationGeoPoint
OrientationGeoPoint::OrientationGeoPoint()
   : Orientation(true)
{
}

OrientationGeoPoint::OrientationGeoPoint(const OrientationGeoPoint& aSrc)
   : Orientation(aSrc)
   , mGeoPointName(aSrc.mGeoPointName)
   , mGeoPointPtr(nullptr)
{
   if (aSrc.mGeoPointPtr != nullptr)
   {
      mGeoPointPtr.reset(aSrc.mGeoPointPtr->Clone());
   }
}
//*****************************************************************************

//*****************************************************************************
// OrientationSolarNadir
OrientationSolarNadir::OrientationSolarNadir()
   : Orientation(false)
{
   SetType(GetTypeName());
}
//*****************************************************************************

//*****************************************************************************
// OrientationNadirSolar
OrientationNadirSolar::OrientationNadirSolar()
   : Orientation(false)
{
   SetType(GetTypeName());
}
//*****************************************************************************

//*****************************************************************************
// OrientationNone
OrientationNone::OrientationNone()
   : Orientation(true)
{
   SetType(GetTypeName());
}
//*****************************************************************************

//*****************************************************************************
// OrientationNadirECEF_Velocity
OrientationNadirECEF_Velocity::OrientationNadirECEF_Velocity()
   : Orientation(false)
{
   SetType(GetTypeName());
}
//*****************************************************************************

//*****************************************************************************
// OrientationNadirECI_Velocity
OrientationNadirECI_Velocity::OrientationNadirECI_Velocity()
   : Orientation(false)
{
   SetType(GetTypeName());
}
//*****************************************************************************

//*****************************************************************************
// OrientationECI_VelocityNadir
OrientationECI_VelocityNadir::OrientationECI_VelocityNadir()
   : Orientation(true)
{
   SetType(GetTypeName());
}
//*****************************************************************************

//*****************************************************************************
// OrientationEntityNadir
OrientationEntityNadir::OrientationEntityNadir()
   : OrientationEntity()
{
   SetType(GetTypeName());
}

bool OrientationEntityNadir::Calculate(double& aPsi, double& aTheta, double& aPhi) const
{
   try
   {
      CalculateAligned(TargetVector(), Nadir(), aPsi, aTheta, aPhi);
   }
   catch (const UtException& aException)
   {
      HandleException(aException);
      return false;
   }
   return true;
}

//*****************************************************************************

//*****************************************************************************
// OrientationEntitySolar
OrientationEntitySolar::OrientationEntitySolar()
   : OrientationEntity()
{
   SetType(GetTypeName());
}
//*****************************************************************************

//*****************************************************************************
// OrientationEntityOrbitPlane
OrientationEntityOrbitPlane::OrientationEntityOrbitPlane()
   : OrientationEntity()
{
   SetType(GetTypeName());
}

bool OrientationEntityOrbitPlane::Calculate(double& aPsi, double& aTheta, double& aPhi) const
{
   UtVec3d targetVector;
   try
   {
      targetVector = TargetVector();
   }
   catch (const UtException& aException)
   {
      HandleException(aException);
      return false;
   }
   CalculateAligned(targetVector, OrbitPlaneConstraint(targetVector), aPsi, aTheta, aPhi);
   return true;
}
//*****************************************************************************

//*****************************************************************************
// OrientationEntityOrbitPlane
OrientationPointOrbitPlane::OrientationPointOrbitPlane()
   : OrientationGeoPoint()

{
   SetType(GetTypeName());
}
//*****************************************************************************

} // namespace space
} // namespace wsf
