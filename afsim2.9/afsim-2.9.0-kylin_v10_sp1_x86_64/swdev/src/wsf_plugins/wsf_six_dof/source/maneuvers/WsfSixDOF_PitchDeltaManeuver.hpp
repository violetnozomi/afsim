// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSF_SIXDOF_PITCHDELTAMANEUVER_HPP
#define WSF_SIXDOF_PITCHDELTAMANEUVER_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_Maneuver.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT PitchDeltaManeuver : public Maneuver
{
public:
   PitchDeltaManeuver();
   PitchDeltaManeuver(const PitchDeltaManeuver& aOther);
   ~PitchDeltaManeuver() override = default;

   PitchDeltaManeuver& operator=(const PitchDeltaManeuver& aOther) = delete;

   PitchDeltaManeuver* Clone() const override;

   const char* GetScriptClassName() const override { return "WsfSixDOF_PitchDeltaManeuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "PITCH-DELTA"; }

   //! Get the pitch delta in radians.
   double GetDelta() const { return mDelta; }

   //! Set the pitch delta in radians.
   void SetDelta(double aDeltaPitch) { mDelta = aDeltaPitch; }

private:
   double mDelta;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_PITCHDELTAMANEUVER_HPP
