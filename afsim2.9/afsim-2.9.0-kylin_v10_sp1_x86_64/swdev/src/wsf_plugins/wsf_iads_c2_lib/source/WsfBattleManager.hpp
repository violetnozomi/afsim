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
*   File: WsfBattleManager.hpp
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
*   Abstract: This is an afsim base class for all battle manager
*     implementation classes to inherit from. It provides necessary common functionality.
*     It's not pure abstract in the sense that we actually instantiate an object of this
*     type purely for type registration purposes with the sole intent of giving AFSIM
*     scripts the ability to up/down cast to and from the inherited types.
*
*******************************************************************************/

#ifndef WSF_BATTLE_MANAGER_HPP
#define WSF_BATTLE_MANAGER_HPP

namespace il
{
   class BattleManagerInterface;
   class AssetManagerInterface;
}

#include <map>
#include <memory>

#include <script/WsfScriptProcessor.hpp>
#include <script/WsfScriptContext.hpp>
#include <UtCallbackHolder.hpp>

#include <WsfC2ComponentRefInterface.hpp>
#include <WsfScriptOverridableProcessor.hpp>
#include <WsfDefaultBattleManagerImpl.hpp>
#include "WsfBMCommon.hpp"
#include "WsfBMAssetRecord.hpp"
#include "WsfAssetMap.hpp"
#include <WsfTrack.hpp>
#include <WsfBMTerrainEngine.hpp>
#include <WsfInterceptCalc.hpp>
#include <WsfBMAssignmentMessage.hpp>
#include <WsfScriptBattleManagerClass.hpp>

WsfProcessor* UninitializedWsfBattleManager(WsfScenario& aScenario, const std::string& calling_context);

class WSF_IADS_C2_LIB_EXPORT WsfBattleManager : public WsfScriptProcessor, public WsfC2ComponentContainer, public WsfScriptOverridableProcessor
{
   public:

      WsfBattleManager(WsfScenario&       aScenario,
                       const std::string& aPartClass = WsfScriptBattleManagerClass::SCRIPT_CLASS_NAME,
                       const std::string& aPartVarName = "PROCESSOR",
                       const std::shared_ptr<il::BattleManagerInterface>& bm_component = std::make_shared<il::BattleManagerInterface>());

      virtual ~WsfBattleManager();

      virtual bool SetSimulation(WsfSimulation* sim_ptr);

      WsfProcessor* Clone() const override;

      // Plugin Interface
      static void RegisterScriptTypes(WsfApplication* aApplicationPtr);
      static WsfProcessor* Create(WsfScenario& aScenario);
      static std::unique_ptr<WsfBattleManager> CreateUnique(WsfScenario& aScenario);
      // End Plugin Interface

      bool ProcessInput(UtInput& aInput) override;

      const char* GetScriptClassName() const override;
      virtual const char* GetBaseClassName();

      // Ownership goes to caller - responsible for destroying
      virtual WsfAssetMap* GetAssetMapPtr();

      // Don't replicate all of the set interfaces, this can be handled outside of here when we deal directly with the asset map itself.

      bool Initialize(double aSimTime) override;
      bool Initialize2(double aSimTime) override;

      void Update(double aSimTime) override;
      void Run(const double aSimTime) override; // HELIOS C2 component interface for invoking processor periodic updates

      bool ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;
      bool ReceiveMessage(double aSimTime, const WsfMessage& aMessage) override;

   protected:
      WsfDefaultBattleManagerImpl m_default_bm_impl;
      std::shared_ptr<WsfBMTerrainInterface>      m_terrain_engine;
      std::shared_ptr<WsfInterceptCalculator>     m_intercept_calculator;

      // Note: Copying the battle manager doesn't make sense because it needs to be initialized
      // and that initialization is tied to a specific platform, processor, etc. However,
      // the copy constructor is required because Clone() needs it in order to properly clone the
      // object (albeit with no initialization tied to it). so we'll basically use it to get a
      // copy of a cloned object that is yet to be initialized in  order for the prototypical
      // inheritance to work.
      WsfBattleManager(const WsfBattleManager& aSrc);

      virtual void SetupPointerCrossReferences();

      virtual bool ProcessAssetManagementInput(UtInput& aInput);
      virtual bool ProcessIFFSettings(UtInput& aInput);
      virtual bool ProcessBattleManagementInput(UtInput& aInput);
      virtual bool ProcessTargetProjectionInput(UtInput& aInput);

   private:
      WsfBattleManager& operator=(const WsfBattleManager& rhs) = delete;;
};

#endif
