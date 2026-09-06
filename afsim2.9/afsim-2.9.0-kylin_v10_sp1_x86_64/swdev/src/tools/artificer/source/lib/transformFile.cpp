// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2022 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "transformFile.hpp"

#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "V1Parser.hpp"
#include "V1PrototypeSummarizer.hpp"
#include "parseAfperf.hpp"

namespace artificer
{
void TransformFile(const std::string& aOutputType, const UtPath& aFilename)
{
   std::map<std::string, std::unique_ptr<AfperfSummarizerInterface>> outputTypes;
   outputTypes.emplace(std::string("v1prototype"), std::unique_ptr<AfperfSummarizerInterface>(new V1PrototypeSummarizer));

   std::vector<std::unique_ptr<AfperfParserInterface>> parsers;
   parsers.emplace_back(static_cast<AfperfParserInterface*>(new V1Parser));

   const auto foundOutput = outputTypes.find(aOutputType);

   if (foundOutput == outputTypes.end())
   {
      std::cerr << "Output type \"" << aOutputType << "\" is not known!" << std::endl;
   }
   else
   {
      std::ifstream afperf;
      afperf.open(aFilename.GetNormalizedPath());
      if (!afperf.good())
      {
         throw std::runtime_error("Unable to read " + aFilename.GetNormalizedPath());
      }
      const std::string header = ReadAfperfHeader(afperf);
      for (const std::unique_ptr<AfperfParserInterface>& parser : parsers)
      {
         if (parser->CanHandle(header))
         {
            parser->Parse(afperf, *foundOutput->second);
            return;
         }
      }
      throw std::runtime_error("Unable to parse file " + aFilename.GetNormalizedPath());
   }
}
} // namespace artificer
