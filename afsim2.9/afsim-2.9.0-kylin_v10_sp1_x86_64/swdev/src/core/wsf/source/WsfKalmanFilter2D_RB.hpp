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

#ifndef WSFKALMANFILTER2DRB_HPP
#define WSFKALMANFILTER2DRB_HPP

#include "wsf_export.h"

#include "KalmanFilter2D_RB.hpp"
#include "WsfFilter.hpp"

//! Implement an alpha-beta filter as a WsfFilter
class WSF_EXPORT WsfKalmanFilter2D_RB : public WsfFilter
{
public:
   WsfKalmanFilter2D_RB()
      : WsfFilter(new TrackingFilters::KalmanFilter2D_RB())
   {
   }

   WsfKalmanFilter2D_RB(const WsfKalmanFilter2D_RB& aSrc)
      : WsfFilter(aSrc)
   {
   }

   ~WsfKalmanFilter2D_RB() override {}

   WsfFilter* Clone() const override { return new WsfKalmanFilter2D_RB(*this); }
};

#endif
