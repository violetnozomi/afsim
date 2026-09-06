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

#include "sa_display/WkfSA_DisplayDataContainer.hpp"

wkf::SA_Display::DataContainer::DataContainer(const wkf::SA_Display::DataContainer& aSrc)
   : mSA_Data(aSrc.mSA_Data)
   , mSA_TruthPlatforms(aSrc.mSA_TruthPlatforms)
   , mSimulationInitialized(aSrc.mSimulationInitialized)
{
}

void wkf::SA_Display::DataContainer::SetSA_TruthPlatforms(const std::vector<SA_Display::EntityTruth>& aSA_TruthPlatforms)
{
   mSA_TruthPlatforms = aSA_TruthPlatforms;
}

void wkf::SA_Display::DataContainer::SetSA_Data(const std::map<size_t, SA_Data>& aSA_Data)
{
   mSA_Data = aSA_Data;
}
