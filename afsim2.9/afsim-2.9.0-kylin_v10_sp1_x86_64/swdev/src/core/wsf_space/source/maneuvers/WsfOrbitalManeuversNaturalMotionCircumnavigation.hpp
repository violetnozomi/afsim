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

#ifndef WSFORBITALMANEUVERNATURALMOTIONCIRCUMNAVIGATION_HPP
#define WSFORBITALMANEUVERNATURALMOTIONCIRCUMNAVIGATION_HPP

#include "wsf_space_export.h"

#include "WsfOrbitalManeuversRendezvous.hpp"

class WsfOrbitalEventVisitor;

namespace WsfOrbitalManeuvers
{

class WSF_SPACE_EXPORT NaturalMotionCircumnavigation : public Rendezvous
{
public:
   explicit NaturalMotionCircumnavigation(const WsfScenario& aScenario);
   NaturalMotionCircumnavigation(const WsfScenario&                    aScenario,
                                 const wsf::space::TargetPointOptions& aOptions,
                                 OptimizeOption                        aOptimizeOption,
                                 const UtTimeValue&                    aMaxTime,
                                 const UtSpeedValue&                   aMaxDeltaV = 0.0);
   NaturalMotionCircumnavigation(const WsfScenario&                      aScenario,
                                 const wsf::space::TargetPointOptions&   aOptions,
                                 const wsf::space::OrbitalTargetingCost& aOptimizeCost,
                                 const UtTimeValue&                      aMaxTime,
                                 const UtSpeedValue&                     aMaxDeltaV = 0.0);
   NaturalMotionCircumnavigation(const NaturalMotionCircumnavigation& aOther) = default;
   ~NaturalMotionCircumnavigation() override                                  = default;

   NaturalMotionCircumnavigation& operator=(const NaturalMotionCircumnavigation& aOther) = delete;

   static WsfStringId GetTypeId() { return "NATURAL_MOTION_CIRCUMNAVIGATION"; }
   bool               Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext) override;
   bool               ValidateParameterRanges(const std::string& aExecutingPlatformName) const override;
   bool               ProcessInput(UtInput& aInput) override;
   NaturalMotionCircumnavigation* Clone() const override { return new NaturalMotionCircumnavigation{*this}; }
   const char*                    GetScriptClassName() const override { return "WsfNaturalMotionCircumnavigation"; }

   void Accept(WsfOrbitalEventVisitor* aVisitorPtr) override;

   UtLengthValue GetOrbitSize() const { return mOrbitSize; }
   UtAngleValue  GetOrbitPhase() const { return mOrbitPhase; }
   UtLengthValue GetOutOfPlaneAmplitude() const { return mOutOfPlaneAmplitude; }
   UtAngleValue  GetOutOfPlanePhase() const { return mOutOfPlanePhase; }

   void SetOrbitSize(const UtLengthValue& aOrbitSize) { mOrbitSize = aOrbitSize; }
   void SetOrbitPhase(const UtAngleValue& aOrbitPhase) { mOrbitPhase = aOrbitPhase; }
   void SetOutOfPlaneAmplitude(const UtLengthValue& aOutOfPlaneAmplitude)
   {
      mOutOfPlaneAmplitude = aOutOfPlaneAmplitude;
   }
   void SetOutOfPlanePhase(const UtAngleValue& aOutOfPlanePhase) { mOutOfPlanePhase = aOutOfPlanePhase; }

private:
   UtLengthValue mOrbitSize{};
   UtAngleValue  mOrbitPhase{};
   UtLengthValue mOutOfPlaneAmplitude{};
   UtAngleValue  mOutOfPlanePhase{};
};

} // namespace WsfOrbitalManeuvers

#endif // WSFORBITALMANEUVERNATURALMOTIONCIRCUMNAVIGATION_HPP
