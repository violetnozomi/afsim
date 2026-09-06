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

#include "BallisticPath.hpp"

#include "BallisticPlatforms.hpp"
#include "FiresTables.hpp"
#include "UtEarth.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMat3.hpp"
#include "UtMemory.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfAero.hpp"
#include "WsfDraw.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfPlatformTypes.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"
#include "WsfTrack.hpp"
#include "script/WsfScriptDefs.hpp"

namespace Fires
{

BallisticPath::BallisticPath(WsfSimulation* aSimPtr)
   : mSimPtr(aSimPtr)
   , mScenario(aSimPtr->GetScenario())
   , mFiresTablesPtr(nullptr)
   , mTargetState()
   , mType()
   , mMass(0.0)
   , mTargetAeroPtr(nullptr)
   , mIntegrationInterval(1.0)
   , mIsInitialized(false)
   , mUseFiresExtrapolation(false)
   , mImpactPoint()
   , mStartTime(0.0)
   , mApogeeTime(0.0)
   , mImpactTime(0.0)
   , mPoints()
{
   mFiresTablesPtr = &(FiresTables&)mScenario.GetExtension("wsf_fires");
   UtVec3d::Set(mLocWCS, 0.0);
   UtVec3d::Set(mVelWCS, 0.0);
   UtMat3d::Set(mWCS_ToNED_Transform, 0.0);
}

BallisticPath::BallisticPath(const BallisticPath& aSrc)
   : mSimPtr(aSrc.mSimPtr)
   , mScenario(aSrc.mScenario)
   , mFiresTablesPtr(aSrc.mFiresTablesPtr)
   , mTargetState()
   , mType(aSrc.mType)
   , mMass(aSrc.mMass)
   , mTargetAeroPtr(nullptr)
   , mIntegrationInterval(aSrc.mIntegrationInterval)
   , mIsInitialized(aSrc.mIsInitialized)
   , mUseFiresExtrapolation(aSrc.mUseFiresExtrapolation)
   , mImpactPoint(aSrc.mImpactPoint)
   , mStartTime(aSrc.mStartTime)
   , mApogeeTime(aSrc.mApogeeTime)
   , mImpactTime(aSrc.mImpactTime)
   , mPoints(aSrc.mPoints)
{
   UtVec3d::Set(mLocWCS, aSrc.mLocWCS);
   UtVec3d::Set(mVelWCS, aSrc.mVelWCS);
   UtMat3d::Set(mWCS_ToNED_Transform, aSrc.mWCS_ToNED_Transform);
   if (mTargetAeroPtr != nullptr)
   {
      mTargetAeroPtr = aSrc.mTargetAeroPtr->Clone();
   }
}


//! Update the trajectory using the given track.
//! @param aTrack The track used to update the trajectory.
bool BallisticPath::Update(const WsfTrack& aTrack)
{
   // Require valid location and velocity from the track.
   if ((!aTrack.LocationValid()) || (!aTrack.VelocityValid()))
   {
      return false;
   }

   double locWCS[3];
   double velWCS[3];

   aTrack.GetLocationWCS(locWCS);
   aTrack.GetVelocityWCS(velWCS);

   mUseFiresExtrapolation = false;

   if (aTrack.TypeIdValid() && mFiresTablesPtr->UseFiresMoverExtrapolation())
   {
      mType = aTrack.GetTypeId();
      if (mFiresTablesPtr->UseFiresMoverExtrapolation())
      {
         WsfPlatform* platformPtr = mSimPtr->GetPlatformByIndex(aTrack.GetTargetIndex());
         if (platformPtr != nullptr)
         {
            WsfMover* moverPtr     = platformPtr->GetMover();
            mUseFiresExtrapolation = ((moverPtr != nullptr) && (moverPtr->GetTypeId() == "WSF_FIRES_MOVER"));
            UpdateFires(aTrack.GetUpdateTime(), *(static_cast<Fires::FiresMover*>(moverPtr)), locWCS, velWCS);
         }
      }
   }
   if (!mUseFiresExtrapolation)
   {
      UpdateSpherical(aTrack.GetUpdateTime(), locWCS, velWCS);
   }
   return true;
}

//! Update the trajectory using the given platform.
void BallisticPath::Update(WsfPlatform* aPlatform)
{
   double locWCS[3];
   double velWCS[3];
   aPlatform->GetLocationWCS(locWCS);
   aPlatform->GetVelocityWCS(velWCS);
   UpdateSpherical(mSimPtr->GetSimTime(), locWCS, velWCS);
}

//! Draw the trajectory using the given WsfDraw object.
void BallisticPath::Draw(WsfDraw& aDraw) const
{
   // Draw the trajectory if requested.
   double tempLocWCS[3];
   aDraw.BeginPolyline();
   for (const auto& point : mPoints)
   {
      if (mUseFiresExtrapolation)
      {
         aDraw.VertexWCS(point.mLocWCS);
      }
      else
      {
         UtSphericalEarth::ConvertSphericalToEllipsoidal(point.mLocWCS, tempLocWCS);
         aDraw.VertexWCS(tempLocWCS);
      }
   }
   aDraw.End();
}

// =================================================================================================
BallisticPath::~BallisticPath()
{
   delete mTargetAeroPtr;
}

//! Return whether the given type is in the list of types
//! defined in the BallisticPlatforms class, using the "ballistic_types" input.
//!@param aType The platform type to check.
bool BallisticPath::IsTargetType(const std::string& aType)
{
   return mFiresTablesPtr->IsType(aType);
}

// =================================================================================================
// static
//! Find the ground range and altitude of the target with respect to some origin (the shooter).
void BallisticPath::GetRangeAndAltitude(const double aOriginLocWCS[3],
                                        const double aTargetLocWCS[3],
                                        double&      aRange,
                                        double&      aAltitude)
{
   double targetLocMag = UtVec3d::Magnitude(aTargetLocWCS);
   aRange = acos(UtVec3d::DotProduct(aTargetLocWCS, aOriginLocWCS) / (targetLocMag * UtVec3d::Magnitude(aOriginLocWCS))) *
            UtSphericalEarth::cEARTH_RADIUS;
   aAltitude = targetLocMag - UtSphericalEarth::cEARTH_RADIUS;
}

// =================================================================================================
//! Return the ellipsoidal WCS location of the target at the specified time.
//! @param aSimTime The time for which the target position is to be returned.
//! @param aLocWCS The spherical WCS location of the target at 'aSimTime'.
bool BallisticPath::GetLocationWCS(double aSimTime, double aLocWCS[3]) const
{
   bool ok = IsValid();
   if (ok)
   {
      int index = static_cast<int>((aSimTime - mStartTime) / mIntegrationInterval);
      assert(mPoints.size() > 0);
      int lastIndex = static_cast<int>(mPoints.size() - 1);
      if (index < 0)
      {
         // Requested time is before the first interval
         ok = false;
         UtSphericalEarth::ConvertSphericalToEllipsoidal(mPoints[0].mLocWCS, aLocWCS);
      }
      else if (index >= lastIndex)
      {
         // Requested time after the last interval.
         ok = (index > lastIndex);
         UtSphericalEarth::ConvertSphericalToEllipsoidal(mPoints[lastIndex].mLocWCS, aLocWCS);
      }
      else
      {
         // Requested time is within a valid interval.
         double frac = ((aSimTime - mStartTime) - (index * mIntegrationInterval)) / mIntegrationInterval;
         double diffLocWCS[3];
         UtVec3d::Subtract(diffLocWCS, mPoints[index + 1].mLocWCS, mPoints[index].mLocWCS);
         UtVec3d::AddProduct(aLocWCS, mPoints[index].mLocWCS, frac, diffLocWCS);
         UtSphericalEarth::ConvertSphericalToEllipsoidal(aLocWCS, aLocWCS);
      }
   }
   return ok;
}

// =================================================================================================
//! Return the SPHERICAL WCS location of the target at the specified time.
//! @param aSimTime The time for which the target position is to be returned.
//! @param aVelWCS The spherical WCS location of the target at 'aSimTime'.
bool BallisticPath::GetVelocityWCS(double aSimTime, double aVelWCS[3]) const
{
   bool ok        = true;
   int  index     = static_cast<int>((aSimTime - mStartTime) / mIntegrationInterval);
   int  lastIndex = static_cast<int>(mPoints.size() - 1);
   if (index < 0)
   {
      // Requested time is before the first interval
      ok = false;
      UtVec3d::Set(aVelWCS, mPoints[0].mVelWCS);
   }
   else if (index >= lastIndex)
   {
      // Requested time after the last interval.
      ok = (index > lastIndex);
      UtVec3d::Set(aVelWCS, mPoints[lastIndex].mVelWCS);
   }
   else
   {
      // Requested time is within a valid interval.
      double frac = ((aSimTime - mStartTime) - (index * mIntegrationInterval)) / mIntegrationInterval;
      double diffVelWCS[3];
      UtVec3d::Subtract(diffVelWCS, mPoints[index + 1].mVelWCS, mPoints[index].mVelWCS);
      UtVec3d::AddProduct(aVelWCS, mPoints[index].mVelWCS, frac, diffVelWCS);
   }
   return ok;
}

// private
void BallisticPath::UpdateSpherical(double aTime, const double aLocWCS[3], const double aVelWCS[3])
{
   mTargetState.SetLocationWCS(aLocWCS);
   mTargetState.SetVelocityWCS(aVelWCS);

   // Integration is internally done in spherical Earth coordinates to be consistent with WsfGuidedMover.

   double lat, lon, alt;
   mTargetState.GetLocationLLA(lat, lon, alt);
   UtSphericalEarth::ComputeNEDTransform(lat, lon, alt, mWCS_ToNED_Transform, mLocWCS);

   // The velocity vector must first be converted into an NED frame and then converted to the spherical frame.

   double velNED[3];
   mTargetState.GetVelocityNED(velNED);
   // Rotate the vector in the NED to the WCS frame
   UtMat3d::InverseTransform(mVelWCS, mWCS_ToNED_Transform, velNED);

   std::map<WsfStringId, WsfStringId>::iterator iter = mFiresTablesPtr->AeroMap().find(mType);
   if (iter != mFiresTablesPtr->AeroMap().end())
   {
      // WsfAero* prototypePtr = WsfAero::FindType(WsfStringId::GetString(iter->second));
      WsfAero* prototypePtr = dynamic_cast<WsfAero*>(mScenario.FindType("aero", iter->second));
      if (prototypePtr != nullptr)
      {
         if (mTargetAeroPtr != nullptr)
         {
            delete mTargetAeroPtr;
         }
         mTargetAeroPtr = prototypePtr->Clone();
         mTargetAeroPtr->Initialize(mStartTime, &mTargetState);

         mMass = mFiresTablesPtr->MassMap()[mType];
      }
      else
      {
         auto logWarning = ut::log::warning() << "BallisticPath::BallisticPath: Could not find aero object of Type.";
         logWarning.AddNote() << "Type: " << mType;
      }
   }


   // Compute the entire trajectory.
   //
   // The trajectory is first stored in a vector whose capacity has been set to be large (about 1800 seconds
   // of flight). The result is copied to a permanent vector to eliminate any usused entries. This should be
   // faster because it eliminated reallocations.

   std::vector<Point> points;
   size_t             initialCapacity = static_cast<size_t>(1800.0 / mIntegrationInterval);
   points.reserve(initialCapacity);

   double altitude       = UtVec3d::Magnitude(mLocWCS) - UtSphericalEarth::cEARTH_RADIUS;
   double apogeeAltitude = altitude;
   mApogeeTime           = mStartTime;
   mImpactTime           = mStartTime;

   Point point;
   // Store the initial point.
   UtVec3d::Set(point.mLocWCS, mLocWCS);
   UtVec3d::Set(point.mVelWCS, mVelWCS);
   points.push_back(point);

   while (altitude >= 0.0)
   {
      mImpactTime += mIntegrationInterval;

      Integrate(mImpactTime, mLocWCS, mVelWCS);

      // The ellipsoidal earth location and velocity is needed for aero computations.
      double lat, lon;

      UtSphericalEarth::ConvertECEFToLLA(mLocWCS, lat, lon, altitude);
      double WCS_ToNED_Transform[3][3];
      double locWCS[3];
      UtSphericalEarth::ComputeNEDTransform(lat, lon, altitude, WCS_ToNED_Transform, locWCS);
      double velNED[3];
      UtSphericalEarth::ConvertECEFToLocal(WCS_ToNED_Transform, mVelWCS, velNED);
      mTargetState.SetLocationLLA(lat, lon, altitude);
      mTargetState.SetVelocityNED(velNED);

      if (points.size() >= 2)
      {
         if (WillImpact(altitude))
         {
            break;
         }
      }

      UtVec3d::Set(point.mLocWCS, mLocWCS);
      UtVec3d::Set(point.mVelWCS, mVelWCS);
      points.push_back(point);
      if (altitude > apogeeAltitude)
      {
         apogeeAltitude = altitude;
         mApogeeTime    = mImpactTime;
      }
   }

   mPoints = points; // Copy to permanent vector, eliminating unused space

   // Set 'mImpactTime' to the approximate time when the target passes through the surface.
   // (The integration goes one step past).

   size_t n = mPoints.size();
   if (n >= 2)
   {
      double alt1, alt2;
      alt1 = UtVec3d::Magnitude(mPoints[n - 2].mLocWCS) - UtSphericalEarth::cEARTH_RADIUS;
      alt2 = UtVec3d::Magnitude(mPoints[n - 1].mLocWCS) - UtSphericalEarth::cEARTH_RADIUS;

      ComputeImpact(alt1, alt2);
   }
}

// private
void BallisticPath::UpdateFires(double aTime, FiresMover& aFiresMover, const double aLocWCS[3], const double aVelWCS[3])
{
   UtVec3d::Set(mLocWCS, aLocWCS);
   UtVec3d::Set(mVelWCS, aVelWCS);

   // Compute the entire trajectory.
   //
   // The trajectory is first stored in a vector whose capacity has been set to be large (about 1800 seconds
   // of flight). The result is copied to a permanent vector to eliminate any usused entries. This should be
   // faster because it eliminated reallocations.

   std::vector<Point> points;
   size_t             initialCapacity = static_cast<size_t>(1800.0 / mIntegrationInterval);
   points.reserve(initialCapacity);
   mTargetState.SetLocationWCS(mLocWCS);
   double lat, lon, altitude;
   mTargetState.GetLocationLLA(lat, lon, altitude);

   double apogeeAltitude = altitude;
   mApogeeTime           = mStartTime;
   mImpactTime           = mStartTime;

   Point point;
   // Store the initial point.
   UtVec3d::Set(point.mLocWCS, mLocWCS);
   UtVec3d::Set(point.mVelWCS, mVelWCS);
   points.push_back(point);

   while (altitude >= 0.0)
   {
      mImpactTime += mIntegrationInterval;

      aFiresMover.GetState(mImpactTime, mLocWCS, mVelWCS);

      double lat, lon;
      UtEntity::ConvertWCSToLLA(mLocWCS, lat, lon, altitude);

      if (points.size() >= 2)
      {
         if (WillImpact(altitude))
         {
            break;
         }
      }

      UtVec3d::Set(point.mLocWCS, mLocWCS);
      UtVec3d::Set(point.mVelWCS, mVelWCS);
      points.push_back(point);
      if (altitude > apogeeAltitude)
      {
         apogeeAltitude = altitude;
         mApogeeTime    = mImpactTime;
      }
   }
   mPoints = points; // Copy to permanent vector, eliminating unused space

   // Set 'mImpactTime' to the approximate time when the target passes through the surface.
   // (The integration goes one step past).

   size_t n = mPoints.size();
   if (n >= 2)
   {
      // TODO Refactor this into a factory method (one for "default" Wsf path, another for fires path.
      double alt1, alt2;
      double lat, lon;
      UtEntity::ConvertWCSToLLA(mPoints[n - 2].mLocWCS, lat, lon, alt1);
      UtEntity::ConvertWCSToLLA(mPoints[n - 1].mLocWCS, lat, lon, alt2);

      ComputeImpact(alt1, alt2);
   }
}


// =================================================================================================
// protected
void BallisticPath::Integrate(double aElapsedTime, double aLocWCS[3], double aVelWCS[3])
{
   // Compute the drag force, if available
   double drag = 0.0;
   if (mTargetAeroPtr != nullptr)
   {
      mTargetAeroPtr->Update(0.0);
      drag = mTargetAeroPtr->GetDrag();
   }

   // Use two-stage Runge-Kutta integration.

   double locWCS_1[3];
   double locWCS_2[3];
   double velWCS_1[3];
   double velWCS_2[3];
   double aclWCS_1[3];
   double aclWCS_2[3];
   double dragWCS[3];
   double r;
   double g;

   // Use derivatives at t to get position and velocity at t + dt/2.

   UtVec3d::Set(locWCS_1, aLocWCS);
   UtVec3d::Set(velWCS_1, aVelWCS);

   // Compute the derivatives at t.

   UtVec3d::Set(aclWCS_1, locWCS_1);
   r = UtVec3d::Magnitude(aclWCS_1);
   g = UtEarth::cGRAVITATIONAL_PARAMETER / (r * r);
   UtVec3d::Multiply(aclWCS_1, -g / r);
   if (drag > 0.0)
   {
      // Drag is opposite the velocity vector
      UtVec3d::Set(dragWCS, velWCS_1);
      double speed = UtVec3d::Magnitude(velWCS_1);
      UtVec3d::Multiply(dragWCS, -drag / speed / mMass);
      UtVec3d::Add(aclWCS_1, aclWCS_1, dragWCS);
   }

   UtVec3d::AddProduct(locWCS_2, locWCS_1, 0.5 * mIntegrationInterval, velWCS_1);
   UtVec3d::AddProduct(velWCS_2, velWCS_1, 0.5 * mIntegrationInterval, aclWCS_1);

   // Compute derivatives at t + dt/2

   UtVec3d::Set(aclWCS_2, locWCS_2);
   r = UtVec3d::Magnitude(aclWCS_2);
   g = UtEarth::cGRAVITATIONAL_PARAMETER / (r * r);
   UtVec3d::Multiply(aclWCS_2, -g / r);
   if (drag > 0.0)
   {
      // Drag is opposite the velocity vector
      UtVec3d::Set(dragWCS, velWCS_2);
      double speed = UtVec3d::Magnitude(velWCS_2);
      UtVec3d::Multiply(dragWCS, -drag / speed / mMass);
      UtVec3d::Add(aclWCS_2, aclWCS_2, dragWCS);
   }

   // Use derivatives at t + dt/2 to get position and velocity at t + dt.

   UtVec3d::AddProduct(aLocWCS, locWCS_1, mIntegrationInterval, velWCS_2);
   UtVec3d::AddProduct(aVelWCS, velWCS_1, mIntegrationInterval, aclWCS_2);
}

bool BallisticPath::WillImpact(double aAltitude)
{
   bool willImpact = false; // above 0 msl
   if (mSimPtr->GetTerrainInterface()->IsEnabled())
   {
      float  elev;
      double impactLat, impactLon, impactAlt;
      UtEntity::ConvertWCSToLLA(mLocWCS, impactLat, impactLon, impactAlt);
      mSimPtr->GetTerrainInterface()->GetGeodeticElev(impactLat, impactLon, elev);
      double dotProduct = UtVec3d::DotProduct(mLocWCS, mVelWCS);
      willImpact        = ((aAltitude <= elev) && (dotProduct < 0.0));
   }
   return willImpact;
}

void BallisticPath::ComputeImpact(double aAlt1, double aAlt2)
{
   double frac = (0.0 - aAlt1) / (aAlt2 - aAlt1);
   double t2   = mImpactTime;
   double t1   = mImpactTime - mIntegrationInterval;
   mImpactTime = t1 + frac * (t2 - t1);

   mIsInitialized = true;

   // Find impact location
   double locWCS[3];
   GetLocationWCS(mImpactTime, locWCS);
   double impactLat, impactLon, impactAlt;
   UtEntity::ConvertWCSToLLA(locWCS, impactLat, impactLon, impactAlt);

   if (mSimPtr->GetTerrainInterface()->IsEnabled())
   {
      float        elev;
      wsf::Terrain terrain(mSimPtr->GetTerrainInterface());
      terrain.GetElevApprox(impactLat, impactLon, elev);
      impactAlt = elev;
   }
   else
   {
      impactAlt = 0.0;
   }
   mImpactPoint = WsfGeoPoint(impactLat, impactLon, impactAlt);
}

// Script Class
ScriptBallisticPathClass::ScriptBallisticPathClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   mConstructible = true;
   mCloneable     = true;

