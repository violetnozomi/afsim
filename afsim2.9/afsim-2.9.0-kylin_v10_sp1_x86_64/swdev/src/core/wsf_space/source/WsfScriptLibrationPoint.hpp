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

#ifndef WSFSCRIPTLIBRATIONPOINT_HPP
#define WSFSCRIPTLIBRATIONPOINT_HPP

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

class WsfScriptLibrationPoint : public UtScriptClass
{
public:
   WsfScriptLibrationPoint(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptLibrationPoint() override = default;

   void Destroy(void* aObjectPtr) override;

   // Static creation Method
   UT_DECLARE_SCRIPT_METHOD(Construct);

   UT_DECLARE_SCRIPT_METHOD(System);
   UT_DECLARE_SCRIPT_METHOD(LocationECI);
   UT_DECLARE_SCRIPT_METHOD(VelocityECI);
   UT_DECLARE_SCRIPT_METHOD(TransformECI_PositionToRF);
   UT_DECLARE_SCRIPT_METHOD(TransformECI_VelocityToRF);
   UT_DECLARE_SCRIPT_METHOD(TransformRF_PositionToECI);
   UT_DECLARE_SCRIPT_METHOD(TransformRF_VelocityToECI);
};

class WsfScriptLibrationPointSystem : public UtScriptClass
{
public:
   WsfScriptLibrationPointSystem(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptLibrationPointSystem() override = default;

   void Destroy(void* aObjectPtr) override;
   bool EqualTo(void* aLHS, void* aRHS) override;

   // Static creation methods
   UT_DECLARE_SCRIPT_METHOD(SUN_EARTH);
   UT_DECLARE_SCRIPT_METHOD(EARTH_MOON);
   UT_DECLARE_SCRIPT_METHOD(SUN_JUPITER);
};

class WsfScriptLibrationPointSelection : public UtScriptClass
{
public:
   WsfScriptLibrationPointSelection(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptLibrationPointSelection() override = default;

   void Destroy(void* aObjectPtr) override;
   bool EqualTo(void* aLHS, void* aRHS) override;

   // Static creation methods
   UT_DECLARE_SCRIPT_METHOD(L1);
   UT_DECLARE_SCRIPT_METHOD(L2);
   UT_DECLARE_SCRIPT_METHOD(L3);
   UT_DECLARE_SCRIPT_METHOD(L4);
   UT_DECLARE_SCRIPT_METHOD(L5);
};

#endif // WSFSCRIPTLIBRATIONPOINT_HPP
