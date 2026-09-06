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

#ifndef WSFTURNTOHEADINGMANEUVER_HPP
#define WSFTURNTOHEADINGMANEUVER_HPP

#include "wsf_p6dof_export.h"

#include "WsfManeuver.hpp"
#include "WsfPath.hpp"

class WSF_P6DOF_EXPORT WsfTurnToHeadingManeuver : public WsfManeuver
{
public:
   explicit WsfTurnToHeadingManeuver(double aHeading);
   WsfTurnToHeadingManeuver(const WsfTurnToHeadingManeuver& aOther) = default;
   ~WsfTurnToHeadingManeuver() override                             = default;

   WsfTurnToHeadingManeuver& operator=(const WsfTurnToHeadingManeuver& aOther) = delete;

   WsfTurnToHeadingManeuver* Clone() const override { return new WsfTurnToHeadingManeuver{*this}; }

   const char* GetScriptClassName() const override { return "WsfTurnToHeadingManeuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "TURN-TO-HEADING"; }

   //! Get heading in radians.
   double GetHeading() const { return mHeading; }

   void SetHeading(double aHeading);

private:
   void UpdateExitConstraint();

   double mHeading;
};

#endif // WSFTURNTOHEADINGMANEUVER_HPP
