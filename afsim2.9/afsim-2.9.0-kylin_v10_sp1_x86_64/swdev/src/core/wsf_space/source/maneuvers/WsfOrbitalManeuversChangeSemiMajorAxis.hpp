// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFORBITALMANEUVERSCHANGESEMIMAJORAXIS_HPP
#define WSFORBITALMANEUVERSCHANGESEMIMAJORAXIS_HPP

#include "wsf_space_export.h"

class UtCalendar;
class UtInput;
#include "UtUnitTypes.hpp"
#include "UtVec3.hpp"
class WsfOrbitalEvent;
#include "WsfOrbitalManeuver.hpp"
#include "WsfStringId.hpp"

//! A set of useful orbital maneuvers (see WsfOrbitalEvent).
namespace WsfOrbitalManeuvers
{
//! Change the semi-major axis of the orbit to a given value.
//! @note If the orbit is not initially circular, this maneuver must be performed using
//! WsfOrbitalManeuver::Condition::cAT_PERIAPSIS or WsfOrbitalManeuver::Condition::cAT_APOAPSIS.
class WSF_SPACE_EXPORT ChangeSemiMajorAxis : public WsfOrbitalManeuver
{
public:
   explicit ChangeSemiMajorAxis(const WsfScenario& aScenario);
   ChangeSemiMajorAxis(const WsfScenario& aScenario, const UtLengthValue& aFinalSMA);
   ~ChangeSemiMajorAxis() override = default;

   static WsfStringId GetTypeId();

   bool                 ProcessInput(UtInput& aInput) override;
   ChangeSemiMajorAxis* Clone() const override { return new ChangeSemiMajorAxis(*this); }
   bool                 EvaluatePreconditions(const wsf::space::OrbitalMissionContext& aContext) const override;
   void                 ComputeDeltaV(const UtCalendar&              aCurrentTime,
                                      const UtCalendar&              aEvaluationTime,
                                      const UtOrbitalPropagatorBase& aPropagator,
                                      UtVec3d&                       aDeltaV) const override;
   const char*          GetScriptClassName() const override { return "WsfChangeSemiMajorAxis"; }

   void Accept(WsfOrbitalEventVisitor* aVisitorPtr) override;

   enum class InputOption
   {
      cFINAL_SMA,
      cRADIUS,
      cAPOAPSIS_ALT,
      cPERIAPSIS_ALT
   };

   UtLengthValue GetFinalSMA() const { return mFinalSMA; }
   UtLengthValue GetRadius() const { return mFinalSMA; }
   UtLengthValue GetApoapsisAltitude() const { return mApoapsisAltitude; }
   UtLengthValue GetPeriapsisAltitude() const { return mPeriapsisAltitude; }
   InputOption   GetInputType() const { return mInputType; }

   void SetFinalSMA(const UtLengthValue& aSMA);
   void SetRadius(const UtLengthValue& aRadius);
   void SetApoapsisAltitude(const UtLengthValue& aAlt);
   void SetPeriapsisAltitude(const UtLengthValue& aAlt);

private:
   bool InitializeVariables(const UtCalendar&                        aCurrentTime,
                            const UtCalendar&                        aEvaluationTime,
                            const wsf::space::OrbitalMissionContext& aContext) override;
   bool EvaluateCompletion(const UtCalendar& aEpoch, const UtOrbitalPropagatorBase& aPropagator) override;

   UtLengthValue mFinalSMA;
   UtLengthValue mApoapsisAltitude;
   UtLengthValue mPeriapsisAltitude;
   InputOption   mInputType;
};

} // namespace WsfOrbitalManeuvers

#endif
