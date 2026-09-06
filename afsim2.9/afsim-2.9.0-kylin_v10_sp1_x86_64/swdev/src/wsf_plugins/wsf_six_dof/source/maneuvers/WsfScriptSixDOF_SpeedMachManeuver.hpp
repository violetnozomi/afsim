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

#ifndef WSF_SIXDOF_SCRIPTSPEEDMACHMANEUVER_HPP
#define WSF_SIXDOF_SCRIPTSPEEDMACHMANEUVER_HPP

#include "wsf_six_dof_export.h"

#include "WsfScriptSixDOF_Maneuver.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
class WSF_SIX_DOF_EXPORT SpeedMachManeuver : public Maneuver
{
public:
   SpeedMachManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~SpeedMachManeuver() override = default;

   UT_DECLARE_SCRIPT_METHOD(Construct);
   UT_DECLARE_SCRIPT_METHOD(GetMach);
};
} // namespace script
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_SCRIPTSPEEDMACHMANEUVER_HPP
