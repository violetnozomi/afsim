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
*   File: WsfWeaponsManagerAI.hpp
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

#ifndef WSF_WEAPONS_MANAGER_AI_HPP
#define WSF_WEAPONS_MANAGER_AI_HPP

#include <memory>
#include <iadsLib/weaponsManagerAI.hpp>
#include <WsfBMWeaponsManager.hpp>
#include <WsfWMAIEngagementMod.hpp>

class WsfWeaponsManagerAI : public WsfBMWeaponsManager
{
   public:

      WsfWeaponsManagerAI(WsfScenario& aScenario, const std::shared_ptr<il::weaponsManagerAI>& wm_component = std::make_shared<il::weaponsManagerAI>());
      virtual ~WsfWeaponsManagerAI() = default;

      WsfProcessor* Clone() const override;

      // Plugin Interface
      static WSF_IADS_C2_LIB_EXPORT void RegisterScriptTypes(WsfApplication* aApplicationPtr);
      static WSF_IADS_C2_LIB_EXPORT WsfProcessor* Create(WsfScenario& aScenario);
      static WSF_IADS_C2_LIB_EXPORT std::unique_ptr<WsfWeaponsManagerAI> CreateUnique(WsfScenario& aScenario);
      // End Plugin Interface

      bool ProcessInput(UtInput& aInput) override;

      const char* GetScriptClassName() const override;

      bool Initialize(double aSimTime) override;
      bool Initialize2(double aSimTime) override;
      void Update(double aSimTime) override;

   protected:
      WsfWeaponsManagerAI(const WsfWeaponsManagerAI& from);

      virtual bool ProcessWeaponsManagerAIInput(UtInput& aInput);

   private:
      std::shared_ptr<WsfWMAIEngagementMod> m_engagement_mod;

      bool ProcessEvasionSettingsBlock(UtInputBlock& block);
      bool ProcessTakeActionBlock(UtInputBlock& block);
      bool ProcessRWRResponseBlock(UtInputBlock& block);
};

#endif
