// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef SENSORMANAGERFOV_HPP
#define SENSORMANAGERFOV_HPP

#include <memory>

#include "WsfProcessor.hpp"
#include "WsfBMCommon.hpp"
#include "WsfSimpleSensorsManager.hpp"
#include "WsfSensorsManagerFovImpl.hpp"

class WsfApplication;
class WsfScenario;

class WsfSensorsManagerFov : public WsfSimpleSensorsManager
{
   public:
      static WSF_IADS_C2_LIB_EXPORT void RegisterScriptTypes(WsfApplication* application);
      static WSF_IADS_C2_LIB_EXPORT WsfProcessor* Create(WsfScenario& scenario);
      static WSF_IADS_C2_LIB_EXPORT std::unique_ptr<WsfSensorsManagerFov> CreateUnique(WsfScenario& aScenario);

      WsfSensorsManagerFov(WsfScenario& scenario,
                           std::shared_ptr<il::SensorsManagerInterface> sensorsManagerComponent = std::make_shared<WsfSensorsManagerFovImpl>());
      virtual ~WsfSensorsManagerFov() = default;

      WsfProcessor* Clone() const override;
      bool ProcessInput(UtInput& input) override;
      bool Initialize(double simTime) override;
      bool Initialize2(double simTime) override;

      const char* GetScriptClassName() const override;
      const char* GetBaseClassName() override;
      void Run(const double simTime) override;

   protected:
      WsfSensorsManagerFov(const WsfSensorsManagerFov&);

   private:
      WsfSensorsManagerFov& operator=(const WsfSensorsManagerFov&) = delete;
};

#endif
