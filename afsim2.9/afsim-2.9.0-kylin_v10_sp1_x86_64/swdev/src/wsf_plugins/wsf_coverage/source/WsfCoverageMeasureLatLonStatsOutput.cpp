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

#include "WsfCoverageMeasureLatLonStatsOutput.hpp"

#include <string>

#include "UtInput.hpp"
#include "UtUnitTypes.hpp"
#include "WsfCoverage.hpp"

namespace wsf
{
namespace coverage
{

MeasureLatLonStatsOutput::MeasureLatLonStatsOutput(std::unique_ptr<TextOutputWriter> aWriterPtr)
   : MeasureTextOutput(std::move(aWriterPtr))
{
}

bool MeasureLatLonStatsOutput::ProcessInput(UtInput& aInput)
{
   bool        myCommand{true};
   std::string command;
   aInput.GetCommand(command);
   if (command == "bin_size")
   {
      UtAngleValue binSize;
      aInput.ReadUnitValue(binSize);
      mBinSizeDeg = binSize.GetAsUnit(UtUnitAngle::cDEGREES);
      aInput.ValueGreater(mBinSizeDeg, 0.0);
   }
   else if (command == "latitude")
   {
      mProjection = MeasureLatLonStats::Projection::cLATITUDE;
   }
   else if (command == "longitude")
   {
      mProjection = MeasureLatLonStats::Projection::cLONGITUDE;
   }
   else
   {
      myCommand = MeasureTextOutput::ProcessInput(aInput);
   }
   return myCommand;
}

void MeasureLatLonStatsOutput::WriteData(const Coverage& aCoverage, const Measure& aMeasure)
{
   MeasureLatLonStats stats{aCoverage, aMeasure, mProjection, mBinSizeDeg};

   // Header Line
   GetWriter().WriteField(stats.GetProjectionDescription());
   GetWriter().WriteField("Min. Value");
   GetWriter().WriteField("Max. Value");
   GetWriter().WriteField("Mean Value");
   GetWriter().WriteField("Count", true);

   // Records
   for (std::size_t i = 0; i < stats.GetSize(); ++i)
   {
      GetWriter().WriteField(stats.GetVariable(i));
      GetWriter().WriteField(stats.GetMinValue(i));
      GetWriter().WriteField(stats.GetMaxValue(i));
      GetWriter().WriteField(stats.GetMeanValue(i));
      GetWriter().WriteField(stats.GetCount(i), true);
   }
}

std::string MeasureLatLonStatsOutput::DefaultFileSuffix() const
{
   std::string retval;
   if (mProjection == MeasureLatLonStats::Projection::cLATITUDE)
   {
      retval = "lat_stats.csv";
   }
   else
   {
      retval = "lon_stats.csv";
   }
   return retval;
}

} // namespace coverage
} // namespace wsf
