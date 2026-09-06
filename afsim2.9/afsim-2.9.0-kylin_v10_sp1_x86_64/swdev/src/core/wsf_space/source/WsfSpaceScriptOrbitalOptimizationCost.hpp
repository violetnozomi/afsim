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

#ifndef WSFSPACESCRIPTORBITALOPTIMIZATIONCOST_HPP
#define WSFSPACESCRIPTORBITALOPTIMIZATIONCOST_HPP

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

namespace wsf
{
namespace space
{
namespace script
{

class OrbitalOptimizationCost : public UtScriptClass
{
public:
   OrbitalOptimizationCost(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~OrbitalOptimizationCost() override = default;

   void Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(Cost);
};

} // namespace script
} // namespace space
} // namespace wsf

#endif // WSFSPACESCRIPTORBITALOPTIMIZATIONCOST_HPP
