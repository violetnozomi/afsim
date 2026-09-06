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

#ifndef WSF_SIXDOF_PITCHGLOADMANEUVER_HPP
#define WSF_SIXDOF_PITCHGLOADMANEUVER_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_Maneuver.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT PitchGLoadManeuver : public Maneuver
{
public:
   PitchGLoadManeuver();
   PitchGLoadManeuver(const PitchGLoadManeuver& aOther);
   ~PitchGLoadManeuver() override = default;

   PitchGLoadManeuver& operator=(const PitchGLoadManeuver& aOther) = delete;

   PitchGLoadManeuver* Clone() const override;

   const char* GetScriptClassName() const override { return "WsfSixDOF_PitchGLoadManeuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "PITCH-GLOAD"; }

   //! Get the g-load of the maneuver.
   double GetG_Load() const { return mG_Load; }

   //! Set the g-load of the maneuver.
   void SetG_Load(double aG_Load) { mG_Load = aG_Load; }

private:
   double mG_Load;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_PITCHGLOADMANEUVER_HPP
