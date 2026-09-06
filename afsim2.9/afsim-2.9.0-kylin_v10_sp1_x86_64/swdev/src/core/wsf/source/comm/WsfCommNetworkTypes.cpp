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

#include "WsfCommNetworkTypes.hpp"

#include "UtMemory.hpp"
#include "WsfCommNetworkAdHoc.hpp"
#include "WsfCommNetworkGeneric.hpp"
#include "WsfCommNetworkMesh.hpp"
#include "WsfCommNetworkPointToPoint.hpp"
#include "WsfCommNetworkRing.hpp"
#include "WsfCommNetworkStar.hpp"
#include "WsfScenario.hpp"

namespace wsf
{
namespace comm
{

// =================================================================================================
NetworkTypes& NetworkTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetNetworkTypes();
}

// =================================================================================================
const NetworkTypes& NetworkTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetNetworkTypes();
}

// =================================================================================================
NetworkTypes::NetworkTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<Network>(aScenario, "network")
{
   //! Register all unique network types here.
   AddCoreType("WSF_COMM_NETWORK_AD_HOC", ut::make_unique<NetworkAdHoc>());
   AddCoreType("WSF_COMM_NETWORK_GENERIC", ut::make_unique<NetworkGeneric>());
   AddCoreType("WSF_COMM_NETWORK_P2P", ut::make_unique<NetworkPointToPoint>());
   AddCoreType("WSF_COMM_NETWORK_MESH", ut::make_unique<NetworkMesh>());
   AddCoreType("WSF_COMM_NETWORK_MESH_LEGACY", ut::make_unique<NetworkMeshLegacy>());
   AddCoreType("WSF_COMM_NETWORK_STAR", ut::make_unique<NetworkStar>());
   AddCoreType("WSF_COMM_NETWORK_RING", ut::make_unique<NetworkRing>());
   AddCoreType("WSF_COMM_NETWORK_RING_DIRECTED", ut::make_unique<NetworkDirectedRing>());
}

} // namespace comm
} // namespace wsf
