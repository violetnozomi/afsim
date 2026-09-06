// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "UtMemory.hpp"
#include "UtPlugin.hpp"
#include "UtScriptClassDefine.hpp"
#include "UtScriptData.hpp"
#include "UtScriptTypes.hpp"

#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfAssetManager.hpp"
#include "WsfBMDisseminateC2.hpp"
#include "WsfBMUtils.hpp"
#include "WsfBMSensorsManager.hpp"
#include "WsfBMSensorsManager.hpp"
#include "WsfBMUtils.hpp"
#include "WsfCommandChain.hpp"
#include "WsfDefaultSensorScheduler.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlugin.hpp"
#include "WsfRadarSensor.hpp"
#include "script/WsfScriptContext.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorsManagerFov.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackProcessor.hpp"
#include "WsfWeapon.hpp"
#include "WsfWeaponsManagerAI.hpp"

#include "iadsLib/util.hpp"
#include "ScenarioAnalyzerIADSC2.hpp"
#include "WsfIADSC2ScenarioExtension.hpp"

#include <string>
#include <vector>
#include "wsf/source/include/wsf_version_defines.hpp"

// Beginning with a sensor's platform, look up the default command chain until we find a platform with a
// sensors manager. The first one we hit manages that sensor. This function assumes that conflicting
// sensors managers do not appear above the sensor platform in the defaul command chain: a different check
// looks for that situation. If the sensor has no managing sensors manager, returns a nullptr.
WsfBMSensorsManager* getManagingSensorsManager(WsfSensor* sensor)
{
   WsfPlatform* current = sensor->GetPlatform();
   WsfPlatform* commander = current;
   while (current && commander)
   {
      WsfBMSensorsManager* SM = WsfBMUtils::FindAttachedSensorsManagerRef2(commander);
      if (SM)
      {
         return SM;
      }
      current = commander;
      commander = current->GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId())->GetCommander();
      if (commander && commander->GetNameId() == current->GetNameId())
      {
         break;
      }
   }
   return nullptr;
}

bool isC2CapableHelper(WsfPlatform* platform, ScenarioAnalyzerMessage& resultMessage)
{
   bool isC2Capable = true;

   bool hasTrackProcessor = false;
   unsigned processorCount = platform->GetComponentCount<WsfProcessor>();
   for (unsigned j = 0; j < processorCount; ++j)
   {
      if (dynamic_cast<WsfTrackProcessor*>(platform->GetComponentEntry<WsfProcessor>(j)))
      {
         hasTrackProcessor = true;
         break;
      }
   }
   bool hasDC2 = false;
   if (WsfBMUtils::FindAttachedDisseminationManagerRef2(platform))
   {
      hasDC2 = true;
   }
   if (!hasTrackProcessor || !hasDC2)
   {
      isC2Capable = false;

      resultMessage.AddDetails("This platform is missing ");
      if (!hasTrackProcessor && !hasDC2)
      {
         resultMessage.AddDetails("a track processor and a disseminate C2 manager.");
      }
      else if (!hasTrackProcessor)
      {
         resultMessage.AddDetails("a track processor.");
      }
      else
      {
         resultMessage.AddDetails("a disseminate C2 manager.");
      }
   }
   return isC2Capable;
}

// If platform deploys a Weapons Manager AI, return a pointer to it. Else, return nullptr.
WsfWeaponsManagerAI* findWMAI(WsfPlatform* platform)
{
   unsigned procCount = platform->GetComponentCount<WsfProcessor>();
   for (unsigned i = 0; i != procCount; ++i)
   {
      auto wmai = dynamic_cast<WsfWeaponsManagerAI*>(platform->GetComponentEntry<WsfProcessor>(i));
      if (wmai != nullptr)
      {
         return wmai;
      }
   }
   return nullptr;
}

namespace
{
static const std::string cSUITE_NAME = "IADS_C2";

bool checkAssetManagersHaveUpdateIntervalHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      const WsfPlatform* platform = const_cast<WsfPlatform*>(sim.GetPlatformEntry(i));
      WsfAssetManager* assetManager = WsfBMUtils::FindAttachedAssetManagerRef2(platform);
      if (assetManager && assetManager->GetUpdateInterval() <= 0)
      {
         passedCheck = false;

         message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
         message.AddScenarioFileLocation(ScenarioFileLocation("processors", assetManager->GetName()));
         message.SetDetails("Asset manager " + assetManager->GetName() + " on platform " + platform->GetName()
                            + " has an update interval <= 0, so it and the other IADS C2 components it managers will never be updated. "
                            + "Use the 'update_interval <time-reference>' command to set this value.");

         messages.push_back(message);
         message.ClearDetails();
      }
   }
   return passedCheck;
}

bool checkPlatformWithAssetManagerHasDisseminateC2ManagerHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      const WsfPlatform* platform = const_cast<WsfPlatform*>(sim.GetPlatformEntry(i));
      if (WsfBMUtils::FindAttachedAssetManagerRef2(platform) && !WsfBMUtils::FindAttachedDisseminationManagerRef2(platform))
      {
         passedCheck = false;

         message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
         message.SetDetails("Platform " + platform->GetName() +
                            " is deploying an asset manager, so it should also deploy a disseminate C2 manager.");
         messages.push_back(message);
         message.ClearDetails();
      }
   }
   return passedCheck;
}

bool checkBattleManagerConnectedToSubordinateSensorsManagerByC2CapablePlatformsHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   PlatformCheck isC2Capable = [](WsfPlatform * p)
   {
      if ((!WsfBMUtils::FindAttachedAssetManagerRef(p)) | (!WsfBMUtils::FindAttachedDisseminationManagerRef(p)))
      {
         return false;
      }
      return true;
   };

   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i != platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      auto bm = WsfBMUtils::FindAttachedBattleManagerRef(platform);
      if (bm && bm->getCommitAuthority()) // TODO: is commit authority necessary to trigger the check?
      {
         std::vector<WsfBMSensorsManager*> subordinateSMs = getAllPartsDownCommandChain<WsfBMSensorsManager>(platform, WsfCommandChain::GetDefaultName());
         // If the BM has no subordinate WMs, the BM has failed this check.
         if (subordinateSMs.empty())
         {
            passedCheck = false;

            message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
            message.SetDetails("The battle manager on platform " + platform->GetName() +
                               " has no sensors manager subordinate to it on the default command chain.");
            messages.push_back(message);
            message.ClearDetails();
         }
         // If the BM has subordinate WMs, check that each is connected to the BM via C2-capable platforms.
         else
         {
            for (auto sm : subordinateSMs)
            {
               // Store names of platforms btw (and including) WM and BM platforms that aren't "C2 capable."
               std::vector<std::string> nonC2PlatformNames;
               WsfPlatform* current = sm->GetPlatform();
               if (!isC2Capable(current))
               {
                  nonC2PlatformNames.emplace_back(current->GetName());
               }
               WsfPlatform* commander = current->GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId())->GetCommander();
               // Keep going until we get back to the BM platform or we hit the top of the command chain (which shouldn't happen)
               while (current && current->GetNameId() != platform->GetNameId() && current->GetNameId() != commander->GetNameId())
               {
                  if (!isC2Capable(commander))
                  {
                     nonC2PlatformNames.emplace_back(commander->GetName());
                  }
                  current = commander;
                  commander = commander->GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId())->GetCommander();
               }

               if (!nonC2PlatformNames.empty())
               {
                  passedCheck = false;

                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", sm->GetPlatform()->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
                  message.SetDetails("The sensors manager on " + sm->GetPlatform()->GetName() + " is subordinate to the battle manager on " +
                                     platform->GetName() + ", but the following platforms between the two on the default command chain are not C2-capable in this context: ");

                  size_t nameCount = nonC2PlatformNames.size();
                  for (size_t i = 0; i != nameCount; ++i)
                  {
                     message.AddDetails(nonC2PlatformNames[i]);
                     if (i != nameCount - 1)
                     {
                        message.AddDetails(", ");
                     }
                     message.AddScenarioFileLocation(ScenarioFileLocation("platform", nonC2PlatformNames[i]));
                  }

                  message.AddDetails(". To fix this, ensure that each of these platforms has an asset manager and a disseminate C2 manager.");
                  messages.push_back(message);
                  message.ClearDetails();
               }
            }
         }
      }
   }
   return passedCheck;
}

