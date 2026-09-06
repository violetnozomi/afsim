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
*   File: WsfScriptBMCueMessageClass.hpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name:	Radiance Technologies
*   Address:			350 Wynn Drive
*   					   Huntsville, AL 35805
*
*   Abstract:
*
*******************************************************************************/

#ifndef WSF_SCRIPT_BM_CUE_MESSAGE_CLASS_HPP
#define WSF_SCRIPT_BM_CUE_MESSAGE_CLASS_HPP

#include <script/WsfScriptMessageClass.hpp>

class WsfScriptBMCueMessageClass : public WsfScriptMessageClass
{
   public:
      WsfScriptBMCueMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
      virtual ~WsfScriptBMCueMessageClass();

      void* Create(const UtScriptContext& aContext) override;
      void* Clone(void* aObjectPtr) override;
      void Destroy(void* aObjectPtr) override;

      static const char* SCRIPT_CLASS_NAME;
      static const char* BASE_CLASS_NAME;

      UT_DECLARE_SCRIPT_METHOD(SetCueTime);
      UT_DECLARE_SCRIPT_METHOD(GetCueTime);

      UT_DECLARE_SCRIPT_METHOD(SetReferenceTrackID);
      UT_DECLARE_SCRIPT_METHOD(GetReferenceTrackID);

      UT_DECLARE_SCRIPT_METHOD(SetMasterTrackID);
      UT_DECLARE_SCRIPT_METHOD(GetMasterTrackID);

      UT_DECLARE_SCRIPT_METHOD(SetInitiatingPlatform);
      UT_DECLARE_SCRIPT_METHOD(GetInitiatingPlatform);

      UT_DECLARE_SCRIPT_METHOD(SetCuedSensor);
      UT_DECLARE_SCRIPT_METHOD(GetCuedPlatform);
      UT_DECLARE_SCRIPT_METHOD(GetCuedSensor);

      UT_DECLARE_SCRIPT_METHOD(SetCuedReasonNewCue);
      UT_DECLARE_SCRIPT_METHOD(SetCuedReasonCancelCue);
      UT_DECLARE_SCRIPT_METHOD(IsCancelCue);

      UT_DECLARE_SCRIPT_METHOD(Print);
};

#endif
