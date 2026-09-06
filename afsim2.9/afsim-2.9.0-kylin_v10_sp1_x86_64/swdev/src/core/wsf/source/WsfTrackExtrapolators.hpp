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

#ifndef WSFTRACKEXTRAPOLATORS_HPP
#define WSFTRACKEXTRAPOLATORS_HPP

#include "wsf_export.h"

#include <map>

#include "UtCloneablePtr.hpp"
#include "WsfStringId.hpp"

namespace wsf
{
class TrackExtrapolationBase;

//! An encapsulation of a map of wsf::TrackExtrapolation objects.
class WSF_EXPORT TrackExtrapolators
{
public:
   TrackExtrapolators()  = default;
   ~TrackExtrapolators() = default;
   void                               Add(std::unique_ptr<wsf::TrackExtrapolationBase> aExtrapolatorPtr);
   const wsf::TrackExtrapolationBase* Find(WsfStringId aExtrapolatorType) const;
   TrackExtrapolators*                Clone() const { return new TrackExtrapolators(*this); }

private:
   std::map<WsfStringId, ut::CloneablePtr<wsf::TrackExtrapolationBase>> mExtrapolators;
};
} // namespace wsf

#endif
