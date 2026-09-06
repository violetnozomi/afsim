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

#ifndef WSFPITCHGLOADMANEUVER_HPP
#define WSFPITCHGLOADMANEUVER_HPP

#include "wsf_p6dof_export.h"

#include "WsfManeuver.hpp"

class WSF_P6DOF_EXPORT WsfPitchGLoadManeuver : public WsfManeuver
{
public:
   WsfPitchGLoadManeuver();
   WsfPitchGLoadManeuver(const WsfPitchGLoadManeuver& aOther);
   ~WsfPitchGLoadManeuver() override = default;

   WsfPitchGLoadManeuver& operator=(const WsfPitchGLoadManeuver& aOther) = delete;

   WsfPitchGLoadManeuver* Clone() const override;

   const char* GetScriptClassName() const override { return "WsfPitchGLoadManeuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "PITCH-GLOAD"; }

   //! Get the g-load of the maneuver.
   double GetG_Load() const { return mG_Load; }

   //! Set the g-load of the maneuver.
   void SetG_Load(double aG_Load) { mG_Load = aG_Load; }

private:
   double mG_Load;
};

#endif // WSFPITCHGLOADMANEUVER_HPP
