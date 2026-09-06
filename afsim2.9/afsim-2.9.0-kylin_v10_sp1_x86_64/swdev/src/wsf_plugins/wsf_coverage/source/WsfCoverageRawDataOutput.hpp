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

#ifndef WSFCOVERAGERAWDATAOUTPUT_HPP
#define WSFCOVERAGERAWDATAOUTPUT_HPP

#include <vector>

class UtCalendar;

#include "wsf_coverage_export.h"

#include "WsfCoverageAccessInterval.hpp"
#include "WsfCoverageAsset.hpp"
#include "WsfCoverageTextOutputWriter.hpp"

namespace wsf
{
namespace coverage
{

//! This object will store and output to file the raw access interval data for a coverage computation.
class WSF_COVERAGE_EXPORT RawDataOutput
{
public:
   explicit RawDataOutput(const std::string& aFileName)
      : mFileName{aFileName}
   {
   }
   RawDataOutput(const RawDataOutput& aOther) = default;

   RawDataOutput* Clone() { return new RawDataOutput{*this}; }

   void AddInterval(const GridAsset& aGridAsset, const FreeAsset& aFreeAsset, const AccessInterval& aInterval);

   //! Return the number of stored intervals.
   std::size_t GetNumIntervals() const { return mAccessData.size(); }

   void Write(const UtCalendar& aSimStartEpoch) const;
   void Write(TextOutputWriter& aWriter, const UtCalendar& aSimStartEpoch) const;

   //! Return the name of the file into which the data will be written.
   const std::string& GetFileName() const { return mFileName; }

   //! Return the directory into which the data file will be written.
   const std::string& GetOutputDir() const { return mOutputDir; }

   //! Set the name of the file into which the data will be written.
   void SetFileName(const std::string& aFileName) { mFileName = aFileName; }

   //! Set the directory into which the data file will be written.
   void SetOutputDir(const std::string& aOutputDir) { mOutputDir = aOutputDir; }

private:
   void WriteData(TextOutputWriter& aWriter, const UtCalendar& aSimStartEpoch) const;

   std::vector<std::tuple<GridAsset, FreeAsset, AccessInterval>> mAccessData{};
   std::string                                                   mFileName;
   std::string                                                   mOutputDir{};
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGERAWDATAOUTPUT_HPP
