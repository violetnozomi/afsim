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
*   File: WsfUnclassDisseminateC2.hpp
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

#ifndef WSF_UNCLASS_DISSEMINATE_C2_HPP
#define WSF_UNCLASS_DISSEMINATE_C2_HPP

#include <memory>
#include "WsfBMDisseminateC2.hpp"
#include "iadsLib/unclassifiedDisseminateC2.hpp"

class WsfUnclassifiedDisseminateC2 : public WsfBMDisseminateC2
{
   public:

      WsfUnclassifiedDisseminateC2(WsfScenario& aScenario, const std::shared_ptr<il::DisseminateC2Interface>& dc2_component = std::make_shared<il::unclassifiedDisseminateC2>());
      WsfProcessor* Clone() const override;

      // Plugin Interface
      static WSF_IADS_C2_LIB_EXPORT void RegisterScriptTypes(WsfApplication* aApplicationPtr);
      static WSF_IADS_C2_LIB_EXPORT WsfProcessor* Create(WsfScenario& aScenario);
      static WSF_IADS_C2_LIB_EXPORT std::unique_ptr<WsfUnclassifiedDisseminateC2> CreateUnique(WsfScenario& aScenario);
      // End Plugin Interface

      bool ProcessInput(UtInput& aInput) override;

      const char* GetScriptClassName() const override;
      const char* GetBaseClassName() override;

   protected:
      WsfUnclassifiedDisseminateC2(const WsfUnclassifiedDisseminateC2& from);
};

#endif
