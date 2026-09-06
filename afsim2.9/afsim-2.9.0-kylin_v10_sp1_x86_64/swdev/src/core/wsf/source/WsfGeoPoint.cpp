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

#include "WsfGeoPoint.hpp"

#include <memory>

#include "UtCalendar.hpp"
#include "UtCentralBody.hpp"
#include "UtEllipsoidalCentralBody.hpp"
#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtMath.hpp"
#include "UtMeasurementUtil.hpp"
#include "UtMemory.hpp"
#include "UtSphericalEarth.hpp"
#include "UtSun.hpp"
#include "UtVec3.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfDateTime.hpp"
#include "WsfEnvironment.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"


namespace
{
//! Component factory to process platform input.
class GeoPointComponentFactory : public WsfComponentFactory<WsfPlatform>
{
public:
   bool ProcessInput(UtInput& aInput, WsfPlatform& aPlatform) override
   {
      bool myCommand(false);
      if (aInput.GetCommand() == "geo_point")
      {
         myCommand        = true;
         auto geoPointPtr = ut::make_unique<WsfGeoPoint>();
         geoPointPtr->ProcessInput(aInput);
         aPlatform.DeleteComponent<WsfGeoPoint>(geoPointPtr->GetNameId());
         if (!aPlatform.AddComponent(geoPointPtr.get())) // Should never fail...
         {
            throw UtInput::BadValue(aInput, "Duplicate geo_point: " + geoPointPtr->GetName());
         }
         geoPointPtr.release();
      }
      return myCommand;
   }
};
} // namespace

// static
void WsfGeoPoint::RegisterComponentFactory(WsfScenario& aScenario)
{
   aScenario.RegisterComponentFactory(ut::make_unique<GeoPointComponentFactory>());
}

// ================================================================================================
WsfGeoPoint::WsfGeoPoint()
   : WsfSimplePlatformComponent(cWSF_COMPONENT_GEOPOINT)
{
}

// ================================================================================================
//! Constructor from an input point in world coordinates (WCS).
//! @param aPointWCS The world coordinates of the geo point.
WsfGeoPoint::WsfGeoPoint(const double aPointWCS[3])
   : WsfSimplePlatformComponent(cWSF_COMPONENT_GEOPOINT)
{
   SetLocationWCS(aPointWCS);
}

// ================================================================================================
//! Constructor from an input in (lat, lon, alt).
//! @param aLat The latitude of the geo point.
//! @param aLon The longitude of the geo point.
//! @param aAlt The altitude of the geo point.
WsfGeoPoint::WsfGeoPoint(double aLat, double aLon, double aAlt)
   : WsfSimplePlatformComponent(cWSF_COMPONENT_GEOPOINT)
{
   SetLocationLLA(aLat, aLon, aAlt);
}

// ================================================================================================
// protected
//! Copy constructor (for Clone())
WsfGeoPoint::WsfGeoPoint(const WsfGeoPoint& aSrc)
   : WsfSimplePlatformComponent(aSrc)
   , mRefPointWCS(aSrc.mRefPointWCS)
   , mRefLat(aSrc.mRefLat)
   , mRefLon(aSrc.mRefLon)
   , mRefAlt(aSrc.mRefAlt)
   , mWCS_LocationValid(aSrc.mWCS_LocationValid)
   , mLLA_LocationValid(aSrc.mLLA_LocationValid)
{
}

//! Assignment operator
WsfGeoPoint& WsfGeoPoint::operator=(const WsfGeoPoint& aRhs)
{
   if (this != &aRhs)
   {
      WsfSimplePlatformComponent::operator=(aRhs);
      mRefLat                             = aRhs.mRefLat;
      mRefLon                             = aRhs.mRefLon;
      mRefAlt                             = aRhs.mRefAlt;
      mRefPointWCS                        = aRhs.mRefPointWCS;
      mWCS_LocationValid                  = aRhs.mWCS_LocationValid;
      mLLA_LocationValid                  = aRhs.mLLA_LocationValid;
   }
   return *this;
}