bool checkBattleManagerConnectedToSubordinateWeaponsManagerByC2CapablePlatformsHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   PlatformCheck isC2Capable = [](WsfPlatform * p)
   {
      auto am = WsfBMUtils::FindAttachedAssetManagerRef(p);
      if (!am || am->getMaxAssignments() == 0 || !WsfBMUtils::FindAttachedDisseminationManagerRef(p))
      {
         return false;
      }
      return true;
   };

   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i != platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      auto bm = WsfBMUtils::FindAttachedBattleManagerRef(platform);
      if (bm && bm->getCommitAuthority()) // TODO: is commit authority necessary to trigger the check?
      {
         std::vector<WsfBMWeaponsManager*> subordinateWMs = getAllPartsDownCommandChain<WsfBMWeaponsManager>(platform, WsfCommandChain::GetDefaultName());
         // If the BM has no subordinate WMs, the BM has failed this check.
         if (subordinateWMs.empty())
         {
            passedCheck = false;

            message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
            message.SetDetails("The battle manager on platform " + platform->GetName() +
                               " has no weapons manager subordinate to it on the default command chain.");
            messages.push_back(message);
            message.ClearDetails();
         }
         // If the BM has subordinate WMs, check that each is connected to the BM via C2-capable platforms.
         else
         {
            for (auto wm : subordinateWMs)
            {
               // Store names of platforms btw (and including) WM and BM platforms that aren't "C2 capable."
               std::vector<std::string> nonC2PlatformNames;
               WsfPlatform* current = wm->GetPlatform();
               if (!isC2Capable(current))
               {
                  nonC2PlatformNames.emplace_back(current->GetName());
               }
               WsfPlatform* commander = current->GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId())->GetCommander();
               // Keep going until we get back to the BM platform or we hit the top of the command chain (which shouldn't happen)
               while (current && current->GetNameId() != platform->GetNameId() && current->GetNameId() != commander->GetNameId())
               {
                  if (!isC2Capable(commander))
                  {
                     nonC2PlatformNames.emplace_back(commander->GetName());
                  }
                  current = commander;
                  commander = commander->GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId())->GetCommander();
               }

               if (!nonC2PlatformNames.empty())
               {
                  passedCheck = false;

                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", wm->GetPlatform()->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
                  message.SetDetails("The weapons manager on " + wm->GetPlatform()->GetName() + " is subordinate to the battle manager on " +
                                     platform->GetName() + ", but the following platforms between the two on the default command chain are not C2-capable in this context: ");
                  size_t nameCount = nonC2PlatformNames.size();
                  for (size_t i = 0; i != nameCount; ++i)
                  {
                     message.AddDetails(nonC2PlatformNames[i]);
                     if (i != nameCount - 1)
                     {
                        message.AddDetails(", ");
                     }

                     message.AddScenarioFileLocation(ScenarioFileLocation("platform", nonC2PlatformNames[i]));
                  }
                  message.AddDetails(". To fix this, ensure that each of these platforms has (1) an asset manager, (2) a disseminate C2 manager, and (3) max assignments > 0.");
                  messages.push_back(message);
                  message.ClearDetails();
               }
            }
         }
      }
   }
   return passedCheck;
}

bool checkBattleManagerReachableBySubordinateSensorsHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      unsigned procCount = platform->GetComponentCount<WsfProcessor>();
      for (unsigned j = 0; j < procCount; ++j)
      {
         auto battleManager = dynamic_cast<WsfBattleManager*>(platform->GetComponentEntry<WsfProcessor>(j));
         if (battleManager != nullptr)
         {
            // Find all sensors subordinate to this battle manager
            std::vector<WsfSensor*> subordinateSensors = getAllPartsDownCommandChain<WsfSensor>(battleManager->GetPlatform());

            // Ensure that each subordinate sensor has a path to communicate with the battle manager (i.e., is connected
            // via internal/external links with appropriate comms on both sides of each external link).
            for (WsfSensor* sensor : subordinateSensors)
            {
               std::vector<WsfBattleManager*> reachableBMs;

               std::vector<WsfTrackProcessor*> trackProcs = getInternallyLinkedPlatformParts<WsfTrackProcessor>(sensor);
               // We've found a connection to a battle manager if the sensor is internally linked to a track processor AND
               // a battle manager is present on the platform:
               if (!trackProcs.empty() && WsfBMUtils::FindAttachedBattleManagerRef2(sensor->GetPlatform()))
               {
                  reachableBMs.push_back(WsfBMUtils::FindAttachedBattleManagerRef2(sensor->GetPlatform()));
               }

               // Get all track processors connected to the sensor by a network of internal links, external links, and compatible comms.
               // ONLY follow external links from WsfTrackProcessor's and WsfLinkedProcessor's.
               // (WsfTrackProcessor may have a type of "WSF_TRACK_MANAGER").
               std::vector<WsfTrackProcessor*> linkedAndReachableTrackProcs = getLinkedAndReachablePlatformParts<WsfTrackProcessor>(
                                                                                 sensor, std::vector<std::string> {"WSF_TRACK_PROCESSOR", "WSF_LINKED_PROCESSOR", "WSF_TRACK_MANAGER"}, true);

               for (WsfTrackProcessor* trackProc : linkedAndReachableTrackProcs)
               {
                  // If a battle manager is on the same platform as a linked and reachable track processor, the BM
                  // is linked and reachable by the sensor.
                  WsfBattleManager* BM = WsfBMUtils::FindAttachedBattleManagerRef2(trackProc->GetPlatform());
                  if (BM)
                  {
                     reachableBMs.push_back(BM);
                  }
               }

               // Now that we've identified all battle managers reachable by the subordinate sensor,
               // check that the original battle manager is among those reachable.
               if (std::find(reachableBMs.begin(), reachableBMs.end(), battleManager) == reachableBMs.end())
               {
                  passedCheck = false;

                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("processors", battleManager->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", sensor->GetPlatform()->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("sensors", sensor->GetName()));
                  message.AddDetails("Platform " + platform->GetName() + "'s battle manager " + battleManager->GetName() + " has sensor "
                                     + sensor->GetName() + " on platform " + sensor->GetPlatform()->GetName()
                                     + " as a subordinate, but this sensor is not able to communicate back to the battle manager. "
                                     + "Ensure that a network of internal links, 'report_to' statements on WSF_LINKED_PROCESSORs and WSF_TRACK_PROCESSORs, "
                                     + "and compatible comms on either side of each 'report_to' connects the sensor to the battle manager.");
                  messages.push_back(message);
                  message.ClearDetails();
               }
            }
         }
      }
   }
   return passedCheck;
}

bool checkBattleManagersDontConflictHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i != platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      auto bm = WsfBMUtils::FindAttachedBattleManagerRef(platform);
      if (bm && bm->getCommitAuthority())
      {
         // We've found a BM with commit authority, so we need to ensure that there is no platform
         // superior to it in the default command chain that also has a BM with commit authority.
         WsfPlatform* current = platform;
         WsfPlatform* commander = current->GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId())->GetCommander();
         while (commander && commander->GetNameId() != current->GetNameId())
         {
            auto otherBM = WsfBMUtils::FindAttachedBattleManagerRef(commander);
            if (otherBM && otherBM->getCommitAuthority())
            {
               passedCheck = false;

               message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
               message.AddScenarioFileLocation(ScenarioFileLocation("platform", commander->GetName()));
               message.SetDetails("Platform " + platform->GetName() + " is a subordinate of " + commander->GetName() +
                                  " in the default command chain, but both have battle mangers with commit authority, which creates a conflict. " +
                                  "Note that even if the default command chain isn't explicitly defined, it still gets created: " +
                                  "If a declared command chain is present, the default command chain will copy that structure. As a result, the solution " +
                                  "may be to define a default command chain that avoids BM conflicts, if one has not yet been defined.");
               messages.push_back(message);
               message.ClearDetails();
               break;
            }
            current = commander;
            commander = commander->GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId())->GetCommander();
         }
      }
   }
   return passedCheck;
}

bool checkMaxAquisitionTimeLongEnoughForSensorsToFormTracksHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      unsigned sensorCount = platform->GetComponentCount<WsfSensor>();
      for (unsigned j = 0; j != sensorCount; ++j)
      {
         WsfSensor* sensor = platform->GetComponentEntry<WsfSensor>(j);
         WsfBMSensorsManager* SM = getManagingSensorsManager(sensor);
         if (SM && SM->HasSMComponent())
         {
            auto SMInterface = SM->GetCoreSMRef();
            double maxAcqTime = 0.0;
            std::string sensorType = "";
            if (SMInterface && WsfBMUtils::GetSensorType(*sensor) == il::SENSOR_TTR)
            {
               maxAcqTime = SMInterface->GetMaxTTRAcqTime();
               sensorType += "TTR";
            }
            else if (SMInterface && WsfBMUtils::GetSensorType(*sensor) == il::SENSOR_TAR)
            {
               maxAcqTime = SMInterface->GetMaxTARAcqTime();
               sensorType += "TAR";
            }
            else
            {
               continue; // Check only applies to TTRs and TARs
            }
            size_t modeCount = sensor->GetModeCount();
            for (size_t k = 0; k != modeCount; ++k)
            {
               WsfSensorMode* mode = sensor->GetModeEntry(k);
               double absoluteMinTime = mode->GetFrameTime() * mode->HitsToEstablishTrack();
               double detectionWindowTime = mode->GetFrameTime() * mode->EstablishTrackWindow();

               // The sensors manager will CANTCO a sensor's assignment if it hasn't started tracking
               // within maxAcqTime of being cued. If absoluteMinTime > maxAcqTime, the sensor can't
               // possibly establish a track within maxAcqTime.
               if (absoluteMinTime > maxAcqTime)
               {
                  passedCheck = false;

                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", SM->GetPlatform()->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("processors", SM->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", sensor->GetPlatform()->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("sensors", sensor->GetName()));
                  message.SetDetails("The sensors manager on platform " + SM->GetPlatform()->GetName() + " has a max " + sensorType +
                                     " acquisition time of " + std::to_string(maxAcqTime) + " seconds, but mode " + mode->GetName() + " on its subordinate sensor " + sensor->GetName() +
                                     " requires a minimum of " + std::to_string(absoluteMinTime) + " seconds to establish tracks. " +
                                     "To address this, (1) increase the max acquisition time, (2) decrease the mode's 'frame time', or " +
                                     "(3) decrease the first value for the mode's 'hits_to_establish_track'.");
                  messages.push_back(message);
                  message.ClearDetails();
               }
               // Or, if detectionWindowTime > maxAcqTime, the sensor might be able to establish a track
               // within maxAcqTime but it is unlikely.
               else if (detectionWindowTime > maxAcqTime)
               {
                  passedCheck = false;

                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", SM->GetPlatform()->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("processors", SM->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", sensor->GetPlatform()->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("sensors", sensor->GetName()));
                  message.SetDetails("The sensors manager on platform " + SM->GetPlatform()->GetName() + " has a max " + sensorType +
                                     " acquisition time of " + std::to_string(maxAcqTime) + " seconds, but mode " + mode->GetName() + " on its subordinate sensor " + sensor->GetName() +
                                     " has a detection window time of " + std::to_string(detectionWindowTime) + " seconds, so it is unlikely to establish tracks. " +
                                     "To address this, (1) increase the max acquisition time, (2) decrease the mode's 'frame time', or " +
                                     "(3) decrease the second value for the mode's 'hits_to_establish_track'.");
                  messages.push_back(message);
                  message.ClearDetails();
               }
            }
         }
      }
   }
   return passedCheck;
}

