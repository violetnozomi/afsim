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

#include "UtScriptCentralBody.hpp"

#include "UtEarth.hpp"
#include "UtMoon.hpp"
#include "UtSun.hpp"

namespace ut
{
namespace script
{

CentralBody::CentralBody(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("CentralBody");

   mEqualityComparable = true;

   AddStaticMethod(ut::make_unique<EarthWGS72>());
   AddStaticMethod(ut::make_unique<EarthWGS84>());
   AddStaticMethod(ut::make_unique<EarthEGM96>());
   AddStaticMethod(ut::make_unique<Sun>());
   AddStaticMethod(ut::make_unique<Moon>());
}

void CentralBody::Destroy(void* aObjectPtr)
{
   delete static_cast<ut::CentralBody*>(aObjectPtr);
}

bool CentralBody::EqualTo(void* aLHS, void* aRHS)
{
   ut::CentralBody* lhsPtr = static_cast<ut::CentralBody*>(aLHS);
   ut::CentralBody* rhsPtr = static_cast<ut::CentralBody*>(aRHS);
   return lhsPtr->GetName() == rhsPtr->GetName();
}

UT_DEFINE_SCRIPT_METHOD(CentralBody, ut::CentralBody, EarthWGS72, 0, "CentralBody", "")
{
   auto centralBodyPtr = ut::make_unique<ut::EarthWGS72>();
   aReturnVal.SetPointer(new UtScriptRef(centralBodyPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(CentralBody, ut::CentralBody, EarthWGS84, 0, "CentralBody", "")
{
   auto centralBodyPtr = ut::make_unique<ut::EarthWGS84>();
   aReturnVal.SetPointer(new UtScriptRef(centralBodyPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(CentralBody, ut::CentralBody, EarthEGM96, 0, "CentralBody", "")
{
   auto centralBodyPtr = ut::make_unique<ut::EarthEGM96>();
   aReturnVal.SetPointer(new UtScriptRef(centralBodyPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(CentralBody, ut::CentralBody, Sun, 0, "CentralBody", "")
{
   auto centralBodyPtr = ut::make_unique<ut::Sun>();
   aReturnVal.SetPointer(new UtScriptRef(centralBodyPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(CentralBody, ut::CentralBody, Moon, 0, "CentralBody", "")
{
   auto centralBodyPtr = ut::make_unique<ut::Moon>();
   aReturnVal.SetPointer(new UtScriptRef(centralBodyPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

} // namespace script
} // namespace ut
