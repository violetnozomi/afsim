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

#include "WsfCoverageMeasureCSV_Output.hpp"

#include "UtInput.hpp"
#include "WsfCoverage.hpp"
#include "WsfCoverageAsset.hpp"
#include "WsfCoverageGrid.hpp"
#include "WsfCoverageMeasure.hpp"

namespace wsf
{
namespace coverage
{

MeasureCSV_Output::MeasureCSV_Output(std::unique_ptr<TextOutputWriter> aWriterPtr)
   : MeasureTextOutput(std::move(aWriterPtr))
{
}

void MeasureCSV_Output::WriteData(const Coverage& aCoverage, const Measure& aMeasure)
{
   // Write header line
   GetWriter().WriteField("Lat. [deg]");
   GetWriter().WriteField("Lon. [deg]");
   GetWriter().WriteField("Alt. [m]");
   GetWriter().WriteField("Point ID");
   GetWriter().WriteField(aMeasure.GetValueHeader(), true);

   // Write lines
   auto writeLinePtr = CreateGridAction(
      [this, &aMeasure](const Grid& aGrid, const Point& aPoint)
      {
         GetWriter().WriteField(aPoint.GetLatitude());
         GetWriter().WriteField(aPoint.GetLongitude());
         GetWriter().WriteField(aPoint.GetAltitude());
         GetWriter().WriteField(aPoint.GetID());
         GetWriter().WriteField(aMeasure.GetMeasuredValue(aGrid.GetAsset(aPoint.GetID())), true);
      });
   aCoverage.GetGrid()->Invoke(*writeLinePtr);
}

std::string MeasureCSV_Output::DefaultFileSuffix() const
{
   return "data.csv";
}

} // namespace coverage
} // namespace wsf
