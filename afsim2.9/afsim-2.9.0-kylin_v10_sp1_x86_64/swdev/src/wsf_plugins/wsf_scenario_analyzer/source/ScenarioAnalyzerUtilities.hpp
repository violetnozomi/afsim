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
*   File: ScenarioAnalyzerUtilities.hpp
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

#ifndef SCENARIO_ANALYZER_UTILITIES_HPP
#define SCENARIO_ANALYZER_UTILITIES_HPP

#include "wsf_scenario_analyzer_lib_export.h"

#include <functional>
#include <string>
#include <vector>

#include "WsfCommNetworkManager.hpp"
#include "WsfCommUtil.hpp"
#include "WsfCommXmtrRcvr.hpp"
#include "WsfCommandChain.hpp"
#include "WsfExternalLinks.hpp"
#include "WsfGroupManager.hpp"
#include "WsfInternalLinks.hpp"
#include "WsfLinkedProcessor.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformPart.hpp"
#include "WsfProcessor.hpp"
#include "script/WsfScriptProcessor.hpp"
#include "WsfStringId.hpp"
#include "ScenarioFileLocation.hpp"
#include "ScenarioAnalyzerMessage.hpp"

class WsfApplication;
class WsfBattleManager;
class WsfBMSensorsManager;
namespace wsf
{
namespace comm
{
class Comm;
}
}


// plug-in interface
void RegisterUtilsScriptTypes(WsfApplication& application);

using PlatformCheck = std::function<bool(WsfPlatform*)>;
using Messages = std::vector<ScenarioAnalyzerMessage>;

// Templated function for running a check.
// Expects a function pointer that will add any error/warning messages to "Messages"
// and return true/false if there were no errors/warnings.
//
// Should be called by all check functions;
template<typename TFunc>
WSF_SCENARIO_ANALYZER_LIB_EXPORT Messages runSuiteCheck(const std::string& aSuiteName,
                                                        const std::string& aCheckName,
                                                        ScenarioAnalyzerMessage::SeverityLevel aSeverity,
                                                        WsfSimulation& aSimulation,
                                                        TFunc aFunction,
                                                        bool aWriteFlag = true)
{
   Messages messages;

   ScenarioAnalyzerMessage message;
   message.SetSuiteName(aSuiteName);
   message.SetCheckName(aCheckName);
   message.SetSeverity(aSeverity);

   if (aFunction(aSimulation, messages, message))
   {
      message.SetSuccessful();
      messages.push_back(message);
   }

   if (aWriteFlag)
   {
      for (const auto& m : messages)
      {
         m.Write();
      }
   }

   return messages;
}

// Utilities for identifying platforms and platforms parts linked in various ways to a source platform or platform part
template<typename T>
std::vector<T*> getInternallyLinkedPlatformParts(WsfPlatformPart* origin);
template<typename T>
std::vector<T*> getInternallyLinkedPlatformParts(WsfPlatformPart* origin, std::vector<WsfPlatformPart*>& visited);

template<typename T>
extern std::vector<T*> getLinkedAndReachablePlatformParts(WsfPlatformPart* origin, const std::vector<std::string>& ignoreExternalLinks, bool follow);
template<typename T>
extern std::vector<T*> getLinkedAndReachablePlatformParts(WsfPlatformPart* origin);

template<typename T>
extern std::vector<T*> getLinkedAndReachablePlatformParts(
   WsfPlatformPart* origin,
   std::vector<WsfPlatform*>& visitedPlatforms,
   std::vector<wsf::comm::Comm*>& visitedComms,
   const std::vector<std::string>& ignoreOrFollowExternalLinks,
   bool follow);

template<typename T>
extern std::vector<T*> getLinkedAndReachablePlatformParts(
   wsf::ExternalLinks& links,
   WsfPlatform* platform,
   std::vector<WsfPlatform*>& visitedPlatforms,
   std::vector<wsf::comm::Comm*>& visitedComms,
   const std::vector<std::string>& ignoreOrFollowExternalLinks,
   bool follow);


// Utilities for navigating and performing checks on command chains
WSF_SCENARIO_ANALYZER_LIB_EXPORT WsfPlatform* getTopCommander(WsfStringId commandChainName, WsfPlatform* platform);
WSF_SCENARIO_ANALYZER_LIB_EXPORT bool checkUpCommandChain(WsfPlatform* platform, PlatformCheck check);
WSF_SCENARIO_ANALYZER_LIB_EXPORT bool checkUpCommandChain(WsfPlatform* platform, PlatformCheck check, WsfStringId commandChainName);
WSF_SCENARIO_ANALYZER_LIB_EXPORT bool checkDownCommandChain(WsfPlatform* platform, PlatformCheck check);
WSF_SCENARIO_ANALYZER_LIB_EXPORT bool checkDownCommandChain(WsfPlatform* platform, PlatformCheck check, WsfStringId commandChainName);
WSF_SCENARIO_ANALYZER_LIB_EXPORT bool checkFullCommandChain(WsfPlatform* platform, PlatformCheck check);

