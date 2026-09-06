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

#ifndef WSF_SIXDOF_FLIGHTPATHANGLEMANEUVER_HPP
#define WSF_SIXDOF_FLIGHTPATHANGLEMANEUVER_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_Maneuver.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT FlightPathAngleManeuver : public Maneuver
{
public:
   FlightPathAngleManeuver();
   FlightPathAngleManeuver(const FlightPathAngleManeuver& aOther);
   ~FlightPathAngleManeuver() override = default;

   FlightPathAngleManeuver& operator=(const FlightPathAngleManeuver& aOther) = delete;

   FlightPathAngleManeuver* Clone() const override { return new FlightPathAngleManeuver{*this}; }

   const char* GetScriptClassName() const override { return "WsfSixDOF_FlightPathAngleManeuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "FLIGHT-PATH-ANGLE"; }

   //! Get the flight path angle in radians.
   double GetFlightPathAngle() const { return mAngle; }

   //! Set the flight path angle in radians.
   void SetFlightPathAngle(double aFlightPathAngle) { mAngle = aFlightPathAngle; }

private:
   double mAngle;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_FLIGHTPATHANGLEMANEUVER_HPP
