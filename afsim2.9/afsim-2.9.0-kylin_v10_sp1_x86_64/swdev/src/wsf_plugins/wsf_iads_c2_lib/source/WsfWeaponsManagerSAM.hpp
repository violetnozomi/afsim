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
*   File: WsfWeaponsManagerSAM.hpp
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

#ifndef WSF_WEAPONS_MANAGER_SAM_HPP
#define WSF_WEAPONS_MANAGER_SAM_HPP

#include <memory>

#include <WsfBMWeaponsManager.hpp>
#include <WsfWMSAMEngagementMod.hpp>
#include <iadsLib/weaponsManagerSAM.hpp>

class WsfWeaponsManagerSAM : public WsfBMWeaponsManager
{
   public:

      WsfWeaponsManagerSAM(WsfScenario& aScenario, const std::shared_ptr<il::WeaponsManagerInterface>& wm_component = std::make_shared<il::weaponsManagerSAM>());
      WsfProcessor* Clone() const override;

      // Plugin Interface
      static WSF_IADS_C2_LIB_EXPORT void RegisterScriptTypes(WsfApplication* aApplicationPtr);
      static WSF_IADS_C2_LIB_EXPORT WsfProcessor* Create(WsfScenario& aScenario);
      static WSF_IADS_C2_LIB_EXPORT std::unique_ptr<WsfWeaponsManagerSAM> CreateUnique(WsfScenario& aScenario);
      // End Plugin Interface

      bool ProcessInput(UtInput& aInput) override;

      const char* GetScriptClassName() const override;

      bool Initialize(double aSimTime) override;
      bool Initialize2(double aSimTime) override;
      void Update(double aSimTime)  override;

      void SystemicCANTCOAssignmentsForWeapon(WsfStringId name);

   protected:
      WsfWeaponsManagerSAM(const WsfWeaponsManagerSAM& from);

   private:
      std::shared_ptr<WsfWMSAMEngagementMod> m_engagement_mod;
};

#endif
