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
#include "SimulationData.hpp"

#include <sstream>

#include "RunData.hpp"

namespace artificer
{
void SimulationData::CollectStats(const artificer::AfperfRecord& aRecord)
{
   const RunID curRunID = GetRunID(aRecord);
   RunData&    run      = mRuns[curRunID];
   mLastRunID           = curRunID;
   run.CollectStats(aRecord);
}

std::string SimulationData::RenderStatisticsSummary() const
{
   std::stringstream stream;
   const size_t      dividerWidth = 140;
   const std::string runDivider(dividerWidth, '=');
   for (auto& run : mRuns)
   {
      stream << runDivider << std::endl
             << std::endl
             << "   RunID " << run.first << std::endl
             << std::endl
             << std::string(dividerWidth, '-') << std::endl
             << run.second.RenderStatisticsSummary();
   }
   stream << runDivider << std::endl;
   return stream.str();
}

SimulationData::RunID SimulationData::GetRunID(const artificer::AfperfRecord& aRecord)
{
   const auto evtType       = RecordTypeFrom(aRecord);
   size_t     runIdFieldIdx = 0;
   switch (evtType)
   {
   case V1RecordType::RunInfo:
      runIdFieldIdx = 5;
      break;

   case V1RecordType::RunAggregate:
      runIdFieldIdx = 3;
      break;

   case V1RecordType::MeasurementType:
   case V1RecordType::SectionInfo:
   case V1RecordType::RunPoint:
   case V1RecordType::RegionStart:
      runIdFieldIdx = 2;
      break;

   default:
      return mLastRunID;
   }
   if (runIdFieldIdx >= aRecord.size())
   {
      throw std::runtime_error("Expected to find run ID field at index " + std::to_string(runIdFieldIdx) +
                               " for record type " + std::to_string(static_cast<int>(evtType)) +
                               " but record length is " + std::to_string(aRecord.size()));
   }
   mLastRunID = stoull(aRecord[runIdFieldIdx]);
   return mLastRunID;
}

} // namespace artificer
