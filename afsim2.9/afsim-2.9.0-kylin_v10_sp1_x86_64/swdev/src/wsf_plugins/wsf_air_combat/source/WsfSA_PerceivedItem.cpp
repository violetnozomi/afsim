// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2022 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSA_PerceivedItem.hpp"

#include <string>

#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "UtMemory.hpp"
#include "UtScriptVec3.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfIntercept.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfProcessor.hpp"
#include "WsfSA_Processor.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "WsfUtil.hpp"
#include "WsfWaypoint.hpp"
#include "WsfWeapon.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptObjectClass.hpp"

// -----------------------------------------------------------------------------------------------------

// Note: This distance is greater than 100 times the distance from the earth to the sun
double WsfSA_PerceivedItem::mAssumedRangeForAngleOnlyTargets = 2.0e13;

WsfSA_PerceivedItem::WsfSA_PerceivedItem(const WsfSA_PerceivedItem& aSrc)
   : UtScriptAccessible()
   , mOwningPlatform(aSrc.mOwningPlatform)
   , mBearing_deg(aSrc.mBearing_deg)
   , mThreatLevel(aSrc.mThreatLevel)
   , mTargetValue(aSrc.mTargetValue)
   , mPerceivedType(aSrc.mPerceivedType)
   , mAltitudeValid(aSrc.mAltitudeValid)
   , mSpeedValid(aSrc.mSpeedValid)
   , mHeadingValid(aSrc.mHeadingValid)
   , mSelectionScore(aSrc.mSelectionScore)
   , mMalformedPerception(aSrc.mMalformedPerception)
   , mSimulationPtr(aSrc.mSimulationPtr)
{
}

WsfSA_PerceivedItem& WsfSA_PerceivedItem::operator=(const WsfSA_PerceivedItem& aRhs)
{
   if (this != &aRhs)
   {
      SetLat_deg(aRhs.GetLat_deg());
      SetLon_deg(aRhs.GetLon_deg());
      SetAltitude_ft(aRhs.GetAltitude_ft());
      SetBearing_deg(aRhs.GetBearing_deg());
      SetSpeed_kts(aRhs.GetSpeed_kts());
      SetHeading_deg(aRhs.GetHeading_deg());
      mOwningPlatform      = aRhs.mOwningPlatform;
      mThreatLevel         = aRhs.mThreatLevel;
      mTargetValue         = aRhs.mTargetValue;
      mPerceivedType       = aRhs.mPerceivedType;
      mAltitudeValid       = aRhs.mAltitudeValid;
      mSpeedValid          = aRhs.mSpeedValid;
      mHeadingValid        = aRhs.mHeadingValid;
      mSelectionScore      = aRhs.mSelectionScore;
      mMalformedPerception = aRhs.mMalformedPerception;
      mSimulationPtr       = aRhs.mSimulationPtr;
   }
   return *this;
}

double WsfSA_PerceivedItem::CalcRangeForPlatform(const WsfPlatform* aPlatform)
{
   double platLat, platLon, alt_m;
   aPlatform->GetLocationLLA(platLat, platLon, alt_m);

   double heading_deg, distance_m;
   UtSphericalEarth::GreatCircleHeadingAndDistance(platLat, platLon, GetLat_deg(), GetLon_deg(), heading_deg, distance_m);

   mSelectionScore = distance_m;

   return mSelectionScore;
}

double WsfSA_PerceivedItem::AngleOffHeadingFromOwningPlatform_deg()
{
   auto platform = OwningPlatform();
   if (platform == nullptr)
   {
      return -180.0;
   }

   double heading, pitch, roll; // Only heading is used
   platform->GetOrientationNED(heading, pitch, roll);
   double ret;
   if (GetAngleOnly())
   {
      ret = GetBearing_deg() - (heading * UtMath::cDEG_PER_RAD);
   }
   else
   {
      double lat = GetLat_deg();
      double lon = GetLon_deg();
      double alt = 0.0;
      if (GetAltitudeValid())
      {
         alt = GetAltitude_ft() * UtMath::cM_PER_FT;
      }
      WsfGeoPoint entityPt = WsfGeoPoint(lat, lon, alt);
      double      wcs[3];
      entityPt.GetLocationWCS(wcs);
      double az, el;
      platform->ComputeAzimuthAndElevation(wcs, az, el);
      ret = (az - heading) * UtMath::cDEG_PER_RAD;
   }
   ret = UtMath::NormalizeAngleMinus180_180(ret);
   return ret;
}

bool WsfSA_PerceivedItem::CompareScoreLowGood(const WsfSA_PerceivedItem* aFirstItem, const WsfSA_PerceivedItem* aSecondItem)
{
   // Return true for a better score, so here a low selection score is better
   if (aFirstItem->mSelectionScore < aSecondItem->mSelectionScore)
   {
      return true;
   }
   return false;
}

bool WsfSA_PerceivedItem::CompareScoreHighGood(const WsfSA_PerceivedItem* aFirstItem, const WsfSA_PerceivedItem* aSecondItem)
{
   // Return true for a better score, so here a high selection score is better
   if (aFirstItem->mSelectionScore > aSecondItem->mSelectionScore)
   {
      return true;
   }
   return false;
}

bool WsfSA_PerceivedItem::CompareThreatLevelHighGood(const WsfSA_PerceivedItem* aFirstItem,
                                                     const WsfSA_PerceivedItem* aSecondItem)
{
   // Return true for a better score, so here a high selection score is better
   if (aFirstItem->mThreatLevel > aSecondItem->mThreatLevel)
   {
      return true;
   }
   return false;
}

