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

#ifndef WSFROLLANGLEMANEUVER_HPP
#define WSFROLLANGLEMANEUVER_HPP

#include "wsf_p6dof_export.h"

#include "WsfManeuver.hpp"

class WSF_P6DOF_EXPORT WsfRollAngleManeuver : public WsfManeuver
{
public:
   WsfRollAngleManeuver();
   WsfRollAngleManeuver(const WsfRollAngleManeuver& aOther);
   ~WsfRollAngleManeuver() override = default;

   WsfRollAngleManeuver& operator=(const WsfRollAngleManeuver& aOther) = delete;

   WsfRollAngleManeuver* Clone() const override { return new WsfRollAngleManeuver{*this}; }

   const char* GetScriptClassName() const override { return "WsfRollAngleManeuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "ROLL-ANGLE"; }

   //! Get the roll angle in radians.
   double GetAngle() const { return mAngle; }

   //! Set the roll angle in radians.
   void SetAngle(double aRollAngle) { mAngle = aRollAngle; }

private:
   double mAngle;
};

#endif // WSFROLLANGLEMANEUVER_HPP
