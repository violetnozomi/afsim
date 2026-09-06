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
#include <logger.hpp>
#include "WsfInterceptCalc.hpp"
#include <WsfBMUtils.hpp>
#include <WsfTrackList.hpp>
#include <WsfBMInterceptCalcResult.hpp>
#include <WsfWeapon.hpp>

#include "WsfIADSC2ScenarioExtension.hpp"

// TTD: Write Copy constructor and assignment operator!
WsfInterceptCalculator::WsfInterceptCalculator()
   : m_script_proc(nullptr)
   , m_script(nullptr)
{
}

bool WsfInterceptCalculator::Initialize(WsfScriptProcessor* script_proc)
{
   bool success = true;

   auto globalLogger = IADSC2ScenarioExtension::GetGlobalLogger(script_proc->GetScenario());

   m_script_proc = script_proc;
   m_script = GetInterceptCalculator(m_script_proc->GetScriptContext());
   if (!m_script)
   {
      HCL_WARN_LOGGER(globalLogger) << "WsfInterceptCalculator::Initialize(): processor does not have a can_intercept function...BM will rely on a can_intercept function being present on each weapon platform...";
   }
   else
   {
      HCL_DEBUG_LOGGER(globalLogger) << "Processor context is tied to = " << WsfScriptContext::GetPROCESSOR(m_script_proc->GetScriptContext().GetContext())->GetName();
   }

   HCL_DEBUG_LOGGER(globalLogger) << "WsfInterceptCalculator::Initialize() invoked..." << success;

   return success;
}

bool WsfInterceptCalculator::ScriptCalculatorExists(WsfScriptProcessor* processor, WsfPlatform* owning_platform)
{
   return (processor && GetInterceptCalculator(processor->GetScriptContext())) || (owning_platform && GetInterceptCalculatorOnPlatform(owning_platform, false));
}

bool WsfInterceptCalculator::CanIntercept(const il::trackRecord& track, const il::weaponRecord* weapon, double& interceptTime_s, double pred_pt_ecef_m[3], double& est_pk, double& weap_max_range, std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger)
{
   // get the weapon from the reference
   auto sim_ptr = WsfScriptContext::GetSIMULATION(GetContext().GetContext());
   WsfPlatform* owning_platform = sim_ptr->GetPlatformByIndex(weapon->GetParentAssetID().getID());

   if (!owning_platform)
   {
      HCL_WARN_LOGGER(aGlobalLogger) << "WsfInterceptCalculator::CanIntercept(): could not locate platform: " << weapon->GetParentAssetID().getID() << ". Cannot determine intercept.";
      return false;
   }

   WsfWeapon* wpn = owning_platform->GetComponentEntry<WsfWeapon>(weapon->GetWeaponID().getID());

   if (!wpn)
   {
      HCL_WARN_LOGGER(aGlobalLogger) << "WsfInterceptCalculator::CanIntercept(): could not locate weapon: " << weapon->GetWeaponID().getID() << ". Cannot determine intercept.";
      return false;
   }

   // create the track id
   WsfTrackId track_id(WsfBMUtils::ConvertTrackId(track.getID()));

   // find the master track entry to get the latest data
   auto found_track = m_script_proc->GetPlatform()->GetMasterTrackList().FindTrack(track_id);
   if (!found_track)
   {
      HCL_WARN_LOGGER(aGlobalLogger) << "WsfInterceptCalculator::CanIntercept(): could not locate track id: " << track_id << " in master track list. Cannot determine intercept.";
      return false;
   }

   // clone the track because we need to set the location to our incoming projected track
   std::unique_ptr<WsfTrack> track_ptr(found_track->Clone());
   auto& position = track.getPosition();
   track_ptr->SetLocationLLA(position.getLatDegs(), position.getLonDegs(), position.getAltM());
   track_ptr->SetUpdateTime(track.getTime());
   HCL_TRACE_LOGGER(aGlobalLogger) << "CanIntercept(): curr time = " << sim_ptr->GetSimTime() << ", track time = " << track_ptr->GetUpdateTime() << ", position = " << position.getLatDegs() << "/" << position.getLonDegs() << "/" << position.getAltM();

   return CanIntercept(track_ptr.get(), owning_platform, wpn, interceptTime_s, pred_pt_ecef_m, est_pk, weap_max_range);
}