// ================================================================================================
// virtual
/** Process input from a generic source.

    Examine the current input command.  If it is NOT one of the commands
    recognized by this class then it simply returns 'false'.  If it is
    one of the commands recognized by this class then it processes the
    command and returns 'true'.

   @param aInput a reference to a UtInput object.
   @returns 'true' if the command was one recognized by the class or 'false' if not.
*/
bool WsfGeoPoint::ProcessInput(UtInput& aInput)
{
   std::string name;
   aInput.ReadValue(name);
   SetName(name);
   aInput.ReadValueOfType(mRefLat, UtInput::cLATITUDE);
   aInput.ReadValueOfType(mRefLon, UtInput::cLONGITUDE);
   aInput.ReadValueOfType(mRefAlt, UtInput::cLENGTH);
   mLLA_LocationValid = true;
   mWCS_LocationValid = false;
   return true;
}

// ================================================================================================
//! Get the distance from a world coordinate location to a geo point.
//! @param aLocationWCS The reference location in world coordinates.
double WsfGeoPoint::GetDistanceFrom(double aLocationWCS[3]) const
{
   double result[3];
   UpdateWCS_Location();
   UtVec3d::Subtract(result, mRefPointWCS.GetData(), aLocationWCS);
   return UtVec3d::Magnitude(result);
}

// ================================================================================================
//! Get the distance from one geo point to another.
//! @param aRefPoint The reference geo point.
double WsfGeoPoint::GetDistanceFrom(const WsfGeoPoint& aRefPoint) const
{
   UpdateWCS_Location();
   aRefPoint.UpdateWCS_Location();
   return ((mRefPointWCS - aRefPoint.mRefPointWCS).Magnitude());
}

// ================================================================================================
//! Get the geo point location in world coordinates.
//! @param aLocationWCS [output] The geo point location in world coordinates.
void WsfGeoPoint::GetLocationWCS(double aLocationWCS[3]) const
{
   UpdateWCS_Location();
   UtVec3d::Set(aLocationWCS, mRefPointWCS.GetData());
}

// ================================================================================================
//! Set the geo point location in world coordinates.
void WsfGeoPoint::SetLocationWCS(const double aLocationWCS[3])
{
   UtVec3d::Set(mRefPointWCS.GetData(), aLocationWCS);
   mWCS_LocationValid = true;
   mLLA_LocationValid = false;
}

// ================================================================================================
//! Get the geo point location in earth-centered inertial coordinates.
//! @param aTime The epoch at which the ECI location is desired.
//! @param aLocationECI The returned ECI location.
void WsfGeoPoint::GetLocationECI(const UtCalendar& aTime, double aLocationECI[3]) const
{
   auto& refEntity = GetReferenceEntity();
   refEntity.SetECI_ReferenceEpoch(aTime);
   UpdateWCS_Location();
   refEntity.SetLocationWCS(mRefPointWCS.GetData());
   refEntity.GetLocationECI(aLocationECI);
}

// ================================================================================================
//! Get the latitude of a geo point.
//! @returns The latitude (in degrees, range  [-90, 90]) of the geo point.
double WsfGeoPoint::GetLat() const
{
   UpdateLLA_Location();
   return mRefLat;
}

// ================================================================================================
//! Get the longitude of a geo point.
//! @returns The longitude (in degrees, range  [-180, 180]) of the geo point.
double WsfGeoPoint::GetLon() const
{
   UpdateLLA_Location();
   return mRefLon;
}

// ================================================================================================
//! Get the altitude of a geo point.
//! @returns The altitude of the geo point above the ellipsoid.
double WsfGeoPoint::GetAlt() const
{
   UpdateLLA_Location();
   return mRefAlt;
}

// ================================================================================================
//! Get the location of the geo point in (lat, lon, alt) format.
//! @param aLat [output] The geodetic latitude in degrees in the range [-90, 90]
//! @param aLon [output] The longitude in degrees in the range [-180, 180]
//! @param aAlt [output] The altitude (in meters) above the ellipsoid.
void WsfGeoPoint::GetLocationLLA(double& aLat, double& aLon, double& aAlt) const
{
   UpdateLLA_Location();
   aLat = mRefLat;
   aLon = mRefLon;
   aAlt = mRefAlt;
}