// Convert a list of ScenarioAnalyzerMessages to Array<ScenarioAnalyzerMessage>
WSF_SCENARIO_ANALYZER_LIB_EXPORT void ConvertMessagesToScript(const Messages& aMessages,
                                                              UtScriptData& aReturnVal,
                                                              UtScriptContext& aContext,
                                                              UtScriptClass* aReturnClassPtr);

template<typename T>
std::vector<T*> getAllPartsDownCommandChain(WsfPlatform* platform);
template<typename T>
std::vector<T*> getAllPartsDownCommandChain(WsfPlatform* platform, WsfStringId commandChainName);

// Returns a vector of pointers to all WsfPlatformParts of type T that are (1) on the same platform as platformPart
// (2) are connected to platformPart by an internal link coming from platformPart.
template<typename T>
std::vector<T*> getInternallyLinkedPlatformParts(WsfPlatformPart* platformPart)
{
   std::vector<WsfPlatformPart*> visited;
   return getInternallyLinkedPlatformParts<T>(platformPart, visited);
}

// Helper function for getInternallyLinkedPlatformParts(WsfPlatformPart*)
template<typename T>
std::vector<T*> getInternallyLinkedPlatformParts(WsfPlatformPart* platformPart, std::vector<WsfPlatformPart*>& visited)
{
   std::vector<T*> linkedPlatformParts;
   visited.push_back(platformPart);

   WsfInternalLinks& internalLinks = platformPart->GetInternalLinks();
   unsigned internalLinksCount = internalLinks.GetRecipientCount();
   for (unsigned int i = 0; i != internalLinksCount; ++i)
   {
      WsfPlatformPart* recipient = internalLinks.GetRecipientEntry(i);
      // Don't check or recurse on this recipient if it has already been visited.
      if (recipient && find(visited.begin(), visited.end(), recipient) == visited.end())
      {
         auto tPtr = dynamic_cast<T*>(recipient);
         if (tPtr != nullptr)
         {
            linkedPlatformParts.push_back(tPtr);
         }
         // Whether or not recipient matches the specified type, recursively check its internal links
         std::vector<T*> indirectlyLinked = getInternallyLinkedPlatformParts<T>(recipient, visited);
         linkedPlatformParts.insert(linkedPlatformParts.end(), indirectlyLinked.begin(), indirectlyLinked.end());
      }
   }
   return linkedPlatformParts;
}

// Returns a vector of pointers to all platform parts of type T on platform or any of its subordinates on any command chain.
template<typename T>
std::vector<T*> getAllPartsDownCommandChain(WsfPlatform* platform)
{
   std::vector<T*> results;
   // First, check for any parts of type T on the origin platform that pass the check
   unsigned partCount = platform->GetComponentCount<WsfPlatformPart>();
   for (unsigned i = 0; i != partCount; ++i)
   {
      auto part = dynamic_cast<T*>(platform->GetComponentEntry<WsfPlatformPart>(i));
      if (part != nullptr)
      {
         results.push_back(part);
      }
   }

   // Then, check subordiantes
   unsigned chainCount = platform->GetComponentCount<WsfCommandChain>();
   for (unsigned j = 0; j < chainCount; ++j)
   {
      std::vector<WsfPlatform*> subordinates = platform->GetComponentEntry<WsfCommandChain>(j)->GetSubordinates();
      for (auto s : subordinates)
      {
         if (s != nullptr)
         {
            std::vector<T*> subResults = getAllPartsDownCommandChain<T>(s);
            results.insert(results.end(), subResults.begin(), subResults.end());
         }
      }
   }

   std::sort(results.begin(), results.end(), [](T * a, T * b)
   {
      return (a->GetPlatform()->GetNameId() < b->GetPlatform()->GetNameId()) ||
             (a->GetPlatform()->GetNameId() == b->GetPlatform()->GetNameId() && a->GetNameId() < b->GetNameId());
   });
   typename std::vector<T*>::iterator end = std::unique(results.begin(), results.end(), [](T * a, T * b) { return a->GetNameId() == b->GetNameId(); });
   std::vector<T*> uniqueResults(results.begin(), end);
   return uniqueResults;
}

