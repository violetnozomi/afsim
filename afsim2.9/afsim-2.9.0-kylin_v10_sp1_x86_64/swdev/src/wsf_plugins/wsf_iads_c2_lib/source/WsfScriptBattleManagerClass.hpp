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
*   File: WsfScriptBattleManagerClass.hpp
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
*   Abstract: This is an abstract script base class for all battle manager
*     script classes to inherit from. It provides necessary common functionality.
*     It's not pure abstract in the sense that we actually instantiate an object of this
*     type purely for type registration purposes with the sole intent of giving AFSIM
*     scripts the ability to up/down cast to and from the inherited types.
*
*******************************************************************************/

#ifndef WSF_SCRIPT_BATTLE_MANAGER_CLASS_HPP
#define WSF_SCRIPT_BATTLE_MANAGER_CLASS_HPP

#include <script/WsfScriptProcessorClass.hpp>
#include <WsfBMCommon.hpp>

class WSF_IADS_C2_LIB_EXPORT WsfScriptBattleManagerClass : public WsfScriptProcessorClass
{
   public:

      WsfScriptBattleManagerClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

      void* Create(const UtScriptContext& aContext) override;
      void* Clone(void* aObjectPtr) override;
      void  Destroy(void* aObjectPtr) override;

      static const char* SCRIPT_CLASS_NAME;
      static const char* BASE_CLASS_NAME;

      // Plugin API.
      static const char* GetScriptClassName();
      static const char* GetBaseClassName();

      UT_DECLARE_SCRIPT_METHOD(RunModel);
      UT_DECLARE_SCRIPT_METHOD(HasCommitAuthority);
};

#endif