// ================================================================================================
//! Set the location of the geo point in (lat, lon, alt) format.
//! @param aLat The GEODETIC latitude in degrees in the range [-90, 90]
//! @param aLon The longitude in degrees in the range [-180, 180]
//! @param aAlt The altitude (in meters) above the ellipsoid.
void WsfGeoPoint::SetLocationLLA(double aLat, double aLon, double aAlt)
{
   mRefLat            = aLat;
   mRefLon            = aLon;
   mRefAlt            = aAlt;
   mLLA_LocationValid = true;
   mWCS_LocationValid = false;
}

// ================================================================================================
//! Add an offset to the current location define by the geo point.
//!
//! The addition takes place in a local frame whose origin is the current point. The X-Y plane is
//! tangent to the Earth's surface at the location of the current point, with the X-axis aligned
//! pointed in the direction specified by the first argument. The Z-axis points down and the Y-axis
//! is formed such that the resulting coordinate system is right-handed (i.e.: Y = Z cross X).
//!
//! @param aHeading The heading that defines the X-axis (radians CW from North).
//! @param aOffset  The offset to be applied (X/Y/Z) (meters).
//!
//! @note A Z-offset of 0 does not preserve altitude. Use Extrapolate to perform that function.
void WsfGeoPoint::AddOffset(double aHeading, const double aOffset[3])
{
   auto& entity = GetReferenceEntity();
   UpdateLLA_Location();
   entity.SetLocationLLA(mRefLat, mRefLon, mRefAlt);
   entity.SetOrientationNED(aHeading, 0.0, 0.0);
   entity.ConvertECSToWCS(aOffset, mRefPointWCS.GetData());
   mWCS_LocationValid = true;
   mLLA_LocationValid = false;
}

// ================================================================================================
//! Add an north-east-down offset to the current location define by the geo point.
//!
//! The addition takes place in the local NED frame whose origin is the current point.
//!
//! @param aOffsetNED  The offset to be applied (North/East/Down) (meters).
//!
//! @note A down-offset of 0 does not preserve altitude.
void WsfGeoPoint::AddOffsetNED(const double aOffsetNED[3])
{
   double transECEF[3][3];
   double refLocWCS[3];
   UpdateLLA_Location();
   ut::EllipsoidalCentralBody::ComputeNEDTransform(mRefLat, mRefLon, mRefAlt, GetCentralBody().GetEllipsoid(), transECEF, refLocWCS);
   ut::EllipsoidalCentralBody::ConvertLocalToECEF(refLocWCS, transECEF, aOffsetNED, mRefPointWCS.GetData());
   mWCS_LocationValid = true;
   mLLA_LocationValid = false;
}

// ================================================================================================
//! Extrapolate a geo point for a given distance along a heading.
//!
//! @param aHeading The heading along which the extrapolation should occur (radians CW from North)
//! @param aDistance The extrapolation distance (meters).
void WsfGeoPoint::Extrapolate(double aHeading, double aDistance)
{
   double newLat, newLon;
   UpdateLLA_Location();
   UtSphericalEarth::ExtrapolateGreatCirclePosition(mRefLat, mRefLon, aHeading * UtMath::cDEG_PER_RAD, aDistance, newLat, newLon);
   SetLocationLLA(newLat, newLon, mRefAlt);
}

// ================================================================================================
//! Sets the altitude in meters above sea level.
void WsfGeoPoint::SetAltitudeMSL(double aAltitude)
{
   double lat, lon, alt;
   GetLocationLLA(lat, lon, alt);
   SetLocationLLA(lat, lon, aAltitude);
}

// ================================================================================================
//! Returns the true bearing to another geopoint in radians
double WsfGeoPoint::TrueBearingTo(const WsfGeoPoint& aRhs) const
{
   double thisLocWCS[3];
   double otherLocWCS[3];
   GetLocationWCS(thisLocWCS);
   aRhs.GetLocationWCS(otherLocWCS);
   auto& thisEntity = GetReferenceEntity();
   thisEntity.SetLocationWCS(thisLocWCS);
   thisEntity.SetOrientationNED(0.0, 0.0, 0.0);
   return thisEntity.TrueBearing(otherLocWCS);
}

