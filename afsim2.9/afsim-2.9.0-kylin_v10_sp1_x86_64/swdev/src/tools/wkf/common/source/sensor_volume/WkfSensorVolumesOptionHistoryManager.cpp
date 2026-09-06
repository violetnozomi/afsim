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

#include "WkfSensorVolumesOptionHistoryManager.hpp"

#include "WkfEnvironment.hpp"
#include "WkfPlatform.hpp"
#include "WkfScenario.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::SensorVolumesOptionHistoryManager::SensorVolumesOptionHistoryManager(const QString& aPluginName, QObject* aParent)
   : wkf::OptionHistoryManager(aPluginName, aParent)
{
}

// add a new rule
void wkf::SensorVolumesOptionHistoryManager::UpdateOptionsRulesList(int                          aOptionId,
                                                                    bool                         aState,
                                                                    const wkf::PlatformGrouping& aGrouping)
{
   int p = GetParentOption(aOptionId);

   if (aGrouping.IsGlobal())
   {
      mOptionRules[p].clear();
   }
   mOptionRules[p].push_front(Rule(aGrouping, std::make_pair(aOptionId, aState)));
}

// apply rules to a platform
void wkf::SensorVolumesOptionHistoryManager::ApplyOptions(const wkf::PlatformList& aPlatforms)
{
   for (const auto& platform : aPlatforms)
   {
      for (const auto& kt : mOptionRules)
      {
         int                 p     = kt.first;
         bool                found = false;
         std::map<int, bool> stateMap;
         for (const auto& it : kt.second)
         {
            if ((it.first.Type() == wkf::PlatformGrouping::eIndividual) && it.first.Name() == platform->GetName())
            {
               // apply the option and stop traversing rules for this option
               if (it.second.first != p)
               {
                  if (stateMap.find(it.second.first) == stateMap.end())
                  {
                     stateMap[it.second.first] = it.second.second;
                  }
                  // store it off
               }
               else
               {
                  found = true;
                  emit SetPlatformOptionState(p, it.second.second, platform);
                  for (auto& jt : stateMap)
                  {
                     if (jt.second != it.second.second)
                     {
                        emit SetPlatformOptionState(jt.first, jt.second, platform);
                     }
                  }
                  // apply the change
                  break;
               }
            }
            else
            {
               const auto catmap = wkfEnv.GetGroupings().find(it.first);
               // if the grouping described in the rule exists in the scenario
               if (catmap != wkfEnv.GetGroupings().end())
               {
                  // if our platform is a member of the grouping
                  if (catmap->second.find(platform->GetIndex()) != catmap->second.end())
                  {
                     // apply the option and stop traversing rules for this option
                     if (it.second.first != p)
                     {
                        if (stateMap.find(it.second.first) == stateMap.end())
                        {
                           stateMap[it.second.first] = it.second.second;
                        }
                     }
                     else
                     {
                        found = true;
                        emit SetPlatformOptionState(p, it.second.second, platform);
                        for (auto& jt : stateMap)
                        {
                           if (jt.second != it.second.second)
                           {
                              emit SetPlatformOptionState(jt.first, jt.second, platform);
                           }
                        }
                        // apply the change
                        break;
                     }
                  }
               }
            }
         }
         if (!found)
         {
            emit SetPlatformOptionState(p, false, platform);
            for (auto& jt : stateMap)
            {
               if (jt.second)
               {
                  emit SetPlatformOptionState(jt.first, jt.second, platform);
               }
            }
         }
      }
   }
}

// clear out all rules
void wkf::SensorVolumesOptionHistoryManager::Clear()
{
   mOptionRules.clear();
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
               for (auto& it : mParents)
               {
                  emit SetPlatformOptionState(it, false, platformPtr);
               }
            }
         }
      }
   }
}

// save rules
void wkf::SensorVolumesOptionHistoryManager::Save(QSettings& aSettings)
{
   aSettings.beginGroup("PlatformOptions");
   aSettings.beginGroup(mPluginName);

   std::set<std::pair<int, wkf::PlatformGrouping>> visited;

   for (auto&& it : mOptionRules)
   {
      int i = 0;
      aSettings.remove(OptionName(it.first));
      aSettings.beginWriteArray(OptionName(it.first));
      for (auto& jt : it.second)
      {
         // once you have toggled on a specific grouping, all following actions on that grouping are irrelevant.
         // so ensure we haven't visited this before we write to file
         auto ruleId = std::make_pair(jt.second.first, jt.first);
         if (visited.find(ruleId) == visited.end())
         {
            visited.insert(ruleId);

            aSettings.setArrayIndex(i);
            aSettings.setValue("rule", OptionName(jt.second.first));
            aSettings.setValue("groupingType", jt.first.Type());
            aSettings.setValue("groupingName", QString(jt.first.Name().c_str()));
            aSettings.setValue("state", jt.second.second);
            i++;
         }
      }
      aSettings.endArray();
   }
   aSettings.endGroup();
   aSettings.endGroup();
}

