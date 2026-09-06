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
#ifndef RvPlatformHistoryMonoStateData_HPP
#define RvPlatformHistoryMonoStateData_HPP

#include <map>

#include "RvPlatformHistoryStateData.hpp"
#include "RvResultPlatform.hpp"

namespace RvPlatformHistory
{
/**
 * A state data implementation for situations with only one state such as team color.
 */
class MonoStateData : public StateData
{
public:
   std::map<float, int> GetStateChanges(float               aStart,
                                        float               aStop,
                                        int&                aStartState,
                                        rv::ResultPlatform* aPlatformData = nullptr) const override;
};
} // namespace RvPlatformHistory
#endif // RvPlatformHistoryMonoStateData_HPP
