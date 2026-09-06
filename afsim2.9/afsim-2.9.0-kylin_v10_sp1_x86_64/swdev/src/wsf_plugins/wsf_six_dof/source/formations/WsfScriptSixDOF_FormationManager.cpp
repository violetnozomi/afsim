// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptSixDOF_FormationManager.hpp"

#include <string>

#include "UtMemory.hpp"
#include "WsfSixDOF_FormationManager.hpp"
#include "WsfSixDOF_FormationSimulationExtension.hpp"
#include "script/WsfScriptDefs.hpp"

namespace wsf
{
namespace six_dof
{
namespace script
{
FormationManager::FormationManager(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   mConstructible = false;
   SetClassName("WsfSixDOF_FormationManager");

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

UT_DEFINE_SCRIPT_METHOD(FormationManager, wsf::six_dof::FormationManager, GetFormation, 1, "WsfSixDOF_Formation", "string")
{
   wsf::six_dof::FormationManager& mgr     = wsf::six_dof::FormationSimulationExtension::Get(SIMULATION).GetManager();
   wsf::six_dof::Formation*        formPtr = mgr.GetFormation(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(formPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(FormationManager, wsf::six_dof::FormationManager, HasFormation, 1, "bool", "string")
{
   wsf::six_dof::FormationManager& mgr = wsf::six_dof::FormationSimulationExtension::Get(SIMULATION).GetManager();
   aReturnVal.SetBool(mgr.HasFormation(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(FormationManager, wsf::six_dof::FormationManager, GetTopLevelFormations, 0, "Array<WsfSixDOF_Formation>", "")
{
   wsf::six_dof::FormationManager&       mgr = wsf::six_dof::FormationSimulationExtension::Get(SIMULATION).GetManager();
   std::vector<wsf::six_dof::Formation*> topForms       = mgr.GetTopLevelFormations();
   std::vector<UtScriptData>*            retvalPtr      = new std::vector<UtScriptData>{};
   UtScriptClass*                        formationClass = aContext.GetTypes()->GetClass("WsfSixDOF_Formation");
   for (auto i : topForms)
   {
      retvalPtr->push_back(UtScriptData(new UtScriptRef(i, formationClass)));
   }
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(FormationManager, wsf::six_dof::FormationManager, GetAllFormations, 0, "Array<WsfSixDOF_Formation>", "")
{
   wsf::six_dof::FormationManager&       mgr = wsf::six_dof::FormationSimulationExtension::Get(SIMULATION).GetManager();
   std::vector<wsf::six_dof::Formation*> allForms       = mgr.GetAllFormations();
   std::vector<UtScriptData>*            retvalPtr      = new std::vector<UtScriptData>{};
   UtScriptClass*                        formationClass = aContext.GetTypes()->GetClass("WsfSixDOF_Formation");
   for (auto i : allForms)
   {
      retvalPtr->push_back(UtScriptData(new UtScriptRef(i, formationClass)));
   }
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(FormationManager, wsf::six_dof::FormationManager, GetFormationFromMember, 1, "WsfSixDOF_Formation", "string")
{
   wsf::six_dof::FormationManager& mgr     = wsf::six_dof::FormationSimulationExtension::Get(SIMULATION).GetManager();
   wsf::six_dof::Formation*        formPtr = mgr.GetFormationFromMember(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(formPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(FormationManager, wsf::six_dof::FormationManager, DisbandFormation, 1, "bool", "string")
{
   wsf::six_dof::FormationManager& mgr    = wsf::six_dof::FormationSimulationExtension::Get(SIMULATION).GetManager();
   bool                            retval = mgr.DisbandFormation(aVarArgs[0].GetString());
   aReturnVal.SetBool(retval);
}

UT_DEFINE_SCRIPT_METHOD(FormationManager, wsf::six_dof::FormationManager, CreateUnit, 1, "WsfSixDOF_Formation", "string")
{
   wsf::six_dof::FormationManager& mgr       = wsf::six_dof::FormationSimulationExtension::Get(SIMULATION).GetManager();
   wsf::six_dof::Formation*        retvalPtr = mgr.CreateFormation("unit", aVarArgs[0].GetString());
   retvalPtr->SetAttached(false);
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(FormationManager, wsf::six_dof::FormationManager, CreateSection, 1, "WsfSixDOF_Formation", "string")
{
   wsf::six_dof::FormationManager& mgr       = wsf::six_dof::FormationSimulationExtension::Get(SIMULATION).GetManager();
   wsf::six_dof::Formation*        retvalPtr = mgr.CreateFormation("section", aVarArgs[0].GetString());
   retvalPtr->SetAttached(false);
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(FormationManager, wsf::six_dof::FormationManager, CreateFormation_1, 1, "WsfSixDOF_Formation", "string")
{
   wsf::six_dof::FormationManager& mgr       = wsf::six_dof::FormationSimulationExtension::Get(SIMULATION).GetManager();
   wsf::six_dof::Formation*        retvalPtr = mgr.CreateFormation("formation", aVarArgs[0].GetString());
   if (retvalPtr)
   {
      retvalPtr->SetAttached(false);
   }
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(FormationManager, wsf::six_dof::FormationManager, CreateFormation_2, 2, "WsfSixDOF_Formation", "string, string")
{
   wsf::six_dof::FormationManager& mgr       = wsf::six_dof::FormationSimulationExtension::Get(SIMULATION).GetManager();
   wsf::six_dof::Formation*        retvalPtr = mgr.CreateFormation(aVarArgs[0].GetString(), aVarArgs[1].GetString());
   if (retvalPtr)
   {
      retvalPtr->SetAttached(false);
   }
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr, aReturnClassPtr));
}
} // namespace script
} // namespace six_dof
} // namespace wsf
