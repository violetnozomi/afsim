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

#include "WsfOSM_TrafficInput.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "XWsfRouteNetwork.hpp"

namespace
{
// Some convenient aliases for use outside class scope
using VehicleInput            = WsfOSM_TrafficInput::VehicleInput;
using ConvoyInput             = WsfOSM_TrafficInput::ConvoyInput;
using RouteAuxMapping         = WsfOSM_TrafficInput::RouteAuxMapping;
using WaypointAuxMapping      = WsfOSM_TrafficInput::WaypointAuxMapping;
using RoadTrafficNetworkInput = WsfOSM_TrafficInput::RoadTrafficNetworkInput;

void ProcessVehicleBlock(UtInput& aInput, VehicleInput& aVehicle)
{
   UtInputBlock inputBlock(aInput);
   while (inputBlock.ReadCommand())
   {
      std::string command = inputBlock.GetCommand();
      if (command == "fraction")
      {
         double fraction;
         aInput.ReadValue(fraction);
         aInput.ValueGreater(fraction, 0.0);
         aInput.ValueLessOrEqual(fraction, 1.0);
         aVehicle.mFraction = fraction;
      }
      else if (command == "mean_speed")
      {
         double speed;
         aInput.ReadValueOfType(speed, UtInput::cSPEED);
         aInput.ValueGreaterOrEqual(speed, 0.0);
         aVehicle.mMeanSpeed = speed;
      }
      else if (command == "sigma_speed")
      {
         double speed;
         aInput.ReadValueOfType(speed, UtInput::cSPEED);
         aInput.ValueGreaterOrEqual(speed, 0.0);
         aVehicle.mSigmaSpeed = speed;
      }
      else if (command == "ignore_speed_limits")
      {
         aVehicle.mIgnoreSpeedLimits = true;
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
}

// =================================================================================================
void ProcessConvoyBlock(UtInput& aInput, ConvoyInput& aConvoyInput)
{
   UtInputBlock inputBlock(aInput);
   while (inputBlock.ReadCommand())
   {
      std::string command = inputBlock.GetCommand();
      if (command == "starting_route")
      {
         aInput.ReadValue(aConvoyInput.mStartingRoute);
      }
      else if (command == "route_distance")
      {
         aInput.ReadValueOfType(aConvoyInput.mDistanceAlongRoute, UtInput::cLENGTH);
      }
      else if (command == "spacing")
      {
         aInput.ReadValueOfType(aConvoyInput.mSpacing, UtInput::cTIME);
         aInput.ValueGreater(aConvoyInput.mSpacing, 0.0);
      }
      else if (command == "speed")
      {
         aInput.ReadValueOfType(aConvoyInput.mSpeed, UtInput::cSPEED);
         aInput.ValueGreater(aConvoyInput.mSpeed, 0.0);
      }
      else if (command == "vehicle")
      {
         // Read the number of vehicles of this type.
         int count;
         aInput.ReadValue(count);
         aInput.ValueGreater(count, 0);

         // The vehicle type.
         std::string vehicleType;
         aInput.ReadValue(vehicleType);

         WsfOSM_TrafficInput::VehicleInput vehInput(vehicleType);
         vehInput.mCount = count;
         aConvoyInput.mVehicleInput.push_back(vehInput);
         ProcessVehicleBlock(aInput, aConvoyInput.mVehicleInput.back());
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
}

// =================================================================================================
void ProcessRouteAuxMapBlock(UtInput& aInput, RouteAuxMapping& aRouteAuxInput)
{
   UtInputBlock inputBlock(aInput);
   while (inputBlock.ReadCommand())
   {
      std::string command = inputBlock.GetCommand();
      if (command == "minimum_speed")
      {
         aInput.ReadValue(aRouteAuxInput.mMinSpeed);
      }
      else if (command == "maximum_speed")
      {
         aInput.ReadValue(aRouteAuxInput.mMaxSpeed);
      }
      else if (command == "surface")
      {
         aInput.ReadValue(aRouteAuxInput.mSurface);
      }
      else if (command == "lanes")
      {
         aInput.ReadValue(aRouteAuxInput.mLanes);
      }
      else if (command == "density")
      {
         aInput.ReadValue(aRouteAuxInput.mDensity);
      }
      else if (command == "one_way")
      {
         aInput.ReadValue(aRouteAuxInput.mOneWay);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
}

// =================================================================================================
void ProcessWaypointAuxMapBlock(UtInput& aInput, WaypointAuxMapping& aWaypointAuxInput)
{
   UtInputBlock inputBlock(aInput);
   while (inputBlock.ReadCommand())
   {
      std::string command = inputBlock.GetCommand();
      if (command == "stop")
      {
         aInput.ReadValue(aWaypointAuxInput.mStop);
      }
      else if (command == "traffic_signals")
      {
         aInput.ReadValue(aWaypointAuxInput.mTrafficSignals);
      }
      else if (command == "highway")
      {
         aInput.ReadValue(aWaypointAuxInput.mHighway);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
}

// =================================================================================================
bool ProcessTrafficNetworkBlock(UtInput& aInput, RoadTrafficNetworkInput& aNetwork)
{
   bool         myCommand = true;
   UtInputBlock inputBlock(aInput);
   while (inputBlock.ReadCommand())
   {
      std::string command(aInput.GetCommand());
      if (command == "convoy")
      {
         aNetwork.mConvoyInput.emplace_back();
         ProcessConvoyBlock(aInput, aNetwork.mConvoyInput.back());
      }
      else if (command == "maximum_speed")
      {
         double maxSpeed;
         aInput.ReadValueOfType(maxSpeed, UtInput::cSPEED);
         aInput.ValueGreater(maxSpeed, 0.0);
         aNetwork.mMaxSpeed = maxSpeed;
      }
      else if (command == "vehicle")
      {
         std::string vehicleType;
         aInput.ReadValue(vehicleType);
         // Make sure we're not duplicating an existing specification
         for (auto& i : aNetwork.mVehicleInput)
         {
            if (vehicleType == i.mTypeId)
            {
               throw UtInput::BadValue(aInput, "Duplicate vehicle type specified: " + vehicleType);
            }
         }
         aNetwork.mVehicleInput.emplace_back(vehicleType);
         ProcessVehicleBlock(aInput, aNetwork.mVehicleInput.back());
      }
      else if (command == "route_aux_data_mapping")
      {
         ProcessRouteAuxMapBlock(aInput, aNetwork.mRouteAuxDataMap);
      }
      else if (command == "waypoint_aux_data_mapping")
      {
         ProcessWaypointAuxMapBlock(aInput, aNetwork.mWaypointAuxDataMap);
      }
      else if (command == "vehicle_count")
      {
         int vehicleCount;
         aInput.ReadValue(vehicleCount);
         aInput.ValueGreater(vehicleCount, 0);
         aNetwork.mVehicleCount = vehicleCount;
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   return myCommand;
}
} // namespace

// =================================================================================================
bool WsfOSM_TrafficInput::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   if (aInput.GetCommand() == "osm_traffic")
   {
      mProcessedInput = true;
      ProcessRoadTraffic(aInput);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
bool WsfOSM_TrafficInput::ProcessRoadTraffic(UtInput& aInput)
{
   bool         myCommand = true;
   UtInputBlock inputBlock(aInput);
   while (inputBlock.ReadCommand())
   {
      std::string command(aInput.GetCommand());
      if (command == "network")
      {
         std::string networkName;
         aInput.ReadValue(networkName);

         // Note: Duplicate network specifications are allowed. Each specification is treated
         // independently from other specifications.

         auto networkInputPtr = ut::make_unique<RoadTrafficNetworkInput>(networkName);
         ProcessTrafficNetworkBlock(aInput, *networkInputPtr);
         mNetworks.emplace_back(std::move(networkInputPtr));
      }
      else if (command == "debug")
      {
         mDebug = true;
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   return myCommand;
}
