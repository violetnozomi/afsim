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
*   File: WsfScriptAssetManagerClass.hpp
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
*   Abstract: This is an abstract script base class for all asset manager processor
*     script classes to inherit from. It provides necessary common functionality.
*     It's not pure abstract in the sense that we actually instantiate an object of this
*     type purely for type registration purposes with the sole intent of giving AFSIM
*     scripts the ability to up/down cast to and from the inherited types.
*
*******************************************************************************/

#ifndef WSF_SCRIPT_ASSET_MANAGER_CLASS_HPP
#define WSF_SCRIPT_ASSET_MANAGER_CLASS_HPP

#include <script/WsfScriptProcessorClass.hpp>
#include <WsfBMCommon.hpp>

class WsfScriptAssetManagerClass : public WsfScriptProcessorClass
{
   public:

      WsfScriptAssetManagerClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

      void* Create(const UtScriptContext& aContext) override;
      void* Clone(void* aObjectPtr) override;
      void  Destroy(void* aObjectPtr) override;

      static const char* SCRIPT_CLASS_NAME;
      static const char* BASE_CLASS_NAME;

      // Plugin API.
      static WSF_IADS_C2_LIB_EXPORT const char* GetScriptClassName();
      static WSF_IADS_C2_LIB_EXPORT const char* GetBaseClassName();

      UT_DECLARE_SCRIPT_METHOD(InitOwnNode);

      UT_DECLARE_SCRIPT_METHOD(GetAssetYellowTime);
      UT_DECLARE_SCRIPT_METHOD(GetAssetRedTime);
      UT_DECLARE_SCRIPT_METHOD(GetReportPositionEveryMeters);
      UT_DECLARE_SCRIPT_METHOD(GetReportPositionEverySeconds);
      UT_DECLARE_SCRIPT_METHOD(GetReportStatusEverySeconds);
      UT_DECLARE_SCRIPT_METHOD(GetAggregateUnitStatus);
      UT_DECLARE_SCRIPT_METHOD(GetStationaryOpnsOnly);
      UT_DECLARE_SCRIPT_METHOD(GetWeaponRequired);
      UT_DECLARE_SCRIPT_METHOD(GetRequireAllWeapons);
      UT_DECLARE_SCRIPT_METHOD(GetEWRequired);
      UT_DECLARE_SCRIPT_METHOD(GetTARRequired);
      UT_DECLARE_SCRIPT_METHOD(GetTTRRequired);

      UT_DECLARE_SCRIPT_METHOD(GetAssetMap);
      UT_DECLARE_SCRIPT_METHOD(GetAsset);
      UT_DECLARE_SCRIPT_METHOD(PrintStatus);
      UT_DECLARE_SCRIPT_METHOD(SetStartTime);
      UT_DECLARE_SCRIPT_METHOD(ProgressTime);
      UT_DECLARE_SCRIPT_METHOD(CheckSubordinateTimeout);
      UT_DECLARE_SCRIPT_METHOD(ProcessTrackMessage);
      UT_DECLARE_SCRIPT_METHOD(ProcessStatusMessage);
      UT_DECLARE_SCRIPT_METHOD(ProcessAssignmentStatusMessage);
      UT_DECLARE_SCRIPT_METHOD(ProcessAssignmentMessage);
      UT_DECLARE_SCRIPT_METHOD(ClearProcessedMessages);
      UT_DECLARE_SCRIPT_METHOD(RunModel);
      UT_DECLARE_SCRIPT_METHOD(CreateAssignments);
      UT_DECLARE_SCRIPT_METHOD(CANTCOAssignment);
      UT_DECLARE_SCRIPT_METHOD(MonitorAssignments);
      UT_DECLARE_SCRIPT_METHOD(ConnectLocalTrackDropCallback);
      UT_DECLARE_SCRIPT_METHOD(ConnectLocalTrackInitiatedCallback);
      UT_DECLARE_SCRIPT_METHOD(ConnectLocalTrackUpdatedCallback);
      UT_DECLARE_SCRIPT_METHOD(UpdateOwnStatus);
      UT_DECLARE_SCRIPT_METHOD(AssessOngoingAssignments);
      UT_DECLARE_SCRIPT_METHOD(ReadyCompleteAssignments);
      UT_DECLARE_SCRIPT_METHOD(CalculateTrackStrengths);
      UT_DECLARE_SCRIPT_METHOD(PrepareTracksForEvaluation);
      UT_DECLARE_SCRIPT_METHOD(UpdateDefendedAssetsArray);
      UT_DECLARE_SCRIPT_METHOD(GetAssignment);
};

#endif
