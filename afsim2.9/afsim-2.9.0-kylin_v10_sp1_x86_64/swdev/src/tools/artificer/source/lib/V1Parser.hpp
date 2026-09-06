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
#ifndef V1PARSER_HPP
#define V1PARSER_HPP

#include "artificer_export.h"

#include <string>

#include "AfperfParserInterface.hpp"

namespace artificer
{
///@brief A parser for version 1 afperf files, which are mostly .csv.
/// Every record starts with a type and a time stamp.  The type determines the minimum number of fields for the row.
///@note Exported for unit testing.
class ARTIFICER_EXPORT V1Parser : public AfperfParserInterface
{
public:
   V1Parser()          = default;
   virtual ~V1Parser() = default;

   ///@brief Returns true if the given header indicates a V1 afperf file
   bool CanHandle(const std::string& aHeader) override;
   ///@brief Parses the given `aData` and gives each record to `aSummarizer`
   void Parse(std::istream& aData, AfperfSummarizerInterface& aSummarizer) override;
};

} // namespace artificer

#endif
