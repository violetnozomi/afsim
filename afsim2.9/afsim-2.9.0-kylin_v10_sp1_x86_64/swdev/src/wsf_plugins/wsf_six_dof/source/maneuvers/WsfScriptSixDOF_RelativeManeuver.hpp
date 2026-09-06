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

#ifndef WSF_SIXDOF_SCRIPTRELATIVEMANEUVER_HPP
#define WSF_SIXDOF_SCRIPTRELATIVEMANEUVER_HPP

#include "WsfScriptSixDOF_Maneuver.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
class RelativeManeuver : public Maneuver
{
public:
   RelativeManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~RelativeManeuver() override = default;

   UT_DECLARE_SCRIPT_METHOD(GetG_LoadMax);
   UT_DECLARE_SCRIPT_METHOD(SetG_LoadMax);
   UT_DECLARE_SCRIPT_METHOD(GetG_LoadMin);
   UT_DECLARE_SCRIPT_METHOD(SetG_LoadMin);
   UT_DECLARE_SCRIPT_METHOD(GetSpeedKTAS_Max);
   UT_DECLARE_SCRIPT_METHOD(SetSpeedKTAS_Max);
   UT_DECLARE_SCRIPT_METHOD(GetSpeedKTAS_Min);
   UT_DECLARE_SCRIPT_METHOD(SetSpeedKTAS_Min);
   UT_DECLARE_SCRIPT_METHOD(GetSpeedMpsMax);
   UT_DECLARE_SCRIPT_METHOD(SetSpeedMpsMax);
   UT_DECLARE_SCRIPT_METHOD(GetSpeedMpsMin);
   UT_DECLARE_SCRIPT_METHOD(SetSpeedMpsMin);
};
} // namespace script
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_SCRIPTRELATIVEMANEUVER_HPP
