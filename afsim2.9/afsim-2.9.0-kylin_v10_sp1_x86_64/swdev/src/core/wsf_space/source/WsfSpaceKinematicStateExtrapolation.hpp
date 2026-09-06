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

#ifndef WSFSPACEKINEMATICSTATEMEASUREMENTEXTRAPOLATION_HPP
#define WSFSPACEKINEMATICSTATEMEASUREMENTEXTRAPOLATION_HPP

#include "wsf_space_export.h"

#include "UtCloneablePtr.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "WsfKinematicStateExtrapolation.hpp"
class WsfMeasurement;

namespace wsf
{

namespace space
{
//! A kinematic state extrapolation strategy that uses orbital propagators and the orbit determination filter
//! to provide correct extrapolation of orbiting space platforms.
class WSF_SPACE_EXPORT KinematicStateExtrapolation : public wsf::KinematicStateExtrapolation
{
public:
   KinematicStateExtrapolation()           = default;
   ~KinematicStateExtrapolation() override = default;

   static WsfStringId GetTypeId();

   wsf::KinematicStateExtrapolation* Clone() const override { return new KinematicStateExtrapolation(*this); }

   std::unique_ptr<UtAttribute<KinematicState>> Extrapolate(double                aSimTime,
                                                            const WsfMeasurement& aMeasurement) const override;

private:
   bool UpdatePropagator(const UtCalendar& aMeasurementEpoch, const WsfMeasurement& aMeasurement) const;

   mutable ut::CloneablePtr<UtOrbitalPropagatorBase> mOrbitalPropagatorPtr{nullptr};
};

} // namespace space
} // namespace wsf

#endif
