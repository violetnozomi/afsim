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
#ifndef RvPlatformHistoryBehaviorStateData_HPP
#define RvPlatformHistoryBehaviorStateData_HPP

#include <map>
#include <string>

#include "RvPlatformHistoryStateData.hpp"

namespace rv
{
class ResultPlatform;
}

namespace RvPlatformHistory
{
/**
 * Handles state data from a platform's behavior tree.
 */
class BehaviorStateData : public StateData
{
public:
   BehaviorStateData(rv::ResultPlatform* aPlatform, std::map<std::string, int>& aStateNames);

   std::map<float, int> GetStateChanges(float               aStart,
                                        float               aStop,
                                        int&                aStartState,
                                        rv::ResultPlatform* aPlatformData = nullptr) const override;

private:
   std::map<std::string, int>&         mStateNames;
   std::map<unsigned int, std::string> mBehaviorMap;
};
} // namespace RvPlatformHistory
#endif // RvPlatformHistoryBehaviorStateData_HPP
