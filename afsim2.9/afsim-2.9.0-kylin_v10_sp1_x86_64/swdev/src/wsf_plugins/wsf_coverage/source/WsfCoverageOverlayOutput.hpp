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

#ifndef WSFCOVERAGEOVERLAYOUTPUT_HPP
#define WSFCOVERAGEOVERLAYOUTPUT_HPP

#include <string>
#include <vector>

#include "UtCloneablePtr.hpp"

namespace wsf
{
namespace coverage
{

class Grid;
class Measure;
class TextOutputWriter;

class OverlayOutput
{
public:
   OverlayOutput(const std::string& aFileName, const std::string& aOutputDir)
      : mFileName{aFileName}
      , mOutputDir{aOutputDir}
   {
   }

   ~OverlayOutput() = default;

   bool Write(const std::string& aCoverageName, const std::vector<const Measure*>& aMeasures, const Grid* aGridPtr);

private:
   void WriteHeader(TextOutputWriter&                  aWriter,
                    const std::string&                 aCoverageName,
                    const std::vector<const Measure*>& aMeasures,
                    const Grid*                        aGridPtr);
   void WriteBlock(TextOutputWriter&                  aWriter,
                   const std::vector<const Measure*>& aMeasures,
                   const Grid*                        aGridPtr,
                   std::size_t                        aBlock);

   std::string mFileName;
   std::string mOutputDir;
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGEOVERLAYOUTPUT_HPP
