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

#ifndef WSFROLLDELTAMANEUVER_HPP
#define WSFROLLDELTAMANEUVER_HPP

#include "wsf_p6dof_export.h"

#include "WsfManeuver.hpp"

class WSF_P6DOF_EXPORT WsfRollDeltaManeuver : public WsfManeuver
{
public:
   WsfRollDeltaManeuver();
   WsfRollDeltaManeuver(const WsfRollDeltaManeuver& aOther);
   ~WsfRollDeltaManeuver() override = default;

   WsfRollDeltaManeuver& operator=(const WsfRollDeltaManeuver& aOther) = delete;

   WsfRollDeltaManeuver* Clone() const override;

   const char* GetScriptClassName() const override { return "WsfRollDeltaManeuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "ROLL-DELTA"; }

   //! Get the roll delta in radians.
   double GetDelta() const { return mDelta; }

   //! Set the roll delta in radians.
   void SetDelta(double aDeltaRoll) { mDelta = aDeltaRoll; }

private:
   double mDelta;
};

#endif // WSFROLLDELTAMANEUVER_HPP
