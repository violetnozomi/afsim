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

#ifndef WSFBATCHTRACKREPORTING_HPP
#define WSFBATCHTRACKREPORTING_HPP

#include "wsf_export.h"

#include "WsfTrackReportingStrategy.hpp"

//! An instance of WsfTrackReportingStrategy that sends tracks all at once in a batch.
class WSF_EXPORT WsfBatchTrackReporting : public WsfTrackReportingStrategy
{
public:
   ~WsfBatchTrackReporting() override = default;

   WsfTrackReportingStrategy* Clone() const override;

   void ReportFusedTracks(double aSimTime) override;
   void ReportRawTracks(double aSimTime) override;
};

#endif
