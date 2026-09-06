// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFORBITALMANEUVERSDRIFT_HPP
#define WSFORBITALMANEUVERSDRIFT_HPP

#include "wsf_space_export.h"

#include "WsfOrbitalManeuversRendezvous.hpp"

class WsfOrbitalEventVisitor;

namespace WsfOrbitalManeuvers
{

//! A Drift maneuver will transfer a platform from one circular orbit to another with a similar radius.
//! If two platforms were placed on the two circular orbits connected by this maneuver with their
//! position vectors parallel at the initial time, then one platform would drift forward or backward
//! at the configured angular rate. Because the two orbits are non-intersecting, to maneuver from the
//! initial orbit to the final requires a sequence of maneuvers. This maneuver makes use of many of
//! the options for other targeting-class maneuvers: clients may specify that optimization is to be
//! performed in delta-V, or delta-T, or the user can select a fixed time to complete the transfer.
class WSF_SPACE_EXPORT Drift : public Rendezvous
{
public:
   explicit Drift(const WsfScenario& aScenario);
   Drift(const WsfScenario&  aScenario,
         OptimizeOption      aOptimizeOption,
         const UtTimeValue&  aMaxTime,
         const UtSpeedValue& aMaxDeltaV = 0.0);
   Drift(const WsfScenario&                      aScenario,
         const wsf::space::OrbitalTargetingCost& aOptimizeCost,
         const UtTimeValue&                      aMaxTime,
         const UtSpeedValue&                     aMaxDeltaV = 0.0);
   Drift(const Drift& aOther) = default;
   ~Drift() override          = default;

   Drift& operator=(const Drift& aOther) = delete;

   static WsfStringId GetTypeId() { return "DRIFT"; }
   bool               Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext) override;
   bool               VerifyCondition() const override;
   bool               ValidateParameterRanges(const std::string& aExecutingPlatformName) const override;
   bool               ProcessInput(UtInput& aInput) override;
   Drift*             Clone() const override { return new Drift{*this}; }
   const char*        GetScriptClassName() const override { return "WsfDriftManeuver"; }

   void Accept(WsfOrbitalEventVisitor* aVisitorPtr) override;

   //! Get the configured drift rate in radians per second.
   UtAngularRateValue GetDriftRate() const { return mDriftRate; }

   //! Set the drift rate in radians per second.
   void SetDriftRate(const UtAngularRateValue& aDriftRate) { mDriftRate = aDriftRate; }

private:
   UtAngularRateValue mDriftRate{};
};

} // namespace WsfOrbitalManeuvers

#endif // WSFORBITALMANEUVERSDRIFT_HPP