bool checkPlatformWithBattleManagerIsC2CapableHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      // If platform has a weapons manager, check that it also has both a track processor and a disseminate C2 manager.
      if (WsfBMUtils::FindAttachedBattleManagerRef2(platform))
      {
         message.SetDetails("Platform " + platform->GetName() + " is deploying a battle manager, so it must also deploy an asset manager, " +
                            "a track processor, and a disseminate C2 manager. ");
         if (!isC2CapableHelper(platform, message))
         {
            passedCheck = false;

            message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
            messages.push_back(message);
            message.ClearDetails();
         }
      }
   }
   return passedCheck;
}

bool checkPlatformWithDisseminateC2ManagerHasInternalCommLinksHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      const WsfPlatform* platform = const_cast<WsfPlatform*>(sim.GetPlatformEntry(i));
      WsfBMDisseminateC2* dc2 = WsfBMUtils::FindAttachedDisseminationManagerRef2(platform);
      if (dc2)
      {
         bool linkedToComm = false;
         unsigned commCount = platform->GetComponentCount<wsf::comm::Comm>();
         for (unsigned j = 0; j < commCount; ++j)
         {
            WsfInternalLinks& commLinks = platform->GetComponentEntry<wsf::comm::Comm>(j)->GetInternalLinks();
            unsigned recipientCount = commLinks.GetRecipientCount();
            for (unsigned k = 0; k < recipientCount; ++k)
            {
               if (commLinks.GetRecipientEntry(k)->GetComponentName() == dc2->GetComponentName())
               {
                  linkedToComm = true;
                  break;
               }
            }
         }
         if (!linkedToComm)
         {
            passedCheck = false;

            message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
            message.AddScenarioFileLocation(ScenarioFileLocation("processors", dc2->GetName()));
            for (unsigned k = 0; k != commCount; ++k)
            {
               message.AddScenarioFileLocation(ScenarioFileLocation("comms", platform->GetComponentEntry<wsf::comm::Comm>(k)->GetName()));
            }

            message.SetDetails("Platform " + platform->GetName() +
                               " is deploying a disseminate C2 manager, but no comm on the platform is internally linked to it. " +
                               "Add an 'internal_link' from any comm to the disseminate C2 manager.");
            messages.push_back(message);
            message.ClearDetails();
         }
      }
   }
   return passedCheck;
}

bool checkPlatformWithSensorsManagerConnectedToBattleManagerWithCommitAuthorityHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   // check returns true if platform has a battle manager that has commit authority
   PlatformCheck check = [](WsfPlatform * p)
   {
      std::shared_ptr<il::BattleManagerInterface> bm = WsfBMUtils::FindAttachedBattleManagerRef(p);
      return bm ? bm->getCommitAuthority() : false;
   };

   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      if (WsfBMUtils::FindAttachedSensorsManagerRef(const_cast<WsfPlatform*>(platform)))
      {
         // Run check on platform and recurisvely up the default command chain, until a platform that passes is found or all are checked
         if (!checkUpCommandChain(platform, check, WsfCommandChain::GetDefaultNameId()))
         {
            passedCheck = false;

            message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
            message.SetDetails("Platform " + platform->GetName() +
                               " deploys a sensors manager, but neither this platform nor any platform above it in the " +
                               "default command chain deploys a battle manager with commit authority.");
            messages.push_back(message);
            message.ClearDetails();
         }
      }
   }
   return passedCheck;
}

bool checkPlatformWithSensorsManagerConnectedToTAROrTTRHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   // check returns true if platform has a sensor of type TAR or TTR
   PlatformCheck check = [](WsfPlatform * p)
   {
      unsigned sensorCount = p->GetComponentCount<WsfSensor>();
      for (unsigned idx = 0; idx < sensorCount; ++idx)
      {
         il::SensorEnum sensorType = WsfBMUtils::GetSensorType(*(p->GetComponentEntry<WsfSensor>(idx)));
         if (sensorType == il::SENSOR_TAR || sensorType == il::SENSOR_TTR)
         {
            return true;
         }
      }
      return false;
   };

   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      // Run check if platform deploys a sensors mananger
      if (WsfBMUtils::FindAttachedSensorsManagerRef2(const_cast<WsfPlatform*>(platform)))
      {
         if (!checkDownCommandChain(platform, check, WsfCommandChain::GetDefaultNameId()))
         {
            passedCheck = false;

            message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
            message.SetDetails("Platform " + platform->GetName() + " has a sensors manager, "
                               + "but neither this platform nor any platform below it in the default command chain has a TAR or TTR.");
            messages.push_back(message);
            message.ClearDetails();
         }
      }
   }
   return passedCheck;
}

bool checkPlatformWithSensorsManagerIsC2CapableHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      // If platform has a sensors manager, check that it also has both a track processor and a disseminate C2 manager.
      if (WsfBMUtils::FindAttachedSensorsManagerRef2(platform))
      {
         message.SetDetails("Platform " + platform->GetName() + " is deploying a sensors manager, so it must also deploy an asset manager, " +
                            "a track processor, and a disseminate C2 manager. ");
         if (!isC2CapableHelper(platform, message))
         {
            passedCheck = false;

            message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
            messages.push_back(message);
            message.ClearDetails();
         }
      }
   }
   return passedCheck;
}

bool checkPlatformWithTAROrTTRConnectedToSensorsManagerHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   // check returns true if platform deploys a sensors manager
   PlatformCheck check = [](WsfPlatform * p)
   {
      return WsfBMUtils::FindAttachedSensorsManagerRef2(p) != nullptr;
   };

   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      unsigned sensorsCount = platform->GetComponentCount<WsfSensor>();
      for (unsigned j = 0; j < sensorsCount; ++j)
      {
         WsfSensor* sensor = platform->GetComponentEntry<WsfSensor>(j);
         if (WsfBMUtils::GetSensorType(*sensor) == il::SENSOR_TAR || WsfBMUtils::GetSensorType(*sensor) == il::SENSOR_TTR)
         {
            if (!checkUpCommandChain(platform, check, WsfCommandChain::GetDefaultNameId()))
            {
               passedCheck = false;

               message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
               message.AddScenarioFileLocation(ScenarioFileLocation("sensors", sensor->GetName()));
               message.SetDetails("Platform " + platform->GetName() + " has a TAR or TTR sensor, "
                                  + "but neither this platform nor any platform above it in the default command chain deploys a sensors manager.");
               messages.push_back(message);
               message.ClearDetails();
            }
         }
      }

   }
   return passedCheck;
}

bool checkPlatformWithWeaponHasWeaponsManagerHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      const WsfPlatform* platform = const_cast<WsfPlatform*>(sim.GetPlatformEntry(i));
      if (platform->GetComponentCount<WsfWeapon>() > 0 && !WsfBMUtils::FindAttachedWeaponsManagerRef2(platform))
      {
         passedCheck = false;

         message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
         message.AddScenarioFileLocation(ScenarioFileLocation("weapons", platform->GetComponentEntry<WsfWeapon>(0)->GetName()));
         message.SetDetails("Platform " + platform->GetName() + " has a weapon but is not deploying a weapons manager.");
         messages.push_back(message);
         message.ClearDetails();
      }
   }
   return passedCheck;
}

