// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSF_SIXDOF_PURSUEMANEUVER_HPP
#define WSF_SIXDOF_PURSUEMANEUVER_HPP

#include "wsf_six_dof_export.h"

#include <string>

#include "WsfSixDOF_RelativeManeuver.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT PursueManeuver : public RelativeManeuver
{
public:
   PursueManeuver();
   PursueManeuver(const PursueManeuver& aOther);
   PursueManeuver(PursueManeuver&& aOther) = delete;

   ~PursueManeuver() override = default;

   PursueManeuver& operator=(const PursueManeuver& aOther) = delete;
   PursueManeuver& operator=(PursueManeuver&& aOther) = delete;

   PursueManeuver* Clone() const override { return new PursueManeuver{*this}; }

   const char* GetScriptClassName() const override { return "WsfSixDOF_PursueManeuver"; }

   bool        UpdateData() override;
   const char* Type() const override { return "PURSUE"; }

   const std::string& GetTargetPlatformName() const { return mTargetPlatformName; }
   void SetTargetPlatformName(const std::string& aTargetPlatformName) { mTargetPlatformName = aTargetPlatformName; }

   double GetPursuitDistanceMeters() const { return mPursuitDistance_m; }
   void   SetPursuitDistanceMeters(double aDistance_m) { mPursuitDistance_m = aDistance_m; }

private:
   std::string mTargetPlatformName{};
   double      mPursuitDistance_m{0.0}; // Distance ahead of the target
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_PURSUEMANEUVER_HPP
