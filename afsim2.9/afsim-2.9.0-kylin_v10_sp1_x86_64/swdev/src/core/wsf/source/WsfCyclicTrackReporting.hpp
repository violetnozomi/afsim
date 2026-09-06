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

#ifndef WSFCYCLICTRACKREPORTING_HPP
#define WSFCYCLICTRACKREPORTING_HPP

#include "wsf_export.h"

#include "WsfTrackReportingStrategy.hpp"

//! An instance of WsfTrackReportingStrategy that reports all tracks in the
//! track lists once per reporting interval.  If the number of tracks in
//! a list changes during the reporting interval, or, if the reporting interval
//! changes, the time interval between  tracks will be adjusted.
//! @note Currently the implementation allows for duplicate tracks sent during a
//! reporting interval, if a track is removed from a list.
class WSF_EXPORT WsfCyclicTrackReporting : public WsfTrackReportingStrategy
{
public:
   WsfTrackReportingStrategy* Clone() const override;

   void ReportFusedTracks(double aSimTime) override;
   void ReportRawTracks(double aSimTime) override;
};

#endif
