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

#include "WkfPlatform.hpp"

#include <sstream>

#include "WkfEnvironment.hpp"
#include "WkfObserver.hpp"
#include "WkfUnitsObject.hpp"

wkf::Platform::Platform(const std::string& aName)
   : Entity(aName)
   , mIndex(0)
   , mSpatialDomain(SPATIAL_DOMAIN_UNKNOWN)
   , mBrokenTime(-1.0)
   , mDeletionTime(-1.0)
   , mIsBeingDragged(false)
{
   SetType<Platform>();
}

std::string wkf::Platform::GetTypeAFSIM() const
{
   if (!mTypeList.empty())
   {
      return mTypeList.front();
   }
   return std::string();
}

void wkf::Platform::SetBroken(double aBrokenTime)
{
   if (mBrokenTime < 0.0)
   {
      mBrokenTime = aBrokenTime;
      Observer::PlatformBroken(this);
   }
}

void wkf::Platform::SetDeleted(double aDeletionTime)
{
   if (mDeletionTime < 0.0)
   {
      mDeletionTime = aDeletionTime;
      Observer::PlatformPendingDeletion(this);
   }
}

std::string wkf::Platform::GetDescription()
{
   std::string status = "alive";
   if (mBrokenTime != -1.0)
   {
      status = "broken";
   }
   UnitsObject*       uo = wkfEnv.GetPreferenceObject<UnitsObject>();
   UtLonPos           latitude(GetLat(), uo->GetLatLonFormatting());
   std::ostringstream geomstream;
   geomstream << "Position: " << latitude << ", ";
   UtLonPos longitude(GetLon(), uo->GetLatLonFormatting());
   geomstream << longitude << ", ";
   geomstream << uo->GetFormattedValue(ValueType::cALTITUDE, GetAlt()).toStdString();
   geomstream << "\nHeading: " << uo->GetFormattedValue(ValueType::cANGLE, GetHeading()).toStdString();
   geomstream << "\nSpeed: " << uo->GetFormattedValue(ValueType::cSPEED, GetVelocityWCS().Magnitude()).toStdString();

   return "platform: " + GetName() + "\n" + status + "\n" + geomstream.str();
}

void wkf::Platform::AddSubobject(const std::string& aName,
                                 const std::string& aIcon,
                                 const double       aPositionECS[3],
                                 const double       aOrientationECS[3])
{
   Subobject so;
   so.mIcon              = aIcon;
   so.mPositionECS[0]    = aPositionECS[0];
   so.mPositionECS[1]    = aPositionECS[1];
   so.mPositionECS[2]    = aPositionECS[2];
   so.mOrientationECS[0] = aOrientationECS[0];
   so.mOrientationECS[1] = aOrientationECS[1];
   so.mOrientationECS[2] = aOrientationECS[2];
   mSubobjectMap[aName]  = so;
}

void wkf::Platform::UpdateSubobject(const std::string& aName, const double aPositionECS[3], const double aOrientationECS[3])
{
   mSubobjectMap[aName].mPositionECS[0]    = aPositionECS[0];
   mSubobjectMap[aName].mPositionECS[1]    = aPositionECS[1];
   mSubobjectMap[aName].mPositionECS[2]    = aPositionECS[2];
   mSubobjectMap[aName].mOrientationECS[0] = aOrientationECS[0];
   mSubobjectMap[aName].mOrientationECS[1] = aOrientationECS[1];
   mSubobjectMap[aName].mOrientationECS[2] = aOrientationECS[2];
}

void wkf::Platform::RemoveSubobject(const std::string& aName)
{
   mSubobjectMap.erase(aName);
}
