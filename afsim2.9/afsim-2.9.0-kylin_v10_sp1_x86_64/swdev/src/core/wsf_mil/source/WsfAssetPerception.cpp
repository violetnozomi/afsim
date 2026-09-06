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

#include "WsfAssetPerception.hpp"

#include <string>

#include "UtEntity.hpp"
#include "UtMemory.hpp"
#include "UtScriptVec3.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfIntercept.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfRF_Jammer.hpp"
#include "WsfSensor.hpp"
#include "WsfTaskResource.hpp"
#include "WsfUtil.hpp"
#include "WsfWaypoint.hpp"
#include "WsfWeapon.hpp"
#include "WsfWeaponTaskResource.hpp"
#include "script/WsfScriptContext.hpp"

namespace
{
std::string EnumToString(unsigned int aEnum)
{
   std::string kind("none");
   switch (aEnum)
   {
   case cTASK_RESOURCE_TYPE_SENSOR:
   {
      kind = "sensor";
   }
   break;
   case cTASK_RESOURCE_TYPE_WEAPON:
   {
      kind = "weapon";
   }
   break;
   case cTASK_RESOURCE_TYPE_JAMMER:
   {
      kind = "jammer";
   }
   break;
   case cTASK_RESOURCE_TYPE_PROCESSOR:
   {
      kind = "processor";
   }
   break;
   case cTASK_RESOURCE_TYPE_UPLINK:
   {
      kind = "uplink";
   }
   break;
   case cTASK_RESOURCE_TYPE_NONE:
   default:
   {
      kind = "none";
   }
   break;
   };
   return kind;
}
} // namespace

WsfAssetPerception::AssetSystem::AssetSystem() {}

WsfAssetPerception::AssetSystem::AssetSystem(WsfSensor& aSensor)
   : mSystemKind(cTASK_RESOURCE_TYPE_SENSOR)
   , mSystemType(aSensor.GetTypeId())
   , mSystemName(aSensor.GetNameId())
{
   // Indicates the number of explicit cued-search or tracking requests the sensor can still support
   // These counts are for all modes on the sensor
   mReadyAssignment = (aSensor.GetMaximumRequestCount() - aSensor.GetActiveRequestCount());

   // For sensors, quantity remaining does not quite make sense so set it equal
   // to number of available requests (ready assignments)
   mQuantityRemaining = static_cast<double>(mReadyAssignment);
}

WsfAssetPerception::AssetSystem::AssetSystem(WsfWeapon& aWeapon)
{
   mSystemType = aWeapon.GetTypeId();
   mSystemName = aWeapon.GetNameId();

   if ((aWeapon.GetEM_XmtrCount() != 0) || aWeapon.IsA_TypeOf(UtStringIdLiteral("WSF_RF_JAMMER")))
   {
      mSystemKind = cTASK_RESOURCE_TYPE_JAMMER;
      // StartJamming commands check number of available spots to the maximum number of spots
      // for the current mode! This is different than the sensor so sum up available spots across all modes
      // This is calculated by the GetQuantityRemaining override in the jammer
      // it just takes the GetMaximumRequestCount() - GetActiveRequestCount
      mReadyAssignment = 0U;
      for (size_t i = 0; i < aWeapon.GetModeCount(); ++i)
      {
         auto modePtr = dynamic_cast<WsfRF_Jammer::JammerMode*>(aWeapon.GetModeEntry(i));
         mReadyAssignment +=
            (modePtr->GetMaximumXmtrSpotCount() - ut::safe_cast<unsigned int>(modePtr->GetActiveXmtrSpotCount()));
      }

      // For jammers, quantity remaining does not quite make sense so set it equal
      // to number of available requests (ready assignments)
      mQuantityRemaining = static_cast<double>(mReadyAssignment);
   }
   else
   {
      mSystemKind = cTASK_RESOURCE_TYPE_WEAPON;

      // Indicates the number of firing requests still available
      // The maximum number of FireSalvo calls that may be in process at a time
      // minus the number of FireSalvo calls that are active
      mReadyAssignment = (aWeapon.GetMaximumRequestCount() - aWeapon.GetActiveRequestCount());

      // If a weapon is reloading and it can't shoot while reloading then it is not available
      if (aWeapon.IsReloading() && aWeapon.InhibitWhileReloading())
      {
         mReadyAssignment = 0;
      }

      // Number of weapons remaining
      mQuantityRemaining = aWeapon.GetQuantityRemaining();
   }
}

