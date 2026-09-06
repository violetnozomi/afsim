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

#ifndef WSFZONEATTENUATION_HPP
#define WSFZONEATTENUATION_HPP

#include "wsf_export.h"

#include <map>
#include <vector>

class WsfNoiseCloud;
class WsfScenario;
class WsfSimulation;
#include "WsfStringId.hpp"
class WsfZone;

class WSF_EXPORT WsfZoneAttenuation
{
public:
   explicit WsfZoneAttenuation(WsfSimulation* aSimulationPtr);
   ~WsfZoneAttenuation();

   double ComputeAttenuation(WsfStringId aCategoryId, double aLocationFromLLA[3], double aLocationToLLA[3], bool aIsTwoWay);
   bool Initialize(const WsfScenario& aScenario);

private:
   WsfSimulation*                                     mSimulationPtr;
   std::map<WsfStringId, std::vector<WsfZone*>>       mCategoryToZoneMap;
   std::map<WsfStringId, double>                      mCategoryToValueMap;
   std::map<WsfStringId, std::vector<WsfNoiseCloud*>> mCategoryToNoiseCloudMap;
   std::vector<WsfNoiseCloud*>                        mNoiseClouds;
};

#endif
