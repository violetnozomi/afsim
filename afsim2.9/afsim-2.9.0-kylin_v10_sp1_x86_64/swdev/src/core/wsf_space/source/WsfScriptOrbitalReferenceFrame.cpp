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

#include "WsfScriptOrbitalReferenceFrame.hpp"

#include "WsfOrbitalReferenceFrame.hpp"

namespace wsf
{
namespace space
{

ScriptOrbitalReferenceFrame::ScriptOrbitalReferenceFrame(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfOrbitalReferenceFrame");
   mEqualityComparable = true;

   AddStaticMethod(ut::make_unique<INERTIAL>());
   AddStaticMethod(ut::make_unique<RIC>());
   AddStaticMethod(ut::make_unique<NTW>());
}

void ScriptOrbitalReferenceFrame::Destroy(void* aObjectPtr)
{
   delete static_cast<OrbitalReferenceFrame*>(aObjectPtr);
}

bool ScriptOrbitalReferenceFrame::EqualTo(void* aLHS, void* aRHS)
{
   auto lhsPtr = static_cast<OrbitalReferenceFrame*>(aLHS);
   auto rhsPtr = static_cast<OrbitalReferenceFrame*>(aRHS);
   return *lhsPtr == *rhsPtr;
}

UT_DEFINE_SCRIPT_METHOD(ScriptOrbitalReferenceFrame, OrbitalReferenceFrame, INERTIAL, 0, "WsfOrbitalReferenceFrame", "")
{
   auto objectPtr = ut::make_unique<OrbitalReferenceFrame>(OrbitalReferenceFrame::cINERTIAL);
   aReturnVal.SetPointer(new UtScriptRef{objectPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(ScriptOrbitalReferenceFrame, OrbitalReferenceFrame, RIC, 0, "WsfOrbitalReferenceFrame", "")
{
   auto objectPtr = ut::make_unique<OrbitalReferenceFrame>(OrbitalReferenceFrame::cRIC);
   aReturnVal.SetPointer(new UtScriptRef{objectPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(ScriptOrbitalReferenceFrame, OrbitalReferenceFrame, NTW, 0, "WsfOrbitalReferenceFrame", "")
{
   auto objectPtr = ut::make_unique<OrbitalReferenceFrame>(OrbitalReferenceFrame::cNTW);
   aReturnVal.SetPointer(new UtScriptRef{objectPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

} // namespace space
} // namespace wsf
