//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#include "WsfTrackExtrapolators.hpp"

#include "WsfTrackExtrapolation.hpp"

void wsf::TrackExtrapolators::Add(std::unique_ptr<wsf::TrackExtrapolationBase> aExtrapolatorPtr)
{
   mExtrapolators.emplace(std::make_pair(aExtrapolatorPtr->GetType(), std::move(aExtrapolatorPtr)));
}

const wsf::TrackExtrapolationBase* wsf::TrackExtrapolators::Find(WsfStringId aExtrapolatorType) const
{
   auto extrapolatorBaseIter = mExtrapolators.find(aExtrapolatorType);
   if (extrapolatorBaseIter != mExtrapolators.end())
   {
      return extrapolatorBaseIter->second;
   }
   return nullptr;
}
