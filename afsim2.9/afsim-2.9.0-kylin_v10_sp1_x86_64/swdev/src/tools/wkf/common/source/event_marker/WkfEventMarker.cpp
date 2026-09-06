// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfEventMarker.hpp"

#include "WkfEnvironment.hpp"
#include "WkfUnitsObject.hpp"

wkf::BaseEventMarker::BaseEventMarker(const std::string& aName, double aTime, const std::string& aType)
   : wkf::Entity(aName)
   , mTime(aTime)
   , mType(aType)
{
   SetType<BaseEventMarker>();
}

bool wkf::BaseEventMarker::GetNamedEventInfo(const std::string& aItem, std::string& aLabel, std::string& aValue) const
{
   if (aItem == "Time")
   {
      const auto& units = *wkfEnv.GetPreferenceObject<wkf::UnitsObject>();
      aValue            = UtTime::ToString(mTime, units.GetTimeFormatting());
   }
   else if (aItem == "Event Type")
   {
      aValue = GetEventType();
   }
   else
   {
      return false;
   }

   aLabel = aItem;
   return true;
}

wkf::PlatformEventMarker::PlatformEventMarker(double aTime, const std::string& aType, const wkf::PlatformEventInfo& aInfo)
   : BaseEventMarker(std::string(), aTime, aType)
   , mInfo(aInfo)
{
   SetType<PlatformEventMarker>();
   SetName(GetSummary());

   connect(wkfEnv.GetPreferenceObject<wkf::UnitsObject>(),
           &wkf::UnitsObject::TimeFormatChanged,
           [this](int /*unused*/) { SetName(GetSummary()); });
}

bool wkf::PlatformEventMarker::GetNamedInfo(const std::string& aItem, std::string& aLabel, std::string& aValue) const
{
   if (GetNamedPositionInfo(aItem, aLabel, aValue))
   {
      return true;
   }
   else if (GetNamedEventInfo(aItem, aLabel, aValue))
   {
      return true;
   }
   else if (aItem == "Platform")
   {
      aValue = mInfo.mPlatform;
   }
   else
   {
      return false;
   }

   aLabel = aItem;
   return true;
}

std::string wkf::PlatformEventMarker::GetSummary() const
{
   std::string retval = mInfo.mPlatform + " ";
   std::string label;
   std::string value;

   GetNamedEventInfo("Event Type", label, value);
   retval += value;

   return retval;
}

wkf::WeaponEventMarker::WeaponEventMarker(double aTime, const std::string& aType, const wkf::WeaponEventInfo& aInfo)
   : BaseEventMarker(std::string(), aTime, aType)
   , mInfo(aInfo)
{
   SetType<WeaponEventMarker>();
   SetName(GetSummary());

   connect(wkfEnv.GetPreferenceObject<wkf::UnitsObject>(),
           &wkf::UnitsObject::TimeFormatChanged,
           [this](int /*unused*/) { SetName(GetSummary()); });
}

bool wkf::WeaponEventMarker::GetNamedInfo(const std::string& aItem, std::string& aLabel, std::string& aValue) const
{
   if (GetNamedPositionInfo(aItem, aLabel, aValue))
   {
      return true;
   }
   else if (GetNamedEventInfo(aItem, aLabel, aValue))
   {
      return true;
   }

   const auto& units = *wkfEnv.GetPreferenceObject<wkf::UnitsObject>();

   if (aItem == "Firing Platform")
   {
      aValue = mInfo.mFiringName;
   }
   else if (aItem == "Weapon Platform")
   {
      aValue = mInfo.mWeaponName;
   }
   else if (aItem == "Target Platform")
   {
      aValue = mInfo.mTargetName;
   }
   else if (aItem == "Result")
   {
      aValue = mInfo.mResult;
   }
   else if (aItem == "Intercept PK")
   {
      aValue = std::to_string(mInfo.mInterceptPk);
   }
   else if (aItem == "PK Drawn")
   {
      if (mInfo.mPkDrawn < 0)
      {
         aValue = "indestructible";
      }
      else
      {
         aValue = std::to_string(mInfo.mPkDrawn);
      }
   }
   else if (aItem == "Miss Distance")
   {
      aValue = units.GetFormattedValue(ValueType::cLENGTH, mInfo.mMissDistance).toStdString();
   }
   else if (aItem == "Weapon Speed")
   {
      aValue = units.GetFormattedValue(ValueType::cSPEED, mInfo.mWeaponSpeed).toStdString();
   }
   else if (aItem == "Target Speed")
   {
      aValue = units.GetFormattedValue(ValueType::cSPEED, mInfo.mTargetSpeed).toStdString();
   }
   else
   {
      return false;
   }

   aLabel = aItem;
   return true;
}

std::string wkf::WeaponEventMarker::GetSummary() const
{
   std::string retval = mInfo.mTargetName + " ";
   std::string label;
   std::string value;

   GetNamedEventInfo("Event", label, value);
   retval += value;

   return retval;
}
