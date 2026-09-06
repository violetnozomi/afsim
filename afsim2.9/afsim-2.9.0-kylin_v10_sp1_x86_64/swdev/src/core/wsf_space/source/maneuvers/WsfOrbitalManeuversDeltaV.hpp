// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFDELTAVMANEUVER_HPP
#define WSFDELTAVMANEUVER_HPP

#include "wsf_space_export.h"

class UtCalendar;
class UtInput;
#include "UtUnitTypes.hpp"
#include "UtVec3.hpp"
#include "WsfOrbitalManeuver.hpp"
#include "WsfOrbitalReferenceFrame.hpp"
#include "WsfStringId.hpp"

//! A set of useful orbital maneuvers (see WsfOrbitalEvent).
namespace WsfOrbitalManeuvers
{
//! Perform a maneuver that applies a change in velocity using a vector specified
//! in the ECI coordinate system.
class WSF_SPACE_EXPORT DeltaV : public WsfOrbitalManeuver
{
public:
   explicit DeltaV(const WsfScenario& aScenario)
      : WsfOrbitalManeuver(aScenario)
   {
   }

   DeltaV(const WsfScenario&                aScenario,
          const double                      dv_x,
          const double                      dv_y,
          const double                      dv_z,
          wsf::space::OrbitalReferenceFrame aFrame);
   DeltaV(const WsfScenario& aScenario, const UtVec3d& aDeltaV, wsf::space::OrbitalReferenceFrame aFrame);
   ~DeltaV() override = default;

   static WsfStringId GetTypeId();
   bool               ProcessInput(UtInput& aInput) override;
   DeltaV*            Clone() const override { return new DeltaV(*this); }
   bool               EvaluatePreconditions(const wsf::space::OrbitalMissionContext& aContext) const override;
   bool               EvaluatePostconditions(const wsf::space::OrbitalMissionContext& aContext) const override;
   void               ComputeDeltaV(const UtCalendar&              aCurrentTime,
                                    const UtCalendar&              aEvaluationTime,
                                    const UtOrbitalPropagatorBase& aPropagator,
                                    UtVec3d&                       aDeltaV) const override;
   const char*        GetScriptClassName() const override { return "WsfDeltaV_Maneuver"; }


   UtSpeedValue   GetDeltaV_Component(int aIndex) const;
   void           SetDeltaV_Component(int aIndex, const UtSpeedValue& aDeltaV);
   const UtVec3d& GetConfiguredDeltaV() const { return mConfiguredDeltaV; }

   wsf::space::OrbitalReferenceFrame GetDeltaV_Frame() const { return mFrame; }
   void                              SetDeltaV_Frame(wsf::space::OrbitalReferenceFrame aFrame) { mFrame = aFrame; }

   void Accept(WsfOrbitalEventVisitor* aVisitorPtr) override;

   static bool IsFrameSupported(wsf::space::OrbitalReferenceFrame aFrame);

private:
   UtVec3d                           mConfiguredDeltaV{};
   wsf::space::OrbitalReferenceFrame mFrame{wsf::space::OrbitalReferenceFrame::cUNKNOWN};
};

} // namespace WsfOrbitalManeuvers

#endif
