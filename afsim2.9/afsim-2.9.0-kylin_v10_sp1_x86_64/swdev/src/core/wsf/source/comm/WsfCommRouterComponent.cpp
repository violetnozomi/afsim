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

#include "WsfCommRouterComponent.hpp"

#include "WsfComm.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommPhysicalLayer.hpp"
#include "WsfCommRouter.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace comm
{
namespace router
{

// =================================================================================================
WsfSimulation* RouterComponent::GetSimulation() const
{
   return GetComponentParent()->GetSimulation();
}

// =================================================================================================
NetworkManager* RouterComponent::GetNetworkManager() const
{
   auto routerPtr = GetRouter();
   if (!routerPtr)
   {
      return nullptr;
   }
   return routerPtr->GetNetworkManager();
}

} // namespace router
} // namespace comm
} // namespace wsf