bool checkPlatformWithWeaponsManagerConnectedToBattleManagerWithCommitAuthorityHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   // check returns true if platform has a battle manager that has commit authority
   PlatformCheck check = [](WsfPlatform * p)
   {
      auto bm = WsfBMUtils::FindAttachedBattleManagerRef(p);
      return bm.get() ? bm->getCommitAuthority() : false;
   };

   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      if (WsfBMUtils::FindAttachedWeaponsManagerRef(const_cast<WsfPlatform*>(platform)))
      {
         // Run check on platform and recurisvely up the default command chain, until a platform that passes is found or all are checked
         if (!checkUpCommandChain(platform, check, WsfCommandChain::GetDefaultNameId()))
         {
            passedCheck = false;

            message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
            message.SetDetails("Platform " + platform->GetName() +
                               " deploys a weapons manager, but neither this platform nor any platform " +
                               "above it in the default command chain deploys a battle manager with commit authority.");
            messages.push_back(message);
            message.ClearDetails();
         }
      }
   }
   return passedCheck;
}

bool checkPlatformWithWeaponsManagerHasAccessToRequiredSensorHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      auto wm = WsfBMUtils::FindAttachedWeaponsManagerRef(platform);
      if (wm)
      {
         il::SensorEnum requiredSensor = il::SENSOR_UNKNOWN;
         if (wm->getEngageEWTargets())
         {
            requiredSensor = il::SENSOR_EW;
         }
         else if (wm->getEngageTARTargets())
         {
            requiredSensor = il::SENSOR_TAR;
         }
         else if (wm->getEngageTTRTargets())
         {
            requiredSensor = il::SENSOR_TTR;
         }

         // check returns true if any sensor on p matches requiredSensor type
         PlatformCheck check = [requiredSensor](WsfPlatform * p)
         {
            unsigned sensorCount = p->GetComponentCount<WsfSensor>();
            for (unsigned j = 0; j < sensorCount; ++j)
            {
               WsfSensor* sensor = p->GetComponentEntry<WsfSensor>(j);
               if (WsfBMUtils::GetSensorType(*sensor) >= requiredSensor)
               {
                  return true;
               }
            }
            return false;
         };

         if (!checkFullCommandChain(platform, check))
         {
            passedCheck = false;

            message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
            message.SetDetails("Platform " + platform->GetName() + "'s weapons manager requires a sensor of at least type "
                               + getSensorTypeStringFromEnum(requiredSensor, IADSC2ScenarioExtension::GetGlobalLogger(platform->GetSimulation()->GetScenario()))
                               + ", but no platform connected to it through any of its command chains has a sensor of this type.");

            // If platform doesn't have access to a sensor of sufficient quality, search for a sufficient sensor anywhere on the platform's side
            // and inform the user of the first one we find.
            WsfStringId platformSide = platform->GetSideId();
            for (unsigned j = 0; j != platformCount; ++j)
            {
               WsfPlatform* otherPlatform = sim.GetPlatformEntry(j);
               if (otherPlatform->GetSideId() == platformSide)
               {
                  if (check(otherPlatform))
                  {
                     message.AddDetails(" At least one sensor of sufficient quality exists on platform's side, located on " + otherPlatform->GetName() + ".");
                     message.AddScenarioFileLocation(ScenarioFileLocation("platform", otherPlatform->GetName()));
                     break;
                  }
               }
            }

            messages.push_back(message);
            message.ClearDetails();
         }
      }
   }
   return passedCheck;
}

bool checkPlatformWithWeaponsManagerHasWeaponHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      const WsfPlatform* platform = const_cast<WsfPlatform*>(sim.GetPlatformEntry(i));
      if (WsfBMUtils::FindAttachedWeaponsManagerRef2(platform) && platform->GetComponentCount<WsfWeapon>() <= 0)
      {
         passedCheck = false;

         message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
         message.SetDetails("Platform " + platform->GetName() + " is deploying a weapons manager but does not have any weapons.");
         messages.push_back(message);
         message.ClearDetails();
      }
   }
   return passedCheck;
}

bool checkPlatformWithWeaponsManagerIsC2CapableHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      // If platform has a weapons manager, check that it also has both a track processor and a disseminate C2 manager.
      if (WsfBMUtils::FindAttachedWeaponsManagerRef2(platform))
      {
         message.SetDetails("Platform " + platform->GetName() + " is deploying a weapons manager, so it must also deploy an asset manager, " +
                            "a track processor, and a disseminate C2 manager. ");
         if (!isC2CapableHelper(platform, message))
         {
            passedCheck = false;

            message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
            messages.push_back(message);
            message.ClearDetails();
         }
      }
   }
   return passedCheck;
}

bool checkSensorsManagersMustNotConflictHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i != platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      auto bm = WsfBMUtils::FindAttachedBattleManagerRef(platform);
      if (bm && bm->getCommitAuthority())
      {
         std::vector<WsfBMWeaponsManager*> subordinateWeaponsManagers = getAllPartsDownCommandChain<WsfBMWeaponsManager>(platform, WsfCommandChain::GetDefaultName());
         // For any weapons manager subordinate to a battle manager with commit authority on the default command chain, a sensors manager conflict exists if:
         // there is more than one platform between the WM platform and the BM platform (including the BM platform) on the default command chain
         // that has a sensors manager. HOWEVER, there is never a conflict from the perspective of a WM platform if the WM platform deploys
         // its own sensors manager, regardless of the number of other SMs between it and the BM. This means that the same configuration of SMs
         // in the default command chain can create SM conflicts relative to some subordinate platforms while avoiding conflicts relative to others.
         for (auto wm : subordinateWeaponsManagers)
         {
            bool sensorsManagerConflict = false;

            // First, determine if the wm platform falls into the special "no conflict possible" case by deploying its own sensors manager.
            if (WsfBMUtils::FindAttachedSensorsManagerRef(wm->GetPlatform()))
            {
               continue;
            }
            // If not, follow the path back up the default command chain to the BM platform.
            else
            {
               unsigned sensorsManagerCount = 0;
               WsfPlatform* next = wm->GetPlatform();
               while (next && next->GetNameId() != platform->GetNameId())
               {
                  WsfPlatform* commander = next->GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId())->GetCommander();
                  if (commander && commander->GetNameId() == next->GetNameId())
                  {
                     // 'next' is a top-level commander, so there's nowhere else to go. Shouldn't reach this, since we should always
                     // hit the BM platform we started with first.
                     break;
                  }
                  next = commander;

                  if (next)
                  {
                     if (WsfBMUtils::FindAttachedSensorsManagerRef(next))
                     {
                        // Count the number of SMs encountered on platforms btw the WM platform and the BM platform.
                        ++sensorsManagerCount;
                        if (sensorsManagerCount > 1)
                        {
                           // If more than one encountered, we've found a conflict. No need to keep looking.
                           sensorsManagerConflict = true;
                           break;
                        }
                     }
                  }
               }
            }
            if (sensorsManagerConflict)
            {
               passedCheck = false;

               message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
               message.AddScenarioFileLocation(ScenarioFileLocation("platform", wm->GetPlatform()->GetName()));
               message.AddDetails("Multiple sensors managers located on platforms between a battle manager platform (" + platform->GetName() +
                                  ") and a weapons manager platform (" + wm->GetPlatform()->GetName() + ") will send conflicting cues to subordinate sensors. " +
                                  "To fix this, either (1) place a sensors manager on the weapons manager platform, or (2) ensure that only one sensors manager " +
                                  "appears on any platform between the battle manager and weapons manager platforms on the default command chain.");
               messages.push_back(message);
               message.ClearDetails();
            }
         }
      }
   }
   return passedCheck;
}

bool checkTTRWithFOVSensorsManagerDoesNotManipulateOnOffHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      unsigned commCount = platform->GetComponentCount<WsfSensor>();
      for (unsigned j = 0; j < commCount; ++j)
      {
         WsfSensor* sensor = platform->GetComponentEntry<WsfSensor>(j);
         if (WsfBMUtils::GetSensorType(*sensor) == il::SENSOR_TTR)
         {
            WsfBMSensorsManager* SM = getManagingSensorsManager(sensor);
            if (SM && dynamic_cast<WsfSensorsManagerFov*>(SM))
            {
               // For WsfSensor, default value for both is false
               if (sensor->InitiallyTurnedOn() || sensor->IsTurnedOn())
               {
                  passedCheck = false;

                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("sensors", sensor->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", SM->GetPlatform()->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("processors", SM->GetName()));
                  message.SetDetails("Sensor " + sensor->GetName() + " on platform " + platform->GetName() +
                                     " is a TTR managed by a FOV sensors manager, so its on/off state must not be manipulated. " +
                                     "Do not explicitly turn the sensor 'on': the FOV sensors manager will handle turning sensors on and off as needed.");
                  messages.push_back(message);
                  message.ClearDetails();
               }
            }
         }
      }
   }
   return passedCheck;
}

