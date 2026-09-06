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
*   File: WsfBMInterceptCalcResult.hpp
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

#ifndef WSF_BM_INTERCEPT_CALC_RESULT_HPP
#define WSF_BM_INTERCEPT_CALC_RESULT_HPP

#include <WsfBMCommon.hpp>
#include <WsfObject.hpp>
#include <WsfGeoPoint.hpp>

#include <UtMemory.hpp>

#define INT_CALC_RESULT_SCRIPT_CLASS_NAME "WsfBMIntCalcResult"
#define INT_CALC_RESULT_BASE_CLASS_NAME "WSF_BM_INT_CALC_RESULT"

class WSF_IADS_C2_LIB_EXPORT WsfBMInterceptCalcResult : public WsfObject
{
   public:

      WsfBMInterceptCalcResult();

      WsfBMInterceptCalcResult* Clone() const override;

      const char* GetScriptClassName() const override { return INT_CALC_RESULT_SCRIPT_CLASS_NAME; }
      virtual const char* GetBaseClassName() { return INT_CALC_RESULT_BASE_CLASS_NAME; }

      inline virtual void SetCanIntercept(const bool value) { m_can_intercept = value; }
      inline virtual bool GetCanIntercept() const { return m_can_intercept; }

      inline virtual void SetTTI(const double value) { m_tti = value; }
      inline virtual double GetTTI() const { return m_tti; }

      inline virtual void SetPredPoint(const WsfGeoPoint& value) { m_pred_int_pt = value; }
      inline virtual WsfGeoPoint GetPredPoint() const { return m_pred_int_pt; }

      inline virtual void SetEstimatedPK(const double value) { m_estimated_pk = value; }
      inline virtual double GetEstimatedPK() const { return m_estimated_pk; }

      inline virtual void SetWeaponMaxRange(const double value) { m_max_range = value; }
      inline virtual double GetWeaponMaxRange() const { return m_max_range; } // max range of the weapon given current conditions against this target

   private:
      bool m_can_intercept;
      double m_tti;
      WsfGeoPoint m_pred_int_pt;
      double m_estimated_pk;
      double m_max_range;
};

//////////////////
// Script Class //
//////////////////
#include <UtScriptBasicTypes.hpp>

class WSF_IADS_C2_LIB_EXPORT WsfScriptBMInterceptCalcResultClass : public UtScriptClass
{
   public:
      WsfScriptBMInterceptCalcResultClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
      ~WsfScriptBMInterceptCalcResultClass();

      void* Create(const UtScriptContext& aContext) override;
      void* Clone(void* aObjectPtr) override;
      void Destroy(void* aObjectPtr) override;

      UT_DECLARE_SCRIPT_METHOD(SetCantIntercept);
      UT_DECLARE_SCRIPT_METHOD(SetCanIntercept);
      UT_DECLARE_SCRIPT_METHOD(SetCanIntercept_2);
      UT_DECLARE_SCRIPT_METHOD(GetCanIntercept);

      UT_DECLARE_SCRIPT_METHOD(SetTTI);
      UT_DECLARE_SCRIPT_METHOD(GetTTI);

      UT_DECLARE_SCRIPT_METHOD(SetPredPoint);
      UT_DECLARE_SCRIPT_METHOD(GetPredPoint);

      UT_DECLARE_SCRIPT_METHOD(GetEstimatedPK);
      UT_DECLARE_SCRIPT_METHOD(SetEstimatedPK);

      UT_DECLARE_SCRIPT_METHOD(GetWeaponMaxRange);
      UT_DECLARE_SCRIPT_METHOD(SetWeaponMaxRange);
};

#endif
