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

#ifndef WSFFLIGHTPATHANGLEMANEUVER_HPP
#define WSFFLIGHTPATHANGLEMANEUVER_HPP

#include "wsf_p6dof_export.h"

#include "WsfManeuver.hpp"

class WSF_P6DOF_EXPORT WsfFlightPathAngleManeuver : public WsfManeuver
{
public:
   WsfFlightPathAngleManeuver();
   WsfFlightPathAngleManeuver(const WsfFlightPathAngleManeuver& aOther);
   ~WsfFlightPathAngleManeuver() override = default;

   WsfFlightPathAngleManeuver& operator=(const WsfFlightPathAngleManeuver& aOther) = delete;

   WsfFlightPathAngleManeuver* Clone() const override { return new WsfFlightPathAngleManeuver{*this}; }

   const char* GetScriptClassName() const override { return "WsfFlightPathAngleManeuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "FLIGHT-PATH-ANGLE"; }

   //! Get the flight path angle in radians.
   double GetFlightPathAngle() const { return mAngle; }

   //! Set the flight path angle in radians.
   void SetFlightPathAngle(double aFlightPathAngle) { mAngle = aFlightPathAngle; }

private:
   double mAngle;
};

#endif // WSFFLIGHTPATHANGLEMANEUVER_HPP