bool WsfSA_PerceivedItem::CompareTargetValueHighGood(const WsfSA_PerceivedItem* aFirstItem,
                                                     const WsfSA_PerceivedItem* aSecondItem)
{
   // Return true for a better score, so here a high selection score is better
   if (aFirstItem->mTargetValue > aSecondItem->mTargetValue)
   {
      return true;
   }
   return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WsfScriptSA_PerceivedItemClass::WsfScriptSA_PerceivedItemClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptObjectClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSA_PerceivedItem");
   mConstructible = false;
   mCloneable     = false;
   AddExplicitCastType("WsfSA_EntityPerception");
   AddExplicitCastType("WsfSA_Group");

   AddMethod(ut::make_unique<ItemType>());
   AddMethod(ut::make_unique<Lat>());
   AddMethod(ut::make_unique<Lon>());
   AddMethod(ut::make_unique<Altitude>());
   AddMethod(ut::make_unique<Bearing>());
   AddMethod(ut::make_unique<Speed>());
   AddMethod(ut::make_unique<Heading>());
   AddMethod(ut::make_unique<ThreatLevel>());
   AddMethod(ut::make_unique<TargetValue>());
   AddMethod(ut::make_unique<PerceivedName>());
   AddMethod(ut::make_unique<Identification>());
   AddMethod(ut::make_unique<AngleOnly>());
   AddMethod(ut::make_unique<AltitudeValid>());
   AddMethod(ut::make_unique<SpeedValid>());
   AddMethod(ut::make_unique<HeadingValid>());
   AddMethod(ut::make_unique<SelectionScore>());
   AddMethod(ut::make_unique<EstimatedTimeToIntercept>());
}

void WsfScriptSA_PerceivedItemClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfSA_PerceivedItem*>(aObjectPtr);
}

std::string WsfScriptSA_PerceivedItemClass::ToString(void* aObjectPtr) const
{
   std::stringstream ss;
   ss << GetClassName() << '(';
   if (aObjectPtr != nullptr)
   {
      WsfSA_PerceivedItem* itemPtr = static_cast<WsfSA_PerceivedItem*>(aObjectPtr);
      ss << "ID: " << itemPtr->GetPerceivedName();
   }
   else
   {
      ss << "null";
   }
   ss << ')';
   return ss.str();
}

void WsfScriptSA_PerceivedItemClass::OnNewScriptRef(UtScriptRef& aReference)
{
   if (aReference.GetMemManagement() != UtScriptRef::cMANAGE)
   {
      WsfSA_PerceivedItem* itemPtr = static_cast<WsfSA_PerceivedItem*>(aReference.GetAppObject());
      if (itemPtr != nullptr)
      {
         aReference.SetExternalReference(itemPtr->GetReferenceCount());
      }
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_PerceivedItemClass, WsfSA_PerceivedItem, ItemType, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetItemType()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_PerceivedItemClass, WsfSA_PerceivedItem, Lat, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetLat_deg());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_PerceivedItemClass, WsfSA_PerceivedItem, Lon, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetLon_deg());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_PerceivedItemClass, WsfSA_PerceivedItem, Altitude, 0, "double", "")
{
   if (aObjectPtr->GetAltitudeValid())
   {
      aReturnVal.SetDouble(aObjectPtr->GetAltitude_ft() * UtMath::cM_PER_FT);
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_PerceivedItemClass, WsfSA_PerceivedItem, Bearing, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetBearing_deg());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_PerceivedItemClass, WsfSA_PerceivedItem, Speed, 0, "double", "")
{
   if (aObjectPtr->GetSpeedValid())
   {
      aReturnVal.SetDouble(aObjectPtr->GetSpeed_kts() * UtMath::cMPS_PER_NMPH);
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_PerceivedItemClass, WsfSA_PerceivedItem, Heading, 0, "double", "")
{
   if (aObjectPtr->GetHeadingValid())
   {
      aReturnVal.SetDouble(aObjectPtr->GetHeading_deg());
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_PerceivedItemClass, WsfSA_PerceivedItem, ThreatLevel, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetThreatLevel());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_PerceivedItemClass, WsfSA_PerceivedItem, TargetValue, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTargetValue());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_PerceivedItemClass, WsfSA_PerceivedItem, PerceivedName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetPerceivedName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_PerceivedItemClass, WsfSA_PerceivedItem, Identification, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetIdentification()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_PerceivedItemClass, WsfSA_PerceivedItem, AngleOnly, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAngleOnly());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_PerceivedItemClass, WsfSA_PerceivedItem, AltitudeValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAltitudeValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_PerceivedItemClass, WsfSA_PerceivedItem, SpeedValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetSpeedValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_PerceivedItemClass, WsfSA_PerceivedItem, HeadingValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetHeadingValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_PerceivedItemClass, WsfSA_PerceivedItem, SelectionScore, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetSelectionScore());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_PerceivedItemClass, WsfSA_PerceivedItem, EstimatedTimeToIntercept, 1, "double", "WsfPlatform")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (platformPtr != nullptr)
   {
      double time_sec = aObjectPtr->EstimatedTimeToIntercept(platformPtr);
      aReturnVal.SetDouble(time_sec);
   }
   else
   {
      aReturnVal.SetDouble(-1.0);
   }
}
