// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: ScenarioAnalyzerUtilities.cpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name: Radiance Technologies
*   Address:         350 Wynn Drive
*                    Huntsville, AL 35805
*
*   Abstract:
*
*******************************************************************************/

#include <iostream>
#include <string>
#include <vector>

#include "UtMemory.hpp"
#include "UtPlugin.hpp"
#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"
#include "UtScriptData.hpp"
#include "UtScriptTypes.hpp"

#include "WsfApplication.hpp"
#include "WsfComm.hpp"
#include "WsfCommAddress.hpp"
#include "WsfCommandChain.hpp"
#include "WsfExternalLinks.hpp"
#include "WsfInternalLinks.hpp"
#include "WsfGroupManager.hpp"
#include "WsfLinkedProcessor.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformPart.hpp"
#include "script/WsfScriptContext.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTrackProcessor.hpp"


#include "ScenarioAnalyzerUtilities.hpp"

// Given a starting platform and the name of command chain that the platform belongs to, returns a pointer to the top-level
// commander in that command chain (commander == self). Returns nullptr if platform doesn't belong to specified command chain.
WsfPlatform* getTopCommander(WsfStringId commandChainName, WsfPlatform* platform)
{
   WsfPlatform* commander = platform->GetComponent<WsfCommandChain>(commandChainName)->GetCommander();
   WsfPlatform* current = platform;
   while (commander && commander->GetNameId() != current->GetNameId())
   {
      current = commander;
      commander = current->GetComponent<WsfCommandChain>(commandChainName)->GetCommander();
   }
   // commander may be a nullptr here: if so, current is the top commmander, so return current.
   return commander ? commander : current;
}

// Returns true if check passes for specified platform or any platform above it in any command chain to which it belongs.
bool checkUpCommandChain(WsfPlatform* platform, PlatformCheck check)
{
   // Return true if platform itself passes the check
   if (check(platform))
   {
      return true;
   }

   // Recursively check up each of the direct commander's command chains.
   unsigned chainCount = platform->GetComponentCount<WsfCommandChain>();
   for (unsigned j = 0; j < chainCount; ++j)
   {
      WsfPlatform* commander = platform->GetComponentEntry<WsfCommandChain>(j)->GetCommander();
      if (commander && commander->GetNameId() != platform->GetNameId())
      {
         if (checkUpCommandChain(platform->GetComponentEntry<WsfCommandChain>(j)->GetCommander(), check))
         {
            return true;
         }
      }
   }
   return false;
}

// Returns true if check passes for specified platform or any platform above it in any command chain to which it belongs.
// Used by UtScriptMethod CheckUpAllCommandChains. Assumes that platform, platformClass, and check are valid pointers.
bool checkUpCommandChain(WsfPlatform* platform, WsfScriptContext& context, UtScriptClass* platformClass, UtScript* check)
{
   UtScriptData retVal;
   UtScriptDataList argList;
   argList.push_back(UtScriptData(new UtScriptRef(platform, platformClass)));
   context.ExecuteScript(check, retVal, argList);
   bool result = retVal.GetBool();
   if (result)
   {
      return true;
   }

   // Recursively check up each of the direct commander's command chains.
   unsigned chainCount = platform->GetComponentCount<WsfCommandChain>();
   for (unsigned j = 0; j < chainCount; ++j)
   {
      WsfPlatform* commander = platform->GetComponentEntry<WsfCommandChain>(j)->GetCommander();
      if (commander && commander->GetNameId() != platform->GetNameId())
      {
         if (checkUpCommandChain(platform->GetComponentEntry<WsfCommandChain>(j)->GetCommander(), context, platformClass, check))
         {
            return true;
         }
      }
   }
   return false;
}

