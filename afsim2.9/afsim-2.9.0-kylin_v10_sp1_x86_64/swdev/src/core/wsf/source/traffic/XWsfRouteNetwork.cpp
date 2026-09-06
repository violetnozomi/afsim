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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "traffic/XWsfRouteNetwork.hpp"

#include <algorithm>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfRouteNetworkTypes.hpp"
#include "WsfStringId.hpp"
// virtual
//! Verifies and initializes each of the route networks
bool XWsfRouteNetwork::Initialize(const WsfScenario& aScenario)
{
   bool ok = true;

   for (const auto& netPtr : mNetworks)
   {
      netPtr->mRouteNetworkPtr = WsfRouteNetworkTypes::Get(aScenario).Find(netPtr->mNetworkId);
      if (netPtr->mRouteNetworkPtr == nullptr)
      {
         auto out = ut::log::error() << "Invalid route network.";
         out.AddNote() << "Network: " << netPtr->mNetworkId;
         return false;
      }

      if (!netPtr->mRouteNetworkPtr->Initialize())
      {
         auto out = ut::log::error() << "Unable to initialize route network.";
         out.AddNote() << "Network: " << netPtr->mNetworkId;
         return false;
      }

      // If no vehicles were specified use the density to calculate count.
      if (netPtr->mVehicleCount == 0)
      {
         double distance = 0.0;
         for (unsigned int ii = 0; ii < netPtr->mRouteNetworkPtr->GetRouteCount(); ++ii)
         {
            distance += (*netPtr->mRouteNetworkPtr)[ii].GetTotalLength();
         }
         netPtr->mVehicleCount = static_cast<int>(netPtr->mVehicleDensity * distance);

         auto out = ut::log::info() << "Unspecified 'vehicle_count'. Setting it based on input density.";
         out.AddNote() << "New Value: " << netPtr->mVehicleCount;
      }
   }
   return ok;
}

// virtual
//! Processes basic commands in a network end_network block.
//! Note: If a derived class redefines ProcessInput, this version
//! should not be called.  Instead ProcessCommand should be used (see below).
bool XWsfRouteNetwork::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;

   std::string command(aInput.GetCommand());

   if (command == "network")
   {
      std::string networkName;
      aInput.ReadValue(networkName);

      // Note: Duplicate network specifications are allowed.  Each specification is treated
      // independently from other specifications.

      mNetworks.emplace_back(ut::make_unique<NetworkInput>(networkName));
      ProcessNetworkBlock(aInput, *mNetworks.back());
   }
   else
   {
      if (!mNetworks.empty())
      {
         myCommand = ProcessCommand(aInput, *mNetworks.back());
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }

   if (!myCommand)
   {
      throw UtInput::UnknownCommand(aInput);
   }

   return true;
}

// protected
//! Derived classes call ProcessCommand for commands that exist in their
//! network end_network blocks that aren't recognized.  If the command
//! isn't recognized at this level an exception is thrown.
bool XWsfRouteNetwork::ProcessCommand(UtInput& aInput, NetworkInput& aNetworkInput)
{
   bool myCommand = true;

   std::string command;
   aInput.GetCommand(command);

   if (command == "vehicle_count")
   {
      int vehicleCount;
      aInput.ReadValue(vehicleCount);
      aInput.ValueGreater(vehicleCount, 0);
      aNetworkInput.mVehicleCount = vehicleCount;
   }
   else if (command == "vehicle_density")
   {
      // density is as vehicle_density <#> per <length-unit>
      double      vehicleDensity;
      std::string per;
      std::string units;
      aInput.ReadValue(vehicleDensity);
      aInput.ReadValue(per);
      aInput.StringEqual(per, "per");
      aInput.ReadValue(units);
      double multiplier = aInput.ConvertValue(1.0, units, UtInput::cLENGTH);
      aInput.ValueGreater(vehicleDensity, 0.0);
      aNetworkInput.mVehicleDensity = vehicleDensity / multiplier;
   }
   else
   {
      throw UtInput::UnknownCommand(aInput);
   }
   return myCommand;
}

// private
bool XWsfRouteNetwork::ProcessNetworkBlock(UtInput& aInput, NetworkInput& aNetworkInput)
{
   bool myCommand = true;

   UtInputBlock inputBlock(aInput, "end_network");
   while (inputBlock.ReadCommand())
   {
      myCommand = ProcessCommand(aInput, aNetworkInput);
   }

   return myCommand;
}
