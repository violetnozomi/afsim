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

#ifndef WSFCOVERAGEMEASURETEXTOUTPUT_HPP
#define WSFCOVERAGEMEASURETEXTOUTPUT_HPP

#include "wsf_coverage_export.h"

#include <string>

#include "UtCloneablePtr.hpp"
#include "WsfCoverageGridPoint.hpp"
#include "WsfCoverageMeasureOutput.hpp"
#include "WsfCoverageTextOutputWriter.hpp"

namespace wsf
{
namespace coverage
{

//! This class collects some common behavior for text output file for measure data.
//!
//! This class defers both settings management and the actual writing to streams to
//! an owned instance of a TextOutputWriter. This member has a sensible default, but
//! that default can be overridden. For example, a subclass of TextOutputWriter that
//! writes to a string stream instead of a file is useful for unit tests.
class WSF_COVERAGE_EXPORT MeasureTextOutput : public MeasureOutput
{
public:
   MeasureTextOutput();
   explicit MeasureTextOutput(std::unique_ptr<TextOutputWriter> aWriterPtr);
   MeasureTextOutput(const MeasureTextOutput& aOther) = default;
   ~MeasureTextOutput() override                      = default;

   bool ProcessInput(UtInput& aInput) override;
   void Write(const Coverage& aCoverage, const Measure& aMeasure) override;
   void SetFileNameToDefault(const Coverage& aCoverage, const Measure& aMeasure) override;

   //! Return the writer being used by this object.
   TextOutputWriter& GetWriter() { return *mWriterPtr.get(); }

private:
   virtual void        WriteData(const Coverage& aCoverage, const Measure& aMeasure) = 0;
   virtual std::string DefaultFileSuffix() const                                     = 0;

   ut::CloneablePtr<TextOutputWriter> mWriterPtr;
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGEMEASURETEXTOUTPUT_HPP
