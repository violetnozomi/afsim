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

#include "WsfCoverageMeasureGridStatsOutput.hpp"

#include "WsfCoverage.hpp"
#include "WsfCoverageMeasure.hpp"
#include "WsfCoverageMeasureGridStats.hpp"

namespace wsf
{
namespace coverage
{

MeasureGridStatsOutput::MeasureGridStatsOutput(std::unique_ptr<TextOutputWriter> aWriterPtr)
   : MeasureTextOutput(std::move(aWriterPtr))
{
}

void MeasureGridStatsOutput::WriteData(const Coverage& aCoverage, const Measure& aMeasure)
{
   MeasureGridStats stats{aCoverage, aMeasure};

   // Header line
   GetWriter().WriteField("Min. Value");
   GetWriter().WriteField("Max. Value");
   GetWriter().WriteField("Mean Value");
   GetWriter().WriteField("Count", true);

   // Do stats
   GetWriter().WriteField(stats.GetMinValue());
   GetWriter().WriteField(stats.GetMaxValue());
   GetWriter().WriteField(stats.GetMeanValue());
   GetWriter().WriteField(stats.GetCount(), true);
}

std::string MeasureGridStatsOutput::DefaultFileSuffix() const
{
   return "grid_stats.csv";
}

} // namespace coverage
} // namespace wsf
