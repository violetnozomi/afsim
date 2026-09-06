// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#ifndef WSF_SCRIPT_BM_ASSET_RECORD_CLASS_HPP
#define WSF_SCRIPT_BM_ASSET_RECORD_CLASS_HPP

#include <WsfBMCommon.hpp>
#include <UtScriptBasicTypes.hpp>
#include <iadsLib/enum.hpp>
#include <UtMemory.hpp>

class WsfBMAssetRecord;
class WsfSimulation;

class WSF_IADS_C2_LIB_EXPORT WsfScriptBMAssetRecordClass : public UtScriptClass
{
   public:
      WsfScriptBMAssetRecordClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
      virtual ~WsfScriptBMAssetRecordClass();

      void* Create(const UtScriptContext& aContext) override;
      void* Clone(void* aObjectPtr) override;
      void Destroy(void* aObjectPtr) override;

      UT_DECLARE_SCRIPT_METHOD(InitNode);
      UT_DECLARE_SCRIPT_METHOD(AddDirectSubordinate);
      UT_DECLARE_SCRIPT_METHOD(AddPeer);
      UT_DECLARE_SCRIPT_METHOD(AddDirectCommander);

      UT_DECLARE_SCRIPT_METHOD(AddWeaponInfo);

      UT_DECLARE_SCRIPT_METHOD(SetLocationECEF);
      UT_DECLARE_SCRIPT_METHOD(SetVelocityECEF);
      UT_DECLARE_SCRIPT_METHOD(SetDefended);
      UT_DECLARE_SCRIPT_METHOD(AttachDA);

      UT_DECLARE_SCRIPT_METHOD(SetMaxPlatformAssignments);
      UT_DECLARE_SCRIPT_METHOD(SetCurrPlatformAssignments);
      UT_DECLARE_SCRIPT_METHOD(SetPlatformAssignmentDelay);

      UT_DECLARE_SCRIPT_METHOD(SetStatusAgeouts);

      UT_DECLARE_SCRIPT_METHOD(SetSystemStatusRed);
      UT_DECLARE_SCRIPT_METHOD(SetSystemStatusGreen);
      UT_DECLARE_SCRIPT_METHOD(SetSystemStatusWhite);
      UT_DECLARE_SCRIPT_METHOD(SetSystemStatusYellow);
      UT_DECLARE_SCRIPT_METHOD(SetExternallyDefinedSystemStatus);

      UT_DECLARE_SCRIPT_METHOD(GetID);

      UT_DECLARE_SCRIPT_METHOD(SetC2Capable);
      UT_DECLARE_SCRIPT_METHOD(GetC2Capable);

      UT_DECLARE_SCRIPT_METHOD(SetDefaultDefendedPriority);

   public:
      static void AttachUnitZone(WsfBMAssetRecord* aObjectPtr, void* wsf_zone_obj, il::tZonePriority priority, il::eZoneType type, WsfSimulation* sim_ptr);
};

#endif
