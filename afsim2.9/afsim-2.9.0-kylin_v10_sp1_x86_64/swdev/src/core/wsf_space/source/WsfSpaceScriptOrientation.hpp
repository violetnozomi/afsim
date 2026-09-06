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

#ifndef WSFSCRIPTORIENTATION_HPP
#define WSFSCRIPTORIENTATION_HPP

#include "wsf_space_export.h"

#include "UtScriptClassDefine.hpp"
#include "script/WsfScriptObjectClass.hpp"

namespace wsf
{

namespace space
{

namespace script
{

//! Define a script class for the WsfSpaceOrientation object.
//! These objects are meant to be passed as arguments to the script method WsfSpaceMover::SetOrientation.
class WSF_SPACE_EXPORT Orientation : public WsfScriptObjectClass
{
public:
   Orientation(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~Orientation() override = default;

   void Destroy(void* aObjectPtr) override;

   //!@name Script methods documented in the AFSIM Reference Guide:
   // Methods that return WsfSpaceOrientation
   UT_DECLARE_SCRIPT_METHOD(NONE);
   UT_DECLARE_SCRIPT_METHOD(NADIR_WITH_ECI_VELOCITY_CONSTRAINT);
   UT_DECLARE_SCRIPT_METHOD(NADIR_WITH_ECEF_VELOCITY_CONSTRAINT);
   UT_DECLARE_SCRIPT_METHOD(SOLAR_WITH_NADIR_CONSTRAINT);
   UT_DECLARE_SCRIPT_METHOD(NADIR_WITH_SOLAR_CONSTRAINT);
   UT_DECLARE_SCRIPT_METHOD(ECI_VELOCITY_WITH_NADIR_CONSTRAINT);
   UT_DECLARE_SCRIPT_METHOD(ECI_VELOCITY_WITH_SOLAR_CONSTRAINT);
   UT_DECLARE_SCRIPT_METHOD(POINT_WITH_ORBIT_PLANE_CONSTRAINT);
   UT_DECLARE_SCRIPT_METHOD(ENTITY_WITH_SOLAR_CONSTRAINT_1);
   UT_DECLARE_SCRIPT_METHOD(ENTITY_WITH_NADIR_CONSTRAINT_1);
   UT_DECLARE_SCRIPT_METHOD(ENTITY_WITH_ORBIT_PLANE_CONSTRAINT_1);
   UT_DECLARE_SCRIPT_METHOD(ENTITY_WITH_SOLAR_CONSTRAINT_2);
   UT_DECLARE_SCRIPT_METHOD(ENTITY_WITH_NADIR_CONSTRAINT_2);
   UT_DECLARE_SCRIPT_METHOD(ENTITY_WITH_ORBIT_PLANE_CONSTRAINT_2);

   UT_DECLARE_SCRIPT_METHOD(SwapAxes);
};

} // namespace script

} // namespace space

} // namespace wsf

#endif