// Returns true if check passes for specified platform or any platform above it in the specified command chain.
bool checkUpCommandChain(WsfPlatform* platform, PlatformCheck check, WsfStringId commandChainName)
{
   // Return true if platform itself passes the check
   if (check(platform))
   {
      return true;
   }

   // Recursively check up the specified command chain.
   WsfCommandChain* commandChain = platform->GetComponent<WsfCommandChain>(commandChainName);
   if (commandChain)
   {
      WsfPlatform* commander = commandChain->GetCommander();
      if (commander && commander->GetNameId() != platform->GetNameId())
      {
         if (checkUpCommandChain(commander, check, commandChainName))
         {
            return true;
         }
      }
   }
   return false;
}

// Returns true if check passes for specified platform or any platform above it in the specified command chain.
// Used by UtScriptMethod CheckUpAllCommandChains. Assumes that platform, platformClass, and check are valid pointers.
bool checkUpCommandChain(WsfPlatform* platform, WsfScriptContext& context, UtScriptClass* platformClass, UtScript* check, WsfStringId commandChainName)
{
   UtScriptData retVal;
   UtScriptDataList argList;
   argList.push_back(UtScriptData(new UtScriptRef(platform, platformClass)));
   context.ExecuteScript(check, retVal, argList);
   bool result = retVal.GetBool();
   if (result)
   {
      return true;
   }

   // Recursively check up the specified command chain.
   WsfCommandChain* commandChain = platform->GetComponent<WsfCommandChain>(commandChainName);
   if (commandChain)
   {
      WsfPlatform* commander = commandChain->GetCommander();
      if (commander && commander->GetNameId() != platform->GetNameId())
      {
         if (checkUpCommandChain(commander, context, platformClass, check, commandChainName))
         {
            return true;
         }
      }
   }
   return false;
}


// Returns true if check passes for specified platform or any platform below it in any command chain to which it belongs.
bool checkDownCommandChain(WsfPlatform* platform, PlatformCheck check)
{
   // Return true if platform itself passes the check
   if (check(platform))
   {
      return true;
   }
   // Recursively check down each of the subordinate's command chains
   unsigned chainCount = platform->GetComponentCount<WsfCommandChain>();
   for (unsigned j = 0; j < chainCount; ++j)
   {
      std::vector<WsfPlatform*> subordinates = platform->GetComponentEntry<WsfCommandChain>(j)->GetSubordinates();
      for (auto& s : subordinates)
      {
         if (s && checkDownCommandChain(s, check))
         {
            return true;
         }
      }
   }
   return false;
}

// Returns true if check passes for specified platform or any platform below it in any command chain to which it belongs.
// Used by UtScriptMethod CheckUpAllCommandChains. Assumes that platform, platformClass, and check are valid pointers.
bool checkDownCommandChain(WsfPlatform* platform, WsfScriptContext& context, UtScriptClass* platformClass, UtScript* check)
{
   UtScriptData retVal;
   UtScriptDataList argList;
   argList.push_back(UtScriptData(new UtScriptRef(platform, platformClass)));
   context.ExecuteScript(check, retVal, argList);
   bool result = retVal.GetBool();
   if (result)
   {
      return true;
   }

   // Recursively check down each of the subordinates's command chains.
   unsigned chainCount = platform->GetComponentCount<WsfCommandChain>();
   for (unsigned j = 0; j < chainCount; ++j)
   {
      std::vector<WsfPlatform*> subordinates = platform->GetComponentEntry<WsfCommandChain>(j)->GetSubordinates();
      for (auto& s : subordinates)
      {
         if (s && checkDownCommandChain(s, context, platformClass, check))
         {
            return true;
         }
      }
   }
   return false;
}

// Returns true if check passes for specified platform, or for any platform below it in the specified command chain
bool checkDownCommandChain(WsfPlatform* platform, PlatformCheck check, WsfStringId commandChainName)
{
   if (check(platform))
   {
      return true;
   }
   WsfCommandChain* commandChain = platform->GetComponent<WsfCommandChain>(commandChainName);
   if (commandChain)
   {
      std::vector<WsfPlatform*> subordinates = commandChain->GetSubordinates();
      for (auto& s : subordinates)
      {
         if (s && checkDownCommandChain(s, check, commandChainName))
         {
            return true;
         }
      }
   }
   return false;
}

