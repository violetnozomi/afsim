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

#include "WsfScriptPointMassSixDOF_Mover.hpp"

#include "UtMemory.hpp"
#include "WsfPlatform.hpp"
#include "WsfPointMassSixDOF_Mover.hpp"
#include "WsfPointMassSixDOF_Utils.hpp"
#include "script/WsfScriptDefs.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
PointMassMover::PointMassMover(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : Mover(aClassName, aTypesPtr)
{
   SetClassName("WsfPointMassSixDOF_Mover");

   mEqualityComparable = false;

   // Add each of the method objects to the class.

   // Undocumented test functions
   AddMethod(ut::make_unique<GetPointMassTestSupportObject>("__getTestObject"));
}

UT_DEFINE_SCRIPT_METHOD(PointMassMover,
                        wsf::six_dof::PointMassMover,
                        GetPointMassTestSupportObject,
                        0,
                        "WsfPointMassSixDOF_MoverTestObject",
                        "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetPointMassTestSupportObject(), aReturnClassPtr));
}
} // namespace script
} // namespace six_dof
} // namespace wsf
