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
*   File: WsfBMWeaponsManager.hpp
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
*   Abstract: This is a core class implementation for a HELIOS-like weapons
*    manager base class.
*
*******************************************************************************/

#ifndef WSF_BM_WEAPONS_MANAGER_HPP
#define WSF_BM_WEAPONS_MANAGER_HPP

namespace il
{
   class AssetManagerInterface;
   class WeaponsManagerInterface;
}

#include <memory>

#include <script/WsfScriptProcessor.hpp>
#include <script/WsfScriptContext.hpp>
#include <WsfApplication.hpp>

#include <WsfC2ComponentRefInterface.hpp>
#include <WsfScriptOverridableProcessor.hpp>
#include "WsfBMCommon.hpp"
#include <script/WsfScriptProcessor.hpp>
#include <WsfDefaultWeaponsManagerImpl.hpp>
#include <WsfScriptWeaponsManagerClass.hpp>

class WSF_IADS_C2_LIB_EXPORT WsfBMWeaponsManager : public WsfScriptProcessor, public WsfC2ComponentContainer, public WsfScriptOverridableProcessor
{
   public:

      WsfBMWeaponsManager(WsfScenario&       aScenario,
                          const std::string& aPartClass = WsfScriptWeaponsManagerClass::SCRIPT_CLASS_NAME,
                          const std::string& aPartVarName = "PROCESSOR",
                          const std::shared_ptr<il::WeaponsManagerInterface>& wm_component = std::make_shared<il::WeaponsManagerInterface>());
      virtual ~WsfBMWeaponsManager();

      WsfProcessor* Clone() const override;

      // Plugin Interface
      static void RegisterScriptTypes(WsfApplication* aApplicationPtr);
      static WsfProcessor* Create(WsfScenario& aScenario);
      static std::unique_ptr<WsfBMWeaponsManager> CreateUnique(WsfScenario& aScenario);
      // End Plugin Interface

      bool ProcessInput(UtInput& aInput) override;

      const char* GetScriptClassName() const override;
      virtual const char* GetBaseClassName();

      bool Initialize(double aSimTime) override;
      bool Initialize2(double aSimTime) override;
      void Update(double aSimTime) override;
      void Run(const double aSimTime) override; // HELIOS C2 component interface for invoking processor periodic updates
      bool ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;
      bool ReceiveMessage(double aSimTime, const WsfMessage& aMessage) override;

   protected:
      WsfBMWeaponsManager(const WsfBMWeaponsManager& aSrc);

      virtual bool ProcessWeaponsManagerInput(UtInput& aInput);

   private:
      WsfBMWeaponsManager& operator=(const WsfBMWeaponsManager& rhs) = delete;

      WsfDefaultWeaponsManagerImpl m_default_weapons_manager_impl;

      bool ProcessSelfDefenseBlock(UtInputBlock& block);
      bool ProcessEngagementSettingsBlock(UtInputBlock& block);
      bool ProcessDelaysBlock(UtInputBlock& block);
      bool ProcessWEZBlock(UtInputBlock& block);
};

#endif
