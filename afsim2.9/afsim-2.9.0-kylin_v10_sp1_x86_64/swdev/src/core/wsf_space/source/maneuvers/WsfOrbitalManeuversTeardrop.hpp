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

#ifndef WSFORBITALMANEUVERSTEARDROP_HPP
#define WSFORBITALMANEUVERSTEARDROP_HPP

#include "wsf_space_export.h"

#include "WsfOrbitalManeuversRendezvous.hpp"

class WsfOrbitalEventVisitor;

namespace WsfOrbitalManeuvers
{
class WSF_SPACE_EXPORT Teardrop : public Rendezvous
{
public:
   explicit Teardrop(const WsfScenario& aScenario);
   Teardrop(const WsfScenario&                    aScenario,
            const wsf::space::TargetPointOptions& aOptions,
            OptimizeOption                        aOptimizeOption,
            const UtTimeValue&                    aMaxTime,
            const UtSpeedValue&                   aMaxDeltaV = 0.0);
   Teardrop(const WsfScenario&                      aScenario,
            const wsf::space::TargetPointOptions&   aOptions,
            const wsf::space::OrbitalTargetingCost& aOptimizeCost,
            const UtTimeValue&                      aMaxTime,
            const UtSpeedValue&                     aMaxDeltaV = 0.0);
   Teardrop(const Teardrop& aOther) = default;
   ~Teardrop() override             = default;

   Teardrop& operator=(const Teardrop& aOther) = delete;

   static WsfStringId GetTypeId() { return "TEARDROP"; }
   bool               Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext) override;
   bool               ValidateParameterRanges(const std::string& aExecutingPlatformName) const override;
   bool               ProcessInput(UtInput& aInput) override;
   Teardrop*          Clone() const override { return new Teardrop{*this}; }
   const char*        GetScriptClassName() const override { return "WsfTeardropManeuver"; }

   void Accept(WsfOrbitalEventVisitor* aVisitorPtr) override;

   const UtLengthValue& GetRadialOffsetAtPOCA() const { return mRadialOffsetAtPOCA; }
   const UtTimeValue&   GetTeardropPeriod() const { return mPeriod; }
   const UtTimeValue&   GetTimeToPOCA() const { return mTimeToPOCA; }
   unsigned int         GetRepetitions() const { return mReps; }

   void SetRadialOffsetAtPOCA(const UtLengthValue& aRadialOffsetAtPOCA) { mRadialOffsetAtPOCA = aRadialOffsetAtPOCA; }
   void SetTeardropPeriod(const UtTimeValue& aPeriod) { mPeriod = aPeriod; }
   void SetTimeToPOCA(const UtTimeValue& aTimeToPOCA) { mTimeToPOCA = aTimeToPOCA; }
   void SetRepetitions(unsigned int aReps) { mReps = aReps; }

private:
   UtLengthValue mRadialOffsetAtPOCA{0.0};
   UtTimeValue   mPeriod{0.0};
   UtTimeValue   mTimeToPOCA{0.0};
   unsigned int  mReps{1};
};

} // namespace WsfOrbitalManeuvers

#endif // WSFORBITALMANEUVERSTEARDROP_HPP
