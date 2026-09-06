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

#include "WsfCoverageRawDataOutput.hpp"

#include <tuple>

#include "UtCalendar.hpp"
#include "UtLog.hpp"
#include "UtPath.hpp"

namespace wsf
{
namespace coverage
{

//! Add an interval to the stored data.
//!
//! This will add the given @p aInterval between for an interaction between the
//! given @p aGridAsset and the given @p aFreeAsset.
//!
//! \param aGridAsset - The grid asset participating in the interaction.
//! \param aFreeAsset - The free asset participating in the interaction.
//! \param aInterval  - The interaction interval.
void RawDataOutput::AddInterval(const GridAsset& aGridAsset, const FreeAsset& aFreeAsset, const AccessInterval& aInterval)
{
   mAccessData.emplace_back(aGridAsset, aFreeAsset, aInterval);
}

//! Write the data to file.
//!
//! \param aSimStartEpoch - The date and time of the start of the simulation.
void RawDataOutput::Write(const UtCalendar& aSimStartEpoch) const
{
   TextOutputWriter writer{};
   Write(writer, aSimStartEpoch);
}

//! Write the data using the given output writer.
//!
//! This will put the data out using the given @p aWriter. This is primarily useful for
//! unit testing.
//!
//! \param aWriter        - The text output writer to use to output the data in this object.
//! \param aSimStartEpoch - The date and time of the start of the simulation.
void RawDataOutput::Write(TextOutputWriter& aWriter, const UtCalendar& aSimStartEpoch) const
{
   if (aWriter.CreateContainingFolder(mOutputDir))
   {
      if (aWriter.OpenFile(mOutputDir, mFileName))
      {
         aWriter.SetRightJustified(true);
         aWriter.SetFixedFormat(true);
         aWriter.SetWidth(18);
         aWriter.SetStreamFormat();
         aWriter.SetStreamJustify();

         WriteData(aWriter, aSimStartEpoch);

         aWriter.CloseStream();
      }
   }
}

void RawDataOutput::WriteData(TextOutputWriter& aWriter, const UtCalendar& aSimStartEpoch) const
{
   aWriter.WriteField("Grid Asset ID");
   aWriter.WriteField("Grid Asset Device");
   aWriter.WriteField("Free Asset Name");
   aWriter.WriteField("Free Asset Device");
   aWriter.WriteFieldWidth("Start Epoch", 25);
   aWriter.WriteFieldWidth("End Epoch", 25);
   aWriter.WriteField("Start [s]");
   aWriter.WriteField("End [s]");
   aWriter.WriteField("Duration [s]", true);

   for (auto& iter : mAccessData)
   {
      aWriter.WriteField(std::get<0>(iter).GetPointId());
      aWriter.WriteField(std::get<0>(iter).GetDeviceName());

      aWriter.WriteField(std::get<1>(iter).GetPlatformName());
      aWriter.WriteField(std::get<1>(iter).GetDeviceName());

      double     start = std::get<2>(iter).GetStart();
      double     end   = std::get<2>(iter).GetEnd();
      UtCalendar startEpoch{aSimStartEpoch};
      startEpoch.AdvanceTimeBy(start);
      UtCalendar endEpoch{aSimStartEpoch};
      endEpoch.AdvanceTimeBy(end);
      aWriter.WriteFieldWidth(startEpoch.GetISO8601(), 25);
      aWriter.WriteFieldWidth(endEpoch.GetISO8601(), 25);
      aWriter.WriteField(start);
      aWriter.WriteField(end);
      aWriter.WriteField(std::get<2>(iter).GetDuration(), true);
   }
}

} // namespace coverage
} // namespace wsf
