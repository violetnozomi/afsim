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

#include "WsfPlatformAvailability.hpp"

#include "UtCentralBody.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtRandom.hpp"
#include "UtVec3.hpp"
#include "WsfEnvironment.hpp"
#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfZoneTypes.hpp"

namespace
{
// =================================================================================================
//! Get the platform's WCS location based on the global central body.
UtVec3d GetPlatformLocationWCS(const WsfPlatform& aPlatform, const WsfSimulation& aSimulation)
{
   double lat, lon, alt;
   aPlatform.GetLocationLLA(lat, lon, alt);
   UtVec3d locWCS;
   aSimulation.GetEnvironment().GetCentralBody().ConvertLLA_ToWCS(lat, lon, alt, locWCS);
   return locWCS;
}
} // namespace

// =================================================================================================
WsfPlatformAvailability::WsfPlatformAvailability()
{
   mDefaultAvailability = 1.0;
}

// =================================================================================================
bool WsfPlatformAvailability::ProcessInput(UtInput& aInput)
{
   bool myCommand = false;
   if (aInput.GetCommand() == "platform_availability")
   {
      myCommand = true;
      std::string  command;
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if ((command == "name") || (command == "type") || (command == "category"))
         {
            std::string key;
            std::string word;
            double      probability;
            aInput.ReadValue(key);
            aInput.ReadValue(word);
            aInput.StringEqual(word, "availability");
            aInput.ReadValue(probability);
            aInput.ValueInClosedRange(probability, 0.0, 1.0);
            if (command == "name")
            {
               mAvailabilityByName[key] = probability;
            }
            else if (command == "type")
            {
               mAvailabilityByType[key] = probability;
            }
            else if (command == "category")
            {
               mAvailabilityByCategory[key] = probability;
            }
         }
         else if (command == "default")
         {
            std::string word;
            double      probability;
            aInput.ReadValue(word);
            aInput.StringEqual(word, "availability");
            aInput.ReadValue(probability);
            aInput.ValueInClosedRange(probability, 0.0, 1.0);
            mDefaultAvailability = probability;
         }
         else if (command == "exclusion_zone")
         {
            if (!mInclusionZones.empty())
            {
               throw UtInput::BadValue(aInput, "exclusion_zone and inclusion_zone are mutually exclusive");
            }
            std::string zoneName;
            aInput.ReadValue(zoneName);
            mExclusionZones.emplace_back(zoneName);
         }
         else if (command == "inclusion_zone")
         {
            if (!mExclusionZones.empty())
            {
               throw UtInput::BadValue(aInput, "exclusion_zone and inclusion_zone are mutually exclusive");
            }
            std::string zoneName;
            aInput.ReadValue(zoneName);
            mInclusionZones.emplace_back(zoneName);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   return myCommand;
}

// =================================================================================================
//! Determine if the specified input platform should be added to the simulation.
//! A user may specify that an input platform should be eliminated from the simulation.
//! This may be unconditional or based on a random number. This allows platforms to be
//! be excluded without having to explicitly modify the scenario.
// private
bool WsfPlatformAvailability::PlatformIsAvailable(WsfPlatform* aPlatformPtr, WsfSimulation& aSimulation) const
{
   bool   isAvailable  = true;
   double availability = mDefaultAvailability;

   // First attempt to locate an entry using the platform name.
   // If that fails, attempt to locate an entry using the types in the platforms type hierarchy.
   // If that fails, attempt to locate an entry using the platform categories.

   bool found = false;
   auto ami   = mAvailabilityByName.find(aPlatformPtr->GetNameId());
   if (ami != mAvailabilityByName.end())
   {
      availability = (*ami).second; // Matched on platform name
      found        = true;
   }

   if (!found)
   {
      const WsfObject::TypeList& typeList = aPlatformPtr->GetTypeList();
      for (auto typeId : typeList)
      {
         ami = mAvailabilityByType.find(typeId);
         if (ami != mAvailabilityByType.end())
         {
            availability = (*ami).second; // Matched on platform type
            found        = true;
            break;
         }
      }
   }

   if (!found)
   {
      const WsfCategoryList::CategoryList& categoryList = aPlatformPtr->GetCategories().GetCategoryList();
      for (auto categoryId : categoryList)
      {
         ami = mAvailabilityByCategory.find(categoryId);
         if (ami != mAvailabilityByCategory.end())
         {
            availability = (*ami).second; // Matched on category
            break;
         }
      }
   }

   // Check for availability base on name and type.

   if (availability < 1.0)
   {
      isAvailable = aSimulation.GetRandom().Bernoulli(availability);
   }

   // If the platform has passed the name and type availability, check to see if it passes the zone availability.
   //
   // Note that these checks are not applied to objects in space because their initial position has a high probability
   // of not being in a zone. Furthermore, note that the platforms spatial domain is not yet valid

   WsfSpatialDomain spatialDomain = aPlatformPtr->GetSpatialDomain();
   if (spatialDomain == WSF_SPATIAL_DOMAIN_UNKNOWN)
   {
      spatialDomain = WSF_SPATIAL_DOMAIN_LAND;
      if (aPlatformPtr->GetMover() != nullptr)
      {
         spatialDomain = aPlatformPtr->GetMover()->GetSpatialDomain();
      }
   }
   std::set<std::string> warningZones;
   if (isAvailable && (spatialDomain != WSF_SPATIAL_DOMAIN_SPACE))
   {
      if (!mExclusionZones.empty())
      {
         isAvailable = true; // platform is available unless specifically excluded.
         for (auto exclusionZone : mExclusionZones)
         {
            if (!exclusionZone.Empty())
            {
               WsfZone* zonePtr = WsfZoneTypes::Get(aSimulation.GetScenario()).Find(exclusionZone);
               if (zonePtr != nullptr)
               {
                  auto locWCS = GetPlatformLocationWCS(*aPlatformPtr, aSimulation);
                  if (zonePtr->PointIsInside(&aSimulation, locWCS.GetData(), locWCS.GetData(), 0.0))
                  {
                     isAvailable = false;
                     break;
                  }
               }
               else if (warningZones.find(exclusionZone) != warningZones.end())
               {
                  auto out = ut::log::warning() << "Undefined platform_availability exclusion_zone.";
                  out.AddNote() << "Zone: " << exclusionZone;
                  warningZones.insert(exclusionZone); // Prevent repetitive messages
               }
            }
         }
      }
      else if (!mInclusionZones.empty())
      {
         isAvailable = false; // platform is unavailable unless specifically included.
         for (auto inclusionZone : mInclusionZones)
         {
            if (!inclusionZone.Empty())
            {
               WsfZone* zonePtr = WsfZoneTypes::Get(aSimulation.GetScenario()).Find(inclusionZone);
               if (zonePtr != nullptr)
               {
                  auto locWCS = GetPlatformLocationWCS(*aPlatformPtr, aSimulation);
                  if (zonePtr->PointIsInside(&aSimulation, locWCS.GetData(), locWCS.GetData(), 0.0))
                  {
                     isAvailable = true;
                     break;
                  }
               }
               else if (warningZones.find(inclusionZone) != warningZones.end())
               {
                  auto out = ut::log::warning() << "Undefined platform_availability inclusion_zone.";
                  out.AddNote() << "Zone: " << inclusionZone;
                  warningZones.insert(inclusionZone); // Prevent repetitive messages
               }
            }
         }
      }
   }

   return isAvailable;
}