// Returns true if check passes for specified platform or any platform below it in the specified command chain.
// Used by UtScriptMethod CheckUpAllCommandChains. Assumes that platform, platformClass, and check are valid pointers.
bool checkDownCommandChain(WsfPlatform* platform, WsfScriptContext& context, UtScriptClass* platformClass, UtScript* check, WsfStringId commandChainName)
{
   UtScriptData retVal;
   UtScriptDataList argList;
   argList.push_back(UtScriptData(new UtScriptRef(platform, platformClass)));
   context.ExecuteScript(check, retVal, argList);
   if (retVal.GetBool())
   {
      return true;
   }

   // Recursively check down the specified command chain.
   WsfCommandChain* commandChain = platform->GetComponent<WsfCommandChain>(commandChainName);
   if (commandChain)
   {
      std::vector<WsfPlatform*> subordinates = commandChain->GetSubordinates();
      for (auto& s : subordinates)
      {
         if (s && checkDownCommandChain(s, context, platformClass, check, commandChainName))
         {
            return true;
         }
      }
   }
   return false;
}


// Returns true if check passes for specified platform or for any platform in any of its command chains
// If platform is part of an explicit command chain, ignore "default," which is implicitly populated using
// structure of named command chains. (See comment at top of WsfCommandChain.cpp).
bool checkFullCommandChain(WsfPlatform* platform, PlatformCheck check)
{
   // For each command chain that platform belongs to, find the top-level commander (the platform that is its own commander).
   unsigned commandChainCount = platform->GetComponentCount<WsfCommandChain>();
   for (unsigned i = 0; i < commandChainCount; ++i)
   {
      WsfStringId commandChainName = platform->GetComponentEntry<WsfCommandChain>(i)->GetNameId();
      // Ignore the default command chain if platform belongs to at least one declared command chain.
      if (commandChainCount > 1 && commandChainName == WsfCommandChain::GetDefaultNameId())
      {
         continue;
      }
      WsfPlatform* topCommander = getTopCommander(commandChainName, platform);
      // Return true if any of platform's top-level commanders has a platform below it (in any of the commander's chains) that passes the check.
      if (checkDownCommandChain(topCommander, check, commandChainName))
      {
         return true;
      }
   }
   // No platform below any of platform's top-level commanders passes the check - return false.
   return false;
}

// Returns true if check passes for specified platform or for any platform in any of its command chains
// If platform is part of an explicit command chain, ignore "default," which is implicitly populated using
// structure of named command chains. (See comment at top of WsfCommandChain.cpp).
// Used by UtScriptMethod CheckUpAllCommandChains. Assumes that platform, platformClass, and check are valid pointers.
bool checkFullCommandChain(WsfPlatform* platform, WsfScriptContext& context, UtScriptClass* platformClass, UtScript* check)
{
   // For each command chain that platform belongs to, find the top-level commander (the platform that is its own commander).
   unsigned commandChainCount = platform->GetComponentCount<WsfCommandChain>();
   for (unsigned i = 0; i < commandChainCount; ++i)
   {
      WsfStringId commandChainName = platform->GetComponentEntry<WsfCommandChain>(i)->GetNameId();
      // Ignore the default command chain if platform belongs to at least one declared command chain.
      if (commandChainCount > 1 && commandChainName == WsfCommandChain::GetDefaultNameId())
      {
         continue;
      }
      WsfPlatform* topCommander = getTopCommander(commandChainName, platform);
      // Return true if any of platform's top-level commanders has a platform below it (in any of the commander's chains) that passes the check.
      if (checkDownCommandChain(topCommander, context, platformClass, check, commandChainName))
      {
         return true;
      }
   }
   // No platform below any of platform's top-level commmanders passes the check - return false.
   return false;
}


