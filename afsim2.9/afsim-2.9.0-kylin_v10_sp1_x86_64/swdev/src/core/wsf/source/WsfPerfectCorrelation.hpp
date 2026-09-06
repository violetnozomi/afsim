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

#ifndef WSFPERFECTCORRELATION_HPP
#define WSFPERFECTCORRELATION_HPP

#include "wsf_export.h"

#include "WsfCorrelationStrategy.hpp"

//! An implementation of WsfCorrelationStrategy for perfect correlation
//! (i.e., each track report is correctly correlated with the corresponding target platform).
class WSF_EXPORT WsfPerfectCorrelation : public WsfCorrelationStrategy
{
public:
   WsfPerfectCorrelation();
   ~WsfPerfectCorrelation() override;

   WsfCorrelationStrategy* Clone() const override { return new WsfPerfectCorrelation(*this); }

   WsfLocalTrack* CorrelateImpl(double aSimTime, const WsfTrack& aNonLocalTrack, WsfLocalTrackList& aTrackList) override;
};

#endif
