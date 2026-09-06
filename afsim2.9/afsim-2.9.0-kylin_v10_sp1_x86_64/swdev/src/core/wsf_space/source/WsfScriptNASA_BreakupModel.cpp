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

#include "WsfScriptNASA_BreakupModel.hpp"

#include "UtCast.hpp"
#include "UtMemory.hpp"
#include "WsfNASA_BreakupModel.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptDefs.hpp"

WsfScriptNASA_BreakupModel::WsfScriptNASA_BreakupModel(UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass("WsfNASA_BreakupModel", aScriptTypesPtr)
{
   mConstructible = true;

   SetClassName("WsfNASA_BreakupModel");

   AddMethod(ut::make_unique<ModelExplosion>());
   AddMethod(ut::make_unique<ModelCollision>());
   AddMethod(ut::make_unique<HasGeneratedDebris>());
   AddMethod(ut::make_unique<GetFragmentCount>());
   AddMethod(ut::make_unique<GetFragment>());
   AddMethod(ut::make_unique<SetFragmentPlatformType>());
   AddMethod(ut::make_unique<GetFragmentPlatformType>());
   AddMethod(ut::make_unique<GetDebrisName>());
   AddMethod(ut::make_unique<SetDebrisName>());
   AddMethod(ut::make_unique<GetLargeFragmentFraction>());
   AddMethod(ut::make_unique<SetLargeFragmentFraction>());
   AddMethod(ut::make_unique<IsModeledAsSpacecraft>());
   AddMethod(ut::make_unique<SetModeledAsSpacecraft>());
   AddMethod(ut::make_unique<GetMinFragmentSize>());
   AddMethod(ut::make_unique<SetMinFragmentSize>());
   AddMethod(ut::make_unique<GetExplosionS_Factor>());
   AddMethod(ut::make_unique<SetExplosionS_Factor>());
   AddMethod(ut::make_unique<GetExplosionProgenitor>());
   AddMethod(ut::make_unique<GetExplosionMass>());
   AddMethod(ut::make_unique<GetCollisionProgenitorOne>());
   AddMethod(ut::make_unique<GetCollisionProgenitorOneMass>());
   AddMethod(ut::make_unique<GetCollisionProgenitorTwo>());
   AddMethod(ut::make_unique<GetCollisionProgenitorTwoMass>());
   AddMethod(ut::make_unique<GetCollisionMass>());
}

void* WsfScriptNASA_BreakupModel::Create(const UtScriptContext& aInstance)
{
   WsfSimulation* simulationPtr = WsfScriptContext::GetSIMULATION(aInstance);
   if (simulationPtr)
   {
      return new WsfNASA_BreakupModel{*simulationPtr};
   }
   return nullptr;
}

void WsfScriptNASA_BreakupModel::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfNASA_BreakupModel*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNASA_BreakupModel, WsfNASA_BreakupModel, ModelExplosion, 3, "bool", "string, double, double")
{
   std::string platformName = aVarArgs[0].GetString();
   double      mass         = aVarArgs[1].GetDouble();
   double      size         = aVarArgs[2].GetDouble();
   bool        retval =
      aObjectPtr->ExplosiveBreakup(TIME_NOW, platformName, WsfNASA_BreakupModel::PackInputParameters(mass, size));
   aReturnVal.SetBool(retval);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNASA_BreakupModel,
                        WsfNASA_BreakupModel,
                        ModelCollision,
                        6,
                        "bool",
                        "string, double, double, string, double, double")
{
   std::string         platformOneName = aVarArgs[0].GetString();
   std::vector<double> platformOneParams =
      WsfNASA_BreakupModel::PackInputParameters(aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble());
   std::string         platformTwoName = aVarArgs[3].GetString();
   std::vector<double> platformTwoParams =
      WsfNASA_BreakupModel::PackInputParameters(aVarArgs[4].GetDouble(), aVarArgs[5].GetDouble());
   bool retval =
      aObjectPtr->CollisionalBreakup(TIME_NOW, platformOneName, platformOneParams, platformTwoName, platformTwoParams);
   aReturnVal.SetBool(retval);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNASA_BreakupModel, WsfNASA_BreakupModel, HasGeneratedDebris, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->HasGenerated());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNASA_BreakupModel, WsfNASA_BreakupModel, GetFragmentCount, 0, "int", "")
{
   aReturnVal.SetInt(ut::safe_cast<int>(aObjectPtr->GetFragmentCount()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNASA_BreakupModel, WsfNASA_BreakupModel, GetFragment, 1, "WsfSatelliteBreakupFragment", "int")
{
   size_t index       = ut::safe_cast<size_t>(aVarArgs[0].GetInt());
   auto   fragmentPtr = new WsfSatelliteBreakupModel::Fragment{aObjectPtr->GetFragment(index)};
   aReturnVal.SetPointer(new UtScriptRef(fragmentPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNASA_BreakupModel, WsfNASA_BreakupModel, SetFragmentPlatformType, 1, "void", "string")
{
   aObjectPtr->SetFragmentPlatformType(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNASA_BreakupModel, WsfNASA_BreakupModel, GetFragmentPlatformType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetFragmentPlatformType());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNASA_BreakupModel, WsfNASA_BreakupModel, GetDebrisName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetDebrisName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNASA_BreakupModel, WsfNASA_BreakupModel, SetDebrisName, 1, "void", "string")
{
   aObjectPtr->SetDebrisName(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNASA_BreakupModel, WsfNASA_BreakupModel, GetLargeFragmentFraction, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetLargeFragmentMassFraction());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNASA_BreakupModel, WsfNASA_BreakupModel, SetLargeFragmentFraction, 1, "void", "double")
{
   aObjectPtr->SetLargeFragmentMassFraction(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNASA_BreakupModel, WsfNASA_BreakupModel, IsModeledAsSpacecraft, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsModeledAsSpacecraft());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNASA_BreakupModel, WsfNASA_BreakupModel, SetModeledAsSpacecraft, 1, "void", "bool")
{
   aObjectPtr->SetModeledAsSpacecraft(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNASA_BreakupModel, WsfNASA_BreakupModel, GetMinFragmentSize, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMinFragmentSize());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNASA_BreakupModel, WsfNASA_BreakupModel, SetMinFragmentSize, 1, "void", "double")
{
   aObjectPtr->SetMinFragmentSize(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNASA_BreakupModel, WsfNASA_BreakupModel, GetExplosionS_Factor, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetExplosionS_Factor());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNASA_BreakupModel, WsfNASA_BreakupModel, SetExplosionS_Factor, 1, "void", "double")
{
   aObjectPtr->SetExplosionS_Factor(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNASA_BreakupModel, WsfNASA_BreakupModel, GetExplosionProgenitor, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetExplosionProgenitor());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNASA_BreakupModel, WsfNASA_BreakupModel, GetExplosionMass, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetExplosionMass());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNASA_BreakupModel, WsfNASA_BreakupModel, GetCollisionProgenitorOne, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetCollisionProgenitorOne());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNASA_BreakupModel, WsfNASA_BreakupModel, GetCollisionProgenitorOneMass, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetCollisionProgenitorOneMass());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNASA_BreakupModel, WsfNASA_BreakupModel, GetCollisionProgenitorTwo, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetCollisionProgenitorTwo());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNASA_BreakupModel, WsfNASA_BreakupModel, GetCollisionProgenitorTwoMass, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetCollisionProgenitorTwoMass());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptNASA_BreakupModel, WsfNASA_BreakupModel, GetCollisionMass, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetCollisionMass());
}
