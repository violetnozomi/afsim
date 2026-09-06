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

#ifndef WSFCOVERAGEMEASURECSVOUTPUT_HPP
#define WSFCOVERAGEMEASURECSVOUTPUT_HPP

#include "wsf_coverage_export.h"

#include "WsfCoverageMeasureTextOutput.hpp"

namespace wsf
{
namespace coverage
{

//! Write the measued data out, with one line per grid point.
class WSF_COVERAGE_EXPORT MeasureCSV_Output : public MeasureTextOutput
{
public:
   MeasureCSV_Output() = default;
   explicit MeasureCSV_Output(std::unique_ptr<TextOutputWriter> aWriterPtr);
   MeasureCSV_Output(const MeasureCSV_Output& aOther) = default;
   ~MeasureCSV_Output() override                      = default;

   MeasureCSV_Output* Clone() const override { return new MeasureCSV_Output{*this}; }

   std::string GetOutputType() const override { return cTYPE; }

   constexpr static const char* cTYPE{"data"};

private:
   void        WriteData(const Coverage& aCoverage, const Measure& aMeasure) override;
   std::string DefaultFileSuffix() const override;
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGEMEASURECSVOUTPUT_HPP
