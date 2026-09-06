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

#include "P6DofTerrain.hpp"

#include "UtException.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"

P6DofTerrain::P6DofTerrain(const std::string& aFilename)
{
   ReadTerrainFile(aFilename);
}

P6DofTerrain* P6DofTerrain::Clone() const
{
   return new P6DofTerrain(*this);
}

bool P6DofTerrain::ReadTerrainFile(const std::string& aFilename)
{
   try
   {
      UtInput inputObj;
      inputObj.PushInput(ut::make_unique<UtInputFile>(aFilename));

      std::string command;
      bool        continueProcessing = true;
      while (continueProcessing)
      {
         inputObj.GetCommand(command);

         if (command == "p6dof_terrain")
         {
            continueProcessing = ProcessInput(inputObj);
         }
         else
         {
            inputObj.ReadCommand(command);
         }
      }
   }
   catch (UtInput::EndOfData&)
   {
      return true;
   }
   catch (UtInput::ExceptionBase& exception)
   {
      auto out = ut::log::error() << "Exception thrown while reading terrain file in P6DofTerrain::ReadTerrainFile.";
      out.AddNote() << "Message: " << exception.GetMessage();
      out.AddNote() << "Location: " << exception.GetLocation();
      out.AddNote() << "File: " << aFilename;
      return false;
   }
   catch (UtInputFile::OpenError&)
   {
      auto out = ut::log::error() << "Unable to open terrain file in P6DofTerrain::ReadTerrainFile.";
      out.AddNote() << "File: " << aFilename;
      return false;
   }
   catch (std::exception& e)
   {
      auto out = ut::log::error() << "Exception in terrain file in P6DofTerrain::ReadTerrainFile.";
      out.AddNote() << "File: " << aFilename;
      out.AddNote() << "Type: " << ut::TypeNameOf(e);
      out.AddNote() << "What: " << e.what();
      return false;
   }

   return false;
}

bool P6DofTerrain::ProcessInput(UtInput& aInput)
{
   bool myCommand = false;
   if (aInput.GetCommand() == "p6dof_terrain")
   {
      myCommand = true;
      UtInputBlock block(aInput, "end_p6dof_terrain");

      std::string command;
      while (block.ReadCommand(command))
      {
         if (command == "region")
         {
            ProcessRegion(aInput);
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within P6DofTerrain::ProcessInput().";
            out.AddNote() << "Command: " << command;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return myCommand;
}

bool P6DofTerrain::ProcessRegion(UtInput& aInput)
{
   TerrainRegion tempRegion;
   tempRegion.minlat_deg = 0.0;
   tempRegion.maxlat_deg = 0.0;
   tempRegion.minlon_deg = 0.0;
   tempRegion.maxlon_deg = 0.0;
   bool minlat_valid     = false;
   bool maxlat_valid     = false;
   bool minlon_valid     = false;
   bool maxlon_valid     = false;

   bool myCommand = false;
   if (aInput.GetCommand() == "region")
   {
      myCommand = true;
      UtInputBlock block(aInput, "end_region");

      std::string command;
      while (block.ReadCommand(command))
      {
         if (command == "minLat")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            tempRegion.minlat_deg = value;
            minlat_valid          = true;
         }
         else if (command == "maxLat")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            tempRegion.maxlat_deg = value;
            maxlat_valid          = true;
         }
         else if (command == "minLon")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            tempRegion.minlon_deg = value;
            minlon_valid          = true;
         }
         else if (command == "maxLon")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            tempRegion.maxlon_deg = value;
            maxlon_valid          = true;
         }
         else if (command == "location")
         {
            ProcessLocation(aInput, tempRegion);
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within P6DofTerrain::ProcessRegion().";
            out.AddNote() << "Command: " << command;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   // We can have a valid region with min/max set defined or by having at least one location
   if (minlat_valid && maxlat_valid && minlon_valid && maxlon_valid)
   {
      // Add region to list
      mTerrainRegionList.push_back(tempRegion);
   }
   else if (!tempRegion.locationList.empty())
   {
      // First, set the limits using the first element in the list
      double minLat = tempRegion.locationList.at(0).minlat_deg;
      double maxLat = tempRegion.locationList.at(0).maxlat_deg;
      double minLon = tempRegion.locationList.at(0).minlon_deg;
      double maxLon = tempRegion.locationList.at(0).maxlon_deg;

      // Now, loop through locations
      for (auto& iter : tempRegion.locationList)
      {
         if (iter.minlat_deg < minLat)
         {
            minLat = iter.minlat_deg;
         }
         if (iter.maxlat_deg > maxLat)
         {
            maxLat = iter.maxlat_deg;
         }
         if (iter.minlon_deg < minLon)
         {
            minLon = iter.minlon_deg;
         }
         if (iter.maxlon_deg > maxLon)
         {
            maxLon = iter.maxlon_deg;
         }
      }

      tempRegion.minlat_deg = minLat;
      tempRegion.maxlat_deg = maxLat;
      tempRegion.minlon_deg = minLon;
      tempRegion.maxlon_deg = maxLon;

      // Add region to list
      mTerrainRegionList.push_back(tempRegion);
   }
   else
   {
      auto out = ut::log::error() << "Incomplete region definition in P6DofTerrain::ProcessRegion().";
      out.AddNote() << "Please specify min/max values for lat/lon.";
      out.AddNote() << "Location: " << aInput.GetLocation();
      return false;
   }

   return myCommand;
}

bool P6DofTerrain::ProcessLocation(UtInput& aInput, TerrainRegion& aRegion)
{
   TerrainLocation tempLocation;
   tempLocation.alt_ft  = 0.0;
   tempLocation.lat_deg = 0.0;
   tempLocation.lon_deg = 0.0;

   bool lat_valid    = false;
   bool lon_valid    = false;
   bool minlat_valid = false;
   bool maxlat_valid = false;
   bool minlon_valid = false;
   bool maxlon_valid = false;
   bool alt_valid    = false;

   bool myCommand = false;
   if (aInput.GetCommand() == "location")
   {
      myCommand = true;
      UtInputBlock block(aInput, "end_location");

      std::string command;
      while (block.ReadCommand(command))
      {
         if (command == "lat")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            tempLocation.lat_deg = value;
            lat_valid            = true;
         }
         else if (command == "lon")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            tempLocation.lon_deg = value;
            lon_valid            = true;
         }
         else if (command == "minLat")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            tempLocation.minlat_deg = value;
            minlat_valid            = true;
         }
         else if (command == "maxLat")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            tempLocation.maxlat_deg = value;
            maxlat_valid            = true;
         }
         else if (command == "minLon")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            tempLocation.minlon_deg = value;
            minlon_valid            = true;
         }
         else if (command == "maxLon")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            tempLocation.maxlon_deg = value;
            maxlon_valid            = true;
         }
         else if (command == "alt_ft")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            tempLocation.alt_ft = value;
            alt_valid           = true;
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within P6DofTerrain::ProcessLocation().";
            out.AddNote() << "Command: " << command;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   if (lat_valid && lon_valid && alt_valid)
   {
      // We have enough data -- now check if we have a set of min/max
      if (minlat_valid && maxlat_valid && minlon_valid && maxlon_valid)
      {
         // We have a "full set" of data, so add location to the region
         aRegion.locationList.push_back(tempLocation);
      }
      else
      {
         // We need to generate some of the min/max values. We want to stay
         // within 3 nm of the center location
         const double minute_arc         = 1.0 / 60.0;
         const double threeNauticalMiles = 3.0 * minute_arc;

         if (!minlat_valid)
         {
            tempLocation.minlat_deg = tempLocation.lat_deg - threeNauticalMiles;
         }
         if (!maxlat_valid)
         {
            tempLocation.maxlat_deg = tempLocation.lat_deg + threeNauticalMiles;
         }
         if (!minlon_valid)
         {
            tempLocation.minlon_deg = tempLocation.lon_deg - threeNauticalMiles;
         }
         if (!maxlon_valid)
         {
            tempLocation.maxlon_deg = tempLocation.lon_deg + threeNauticalMiles;
         }

         // Now, add location to the region
         aRegion.locationList.push_back(tempLocation);
      }
   }
   else
   {
      auto out = ut::log::error() << "Incomplete region definition in P6DofTerrain::ProcessLocation().";
      out.AddNote() << "Please specify at least the lat/lon/alt value for the location.";
      out.AddNote() << "Location: " << aInput.GetLocation();
      return false;
   }

   return myCommand;
}

