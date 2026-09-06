// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#ifndef WSF_SCRIPT_BM_UTILS_CLASS_HPP
#define WSF_SCRIPT_BM_UTILS_CLASS_HPP

#include <WsfBMCommon.hpp>
#include <ostream>
#include <UtScriptBasicTypes.hpp>

class WSF_IADS_C2_LIB_EXPORT WsfScriptBMUtilsClass : public UtScriptClass
{
   public:
      WsfScriptBMUtilsClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
      virtual ~WsfScriptBMUtilsClass();

      void* Create(const UtScriptContext& aContext) override;

      UT_DECLARE_SCRIPT_METHOD(GetStringID);
      UT_DECLARE_SCRIPT_METHOD(GetStringFromID);
      UT_DECLARE_SCRIPT_METHOD(GetSpatialDomain);

      UT_DECLARE_SCRIPT_METHOD(InitLoggingFacilities);
      UT_DECLARE_SCRIPT_METHOD(SetInternalLoggingLevel);

      UT_DECLARE_SCRIPT_METHOD(writeln_trace);
      UT_DECLARE_SCRIPT_METHOD(writeln_debug);
      UT_DECLARE_SCRIPT_METHOD(writeln_info);
      UT_DECLARE_SCRIPT_METHOD(writeln_warn);
      UT_DECLARE_SCRIPT_METHOD(writeln_error);
      UT_DECLARE_SCRIPT_METHOD(writeln_fatal);

      UT_DECLARE_SCRIPT_METHOD(GetUpdatedTracksSinceTime);

      UT_DECLARE_SCRIPT_METHOD(ConvertTrackId);

      UT_DECLARE_SCRIPT_METHOD(SetTaskResource_Weapon);
      UT_DECLARE_SCRIPT_METHOD(SetTaskResource_Weapon_2);
      UT_DECLARE_SCRIPT_METHOD(SetTaskResource_Processor);
      UT_DECLARE_SCRIPT_METHOD(SetTaskResource_Processor_2);

      UT_DECLARE_SCRIPT_METHOD(GetMessageDestination);

      UT_DECLARE_SCRIPT_METHOD(GetUnitIDFromPlatform);
      UT_DECLARE_SCRIPT_METHOD(GetPlatformFromUnitID);

      UT_DECLARE_SCRIPT_METHOD(GetWeaponByName);
      UT_DECLARE_SCRIPT_METHOD(GetWeaponByID);
      UT_DECLARE_SCRIPT_METHOD(GetWeaponIDByName);
      UT_DECLARE_SCRIPT_METHOD(GetWeaponIDByPointer);
      UT_DECLARE_SCRIPT_METHOD(IsWeaponIDValid);

      UT_DECLARE_SCRIPT_METHOD(FindCorrelatedTrack);

      UT_DECLARE_SCRIPT_METHOD(GetConstWeaponZoneAuxData);
      UT_DECLARE_SCRIPT_METHOD(GetConstPlatformZoneAuxData);
      UT_DECLARE_SCRIPT_METHOD(SetWeaponZoneAuxData);
      UT_DECLARE_SCRIPT_METHOD(SetPlatformZoneAuxData);

      // non-projection versions of InterceptLocation2D and InterceptLocation3D
      UT_DECLARE_SCRIPT_METHOD(InterceptLocation2DNoProject_1);    // InterceptLocation2D(WsfTrack, WsfPlatform, WsfWaypoint)
      UT_DECLARE_SCRIPT_METHOD(InterceptLocation2DNoProject_2);    // InterceptLocation2D(WsfTrack, WsfPlatform, WsfWaypoint, bool)
      UT_DECLARE_SCRIPT_METHOD(InterceptLocation2DNoProject_3);    // InterceptLocation2D(WsfTrack, WsfPlatform, WsfWaypoint, double)
      UT_DECLARE_SCRIPT_METHOD(InterceptLocation3DNoProject_1);    // InterceptLocation3D(WsfTrack, WsfPlatform, WsfWaypoint)
      UT_DECLARE_SCRIPT_METHOD(InterceptLocation3DNoProject_2);    // InterceptLocation3D(WsfTrack, WsfPlatform, WsfWaypoint, bool)
      UT_DECLARE_SCRIPT_METHOD(InterceptLocation3DNoProject_3);    // InterceptLocation3D(WsfTrack, WsfPlatform, WsfWaypoint, double)

      UT_DECLARE_SCRIPT_METHOD(IsSensorEW);
      UT_DECLARE_SCRIPT_METHOD(IsSensorTAR);
      UT_DECLARE_SCRIPT_METHOD(IsSensorTTR);
      UT_DECLARE_SCRIPT_METHOD(IsSensorRWR);

      static void writeln_to_strm(std::ostream& os, UtScriptMethodArgs aVarArgs);
      static std::string get_context_string(/* const */ UtScriptContext& aContext, bool make_empty = false);
};

#endif