// load rules
void wkf::SensorVolumesOptionHistoryManager::Load(QSettings& aSettings)
{
   mOptionRules.clear();
   emit ResetOptionStates();
   aSettings.beginGroup("PlatformOptions");
   aSettings.beginGroup(mPluginName);

   for (auto& it : mParents)
   {
      int rules = aSettings.beginReadArray(OptionName(it));
      LoadRules(rules, it, aSettings);
      aSettings.endArray();
   }

   aSettings.endGroup();
   aSettings.endGroup();
}

void wkf::SensorVolumesOptionHistoryManager::LoadRules(int aNumber, int aParent, QSettings& aSettings)
{
   // Values read first in the config are the most recently updated, so these sets are used to skip information about
   // sensors already read in
   std::set<int> visitedPlatforms; // Set of Platform Ids, used to skip additional updates to the "Sensor Volumes"
                                   // platform-wide checkbox
   std::set<std::pair<int, int>> visitedSensors; // Set of Platform Ids and Sensor names, used to skip additional
                                                 // updates to sensor-specific checkboxes

   std::map<int, std::map<int, bool>> sensorMap;
   auto                               catmap = wkfEnv.GetGroupings();
   for (int i = 0; i < aNumber; ++i)
   {
      aSettings.setArrayIndex(i);
      QString     ruleId = aSettings.value("rule").toString();
      int         type   = aSettings.value("groupingType").toInt();
      std::string name   = aSettings.value("groupingName").toString().toStdString();
      bool        state  = aSettings.value("state").toBool();

      int         ruleType = aParent;
      const auto& it       = mStringToId.find(ruleId);
      if (it == mStringToId.end())
      {
         ruleType = AddOption(ruleId, aParent);
      }
      else
      {
         ruleType = it->second;
      }
      mOptionRules[aParent].emplace_back(wkf::PlatformGrouping(type, name), std::make_pair(ruleType, state));
      auto* scenarioPtr = vaEnv.GetStandardScenario();
      if (scenarioPtr)
      {
         auto catIt = catmap.find(wkf::PlatformGrouping(type, name));
         if (catIt != catmap.end())
         {
            for (auto&& jt : catIt->second)
            {
               if (ruleType == aParent && visitedPlatforms.find(jt) == visitedPlatforms.end())
               {
                  visitedPlatforms.insert(jt);

                  wkf::Platform* platformPtr = scenarioPtr->FindPlatformByIndex(jt);
                  if (platformPtr)
                  {
                     emit SetPlatformOptionState(ruleType, state, platformPtr);
                     for (auto& kt : sensorMap[jt])
                     {
                        if (kt.second != state)
                        {
                           emit SetPlatformOptionState(kt.first, kt.second, platformPtr);
                        }
                     }
                  }
               }
               else if (ruleType != aParent && visitedSensors.find(std::make_pair(jt, ruleType)) == visitedSensors.end())
               {
                  visitedSensors.insert(std::make_pair(jt, ruleType));
                  sensorMap[jt][ruleType] = state;
               }
            }
         }
         else if (type == wkf::PlatformGrouping::eIndividual)
         {
            wkf::Platform* platformPtr = scenarioPtr->FindPlatformByName(name);
            if (platformPtr)
            {
               int idx = platformPtr->GetIndex();

               // If the current checkbox is platform-wide ("Sensor Volumes"), and the platform-wide box has not been read
               if (ruleType == aParent && visitedPlatforms.find(idx) == visitedPlatforms.end())
               {
                  visitedPlatforms.insert(idx);

                  // Change volume visibilities of all sensors
                  emit SetPlatformOptionState(ruleType, state, platformPtr);

                  // Resets any sensors specified before to what they were, since the sensor-specific checkboxes are
                  // higher priority
                  for (auto& kt : sensorMap[idx])
                  {
                     if (kt.second != state)
                     {
                        emit SetPlatformOptionState(kt.first, kt.second, platformPtr);
                     }
                  }
               }

               // If the current checkbox is for a specific sensor, and the sensor-specific box has not been read
               else if (ruleType != aParent && visitedSensors.find(std::make_pair(idx, ruleType)) == visitedSensors.end())
               {
                  // Directly change the visibility of the sensor volume, since this is the highest-priority checkbox
                  visitedSensors.insert(std::make_pair(idx, ruleType));
                  sensorMap[idx][ruleType] = state;
                  emit SetPlatformOptionState(ruleType, state, platformPtr);
               }
            }
         }
      }
   }
}

void wkf::SensorVolumesOptionHistoryManager::SetParentOption(int aType, int aParent)
{
   if (aParent != -1)
   {
      assert(mParents.find(aType) ==
             mParents.end()); // this structure only supports 1 level of depth in parent child relationships
                              // if you are a parent you can't be a child, and vice versa
      mParent[aType] = aParent;
   }
   else
   {
      mParents.insert(aType);
   }
}

int wkf::SensorVolumesOptionHistoryManager::GetParentOption(int aType) const
{
   const auto& it = mParent.find(aType);
   if (it != mParent.end())
   {
      return it->second;
   }
   else
   {
      return aType;
   }
}
