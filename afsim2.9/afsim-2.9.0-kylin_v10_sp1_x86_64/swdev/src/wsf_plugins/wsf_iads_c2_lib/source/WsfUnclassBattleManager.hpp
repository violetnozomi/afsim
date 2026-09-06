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
*   File: WsfUnclassBattleManager.hpp
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

#ifndef WSF_UNCLASS_BATTLE_MANAGER_HPP
#define WSF_UNCLASS_BATTLE_MANAGER_HPP

#include <memory>

#include <WsfBattleManager.hpp>
#include <WsfInterceptCalc.hpp>

#include <WsfBMTerrainEngine.hpp>
#include <iadsLib/unclassifiedBattleManager.hpp>

WsfProcessor* UninitializedWsfUnclassifiedBattleManager(WsfScenario& aScenario, const std::string& calling_context);

class WsfUnclassifiedBattleManager : public WsfBattleManager
{
   public:
      WsfUnclassifiedBattleManager(WsfScenario& aScenario);
      WsfUnclassifiedBattleManager(WsfScenario& aScenario, const std::shared_ptr<il::BattleManagerInterface>& bm);
      WsfProcessor* Clone() const override;

      // Plugin Interface
      static WSF_IADS_C2_LIB_EXPORT void RegisterScriptTypes(WsfApplication* aApplicationPtr);
      static WSF_IADS_C2_LIB_EXPORT WsfProcessor* Create(WsfScenario& aScenario);
      static WSF_IADS_C2_LIB_EXPORT std::unique_ptr<WsfUnclassifiedBattleManager> CreateUnique(WsfScenario& aScenario);
      // End Plugin Interface

      bool ProcessInput(UtInput& aInput)  override;

      const char* GetScriptClassName() const override;

   protected:
      // Note: Copying the battle manager doesn't make sense becuase it needs to be initialized
      // and that initialization is tied to a specific platform, processor, etc. However,
      // the copy constructor is required because Clone() needs it in order to properly clone the
      // object (albeit with no initialization tied to it). so we'll basically use it to get a
      // copy of a cloned object that is yet to be initialized in  order for the prototypical
      // inheritance to work.
      WsfUnclassifiedBattleManager(const WsfUnclassifiedBattleManager& from);

      virtual bool ProcessCoreUnclassBMInput(UtInput& aInput);
      virtual bool ProcessPrintSettings(UtInput& aInput);
      virtual bool ProcessWeaponTable(UtInput& aInput);
      virtual bool ProcessScoringfactors(UtInput& aInput);

   private:
      WsfUnclassifiedBattleManager& operator=(const WsfUnclassifiedBattleManager& rhs) = delete;
};

#endif