WsfAssetPerception::AssetSystem::AssetSystem(AssetSystem& aSrc, unsigned int aReady, double aQuantity)
   : mSystemKind(aSrc.mSystemKind)
   , mSystemType(aSrc.mSystemType)
   , mSystemName(aSrc.mSystemName)
   , mReadyAssignment(aReady)
   , mQuantityRemaining(aQuantity)
{
}

WsfAssetPerception::WsfAssetPerception()
   : WsfObject()
   , mIndex((unsigned int)2147483647) // max int value?
   , mTime(0.0)
   , mSystems()
{
   mLocationWCS[0] = mLocationWCS[1] = mLocationWCS[2] = 0.0;
   mVelocityWCS[0] = mVelocityWCS[1] = mVelocityWCS[2] = 0.0;
   mOrientationNED[0] = mOrientationNED[1] = mOrientationNED[2] = 0.0;
}

WsfAssetPerception::WsfAssetPerception(const WsfAssetPerception& aSrc)
   : WsfObject(aSrc)
   , mIndex(aSrc.mIndex)
   , mTime(aSrc.mTime)
   , mSystems(aSrc.mSystems)
{
   mLocationWCS[0]    = aSrc.mLocationWCS[0];
   mLocationWCS[1]    = aSrc.mLocationWCS[1];
   mLocationWCS[2]    = aSrc.mLocationWCS[2];
   mVelocityWCS[0]    = aSrc.mVelocityWCS[0];
   mVelocityWCS[1]    = aSrc.mVelocityWCS[1];
   mVelocityWCS[2]    = aSrc.mVelocityWCS[2];
   mOrientationNED[0] = aSrc.mOrientationNED[0];
   mOrientationNED[1] = aSrc.mOrientationNED[1];
   mOrientationNED[2] = aSrc.mOrientationNED[2];
}

WsfAssetPerception::WsfAssetPerception(const WsfAssetPerception& aSrc, const AssetSystem& aSys)
   : WsfObject(aSrc)
   , mIndex(aSrc.mIndex)
   , mTime(aSrc.mTime)
{
   mLocationWCS[0]    = aSrc.mLocationWCS[0];
   mLocationWCS[1]    = aSrc.mLocationWCS[1];
   mLocationWCS[2]    = aSrc.mLocationWCS[2];
   mVelocityWCS[0]    = aSrc.mVelocityWCS[0];
   mVelocityWCS[1]    = aSrc.mVelocityWCS[1];
   mVelocityWCS[2]    = aSrc.mVelocityWCS[2];
   mOrientationNED[0] = aSrc.mOrientationNED[0];
   mOrientationNED[1] = aSrc.mOrientationNED[1];
   mOrientationNED[2] = aSrc.mOrientationNED[2];
   // only copy this one system into new perception
   mSystems.emplace_back(aSys);
}

WsfAssetPerception::WsfAssetPerception(double aSimTime, WsfPlatform& aPlatform)
   : WsfObject(aPlatform)
{
   aPlatform.Update(aSimTime);
   mIndex = aPlatform.GetIndex();
   mTime  = aSimTime;

   aPlatform.GetLocationWCS(mLocationWCS);
   aPlatform.GetVelocityWCS(mVelocityWCS);
   aPlatform.GetOrientationNED(mOrientationNED[0], mOrientationNED[1], mOrientationNED[2]);

   for (WsfComponentList::RoleIterator<WsfWeapon> iter(aPlatform); !iter.AtEnd(); ++iter)
   {
      mSystems.emplace_back(**iter);
   }
   for (WsfComponentList::RoleIterator<WsfSensor> iter(aPlatform); !iter.AtEnd(); ++iter)
   {
      mSystems.emplace_back(**iter);
   }
}

bool WsfAssetPerception::operator==(const WsfAssetPerception& aRhs) const
{
   if (mIndex == aRhs.mIndex)
   {
      if (mSystems.size() == aRhs.mSystems.size())
      {
         if (mSystems.empty() || (mSystems[0].mSystemKind == aRhs.mSystems[0].mSystemKind &&
                                  mSystems[0].mSystemName == aRhs.mSystems[0].mSystemName))
         {
            return true;
         }
      }
   }
   return false;
}

