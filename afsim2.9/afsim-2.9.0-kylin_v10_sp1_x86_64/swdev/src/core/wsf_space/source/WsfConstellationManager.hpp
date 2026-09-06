// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCONSTELLATIONMANAGER_HPP
#define WSFCONSTELLATIONMANAGER_HPP

#include <map>
#include <memory>
#include <string>
#include <vector>

class WsfSimulation;
#include "WsfConstellation.hpp"
#include "WsfSimulationExtension.hpp"

//! A simulation extension to manage WsfConstellation objects.
class WsfConstellationManager : public WsfSimulationExtension
{
public:
   WsfConstellationManager()           = default;
   ~WsfConstellationManager() override = default;

   bool                           AddConstellation(std::unique_ptr<WsfConstellation> aConstellationPtr);
   WsfConstellation*              FindConstellation(const std::string& aConstellationName) const;
   std::vector<WsfConstellation*> GetConstellations() const;

   static WsfConstellationManager& Get(const WsfSimulation* aSimulation);

   constexpr static const char* cEXTENSION_NAME{"wsf_constellation_manager"};

private:
   std::map<std::string, std::unique_ptr<WsfConstellation>> mConstellations{};
};

#endif // WSFCONSTELLATIONMANAGER_HPP
