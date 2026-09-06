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

#include "WsfSpaceScriptOrbitalOptimizationCost.hpp"

#include "WsfOrbitalTargetingCost.hpp"

namespace wsf
{
namespace space
{
namespace script
{

OrbitalOptimizationCost::OrbitalOptimizationCost(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfOrbitalOptimizationCost");

   mIsScriptAccessible = true;

   AddMethod(ut::make_unique<Cost>());
}

void OrbitalOptimizationCost::Destroy(void* aObjectPtr)
{
   delete static_cast<wsf::space::OrbitalTargetingCost*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(OrbitalOptimizationCost, wsf::space::OrbitalTargetingCost, Cost, 2, "double", "double, double")
{
   aReturnVal.SetDouble((*aObjectPtr)(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble()));
}

} // namespace script
} // namespace space
} // namespace wsf