WsfSortedPerception::WsfSortedPerception(WsfPlatform* aPlatform, WsfPlatform* aReference)
   : mPlatformPtr(aPlatform)
   , mTrackPtr(nullptr)
   , mAssetPtr(nullptr)
{
   double platformWCS[3];
   aPlatform->GetLocationWCS(platformWCS);
   double relativeWCS[3];
   aReference->GetRelativeLocationWCS(platformWCS, relativeWCS);
   mDistance = UtVec3d::Magnitude(relativeWCS);
}

WsfSortedPerception::WsfSortedPerception(WsfLocalTrack* aTrack, WsfPlatform* aReference)
   : mPlatformPtr(nullptr)
   , mTrackPtr(aTrack)
   , mAssetPtr(nullptr)
{
   double trackWCS[3];
   aTrack->GetLocationWCS(trackWCS);
   double relativeWCS[3];
   aReference->GetRelativeLocationWCS(trackWCS, relativeWCS);
   mDistance = UtVec3d::Magnitude(relativeWCS);
}

WsfSortedPerception::WsfSortedPerception(WsfAssetPerception* aAsset, WsfPlatform* aReference)
   : mPlatformPtr(nullptr)
   , mTrackPtr(nullptr)
   , mAssetPtr(aAsset)
{
   double relativeWCS[3];
   aReference->GetRelativeLocationWCS(aAsset->mLocationWCS, relativeWCS);
   mDistance = UtVec3d::Magnitude(relativeWCS);
}

WsfSortedPerception::WsfSortedPerception(WsfPlatform* aPlatform, double distance)
   : mDistance(distance)
   , mPlatformPtr(aPlatform)
   , mTrackPtr(nullptr)
   , mAssetPtr(nullptr)
{
}

WsfSortedPerception::WsfSortedPerception(WsfLocalTrack* aTrack, double distance)
   : mDistance(distance)
   , mPlatformPtr(nullptr)
   , mTrackPtr(aTrack)
   , mAssetPtr(nullptr)
{
}

WsfSortedPerception::WsfSortedPerception(WsfAssetPerception* aAsset, double distance)
   : mDistance(distance)
   , mPlatformPtr(nullptr)
   , mTrackPtr(nullptr)
   , mAssetPtr(aAsset)
{
}

WsfScriptAssetPerceptionClass::WsfScriptAssetPerceptionClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptObjectClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfAssetPerception");
   mConstructible      = true;
   mCloneable          = true;
   mLessThanComparable = true;

   AddMethod(ut::make_unique<Index>());
   AddMethod(ut::make_unique<Time>());

   AddMethod(ut::make_unique<Location>());
   AddMethod(ut::make_unique<VelocityWCS>());
   AddMethod(ut::make_unique<VelocityNED>());
   AddMethod(ut::make_unique<Speed>());
   AddMethod(ut::make_unique<OrientationNED>());
   AddMethod(ut::make_unique<OrientationWCS>());
   AddMethod(ut::make_unique<Heading>());
   AddMethod(ut::make_unique<Pitch>());
   AddMethod(ut::make_unique<Roll>());

   AddMethod(ut::make_unique<SystemCount>());
   AddMethod(ut::make_unique<SystemKind>());
   AddMethod(ut::make_unique<SystemType>());
   AddMethod(ut::make_unique<SystemName>());
   AddMethod(ut::make_unique<SystemReadyAssignment>());
   AddMethod(ut::make_unique<SystemQuantityRemaining>());

   AddMethod(ut::make_unique<InterceptLocation>()); // double InterceptLocation(WsfTrack, WsfWaypoint)
   AddMethod(ut::make_unique<TrueBearingTo>());     // double TrueBearingTo(WsfGeoPoint)
   AddMethod(ut::make_unique<RelativeBearingTo>()); // double RelativeBearingTo(WsfGeoPoint)
   AddMethod(ut::make_unique<ClosestApproachOf>()); // WsfGeoPoint ClosestApproachOf(WsfTrack)
   AddMethod(ut::make_unique<ClosingSpeedOf>());    // double ClosingSpeedOf(WsfTrack)
}

// virtual
void* WsfScriptAssetPerceptionClass::Create(const UtScriptContext& /*aContext*/)
{
   return new WsfAssetPerception();
}

