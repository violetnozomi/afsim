// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#include <iostream>
#include <sstream>
#include "WsfScriptBMUtilsClass.hpp"
#include "WsfBMUtils.hpp"
#include <logger.hpp>
#include <stringUtils.hpp>

#include <WsfWeapon.hpp>
#include "WsfBMAssignmentMessage.hpp"
#include <WsfBattleManager.hpp>
#include <WsfLocalTrack.hpp>
#include <WsfTrack.hpp>
#include <WsfTrackList.hpp>
#include <WsfBMAuxDataZoneAttachment.hpp>
#include <script/WsfScriptDefs.hpp>
#include <mover/WsfWaypoint.hpp>
#include <WsfIntercept.hpp>
#include <UtMemory.hpp>

#include "WsfIADSC2ScenarioExtension.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// Script Class /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WsfScriptBMUtilsClass::WsfScriptBMUtilsClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfBMUtilsClass");

   mConstructible = false;
   mCloneable = false;

   AddStaticMethod(ut::make_unique<GetStringID>());
   AddStaticMethod(ut::make_unique<GetStringFromID>());
   AddStaticMethod(ut::make_unique<GetSpatialDomain>());

   AddStaticMethod(ut::make_unique<InitLoggingFacilities>());
   AddStaticMethod(ut::make_unique<SetInternalLoggingLevel>());

   AddStaticMethod(ut::make_unique<writeln_trace>());
   AddStaticMethod(ut::make_unique<writeln_debug>());
   AddStaticMethod(ut::make_unique<writeln_info>());
   AddStaticMethod(ut::make_unique<writeln_warn>());
   AddStaticMethod(ut::make_unique<writeln_error>());
   AddStaticMethod(ut::make_unique<writeln_fatal>());

   AddStaticMethod(ut::make_unique<GetUpdatedTracksSinceTime>());

   AddStaticMethod(ut::make_unique<ConvertTrackId>());

   AddStaticMethod(ut::make_unique<GetMessageDestination>());

   AddStaticMethod(ut::make_unique<GetUnitIDFromPlatform>());
   AddStaticMethod(ut::make_unique<GetPlatformFromUnitID>());

   AddStaticMethod(ut::make_unique<GetWeaponByName>("GetWeapon"));
   AddStaticMethod(ut::make_unique<GetWeaponByID>("GetWeapon"));
   AddStaticMethod(ut::make_unique<GetWeaponIDByName>("GetWeaponID"));
   AddStaticMethod(ut::make_unique<GetWeaponIDByPointer>("GetWeaponID"));
   AddStaticMethod(ut::make_unique<IsWeaponIDValid>());

   AddStaticMethod(ut::make_unique<FindCorrelatedTrack>());

   AddStaticMethod(ut::make_unique<GetConstWeaponZoneAuxData>("GetConstZoneAuxData"));
   AddStaticMethod(ut::make_unique<GetConstPlatformZoneAuxData>("GetConstZoneAuxData"));
   AddStaticMethod(ut::make_unique<SetWeaponZoneAuxData>("SetZoneAuxData"));
   AddStaticMethod(ut::make_unique<SetPlatformZoneAuxData>("SetZoneAuxData"));

   AddStaticMethod(ut::make_unique<InterceptLocation2DNoProject_1>("InterceptLocation2DNoProject"));                // InterceptLocation2D(WsfTrack, WsfPlatform, WsfWaypoint)
   AddStaticMethod(ut::make_unique<InterceptLocation2DNoProject_2>("InterceptLocation2DNoProject"));                // InterceptLocation2D(WsfTrack, WsfPlatform, WsfWaypoint, bool)
   AddStaticMethod(ut::make_unique<InterceptLocation2DNoProject_3>("InterceptLocation2DNoProject"));                // InterceptLocation2D(WsfTrack, WsfPlatform, WsfWaypoint, double)
   AddStaticMethod(ut::make_unique<InterceptLocation3DNoProject_1>("InterceptLocation3DNoProject"));                // InterceptLocation3D(WsfTrack, WsfPlatform, WsfWaypoint)
   AddStaticMethod(ut::make_unique<InterceptLocation3DNoProject_2>("InterceptLocation3DNoProject"));                // InterceptLocation3D(WsfTrack, WsfPlatform, WsfWaypoint, bool)
   AddStaticMethod(ut::make_unique<InterceptLocation3DNoProject_3>("InterceptLocation3DNoProject"));                // InterceptLocation3D(WsfTrack, WsfPlatform, WsfWaypoint, double)

   AddStaticMethod(ut::make_unique<IsSensorEW>());
   AddStaticMethod(ut::make_unique<IsSensorTAR>());
   AddStaticMethod(ut::make_unique<IsSensorTTR>());
   AddStaticMethod(ut::make_unique<IsSensorRWR>());
}

