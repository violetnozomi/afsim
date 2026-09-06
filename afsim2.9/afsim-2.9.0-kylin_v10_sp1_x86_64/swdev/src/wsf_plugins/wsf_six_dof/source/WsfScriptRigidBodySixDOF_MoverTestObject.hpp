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

#ifndef WSFSCRIPTRIGIDBODYSIXDOFMOVERTESTOBJECT_HPP
#define WSFSCRIPTRIGIDBODYSIXDOFMOVERTESTOBJECT_HPP

#include "wsf_six_dof_export.h"

#include "WsfScriptSixDOF_MoverTestObject.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
class WSF_SIX_DOF_EXPORT RigidBodyMoverTestObject : public MoverTestObject
{
public:
   RigidBodyMoverTestObject(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   // Insert new methods here.
   UT_DECLARE_SCRIPT_METHOD(Autotune);          // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(GetPitchingMoment); // NO_DOC | FOR_TEST_ONLY
};
} // namespace script
} // namespace six_dof
} // namespace wsf

#endif
