// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2022 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef SIMULATIONDATA_HPP
#define SIMULATIONDATA_HPP

#include <map>

#include "RunData.hpp"
#include "parseAfperf.hpp"

namespace artificer
{
class SimulationData
{
public:
   ///@brief Updates data in this object to reflect the given `aRecord`
   void CollectStats(const artificer::AfperfRecord& aRecord);
   ///@brief Reduces the statistics associated with all runs in this object to a human readable log string and returns it.
   std::string RenderStatisticsSummary() const;

private:
   using RunID = uint64_t;
   ///@brief Gets the run ID associated with the given record - either from the record or last used (depending on event type)
   RunID GetRunID(const artificer::AfperfRecord& aRecord);

   std::map<RunID, RunData> mRuns;
   RunID                    mLastRunID = 0;
};

} // namespace artificer

#endif
