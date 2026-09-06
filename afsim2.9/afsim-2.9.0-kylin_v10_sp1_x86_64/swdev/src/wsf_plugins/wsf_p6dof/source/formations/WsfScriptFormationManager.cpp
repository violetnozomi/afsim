// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptFormationManager.hpp"

#include <string>

#include "UtMemory.hpp"
#include "WsfFormationManager.hpp"
#include "WsfFormationSimulationExtension.hpp"
#include "script/WsfScriptDefs.hpp"

WsfScriptFormationManager::WsfScriptFormationManager(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   mConstructible = false;
   SetClassName("WsfFormationManager");

   AddStaticMethod(ut::make_unique<GetFormation>());
   AddStaticMethod(ut::make_unique<HasFormation>());
   AddStaticMethod(ut::make_unique<GetTopLevelFormations>());
   AddStaticMethod(ut::make_unique<GetAllFormations>());
   AddStaticMethod(ut::make_unique<GetFormationFromMember>());

   AddStaticMethod(ut::make_unique<DisbandFormation>());

   AddStaticMethod(ut::make_unique<CreateUnit>());
   AddStaticMethod(ut::make_unique<CreateSection>());
   AddStaticMethod(ut::make_unique<CreateFormation_1>("CreateFormation"));
   AddStaticMethod(ut::make_unique<CreateFormation_2>("CreateFormation"));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationManager, WsfFormationManager, GetFormation, 1, "WsfFormation", "string")
{
   WsfFormationManager& mgr     = WsfFormationSimulationExtension::Get(SIMULATION).GetManager();
   WsfFormation*        formPtr = mgr.GetFormation(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(formPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationManager, WsfFormationManager, HasFormation, 1, "bool", "string")
{
   WsfFormationManager& mgr = WsfFormationSimulationExtension::Get(SIMULATION).GetManager();
   aReturnVal.SetBool(mgr.HasFormation(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationManager, WsfFormationManager, GetTopLevelFormations, 0, "Array<WsfFormation>", "")
{
   WsfFormationManager&       mgr            = WsfFormationSimulationExtension::Get(SIMULATION).GetManager();
   std::vector<WsfFormation*> topForms       = mgr.GetTopLevelFormations();
   std::vector<UtScriptData>* retvalPtr      = new std::vector<UtScriptData>{};
   UtScriptClass*             formationClass = aContext.GetTypes()->GetClass("WsfFormation");
   for (auto i : topForms)
   {
      retvalPtr->push_back(UtScriptData(new UtScriptRef(i, formationClass)));
   }
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationManager, WsfFormationManager, GetAllFormations, 0, "Array<WsfFormation>", "")
{
   WsfFormationManager&       mgr            = WsfFormationSimulationExtension::Get(SIMULATION).GetManager();
   std::vector<WsfFormation*> allForms       = mgr.GetAllFormations();
   std::vector<UtScriptData>* retvalPtr      = new std::vector<UtScriptData>{};
   UtScriptClass*             formationClass = aContext.GetTypes()->GetClass("WsfFormation");
   for (auto i : allForms)
   {
      retvalPtr->push_back(UtScriptData(new UtScriptRef(i, formationClass)));
   }
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationManager, WsfFormationManager, GetFormationFromMember, 1, "WsfFormation", "string")
{
   WsfFormationManager& mgr     = WsfFormationSimulationExtension::Get(SIMULATION).GetManager();
   WsfFormation*        formPtr = mgr.GetFormationFromMember(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(formPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationManager, WsfFormationManager, DisbandFormation, 1, "bool", "string")
{
   WsfFormationManager& mgr    = WsfFormationSimulationExtension::Get(SIMULATION).GetManager();
   bool                 retval = mgr.DisbandFormation(aVarArgs[0].GetString());
   aReturnVal.SetBool(retval);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationManager, WsfFormationManager, CreateUnit, 1, "WsfFormation", "string")
{
   WsfFormationManager& mgr       = WsfFormationSimulationExtension::Get(SIMULATION).GetManager();
   WsfFormation*        retvalPtr = mgr.CreateFormation("unit", aVarArgs[0].GetString());
   retvalPtr->SetAttached(false);
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationManager, WsfFormationManager, CreateSection, 1, "WsfFormation", "string")
{
   WsfFormationManager& mgr       = WsfFormationSimulationExtension::Get(SIMULATION).GetManager();
   WsfFormation*        retvalPtr = mgr.CreateFormation("section", aVarArgs[0].GetString());
   retvalPtr->SetAttached(false);
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationManager, WsfFormationManager, CreateFormation_1, 1, "WsfFormation", "string")
{
   WsfFormationManager& mgr       = WsfFormationSimulationExtension::Get(SIMULATION).GetManager();
   WsfFormation*        retvalPtr = mgr.CreateFormation("formation", aVarArgs[0].GetString());
   if (retvalPtr)
   {
      retvalPtr->SetAttached(false);
   }
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationManager, WsfFormationManager, CreateFormation_2, 2, "WsfFormation", "string, string")
{
   WsfFormationManager& mgr       = WsfFormationSimulationExtension::Get(SIMULATION).GetManager();
   WsfFormation*        retvalPtr = mgr.CreateFormation(aVarArgs[0].GetString(), aVarArgs[1].GetString());
   if (retvalPtr)
   {
      retvalPtr->SetAttached(false);
   }
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr, aReturnClassPtr));
}
