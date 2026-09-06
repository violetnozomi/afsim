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

#ifndef WSFDEFAULTKINEMATICSTATEEXTRAPOLATION_HPP
#define WSFDEFAULTKINEMATICSTATEEXTRAPOLATION_HPP

#include "wsf_export.h"

#include "WsfKinematicStateExtrapolation.hpp"
class WsfMeasurement;

namespace wsf
{
//! A class providing the default implementation of kinematic state (position, velocity) extrapolation.
//!@note This implementation is not appropriate for kinematic state extrapolation of space platforms (see
//! wsf::space::KinematicStateExtrapolation).
class WSF_EXPORT DefaultKinematicStateExtrapolation : public KinematicStateExtrapolation
{
public:
   KinematicStateExtrapolation* Clone() const override { return new DefaultKinematicStateExtrapolation(*this); }

   std::unique_ptr<UtAttribute<KinematicState>> Extrapolate(double aSimTime, const WsfMeasurement& aData) const override;
};
} // namespace wsf

#endif
