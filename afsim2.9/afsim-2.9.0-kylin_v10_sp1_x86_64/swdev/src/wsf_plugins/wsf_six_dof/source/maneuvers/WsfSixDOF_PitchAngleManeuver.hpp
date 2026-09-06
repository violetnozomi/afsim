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

#ifndef WSF_SIXDOF_PITCHANGLEMANEUVER_HPP
#define WSF_SIXDOF_PITCHANGLEMANEUVER_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_Maneuver.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT PitchAngleManeuver : public Maneuver
{
public:
   PitchAngleManeuver();
   PitchAngleManeuver(const PitchAngleManeuver& aOther);
   ~PitchAngleManeuver() override = default;

   PitchAngleManeuver& operator=(const PitchAngleManeuver& aOther) = delete;

   PitchAngleManeuver* Clone() const override { return new PitchAngleManeuver{*this}; }

   const char* GetScriptClassName() const override { return "WsfSixDOF_PitchAngleManeuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "PITCH-ANGLE"; }

   //! Get the pitch angle in radians.
   double GetAngle() const { return mAngle; }

   //! Set the pitch angle in radians.
   void SetAngle(double aPitchAngle) { mAngle = aPitchAngle; }

private:
   double mAngle;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_PITCHANGLEMANEUVER_HPP