bool checkTTRWithFOVSensorsManagerHasAuxDataFieldsHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      unsigned commCount = platform->GetComponentCount<WsfSensor>();
      for (unsigned j = 0; j < commCount; ++j)
      {
         WsfSensor* sensor = platform->GetComponentEntry<WsfSensor>(j);
         if (WsfBMUtils::GetSensorType(*sensor) == il::SENSOR_TTR)
         {
            WsfBMSensorsManager* SM = getManagingSensorsManager(sensor);
            if (SM && dynamic_cast<WsfSensorsManagerFov*>(SM))
            {

               // TTR is managed by an FOV sensors manager: check that the sensor has the following aux data fields
               std::vector<std::string> auxChecks;
               auxChecks.emplace_back("RESTING_AZIMUTH");
               auxChecks.emplace_back("COARSE_SLEW_RATE_AZIMUTH");
               auxChecks.emplace_back("FINE_SLEW_RATE_AZIMUTH");
               WsfAttributeContainer& auxData = sensor->GetAuxData();
               for (const auto& aux : auxChecks)
               {
                  if (!auxData.AttributeExists(aux))
                  {
                     passedCheck = false;

                     message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
                     message.AddScenarioFileLocation(ScenarioFileLocation("sensors", sensor->GetName()));
                     message.AddScenarioFileLocation(ScenarioFileLocation("platform", SM->GetPlatform()->GetName()));
                     message.AddScenarioFileLocation(ScenarioFileLocation("processors", SM->GetName()));
                     message.SetDetails("Sensor " + sensor->GetName() + " on platform " + platform->GetName()
                                        + " is a TTR managed by an FOV sensors manager, but it is missing an auxiliary data field for " + aux + ".");
                     messages.push_back(message);
                     message.ClearDetails();
                  }
               }
            }
         }
      }
   }
   return passedCheck;
}

bool checkTTRWithFOVSensorsManagerHasOneBeamPerModeHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      unsigned sensorCount = platform->GetComponentCount<WsfSensor>();
      for (unsigned j = 0; j != sensorCount; ++j)
      {
         WsfSensor* sensor = platform->GetComponentEntry<WsfSensor>(j);
         if (WsfBMUtils::GetSensorType(*sensor) == il::SENSOR_TTR)
         {
            WsfBMSensorsManager* SM = getManagingSensorsManager(sensor);
            if (SM && dynamic_cast<WsfSensorsManagerFov*>(SM))
            {
               size_t modeCount = sensor->GetModeCount();
               for (size_t k = 0; k != modeCount; ++k)
               {
                  WsfSensorMode* mode = sensor->GetModeEntry(k);
                  if (mode->GetBeamCount() != 1)
                  {
                     passedCheck = false;

                     message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
                     message.AddScenarioFileLocation(ScenarioFileLocation("sensors", sensor->GetName()));
                     message.AddScenarioFileLocation(ScenarioFileLocation("platform", SM->GetPlatform()->GetName()));
                     message.AddScenarioFileLocation(ScenarioFileLocation("processors", SM->GetName()));
                     message.SetDetails("Sensor " + sensor->GetName() + " platform " + platform->GetName() + " is a TTR managed by an FOV sensors manager, but mode "
                                        + mode->GetName() + " on the sensor has " + std::to_string(mode->GetBeamCount()) + " beams. It should only have one beam per mode.");
                     messages.push_back(message);
                     message.ClearDetails();
                  }
               }
            }
         }
      }
   }
   return passedCheck;
}

bool checkTTRWithFOVSensorsManagerUsesDefaultSensorSchedulerHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      unsigned commCount = platform->GetComponentCount<WsfSensor>();
      for (unsigned j = 0; j < commCount; ++j)
      {
         WsfSensor* sensor = platform->GetComponentEntry<WsfSensor>(j);
         if (WsfBMUtils::GetSensorType(*sensor) == il::SENSOR_TTR)
         {
            WsfBMSensorsManager* SM = getManagingSensorsManager(sensor);
            if (SM && dynamic_cast<WsfSensorsManagerFov*>(SM))
            {
               WsfSensorScheduler* scheduler = sensor->GetScheduler();
               if (!dynamic_cast<WsfDefaultSensorScheduler*>(scheduler))
               {
                  passedCheck = false;

                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("sensors", sensor->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", SM->GetPlatform()->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("processors", SM->GetName()));
                  message.SetDetails("Sensor " + sensor->GetName() + " on platform " + platform->GetName()
                                     + " is a TTR managed by a FOV sensors manager, so its must use the default sensor scheduler.");
                  messages.push_back(message);
                  message.ClearDetails();
               }
            }
         }
      }
   }
   return passedCheck;
}

bool checkWeaponsOnWMAIPlatformHavRequiredAuxDataHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i != platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      WsfWeaponsManagerAI* wmai = findWMAI(platform);
      if (wmai != nullptr)
      {
         // Check that all weapons on the WMAI platform have the required auxiliary data
         std::vector<std::string> auxChecks;
         auxChecks.emplace_back("maxTOF");
         auxChecks.emplace_back("avgSpd");
         auxChecks.emplace_back("maxRng");
         auxChecks.emplace_back("minRng");
         auxChecks.emplace_back("shootDelay");
         auxChecks.emplace_back("estPK");

         unsigned weaponCount = platform->GetComponentCount<WsfWeapon>();
         for (unsigned j = 0; j != weaponCount; ++j)
         {
            WsfWeapon* weapon = platform->GetComponentEntry<WsfWeapon>(j);
            WsfAttributeContainer& auxData = weapon->GetAuxData();
            for (const auto& aux : auxChecks)
            {
               if (!auxData.AttributeExists(aux))
               {
                  passedCheck = false;

                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("weapons", weapon->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", wmai->GetPlatform()->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("processors", wmai->GetName()));
                  message.SetDetails("Weapon " + weapon->GetName() + " on platform " + platform->GetName()
                                     + " is managed by a WSF_WEAPONS_MANAGER_AI, but it is missing an auxiliary data field for " + aux + ".");
                  messages.push_back(message);
                  message.ClearDetails();
               }
            }
         }
      }
   }
   return passedCheck;
}

