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

#ifndef WSFCOVERAGEMEASUREGRIDSTATSOUTPUT_HPP
#define WSFCOVERAGEMEASUREGRIDSTATSOUTPUT_HPP

#include "wsf_coverage_export.h"

#include "WsfCoverageMeasureTextOutput.hpp"

namespace wsf
{
namespace coverage
{

class WSF_COVERAGE_EXPORT MeasureGridStatsOutput : public MeasureTextOutput
{
public:
   MeasureGridStatsOutput() = default;
   explicit MeasureGridStatsOutput(std::unique_ptr<TextOutputWriter> aWriterPtr);
   MeasureGridStatsOutput(const MeasureGridStatsOutput& aOther) = default;
   ~MeasureGridStatsOutput() override                           = default;

   MeasureGridStatsOutput* Clone() const override { return new MeasureGridStatsOutput{*this}; }

   std::string GetOutputType() const override { return cTYPE; }

   constexpr static const char* cTYPE{"grid_stats"};

private:
   void        WriteData(const Coverage& aCoverage, const Measure& aMeasure) override;
   std::string DefaultFileSuffix() const override;
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGEMEASUREGRIDSTATSOUTPUT_HPP
