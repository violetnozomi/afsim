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

#ifndef WSFALPHABETAFILTER_HPP
#define WSFALPHABETAFILTER_HPP

#include "wsf_export.h"

#include "AlphaBetaFilter.hpp"
#include "WsfFilter.hpp"

//! Implement an alpha-beta filter as a WsfFilter
class WSF_EXPORT WsfAlphaBetaFilter : public WsfFilter
{
public:
   WsfAlphaBetaFilter()
      : WsfFilter(new TrackingFilters::AlphaBetaFilter())
   {
   }

   WsfAlphaBetaFilter(const WsfAlphaBetaFilter& aSrc)
      : WsfFilter(aSrc)
   {
   }

   ~WsfAlphaBetaFilter() override {}

   WsfFilter* Clone() const override { return new WsfAlphaBetaFilter(*this); }
};

#endif
