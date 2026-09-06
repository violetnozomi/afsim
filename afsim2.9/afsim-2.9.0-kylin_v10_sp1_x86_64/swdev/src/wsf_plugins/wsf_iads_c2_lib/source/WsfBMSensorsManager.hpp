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
*   File: WsfBMSensorsManager.hpp
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
*   Abstract: This is a core class implementation for a HELIOS-like sensors
*    manager base class.
*
*******************************************************************************/

#ifndef WSF_BM_SENSORS_MANAGER_HPP
#define WSF_BM_SENSORS_MANAGER_HPP

namespace il
{
   class AssetManagerInterface;
   class SensorsManagerInterface;
}

#include <memory>

#include <script/WsfScriptProcessor.hpp>
#include <script/WsfScriptContext.hpp>

#include <WsfC2ComponentRefInterface.hpp>
#include <WsfScriptOverridableProcessor.hpp>
#include "WsfBMCommon.hpp"
#include <script/WsfScriptProcessor.hpp>
#include <WsfDefaultSensorsManagerImpl.hpp>
#include <WsfApplication.hpp>
#include <WsfScriptSensorsManagerClass.hpp>

class WsfBMSensorsManager : public WsfScriptProcessor, public WsfC2ComponentContainer, public WsfScriptOverridableProcessor
{
   public:

      WsfBMSensorsManager(WsfScenario&       aScenario,
                          const std::string& aPartClass = WsfScriptSensorsManagerClass::SCRIPT_CLASS_NAME,
                          const std::string& aPartVarName = "PROCESSOR",
                          const std::shared_ptr<il::SensorsManagerInterface>& sm_component = std::make_shared<il::SensorsManagerInterface>());

      virtual ~WsfBMSensorsManager();

      WsfProcessor* Clone() const override;

      // Plugin Interface
      static WSF_IADS_C2_LIB_EXPORT void RegisterScriptTypes(WsfApplication* aApplicationPtr);
      static WSF_IADS_C2_LIB_EXPORT WsfProcessor* Create(WsfScenario& aScenario);
      static WSF_IADS_C2_LIB_EXPORT std::unique_ptr<WsfBMSensorsManager> CreateUnique(WsfScenario& aScenario);
      // End Plugin Interface

      bool ProcessInput(UtInput& aInput)override;

      const char* GetScriptClassName() const override;
      virtual const char* GetBaseClassName();

      bool Initialize(double aSimTime) override;
      bool Initialize2(double aSimTime) override;
      void Update(double aSimTime) override;
      void Run(const double aSimTime) override; // HELIOS C2 component interface for invoking processor periodic updates
      bool ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;
      bool ReceiveMessage(double aSimTime, const WsfMessage& aMessage) override;

   protected:
      WsfBMSensorsManager(const WsfBMSensorsManager& aSrc);

      virtual bool ProcessSensorsManagerInput(UtInput& aInput);
      virtual void UpdateSensorCuePerception();

      virtual void SetTTRTrackingModeName(const std::string& opt_mode_name);
      virtual std::string GetTTRTrackingModeName() const;

   private:
      WsfBMSensorsManager& operator=(const WsfBMSensorsManager& rhs) = delete;;

      std::string m_ttr_tracking_mode_name;
      WsfDefaultSensorsManagerImpl m_default_sensors_manager_impl;

      friend class WsfDefaultSensorsManagerImpl;
};

#endif