// Returns a vector of pointers to all platform parts of type T on platform or any of its subordinates on the specified command chain.
template<typename T>
std::vector<T*> getAllPartsDownCommandChain(WsfPlatform* platform, WsfStringId commandChainName)
{
   std::vector<T*> results;
   // First, check for any parts of type T on the origin platform that pass the check
   unsigned partCount = platform->GetComponentCount<WsfPlatformPart>();
   for (unsigned i = 0; i != partCount; ++i)
   {
      auto part = dynamic_cast<T*>(platform->GetComponentEntry<WsfPlatformPart>(i));
      if (part != nullptr)
      {
         results.push_back(part);
      }
   }

   // Then, check subordiantes
   WsfCommandChain* commandChain = platform->GetComponent<WsfCommandChain>(commandChainName);
   if (commandChain)
   {
      std::vector<WsfPlatform*> subordinates = commandChain->GetSubordinates();
      for (auto s : subordinates)
      {
         if (s != nullptr)
         {
            std::vector<T*> subResults = getAllPartsDownCommandChain<T>(s, commandChainName);
            results.insert(results.end(), subResults.begin(), subResults.end());
         }
      }
   }
   return results;
}

template<typename T>
std::vector<T*> getLinkedAndReachablePlatformParts(WsfPlatformPart* origin)
{
   std::vector<std::string> ignoreOrFollowExternalLinks;
   return getLinkedAndReachablePlatformParts<T>(origin, ignoreOrFollowExternalLinks, false);
}


template<typename T>
std::vector<T*> getLinkedAndReachablePlatformParts(WsfPlatformPart* origin, const std::vector<std::string>& ignoreOrFollowExternalLinks, bool follow)
{
   std::vector<WsfPlatform*> visitedPlatforms;
   std::vector<wsf::comm::Comm*> visitedComms;
   return getLinkedAndReachablePlatformParts<T>(origin, visitedPlatforms, visitedComms, ignoreOrFollowExternalLinks, follow);
}


template<typename T>
std::vector<T*> getLinkedAndReachablePlatformParts(
   WsfPlatformPart* origin,
   std::vector<WsfPlatform*>& visitedPlatforms,
   std::vector<wsf::comm::Comm*>& visitedComms,
   const std::vector<std::string>& ignoreOrFollowExternalLinks,
   bool follow)
{
   WsfPlatform* platform = origin->GetPlatform();
   if (std::find(visitedPlatforms.begin(), visitedPlatforms.end(), platform) == visitedPlatforms.end() &&
       std::find(visitedComms.begin(), visitedComms.end(), origin) == visitedComms.end())
   {
      // For now, we're not going to keep track of visited platforms - keeping track of visited comms is enough to avoid
      // getting stuck in cycles, and occasionally we may need to visit a platform more than once to find a linked
      // and reachable platform part. We may want to change this back eventually.
      //visitedPlatforms.push_back(platform);

      // Start with platform parts of type T to which origin has internal links
      std::vector<T*> linkedAndReachableParts = getInternallyLinkedPlatformParts<T>(origin);

      // If origin itself has external links, follow those links, even if origin is of a type listed in ignoreExternalLinks.
      if (dynamic_cast<WsfLinkedProcessor*>(origin))
      {
         wsf::ExternalLinks& links = static_cast<WsfLinkedProcessor*>(origin)->GetExternalLinks();
         std::vector<T*> results = getLinkedAndReachablePlatformParts<T>(links, origin->GetPlatform(), visitedPlatforms, visitedComms, ignoreOrFollowExternalLinks, follow);
         linkedAndReachableParts.reserve(linkedAndReachableParts.size() + results.size());
         linkedAndReachableParts.insert(linkedAndReachableParts.end(), results.begin(), results.end());
      }
      else if (dynamic_cast<WsfScriptProcessor*>(origin))
      {
         wsf::ExternalLinks& links = static_cast<WsfScriptProcessor*>(origin)->GetExternalLinks();
         std::vector<T*> results = getLinkedAndReachablePlatformParts<T>(links, origin->GetPlatform(), visitedPlatforms, visitedComms, ignoreOrFollowExternalLinks, follow);
         linkedAndReachableParts.reserve(linkedAndReachableParts.size() + results.size());
         linkedAndReachableParts.insert(linkedAndReachableParts.end(), results.begin(), results.end());
      }

      // Linked processors and script processors may have external links used to communicate with other platforms:
      // Find all sets of external links that origin has access to via internal links. (E.g., origin has an internal
      // link to a WsfLinkedProcessor, which has external links to other platforms.) Ignore external links from
      // script processors or linked processors of a type named in ignoreExternalLinks.
      std::vector<WsfLinkedProcessor*> linkedProcessors = getInternallyLinkedPlatformParts<WsfLinkedProcessor>(origin);
      std::vector<WsfScriptProcessor*> scriptProcessors = getInternallyLinkedPlatformParts<WsfScriptProcessor>(origin);

      // If follow == true, ONLY follow external links from processor types listed in ignoreOrFollowExternalLinks.
      // So, assume that we will NOT consider a processor type unless we see it in ignoreOrFollowExternalLinks.
      // If follow == false, we'll follow external links from all processor types EXCEPT those listed in ignoreOrFollowExternalLinks.
      // So, assume that we WILL consider a processor type unless we see it in the vector of type names.
      for (WsfLinkedProcessor* lp : linkedProcessors)
      {
         bool considerLP = !follow;
         for (const std::string& type : ignoreOrFollowExternalLinks)
         {
            if (lp->IsA_TypeOf(type))
            {
               considerLP = !considerLP;
               break;
            }
         }
         if (considerLP)
         {
            wsf::ExternalLinks& links = lp->GetExternalLinks();
            std::vector<T*> results = getLinkedAndReachablePlatformParts<T>(links, origin->GetPlatform(), visitedPlatforms, visitedComms, ignoreOrFollowExternalLinks, follow);
            linkedAndReachableParts.reserve(linkedAndReachableParts.size() + results.size());
            linkedAndReachableParts.insert(linkedAndReachableParts.end(), results.begin(), results.end());
         }
      }
      for (WsfScriptProcessor* sp : scriptProcessors)
      {
         bool considerSP = !follow;
         for (const std::string& type : ignoreOrFollowExternalLinks)
         {
            if (sp->IsA_TypeOf(type))
            {
               considerSP = !considerSP;
               break;
            }
         }
         if (considerSP)
         {
            wsf::ExternalLinks& links = sp->GetExternalLinks();
            std::vector<T*> results = getLinkedAndReachablePlatformParts<T>(links, origin->GetPlatform(), visitedPlatforms, visitedComms, ignoreOrFollowExternalLinks, follow);
            linkedAndReachableParts.reserve(linkedAndReachableParts.size() + results.size());
            linkedAndReachableParts.insert(linkedAndReachableParts.end(), results.begin(), results.end());
         }
      }
      return linkedAndReachableParts;
   }
   else // Platform has already been visited
   {
      return std::vector<T*>();
   }
}

