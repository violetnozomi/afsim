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
*   File: WsfUnclassAssetManager.hpp
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

#ifndef WSF_UNCLASS_ASSET_MANAGER_HPP
#define WSF_UNCLASS_ASSET_MANAGER_HPP

#include <memory>
#include <WsfAssetManager.hpp>

class WsfUnclassifiedAssetManager : public WsfAssetManager
{
   public:
      WsfUnclassifiedAssetManager(WsfScenario& aScenario);
      WsfUnclassifiedAssetManager(WsfScenario& aScenario, const std::shared_ptr<il::AssetManagerInterface>& am_component);
      WsfProcessor* Clone() const override;

      // Plugin Interface
      static WSF_IADS_C2_LIB_EXPORT void RegisterScriptTypes(WsfApplication* aApplicationPtr);
      static WSF_IADS_C2_LIB_EXPORT WsfProcessor* Create(WsfScenario& aScenario);
      static WSF_IADS_C2_LIB_EXPORT std::unique_ptr<WsfUnclassifiedAssetManager> CreateUnique(WsfScenario& aScenario);
      // End Plugin Interface

      bool ProcessInput(UtInput& aInput) override;

      const char* GetScriptClassName() const override;
      const char* GetBaseClassName() override;

   protected:
      WsfUnclassifiedAssetManager(const WsfUnclassifiedAssetManager& from);
};

#endif
