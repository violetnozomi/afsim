// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptBrawlerMoverClass.hpp"

#include "UtMemory.hpp"
#include "WsfBrawlerMover.hpp"

WsfScriptBrawlerMoverClass::WsfScriptBrawlerMoverClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : WsfScriptMoverClass(aClassName, aTypesPtr)
{
   SetClassName("WsfBrawlerMover");

   AddMethod(ut::make_unique<Slice_1>("Slice"));
   AddMethod(ut::make_unique<Slice_2>("Slice"));
   AddMethod(ut::make_unique<Prlvl>("Prlvl"));
   AddMethod(ut::make_unique<Prlvlg>("Prlvl"));
   AddMethod(ut::make_unique<SetMaxThrottle>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerMoverClass, WsfBrawlerMover, Slice_1, 2, "void", "double, double")
{
   aObjectPtr->Slice(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), 0);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerMoverClass, WsfBrawlerMover, Slice_2, 3, "void", "double, double, double")
{
   aObjectPtr->Slice(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerMoverClass, WsfBrawlerMover, Prlvl, 2, "void", "double, double")
{
   aObjectPtr->Prlvl(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerMoverClass, WsfBrawlerMover, Prlvlg, 3, "void", "double, double, double")
{
   aObjectPtr->Prlvlg(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBrawlerMoverClass, WsfBrawlerMover, SetMaxThrottle, 1, "void", "double")
{
   aObjectPtr->SetMaxThrottle(aVarArgs[0].GetDouble());
}
