// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFPPROXYDIFF_HPP
#define WSFPPROXYDIFF_HPP
#include "wsf_parser_export.h"

#include "WsfPProxyPath.hpp"
#include "WsfPProxyValue.hpp"
struct WSF_PARSER_EXPORT WsfPProxyDiff
{
   enum DiffType
   {
      cADDED,
      cCHANGED,
      cREMOVED
   };
   struct DiffEntry
   {
      DiffType      mType;
      WsfPProxyPath mPath;
   };
   void AddDiff(const WsfPProxyPath& aPath, DiffType aType);
   void Diff(WsfPProxyValue aOld, WsfPProxyValue aNew, const WsfPProxyPath& aPath);
   bool IsEqual(WsfPProxyValue aOld, WsfPProxyValue aNew);

   std::vector<DiffEntry> mDiffs;
};

#endif