// virtual
void* WsfScriptAssetPerceptionClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfAssetPerception*>(aObjectPtr)->Clone();
}

// virtual
void WsfScriptAssetPerceptionClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfAssetPerception*>(aObjectPtr);
}

// virtual
std::string WsfScriptAssetPerceptionClass::ToString(void* aObjectPtr) const
{
   std::stringstream ss;
   ss << GetClassName() << "(";
   if (aObjectPtr)
   {
      auto assetPerceptionPtr = static_cast<WsfAssetPerception*>(aObjectPtr);
      ss << "Name: " << assetPerceptionPtr->GetName();
      ss << "; Type: " << assetPerceptionPtr->GetType();
      ss << "; Systems: " << assetPerceptionPtr->mSystems.size();
      ss << " { ";
      for (size_t i = 0; i < assetPerceptionPtr->mSystems.size(); ++i)
      {
         if (i != 0)
         {
            ss << ", ";
         }
         ss << EnumToString(assetPerceptionPtr->mSystems[i].mSystemKind) << "|"
            << assetPerceptionPtr->mSystems[i].mSystemName;
      }
      ss << " }";
   }
   else
   {
      ss << "null";
   }
   ss << ')';
   return ss.str();
}

// virtual
bool WsfScriptAssetPerceptionClass::LessThan(void* aLHS, void* aRHS)
{
   auto* LHS = static_cast<WsfAssetPerception*>(aLHS);
   auto* RHS = static_cast<WsfAssetPerception*>(aRHS);
   return *LHS < *RHS;
}

