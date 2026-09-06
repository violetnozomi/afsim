// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptFormationOffset.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfFormationOffset.hpp"

WsfScriptFormationOffset::WsfScriptFormationOffset(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfFormationOffset");

   AddStaticMethod(ut::make_unique<Construct_1>("Construct"));
   AddStaticMethod(ut::make_unique<Construct_2>("Construct"));

   AddMethod(ut::make_unique<GetStack>());
   AddMethod(ut::make_unique<GetRight>());
   AddMethod(ut::make_unique<GetAhead>());
   AddMethod(ut::make_unique<GetRelativeBearing>());
   AddMethod(ut::make_unique<GetRange>());
   AddMethod(ut::make_unique<IsWelded>());

   AddMethod(ut::make_unique<SetStack>());
   AddMethod(ut::make_unique<SetRight>());
   AddMethod(ut::make_unique<SetAhead>());
   AddMethod(ut::make_unique<SetRangeAndBearing>());
   AddMethod(ut::make_unique<SetWelded>());

   AddStaticMethod(ut::make_unique<Add>());
   AddStaticMethod(ut::make_unique<Subtract>());
}

void* WsfScriptFormationOffset::Create(const UtScriptContext& aContext)
{
   return new WsfFormationOffset{};
}

void WsfScriptFormationOffset::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfFormationOffset*>(aObjectPtr);
}

void* WsfScriptFormationOffset::Clone(void* aObjectPtr)
{
   return new WsfFormationOffset(*static_cast<WsfFormationOffset*>(aObjectPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationOffset, WsfFormationOffset, Construct_1, 4, "WsfFormationOffset", "double, double, double, bool")
{
   WsfFormationOffset* offsetPtr = new WsfFormationOffset{};
   offsetPtr->SetRangeAndBearing(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG);
   offsetPtr->SetStack(aVarArgs[2].GetDouble());
   offsetPtr->SetWelded(aVarArgs[3].GetBool());
   aReturnVal.SetPointer(new UtScriptRef(offsetPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationOffset, WsfFormationOffset, Construct_2, 2, "WsfFormationOffset", "Vec3, bool")
{
   WsfFormationOffset* offsetPtr = new WsfFormationOffset{};
   UtVec3d*            coordsPtr = static_cast<UtVec3d*>(aVarArgs[0].GetPointer()->GetAppObject());
   offsetPtr->SetRight(coordsPtr->Get(0));
   offsetPtr->SetAhead(coordsPtr->Get(1));
   offsetPtr->SetStack(coordsPtr->Get(2));
   offsetPtr->SetWelded(aVarArgs[1].GetBool());
   aReturnVal.SetPointer(new UtScriptRef(offsetPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationOffset, WsfFormationOffset, GetStack, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetStack());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationOffset, WsfFormationOffset, GetRight, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRight());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationOffset, WsfFormationOffset, GetAhead, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetAhead());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationOffset, WsfFormationOffset, GetRelativeBearing, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRelativeBearing() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationOffset, WsfFormationOffset, GetRange, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRange());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationOffset, WsfFormationOffset, IsWelded, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsWelded());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationOffset, WsfFormationOffset, SetStack, 1, "void", "double")
{
   aObjectPtr->SetStack(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationOffset, WsfFormationOffset, SetRight, 1, "void", "double")
{
   aObjectPtr->SetRight(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationOffset, WsfFormationOffset, SetAhead, 1, "void", "double")
{
   aObjectPtr->SetAhead(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationOffset, WsfFormationOffset, SetRangeAndBearing, 2, "void", "double, double")
{
   aObjectPtr->SetRangeAndBearing(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationOffset, WsfFormationOffset, SetWelded, 1, "void", "bool")
{
   aObjectPtr->SetWelded(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationOffset, WsfFormationOffset, Add, 2, "WsfFormationOffset", "WsfFormationOffset, WsfFormationOffset")
{
   WsfFormationOffset* leftPtr   = static_cast<WsfFormationOffset*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfFormationOffset* rightPtr  = static_cast<WsfFormationOffset*>(aVarArgs[1].GetPointer()->GetAppObject());
   WsfFormationOffset* retvalPtr = new WsfFormationOffset(*leftPtr);
   retvalPtr->         operator+=(*rightPtr);
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationOffset,
                        WsfFormationOffset,
                        Subtract,
                        2,
                        "WsfFormationOffset",
                        "WsfFormationOffset, WsfFormationOffset")
{
   WsfFormationOffset* leftPtr   = static_cast<WsfFormationOffset*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfFormationOffset* rightPtr  = static_cast<WsfFormationOffset*>(aVarArgs[1].GetPointer()->GetAppObject());
   WsfFormationOffset* retvalPtr = new WsfFormationOffset(*leftPtr);
   retvalPtr->         operator-=(*rightPtr);
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