bool checkWMAIPlatformDetectsEnemyTTRsHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();

   // Build up lists of all EM transmitters on WsfRadarSensors on each side in the simulation
   std::map<WsfStringId, std::vector<const WsfEM_Xmtr*>> xmtrsOnEachSide;

   for (size_t p = 0; p != platformCount; ++p)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(p);
      std::string platformName = platform->GetName();
      unsigned sensorCount = platform->GetComponentCount<WsfSensor>();
      for (unsigned s = 0; s != sensorCount; ++s)
      {
         // TODO: need to consider more than just radar sensors for RWR?
         auto radarSensor = dynamic_cast<WsfRadarSensor*>(platform->GetComponentEntry<WsfSensor>(s));
         if (radarSensor != nullptr)
         {
            size_t xmtrCnt = radarSensor->GetEM_XmtrCount();
            for (size_t i = 0; i < xmtrCnt; ++i)
            {
               WsfEM_Xmtr& xmtr = radarSensor->GetEM_Xmtr(i);
               xmtrsOnEachSide[platform->GetSideId()].push_back(&xmtr);
            }
         }
      }
   }

   for (unsigned i = 0; i != platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      WsfWeaponsManagerAI* wmai = findWMAI(platform);
      if (wmai != nullptr)
      {
         auto wm = dynamic_cast<il::weaponsManagerAI*>(wmai->GetCoreWMRef().get());
         if (wm != nullptr && wm->getEvasionRWRResponse())
         {
            unsigned sensorCount = platform->GetComponentCount<WsfSensor>();
            for (unsigned k = 0; k != sensorCount; ++k)
            {
               WsfSensor* sensor = platform->GetComponentEntry<WsfSensor>(k);
               if (WsfBMUtils::GetSensorType(*sensor) == il::SensorEnum::SENSOR_RWR &&
                   dynamic_cast<WsfESM_Sensor*>(sensor) != nullptr)
               {
                  // We've found our RWR...
                  auto RWR = static_cast<WsfESM_Sensor*>(sensor);

                  std::string TTRs = "";

                  unsigned interactingEnemyTTRs = 0;
                  unsigned interactingEnemyTARs = 0;
                  unsigned interactingEnemyEWs = 0;
                  unsigned interactingEnemyOther = 0;

                  // ... Now, determine which enemy platforms it interacts with.
                  // (Whether a xmtr and rcvr "interact" depends on whether the
                  // xtmr transmits within the rcvr's bandwidth.)
                  size_t rcvrCnt = RWR->GetEM_RcvrCount();
                  for (size_t i = 0; i < rcvrCnt; ++i)
                  {
                     WsfEM_Rcvr& rcvr = RWR->GetEM_Rcvr(i);
                     for (const auto& entry : xmtrsOnEachSide)
                     {
                        WsfStringId sensorSideId = entry.first;
                        if (sensorSideId != platform->GetSideId())
                        {
                           const std::vector<const WsfEM_Xmtr*>& enemyXmtrs = entry.second;
                           for (auto xmtr : enemyXmtrs)
                           {
                              // We've found an enemy transmitter that the RWR can potentially sense.
                              // (CanInteractWith looks for overlap of receiver's frequency spectrum with transmitter's.)
                              if (rcvr.CanInteractWith(const_cast<WsfEM_Xmtr*>(xmtr)))
                              {
                                 auto enemySensor = dynamic_cast<WsfSensor*>(xmtr->GetArticulatedPart());
                                 // RWR is interacting with an enemy sensor, so determine what category of sensor it is.
                                 if (enemySensor != nullptr)
                                 {
                                    il::SensorEnum enemySensorType = WsfBMUtils::GetSensorType(*enemySensor);
                                    switch (enemySensorType)
                                    {
                                       case il::SensorEnum::SENSOR_TTR:
                                          interactingEnemyTTRs++;
                                          TTRs += enemySensor->GetPlatform()->GetName() + ", ";
                                          break;

                                       case il::SensorEnum::SENSOR_TAR:
                                          interactingEnemyTARs++;
                                          break;

                                       case il::SensorEnum::SENSOR_EW:
                                          interactingEnemyEWs++;
                                          break;

                                       case il::SensorEnum::SENSOR_RWR:
                                          // Minimal handling to address compiler warning of missing case
                                          break;

                                       case il::SensorEnum::SENSOR_UNKNOWN:
                                          interactingEnemyOther++;
                                          break;
                                    };
                                 }
                              }
                           }
                        }
                     }
                  }
                  unsigned nonTTR_Interactions = interactingEnemyTARs + interactingEnemyEWs + interactingEnemyOther;
                  // Check passes: RWR is interacting with at least one enemy TTR and no other enemy sensors.
                  if (interactingEnemyTTRs > 0 && nonTTR_Interactions == 0)
                  {
                     break;
                  }

                  passedCheck = false;

                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("processors", wmai->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("sensors", sensor->GetName()));

                  std::string fixMessage = "This can be fixed by either (1) adjusting the frequency_band on the RWR (" + sensor->GetName() +
                                           ") or (2) changing the frequencies on which enemy sensors are transmitting.";
                  // Check fails: RWR is not interacting with any enemy sensors, so WMAI will never evade.
                  if (interactingEnemyTTRs == 0 && nonTTR_Interactions == 0)
                  {
                     message.SetDetails("Platform " + platform->GetName() + " has a WMAI with RWR response enabled, but the WSF_ESM_SENSOR configured as " +
                                        "the RWR sensor will not pick up any enemy platforms. As a result, platform will not perform any evasion behaviors. " +
                                        fixMessage);
                  }
                  // Check fails: RWR is interacting with at least some enemy sensors, but not the combination we'd expect (TTRs only).
                  else
                  {
                     std::string lowerQualityMessage = "However, the RWR is tuned in such a way that it will pick up (and trigger evasions relative to) enemy sensors of lower quality. This includes " +
                                                       std::to_string(interactingEnemyTARs) + " TARs, " +
                                                       std::to_string(interactingEnemyEWs) + " EWs, and " +
                                                       std::to_string(interactingEnemyOther) + " uncategorized sensors. ";
                     if (interactingEnemyTTRs == 0)
                     {
                        message.SetDetails("Platform " + platform->GetName() + " has a WMAI with RWR response enabled, but the WSF_ESM_SENSOR configured as " +
                                           "the RWR sensor will not pick up any enemy TTRs to be potentially evaded. " +
                                           lowerQualityMessage + fixMessage);
                     }
                     else
                     {
                        if (TTRs.size() > 2) { TTRs = TTRs.substr(0, TTRs.size() - 2); } // strip off trailing ", "
                        message.SetDetails("Platform " + platform->GetName() + " has a WMAI with RWR response enabled, and the WSF_ESM_SENSOR configured as " +
                                           "the RWR sensor will pick up " + std::to_string(interactingEnemyTTRs) + " enemy TTRs (on: " + TTRs + ") to be potentially evaded. " +
                                           lowerQualityMessage + fixMessage);
                     }
                  }

                  messages.push_back(message);
                  message.ClearDetails();
               }
            }
         }
      }
   }
   return passedCheck;
}

bool checkWMAIPlatformHasRequiredRWRHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      WsfWeaponsManagerAI* wmai = findWMAI(platform);
      if (wmai != nullptr)
      {
         auto wm = dynamic_cast<il::weaponsManagerAI*>(wmai->GetCoreWMRef().get());
         if (wm != nullptr && wm->getEvasionRWRResponse())
         {
            // We've determined that the processor is a WMAI that has RWR response enabled.
            // Therefore, a passive ESM sensor with category "RWR" must be deployed on the platform.
            bool hasRWRLinkedToTrackManager = false;

            unsigned sensorCount = platform->GetComponentCount<WsfSensor>();
            for (unsigned k = 0; k != sensorCount; ++k)
            {
               WsfSensor* sensor = platform->GetComponentEntry<WsfSensor>(k);
               if (WsfBMUtils::GetSensorType(*sensor) == il::SensorEnum::SENSOR_RWR &&
                   dynamic_cast<WsfESM_Sensor*>(sensor) != nullptr)
               {
                  // We've identified a WsfESM_Sensor on the platform with category "RWR," but this sensor
                  // must also be linked to the track manager. The sensor is linked to the track manager if:
                  // it's connected by internal links to a track processor whose track manager == the plaform's track manager.
                  hasRWRLinkedToTrackManager = false;
                  std::vector<WsfTrackProcessor*> linkedTrackProcs = getInternallyLinkedPlatformParts<WsfTrackProcessor>(sensor);
                  for (auto trackProc : linkedTrackProcs)
                  {
                     if (&trackProc->GetTrackManager() == &platform->GetTrackManager())
                     {
                        hasRWRLinkedToTrackManager = true;
                        goto finishedCheckingPlatform;
                     }
                  }
               }
            }
            if (!hasRWRLinkedToTrackManager)
            {
               passedCheck = false;

               message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
               message.AddScenarioFileLocation(ScenarioFileLocation("processors", wmai->GetName()));
               message.SetDetails("Platform " + platform->GetName() + " has a WSF_WEAPONS_MANAGER_AI with RWR response enabled, " +
                                  "but there is no WSF_ESM_SENSOR with category 'RWR' on the platform to serve as the RWR that is internally " +
                                  "linked to the master track manager.");
               messages.push_back(message);
               message.ClearDetails();
            }

         }
      }
finishedCheckingPlatform:
      ;
   }
   return passedCheck;
}
} // end anonymous namespace

Messages checkPlatformWithSensorsManagerIsC2Capable(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "Sensors manager platforms must be C2 capable",
                        ScenarioAnalyzerMessage::cERROR, sim, checkPlatformWithSensorsManagerIsC2CapableHelper);
}

Messages checkPlatformWithWeaponsManagerIsC2Capable(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "Weapons manager platforms must be C2 capable",
                        ScenarioAnalyzerMessage::cERROR, sim, checkPlatformWithWeaponsManagerIsC2CapableHelper);
}

Messages checkPlatformWithBattleManagerIsC2Capable(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "Battle manager platforms must be C2 capable",
                        ScenarioAnalyzerMessage::cERROR, sim, checkPlatformWithBattleManagerIsC2CapableHelper);
}

Messages checkPlatformWithSensorsManagerConnectedToBattleManagerWithCommitAuthority(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "Sensors manager platforms must be connected to battle manager with commit authority",
                        ScenarioAnalyzerMessage::cERROR, sim, checkPlatformWithSensorsManagerConnectedToBattleManagerWithCommitAuthorityHelper);
}

Messages checkPlatformWithWeaponsManagerConnectedToBattleManagerWithCommitAuthority(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "Weapons manager platforms must be connected to battle manager with commit authority",
                        ScenarioAnalyzerMessage::cERROR, sim, checkPlatformWithWeaponsManagerConnectedToBattleManagerWithCommitAuthorityHelper);
}

Messages checkPlatformWithSensorsManagerConnectedToTAROrTTR(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "Sensors manager platforms must be connected to TAR or TTR",
                        ScenarioAnalyzerMessage::cERROR, sim, checkPlatformWithSensorsManagerConnectedToTAROrTTRHelper);
}

Messages checkPlatformWithTAROrTTRConnectedToSensorsManager(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "Platforms with TAR or TTR should be connected to sensors manager",
                        ScenarioAnalyzerMessage::cWARNING, sim, checkPlatformWithTAROrTTRConnectedToSensorsManagerHelper);
}

Messages checkPlatformWithWeaponsManagerHasWeapon(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "Weapons manager platforms must deploy weapons",
                        ScenarioAnalyzerMessage::cERROR, sim, checkPlatformWithWeaponsManagerHasWeaponHelper);
}

Messages checkPlatformWithWeaponHasWeaponsManager(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "Platforms with weapons should deploy weapons manager",
                        ScenarioAnalyzerMessage::cWARNING, sim, checkPlatformWithWeaponHasWeaponsManagerHelper);
}

Messages checkPlatformWithAssetManagerHasDisseminateC2Manager(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "Asset manager platforms should deploy disseminate C2 manager",
                        ScenarioAnalyzerMessage::cWARNING, sim, checkPlatformWithAssetManagerHasDisseminateC2ManagerHelper);
}