double P6DofTerrain::HeightOfTerrain_m(double aLatitude, double aLongitude) const
{
   double alt_ft = 0.0;
   if (HeightOfRegionLocation_ft(aLatitude, aLongitude, alt_ft))
   {
      return alt_ft * UtMath::cM_PER_FT;
   }

   return 0.0;
}

double P6DofTerrain::HeightOfTerrain_m(double    aLatitude,
                                       double    aLongitude,
                                       UtVec3dX& aUpVectorNED,
                                       UtVec3dX& aSpeedVectorNED_mps,
                                       double&   aFrictionMultiplier) const
{
   aUpVectorNED.Set(0.0, 0.0, -1.0);
   aSpeedVectorNED_mps.Set(0.0, 0.0, 0.0);
   aFrictionMultiplier = 1.0;

   double alt_ft = 0.0;
   if (HeightOfRegionLocation_ft(aLatitude, aLongitude, alt_ft))
   {
      return alt_ft * UtMath::cM_PER_FT;
   }

   return 0.0;
}

bool P6DofTerrain::HeightOfRegionLocation_ft(double aLatitude, double aLongitude, double& aAlt_ft) const
{
   // Loop through regions
   for (const auto& iter : mTerrainRegionList)
   {
      if (WithinRegion(iter, aLatitude, aLongitude))
      {
         // Loop through locations
         for (const auto& iterLoc : iter.locationList)
         {
            double alt_ft = 0.0;
            if (WithinLocation(iterLoc, aLatitude, aLongitude, alt_ft))
            {
               aAlt_ft = alt_ft;
               return true;
            }
         }
      }
   }

   return false;
}

bool P6DofTerrain::WithinRegion(const TerrainRegion& aRegion, double aLatitude, double aLongitude) const
{
   if (aLatitude > aRegion.minlat_deg)
   {
      if (aLatitude < aRegion.maxlat_deg)
      {
         // TODO - Check for cross over prime meridian
         // TODO - Check for cross over international dateline

         if (aLongitude > aRegion.minlon_deg)
         {
            if (aLongitude < aRegion.maxlon_deg)
            {
               // Within the region
               return true;
            }
         }
      }
   }

   return false;
}

bool P6DofTerrain::WithinLocation(const TerrainLocation& aLocation, double aLatitude, double aLongitude, double& aAlt_ft) const
{
   if (aLatitude > aLocation.minlat_deg)
   {
      if (aLatitude < aLocation.maxlat_deg)
      {
         // TODO - Check for cross over prime meridian
         // TODO - Check for cross over international dateline

         if (aLongitude > aLocation.minlon_deg)
         {
            if (aLongitude < aLocation.maxlon_deg)
            {
               // Within the region
               aAlt_ft = aLocation.alt_ft;

               return true;
            }
         }
      }
   }

   return false;
}