WsfScriptBMUtilsClass::~WsfScriptBMUtilsClass()
{
}

void* WsfScriptBMUtilsClass::Create(const UtScriptContext& aContext)
{
   WsfBMUtils* ptr = new WsfBMUtils();
   return ptr;
}

void WsfScriptBMUtilsClass::writeln_to_strm(std::ostream& os, UtScriptMethodArgs varArgs)
{
   for (size_t i = 0; i < varArgs.size(); ++i)
   {
      os << varArgs[i];
   }
}

std::string WsfScriptBMUtilsClass::get_context_string(/* const */ UtScriptContext& aContext, bool make_empty)
{
   std::string context_str;

   if (!make_empty)
   {
      if (aContext.GetContextName().length())
      {
         return aContext.GetContextName() + ": ";
      }
      else if (aContext.GetParent() && aContext.GetParent()->GetContextName().size())
      {
         return aContext.GetParent()->GetContextName() + ": ";
      }
   }

   return context_str;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, GetStringID, 1, "int", "string")
{
   aReturnVal.SetInt(WsfBMUtils::GetStringId(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, GetStringFromID, 1, "string", "int")
{
   aReturnVal.SetString(WsfBMUtils::GetStringFromId(aVarArgs[0].GetInt()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, GetSpatialDomain, 1, "string", "WsfPlatform")
{
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();

   aReturnVal.SetString(WsfBMUtils::GetSpatialDomain(platformPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, SetInternalLoggingLevel, 1, "void", "string")
{
   auto level_string = aVarArgs[0].GetString();
   auto globalLogger = IADSC2ScenarioExtension::GetGlobalLogger(*WsfScriptContext::GetSCENARIO(aContext));

   if (il::equalsIgnoreCase(level_string, "trace"))
   {
      il::logger::Set_Global_Log_Level(il::logger::TRACE, globalLogger.lock().get());
   }
   else if (il::equalsIgnoreCase(level_string, "debug"))
   {
      il::logger::Set_Global_Log_Level(il::logger::DEBUG, globalLogger.lock().get());
   }
   else if (il::equalsIgnoreCase(level_string, "info"))
   {
      il::logger::Set_Global_Log_Level(il::logger::INFO, globalLogger.lock().get());
   }
   else if (il::equalsIgnoreCase(level_string, "warn"))
   {
      il::logger::Set_Global_Log_Level(il::logger::WARN, globalLogger.lock().get());
   }
   else if (il::equalsIgnoreCase(level_string, "error"))
   {
      il::logger::Set_Global_Log_Level(il::logger::ERROR, globalLogger.lock().get());
   }
   else if (il::equalsIgnoreCase(level_string, "fatal"))
   {
      il::logger::Set_Global_Log_Level(il::logger::FATAL, globalLogger.lock().get());
   }
   else if (il::equalsIgnoreCase(level_string, "off"))
   {
      il::logger::Set_Global_Log_Level(il::logger::OFF, globalLogger.lock().get());
   }
   else
   {
      il::logger::Set_Global_Log_Level(il::logger::OFF, globalLogger.lock().get());
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, InitLoggingFacilities, 1, "void", "string")
{
   auto format_specification = aVarArgs[0].GetString();
   il::logger::Init_Loggers(format_specification, IADSC2ScenarioExtension::GetGlobalLogger(*WsfScriptContext::GetSCENARIO(aContext)).lock().get());
}

UT_DEFINE_SCRIPT_METHOD_NO_ARG_CHECK(WsfScriptBMUtilsClass, WsfBMUtils, writeln_trace, -1, "void", "Object")
{
   std::stringstream strm_out;
   WsfScriptBMUtilsClass::writeln_to_strm(strm_out, aVarArgs);
   if (strm_out.str().size() > 0)
   {
      HCL_TRACE_LOGGER_NOCONTEXT(IADSC2ScenarioExtension::GetGlobalLogger(*WsfScriptContext::GetSCENARIO(aContext))) << WsfScriptBMUtilsClass::get_context_string(aContext) << strm_out.str() << std::endl;
   }

   aContext.GetContextName();
}

UT_DEFINE_SCRIPT_METHOD_NO_ARG_CHECK(WsfScriptBMUtilsClass, WsfBMUtils, writeln_debug, -1, "void", "Object")
{
   std::stringstream strm_out;
   WsfScriptBMUtilsClass::writeln_to_strm(strm_out, aVarArgs);
   if (strm_out.str().size() > 0)
   {
      HCL_DEBUG_LOGGER_NOCONTEXT(IADSC2ScenarioExtension::GetGlobalLogger(*WsfScriptContext::GetSCENARIO(aContext))) << WsfScriptBMUtilsClass::get_context_string(aContext) << strm_out.str() << std::endl;
   }

   aContext.GetContextName();
}

UT_DEFINE_SCRIPT_METHOD_NO_ARG_CHECK(WsfScriptBMUtilsClass, WsfBMUtils, writeln_info, -1, "void", "Object")
{
   std::stringstream strm_out;
   WsfScriptBMUtilsClass::writeln_to_strm(strm_out, aVarArgs);
   if (strm_out.str().size() > 0)
   {
      HCL_INFO_LOGGER_NOCONTEXT(IADSC2ScenarioExtension::GetGlobalLogger(*WsfScriptContext::GetSCENARIO(aContext))) << WsfScriptBMUtilsClass::get_context_string(aContext) << strm_out.str() << std::endl;
   }

   aContext.GetContextName();
}

UT_DEFINE_SCRIPT_METHOD_NO_ARG_CHECK(WsfScriptBMUtilsClass, WsfBMUtils, writeln_warn, -1, "void", "Object")
{
   std::stringstream strm_out;
   WsfScriptBMUtilsClass::writeln_to_strm(strm_out, aVarArgs);
   if (strm_out.str().size() > 0)
   {
      HCL_WARN_LOGGER_NOCONTEXT(IADSC2ScenarioExtension::GetGlobalLogger(*WsfScriptContext::GetSCENARIO(aContext))) << WsfScriptBMUtilsClass::get_context_string(aContext) << strm_out.str() << std::endl;
   }

   aContext.GetContextName();
}

UT_DEFINE_SCRIPT_METHOD_NO_ARG_CHECK(WsfScriptBMUtilsClass, WsfBMUtils, writeln_error, -1, "void", "Object")
{
   std::stringstream strm_out;
   WsfScriptBMUtilsClass::writeln_to_strm(strm_out, aVarArgs);
   if (strm_out.str().size() > 0)
   {
      HCL_ERROR_LOGGER_NOCONTEXT(IADSC2ScenarioExtension::GetGlobalLogger(*WsfScriptContext::GetSCENARIO(aContext))) << WsfScriptBMUtilsClass::get_context_string(aContext) << strm_out.str() << std::endl;
   }

   aContext.GetContextName();
}

UT_DEFINE_SCRIPT_METHOD_NO_ARG_CHECK(WsfScriptBMUtilsClass, WsfBMUtils, writeln_fatal, -1, "void", "Object")
{
   std::stringstream strm_out;
   WsfScriptBMUtilsClass::writeln_to_strm(strm_out, aVarArgs);

   if (strm_out.str().size() > 0)
   {
      HCL_FATAL_LOGGER_NOCONTEXT(IADSC2ScenarioExtension::GetGlobalLogger(*WsfScriptContext::GetSCENARIO(aContext))) << WsfScriptBMUtilsClass::get_context_string(aContext) << strm_out.str() << std::endl;
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, GetUpdatedTracksSinceTime, 2, "Array<WsfLocalTrack>", "WsfAssetManager, double") // TODO: check format of WsfAssetManager
{
   // Script hooks
   auto ret_tracks = ut::make_unique<std::vector<ut::script::Data>>();
   UtScriptClass* classPtr = aContext.GetTypes()->GetClass(aReturnClassPtr->GetContainerDataTypeId());

   // Parse inputs
   auto am_proc = (WsfAssetManager*)aVarArgs[0].GetPointer()->GetAppObject();
   auto current_time = aVarArgs[1].GetDouble();

   // Get only those tracks updated since the input time
   std::vector<WsfLocalTrack*> updated_tracks;
   WsfBMUtils::GetTracksUpdated(am_proc, current_time, updated_tracks);
   for (auto it = updated_tracks.cbegin(); it != updated_tracks.cend(); ++it)
   {
      const auto& track = *it;
      ret_tracks->emplace_back(new UtScriptRef(track, classPtr, UtScriptRef::cDONT_MANAGE));
   }

   // return results to the script
   aReturnVal.SetPointer(new UtScriptRef(ret_tracks.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, ConvertTrackId, 2, "WsfTrackId", "int, int")
{
   // Script hooks
   WsfTrackId* ret_id = new WsfTrackId;

   // Parse inputs
   auto owning_plat_id = aVarArgs[0].GetInt();
   auto track_id = aVarArgs[1].GetInt();

   *ret_id = WsfBMUtils::ConvertTrackId(il::idRecord(owning_plat_id, track_id));

   // return results to the script
   aReturnVal.SetPointer(new UtScriptRef(ret_id, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, GetMessageDestination, 1, "WsfPlatform", "WsfBMAssignmentMessage")
{
   // Parse inputs
   auto assignment = (WsfBMAssignmentMessage*)aVarArgs[0].GetPointer()->GetAppObject();

   WsfPlatform* dest_plat = WsfBMUtils::GetMessageDestination(SIMULATION, assignment->GetRawMessagePtr());

   // return results to the script
   aReturnVal.SetPointer(new UtScriptRef(dest_plat, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, GetUnitIDFromPlatform, 1, "int", "WsfPlatform")
{
   // Parse inputs
   auto platform = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();

   // return results to the script
   aReturnVal.SetInt(static_cast<int>(platform->GetIndex()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, GetPlatformFromUnitID, 1, "WsfPlatform", "int")
{
   // Parse inputs
   auto unit_id = aVarArgs[0].GetInt();

   WsfPlatform* platform = WsfBMUtils::GetPlatformFromUnitID(SIMULATION, unit_id);

   // return results to the script
   aReturnVal.SetPointer(new UtScriptRef(platform, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, GetWeaponByName, 2, "WsfWeapon", "WsfPlatform, string")
{
   // Parse inputs
   auto attached_plat = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   auto weapon_name = aVarArgs[1].GetString();

   // return results to the script
   aReturnVal.SetPointer(new UtScriptRef(WsfBMUtils::GetWeapon(attached_plat, weapon_name), aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, GetWeaponByID, 2, "WsfWeapon", "WsfPlatform, int")
{
   // Parse inputs
   auto attached_plat = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   auto weapon_id = aVarArgs[1].GetInt();

   // return results to the script
   //aReturnVal.SetPointer(new UtScriptRef(attached_plat->GetWeaponEntry (weapon_id), aReturnClassPtr, UtScriptRef::cDONT_MANAGE), UtScriptData::cDONT_MANAGE);
   aReturnVal.SetPointer(new UtScriptRef(attached_plat->GetComponentEntry<WsfWeapon>(weapon_id), aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, GetWeaponIDByName, 2, "int", "WsfPlatform, string")
{
   // Parse inputs
   auto attached_plat = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   auto weapon_name = aVarArgs[1].GetString();

   // return results to the script
   aReturnVal.SetInt(WsfBMUtils::GetWeaponID(attached_plat, weapon_name).getID());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, GetWeaponIDByPointer, 2, "int", "WsfPlatform, WsfWeapon")
{
   // Parse inputs
   auto attached_plat = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   auto weapon = (WsfWeapon*)aVarArgs[1].GetPointer()->GetAppObject();

   // return results to the script
   aReturnVal.SetInt(WsfBMUtils::GetWeaponID(attached_plat, weapon).getID());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, IsWeaponIDValid, 1, "bool", "int")
{
   // Parse inputs
   auto weap_idx = aVarArgs[0].GetInt();

   // return results to the script
   aReturnVal.SetBool(WsfBMUtils::IsWeaponIDValid(weap_idx));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, FindCorrelatedTrack, 2, "WsfLocalTrack", "WsfTrackManager,WsfTrackId")
{
   WsfTrackManager* mgrPtr = (WsfTrackManager*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfTrackId* idPtr = (WsfTrackId*)aVarArgs[1].GetPointer()->GetAppObject();

   aReturnVal.SetPointer(new UtScriptRef(WsfBMUtils::FindCorrelatedTrack(mgrPtr, idPtr), aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

namespace
{
template <typename T>
void GetConstZoneAttachAuxData(UtScriptMethodArgs aVarArgs,
                               UtScriptClass*     aReturnClassPtr,
                               ut::script::Data&  aReturnVal)
{
   auto* obj_ptr = (T*)aVarArgs[0].GetPointer()->GetAppObject();
   auto& aux_data_name = aVarArgs[1].GetString();

   WsfBMAuxDataZoneAttachment* aux_zone_attach = new WsfBMAuxDataZoneAttachment();
   try
   {
      if (obj_ptr->GetAuxData().AttributeExists(aux_data_name))
      {
         obj_ptr->GetAuxData().GetAttribute(aux_data_name).Get(*aux_zone_attach);
      }
   }
   catch (...) // can throw NotConvertibleException and the more generic UtException
   {
   }

   aReturnVal.SetPointer(new UtScriptRef(aux_zone_attach, aReturnClassPtr, UtScriptRef::cMANAGE));
}
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, GetConstWeaponZoneAuxData, 2, "WsfBMAuxDataZoneAttachment", "WsfWeapon,string")
{
   GetConstZoneAttachAuxData<WsfWeapon>(aVarArgs, aReturnClassPtr, aReturnVal);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, GetConstPlatformZoneAuxData, 2, "WsfBMAuxDataZoneAttachment", "WsfPlatform,string")
{
   GetConstZoneAttachAuxData<WsfPlatform>(aVarArgs, aReturnClassPtr, aReturnVal);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, SetWeaponZoneAuxData, 3, "void", "WsfWeapon, string, WsfBMAuxDataZoneAttachment")
{
   auto* wpn_ptr = (WsfWeapon*)aVarArgs[0].GetPointer()->GetAppObject();
   auto& aux_data_name = aVarArgs[1].GetString();
   auto* zone_attach_ptr = (WsfBMAuxDataZoneAttachment*)aVarArgs[2].GetPointer()->GetAppObject();

   wpn_ptr->GetAuxData().Set(aux_data_name, *zone_attach_ptr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, SetPlatformZoneAuxData, 3, "void", "WsfPlatform, string, WsfBMAuxDataZoneAttachment")
{
   auto* plat_ptr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   auto& aux_data_name = aVarArgs[1].GetString();
   auto* zone_attach_ptr = (WsfBMAuxDataZoneAttachment*)aVarArgs[2].GetPointer()->GetAppObject();

   plat_ptr->GetAuxData().Set(aux_data_name, *zone_attach_ptr);
}

// Helper function lifted from AFSIM but fixed up to not perform track projections
namespace
{
std::ostream& operator<<(std::ostream& aOut, const double aVec[])
{
   aOut << ' ' << aVec[0] << ' ' << aVec[1] << ' ' << aVec[2];
   return aOut;
}

//! Helps calculate intercept bearing and altitude (if dimensions >= 3)
//! Assumes constant velocity for both target and ownship
//! Differs in the AFSIM implementation in that it does not forward-project the track
double InterceptHelperNoProject(double       aSimTime,
                                WsfPlatform* aOwnshipPtr,
                                double       aOwnshipSpeed,
                                WsfTrack*    aTrackPtr,
                                WsfWaypoint* aWaypointPtr,
                                int          aDimensions,
                                bool         aDebugEnabled)
{
   double timeToIntercept = -1.0;
   if ((aTrackPtr != 0) && (aWaypointPtr != 0))
   {
      // Get the ownship location in WCS
      double ownLocWCS[3];
      aOwnshipPtr->GetLocationWCS(ownLocWCS);

      // Get target location in WCS
      double tgtLocWCS[3];
      aTrackPtr->GetLocationWCS(tgtLocWCS);

      // Get target velocity in WCS
      double tgtVelWCS[3];
      aTrackPtr->GetVelocityWCS(tgtVelWCS);

      // Compute the intercept time and location.
      double intLocWCS[3];
      timeToIntercept = WsfIntercept::Intercept(ownLocWCS, aOwnshipSpeed, tgtLocWCS, tgtVelWCS, intLocWCS);

      // Convert the intercept location to spherical coordinates and stuff in the waypoint.
      double intLat;
      double intLon;
      double intAlt;
      WsfPlatform::ConvertWCSToLLA(intLocWCS, intLat, intLon, intAlt);
      aWaypointPtr->SetLat(intLat);
      aWaypointPtr->SetLon(intLon);

      // Also compute the 2D heading to the intercept point as some find it useful.
      double intLocNED[3];
      aOwnshipPtr->ConvertWCSToNED(intLocWCS, intLocNED);
      aWaypointPtr->SetHeading(atan2(intLocNED[1], intLocNED[0]));

      // Set the 'extrapolate' option in the waypoint so if it the last point in the mover then
      // it will continue at the current heading.
      aWaypointPtr->SetEndOfPathOption(WsfPath::cEP_EXTRAPOLATE);

      // For 2D we just let it continue at the current altitude.
      // For 3D we use the intercept altitude IF the track has a valid 3D location or elevation data.
      if (aDimensions >= 3)
      {
         if (aTrackPtr->ElevationValid() ||
             aTrackPtr->LocationValid())
         {
            double tgtLat;
            double tgtLon;
            double tgtAlt;
            WsfPlatform::ConvertWCSToLLA(intLocWCS, tgtLat, tgtLon, tgtAlt);
            aWaypointPtr->SetAlt(tgtAlt);
         }
      }

      // Print debug information.
      if (aDebugEnabled)
      {
         std::cout << "=== T=" << aSimTime << "; Intercept Computatation ===" << std::endl;

         // Print ownship information
         double ownLocLLA[3];
         aOwnshipPtr->GetLocationLLA(ownLocLLA[0], ownLocLLA[1], ownLocLLA[2]);
         double ownVelNED[3];
         aOwnshipPtr->GetVelocityNED(ownVelNED);
         double ownHeading = ::atan2(ownVelNED[1], ownVelNED[0]);
         std::cout << " == Interceptor = " << aOwnshipPtr->GetName() << '\n'
                   << "    Location    LLA [deg,m]=" << ownLocLLA << '\n'
                   << "    Velocity    NED [m/s]  =" << ownVelNED << '\n'
                   << "    Speed           [m/s]  = " << UtVec3d::Magnitude(ownVelNED)
                   << "; Requested = " << aOwnshipSpeed << '\n'
                   << "    Heading         [deg]  = " << UtMath::NormalizeAngle0_360(ownHeading * UtMath::cDEG_PER_RAD) << std::endl;

         // Print target information.
         double tgtLocLLA[3];
         WsfPlatform::ConvertWCSToLLA(tgtLocWCS, tgtLocLLA[0], tgtLocLLA[1], tgtLocLLA[2]);
         double tgtLocNED[3];
         aOwnshipPtr->ConvertWCSToNED(tgtLocWCS, tgtLocNED);
         double tgtVelNED[3];
         aOwnshipPtr->ConvertWCSVectorToNED(tgtVelNED, tgtVelWCS);
         double tgtHeading = ::atan2(tgtVelNED[1], tgtVelNED[0]);
         std::cout << " == Target = " << aTrackPtr->GetTargetName() << '\n'
                   << "    Location    LLA [deg,m]=" << tgtLocLLA << '\n'
                   << "    Location    NED [m]    =" << tgtLocNED << '\n'
                   << "    Velocity    NED [m/s]  =" << tgtVelNED << '\n'
                   << "    Speed           [m/s]  = " << UtVec3d::Magnitude(tgtVelWCS) << '\n'
                   << "    Heading         [deg]  = " << UtMath::NormalizeAngle0_360(tgtHeading * UtMath::cDEG_PER_RAD) << std::endl;

         if (timeToIntercept >= 0)
         {
            double intLocLLA[3];
            WsfPlatform::ConvertWCSToLLA(intLocWCS, intLocLLA[0], intLocLLA[1], intLocLLA[2]);
            std::cout << " == Intercept = " << timeToIntercept
                      << " (at T=" << aSimTime + timeToIntercept << ")\n"
                      << "    Location    LLA [deg,m]=" << intLocLLA << '\n'
                      << "    Location    NED [m]    =" << intLocNED << std::endl;
         }
         else
         {
            std::cout << " == No Intercept" << std::endl;
         }
      }
   }
   return timeToIntercept;
}
}

//! Returns time to intercept and intercept bearing
//! Assumes constant velocity for both target and ownship
UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, InterceptLocation2DNoProject_1, 3, "double", "WsfTrack, WsfPlatform, WsfWaypoint")
{
   // Argument 1: Target track <input>
   // Argument 2: Weapon platform <input>
   // Argument 3: Intercept waypoint <output>

   // Extract the function arguments
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfPlatform* platPtr = (WsfPlatform*)aVarArgs[1].GetPointer()->GetAppObject();
   WsfWaypoint* waypointPtr = (WsfWaypoint*)aVarArgs[2].GetPointer()->GetAppObject();

   // Ensure source platform position is current
   double simTime = TIME_NOW;
   platPtr->Update(simTime);

   double timeToIntercept;
   timeToIntercept = InterceptHelperNoProject(simTime, platPtr, platPtr->GetSpeed(), trackPtr, waypointPtr, 2, false);
   aReturnVal.SetDouble(timeToIntercept);
}

//! Returns time to intercept and intercept bearing
//! Assumes constant velocity for both target and ownship
UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, InterceptLocation2DNoProject_2, 4, "double", "WsfTrack, WsfPlatform, WsfWaypoint, bool")
{
   // Argument 1: Target track <input>
   // Argument 2: Weapon platform <input>
   // Argument 3: Intercept waypoint <output>
   // Argument 4: Debug flag

   // Extract the function arguments
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfPlatform* platPtr = (WsfPlatform*)aVarArgs[1].GetPointer()->GetAppObject();
   WsfWaypoint* waypointPtr = (WsfWaypoint*)aVarArgs[2].GetPointer()->GetAppObject();
   bool debugEnabled = aVarArgs[3].GetBool();

   // Ensure source platform position is current
   double simTime = TIME_NOW;
   platPtr->Update(simTime);

   double timeToIntercept;
   timeToIntercept = InterceptHelperNoProject(simTime, platPtr, platPtr->GetSpeed(), trackPtr, waypointPtr, 2, debugEnabled);
   aReturnVal.SetDouble(timeToIntercept);
}

//! Returns time to intercept and intercept bearing
//! Assumes constant velocity for both target and ownship
UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, InterceptLocation2DNoProject_3, 4, "double", "WsfTrack, WsfPlatform, WsfWaypoint, double")
{
   // Argument 1: Target track <input>
   // Argument 2: Weapon platform <input>
   // Argument 3: Intercept waypoint <output>
   // Argument 4: Ownship speed

   // Extract the function arguments
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfPlatform* platPtr = (WsfPlatform*)aVarArgs[1].GetPointer()->GetAppObject();
   WsfWaypoint* waypointPtr = (WsfWaypoint*)aVarArgs[2].GetPointer()->GetAppObject();
   double speedOfOwnship = aVarArgs[3].GetDouble();

   // Ensure source platform position is current
   double simTime = TIME_NOW;
   platPtr->Update(simTime);

   double timeToIntercept;
   timeToIntercept = InterceptHelperNoProject(simTime, platPtr, speedOfOwnship, trackPtr, waypointPtr, 2, false);
   aReturnVal.SetDouble(timeToIntercept);
}

//! Returns time to intercept and intercept bearing and altitude
//! Assumes constant velocity for both target and ownship
UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, InterceptLocation3DNoProject_1, 3, "double", "WsfTrack, WsfPlatform, WsfWaypoint")
{
   // Argument 1: Target track <input>
   // Argument 2: Weapon platform <input>
   // Argument 3: Intercept waypoint <output>

   // Extract the function arguments
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfPlatform* platPtr = (WsfPlatform*)aVarArgs[1].GetPointer()->GetAppObject();
   WsfWaypoint* waypointPtr = (WsfWaypoint*)aVarArgs[2].GetPointer()->GetAppObject();

   // Ensure source platform position is current
   double simTime = TIME_NOW;
   platPtr->Update(simTime);

   double timeToIntercept;
   timeToIntercept = InterceptHelperNoProject(simTime, platPtr, platPtr->GetSpeed(), trackPtr, waypointPtr, 3, false);
   aReturnVal.SetDouble(timeToIntercept);
}

//! Returns time to intercept and intercept bearing and altitude
//! Assumes constant velocity for both target and ownship
UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, InterceptLocation3DNoProject_2, 4, "double", "WsfTrack, WsfPlatform, WsfWaypoint, bool")
{
   // Argument 1: Target track <input>
   // Argument 2: Weapon platform <input>
   // Argument 3: Intercept waypoint <output>
   // Argument 4: Debug flag

   // Extract the function arguments
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfPlatform* platPtr = (WsfPlatform*)aVarArgs[1].GetPointer()->GetAppObject();
   WsfWaypoint* waypointPtr = (WsfWaypoint*)aVarArgs[2].GetPointer()->GetAppObject();
   bool debugEnabled = aVarArgs[3].GetBool();

   // Ensure source platform position is current
   double simTime = TIME_NOW;
   platPtr->Update(simTime);

   double timeToIntercept;
   timeToIntercept = InterceptHelperNoProject(simTime, platPtr, platPtr->GetSpeed(), trackPtr, waypointPtr, 3, debugEnabled);
   aReturnVal.SetDouble(timeToIntercept);
}

//! Returns time to intercept and intercept bearing and altitude
//! Assumes constant velocity for both target and ownship
UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, InterceptLocation3DNoProject_3, 4, "double", "WsfTrack, WsfPlatform, WsfWaypoint, double")
{
   // Argument 1: Target track <input>
   // Argument 2: Weapon platform <input>
   // Argument 3: Intercept waypoint <output>
   // Argument 4: Ownship speed

   // Extract the function arguments
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfPlatform* platPtr = (WsfPlatform*)aVarArgs[1].GetPointer()->GetAppObject();
   WsfWaypoint* waypointPtr = (WsfWaypoint*)aVarArgs[2].GetPointer()->GetAppObject();
   double speedOfOwnship = aVarArgs[3].GetDouble();

   // Ensure source platform position is current
   double simTime = TIME_NOW;
   platPtr->Update(simTime);

   double timeToIntercept;
   timeToIntercept = InterceptHelperNoProject(simTime, platPtr, speedOfOwnship, trackPtr, waypointPtr, 3, false);
   aReturnVal.SetDouble(timeToIntercept);
}


UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, IsSensorEW, 1, "bool", "WsfSensor")
{
   WsfSensor* sensorPtr = (WsfSensor*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetBool(WsfBMUtils::GetSensorType(*sensorPtr) == il::SENSOR_EW);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, IsSensorTAR, 1, "bool", "WsfSensor")
{
   WsfSensor* sensorPtr = (WsfSensor*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetBool(WsfBMUtils::GetSensorType(*sensorPtr) == il::SENSOR_TAR);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, IsSensorTTR, 1, "bool", "WsfSensor")
{
   WsfSensor* sensorPtr = (WsfSensor*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetBool(WsfBMUtils::GetSensorType(*sensorPtr) == il::SENSOR_TTR);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMUtilsClass, WsfBMUtils, IsSensorRWR, 1, "bool", "WsfSensor")
{
   WsfSensor* sensorPtr = (WsfSensor*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetBool(WsfBMUtils::GetSensorType(*sensorPtr) == il::SENSOR_RWR);
}