// Checks that any comm on the platform has an internal link to the disseminate C2 manager
Messages checkPlatformWithDisseminateC2ManagerHasInternalCommLinks(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "Disseminate C2 manager platforms must have internal comm links",
                        ScenarioAnalyzerMessage::cERROR, sim, checkPlatformWithDisseminateC2ManagerHasInternalCommLinksHelper);
}

Messages checkPlatformWithWeaponsManagerHasAccessToRequiredSensor(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "Weapons manager platforms should have access to required sensor",
                        ScenarioAnalyzerMessage::cWARNING, sim, checkPlatformWithWeaponsManagerHasAccessToRequiredSensorHelper);
}

Messages checkMaxAquisitionTimeLongEnoughForSensorsToFormTracks(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "Sensors manager max acquisition times should be long enough for subordinate sensors to form tracks",
                        ScenarioAnalyzerMessage::cWARNING, sim, checkMaxAquisitionTimeLongEnoughForSensorsToFormTracksHelper);
}

Messages checkTTRWithFOVSensorsManagerHasOneBeamPerMode(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "TTRs managed by FOV sensors managers should use one beam per mode",
                        ScenarioAnalyzerMessage::cWARNING, sim, checkTTRWithFOVSensorsManagerHasOneBeamPerModeHelper);
}

Messages checkTTRWithFOVSensorsManagerDoesNotManipulateOnOff(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "TTRs managed by FOV sensors manager must not manipulate on off state",
                        ScenarioAnalyzerMessage::cERROR, sim, checkTTRWithFOVSensorsManagerDoesNotManipulateOnOffHelper);
}

Messages checkTTRWithFOVSensorsManagerHasAuxDataFields(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "TTRs managed by FOV sensors manager should provide auxiliary data",
                        ScenarioAnalyzerMessage::cWARNING, sim, checkTTRWithFOVSensorsManagerHasAuxDataFieldsHelper);
}

Messages checkTTRWithFOVSensorsManagerUsesDefaultSensorScheduler(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "TTRs managed by FOV sensors manager must use default sensor scheduler",
                        ScenarioAnalyzerMessage::cERROR, sim, checkTTRWithFOVSensorsManagerUsesDefaultSensorSchedulerHelper);
}

// For each platform with a WsfWeaponsManageAI (WMAI) for which RWR response has been enabled, verfies that:
// (1) the platform has a WsfESM_Sensor (2) with category "RWR" (3) that is internally linked to a track
// processor which in turn points to the platform's master track manager.
Messages checkWMAIPlatformHasRequiredRWR(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "WMAI platforms using RWR response must deploy an RWR",
                        ScenarioAnalyzerMessage::cERROR, sim, checkWMAIPlatformHasRequiredRWRHelper);
}

// For each platform with a WMAI, checks that user has tuned the RWR
// to a bandwidth that will sense at least one enemy TTR sensor. Also,
// let user know if RWR will pick up any enemy radar of lower quality.

// TODO: right now, gives number of TRANSMITTERS that may be detected, which is not the same as the number of sensors:
// for example, a TTR with two modes would be counted as 2 detectable transmitters. OK? Or also keep track of detected
// sensors and report those numbers for each category?
Messages checkWMAIPlatformDetectsEnemyTTRs(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "WMAI platforms using RWR response should have RWR tuned to detect enemy TTRs",
                        ScenarioAnalyzerMessage::cWARNING, sim, checkWMAIPlatformDetectsEnemyTTRsHelper);
}

Messages checkWeaponsOnWMAIPlatformHavRequiredAuxData(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "Weapons on WMAI platforms should have required auxiliary data",
                        ScenarioAnalyzerMessage::cWARNING, sim, checkWeaponsOnWMAIPlatformHavRequiredAuxDataHelper);
}

Messages checkBattleManagerReachableBySubordinateSensors(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "Battle managers should be reachable by subordinate sensors",
                        ScenarioAnalyzerMessage::cWARNING, sim, checkBattleManagerReachableBySubordinateSensorsHelper);
}

Messages checkSensorsManagersMustNotConflict(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "Sensors managers must not conflict with each other",
                        ScenarioAnalyzerMessage::cERROR, sim, checkSensorsManagersMustNotConflictHelper);
}

// Every battle manager must have at least one weapons manager subordinate to it on the default command chain.
// Every node between the BM and its subordinate WM(s), including the BM and WM platforms, must be "C2 capable."
// Here, "C2 capable" means that the platform has (1) an asset manager, (2) a DC2, and (3) max assignments > 0.
Messages checkBattleManagerConnectedToSubordinateWeaponsManagerByC2CapablePlatforms(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "Battle managers must have subordinate weapons managers linked by C2-capable platforms",
                        ScenarioAnalyzerMessage::cERROR, sim, checkBattleManagerConnectedToSubordinateWeaponsManagerByC2CapablePlatformsHelper);
}

// Every battle manager should have at least one sensors manager subordinate to it on the default command chain.
// Every node between the BM and its subordinate SM(s), including the BM and SM platforms, should be "C2 capable."
// Here, "C2 capable" means that the platform has (1) an asset manager and (2) a DC2.
Messages checkBattleManagerConnectedToSubordinateSensorsManagerByC2CapablePlatforms(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "Battle managers should have subordinate sensors managers linked by C2-capable platforms",
                        ScenarioAnalyzerMessage::cWARNING, sim, checkBattleManagerConnectedToSubordinateSensorsManagerByC2CapablePlatformsHelper);
}

// Ensures that no battle manager with commit authority is subordinate to another battle manager
// with commit authority on the default commmand chain.
Messages checkBattleManagersDontConflict(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "Battle managers should not conflict with each other",
                        ScenarioAnalyzerMessage::cERROR, sim, checkBattleManagersDontConflictHelper);
}

Messages checkAssetManagersHaveUpdateInterval(WsfSimulation& sim)
{
   return runSuiteCheck(cSUITE_NAME, "Asset managers should have update interval defined",
                        ScenarioAnalyzerMessage::cWARNING, sim, checkAssetManagersHaveUpdateIntervalHelper);
}

// this declaration is necessary because this name is referenced in the
// UT_DEFINE_SCRIPT_METHOD macros for ScenarioAnalyzerScriptClass
class ScenarioAnalyzerIADSC2
{
   public:
      const char* GetScriptClassName()
      {
         return "ScenarioAnalyzerIADSC2";
      }
};

class ScenarioAnalyzerIADSC2ScriptClass : public UtScriptClass
{
   public:
      UT_DECLARE_SCRIPT_METHOD(CheckPlatformWithSensorsManagerIsC2Capable);
      UT_DECLARE_SCRIPT_METHOD(CheckPlatformWithWeaponsManagerIsC2Capable);
      UT_DECLARE_SCRIPT_METHOD(CheckPlatformWithBattleManagerIsC2Capable);
      UT_DECLARE_SCRIPT_METHOD(CheckPlatformWithSensorsManagerConnectedToBattleManagerWithCommitAuthority);
      UT_DECLARE_SCRIPT_METHOD(CheckPlatformWithWeaponsManagerConnectedToBattleManagerWithCommitAuthority);
      UT_DECLARE_SCRIPT_METHOD(CheckPlatformWithSensorsManagerConnectedToTAROrTTR);
      UT_DECLARE_SCRIPT_METHOD(CheckPlatformWithTAROrTTRConnectedToSensorsManager);
      UT_DECLARE_SCRIPT_METHOD(CheckPlatformWithWeaponsManagerHasWeapon);
      UT_DECLARE_SCRIPT_METHOD(CheckPlatformWithWeaponHasWeaponsManager);
      UT_DECLARE_SCRIPT_METHOD(CheckPlatformWithDisseminateC2ManagerHasInternalCommLinks);
      UT_DECLARE_SCRIPT_METHOD(CheckPlatformWithWeaponsManagerHasAccessToRequiredSensor);
      UT_DECLARE_SCRIPT_METHOD(CheckPlatformWithAssetManagerHasDisseminateC2Manager);
      UT_DECLARE_SCRIPT_METHOD(CheckMaxAquisitionTimeLongEnoughForSensorsToFormTracks);
      UT_DECLARE_SCRIPT_METHOD(CheckTTRWithFOVSensorsManagerDoesNotManipulateOnOff);
      UT_DECLARE_SCRIPT_METHOD(CheckTTRWithFOVSensorsManagerUsesDefaultSensorScheduler);
      UT_DECLARE_SCRIPT_METHOD(CheckTTRWithFOVSensorsManagerHasOneBeamPerMode);
      UT_DECLARE_SCRIPT_METHOD(CheckTTRWithFOVSensorsManagerHasAuxDataFields);
      UT_DECLARE_SCRIPT_METHOD(CheckWMAIPlatformHasRequiredRWR);
      UT_DECLARE_SCRIPT_METHOD(CheckWMAIPlatformDetectsEnemyTTRs);
      UT_DECLARE_SCRIPT_METHOD(CheckWeaponsOnWMAIPlatformHavRequiredAuxData);
      UT_DECLARE_SCRIPT_METHOD(CheckBattleManagerReachableBySubordinateSensors);
      UT_DECLARE_SCRIPT_METHOD(CheckSensorsManagersMustNotConflict);
      UT_DECLARE_SCRIPT_METHOD(CheckBattleManagerConnectedToSubordinateWeaponsManagerByC2CapablePlatforms);
      UT_DECLARE_SCRIPT_METHOD(CheckBattleManagerConnectedToSubordinateSensorsManagerByC2CapablePlatforms);
      UT_DECLARE_SCRIPT_METHOD(CheckBattleManagersDontConflict);
      UT_DECLARE_SCRIPT_METHOD(CheckAssetManagersHaveUpdateInterval);