template<typename T>
std::vector<T*> getLinkedAndReachablePlatformParts(
   wsf::ExternalLinks& links,
   WsfPlatform* sourcePlatform,
   std::vector<WsfPlatform*>& visitedPlatforms,
   std::vector<wsf::comm::Comm*>& visitedComms,
   const std::vector<std::string>& ignoreOrFollowExternalLinks,
   bool follow)
{
   std::vector<T*> linkedAndReachablePlatformParts;
   if (sourcePlatform)
   {
      wsf::comm::NetworkManager* networkManager = sourcePlatform->GetSimulation()->GetCommNetworkManager();

      for (size_t i = 0, count = links.GetLinkCount(); i != count; ++i)
      {
         wsf::ExternalLinks::ExternalLink& link = links.GetLink(i);
         wsf::comm::Comm* xmtr = link.GetXmtr();
         if (xmtr)
         {
            wsf::comm::Address xmtrAddress = link.GetXmtr()->GetAddress();

            //std::cout << "Source platform: " << sourcePlatform->GetName() << ", xmtr: " << link.GetXmtr()->GetName() << std::endl;

            std::vector<wsf::comm::Address> recipientAddresses = link.GetRecipients();
            for (const wsf::comm::Address& rcvrAddress : recipientAddresses)
            {
               //std::cout << "Checking for path to comm " << networkManager->GetComm(rcvrAddress)->GetName() << std::endl;
               if (networkManager->PathExists(xmtrAddress, rcvrAddress))
               {
                  wsf::comm::Comm* rcvr = networkManager->GetComm(rcvrAddress);
                  //std::cout << "Destination platform: " << rcvrComm->GetPlatform()->GetName() << ", rcvr: " << rcvrComm->GetName() << std::endl;
                  if (rcvr)
                  {
                     visitedComms.push_back(xmtr);
                     auto results = getLinkedAndReachablePlatformParts<T>(rcvr, visitedPlatforms, visitedComms, ignoreOrFollowExternalLinks, follow);
                     visitedComms.push_back(rcvr);
                     linkedAndReachablePlatformParts.reserve(linkedAndReachablePlatformParts.size() + results.size());
                     linkedAndReachablePlatformParts.insert(linkedAndReachablePlatformParts.end(), results.begin(), results.end());
                  }
               }
            }
         }
      }
   }
   return linkedAndReachablePlatformParts;
}



#endif