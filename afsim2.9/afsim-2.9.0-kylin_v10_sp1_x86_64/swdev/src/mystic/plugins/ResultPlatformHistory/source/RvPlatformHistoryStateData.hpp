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
#ifndef RvPlatformHistoryStateData_HPP
#define RvPlatformHistoryStateData_HPP

#include <map>

#include "RvResultPlatform.hpp"

namespace RvPlatformHistory
{

/**
 * An abstract base class for containing state data.
 */
class StateData
{
public:
   static constexpr int NO_STATE = 0;

   /**
    * Get all state changes within the given time frame.
    * @param aStart         The starting time of the time frame.
    * @param aStop          The ending time of the time frame.
    * @param aStartState    The initial state at the start of the time frame. This is an output parameter.
    * @param aPlatformData  The ResultPlatform with which this state data is associated.
    *                       It is assumed that aPlatformData.BeginRead() and aPlatformData.EndRead() are called by the caller.
    * @return               A mapping from the times that state changes happen to the new state.
    */
   virtual std::map<float, int>
   GetStateChanges(float aStart, float aStop, int& aStartState, rv::ResultPlatform* aPlatformData = nullptr) const = 0;
};
} // namespace RvPlatformHistory
#endif // RvPlatformHistoryStateData_HPP
