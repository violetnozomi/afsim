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

#ifndef WSFPITCHANGLEMANEUVER_HPP
#define WSFPITCHANGLEMANEUVER_HPP

#include "wsf_p6dof_export.h"

#include "WsfManeuver.hpp"

class WSF_P6DOF_EXPORT WsfPitchAngleManeuver : public WsfManeuver
{
public:
   WsfPitchAngleManeuver();
   WsfPitchAngleManeuver(const WsfPitchAngleManeuver& aOther);
   ~WsfPitchAngleManeuver() override = default;

   WsfPitchAngleManeuver& operator=(const WsfPitchAngleManeuver& aOther) = delete;

   WsfPitchAngleManeuver* Clone() const override { return new WsfPitchAngleManeuver{*this}; }

   const char* GetScriptClassName() const override { return "WsfPitchAngleManeuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "PITCH-ANGLE"; }

   //! Get the pitch angle in radians.
   double GetAngle() const { return mAngle; }

   //! Set the pitch angle in radians.
   void SetAngle(double aPitchAngle) { mAngle = aPitchAngle; }

private:
   double mAngle;
};

#endif // WSFPITCHANGLEMANEUVER_HPP
