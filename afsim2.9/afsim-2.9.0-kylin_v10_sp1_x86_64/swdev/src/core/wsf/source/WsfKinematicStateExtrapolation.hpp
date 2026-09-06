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

#ifndef WSFKINEMATICSTATEEXTRAPOLATION_HPP
#define WSFKINEMATICSTATEEXTRAPOLATION_HPP

#include "wsf_export.h"

#include "UtInformation.hpp"
#include "UtVec3.hpp"
#include "WsfStringId.hpp"
#include "WsfTrackExtrapolation.hpp"

namespace wsf
{
//! An aggregation of position and velocity used by KinematicStateExtrapolation classes.
class WSF_EXPORT KinematicState
{
public:
   KinematicState() = default;
   KinematicState(const UtVec3d& aLocationWCS, const UtVec3d& aVelocityWCS)
      : mLocationWCS(aLocationWCS)
      , mVelocityWCS(aVelocityWCS)
   {
   }
   KinematicState& operator=(const KinematicState& aRhs) = default;

   void SetLocationWCS(const UtVec3d& aLocationWCS) { mLocationWCS = aLocationWCS; }
   void SetVelocityWCS(const UtVec3d& aVelocityWCS) { mVelocityWCS = aVelocityWCS; }

   UtVec3d GetLocationWCS() const { return mLocationWCS; }
   UtVec3d GetVelocityWCS() const { return mVelocityWCS; }

private:
   UtVec3d mLocationWCS;
   UtVec3d mVelocityWCS;
};

using KinematicStateInformation = UtInformation<KinematicState>;

//! A base class for kinematic state (position, velocity) extrapolation.
class WSF_EXPORT KinematicStateExtrapolation : public TrackExtrapolation<KinematicState>
{
public:
   KinematicStateExtrapolation();

   static WsfStringId GetTypeId();
};
} // namespace wsf

#endif
