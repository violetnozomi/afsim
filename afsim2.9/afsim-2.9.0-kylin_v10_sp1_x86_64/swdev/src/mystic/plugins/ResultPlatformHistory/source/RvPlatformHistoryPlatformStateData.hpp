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
#ifndef RvPlatformHistoryPlatformStateData_HPP
#define RvPlatformHistoryPlatformStateData_HPP

#include <map>

#include "RvPlatformHistoryStateData.hpp"
#include "RvResultPlatform.hpp"

namespace RvPlatformHistory
{
/**
 * Handles "platform states", such as normal, detected, tracked, etc.
 */
class PlatformStateData : public StateData
{
public:
   std::map<float, int> GetStateChanges(float               aStart,
                                        float               aStop,
                                        int&                aStartState,
                                        rv::ResultPlatform* aPlatformData = nullptr) const override;
};
} // namespace RvPlatformHistory
#endif // RvPlatformHistoryPlatformStateData_HPP