// virtual
void WsfScriptAssetPerceptionClass::OnNewScriptRef(UtScriptRef& aReference)
{
   if (aReference.GetMemManagement() != UtScriptRef::cMANAGE)
   {
      auto assetPtr = aReference.GetAppObject<WsfAssetPerception>();
      if (assetPtr != nullptr)
      {
         aReference.SetExternalReference(assetPtr->GetReferenceCount());
      }
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetPerceptionClass, WsfAssetPerception, Index, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->mIndex)); // truth - simulation index
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetPerceptionClass, WsfAssetPerception, Time, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->mTime); // sec
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetPerceptionClass, WsfAssetPerception, Location, 0, "WsfGeoPoint", "")
{
   WsfGeoPoint* pointPtr = new WsfGeoPoint(aObjectPtr->mLocationWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetPerceptionClass, WsfAssetPerception, VelocityWCS, 0, "Vec3", "")
{
   UtVec3d xyz(aObjectPtr->mVelocityWCS);
   aReturnVal.SetPointer(UtScriptVec3::Create(xyz));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetPerceptionClass, WsfAssetPerception, VelocityNED, 0, "Vec3", "")
{
   UtEntity temp;
   temp.SetLocationWCS(aObjectPtr->mLocationWCS);
   double velocityNED[3];
   temp.ConvertWCSVectorToNED(velocityNED, aObjectPtr->mVelocityWCS);
   UtVec3d ned(velocityNED);
   aReturnVal.SetPointer(UtScriptVec3::Create(ned));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetPerceptionClass, WsfAssetPerception, Speed, 0, "double", "")
{
   aReturnVal.SetDouble(UtVec3d::Magnitude(aObjectPtr->mVelocityWCS));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetPerceptionClass, WsfAssetPerception, OrientationNED, 0, "Vec3", "")
{
   UtVec3d ned(aObjectPtr->mOrientationNED);
   ned[0] *= UtMath::cDEG_PER_RAD;
   ned[1] *= UtMath::cDEG_PER_RAD;
   ned[2] *= UtMath::cDEG_PER_RAD;
   aReturnVal.SetPointer(UtScriptVec3::Create(ned));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetPerceptionClass, WsfAssetPerception, OrientationWCS, 0, "Vec3", "")
{
   UtEntity temp;
   temp.SetLocationWCS(aObjectPtr->mLocationWCS);
   temp.SetOrientationNED(aObjectPtr->mOrientationNED[0], aObjectPtr->mOrientationNED[1], aObjectPtr->mOrientationNED[2]);
   UtVec3d wcs;
   temp.GetOrientationWCS(wcs[0], wcs[1], wcs[2]);
   wcs[0] *= UtMath::cDEG_PER_RAD;
   wcs[1] *= UtMath::cDEG_PER_RAD;
   wcs[2] *= UtMath::cDEG_PER_RAD;
   aReturnVal.SetPointer(UtScriptVec3::Create(wcs));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetPerceptionClass, WsfAssetPerception, Heading, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->mOrientationNED[0] * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetPerceptionClass, WsfAssetPerception, Pitch, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->mOrientationNED[1] * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetPerceptionClass, WsfAssetPerception, Roll, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->mOrientationNED[2] * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetPerceptionClass, WsfAssetPerception, SystemCount, 0, "int", "")
{
   aReturnVal.SetInt((int)aObjectPtr->mSystems.size());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetPerceptionClass, WsfAssetPerception, SystemKind, 1, "string", "int")
{
   std::string kind("none");
   int         index = aVarArgs[0].GetInt();
   if ((int)aObjectPtr->mSystems.size() > index)
   {
      kind = EnumToString(aObjectPtr->mSystems[index].mSystemKind);
   }
   aReturnVal.SetString(kind);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetPerceptionClass, WsfAssetPerception, SystemType, 1, "string", "int")
{
   int index = aVarArgs[0].GetInt();
   if ((int)aObjectPtr->mSystems.size() > index)
   {
      aReturnVal.SetString(aObjectPtr->mSystems[index].mSystemType);
   }
   else
   {
      aReturnVal.SetString(""); // empty for now?
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetPerceptionClass, WsfAssetPerception, SystemName, 1, "string", "int")
{
   int index = aVarArgs[0].GetInt();
   if ((int)aObjectPtr->mSystems.size() > index)
   {
      aReturnVal.SetString(aObjectPtr->mSystems[index].mSystemName);
   }
   else
   {
      aReturnVal.SetString(""); // empty for now?
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetPerceptionClass, WsfAssetPerception, SystemReadyAssignment, 1, "int", "int")
{
   int index = aVarArgs[0].GetInt();
   int val   = 0;
   if ((int)aObjectPtr->mSystems.size() > index)
   {
      val = ut::safe_cast<int>(aObjectPtr->mSystems[index].mReadyAssignment);
   }
   aReturnVal.SetInt(val);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetPerceptionClass, WsfAssetPerception, SystemQuantityRemaining, 1, "double", "int")
{
   int    index = aVarArgs[0].GetInt();
   double val   = 0.0;
   if ((int)aObjectPtr->mSystems.size() > index)
   {
      val = aObjectPtr->mSystems[index].mQuantityRemaining;
   }
   aReturnVal.SetDouble(val);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetPerceptionClass, WsfAssetPerception, InterceptLocation, 2, "double", "WsfTrack, WsfWaypoint")
{
   // Argument 1: Target track <input>
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   // Argument 2: Intercept way-point <output>
   WsfWaypoint* waypointPtr = (WsfWaypoint*)aVarArgs[1].GetPointer()->GetAppObject();

   double timeToIntercept = -1.0;
   if ((trackPtr != nullptr) && (waypointPtr != nullptr))
   {
      double simTime = WsfScriptContext::GetTIME_NOW(aContext);

      // Get target location in WCS
      double tgtLocWCS[3];
      trackPtr->GetExtrapolatedLocationWCS(simTime, tgtLocWCS);

      // Get target velocity in WCS
      double tgtVelWCS[3];
      trackPtr->GetVelocityWCS(tgtVelWCS);

      // weapon location in WCS = aObjectPtr->mLocationWCS
      // Compute the intercept time and location.
      double intLocWCS[3];
      timeToIntercept = WsfIntercept::Intercept(aObjectPtr->mLocationWCS,
                                                UtVec3d::Magnitude(aObjectPtr->mVelocityWCS),
                                                tgtLocWCS,
                                                tgtVelWCS,
                                                intLocWCS);

      // Convert the intercept location to spherical coordinates and stuff in the way-point.
      double intLat;
      double intLon;
      double intAlt;
      WsfPlatform::ConvertWCSToLLA(intLocWCS, intLat, intLon, intAlt);
      waypointPtr->SetLat(intLat);
      waypointPtr->SetLon(intLon);

      // Also compute the 2D heading to the intercept point as some find it useful.
      double   intLocNED[3];
      UtEntity temp;
      temp.SetLocationWCS(aObjectPtr->mLocationWCS);
      temp.ConvertWCSToNED(intLocWCS, intLocNED);

      waypointPtr->SetHeading(atan2(intLocNED[1], intLocNED[0]));
      waypointPtr->SetEndOfPathOption(WsfPath::cEP_EXTRAPOLATE); // continue on this heading if last waypoint is reached

      // try to make it a 3D intercept point if possible (if track has valid 3D location or elevation data)
      if (trackPtr->ElevationValid() || trackPtr->LocationValid())
      {
         double tgtLat;
         double tgtLon;
         double tgtAlt;
         WsfPlatform::ConvertWCSToLLA(intLocWCS, tgtLat, tgtLon, tgtAlt);
         waypointPtr->SetAlt(tgtAlt);
      }
   }

   aReturnVal.SetDouble(timeToIntercept);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetPerceptionClass, WsfAssetPerception, TrueBearingTo, 1, "double", "WsfGeoPoint")
{
   WsfGeoPoint* pointPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   double       otherLocWCS[3];
   pointPtr->GetLocationWCS(otherLocWCS);
   UtEntity temp;
   temp.SetLocationWCS(aObjectPtr->mLocationWCS);
   double bearing = temp.TrueBearing(otherLocWCS);
   aReturnVal.SetDouble(bearing * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetPerceptionClass, WsfAssetPerception, RelativeBearingTo, 1, "double", "WsfGeoPoint")
{
   WsfGeoPoint* pointPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   double       otherLocWCS[3];
   pointPtr->GetLocationWCS(otherLocWCS);
   UtEntity temp;
   temp.SetLocationWCS(aObjectPtr->mLocationWCS);
   temp.SetOrientationNED(aObjectPtr->mOrientationNED[0], aObjectPtr->mOrientationNED[1], aObjectPtr->mOrientationNED[2]);
   double bearing = temp.RelativeBearing(otherLocWCS);
   aReturnVal.SetDouble(bearing * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetPerceptionClass, WsfAssetPerception, ClosestApproachOf, 1, "WsfGeoPoint", "WsfTrack")
{
   double    simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    otherLocWCS[3];
   double    otherVelWCS[3] = {0.0, 0.0, 0.0};
   trackPtr->GetExtrapolatedLocationWCS(simTime, otherLocWCS);
   if (trackPtr->VelocityValid())
   {
      trackPtr->GetVelocityWCS(otherVelWCS);
   }
   double pcaLocWCS[3];
   WsfUtil::ClosestApproachPoint2D(otherLocWCS, otherVelWCS, aObjectPtr->mLocationWCS, pcaLocWCS);
   WsfGeoPoint* gpPtr = new WsfGeoPoint(pcaLocWCS);
   aReturnVal.SetPointer(new UtScriptRef(gpPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetPerceptionClass, WsfAssetPerception, ClosingSpeedOf, 1, "double", "WsfTrack")
{
   double    simTime      = WsfScriptContext::GetTIME_NOW(aContext);
   double    closingSpeed = 0.0;
   WsfTrack* trackPtr     = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();

   double otherLocWCS[3];
   trackPtr->GetExtrapolatedLocationWCS(simTime, otherLocWCS);
   double otherVelWCS[3];
   trackPtr->GetVelocityWCS(otherVelWCS);

   double thisVelWCS[3] = {aObjectPtr->mVelocityWCS[0], aObjectPtr->mVelocityWCS[1], aObjectPtr->mVelocityWCS[2]};
   double relLocWCS[3];
   UtVec3d::Subtract(relLocWCS, otherLocWCS, aObjectPtr->mLocationWCS);

   double speed = UtVec3d::Normalize(thisVelWCS);
   if (speed < 0.1)
   {
      // no speed so use line-of-sight to the other object, for now
      UtVec3d::Set(thisVelWCS, relLocWCS);
      UtVec3d::Normalize(thisVelWCS);
   }
   UtVec3d::Multiply(thisVelWCS, UtVec3d::Magnitude(aObjectPtr->mVelocityWCS));
   double relVelWCS[3];
   UtVec3d::Subtract(relVelWCS, otherVelWCS, thisVelWCS);
   if (UtVec3d::Normalize(relLocWCS) > 0.0)
   {
      closingSpeed = -UtVec3d::DotProduct(relVelWCS, relLocWCS);
   }

   aReturnVal.SetDouble(closingSpeed);
}
