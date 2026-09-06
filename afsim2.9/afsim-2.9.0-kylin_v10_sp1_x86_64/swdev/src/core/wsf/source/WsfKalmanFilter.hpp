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

#ifndef WSFKALMANFILTER_HPP
#define WSFKALMANFILTER_HPP

#include "wsf_export.h"

#include "KalmanFilter.hpp"
#include "WsfFilter.hpp"

//! Implement an Kalman filter as a WsfFilter
class WSF_EXPORT WsfKalmanFilter : public WsfFilter
{
public:
   WsfKalmanFilter()
      : WsfFilter(new TrackingFilters::KalmanFilter())
   {
   }
   WsfKalmanFilter(const WsfKalmanFilter& aSrc) = default;
   ~WsfKalmanFilter() override                  = default;

   WsfFilter* Clone() const override { return new WsfKalmanFilter(*this); }

   void SetFilterProcessNoise(const double aProcessNoise[3])
   {
      static_cast<TrackingFilters::KalmanFilter*>(mFilterPtr)->SetFilterProcessNoise(aProcessNoise);
   }
};

#endif
