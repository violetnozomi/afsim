// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfEntity.hpp"

#include <sstream>

#include "VaObserver.hpp"
#include "WkfEnvironment.hpp"
#include "WkfObserver.hpp"
#include "WkfUnitsObject.hpp"

wkf::Entity::Entity(const std::string& aName)
   : vespa::VaEntity(aName)
   , mDecoration(QIcon())
{
   SetType<Entity>();
}

void wkf::Entity::SetDecoration(const QIcon& aIcon, const QString& aDescription)
{
   mDecoration = aIcon;
   wkf::Observer::EntityDecorated(this, mDecoration, aDescription.toStdString());
}

bool wkf::Entity::GetNamedInfo(const std::string& aItem, std::string& aLabel, std::string& aValue) const
{
   return GetNamedPositionInfo(aItem, aLabel, aValue);
}

bool wkf::Entity::GetNamedPositionInfo(const std::string& aItem, std::string& aLabel, std::string& aValue) const
{
   if (aItem == "Latitude")
   {
      std::stringstream ss;
      ss << GetPosition().GetLon();
      aValue = ss.str();
   }
   else if (aItem == "Longitude")
   {
      std::stringstream ss;
      ss << GetPosition().GetLat();
      aValue = ss.str();
   }
   else if (aItem == "Altitude")
   {
      std::stringstream ss;
      ss << GetPosition().GetAlt();
      aValue = ss.str();
   }
   else if (aItem == "Position")
   {
      std::stringstream ss;
      ss << GetPosition().GetLat() << " " << GetPosition().GetLon();
      aValue = ss.str();
   }
   else
   {
      return false;
   }

   aLabel = aItem;
   return true;
}
