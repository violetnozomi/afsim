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

#include "WkfTrack.hpp"

#include <QString>
#include <QStringList>

#include "UtAirspeed.hpp"
#include "VaAttachmentModel.hpp"
#include "WkfEnvironment.hpp"
#include "WkfScenario.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "WkfUnitsObject.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::Track::Track(const TrackId& aTrackId)
   : Entity(std::string("track_") + std::string(aTrackId))
   , mTrackId(aTrackId)
   , mPositionValid(false)
   , mVelocityValid(false)
{
   SetType<Track>();
   SetDraggable(false);
}

void wkf::Track::SetSide(const std::string& aSide)
{
   if (aSide != mSide)
   {
      mSide                              = aSide;
      vespa::VaAttachmentModel* modelPtr = FindFirstAttachmentOfType<vespa::VaAttachmentModel>();

      if (nullptr != modelPtr)
      {
         modelPtr->SetColor(wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->GetTeamUtColor(mSide));
      }
   }
}

void wkf::Track::SetSpatialDomain(const std::string& aDomainString)
{
   mSpatialDomain = StringToSpatialDomain(aDomainString);
}

std::string wkf::Track::GetSpatialDomainAsString() const
{
   return SpatialDomainToString(mSpatialDomain);
}

std::string wkf::Track::GetDescription()
{
   return "Track:\nPlatform: " + mTrackId.GetPlatformName() + "\nIndex: " + std::to_string(mTrackId.GetLocalTrackId());
}

wkf::TrackId::TrackId(const QString& aTrackString)
   : wkf::TrackId::TrackId()
{
   QStringList sl = aTrackString.split(":");
   if (sl.size() == 2)
   {
      bool ok;
      mLocalTrackId = sl[1].toInt(&ok);
      if (ok)
      {
         mOwningPlatformName = sl[0].toStdString();
      }
   }
}

wkf::SpatialDomain wkf::StringToSpatialDomain(const std::string& aDomainString)
{
   QString domain = QString::fromStdString(aDomainString).toLower();

   if (domain == "land")
   {
      return wkf::SPATIAL_DOMAIN_LAND;
   }
   else if (domain == "air")
   {
      return wkf::SPATIAL_DOMAIN_AIR;
   }
   else if (domain == "surface")
   {
      return wkf::SPATIAL_DOMAIN_SURFACE;
   }
   else if (domain == "subsurface")
   {
      return wkf::SPATIAL_DOMAIN_SUBSURFACE;
   }
   else if (domain == "space")
   {
      return wkf::SPATIAL_DOMAIN_SPACE;
   }
   else
   {
      return wkf::SPATIAL_DOMAIN_UNKNOWN;
   }
}

std::string wkf::SpatialDomainToString(wkf::SpatialDomain aSpatialDomain)
{
   switch (aSpatialDomain)
   {
   case wkf::SPATIAL_DOMAIN_LAND:
      return "Land";
   case wkf::SPATIAL_DOMAIN_AIR:
      return "Air";
   case wkf::SPATIAL_DOMAIN_SURFACE:
      return "Surface";
   case wkf::SPATIAL_DOMAIN_SUBSURFACE:
      return "Subsurface";
   case wkf::SPATIAL_DOMAIN_SPACE:
      return "Space";
   default:
      return "Unknown";
   }
}

bool wkf::Track::GetNamedInfo(const std::string& aItem, std::string& aLabel, std::string& aValue) const
{
   if (GetNamedPositionInfo(aItem, aLabel, aValue))
   {
      return true;
   }

   const auto& units    = *wkfEnv.GetPreferenceObject<wkf::UnitsObject>();
   auto*       scenario = vaEnv.GetStandardScenario();
   aLabel               = aItem;
   if (aItem == "Side")
   {
      aValue = GetSide().empty() ? "Unknown" : GetSide();
   }
   else if (aItem == "Type")
   {
      aValue = GetTrackType().empty() ? "Unknown" : GetTrackType();
   }
   else if (aItem == "Heading")
   {
      aValue = units.GetFormattedValue(ValueType::cANGLE, GetHeading()).toStdString();
   }
   else if (scenario && aItem == "Bearing (Bullseye)")
   {
      aLabel              = "Bearing";
      aValue              = "no bullseye";
      double      range   = 0;
      double      bearing = 0;
      std::string name;
      if (scenario->GetRangeBearingFromNearestBullseye(GetPosition(), range, bearing, name))
      {
         // default unit is radians, convert to radians before passing in.
         aValue = units.GetFormattedValue(ValueType::cANGLE, bearing * UtMath::cRAD_PER_DEG).toStdString();
      }
   }
   else if (scenario && aItem == "Range (Bullseye)")
   {
      aLabel              = "Range";
      aValue              = "no bullseye";
      double      range   = 0;
      double      bearing = 0;
      std::string name;
      if (scenario->GetRangeBearingFromNearestBullseye(GetPosition(), range, bearing, name))
      {
         aValue = units.GetFormattedValue(ValueType::cLENGTH, range).toStdString();
      }
   }
   else if (aItem == "Spatial Domain")
   {
      aValue = GetSpatialDomainAsString();
   }
   else if (aItem == "Speed")
   {
      aValue = units.GetFormattedValue(ValueType::cSPEED, GetVelocityWCS().Magnitude()).toStdString();
   }
   else if (aItem == "Mach")
   {
      const double alt   = GetPosition().GetAlt();
      const double speed = GetVelocityWCS().Magnitude();
      const double mach  = UtAirspeed::CalculateMachAtAltitude_m(alt, speed);
      aValue             = QString::number(mach, 'f', 2).toStdString();
   }
   else
   {
      return false;
   }

   return true;
}
