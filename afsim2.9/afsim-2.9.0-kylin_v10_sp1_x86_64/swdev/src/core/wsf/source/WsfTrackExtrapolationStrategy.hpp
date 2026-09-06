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

#ifndef WSFTRACKEXTRAPOLATIONSTRATEGY_HPP
#define WSFTRACKEXTRAPOLATIONSTRATEGY_HPP

#include "wsf_export.h"

#include <map>

#include "UtCloneablePtr.hpp"
class UtInput;
#include "WsfObject.hpp"
class WsfMeasurement;
#include "WsfStringId.hpp"

namespace wsf
{
class TrackExtrapolationBase;
class TrackExtrapolators;

//! A base class used to configure a measurement or track with extrapolation objects (wsf::TrackExtrapolators).
class WSF_EXPORT TrackExtrapolationStrategy : public WsfUncloneableObject
{
public:
   //! Process input from a generic source.
   //! @param aInput The UtInput object from which commands are extracted.
   bool ProcessInput(UtInput& aInput) override { return false; }

   //! Initialize a TrackExtrapolators object for use by a measurement/track and customized by its data.
   //! @param aMeasurement The measurement being used to configure which extrapolation objects to place in aExtrapolators.
   //! @param aExtrapolators The TrackExtrapolators object populated by this strategy and used by the measurement/track
   //! to extrapolate specific data.
   virtual void InitializeExtrapolation(const WsfMeasurement& aMeasurement, TrackExtrapolators& aExtrapolators) const = 0;

   //! Register a wsf::TrackExtrapolation type for (typically, from an external plug-in.)
   void RegisterExtrapolation(const WsfStringId                            aExtrapolatorId,
                              std::unique_ptr<wsf::TrackExtrapolationBase> aExtrapolationPtr)
   {
      mPrototypeExtrapolators.emplace(std::make_pair(aExtrapolatorId, std::move(aExtrapolationPtr)));
   }

protected:
   std::map<WsfStringId, ut::CloneablePtr<wsf::TrackExtrapolationBase>> mPrototypeExtrapolators;
};
} // namespace wsf

#endif
