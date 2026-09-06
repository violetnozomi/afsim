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

#ifndef WSF_SIXDOF_ROLLANGLEMANEUVER_HPP
#define WSF_SIXDOF_ROLLANGLEMANEUVER_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_Maneuver.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT RollAngleManeuver : public Maneuver
{
public:
   RollAngleManeuver();
   RollAngleManeuver(const RollAngleManeuver& aOther);
   ~RollAngleManeuver() override = default;

   RollAngleManeuver& operator=(const RollAngleManeuver& aOther) = delete;

   RollAngleManeuver* Clone() const override { return new RollAngleManeuver{*this}; }

   const char* GetScriptClassName() const override { return "WsfSixDOF_RollAngleManeuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "ROLL-ANGLE"; }

   //! Get the roll angle in radians.
   double GetAngle() const { return mAngle; }

   //! Set the roll angle in radians.
   void SetAngle(double aRollAngle) { mAngle = aRollAngle; }

private:
   double mAngle;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_ROLLANGLEMANEUVER_HPP