   SetClassName("WsfBallisticPath");

   // Add each of the method objects to the class.

   AddMethod(ut::make_unique<Update>());
   AddMethod(ut::make_unique<Draw>());
   AddMethod(ut::make_unique<HasData>());
   AddStaticMethod(ut::make_unique<IsBallisticType>());
   AddMethod(ut::make_unique<ImpactPoint>());
   AddMethod(ut::make_unique<ImpactTime>());
   AddMethod(ut::make_unique<PointAtTime>());
}

ScriptBallisticPathClass::~ScriptBallisticPathClass() {}

// virtual
void* ScriptBallisticPathClass::Create(const UtScriptContext& aContext)
{
   return new BallisticPath(SIMULATION);
}

// virtual
void* ScriptBallisticPathClass::Clone(void* aObjectPtr)
{
   return new BallisticPath(*static_cast<BallisticPath*>(aObjectPtr));
}

// virtual
void ScriptBallisticPathClass::Destroy(void* aObjectPtr)
{
   delete static_cast<BallisticPath*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScriptBallisticPathClass, BallisticPath, Update, 1, "bool", "WsfTrack")
{
   WsfTrack* trackPtr = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   bool      updated  = aObjectPtr->Update(*trackPtr);
   aReturnVal.SetBool(updated);
}

