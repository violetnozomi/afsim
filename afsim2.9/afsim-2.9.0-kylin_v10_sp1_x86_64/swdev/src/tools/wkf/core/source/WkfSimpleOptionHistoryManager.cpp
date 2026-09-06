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

#include "WkfSimpleOptionHistoryManager.hpp"

#include <QSettings>

#include "WkfEnvironment.hpp"
#include "WkfPlatform.hpp"
#include "WkfPlatformGrouping.hpp"
#include "WkfScenario.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::SimpleOptionHistoryManager::SimpleOptionHistoryManager(const QString& aPluginName, QObject* aParentPtr)
   : OptionHistoryManager(aPluginName, aParentPtr)
{
}

// when something is changed on the platform options menu, this will record it as a rule to be applied
// to any entities that get added later.
void wkf::SimpleOptionHistoryManager::UpdateOptionsRulesList(int aOptionId, bool aState, const wkf::PlatformGrouping& aGrouping)
{
   // setting options at a global level negates everything
   if (aGrouping.IsGlobal())
   {
      mOptionRules[aOptionId].clear();
   }
   // add the rule
   mOptionRules[aOptionId].push_front(Rule(aGrouping, aState));
}

// when a platform is added, this walks through the rules for this plugin's options to ensure that the platform
// options are applied consistently based on user actions up to this point
void wkf::SimpleOptionHistoryManager::ApplyOptions(const PlatformList& aPlatformList)
{
   for (const auto& platform : aPlatformList)
   {
      for (auto&& it : mOptionRules)
      {
         for (auto&& jt : it.second)
         {
            if ((jt.first.Type() == wkf::PlatformGrouping::eIndividual) && jt.first.Name() == platform->GetName())
            {
               // apply the option and stop traversing rules for this option
               emit SetPlatformOptionState(it.first, jt.second, platform);
               break;
            }
            else
            {
               const auto catmap = wkfEnv.GetGroupings().find(jt.first);
               // if the grouping described in the rule exists in the scenario
               if (catmap != wkfEnv.GetGroupings().end())
               {
                  // if our platform is a member of the grouping
                  if (catmap->second.find(platform->GetIndex()) != catmap->second.end())
                  {
                     // apply the option and stop traversing rules for this option
                     emit SetPlatformOptionState(it.first, jt.second, platform);
                     break;
                  }
               }
            }
         }
      }
   }
}

// save the rule set to settings
void wkf::SimpleOptionHistoryManager::Save(QSettings& aSettings)
{
   aSettings.beginGroup("PlatformOptions");
   aSettings.beginGroup(mPluginName);
   std::set<QString> visitedItem;

   for (auto&& it : mOptionRules)
   {
      std::set<wkf::PlatformGrouping> visited;
      aSettings.beginWriteArray(mIdToString[it.first]);
      visitedItem.insert(mIdToString[it.first]);
      int i = 0;
      for (auto&& jt : it.second)
      {
         // once you have toggled on a specific grouping, all following actions on that grouping are irrelevant.
         // so ensure we haven't visited this before we write to file
         if (visited.find(jt.first) == visited.end())
         {
            visited.insert(jt.first);
            aSettings.setArrayIndex(i);
            aSettings.setValue("groupingType", jt.first.Type());
            aSettings.setValue("groupingName", QString(jt.first.Name().c_str()));
            aSettings.setValue("state", jt.second);
            i++;
         }
      }
      aSettings.endArray();
   }
   for (auto&& it : mIdToString)
   {
      if (visitedItem.find(it.second) == visitedItem.end())
      {
         aSettings.remove(it.second);
      }
   }
   aSettings.endGroup();
   aSettings.endGroup();
}


void wkf::SimpleOptionHistoryManager::Load(QSettings& aSettings)
{
   mOptionRules.clear();
   emit ResetOptionStates();
   aSettings.beginGroup("PlatformOptions");
   aSettings.beginGroup(mPluginName);
   for (auto&& it : mIdToString)
   {
      ApplyDefaultOptions(aSettings, it);
      ReadRulesArray(aSettings, it);
   }
   aSettings.endGroup();
   aSettings.endGroup();
}

void wkf::SimpleOptionHistoryManager::ApplyDefaultOptions(QSettings& aSettings, const std::pair<const int, QString>& aIdName)
{
   // If no rules for an option are saved in settings and a default flag has been specified, set the default true
   if (mDefaultOptions.count(aIdName.second) > 0)
   {
      int rules = aSettings.beginReadArray(aIdName.second);
      aSettings.endArray();
      if (rules == 0)
      {
         UpdateOptionsRulesList(aIdName.first, true, PlatformGrouping());
         return;
      }
   }
}

void wkf::SimpleOptionHistoryManager::ReadRulesArray(QSettings& aSettings, const std::pair<const int, QString>& aPair)
{
   std::set<int> visitedSet;
   auto          catmap = wkfEnv.GetGroupings();
   int           rules  = aSettings.beginReadArray(aPair.second);
   for (int i = 0; i < rules; ++i)
   {
      aSettings.setArrayIndex(i);
      int         type  = aSettings.value("groupingType").toInt();
      std::string name  = aSettings.value("groupingName").toString().toStdString();
      bool        state = aSettings.value("state").toBool();

      mOptionRules[aPair.first].emplace_back(wkf::PlatformGrouping(type, name), state);
      if (vaEnv.GetStandardScenario())
      {
         auto catIt = catmap.find(wkf::PlatformGrouping(type, name));
         if (catIt != catmap.end())
         {
            for (auto&& jt : catIt->second)
            {
               if (visitedSet.find(jt) == visitedSet.end())
               {
                  visitedSet.insert(jt);
                  wkf::Platform* platformPtr = vaEnv.GetStandardScenario()->FindPlatformByIndex(jt);
                  if (platformPtr)
                  {
                     emit SetPlatformOptionState(aPair.first, state, platformPtr);
                  }
               }
            }
         }
         else if (type == wkf::PlatformGrouping::eIndividual)
         {
            wkf::Platform* platformPtr = vaEnv.GetStandardScenario()->FindPlatformByName(name);
            if (platformPtr)
            {
               int idx = platformPtr->GetIndex();
               if (visitedSet.find(idx) == visitedSet.end())
               {
                  visitedSet.insert(idx);
                  emit SetPlatformOptionState(aPair.first, state, platformPtr);
               }
            }
         }
      }
   }
   aSettings.endArray();
}

void wkf::SimpleOptionHistoryManager::Clear()
{
   for (auto&& it : mIdToString)
   {
      mOptionRules[it.first].clear();

      auto catmap = wkfEnv.GetGroupings();
      auto catIt  = catmap.find(wkf::PlatformGrouping());
      if (catIt != catmap.end())
      {
         auto* scenarioPtr = vaEnv.GetStandardScenario();
         if (scenarioPtr)
         {
            for (auto&& jt : catIt->second)
            {
               wkf::Platform* platformPtr = scenarioPtr->FindPlatformByIndex(jt);
               if (platformPtr)
               {
                  emit SetPlatformOptionState(it.first, false, platformPtr);
               }
            }
         }
      }
   }
}
