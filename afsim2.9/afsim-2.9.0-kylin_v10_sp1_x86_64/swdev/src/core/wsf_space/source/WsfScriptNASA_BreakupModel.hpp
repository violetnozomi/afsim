// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSCRIPTNASABREAKUPMODEL_HPP
#define WSFSCRIPTNASABREAKUPMODEL_HPP

#include "wsf_space_export.h"

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

class WSF_SPACE_EXPORT WsfScriptNASA_BreakupModel : public UtScriptClass
{
public:
   explicit WsfScriptNASA_BreakupModel(UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptNASA_BreakupModel() override = default;

   void* Create(const UtScriptContext& aInstance) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(ModelExplosion);
   UT_DECLARE_SCRIPT_METHOD(ModelCollision);
   UT_DECLARE_SCRIPT_METHOD(HasGeneratedDebris);
   UT_DECLARE_SCRIPT_METHOD(GetFragmentCount);
   UT_DECLARE_SCRIPT_METHOD(GetFragment);
   UT_DECLARE_SCRIPT_METHOD(SetFragmentPlatformType);
   UT_DECLARE_SCRIPT_METHOD(GetFragmentPlatformType);

   UT_DECLARE_SCRIPT_METHOD(GetDebrisName);
   UT_DECLARE_SCRIPT_METHOD(SetDebrisName);
   UT_DECLARE_SCRIPT_METHOD(GetLargeFragmentFraction);
   UT_DECLARE_SCRIPT_METHOD(SetLargeFragmentFraction);
   UT_DECLARE_SCRIPT_METHOD(IsModeledAsSpacecraft);
   UT_DECLARE_SCRIPT_METHOD(SetModeledAsSpacecraft);
   UT_DECLARE_SCRIPT_METHOD(GetMinFragmentSize);
   UT_DECLARE_SCRIPT_METHOD(SetMinFragmentSize);

   UT_DECLARE_SCRIPT_METHOD(GetExplosionS_Factor);
   UT_DECLARE_SCRIPT_METHOD(SetExplosionS_Factor);
   UT_DECLARE_SCRIPT_METHOD(GetExplosionProgenitor);
   UT_DECLARE_SCRIPT_METHOD(GetExplosionMass);

   UT_DECLARE_SCRIPT_METHOD(GetCollisionProgenitorOne);
   UT_DECLARE_SCRIPT_METHOD(GetCollisionProgenitorOneMass);
   UT_DECLARE_SCRIPT_METHOD(GetCollisionProgenitorTwo);
   UT_DECLARE_SCRIPT_METHOD(GetCollisionProgenitorTwoMass);
   UT_DECLARE_SCRIPT_METHOD(GetCollisionMass);
};

#endif // WSFSCRIPTNASABREAKUPMODEL_HPP
