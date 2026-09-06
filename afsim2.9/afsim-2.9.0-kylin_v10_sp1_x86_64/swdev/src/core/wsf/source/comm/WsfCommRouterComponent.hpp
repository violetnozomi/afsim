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

#ifndef WSFCOMMROUTERCOMPONENT_HPP
#define WSFCOMMROUTERCOMPONENT_HPP

#include "wsf_export.h"

#include "WsfComponent.hpp"
class WsfSimulation;

namespace wsf
{
namespace comm
{

class NetworkManager;
class Comm;
class PhysicalLayer;

namespace router
{

class Router;

//! This is an abstract base class for router components.
//! Users wishing to add components to enhance or extend routing capabilities
//! by adding components can use this interface.
class WSF_EXPORT RouterComponent : public WsfComponentT<Router>
{
public:
   RouterComponent()           = default;
   ~RouterComponent() override = default;

   Router*         GetRouter() const { return GetComponentParent(); }
   WsfSimulation*  GetSimulation() const;
   NetworkManager* GetNetworkManager() const;

   using WsfComponent::Initialize;
   using WsfComponent::Initialize2;
   using WsfComponent::PreInitialize;
   using WsfComponent::ProcessInput;
   virtual void Setup() {}
   virtual void PendingStart() {}
};

} // namespace router
} // namespace comm
} // namespace wsf

#endif
