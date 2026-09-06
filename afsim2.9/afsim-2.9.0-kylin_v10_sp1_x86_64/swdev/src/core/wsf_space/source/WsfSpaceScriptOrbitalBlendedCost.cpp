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

#include "WsfSpaceScriptOrbitalBlendedCost.hpp"

#include "WsfOrbitalTargetingCost.hpp"

namespace wsf
{
namespace space
{
namespace script
{

OrbitalBlendedCost::OrbitalBlendedCost(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalOptimizationCost(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfOrbitalBlendedCost");

   AddStaticMethod(ut::make_unique<Construct>());
   AddStaticMethod(ut::make_unique<ConstructLeastTime>());
   AddStaticMethod(ut::make_unique<ConstructLeastDeltaV>());

   AddMethod(ut::make_unique<A_Value>());
   AddMethod(ut::make_unique<B_Value>());
   AddMethod(ut::make_unique<C_Value>());

   AddMethod(ut::make_unique<SetA_Value>());
   AddMethod(ut::make_unique<SetB_Value>());
   AddMethod(ut::make_unique<SetC_Value>());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalBlendedCost, wsf::space::BlendedCost, Construct, 3, "WsfOrbitalBlendedCost", "double, double, double")
{
   auto retvalPtr = ut::make_unique<wsf::space::BlendedCost>();
   retvalPtr->SetA_Value(aVarArgs[0].GetDouble());
   retvalPtr->SetB_Value(aVarArgs[1].GetDouble());
   retvalPtr->SetC_Value(aVarArgs[2].GetDouble());
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalBlendedCost, wsf::space::BlendedCost, ConstructLeastTime, 0, "WsfOrbitalBlendedCost", "")
{
   auto retvalPtr = ut::make_unique<wsf::space::BlendedCost>();
   retvalPtr->SetA_Value(1.0);
   retvalPtr->SetB_Value(0.0);
   retvalPtr->SetC_Value(0.0);
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalBlendedCost, wsf::space::BlendedCost, ConstructLeastDeltaV, 0, "WsfOrbitalBlendedCost", "")
{
   auto retvalPtr = ut::make_unique<wsf::space::BlendedCost>();
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalBlendedCost, wsf::space::BlendedCost, A_Value, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetA_Value());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalBlendedCost, wsf::space::BlendedCost, B_Value, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetB_Value());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalBlendedCost, wsf::space::BlendedCost, C_Value, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetC_Value());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalBlendedCost, wsf::space::BlendedCost, SetA_Value, 1, "void", "double")
{
   aObjectPtr->SetA_Value(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalBlendedCost, wsf::space::BlendedCost, SetB_Value, 1, "void", "double")
{
   aObjectPtr->SetB_Value(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalBlendedCost, wsf::space::BlendedCost, SetC_Value, 1, "void", "double")
{
   aObjectPtr->SetC_Value(aVarArgs[0].GetDouble());
}

} // namespace script
} // namespace space
} // namespace wsf
