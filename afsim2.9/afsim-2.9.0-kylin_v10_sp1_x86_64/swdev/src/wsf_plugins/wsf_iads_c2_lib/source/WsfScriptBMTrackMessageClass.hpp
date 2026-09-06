// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#ifndef WSF_SCRIPT_BM_TRACK_MESSAGE_CLASS_HPP
#define WSF_SCRIPT_BM_TRACK_MESSAGE_CLASS_HPP

#include <script/WsfScriptMessageClass.hpp>
#include <UtMemory.hpp>

class WsfScriptBMTrackMessageClass : public WsfScriptMessageClass
{
   public:
      WsfScriptBMTrackMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
      virtual ~WsfScriptBMTrackMessageClass();

      void* Create(const UtScriptContext& aContext) override;
      void* Clone(void* aObjectPtr) override;
      void Destroy(void* aObjectPtr) override;

      static const char* SCRIPT_CLASS_NAME;
      static const char* BASE_CLASS_NAME;

      UT_DECLARE_SCRIPT_METHOD(SetUpdateInterval);
      UT_DECLARE_SCRIPT_METHOD(GetUpdateInterval);

      UT_DECLARE_SCRIPT_METHOD(SetTrackID);
      UT_DECLARE_SCRIPT_METHOD(GetTrackingSystemID);
      UT_DECLARE_SCRIPT_METHOD(GetTrackingSystemTrackID);

      UT_DECLARE_SCRIPT_METHOD(SetPosition);
      UT_DECLARE_SCRIPT_METHOD(GetPosition);
      UT_DECLARE_SCRIPT_METHOD(GetPositionTime);

      UT_DECLARE_SCRIPT_METHOD(SetVelocityECEF);
      UT_DECLARE_SCRIPT_METHOD(GetVelocityECEF);

      UT_DECLARE_SCRIPT_METHOD(SetCovarianceMatrix);
      UT_DECLARE_SCRIPT_METHOD(GetCovarianceMatrix);

      UT_DECLARE_SCRIPT_METHOD(SetHeadingENUDegs);

      UT_DECLARE_SCRIPT_METHOD(SetOrientationWCSDegs);
      UT_DECLARE_SCRIPT_METHOD(GetOrientationWCSPsiDegs);
      UT_DECLARE_SCRIPT_METHOD(GetOrientationWCSThetaDegs);
      UT_DECLARE_SCRIPT_METHOD(GetOrientationWCSPhiDegs);

      UT_DECLARE_SCRIPT_METHOD(SetTargetType);
      UT_DECLARE_SCRIPT_METHOD(GetTargetType);
      UT_DECLARE_SCRIPT_METHOD(GetTargetSubType);

      UT_DECLARE_SCRIPT_METHOD(SetTrackingStatusNormal);
      UT_DECLARE_SCRIPT_METHOD(SetTrackingStatusCoasting);
      UT_DECLARE_SCRIPT_METHOD(SetTrackingStatusTimedOut);
      UT_DECLARE_SCRIPT_METHOD(SetTrackingStatusDropping);
      UT_DECLARE_SCRIPT_METHOD(SetTrackingStatusDropped);
      UT_DECLARE_SCRIPT_METHOD(IsTrackingStatusNormal);
      UT_DECLARE_SCRIPT_METHOD(IsTrackingStatusCoasting);
      UT_DECLARE_SCRIPT_METHOD(IsTrackingStatusTimedOut);
      UT_DECLARE_SCRIPT_METHOD(IsTrackingStatusDropping);
      UT_DECLARE_SCRIPT_METHOD(IsTrackingStatusDropped);

      UT_DECLARE_SCRIPT_METHOD(SetAltitudeReliable);
      UT_DECLARE_SCRIPT_METHOD(GetAltitudeReliable);

      UT_DECLARE_SCRIPT_METHOD(SetIFFUnknown);
      UT_DECLARE_SCRIPT_METHOD(SetIFFFriendly);
      UT_DECLARE_SCRIPT_METHOD(SetIFFHostile);
      UT_DECLARE_SCRIPT_METHOD(SetIFFNeutral);
      UT_DECLARE_SCRIPT_METHOD(IsIFFUnknown);
      UT_DECLARE_SCRIPT_METHOD(IsIFFFriendly);
      UT_DECLARE_SCRIPT_METHOD(IsIFFHostile);
      UT_DECLARE_SCRIPT_METHOD(IsIFFNeutral);

      UT_DECLARE_SCRIPT_METHOD(SetManeuveringFlag);
      UT_DECLARE_SCRIPT_METHOD(GetManeuveringFlag);

      UT_DECLARE_SCRIPT_METHOD(SetQuantity);
      UT_DECLARE_SCRIPT_METHOD(GetQuantity);

      UT_DECLARE_SCRIPT_METHOD(SetJamming);
      UT_DECLARE_SCRIPT_METHOD(GetJamming);
      UT_DECLARE_SCRIPT_METHOD(SetJammingPower);
      UT_DECLARE_SCRIPT_METHOD(GetJammingPower);

      UT_DECLARE_SCRIPT_METHOD(SetTargetTruthInfo);

      UT_DECLARE_SCRIPT_METHOD(SetReportingSensorTypeEW);
      UT_DECLARE_SCRIPT_METHOD(SetReportingSensorTypeTAR);
      UT_DECLARE_SCRIPT_METHOD(SetReportingSensorTypeTTR);
      UT_DECLARE_SCRIPT_METHOD(IsReportingSensorTypeEW);
      UT_DECLARE_SCRIPT_METHOD(IsReportingSensorTypeTAR);
      UT_DECLARE_SCRIPT_METHOD(IsReportingSensorTypeTTR);
};

#endif
