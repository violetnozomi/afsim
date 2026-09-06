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

#ifndef WSF_SIXDOF_ROLLDELTAMANEUVER_HPP
#define WSF_SIXDOF_ROLLDELTAMANEUVER_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_Maneuver.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT RollDeltaManeuver : public Maneuver
{
public:
   RollDeltaManeuver();
   RollDeltaManeuver(const RollDeltaManeuver& aOther);
   ~RollDeltaManeuver() override = default;

   RollDeltaManeuver& operator=(const RollDeltaManeuver& aOther) = delete;

   RollDeltaManeuver* Clone() const override;

   const char* GetScriptClassName() const override { return "WsfSixDOF_Maneuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "ROLL-DELTA"; }

   //! Get the roll delta in radians.
   double GetDelta() const { return mDelta; }

   //! Set the roll delta in radians.
   void SetDelta(double aDeltaRoll) { mDelta = aDeltaRoll; }

private:
   double mDelta;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_ROLLDELTAMANEUVER_HPP
