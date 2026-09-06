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

#ifndef WSFPITCHDELTAMANEUVER_HPP
#define WSFPITCHDELTAMANEUVER_HPP

#include "wsf_p6dof_export.h"

#include "WsfManeuver.hpp"

class WSF_P6DOF_EXPORT WsfPitchDeltaManeuver : public WsfManeuver
{
public:
   WsfPitchDeltaManeuver();
   WsfPitchDeltaManeuver(const WsfPitchDeltaManeuver& aOther);
   ~WsfPitchDeltaManeuver() override = default;

   WsfPitchDeltaManeuver& operator=(const WsfPitchDeltaManeuver& aOther) = delete;

   WsfPitchDeltaManeuver* Clone() const override;

   const char* GetScriptClassName() const override { return "WsfPitchDeltaManeuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "PITCH-DELTA"; }

   //! Get the pitch delta in radians.
   double GetDelta() const { return mDelta; }

   //! Set the pitch delta in radians.
   void SetDelta(double aDeltaPitch) { mDelta = aDeltaPitch; }

private:
   double mDelta;
};

#endif // WSFPITCHDELTAMANEUVER_HPP
