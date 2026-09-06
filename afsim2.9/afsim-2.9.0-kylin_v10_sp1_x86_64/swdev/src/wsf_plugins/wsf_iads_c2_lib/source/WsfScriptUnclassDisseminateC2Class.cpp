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
*   File: WsfScriptDisseminateC2Class.cpp
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
*   Abstract: This is an abstract script base class for all disseminate C2 processor
*     script classes to inherit from. It provides necessary common functionality.
*     It's not pure abstract in the sense that we actually instantiate an object of this
*     type purely for type registration purposes with the sole intent of giving AFSIM
*     scripts the ability to up/down cast to and from the inherited types.
*
*******************************************************************************/

// We must forward declare here as well because we have a circular inheritance hierarchy
namespace il
{
   class BattleManagerInterface;
   class unclassifiedDisseminateC2;
}

#include "WsfScriptUnclassDisseminateC2Class.hpp"
#include <iadsLib/assetManagerInterface.hpp>
#include <iadsLib/unclassifiedDisseminateC2.hpp>
#include "WsfUnclassDisseminateC2.hpp"

WsfScriptUnclassDisseminateC2Class::WsfScriptUnclassDisseminateC2Class(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptDisseminateC2Class(aClassName, aScriptTypesPtr)
{
   AddClassName(UNCLASS_DC2_SCRIPT_CLASS_NAME);
}

void* WsfScriptUnclassDisseminateC2Class::Create(const UtScriptContext& aContext)
{
   std::shared_ptr<il::unclassifiedDisseminateC2> dc2(new il::unclassifiedDisseminateC2());

#ifdef IC2DEBUG
   HCL_INFO_LOGGER() << "WsfScriptUnclassDisseminateC2Class::Create(): dc2 = " << dc2;
#endif

   return reinterpret_cast<void*>(new WsfUnclassifiedDisseminateC2(*WsfScriptContext::GetSCENARIO(aContext), dc2));
}

void* WsfScriptUnclassDisseminateC2Class::Clone(void* aObjectPtr)
{
   WsfUnclassifiedDisseminateC2* objPtr = reinterpret_cast<WsfUnclassifiedDisseminateC2*>(aObjectPtr);
   return objPtr->Clone();
}

void  WsfScriptUnclassDisseminateC2Class::Destroy(void* aObjectPtr)
{
   WsfUnclassifiedDisseminateC2* objPtr = reinterpret_cast<WsfUnclassifiedDisseminateC2*>(aObjectPtr);
   delete objPtr;
}

extern "C" WSF_IADS_C2_LIB_EXPORT const char* WsfScriptUnclassDisseminateC2Class::GetScriptClassName()
{
   return UNCLASS_DC2_SCRIPT_CLASS_NAME;
}

extern "C" WSF_IADS_C2_LIB_EXPORT const char* WsfScriptUnclassDisseminateC2Class::GetBaseClassName()
{
   return UNCLASS_DC2_BASE_CLASS_NAME;
}