UT_DEFINE_SCRIPT_METHOD(ScriptBallisticPathClass, BallisticPath, HasData, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsValid());
}

UT_DEFINE_SCRIPT_METHOD(ScriptBallisticPathClass, BallisticPath, Draw, 1, "void", "WsfDraw")
{
   WsfDraw* drawPtr = static_cast<WsfDraw*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->Draw(*drawPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScriptBallisticPathClass, BallisticPath, IsBallisticType, 1, "bool", "string")
{
   aReturnVal.SetBool(aObjectPtr->IsTargetType(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(ScriptBallisticPathClass, BallisticPath, ImpactPoint, 0, "WsfGeoPoint", "")
{
   WsfGeoPoint* impactPointPtr = new WsfGeoPoint(aObjectPtr->GetImpactPoint());
   aReturnVal.SetPointer(new UtScriptRef(impactPointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(ScriptBallisticPathClass, BallisticPath, ImpactTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetImpactTime());
}

UT_DEFINE_SCRIPT_METHOD(ScriptBallisticPathClass, BallisticPath, PointAtTime, 1, "WsfGeoPoint", "double")
{
   double wcs[3];
   double time = aVarArgs[0].GetDouble();
   aObjectPtr->GetLocationWCS(time, wcs);
   WsfGeoPoint* pointPtr = new WsfGeoPoint(wcs);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

} // namespace Fires