// ================================================================================================
//! Returns the ground range to a location
double WsfGeoPoint::GroundRangeTo(const WsfGeoPoint& aRhs) const
{
   double thisLocWCS[3];
   double otherLocWCS[3];
   GetLocationWCS(thisLocWCS);
   aRhs.GetLocationWCS(otherLocWCS);
   double distance = UtMeasurementUtil::GroundRange(thisLocWCS, otherLocWCS);
   return distance;
}

// ================================================================================================
//! Return the apparent time in hours since midnight of this point at the given @p aEpoch.
double WsfGeoPoint::GetApparentTime(const UtCalendar& aEpoch) const
{
   double sunTrueRA;
   double dec;
   UtSun::GetSunLocationRA_Dec(aEpoch, sunTrueRA, dec);
   double sunMeanRA = sunTrueRA + UtSun::GetApparentSunExcess(aEpoch);

   double objRA;
   double eciPos[3];
   GetReferenceEntity().SetECI_ReferenceEpoch(aEpoch);
   UpdateWCS_Location();
   GetReferenceEntity().ConvertWCSToECI(mRefPointWCS.GetData(), eciPos);
   UtSphericalEarth::ConvertECIToRA_Dec(eciPos, objRA, dec);

   // See equation 9-61a on page 223 of Space Mission Engineering: The New SMAD, second printing, 2011.
   return UtMath::NormalizeAngleMinus180_180(UtMath::cDEG_PER_RAD * (-sunMeanRA + objRA)) / 15.0 + 12.0;
}

// ================================================================================================
//! Returns the slant range to a location
double WsfGeoPoint::SlantRangeTo(const WsfGeoPoint& aRhs, const bool aUseSpherical) const
{
   double thisLocWCS[3];
   double rhsLocWCS[3];
   double slantRange = 0.0;
   GetLocationWCS(thisLocWCS);
   aRhs.GetLocationWCS(rhsLocWCS);
   if (aUseSpherical)
   {
      slantRange = UtMeasurementUtil::SlantRange(thisLocWCS, rhsLocWCS);
   }
   else
   {
      double thisToOtherLocWCS[3];
      UtVec3d::Subtract(thisToOtherLocWCS, thisLocWCS, rhsLocWCS);
      slantRange = UtVec3d::Magnitude(thisToOtherLocWCS);
   }
   return slantRange;
}

UtEntity& WsfGeoPoint::GetReferenceEntity() const
{
   if (mEntityPtr == nullptr)
   {
      if ((GetComponentParent() != nullptr) && GetComponentParent()->GetCentralPoint().IsCentralBody())
      {
         mEntityPtr = ut::make_unique<UtEntity>(GetComponentParent()->GetCentralBody());
      }
      else
      {
         mEntityPtr = ut::make_unique<UtEntity>(); // using default central body (earthWGS84)
      }
   }
   return *mEntityPtr;
}

void WsfGeoPoint::UpdateLLA_Location() const
{
   if (!mLLA_LocationValid)
   {
      GetCentralBody().ConvertWCS_ToLLA(mRefPointWCS, mRefLat, mRefLon, mRefAlt);
      mLLA_LocationValid = true;
   }
}

void WsfGeoPoint::UpdateWCS_Location() const
{
   if (!mWCS_LocationValid)
   {
      GetCentralBody().ConvertLLA_ToWCS(mRefLat, mRefLon, mRefAlt, mRefPointWCS);
      mWCS_LocationValid = true;
   }
}

const ut::CentralBody& WsfGeoPoint::GetCentralBody() const
{
   if (mCentralBodyPtr == nullptr)
   {
      if ((GetComponentParent() != nullptr) && GetComponentParent()->GetCentralPoint().IsCentralBody())
      {
         mCentralBodyPtr = ut::clone(GetComponentParent()->GetCentralBody());
      }
      else // Either no component parent exists or the parent's central point is not a central body (unusual)
      {
         mCentralBodyPtr = ut::make_unique<ut::EarthWGS84>(); // default
      }
   }
   return *mCentralBodyPtr;
}