bool WsfInterceptCalculator::CanIntercept(WsfTrack* track, WsfPlatform* owning_platform, WsfWeapon* weapon, double& interceptTime_s, double pred_pt_ecef_m[3], double& est_pk, double& weap_max_range)
{
   bool can_intercept = false;

   auto sim_ptr = WsfScriptContext::GetSIMULATION(GetContext().GetContext());

   // predicted point return value
   WsfGeoPoint pred_position;

   // initialize intercept time
   interceptTime_s = -1;

   ut::script::Data retVal;
   ut::script::DataList argList;

   WsfGeoPoint pred_pt;

   if (m_script)
   {
      static UtScriptClass* wpnClassPtr = GetContext().GetClass("WsfWeapon");
      assert(wpnClassPtr != 0);

      static UtScriptClass* trkClassPtr = GetContext().GetClass("WsfTrack");
      assert(trkClassPtr != 0);

      argList.emplace_back(new UtScriptRef(weapon, wpnClassPtr));
      argList.emplace_back(new UtScriptRef(track, trkClassPtr));

      GetContext().ExecuteScript(sim_ptr->GetSimTime(), m_script, retVal, argList);
      WsfBMInterceptCalcResult* result = reinterpret_cast<WsfBMInterceptCalcResult*>(retVal.GetPointer()->GetAppObject());
      // copy reults
      can_intercept = result->GetCanIntercept();
      interceptTime_s = result->GetTTI();
      pred_pt = result->GetPredPoint();
      pred_pt.GetLocationWCS(pred_pt_ecef_m);
      est_pk = result->GetEstimatedPK();
      weap_max_range = result->GetWeaponMaxRange();
   }
   else
   {
      HCL_TRACE_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(sim_ptr->GetScenario())) << "No intercept calculator found on battle manager processor, checking owning platform of weapon...";

      // check the platform that owns the weapon to see if it has an intercept calculator script deployed on the platform
      auto script = GetInterceptCalculatorOnPlatform(owning_platform);
      if (script)
      {
         UtScriptClass* wpnClassPtr = owning_platform->GetScriptContext().GetClass("WsfWeapon");
         assert(wpnClassPtr != 0);

         UtScriptClass* trkClassPtr = owning_platform->GetScriptContext().GetClass("WsfTrack");
         assert(trkClassPtr != 0);

         argList.emplace_back(new UtScriptRef(weapon, wpnClassPtr));
         argList.emplace_back(new UtScriptRef(track, trkClassPtr));

         owning_platform->GetScriptContext().ExecuteScript(sim_ptr->GetSimTime(), script, retVal, argList);
         WsfBMInterceptCalcResult* result = reinterpret_cast<WsfBMInterceptCalcResult*>(retVal.GetPointer()->GetAppObject());
         // copy reults
         can_intercept = result->GetCanIntercept();
         interceptTime_s = result->GetTTI();
         pred_pt = result->GetPredPoint();
         pred_pt.GetLocationWCS(pred_pt_ecef_m);
         est_pk = result->GetEstimatedPK();
         weap_max_range = result->GetWeaponMaxRange();
      }
      else
      {
         HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(sim_ptr->GetScenario())) << "Could not find intercept calculator on battle manager ("
                            << (m_script_proc->GetPlatform() ? m_script_proc->GetPlatform()->GetName() : "**error unknown processor platform**")
                            << ") or weapon owning platform ("
                            << (owning_platform ? owning_platform->GetName() : "**error unknown owning_platform**")
                            << ") ...cannot assess threat";
      }
   }

   double track_lla[3] = { 0 };
   track->GetLocationLLA(track_lla[0], track_lla[1], track_lla[2]);
   HCL_TRACE_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(sim_ptr->GetScenario())) << std::setprecision(4)
                      << "Invoked can_intercept, pred pt LLA = " << pred_pt.GetLat() << "," << pred_pt.GetLon() << "," << pred_pt.GetAlt()
                      << std::setprecision(2)
                      << ", TTI = " << interceptTime_s
                      << ", can intercept? " << can_intercept
                      << std::setprecision(4)
                      << ", track location: " << track_lla[0] << "," << track_lla[1] << "," << track_lla[2] << ", speed = " << track->GetSpeed()
                      << std::setprecision(2);

   return can_intercept;
}

WsfScriptContext& WsfInterceptCalculator::GetContext()
{
   return m_script_proc->GetScriptContext();
}

UtScript* WsfInterceptCalculator::GetInterceptCalculator(WsfScriptContext& context)
{
   auto script = context.FindScript("can_intercept");
   if (script)
   {
      if (!context.ValidateScript(script, "WsfBMInterceptCalcResult", "WsfWeapon, WsfTrack"))
      {
         script = nullptr;
      }
   }

   return script;
}

UtScript* WsfInterceptCalculator::GetInterceptCalculatorOnPlatform(WsfPlatform* platform, bool cache)
{
   UtScript* script = nullptr;

   if (!platform)
   {
      return script;
   }

   auto entry = m_platform_calculators.find(static_cast<int32_t>(platform->GetIndex()));
   if (entry == m_platform_calculators.end())
   {
      // no entry means we haven't checked this platform yet, go ahead and see if the script exists
      script = GetInterceptCalculator(platform->GetScriptContext());
      if (cache)
      {
         if (script)
         {
            m_platform_calculators[static_cast<int32_t>(platform->GetIndex())] = std::make_pair(platform, script);
         }
         else
         {
            m_platform_calculators[static_cast<int32_t>(platform->GetIndex())] = std::make_pair(nullptr, nullptr);
         }
      }
   }
   else
   {
      script = entry->second.second;
   }

   // caller must still check that script is not nullptr
   return script;
}
