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

#include "WsfCoverageOverlayOutput.hpp"

#include "WsfCoverageGrid.hpp"
#include "WsfCoverageMeasure.hpp"
#include "WsfCoverageTextOutputWriter.hpp"

namespace wsf
{
namespace coverage
{

bool OverlayOutput::Write(const std::string& aCoverageName, const std::vector<const Measure*>& aMeasures, const Grid* aGridPtr)
{
   bool             retval{false};
   TextOutputWriter writer{};
   if (writer.CreateContainingFolder(mOutputDir))
   {
      if (writer.OpenFile(mOutputDir, mFileName))
      {
         writer.SetRightJustified(false);
         writer.SetWidth(1);
         writer.SetFixedFormat(true);
         writer.SetStreamFormat();
         writer.SetStreamJustify();

         WriteHeader(writer, aCoverageName, aMeasures, aGridPtr);
         for (std::size_t block = 0; block < aGridPtr->GetNumComponents(); ++block)
         {
            WriteBlock(writer, aMeasures, aGridPtr, block);
         }

         writer.CloseStream();

         retval = true;
      }
   }
   return retval;
}

void OverlayOutput::WriteHeader(TextOutputWriter&                  aWriter,
                                const std::string&                 aCoverageName,
                                const std::vector<const Measure*>& aMeasures,
                                const Grid*                        aGridPtr)
{
   aWriter.WriteField(aCoverageName, true);
   aWriter.WriteField(aGridPtr->GetName(), true);
   aWriter.WriteField(aGridPtr->GetNumComponents(), true);
   aWriter.WriteField(aMeasures.size(), true);

   // Measure names
   for (std::size_t i = 0; i < aMeasures.size() - 1; ++i)
   {
      aWriter.WriteField(aMeasures[i]->GetName());
   }
   aWriter.WriteField(aMeasures[aMeasures.size() - 1]->GetName(), true);

   // Measure types
   for (std::size_t i = 0; i < aMeasures.size() - 1; ++i)
   {
      aWriter.WriteField(aMeasures[i]->GetType());
   }
   aWriter.WriteField(aMeasures[aMeasures.size() - 1]->GetType(), true);

   // Measure subtypes
   for (std::size_t i = 0; i < aMeasures.size() - 1; ++i)
   {
      aWriter.WriteField(aMeasures[i]->GetValueHeader());
   }
   aWriter.WriteField(aMeasures[aMeasures.size() - 1]->GetValueHeader(), true);
}

void OverlayOutput::WriteBlock(TextOutputWriter&                  aWriter,
                               const std::vector<const Measure*>& aMeasures,
                               const Grid*                        aGridPtr,
                               std::size_t                        aBlock)
{
   aWriter.WriteField(aGridPtr->GetComponentName(aBlock));
   auto size = aGridPtr->GetComponentSizes(aBlock);
   aWriter.WriteField(size.first);
   aWriter.WriteField(size.second, true);

   auto actionPtr = CreateGridAction(
      [&aWriter, &aMeasures](const Grid& aGrid, const Point& aPoint)
      {
         auto indices = aGrid.GetPointIndices(aPoint.GetID());
         aWriter.WriteField(indices.first);
         aWriter.WriteField(indices.second);
         aWriter.WriteField(aPoint.GetLatitude());
         aWriter.WriteField(aPoint.GetLongitude());
         aWriter.WriteField(aPoint.GetAltitude());
         GridAsset ga = aGrid.GetAsset(aPoint.GetID());
         for (std::size_t i = 0; i < aMeasures.size() - 1; ++i)
         {
            aWriter.WriteField(aMeasures[i]->GetMeasuredValue(ga));
         }
         aWriter.WriteField(aMeasures[aMeasures.size() - 1]->GetMeasuredValue(ga), true);
      });
   aGridPtr->InvokeOnComponent(*actionPtr, aBlock);
}

} // namespace coverage
} // namespace wsf
