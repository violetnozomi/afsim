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
#ifndef PROTOTYPESUMMARIZERINTERFACE_HPP
#define PROTOTYPESUMMARIZERINTERFACE_HPP

#include <memory>

#include "AfperfSummarizerInterface.hpp"

namespace artificer
{
class SimulationData;

///@brief Collects basic statistics about runs in the data and outputs a summary of each run at the end of the data.
class V1PrototypeSummarizer : public AfperfSummarizerInterface
{
public:
   V1PrototypeSummarizer();
   ~V1PrototypeSummarizer() override;

   ///@brief Updates internal data to reflect the given `aRecord`
   void Summarize(const artificer::AfperfRecord& aRecord) override;
   ///@brief Outputs final statistics
   void EndOfRecords() override;

private:
   std::unique_ptr<SimulationData> mSimulationData;
};
} // namespace artificer

#endif
