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

#ifndef WSFSPACESCRIPTORBITALBLENDEDCOST_HPP
#define WSFSPACESCRIPTORBITALBLENDEDCOST_HPP

#include "WsfSpaceScriptOrbitalOptimizationCost.hpp"

namespace wsf
{
namespace space
{
namespace script
{

class OrbitalBlendedCost : public OrbitalOptimizationCost
{
public:
   OrbitalBlendedCost(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~OrbitalBlendedCost() override = default;

   UT_DECLARE_SCRIPT_METHOD(Construct);
   UT_DECLARE_SCRIPT_METHOD(ConstructLeastTime);
   UT_DECLARE_SCRIPT_METHOD(ConstructLeastDeltaV);

   UT_DECLARE_SCRIPT_METHOD(A_Value);
   UT_DECLARE_SCRIPT_METHOD(B_Value);
   UT_DECLARE_SCRIPT_METHOD(C_Value);

   UT_DECLARE_SCRIPT_METHOD(SetA_Value);
   UT_DECLARE_SCRIPT_METHOD(SetB_Value);
   UT_DECLARE_SCRIPT_METHOD(SetC_Value);
};

} // namespace script
} // namespace space
} // namespace wsf

#endif // WSFSPACESCRIPTORBITALBLENDEDCOST_HPP
