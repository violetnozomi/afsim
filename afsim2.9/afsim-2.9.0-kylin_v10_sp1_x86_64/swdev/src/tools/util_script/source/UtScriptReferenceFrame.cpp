// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtScriptReferenceFrame.hpp"

#include "UtOrbitalState.hpp"

namespace ut
{
namespace script
{

ReferenceFrame::ReferenceFrame(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("ReferenceFrame");

   mEqualityComparable = true;

   AddStaticMethod(ut::make_unique<Inertial>());
   AddStaticMethod(ut::make_unique<J2000>());
   AddStaticMethod(ut::make_unique<TRUE_OF_DATE>());
   AddStaticMethod(ut::make_unique<TEME>());
}

void ReferenceFrame::Destroy(void* aObjectPtr)
{
   delete static_cast<ut::OrbitalState::ReferenceFrame*>(aObjectPtr);
}

bool ReferenceFrame::EqualTo(void* aLHS, void* aRHS)
{
   ut::OrbitalState::ReferenceFrame* lhsPtr = static_cast<ut::OrbitalState::ReferenceFrame*>(aLHS);
   ut::OrbitalState::ReferenceFrame* rhsPtr = static_cast<ut::OrbitalState::ReferenceFrame*>(aRHS);
   return *lhsPtr == *rhsPtr;
}

UT_DEFINE_SCRIPT_METHOD(ReferenceFrame, ut::OrbitalState::ReferenceFrame, Inertial, 0, "ReferenceFrame", "")
{
   auto referenceFramePtr = ut::make_unique<ut::OrbitalState::ReferenceFrame>(ut::OrbitalState::ReferenceFrame::cECI);
   aReturnVal.SetPointer(new UtScriptRef(referenceFramePtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(ReferenceFrame, ut::OrbitalState::ReferenceFrame, J2000, 0, "ReferenceFrame", "")
{
   auto referenceFramePtr = ut::make_unique<ut::OrbitalState::ReferenceFrame>(ut::OrbitalState::ReferenceFrame::cJ2000);
   aReturnVal.SetPointer(new UtScriptRef(referenceFramePtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(ReferenceFrame, ut::OrbitalState::ReferenceFrame, TRUE_OF_DATE, 0, "ReferenceFrame", "")
{
   auto referenceFramePtr =
      ut::make_unique<ut::OrbitalState::ReferenceFrame>(ut::OrbitalState::ReferenceFrame::cTRUE_OF_DATE);
   aReturnVal.SetPointer(new UtScriptRef(referenceFramePtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(ReferenceFrame, ut::OrbitalState::ReferenceFrame, TEME, 0, "ReferenceFrame", "")
{
   auto referenceFramePtr = ut::make_unique<ut::OrbitalState::ReferenceFrame>(ut::OrbitalState::ReferenceFrame::cTEME);
   aReturnVal.SetPointer(new UtScriptRef(referenceFramePtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

} // namespace script
} // namespace ut
