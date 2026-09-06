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
*   File: WsfUnclassDisseminateC2.cpp
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

#include "WsfUnclassDisseminateC2.hpp"
#include "WsfScriptUnclassDisseminateC2Class.hpp"
#include <WsfBMPluginUtilities.hpp>
#include <UtMemory.hpp>
#include <iadsLib/unclassifiedDisseminateC2.hpp>

#include <script/WsfScriptManager.hpp>

WsfUnclassifiedDisseminateC2::WsfUnclassifiedDisseminateC2(WsfScenario& aScenario, const std::shared_ptr<il::DisseminateC2Interface>& dc2_component)
   : WsfBMDisseminateC2(aScenario, UNCLASS_DC2_SCRIPT_CLASS_NAME, "PROCESSOR", dc2_component)
{
}

WsfUnclassifiedDisseminateC2::WsfUnclassifiedDisseminateC2(const WsfUnclassifiedDisseminateC2& from)
   : WsfBMDisseminateC2(from)
{
}

WsfProcessor* WsfUnclassifiedDisseminateC2::Clone() const
{
#ifdef IC2DEBUG
   HCL_TRACE_LOGGER() << "WsfUnclassifiedDisseminateC2::Clone(before)";
#endif
   return new WsfUnclassifiedDisseminateC2(*this);
}

// Plugin Interface
extern "C" WSF_IADS_C2_LIB_EXPORT void WsfUnclassifiedDisseminateC2::RegisterScriptTypes(WsfApplication* aApplicationPtr)
{
   if (!IsClassTypeRegistered(aApplicationPtr, UNCLASS_DC2_SCRIPT_CLASS_NAME))
   {
      // base class registration
      WsfBMDisseminateC2::RegisterScriptTypes(aApplicationPtr);

      auto script_types = aApplicationPtr->GetScriptTypes();
      script_types->Register(ut::make_unique<WsfScriptUnclassDisseminateC2Class>(UNCLASS_DC2_SCRIPT_CLASS_NAME, script_types));
   }
}

extern "C" WSF_IADS_C2_LIB_EXPORT WsfProcessor* WsfUnclassifiedDisseminateC2::Create(WsfScenario& aScenario)
{
   std::shared_ptr<il::unclassifiedDisseminateC2> dc2(new il::unclassifiedDisseminateC2());

#ifdef IC2DEBUG
   HCL_INFO_LOGGER() << "WsfUnclassifiedDisseminateC2::Create(): dc2 = " << dc2;
#endif

   return new WsfUnclassifiedDisseminateC2(aScenario, dc2);
}

WSF_IADS_C2_LIB_EXPORT std::unique_ptr<WsfUnclassifiedDisseminateC2> WsfUnclassifiedDisseminateC2::CreateUnique(WsfScenario& aScenario)
{
   return ut::make_unique<WsfUnclassifiedDisseminateC2>(aScenario);
}

bool WsfUnclassifiedDisseminateC2::ProcessInput(UtInput& aInput)
{
   bool myCommand = false;

   if (WsfBMDisseminateC2::ProcessInput(aInput))
   {
      myCommand = true;
   }

   return myCommand;
}

const char* WsfUnclassifiedDisseminateC2::GetScriptClassName() const
{
   return UNCLASS_DC2_SCRIPT_CLASS_NAME;
}

const char* WsfUnclassifiedDisseminateC2::GetBaseClassName()
{
   return UNCLASS_DC2_BASE_CLASS_NAME;
}
