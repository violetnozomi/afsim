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

#ifndef WSFCOVERAGEMEASURELATLONSTATSOUTPUT_HPP
#define WSFCOVERAGEMEASURELATLONSTATSOUTPUT_HPP

#include "wsf_coverage_export.h"

#include "WsfCoverageMeasureLatLonStats.hpp"
#include "WsfCoverageMeasureTextOutput.hpp"

namespace wsf
{
namespace coverage
{

//! A class to project data in latitude or longitude and produce summary statistics.
//!
//! This class collects projects out either latitude or longitude and then presents
//! summary statistics as a function of the remaining direction. The data in the
//! remaining direction is binned according to a user-provided bin size. Each instance
//! of this class will project in only one direction, so if both projections are needed,
//! two instances with different projections selected are needed.
class WSF_COVERAGE_EXPORT MeasureLatLonStatsOutput : public MeasureTextOutput
{
public:
   MeasureLatLonStatsOutput() = default;
   explicit MeasureLatLonStatsOutput(std::unique_ptr<TextOutputWriter> aWriterPtr);
   MeasureLatLonStatsOutput(const MeasureLatLonStatsOutput& aOther) = default;
   ~MeasureLatLonStatsOutput() override                             = default;

   MeasureLatLonStatsOutput* Clone() const override { return new MeasureLatLonStatsOutput{*this}; }
   bool                      ProcessInput(UtInput& aInput) override;

   //! Return the bin size in degrees.
   double GetBinSizeDeg() const { return mBinSizeDeg; }

   //! Return the direction onto which the data is projected.
   MeasureLatLonStats::Projection GetProjection() const { return mProjection; }

   std::string GetOutputType() const override { return cTYPE; }

   constexpr static const char* cTYPE{"lat_lon_stats"};

private:
   void        WriteData(const Coverage& aCoverage, const Measure& aMeasure) override;
   std::string DefaultFileSuffix() const override;

   MeasureLatLonStats::Projection mProjection{MeasureLatLonStats::Projection::cLONGITUDE};
   double                         mBinSizeDeg{1.0e-3 / 3600.0};
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGEMEASURELATLONSTATSOUTPUT_HPP
