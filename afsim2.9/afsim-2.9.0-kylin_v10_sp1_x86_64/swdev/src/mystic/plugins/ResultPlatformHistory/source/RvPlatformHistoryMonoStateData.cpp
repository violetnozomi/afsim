// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "RvPlatformHistoryMonoStateData.hpp"

std::map<float, int> RvPlatformHistory::MonoStateData::GetStateChanges(float               aStart,
                                                                       float               aStop,
                                                                       int&                aStartState,
                                                                       rv::ResultPlatform* aPlatformData) const
{
   aStartState = 0;
   return {};
}
