// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: WsfBMInterceptCalcResult.cpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name: Radiance Technologies
*   Address:         350 Wynn Drive
*                    Huntsville, AL 35805
*
*   Abstract: Type for returning intercept calculation results.
*
*******************************************************************************/

#include <WsfBMInterceptCalcResult.hpp>

WsfBMInterceptCalcResult::WsfBMInterceptCalcResult()
   : m_can_intercept(false)
   , m_tti(-1)
   , m_estimated_pk(0)
   , m_max_range(0)
{
}

WsfBMInterceptCalcResult* WsfBMInterceptCalcResult::Clone() const
{
   return new WsfBMInterceptCalcResult(*this);
}

//////////////////
// Script Class //
//////////////////
WsfScriptBMInterceptCalcResultClass::WsfScriptBMInterceptCalcResultClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName(INT_CALC_RESULT_SCRIPT_CLASS_NAME);

   mConstructible = true;
   mCloneable = true;

   AddMethod(ut::make_unique<SetCantIntercept>());
   AddMethod(ut::make_unique<SetCanIntercept>());
   AddMethod(ut::make_unique<SetCanIntercept_2>("SetCanIntercept"));
   AddMethod(ut::make_unique<GetCanIntercept>());

   AddMethod(ut::make_unique<SetTTI>());
   AddMethod(ut::make_unique<GetTTI>());

   AddMethod(ut::make_unique<SetPredPoint>());
   AddMethod(ut::make_unique<GetPredPoint>());

   AddMethod(ut::make_unique<SetEstimatedPK>());
   AddMethod(ut::make_unique<GetEstimatedPK>());

   AddMethod(ut::make_unique<SetWeaponMaxRange>());
   AddMethod(ut::make_unique<GetWeaponMaxRange>());
}

WsfScriptBMInterceptCalcResultClass::~WsfScriptBMInterceptCalcResultClass()
{
}

void* WsfScriptBMInterceptCalcResultClass::Create(const UtScriptContext& aContext)
{
   WsfBMInterceptCalcResult* obj_ptr = new WsfBMInterceptCalcResult();
   return obj_ptr;
}

void* WsfScriptBMInterceptCalcResultClass::Clone(void* aObjectPtr)
{
   return reinterpret_cast<WsfBMInterceptCalcResult*>(aObjectPtr)->Clone();
}

void  WsfScriptBMInterceptCalcResultClass::Destroy(void* aObjectPtr)
{
   WsfBMInterceptCalcResult* objPtr = reinterpret_cast<WsfBMInterceptCalcResult*>(aObjectPtr);
   delete objPtr;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMInterceptCalcResultClass, WsfBMInterceptCalcResult, SetCantIntercept, 0, "void", "")
{
   aObjectPtr->SetCanIntercept(false);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMInterceptCalcResultClass, WsfBMInterceptCalcResult, SetCanIntercept, 0, "void", "")
{
   aObjectPtr->SetCanIntercept(true);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMInterceptCalcResultClass, WsfBMInterceptCalcResult, SetCanIntercept_2, 1, "void", "bool")
{
   auto can_intercept = aVarArgs[0].GetBool();
   aObjectPtr->SetCanIntercept(can_intercept);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMInterceptCalcResultClass, WsfBMInterceptCalcResult, GetCanIntercept, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetCanIntercept());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMInterceptCalcResultClass, WsfBMInterceptCalcResult, SetTTI, 1, "void", "double")
{
   auto value = aVarArgs[0].GetDouble();

   aObjectPtr->SetTTI(value);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMInterceptCalcResultClass, WsfBMInterceptCalcResult, GetTTI, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTTI());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMInterceptCalcResultClass, WsfBMInterceptCalcResult, SetPredPoint, 1, "void", "WsfGeoPoint")
{
   WsfGeoPoint* value = reinterpret_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetPredPoint(*value);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMInterceptCalcResultClass, WsfBMInterceptCalcResult, GetPredPoint, 0, "WsfGeoPoint", "")
{
   WsfGeoPoint* pred_pt = new WsfGeoPoint(aObjectPtr->GetPredPoint());
   aReturnVal.SetPointer(new UtScriptRef(pred_pt, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMInterceptCalcResultClass, WsfBMInterceptCalcResult, SetEstimatedPK, 1, "void", "double")
{
   auto value = aVarArgs[0].GetDouble();
   aObjectPtr->SetEstimatedPK(value);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMInterceptCalcResultClass, WsfBMInterceptCalcResult, GetEstimatedPK, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetEstimatedPK());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMInterceptCalcResultClass, WsfBMInterceptCalcResult, SetWeaponMaxRange, 1, "void", "double")
{
   auto value = aVarArgs[0].GetDouble();
   aObjectPtr->SetWeaponMaxRange(value);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMInterceptCalcResultClass, WsfBMInterceptCalcResult, GetWeaponMaxRange, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetWeaponMaxRange());
}
