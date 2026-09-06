// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFPARSETYPEPATH_HPP
#define WSFPARSETYPEPATH_HPP

#include "wsf_parser_export.h"

#include <vector>

#include "UtStringRef.hpp"

using WsfParseTypePath = std::vector<UtStringRef>;

struct WsfParseTypePathCompare
{
   bool operator()(const WsfParseTypePath& a, const WsfParseTypePath& b) const
   {
      return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
   }
};

WsfParseTypePath WSF_PARSER_EXPORT WsfParseTypePathParse(const std::string& aDotSepPath);
std::string WSF_PARSER_EXPORT      WsfParseTypePathString(const WsfParseTypePath& aPaths);

#endif