      explicit ScenarioAnalyzerIADSC2ScriptClass(UtScriptTypes* types)
         : UtScriptClass("ScenarioAnalyzerIADSC2", types)
      {
         SetClassName("ScenarioAnalyzerIADSC2");

         AddStaticMethod(ut::make_unique<CheckPlatformWithSensorsManagerIsC2Capable>());
         AddStaticMethod(ut::make_unique<CheckPlatformWithWeaponsManagerIsC2Capable>());
         AddStaticMethod(ut::make_unique<CheckPlatformWithBattleManagerIsC2Capable>());
         AddStaticMethod(ut::make_unique<CheckPlatformWithSensorsManagerConnectedToBattleManagerWithCommitAuthority>());
         AddStaticMethod(ut::make_unique<CheckPlatformWithWeaponsManagerConnectedToBattleManagerWithCommitAuthority>());
         AddStaticMethod(ut::make_unique<CheckPlatformWithSensorsManagerConnectedToTAROrTTR>());
         AddStaticMethod(ut::make_unique<CheckPlatformWithTAROrTTRConnectedToSensorsManager>());
         AddStaticMethod(ut::make_unique<CheckPlatformWithWeaponsManagerHasWeapon>());
         AddStaticMethod(ut::make_unique<CheckPlatformWithWeaponHasWeaponsManager>());
         AddStaticMethod(ut::make_unique<CheckPlatformWithDisseminateC2ManagerHasInternalCommLinks>());
         AddStaticMethod(ut::make_unique<CheckPlatformWithWeaponsManagerHasAccessToRequiredSensor>());
         AddStaticMethod(ut::make_unique<CheckPlatformWithAssetManagerHasDisseminateC2Manager>());
         AddStaticMethod(ut::make_unique<CheckMaxAquisitionTimeLongEnoughForSensorsToFormTracks>());
         AddStaticMethod(ut::make_unique<CheckTTRWithFOVSensorsManagerDoesNotManipulateOnOff>());
         AddStaticMethod(ut::make_unique<CheckTTRWithFOVSensorsManagerUsesDefaultSensorScheduler>());
         AddStaticMethod(ut::make_unique<CheckTTRWithFOVSensorsManagerHasOneBeamPerMode>());
         AddStaticMethod(ut::make_unique<CheckTTRWithFOVSensorsManagerHasAuxDataFields>());
         AddStaticMethod(ut::make_unique<CheckWMAIPlatformHasRequiredRWR>());
         AddStaticMethod(ut::make_unique<CheckWMAIPlatformDetectsEnemyTTRs>());
         AddStaticMethod(ut::make_unique<CheckWeaponsOnWMAIPlatformHavRequiredAuxData>());
         AddStaticMethod(ut::make_unique<CheckBattleManagerReachableBySubordinateSensors>());
         AddStaticMethod(ut::make_unique<CheckSensorsManagersMustNotConflict>());
         AddStaticMethod(ut::make_unique<CheckBattleManagerConnectedToSubordinateWeaponsManagerByC2CapablePlatforms>());
         AddStaticMethod(ut::make_unique<CheckBattleManagerConnectedToSubordinateSensorsManagerByC2CapablePlatforms>());
         AddStaticMethod(ut::make_unique<CheckBattleManagersDontConflict>());
         AddStaticMethod(ut::make_unique<CheckAssetManagersHaveUpdateInterval>());

      }
      ~ScenarioAnalyzerIADSC2ScriptClass() override = default;
};

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckPlatformWithSensorsManagerIsC2Capable, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkPlatformWithSensorsManagerIsC2Capable(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckPlatformWithWeaponsManagerIsC2Capable, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkPlatformWithWeaponsManagerIsC2Capable(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckPlatformWithBattleManagerIsC2Capable, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkPlatformWithBattleManagerIsC2Capable(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckPlatformWithSensorsManagerConnectedToBattleManagerWithCommitAuthority, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkPlatformWithSensorsManagerConnectedToBattleManagerWithCommitAuthority(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckPlatformWithWeaponsManagerConnectedToBattleManagerWithCommitAuthority, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkPlatformWithWeaponsManagerConnectedToBattleManagerWithCommitAuthority(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckPlatformWithSensorsManagerConnectedToTAROrTTR, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkPlatformWithSensorsManagerConnectedToTAROrTTR(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckPlatformWithTAROrTTRConnectedToSensorsManager, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkPlatformWithTAROrTTRConnectedToSensorsManager(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckPlatformWithWeaponsManagerHasWeapon, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkPlatformWithWeaponsManagerHasWeapon(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckPlatformWithWeaponHasWeaponsManager, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkPlatformWithWeaponHasWeaponsManager(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckPlatformWithDisseminateC2ManagerHasInternalCommLinks, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkPlatformWithDisseminateC2ManagerHasInternalCommLinks(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckPlatformWithWeaponsManagerHasAccessToRequiredSensor, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkPlatformWithWeaponsManagerHasAccessToRequiredSensor(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckPlatformWithAssetManagerHasDisseminateC2Manager, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkPlatformWithAssetManagerHasDisseminateC2Manager(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckTTRWithFOVSensorsManagerDoesNotManipulateOnOff, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkTTRWithFOVSensorsManagerDoesNotManipulateOnOff(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckTTRWithFOVSensorsManagerUsesDefaultSensorScheduler, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkTTRWithFOVSensorsManagerUsesDefaultSensorScheduler(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckTTRWithFOVSensorsManagerHasOneBeamPerMode, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkTTRWithFOVSensorsManagerHasOneBeamPerMode(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckTTRWithFOVSensorsManagerHasAuxDataFields, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkTTRWithFOVSensorsManagerHasAuxDataFields(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckWMAIPlatformHasRequiredRWR, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkWMAIPlatformHasRequiredRWR(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckWMAIPlatformDetectsEnemyTTRs, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkWMAIPlatformDetectsEnemyTTRs(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckWeaponsOnWMAIPlatformHavRequiredAuxData, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkWeaponsOnWMAIPlatformHavRequiredAuxData(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckBattleManagerReachableBySubordinateSensors, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkBattleManagerReachableBySubordinateSensors(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckSensorsManagersMustNotConflict, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkSensorsManagersMustNotConflict(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckBattleManagerConnectedToSubordinateWeaponsManagerByC2CapablePlatforms, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkBattleManagerConnectedToSubordinateWeaponsManagerByC2CapablePlatforms(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckBattleManagerConnectedToSubordinateSensorsManagerByC2CapablePlatforms, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkBattleManagerConnectedToSubordinateSensorsManagerByC2CapablePlatforms(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckBattleManagersDontConflict, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkBattleManagersDontConflict(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckAssetManagersHaveUpdateInterval, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkAssetManagersHaveUpdateInterval(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerIADSC2ScriptClass, ScenarioAnalyzerIADSC2, CheckMaxAquisitionTimeLongEnoughForSensorsToFormTracks, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   Messages messages = checkMaxAquisitionTimeLongEnoughForSensorsToFormTracks(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

class ScenarioAnalyzerIADSC2Extension : public WsfApplicationExtension
{
   public:
      void AddedToApplication(WsfApplication& app) override
      {
         UtScriptTypes* scriptTypes = app.GetScriptTypes();
         scriptTypes->Register(ut::make_unique<ScenarioAnalyzerIADSC2ScriptClass>(scriptTypes));
      }
};

void Register_wsf_scenario_analyzer_iads_c2(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("wsf_scenario_analyzer_iads_c2"))
   {
      aApplication.RegisterFeature("wsf_scenario_analyzer_iads_c2_plugin", "wsf_scenario_analyzer_iads_c2");
      aApplication.RegisterExtension("wsf_scenario_analyzer_iads_c2", ut::make_unique<ScenarioAnalyzerIADSC2Extension>());
      aApplication.ExtensionDepends("wsf_scenario_analyzer_iads_c2", "wsf_scenario_analyzer", true);
      aApplication.ExtensionDepends("wsf_scenario_analyzer_iads_c2", "wsf_iads_c2", true);
   }
}

extern "C"
{

   UT_PLUGIN_EXPORT void WsfPluginVersion(UtPluginVersion& version)
   {
      version = UtPluginVersion(
                   WSF_PLUGIN_API_MAJOR_VERSION, WSF_PLUGIN_API_MINOR_VERSION, WSF_PLUGIN_API_COMPILER_STRING);
   }

   UT_PLUGIN_EXPORT void WsfPluginSetup(WsfApplication& application)
   {
      Register_wsf_scenario_analyzer_iads_c2(application);
   }
}
