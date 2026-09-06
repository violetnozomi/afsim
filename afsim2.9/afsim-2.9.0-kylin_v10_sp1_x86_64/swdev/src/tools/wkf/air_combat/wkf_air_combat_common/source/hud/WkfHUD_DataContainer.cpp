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

#include "WkfHUD_DataContainer.hpp"

wkf::HUD_DataContainer::HUD_DataContainer(const HUD_DataContainer& aSrc)
   : mSimulationInitialized(aSrc.mSimulationInitialized)
   , mPlatformDatas(aSrc.mPlatformDatas)
{
}

bool wkf::HUD_DataContainer::GetSimulationInitialized() const
{
   return mSimulationInitialized;
}

void wkf::HUD_DataContainer::SetSimulationInitialized(bool aValue)
{
   mSimulationInitialized = aValue;
}

void wkf::HUD_DataContainer::SetPlatformData(const std::map<std::string, PlatformData>& aPlatformDatas)
{
   mPlatformDatas = aPlatformDatas;
   emit Update_DataContainer();
}
