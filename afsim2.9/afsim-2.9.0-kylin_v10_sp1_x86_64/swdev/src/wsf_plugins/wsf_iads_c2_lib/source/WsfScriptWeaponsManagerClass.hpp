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
*   File: WsfScriptWeaponsManagerClass.hpp
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
*   Abstract: This is an abstract script base class for all weapons manager processor
*     script classes to inherit from. It provides necessary common functionality.
*     It's not pure abstract in the sense that we actually instantiate an object of this
*     type purely for type registration purposes with the sole intent of giving AFSIM
*     scripts the ability to up/down cast to and from the inherited types.
*
*******************************************************************************/

#ifndef WSF_SCRIPT_WEAPONS_MANAGER_CLASS_HPP
#define WSF_SCRIPT_WEAPONS_MANAGER_CLASS_HPP

#include <WsfBMCommon.hpp>
#include <script/WsfScriptProcessorClass.hpp>

class WsfScriptWeaponsManagerClass : public WsfScriptProcessorClass
{
   public:

      WsfScriptWeaponsManagerClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
      virtual ~WsfScriptWeaponsManagerClass() = default;

      void* Create(const UtScriptContext& aContext) override;
      void* Clone(void* aObjectPtr) override;
      void  Destroy(void* aObjectPtr) override;

      static const char* SCRIPT_CLASS_NAME;
      static const char* BASE_CLASS_NAME;

      // Plugin API.
      static WSF_IADS_C2_LIB_EXPORT const char* GetScriptClassName();
      static WSF_IADS_C2_LIB_EXPORT const char* GetBaseClassName();

      UT_DECLARE_SCRIPT_METHOD(IsWEZLibrarySpecified);
      UT_DECLARE_SCRIPT_METHOD(GetWEZLibraryName);

      UT_DECLARE_SCRIPT_METHOD(IsWEZTacticalRangeAero);
      UT_DECLARE_SCRIPT_METHOD(IsWEZTacticalRangeMax1);
      UT_DECLARE_SCRIPT_METHOD(IsWEZTacticalRangeMax2);

      UT_DECLARE_SCRIPT_METHOD(GetEngageEWTargets);
      UT_DECLARE_SCRIPT_METHOD(GetEngageTARTargets);
      UT_DECLARE_SCRIPT_METHOD(GetEngageTTRTargets);
};

#endif
