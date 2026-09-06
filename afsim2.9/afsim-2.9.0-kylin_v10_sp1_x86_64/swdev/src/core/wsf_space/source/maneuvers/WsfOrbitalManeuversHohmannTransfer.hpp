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

#ifndef WSFORBITALMANEUVERSHOHMANNTRANSFER_HPP
#define WSFORBITALMANEUVERSHOHMANNTRANSFER_HPP

#include "wsf_space_export.h"

class UtCalendar;
#include "UtUnitTypes.hpp"
#include "WsfOrbitalMissionSequence.hpp"
#include "WsfStringId.hpp"

//! A set of useful orbital maneuvers (see WsfOrbitalEvent).
namespace WsfOrbitalManeuvers
{

//! A simple Hohmann transfer to change the orbit to a circular one
//! with different semi-major axis.  This maneuver consists of two
//! separate semi-major axis changes.
class WSF_SPACE_EXPORT HohmannTransfer : public WsfOrbitalMissionSequence
{
public:
   explicit HohmannTransfer(const WsfScenario& aScenario);
   HohmannTransfer(const WsfScenario& aScenario, const UtLengthValue& aFinalRadius);
   HohmannTransfer(const HohmannTransfer&) = default;
   ~HohmannTransfer() override             = default;

   static WsfStringId GetTypeId();
   bool               Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext) override;
   bool               EvaluatePreconditions(const wsf::space::OrbitalMissionContext& aContext) const override;
   bool               ValidateParameterRanges(const std::string& aExecutingPlatformName) const override;
   bool               ProcessInput(UtInput& aInput) override;
   HohmannTransfer*   Clone() const override { return new HohmannTransfer(*this); }
   const char*        GetScriptClassName() const override { return "WsfHohmannTransfer"; }

   void Accept(WsfOrbitalEventVisitor* aVisitorPtr) override;

   UtLengthValue GetFinalSMA() const { return mFinalSMA; }
   UtLengthValue GetFinalRadius() const { return mFinalSMA; }
   bool          IsRadiusInput() const { return mInputAsRadius; }

   void SetFinalSMA(const UtLengthValue& aSMA);
   void SetFinalRadius(const UtLengthValue& aRadius);

private:
   UtLengthValue mFinalSMA;
   bool          mInputAsRadius;
};

} // namespace WsfOrbitalManeuvers

#endif
