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

#include "WsfScriptSixDOF_FormationOffset.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfSixDOF_FormationOffset.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
FormationOffset::FormationOffset(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSixDOF_FormationOffset");

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

void* FormationOffset::Create(const UtScriptContext& aContext)
{
   return new wsf::six_dof::FormationOffset{};
}

void FormationOffset::Destroy(void* aObjectPtr)
{
   delete static_cast<wsf::six_dof::FormationOffset*>(aObjectPtr);
}

void* FormationOffset::Clone(void* aObjectPtr)
{
   return new wsf::six_dof::FormationOffset(*static_cast<wsf::six_dof::FormationOffset*>(aObjectPtr));
}

UT_DEFINE_SCRIPT_METHOD(FormationOffset,
                        wsf::six_dof::FormationOffset,
                        Construct_1,
                        4,
                        "WsfSixDOF_FormationOffset",
                        "double, double, double, bool")
{
   wsf::six_dof::FormationOffset* offsetPtr = new wsf::six_dof::FormationOffset{};
   offsetPtr->SetRangeAndBearing(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG);
   offsetPtr->SetStack(aVarArgs[2].GetDouble());
   offsetPtr->SetWelded(aVarArgs[3].GetBool());
   aReturnVal.SetPointer(new UtScriptRef(offsetPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(FormationOffset, wsf::six_dof::FormationOffset, Construct_2, 2, "WsfSixDOF_FormationOffset", "Vec3, bool")
{
   wsf::six_dof::FormationOffset* offsetPtr = new wsf::six_dof::FormationOffset{};
   UtVec3d*                       coordsPtr = static_cast<UtVec3d*>(aVarArgs[0].GetPointer()->GetAppObject());
   offsetPtr->SetRight(coordsPtr->Get(0));
   offsetPtr->SetAhead(coordsPtr->Get(1));
   offsetPtr->SetStack(coordsPtr->Get(2));
   offsetPtr->SetWelded(aVarArgs[1].GetBool());
   aReturnVal.SetPointer(new UtScriptRef(offsetPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(FormationOffset, wsf::six_dof::FormationOffset, GetStack, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetStack());
}

UT_DEFINE_SCRIPT_METHOD(FormationOffset, wsf::six_dof::FormationOffset, GetRight, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRight());
}

UT_DEFINE_SCRIPT_METHOD(FormationOffset, wsf::six_dof::FormationOffset, GetAhead, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetAhead());
}

UT_DEFINE_SCRIPT_METHOD(FormationOffset, wsf::six_dof::FormationOffset, GetRelativeBearing, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRelativeBearing() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(FormationOffset, wsf::six_dof::FormationOffset, GetRange, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRange());
}

UT_DEFINE_SCRIPT_METHOD(FormationOffset, wsf::six_dof::FormationOffset, IsWelded, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsWelded());
}

UT_DEFINE_SCRIPT_METHOD(FormationOffset, wsf::six_dof::FormationOffset, SetStack, 1, "void", "double")
{
   aObjectPtr->SetStack(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(FormationOffset, wsf::six_dof::FormationOffset, SetRight, 1, "void", "double")
{
   aObjectPtr->SetRight(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(FormationOffset, wsf::six_dof::FormationOffset, SetAhead, 1, "void", "double")
{
   aObjectPtr->SetAhead(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(FormationOffset, wsf::six_dof::FormationOffset, SetRangeAndBearing, 2, "void", "double, double")
{
   aObjectPtr->SetRangeAndBearing(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG);
}

UT_DEFINE_SCRIPT_METHOD(FormationOffset, wsf::six_dof::FormationOffset, SetWelded, 1, "void", "bool")
{
   aObjectPtr->SetWelded(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(FormationOffset,
                        wsf::six_dof::FormationOffset,
                        Add,
                        2,
                        "WsfSixDOF_FormationOffset",
                        "WsfSixDOF_FormationOffset, WsfSixDOF_FormationOffset")
{
   wsf::six_dof::FormationOffset* leftPtr =
      static_cast<wsf::six_dof::FormationOffset*>(aVarArgs[0].GetPointer()->GetAppObject());
   wsf::six_dof::FormationOffset* rightPtr =
      static_cast<wsf::six_dof::FormationOffset*>(aVarArgs[1].GetPointer()->GetAppObject());
   wsf::six_dof::FormationOffset* retvalPtr = new wsf::six_dof::FormationOffset(*leftPtr);
   retvalPtr->                    operator+=(*rightPtr);
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(FormationOffset,
                        wsf::six_dof::FormationOffset,
                        Subtract,
                        2,
                        "WsfSixDOF_FormationOffset",
                        "WsfSixDOF_FormationOffset, WsfSixDOF_FormationOffset")
{
   wsf::six_dof::FormationOffset* leftPtr =
      static_cast<wsf::six_dof::FormationOffset*>(aVarArgs[0].GetPointer()->GetAppObject());
   wsf::six_dof::FormationOffset* rightPtr =
      static_cast<wsf::six_dof::FormationOffset*>(aVarArgs[1].GetPointer()->GetAppObject());
   wsf::six_dof::FormationOffset* retvalPtr = new wsf::six_dof::FormationOffset(*leftPtr);
   retvalPtr->                    operator-=(*rightPtr);
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
} // namespace script
} // namespace six_dof
} // namespace wsf
