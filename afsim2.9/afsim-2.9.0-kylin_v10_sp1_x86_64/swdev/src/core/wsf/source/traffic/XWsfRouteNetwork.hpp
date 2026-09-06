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

#ifndef XWSFROUTENETWORK_HPP
#define XWSFROUTENETWORK_HPP

#include "wsf_export.h"

#include <string>
#include <vector>

#include "UtInput.hpp"
#include "UtMemory.hpp"
#include "UtRandom.hpp"
#include "WsfRouteNetwork.hpp"
class WsfScenario;

//! XWsfRouteNetwork provides an abstract base for applications that need to use a route network.
//! This class supports multiple route networks and stores the input data into mNetworks (protected).
//! It provides an input processor and can process basic route network parameters. Applications
//! that extend XWsfRouteNetwork can also inherit from the NetworkInput class to add any needed
//! parameters (XWsfRoadTraffic is an example).

class WSF_EXPORT XWsfRouteNetwork
{
public:
   XWsfRouteNetwork()          = default;
   virtual ~XWsfRouteNetwork() = default;

   //! If Initialize is redefined, make sure to call XWsfRouteNetwork::Initialize().
   virtual bool Initialize(const WsfScenario& aScenario);

   //! An update method for applications that need simulation updates.
   virtual void Update(double /*aSimTime*/) {}

   //! If ProcessInput is redefined, do no call XWsfRouteNetwork::Initialize().
   //! Instead derived classes must call ProcessCommand for unknown commands
   //! in the input stream.
   virtual bool ProcessInput(UtInput& aInput);

   //! A NetworkInput object is created for each network defined in the input file.  It holds
   //! data that is relevant to anything in its network, such as roads and vehicles.
   class NetworkInput
   {
   public:
      NetworkInput(WsfStringId aNetworkId)
         : mRouteNetworkPtr(nullptr)
         , mNetworkId(aNetworkId)
         , mVehicleCount(0)
         , mVehicleDensity(0.0)
      {
      }

      virtual ~NetworkInput() = default;

      virtual NetworkInput* Clone() const noexcept { return new NetworkInput{*this}; }

      virtual bool Initialize() { return true; }

      virtual void VerifyData(UtInput& aInput)
      {
         if ((mVehicleCount == 0) && (mVehicleDensity == 0.0))
         {
            throw UtInput::BadValue(aInput, "XWsfRouteNetwork must specify either 'vehicle_count' or 'vehicle_density'");
         }

         if ((mVehicleCount != 0) && (mVehicleDensity != 0.0))
         {
            throw UtInput::BadValue(aInput, "XWsfRouteNetwork cannot specify both 'vehicle_count' and 'vehicle_density'");
         }
      }

      WsfRouteNetwork* mRouteNetworkPtr; // A pointer to the route network that mNetworkId represents.
      // Note: This is only valid after simulation initialization.

      WsfStringId mNetworkId;      // String Id of the name of the road network
      int         mVehicleCount;   // Total number of vehicles
      double      mVehicleDensity; // Vehicle density (vehicle / meter)

   protected:
      NetworkInput(const NetworkInput& aRhs) = default;
   };

protected:
   XWsfRouteNetwork(const XWsfRouteNetwork& aSrc) = default;
   XWsfRouteNetwork& operator=(const XWsfRouteNetwork& aRhs) = delete;

   virtual bool ProcessCommand(UtInput& aInput, NetworkInput& aNetworkInput);

   using Networks = std::vector<ut::CloneablePtr<NetworkInput>>;

   ut::Random mRandom;
   Networks   mNetworks;

private:
   bool ProcessNetworkBlock(UtInput& aInput, NetworkInput& aNetworkInput);
};

#endif
