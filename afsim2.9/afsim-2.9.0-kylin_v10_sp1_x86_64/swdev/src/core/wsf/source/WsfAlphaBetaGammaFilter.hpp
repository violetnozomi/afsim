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

#ifndef WSFALPHABETAGAMMAFILTER_HPP
#define WSFALPHABETAGAMMAFILTER_HPP

#include "wsf_export.h"

#include "AlphaBetaGammaFilter.hpp"
#include "WsfFilter.hpp"

//! Implement an alpha-beta filter as a WsfFilter
class WSF_EXPORT WsfAlphaBetaGammaFilter : public WsfFilter
{
public:
   WsfAlphaBetaGammaFilter()
      : WsfFilter(new TrackingFilters::AlphaBetaGammaFilter())
   {
   }

   WsfAlphaBetaGammaFilter(const WsfAlphaBetaGammaFilter& aSrc)
      : WsfFilter(aSrc)
   {
   }

   ~WsfAlphaBetaGammaFilter() override {}

   WsfFilter* Clone() const override { return new WsfAlphaBetaGammaFilter(*this); }
};

#endif
