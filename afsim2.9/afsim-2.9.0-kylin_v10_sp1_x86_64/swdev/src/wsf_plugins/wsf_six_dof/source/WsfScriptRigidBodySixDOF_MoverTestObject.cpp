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

#include "WsfScriptRigidBodySixDOF_MoverTestObject.hpp"

#include "UtMemory.hpp"
#include "WsfRigidBodySixDOF_MoverTestObject.hpp"
#include "WsfRigidBodySixDOF_Utils.hpp"
#include "script/WsfScriptDefs.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
RigidBodyMoverTestObject::RigidBodyMoverTestObject(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : MoverTestObject(aClassName, aTypesPtr)
{
   SetClassName("WsfRigidBodySixDOF_MoverTestObject");

   // Add each of the method objects to the class.

   // Undocumented test functions
   AddMethod(ut::make_unique<Autotune>());
   AddMethod(ut::make_unique<GetPitchingMoment>());
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMoverTestObject, wsf::six_dof::RigidBodyMoverTestObject, Autotune, 0, "void", "") // NO_DOC
                                                                                                                   // | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);
   aObjectPtr->Autotune();
}

UT_DEFINE_SCRIPT_METHOD(RigidBodyMoverTestObject,
                        wsf::six_dof::RigidBodyMoverTestObject,
                        GetPitchingMoment,
                        4,
                        "double",
                        "double,double,double,double") // NO_DOC | FOR_TEST_ONLY
{
   double time = TIME_NOW;
   aObjectPtr->Update(time);

   double moment_ftlb = aObjectPtr->GetPitchingMoment(UtMath::cFT_PER_M * aVarArgs[0].GetDouble(),
                                                      aVarArgs[1].GetDouble(),
                                                      aVarArgs[2].GetDouble(),
                                                      aVarArgs[3].GetDouble());

   aReturnVal.SetDouble(UtMath::cNT_PER_LB * UtMath::cM_PER_FT * moment_ftlb);
}
} // namespace script
} // namespace six_dof
} // namespace wsf
