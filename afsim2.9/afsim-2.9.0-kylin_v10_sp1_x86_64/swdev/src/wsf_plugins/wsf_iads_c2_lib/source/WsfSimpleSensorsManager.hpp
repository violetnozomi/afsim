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
*   File: WsfSimpleSensorsManager.hpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name:	Radiance Technologies
*   Address:			350 Wynn Drive
*   					Huntsville, AL 35805
*
*   Abstract:
*
*******************************************************************************/

#ifndef WSF_SIMPLE_SENSORS_MANAGER_HPP
#define WSF_SIMPLE_SENSORS_MANAGER_HPP

#include <memory>

#include <WsfApplication.hpp>
#include <WsfBMSensorsManager.hpp>
#include <WsfSimpleSensorsManagerImpl.hpp>

class WsfSimpleSensorsManager : public WsfBMSensorsManager
{
   public:

      WsfSimpleSensorsManager(WsfScenario& aScenario,
                              const std::shared_ptr<il::SensorsManagerInterface>& sm_component = std::make_shared<WsfSimpleSensorsManagerImpl>());
      virtual ~WsfSimpleSensorsManager() { }

      WsfProcessor* Clone() const override;

      // Plugin Interface
      static WSF_IADS_C2_LIB_EXPORT void RegisterScriptTypes(WsfApplication* aApplicationPtr);
      static WSF_IADS_C2_LIB_EXPORT WsfProcessor* Create(WsfScenario& aScenario);
      static WSF_IADS_C2_LIB_EXPORT std::unique_ptr<WsfSimpleSensorsManager> CreateUnique(WsfScenario& aScenario);
      // End Plugin Interface

      bool ProcessInput(UtInput& aInput) override;

      const char* GetScriptClassName() const override;

      bool Initialize(double aSimTime) override;
      bool Initialize2(double aSimTime) override;
      void Update(double aSimTime) override;

   protected:

      WsfSimpleSensorsManager(WsfScenario& scenario,
                              const std::string& partName,
                              const std::shared_ptr<il::SensorsManagerInterface>& smInterface);
      WsfSimpleSensorsManager(const WsfSimpleSensorsManager& aSrc);
};

#endif
