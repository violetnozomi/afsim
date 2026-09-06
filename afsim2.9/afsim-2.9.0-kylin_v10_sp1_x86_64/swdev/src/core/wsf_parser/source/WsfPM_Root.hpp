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

#ifndef WSFPM_ROOT_HPP
#define WSFPM_ROOT_HPP

#include "WsfPM_Platform.hpp"
#include "WsfPM_Scenario.hpp"
#include "WsfPM_Simulation.hpp"
#include "WsfPM_Zone.hpp"

class WSF_PARSER_EXPORT WsfPM_Root : public WsfPProxyNode
{
public:
   WsfPM_Root() {}
   WsfPM_Root(WsfPProxyNode aRootNode)
      : WsfPProxyNode(aRootNode)
   {
   }
   WsfPM_Root(WsfPProxy* aProxy)
      : WsfPProxyNode(WsfPProxyNode::FromProxy(aProxy))
   {
   }

   WsfPM_PlatformMap  platforms() const { return WsfPM_PlatformMap(*this + "platform"); }
   WsfPM_PlatformMap  platformTypes() const { return WsfPM_PlatformMap(*this + "platformType"); }
   WsfPM_SensorMap    sensorTypes() const { return WsfPM_SensorMap(*this + "sensorType"); }
   WsfPM_CommMap      commTypes() const { return WsfPM_CommMap(*this + "commType"); }
   WsfPM_ProcessorMap processorTypes() const { return WsfPM_ProcessorMap(*this + "processorType"); }
   WsfPM_MoverMap     moverTypes() const { return WsfPM_MoverMap(*this + "moverType"); }
   WsfPM_FuelMap      fuelTypes() const { return WsfPM_FuelMap(*this + "fuelType"); }
   WsfPM_RouteMap     routes() const { return WsfPM_RouteMap(*this + "routes"); }
   WsfPM_ZoneMap      zones() const { return WsfPM_ZoneMap(*this + "zone"); }

   WsfPM_Scenario   scenario() const { return WsfPM_Scenario(*this + "scenario"); }
   WsfPM_Simulation simulation() const { return WsfPM_Simulation(*this + "scenario" + "simulationInput"); }

   WsfPM_ObjectMap antennaPatterns() const { return WsfPM_ObjectMap(*this + "antennaPattern"); }
   WsfPM_ObjectMap infraredSignatures() const { return WsfPM_ObjectMap(*this + "infraredSignature"); }
   WsfPM_ObjectMap acousticSignatures() const { return WsfPM_ObjectMap(*this + "acousticSignature"); }
   WsfPM_ObjectMap opticalSignatures() const { return WsfPM_ObjectMap(*this + "opticalSignature"); }
   WsfPM_ObjectMap radarSignatures() const { return WsfPM_ObjectMap(*this + "radarSignature"); }
   WsfPM_ObjectMap reflectivitySignatures() const { return WsfPM_ObjectMap(*this + "reflectivitySignature"); }
   WsfPM_ObjectMap inherentContrasts() const { return WsfPM_ObjectMap(*this + "inherentContrast"); }
   WsfPM_ObjectMap networkTypes() const { return WsfPM_ObjectMap(*this + "networkType"); }
};

#endif