class ScenarioAnalyzerUtils
{
   public:
      const char* GetScriptClassName()
      {
         return "ScenarioAnalyzerUtils";
      }
};

class ScenarioAnalyzerUtilsScriptClass : public UtScriptClass
{
   public:
      UT_DECLARE_SCRIPT_METHOD(Message);
      UT_DECLARE_SCRIPT_METHOD(SendResultMessage);
      UT_DECLARE_SCRIPT_METHOD(SendResultMessageWithLinks);
      UT_DECLARE_SCRIPT_METHOD(SendPassMessage);
      UT_DECLARE_SCRIPT_METHOD(SendSessionNoteMessage);

      UT_DECLARE_SCRIPT_METHOD(GetTopCommander);
      UT_DECLARE_SCRIPT_METHOD(CheckPlatform);
      UT_DECLARE_SCRIPT_METHOD(CheckUpOneCommandChain);
      UT_DECLARE_SCRIPT_METHOD(CheckUpAllCommandChains);
      UT_DECLARE_SCRIPT_METHOD(CheckDownOneCommandChain);
      UT_DECLARE_SCRIPT_METHOD(CheckDownAllCommandChains);
      UT_DECLARE_SCRIPT_METHOD(CheckFullCommandChain);
      UT_DECLARE_SCRIPT_METHOD(GetPlatformPartsDownAllCommandChains);
      UT_DECLARE_SCRIPT_METHOD(GetPlatformPartsDownOneCommandChain);

      UT_DECLARE_SCRIPT_METHOD(InternallyLinkedPlatformParts);
      UT_DECLARE_SCRIPT_METHOD(LinkedAndReachablePlatformParts);
      UT_DECLARE_SCRIPT_METHOD(LinkedAndReachablePlatformPartsChooseProcs);

      explicit ScenarioAnalyzerUtilsScriptClass(UtScriptTypes* types)
         : UtScriptClass("ScenarioAnalyzerUtils", types)
      {
         SetClassName("ScenarioAnalyzerUtils");
         SetClassName(GetClassName());
         AddStaticMethod(ut::make_unique<SendResultMessage>());
         AddStaticMethod(ut::make_unique<SendPassMessage>());
         AddStaticMethod(ut::make_unique<SendResultMessageWithLinks>());
         AddStaticMethod(ut::make_unique<SendSessionNoteMessage>());

         AddStaticMethod(ut::make_unique<GetTopCommander>());
         AddStaticMethod(ut::make_unique<CheckPlatform>());
         AddStaticMethod(ut::make_unique<CheckUpOneCommandChain>());
         AddStaticMethod(ut::make_unique<CheckUpAllCommandChains>());
         AddStaticMethod(ut::make_unique<CheckDownOneCommandChain>());
         AddStaticMethod(ut::make_unique<CheckDownAllCommandChains>());
         AddStaticMethod(ut::make_unique<CheckFullCommandChain>());
         AddStaticMethod(ut::make_unique<GetPlatformPartsDownAllCommandChains>());
         AddStaticMethod(ut::make_unique<GetPlatformPartsDownOneCommandChain>());

         AddStaticMethod(ut::make_unique<InternallyLinkedPlatformParts>());
         AddStaticMethod(ut::make_unique<LinkedAndReachablePlatformParts>());
         AddStaticMethod(ut::make_unique<LinkedAndReachablePlatformPartsChooseProcs>());
      }

      ~ScenarioAnalyzerUtilsScriptClass() override = default;
};


UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerUtilsScriptClass, ScenarioAnalyzerUtils, SendResultMessage, 4, "void", "string, string, string, string")
{
   const std::string& suite = aVarArgs[0].GetString();
   const std::string& checkName = aVarArgs[1].GetString();
   const std::string& severity = aVarArgs[2].GetString();
   const std::string& detail = aVarArgs[3].GetString();

   ScenarioAnalyzerMessage message(suite, checkName, detail, ScenarioAnalyzerMessage::SeverityLevelFromString(severity));
   message.Write();
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerUtilsScriptClass, ScenarioAnalyzerUtils, SendPassMessage, 2, "void", "string, string")
{
   const std::string& suite = aVarArgs[0].GetString();
   const std::string& checkName = aVarArgs[1].GetString();

   ScenarioAnalyzerMessage message(suite, checkName);
   message.SetSuccessful();
   message.Write();
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerUtilsScriptClass, ScenarioAnalyzerUtils, SendResultMessageWithLinks, 6, "void", "string, string, string, string, Array<string>, Array<string>")
{
   const std::string& suite = aVarArgs[0].GetString();
   const std::string& severity = aVarArgs[1].GetString();
   const std::string& checkName = aVarArgs[2].GetString();
   const std::string& detail = aVarArgs[3].GetString();

   ScenarioAnalyzerMessage message(suite, checkName, detail, ScenarioAnalyzerMessage::SeverityLevelFromString(severity));

   std::vector<UtScriptData>* typesListPtr = (std::vector<UtScriptData>*)aVarArgs[4].GetPointer()->GetAppObject();
   std::vector<UtScriptData>* namesListPtr = (std::vector<UtScriptData>*)aVarArgs[5].GetPointer()->GetAppObject();

   if (typesListPtr->size() == namesListPtr->size())
   {
      for (size_t i = 0; i != typesListPtr->size(); ++i)
      {
         message.AddScenarioFileLocation(ScenarioFileLocation((*typesListPtr)[i].GetString(), (*namesListPtr)[i].GetString()));
      }
   }

   message.Write();
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerUtilsScriptClass, ScenarioAnalyzerUtils, SendSessionNoteMessage, 2, "void", "string, string")
{
   std::string category = aVarArgs[0].GetString();
   std::string detail = aVarArgs[1].GetString();

   ScenarioAnalyzerMessage message(category, detail);
   message.Write();
}


UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerUtilsScriptClass, ScenarioAnalyzerUtils, GetTopCommander, 2, "WsfPlatform", "string, WsfPlatform")
{
   std::string commandChainName = aVarArgs[0].GetString();
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[1].GetPointer()->GetAppObject());
   for (unsigned i = 0, ccCount = platformPtr->GetComponentCount<WsfCommandChain>(); i != ccCount; ++i)
   {
      if (platformPtr->GetComponentEntry<WsfCommandChain>(i)->GetName() == commandChainName)
      {
         WsfPlatform* commanderPtr = getTopCommander(
                                        platformPtr->GetComponentEntry<WsfCommandChain>(i)->GetNameId(),
                                        platformPtr);
         aReturnVal.SetPointer(new UtScriptRef(commanderPtr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE)); // we don't own this pointer
      }
   }
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerUtilsScriptClass, ScenarioAnalyzerUtils, CheckPlatform, 2, "bool", "WsfPlatform, string")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::string checkName = aVarArgs[1].GetString();

   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   WsfScriptContext& globalContext = sim->GetScriptContext();
   UtScriptClass* platformClass = globalContext.GetClass("WsfPlatform");
   UtScript* checkScript = globalContext.FindScript(checkName);

   if (platformClass && checkScript)
   {
      UtScriptData retVal;
      UtScriptDataList argList;
      argList.push_back(UtScriptData(new UtScriptRef(platformPtr, platformClass)));
      globalContext.ExecuteScript(checkScript, retVal, argList);
      aReturnVal.SetBool(retVal.GetBool());
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerUtilsScriptClass, ScenarioAnalyzerUtils, CheckUpOneCommandChain, 3, "bool", "WsfPlatform, string, string")
{
   WsfPlatform* originPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::string checkName = aVarArgs[1].GetString();
   std::string commandChainName = aVarArgs[2].GetString();

   WsfStringId commandChainNameId;
   for (unsigned i = 0, ccCount = originPtr->GetComponentCount<WsfCommandChain>(); i != ccCount; ++i)
   {
      if (originPtr->GetComponentEntry<WsfCommandChain>(i)->GetName() == commandChainName)
      {
         commandChainNameId = originPtr->GetComponentEntry<WsfCommandChain>(i)->GetNameId();
         break;
      }
   }
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   WsfScriptContext& globalContext = sim->GetScriptContext();

   UtScriptClass* platformClass = globalContext.GetClass("WsfPlatform");
   UtScript* checkScript = globalContext.FindScript(checkName);

   if (originPtr && platformClass && checkScript && !commandChainNameId.IsNull())
   {
      bool checkPassed = checkUpCommandChain(originPtr, globalContext, platformClass, checkScript, commandChainNameId);
      aReturnVal.SetBool(checkPassed);
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerUtilsScriptClass, ScenarioAnalyzerUtils, CheckUpAllCommandChains, 2, "bool", "WsfPlatform, string")
{
   WsfPlatform* originPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::string checkName = aVarArgs[1].GetString();

   WsfSimulation* sim =  WsfScriptContext::GetSIMULATION(aContext);
   WsfScriptContext& globalContext = sim->GetScriptContext();

   UtScriptClass* platformClass = globalContext.GetClass("WsfPlatform");
   UtScript* checkScript = globalContext.FindScript(checkName);

   if (originPtr && platformClass && checkScript)
   {
      bool checkPassed = checkUpCommandChain(originPtr, globalContext, platformClass, checkScript);
      aReturnVal.SetBool(checkPassed);
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerUtilsScriptClass, ScenarioAnalyzerUtils, CheckDownOneCommandChain, 3, "bool", "WsfPlatform, string, string")
{
   WsfPlatform* originPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::string checkName = aVarArgs[1].GetString();
   std::string commandChainName = aVarArgs[2].GetString();

   WsfStringId commandChainNameId;
   for (unsigned i = 0, ccCount = originPtr->GetComponentCount<WsfCommandChain>(); i != ccCount; ++i)
   {
      if (originPtr->GetComponentEntry<WsfCommandChain>(i)->GetName() == commandChainName)
      {
         commandChainNameId = originPtr->GetComponentEntry<WsfCommandChain>(i)->GetNameId();
         break;
      }
   }
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   WsfScriptContext& globalContext = sim->GetScriptContext();

   UtScriptClass* platformClass = globalContext.GetClass("WsfPlatform");
   UtScript* checkScript = globalContext.FindScript(checkName);

   if (originPtr && platformClass && checkScript && !commandChainNameId.IsNull())
   {
      bool checkPassed = checkDownCommandChain(originPtr, globalContext, platformClass, checkScript, commandChainNameId);
      aReturnVal.SetBool(checkPassed);
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerUtilsScriptClass, ScenarioAnalyzerUtils, CheckDownAllCommandChains, 2, "bool", "WsfPlatform, string")
{
   WsfPlatform* originPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::string checkName = aVarArgs[1].GetString();
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   WsfScriptContext& globalContext = sim->GetScriptContext();
   UtScriptClass* platformClass = globalContext.GetClass("WsfPlatform");
   UtScript* checkScript = globalContext.FindScript(checkName);

   if (originPtr && platformClass && checkScript)
   {
      bool checkPassed = checkDownCommandChain(originPtr, globalContext, platformClass, checkScript);
      aReturnVal.SetBool(checkPassed);
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerUtilsScriptClass, ScenarioAnalyzerUtils, CheckFullCommandChain, 2, "bool", "WsfPlatform, string")
{
   WsfPlatform* originPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::string checkName = aVarArgs[1].GetString();
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   WsfScriptContext& globalContext = sim->GetScriptContext();
   UtScriptClass* platformClass = globalContext.GetClass("WsfPlatform");
   UtScript* checkScript = globalContext.FindScript(checkName);

   if (originPtr && platformClass && checkScript)
   {
      bool checkPassed = checkFullCommandChain(originPtr, globalContext, platformClass, checkScript);
      aReturnVal.SetBool(checkPassed);
   }
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerUtilsScriptClass, ScenarioAnalyzerUtils, GetPlatformPartsDownAllCommandChains, 2, "Array<WsfPlatformPart>", "WsfPlatform, string")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::string typeName = aVarArgs[1].GetString();
   std::vector<WsfPlatformPart*> allParts = getAllPartsDownCommandChain<WsfPlatformPart>(platformPtr);

   UtScriptClass* classPtr = aContext.GetTypes()->GetClass(aReturnClassPtr->GetContainerDataTypeId());
   std::vector<UtScriptData>* returnedPartsPtr = new std::vector<UtScriptData>();
   for (size_t i = 0; i != allParts.size(); ++i)
   {
      WsfPlatformPart* partPtr = allParts[i];
      if (partPtr->IsA_TypeOf(typeName))
      {
         returnedPartsPtr->push_back(UtScriptData(new UtScriptRef(partPtr, classPtr, UtScriptRef::cDONT_MANAGE)));
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(returnedPartsPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerUtilsScriptClass, ScenarioAnalyzerUtils, GetPlatformPartsDownOneCommandChain, 3, "Array<WsfPlatformPart>", "WsfPlatform, string, string")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::string typeName = aVarArgs[1].GetString();
   std::string commandChainName = aVarArgs[2].GetString();
   for (unsigned i = 0, ccCount = platformPtr->GetComponentCount<WsfCommandChain>(); i != ccCount; ++i)
   {
      if (platformPtr->GetComponentEntry<WsfCommandChain>(i)->GetName() == commandChainName)
      {
         std::vector<WsfPlatformPart*> allParts = getAllPartsDownCommandChain<WsfPlatformPart>(platformPtr);

         UtScriptClass* classPtr = aContext.GetTypes()->GetClass(aReturnClassPtr->GetContainerDataTypeId());
         std::vector<UtScriptData>* returnedPartsPtr = new std::vector<UtScriptData>();
         for (size_t j = 0; j != allParts.size(); ++j)
         {
            WsfPlatformPart* partPtr = allParts[j];
            if (partPtr->IsA_TypeOf(typeName))
            {
               returnedPartsPtr->push_back(UtScriptData(new UtScriptRef(partPtr, classPtr, UtScriptRef::cDONT_MANAGE)));
            }
         }
         aReturnVal.SetPointer(new UtScriptRef(returnedPartsPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
      }
   }
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerUtilsScriptClass, ScenarioAnalyzerUtils, InternallyLinkedPlatformParts, 2, "Array<WsfPlatformPart>", "WsfPlatformPart, string")
{
   WsfPlatformPart* originPtr = static_cast<WsfPlatformPart*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::string typeName = aVarArgs[1].GetString();

   std::vector<WsfPlatformPart*> internallyLinkedParts = getInternallyLinkedPlatformParts<WsfPlatformPart>(originPtr);

   UtScriptClass* classPtr = aContext.GetTypes()->GetClass(aReturnClassPtr->GetContainerDataTypeId());
   std::vector<UtScriptData>* returnedPartsPtr = new std::vector<UtScriptData>();
   for (size_t i = 0, partCount = internallyLinkedParts.size(); i != partCount; ++i)
   {
      WsfPlatformPart* partPtr = internallyLinkedParts[i];
      if (partPtr->IsA_TypeOf(typeName))
      {
         returnedPartsPtr->push_back(UtScriptData(new UtScriptRef(partPtr, classPtr, UtScriptRef::cDONT_MANAGE)));
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(returnedPartsPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerUtilsScriptClass, ScenarioAnalyzerUtils, LinkedAndReachablePlatformParts, 2, "Array<WsfPlatformPart>", "WsfPlatformPart, string")
{
   WsfPlatformPart* originPtr = static_cast<WsfPlatformPart*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::string typeName = aVarArgs[1].GetString();

   std::vector<WsfPlatformPart*> allPlatformParts = getLinkedAndReachablePlatformParts<WsfPlatformPart>(originPtr);

   UtScriptClass* classPtr = aContext.GetTypes()->GetClass(aReturnClassPtr->GetContainerDataTypeId());
   std::vector<UtScriptData>* returnedPartsPtr = new std::vector<UtScriptData>();
   for (size_t i = 0, partCount = allPlatformParts.size(); i != partCount; ++i)
   {
      WsfPlatformPart* partPtr = allPlatformParts[i];
      if (partPtr->IsA_TypeOf(typeName))
      {
         returnedPartsPtr->push_back(UtScriptData(new UtScriptRef(partPtr, classPtr, UtScriptRef::cDONT_MANAGE)));
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(returnedPartsPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerUtilsScriptClass, ScenarioAnalyzerUtils, LinkedAndReachablePlatformPartsChooseProcs,
                        4, "Array<WsfPlatformPart>", "WsfPlatformPart, string, Array<string>, bool")
{
   WsfPlatformPart* originPtr = static_cast<WsfPlatformPart*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::string typeName = aVarArgs[1].GetString();
   std::vector<UtScriptData>* procTypeListPtr = (std::vector<UtScriptData>*)aVarArgs[2].GetPointer()->GetAppObject();
   bool follow = aVarArgs[3].GetBool();

   std::vector<std::string> procTypes;
   for (size_t i = 0; i != procTypeListPtr->size(); ++i)
   {
      procTypes.push_back((*procTypeListPtr)[i].GetString());
   }

   std::vector<WsfPlatformPart*> allPlatformParts = getLinkedAndReachablePlatformParts<WsfPlatformPart>(originPtr, procTypes, follow);

   UtScriptClass* classPtr = aContext.GetTypes()->GetClass(aReturnClassPtr->GetContainerDataTypeId());
   std::vector<UtScriptData>* returnedPartsPtr = new std::vector<UtScriptData>();
   for (size_t i = 0, partCount = allPlatformParts.size(); i != partCount; ++i)
   {
      WsfPlatformPart* partPtr = allPlatformParts[i];
      if (partPtr->IsA_TypeOf(typeName))
      {
         returnedPartsPtr->push_back(UtScriptData(new UtScriptRef(partPtr, classPtr, UtScriptRef::cDONT_MANAGE)));
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(returnedPartsPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// Convert a list of ScenarioAnalyzerMessages to Array<ScenarioAnalyzerMessage>
void ConvertMessagesToScript(const Messages& aMessages, UtScriptData& aReturnVal, UtScriptContext& aContext, UtScriptClass* aReturnClassPtr)
{
   static UtScriptClass* groupClassPtr = aContext.GetTypes()->GetClass(ScenarioAnalyzerMessage::cSCRIPT_CLASS_NAME);
   auto tmpListPtr = new std::vector<UtScriptData>();

   for (const auto& message : aMessages)
   {
      UtScriptData scriptMessage(UtScriptRef::Ref(new ScenarioAnalyzerMessage(message), groupClassPtr, UtScriptRef::cMANAGE));
      tmpListPtr->push_back(scriptMessage);
   }
   aReturnVal.SetPointer(new UtScriptRef(tmpListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}


// Called when ScenarioAnalyzer plug-in is registered
void RegisterUtilsScriptTypes(WsfApplication& application)
{
   UtScriptTypes* scriptTypes = application.GetScriptTypes();
   scriptTypes->Register(ut::make_unique<ScenarioAnalyzerUtilsScriptClass>(scriptTypes));
}
