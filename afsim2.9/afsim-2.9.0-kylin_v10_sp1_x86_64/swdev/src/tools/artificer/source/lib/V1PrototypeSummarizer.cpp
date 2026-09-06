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
#include "V1PrototypeSummarizer.hpp"

#include <iostream>

#include "SimulationData.hpp"

namespace artificer
{
V1PrototypeSummarizer::V1PrototypeSummarizer()
   : mSimulationData(new SimulationData)
{
}
V1PrototypeSummarizer::~V1PrototypeSummarizer() = default;

void V1PrototypeSummarizer::Summarize(const artificer::AfperfRecord& aRecord)
{
   mSimulationData->CollectStats(aRecord);
}

void V1PrototypeSummarizer::EndOfRecords()
{
   std::cout << mSimulationData->RenderStatisticsSummary() << std::endl;
}
} // namespace artificer
