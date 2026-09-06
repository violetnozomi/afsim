// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#ifndef WSF_INTERCEPT_CALLBACK_HPP
#define WSF_INTERCEPT_CALLBACK_HPP

#include "WsfBMCommon.hpp"

#include <map>

#include "UtScript.hpp"
#include "WsfSimulation.hpp"
#include "WsfPlatform.hpp"
#include "WsfGeoPoint.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptProcessor.hpp"
class    WsfWeapon;

#include <iadsLib/interceptCalculatorInterface.hpp>

class WSF_IADS_C2_LIB_EXPORT WsfInterceptCalculator : public il::InterceptCalculatorIface
{
   public:
      WsfInterceptCalculator();
      virtual ~WsfInterceptCalculator() = default;

      bool Initialize(WsfScriptProcessor* script_proc);
      // can be called on an uninitialized WsfInterceptCalculator()
      virtual bool ScriptCalculatorExists(WsfScriptProcessor* processor, WsfPlatform* owning_platform);

      bool CanIntercept(const il::trackRecord& track, const il::weaponRecord* weapon, double& interceptTime_s, double pred_pt_ecef_m[3], double& est_pk, double& weap_max_range, std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger) override;
      // we can also fold in an AFSIM-layer interface
      virtual bool CanIntercept(WsfTrack* track, WsfPlatform* owning_platform, WsfWeapon* weapon, double& interceptTime_s, double pred_pt_ecef_m[3], double& est_pk, double& weap_max_range);

   protected:
      // own-processor related contexts for intercept calculation
      WsfScriptProcessor* m_script_proc;
      UtScript*           m_script;

      WsfScriptContext& GetContext();

      // if the weapon doesn't have a launch computer then we need to see if there is a calculator deployed on the owning platform
      // we'll cache these for future invocations
      typedef std::map<il::idRecord, std::pair<WsfPlatform*, UtScript*>> tPlatformInterceptCalcCache;
      tPlatformInterceptCalcCache m_platform_calculators;

      UtScript* GetInterceptCalculatorOnPlatform(WsfPlatform* platform, bool cache = true);

      UtScript* GetInterceptCalculator(WsfScriptContext& context);
};

#endif
