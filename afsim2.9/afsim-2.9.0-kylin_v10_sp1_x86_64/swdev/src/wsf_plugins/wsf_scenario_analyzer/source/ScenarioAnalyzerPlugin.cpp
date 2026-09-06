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
*   File: ScenarioAnalyzerPlugin.cpp
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

#if defined(_MSC_VER)
#pragma warning(push, 0)
#endif


#include <cstdio>
#include <functional>
#include <string>
#include <vector>

#include "UtMemory.hpp"
#include "UtPlugin.hpp"
#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"
#include "UtScriptData.hpp"
#include "UtScriptTypes.hpp"
#include "WsfAcousticSignature.hpp"

#include "WsfApplication.hpp"
#include "WsfArticulatedPart.hpp"
#include "WsfComm.hpp"
#include "WsfCommandChain.hpp"
#include "WsfComponentList.hpp"
#include "WsfDefaultSensorScheduler.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfExplicitWeapon.hpp"
#include "WsfImageProcessor.hpp"
#include "WsfInfraredSignature.hpp"
#include "WsfInherentContrast.hpp"
#include "WsfLinkedProcessor.hpp"
#include "WsfOpticalReflectivity.hpp"
#include "WsfOpticalSignature.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlugin.hpp"
#include "WsfProcessor.hpp"
#include "WsfRadarSensor.hpp"
#include "WsfRadarSignature.hpp"
#include "WsfSAR_Sensor.hpp"
#include "WsfScenario.hpp"
#include "script/WsfScriptContext.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSignatureInterface.hpp"
#include "WsfSignatureList.hpp"
#include "WsfSimulation.hpp"
#include "WsfTaskProcessor.hpp"
#include "WsfTrackProcessor.hpp"
#include "WsfWaypointMover.hpp"
#include "WsfWeapon.hpp"

#include "ScenarioAnalyzerPlugin.hpp"
#include "ScenarioAnalyzerMessage.hpp"
#include "ScenarioAnalyzerUtilities.hpp"
#include "UtScriptRef.hpp"

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

// ****************************************************************************
//                   Hidden Scenario Analyzer Methods
//       (Called by accessible methods and used to reduce copy-pasta)
// ****************************************************************************
// TODO: Reduce even more code copy-pasta in the actual functions (some do exactly the same thing in a couple places...)
namespace
{

// Helper function for checkPlatformHasRequiredSignatures
std::string sigIndexToString(int sigIndex)
{
   switch (sigIndex)
   {
      case 0:
         return "WsfRadarSignature";
      case 1:
         return "WsfOpticalSignature";
      case 2:
         return "WsfInfraredSignature";
      case 3:
         return "WsfOpticalReflectivity";
      case 4:
         return "WsfInherentContrast";
      case 5:
         return "WsfAcousticSignature";
      default:
         return "Unknown signature";
   }
}

static bool checkWeaponsNonzeroQuantityHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;

   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      unsigned weaponCount = platform->GetComponentCount<WsfWeapon>();
      for (unsigned j = 0; j != weaponCount; ++j)
      {
         auto weapon = platform->GetComponentEntry<WsfWeapon>(j);
         if (
            dynamic_cast<WsfExplicitWeapon*>(weapon)
            && weapon->GetInitialStateQuantityRemaining() <= 0.0)
         {
            passedCheck = false;

            message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
            message.AddScenarioFileLocation(ScenarioFileLocation("weapons", weapon->GetName()));
            message.SetDetails("Explicit weapon " + weapon->GetName() + " on platform " + platform->GetName() +
                               " has an initial quantity of zero. Use the 'quantity <real>' command to set a non-zero quantity.");
            messages.push_back(message);
            message.ClearDetails();
         }
      }
   }

   return passedCheck;
}

static bool checkSensorOnHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;

   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      unsigned sensorCount = platform->GetComponentCount<WsfSensor>();
      bool anySensorOn = false;
      for (unsigned j = 0; j < sensorCount; ++j)
      {
         auto sensor = platform->GetComponentEntry<WsfSensor>(j);
         if (sensor->IsTurnedOn())
         {
            anySensorOn = true;
            break;
         }
      }
      if (sensorCount > 0 && !anySensorOn)
      {
         passedCheck = false;

         message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
         message.SetDetails("All sensors on " + platform->GetName() + " are turned off. "
                            + "Use the 'on' command to turn on at least one of its sensors. "
                            + "(Note that it may not be appropriate to turn on TTRs.)");
         messages.push_back(message);
         message.ClearDetails();
      }
   }

   return passedCheck;
}

static bool checkSensorInternallyLinkedHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;

   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      unsigned sensorCount = platform->GetComponentCount<WsfSensor>();
      for (unsigned j = 0; j < sensorCount; ++j)
      {
         auto sensor = platform->GetComponentEntry<WsfSensor>(j);
         if (!sensor->HasInternalLinks())
         {
            passedCheck = false;

            message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
            message.AddScenarioFileLocation(ScenarioFileLocation("sensors", sensor->GetName()));
            message.SetDetails("Sensor " + sensor->GetName() + " on platform " + platform->GetName()
                               + " is not internally linked to any other platform component. "
                               + "Link the sensor to a processor, such as a track processor, using the 'internal_link' command.");
            messages.push_back(message);
            message.ClearDetails();
         }
      }
   }
   return passedCheck;
}

static bool checkSensorInternallyLinkedToTrackProcessorHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;

   std::string trackProcType = "WSF_TRACK_PROCESSOR";
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      unsigned sensorCount = platform->GetComponentCount<WsfSensor>();
      for (unsigned j = 0; j < sensorCount; ++j)
      {
         auto sensor = platform->GetComponentEntry<WsfSensor>(j);
         // SAR sensors should be linked to a WSF_IMAGE_PROCESSOR or WSF_VIDEO_PROCESSOR, both of which are
         // implemented with WsfImageProcessor. The WsfImageProcessor must be internally linked to a track processor.
         if (dynamic_cast<WsfSAR_Sensor*>(sensor))
         {
            bool connectedToImageProc = false;
            bool connectedToTrackProc = false;
            std::vector<WsfImageProcessor*> linkedImageProcessors = getLinkedAndReachablePlatformParts<WsfImageProcessor>(
                                                                       sensor, std::vector<std::string> {trackProcType}, false); // ignore external links from WsfTrackProcessors
            if (!linkedImageProcessors.empty())
            {
               connectedToImageProc = true;
            }
            for (WsfImageProcessor* imageProc : linkedImageProcessors)
            {
               std::vector<WsfTrackProcessor*> linkedTrackProcs = getLinkedAndReachablePlatformParts<WsfTrackProcessor>(
                                                                     imageProc, std::vector<std::string> {trackProcType}, false);
               if (!linkedTrackProcs.empty())
               {
                  connectedToTrackProc = true;
                  break;
               }
            }
            if (!connectedToTrackProc)
            {
               passedCheck = false;

               message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
               message.AddScenarioFileLocation(ScenarioFileLocation("sensors", sensor->GetName()));
               message.SetDetails("Sensor " + sensor->GetName() + " on platform " + platform->GetName() +
                                  " is a SAR, so it requires a direct or indirect link to a WSF_IMAGE_PROCESSOR or WSF_VIDEOPROCESSOR"
                                  " that is itself internally linked to a WSF_TRACK_PROCESSOR. ");
               if (connectedToImageProc)
               {
                  message.AddDetails("The sensor is connected to " + std::to_string(linkedImageProcessors.size()) + " video or image processor(s)"
                                     " but none of these are internally linked to a track processor.");
                  for (auto imageProc : linkedImageProcessors)
                  {
                     message.AddScenarioFileLocation(ScenarioFileLocation("processors", imageProc->GetName()));
                  }
               }
               else
               {
                  message.AddDetails("The sensor is not connected to any video or image processors");
               }

               messages.push_back(message);
               message.ClearDetails();
            }
         }
         // All other sensors should be linked to a track processor
         else
         {
            std::vector<WsfTrackProcessor*> linkedTrackProcessors = getLinkedAndReachablePlatformParts<WsfTrackProcessor>(
                                                                       sensor, std::vector<std::string> {trackProcType}, false); // ignore external links from other WsfTrackProcessors
            if (linkedTrackProcessors.empty())
            {
               passedCheck = false;

               message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
               message.AddScenarioFileLocation(ScenarioFileLocation("sensors", sensor->GetName()));
               message.SetDetails("Sensor " + sensor->GetName() + " on platform " + platform->GetName() +
                                  " does not have a direct or indirect link to a track processor.");
               messages.push_back(message);
               message.ClearDetails();
            }
         }
      }
   }
   return passedCheck;
}

static bool checkTrackProcessorHasPurgeIntervalHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      unsigned processorCount = platform->GetComponentCount<WsfProcessor>();
      for (unsigned j = 0; j < processorCount; ++j)
      {
         if (platform->GetComponentEntry<WsfProcessor>(j)->IsA_TypeOf("WSF_TRACK_PROCESSOR"))
         {
            auto trackProcessor = static_cast<WsfTrackProcessor*>(platform->GetComponentEntry<WsfProcessor>(j));
            if (trackProcessor->GetPurgeInterval() <= 0.0)
            {
               passedCheck = false;

               message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
               message.AddScenarioFileLocation(ScenarioFileLocation("processors", trackProcessor->GetName()));
               message.SetDetails("Track processor " + trackProcessor->GetName() + " on platform " + platform->GetName() +
                                  " has a track purging interval of 0 seconds, so tracks will not be purged. " +
                                  "Use 'purge_interval <time-value>' or 'drop_after_inactive <time-value>' to set a non-zero track purging interval.");
               messages.push_back(message);
               message.ClearDetails();
            }
         }
      }
   }
   return passedCheck;
}

static bool checkTrackProcessorsDontReportFusedTracksToEachOtherHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;

   std::string trackProcType = "WSF_TRACK_PROCESSOR";
   std::vector<std::pair<WsfTrackProcessor*, WsfTrackProcessor*>> discovered;

   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      unsigned processorCount = platform->GetComponentCount<WsfProcessor>();
      for (unsigned j = 0; j < processorCount; ++j)
      {
         if (platform->GetComponentEntry<WsfProcessor>(j)->IsA_TypeOf(trackProcType))
         {
            auto trackProcessor = static_cast<WsfTrackProcessor*>(platform->GetComponentEntry<WsfProcessor>(j));
            if (trackProcessor->GetReportFusedTracks())
            {
               // Get all connected track processors, but do NOT follow external links from other track processors.
               std::vector<WsfTrackProcessor*> connectedTrackProcessors = getLinkedAndReachablePlatformParts<WsfTrackProcessor>(
                                                                             trackProcessor, std::vector<std::string> {trackProcType}, false);
               {
                  for (WsfTrackProcessor* otherTrackProcessor : connectedTrackProcessors)
                  {
                     if (otherTrackProcessor->GetReportFusedTracks())
                     {
                        std::vector<WsfTrackProcessor*> otherConnectedTrackProcessors = getLinkedAndReachablePlatformParts<WsfTrackProcessor>(
                                                                                           otherTrackProcessor, std::vector<std::string> {trackProcType}, false);
                        {
                           if (find(otherConnectedTrackProcessors.begin(), otherConnectedTrackProcessors.end(), trackProcessor) !=
                               otherConnectedTrackProcessors.end())
                           {
                              passedCheck = false;

                              if (find(discovered.begin(), discovered.end(), std::pair<WsfTrackProcessor*, WsfTrackProcessor*>(otherTrackProcessor, trackProcessor)) == discovered.end())
                              {
                                 discovered.emplace_back(std::pair<WsfTrackProcessor*, WsfTrackProcessor*>(trackProcessor, otherTrackProcessor));

                                 message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
                                 message.AddScenarioFileLocation(ScenarioFileLocation("processors", trackProcessor->GetName()));
                                 message.AddScenarioFileLocation(ScenarioFileLocation("platform", otherTrackProcessor->GetPlatform()->GetName()));
                                 message.AddScenarioFileLocation(ScenarioFileLocation("processors", otherTrackProcessor->GetName()));
                                 message.SetDetails("Track processors " + trackProcessor->GetName() + " on platform " + platform->GetName()
                                                    + " and " + otherTrackProcessor->GetName() + " on platform " + otherTrackProcessor->GetPlatform()->GetName()
                                                    + " report fused tracks to each other.");

                                 messages.push_back(message);
                                 message.ClearDetails();
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }
   return passedCheck;
}

static bool checkPurgeIntervalLongEnoughToMaintainTrackHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   std::string trackProcType = "WSF_TRACK_PROCESSOR";
   size_t platformCount = sim.GetPlatformCount();

   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      unsigned sensorCount = platform->GetComponentCount<WsfSensor>();
      for (unsigned j = 0; j < sensorCount; ++j)
      {
         auto sensor = platform->GetComponentEntry<WsfSensor>(j);
         std::vector<WsfTrackProcessor*> linkedTrackProcessors = getLinkedAndReachablePlatformParts<WsfTrackProcessor>(
                                                                    sensor, std::vector<std::string> {trackProcType}, false); // Ignore external links from other WsfTrackProcessor's
         for (WsfTrackProcessor* trackProc : linkedTrackProcessors)
         {
            double purgeInterval = trackProc->GetPurgeInterval();

            size_t modeCount = sensor->GetModeCount();
            for (size_t k = 0; k != modeCount; ++k)
            {
               WsfSensorMode* mode = sensor->GetModeEntry(k);
               double absoluteMinTime = mode->GetFrameTime() * mode->HitsToMaintainTrack();
               double detectionWindowTime = mode->GetFrameTime() * mode->MaintainTrackWindow();

               // Track processor will purge data from sensor using this mode before it can possibly receive
               // the minimum number of hits required to establish a track.
               if (absoluteMinTime > purgeInterval && purgeInterval > 0.0)
               {
                  passedCheck = false;

                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", trackProc->GetPlatform()->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("processors", trackProc->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", sensor->GetPlatform()->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("sensors", sensor->GetName()));
                  message.SetDetails("Sensor " + sensor->GetName() + " on " + sensor->GetPlatform()->GetName() + " has a mode "
                                     + mode->GetName() + " that requires " + std::to_string(absoluteMinTime)
                                     + " seconds at a very minimum to get enough hits to maintain a track,"
                                     + " but the sensor eventually reports to track processor " + trackProc->GetName() + " on platform "
                                     + trackProc->GetPlatform()->GetName() + ", which has a purge interval of only "
                                     + std::to_string(purgeInterval) + " seconds. To address this, (1) decrease the mode's 'frame_time',"
                                     + " (2) decrease the first value for 'hits_to_maintain_track', or (3) increase the 'purge_interval'.");
                  message.SetSeverity(ScenarioAnalyzerMessage::cERROR);
                  messages.push_back(message);
                  message.ClearDetails();
               }
               // Track processor will purge data from sensor using this mode before the establish track window completes, making it
               // possibly but unlikely that it will receive the minimum number of hits required to establish a track.
               else if (detectionWindowTime > purgeInterval && purgeInterval != 0)
               {
                  passedCheck = false;

                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", trackProc->GetPlatform()->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("processors", trackProc->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", sensor->GetPlatform()->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("sensors", sensor->GetName()));
                  message.SetDetails("Sensor " + sensor->GetName() + " on " + sensor->GetPlatform()->GetName() + " has a mode "
                                     + mode->GetName() + " with a detection window time of " + std::to_string(detectionWindowTime) + " seconds,"
                                     + " but the sensor eventually reports to track processor " + trackProc->GetName() + " on platform " + trackProc->GetPlatform()->GetName()
                                     + ", which has a purge interval of only " + std::to_string(purgeInterval)
                                     + " seconds. Tracks may be formed, but the odds are low. To address this,"
                                     + " (1) decrease the mode's 'frame_time', (2) decrease the second value for 'hits_to_maintain_track', or"
                                     + " (3) increase the 'purge_interval'.");
                  message.SetSeverity(ScenarioAnalyzerMessage::cWARNING);
                  messages.push_back(message);
                  message.ClearDetails();
               }
            }
         }
      }
   }
   return passedCheck;
}

static bool checkPurgeIntervalLongEnoughToEstablishTrackHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   std::string trackProcType = "WSF_TRACK_PROCESSOR";
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      unsigned sensorCount = platform->GetComponentCount<WsfSensor>();
      for (unsigned j = 0; j < sensorCount; ++j)
      {
         auto sensor = platform->GetComponentEntry<WsfSensor>(j);
         std::vector<WsfTrackProcessor*> linkedTrackProcessors = getLinkedAndReachablePlatformParts<WsfTrackProcessor>(
                                                                    sensor, std::vector<std::string> {trackProcType}, false); // Ignore external links from other WsfTrackProcessor's
         for (WsfTrackProcessor* trackProc : linkedTrackProcessors)
         {
            std::cout << "linked track processor: " << trackProc->GetName() << " on platform " << trackProc->GetPlatform()->GetName() << std::endl;
            double purgeInterval = trackProc->GetPurgeInterval();

            size_t modeCount = sensor->GetModeCount();
            for (size_t k = 0; k != modeCount; ++k)
            {
               WsfSensorMode* mode = sensor->GetModeEntry(k);
               double absoluteMinTime = mode->GetFrameTime() * mode->HitsToEstablishTrack();
               double detectionWindowTime = mode->GetFrameTime() * mode->EstablishTrackWindow();

               // Track processor will purge data from sensor using this mode before it can possibly receive
               // the minimum number of hits required to establish a track.
               if (absoluteMinTime > purgeInterval && purgeInterval > 0.0)
               {
                  passedCheck = false;

                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", trackProc->GetPlatform()->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("processors", trackProc->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", sensor->GetPlatform()->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("sensors", sensor->GetName()));
                  message.SetDetails("Sensor " + sensor->GetName() + " on " + sensor->GetPlatform()->GetName() + " has a mode "
                                     + mode->GetName() + " that requires " + std::to_string(absoluteMinTime)
                                     + " seconds at a very minimum to get enough hits to establish a track,"
                                     + " but the sensor eventually reports to track processor " + trackProc->GetName() + " on platform "
                                     + trackProc->GetPlatform()->GetName() + ", which has a purge interval of only "
                                     + std::to_string(purgeInterval) + " seconds. To address this, (1) decrease the mode's 'frame_time',"
                                     + " (2) decrease the first value for 'hits_to_establish_track', or (3) increase the 'purge_interval'.");
                  message.SetSeverity(ScenarioAnalyzerMessage::cERROR);
                  messages.push_back(message);
                  message.ClearDetails();
               }
               // Track processor will purge data from sensor using this mode before the establish track window completes, making it
               // possibly but unlikely that it will receive the minimum number of hits required to establish a track.
               else if (detectionWindowTime > purgeInterval && purgeInterval != 0)
               {
                  passedCheck = false;

                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", trackProc->GetPlatform()->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("processors", trackProc->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", sensor->GetPlatform()->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("sensors", sensor->GetName()));
                  message.SetDetails("Sensor " + sensor->GetName() + " on " + sensor->GetPlatform()->GetName() + " has a mode "
                                     + mode->GetName() + " with a detection window time of " + std::to_string(detectionWindowTime) + " seconds,"
                                     + " but the sensor eventually reports to track processor " + trackProc->GetName() + " on platform " + trackProc->GetPlatform()->GetName()
                                     + ", which has a purge interval of only " + std::to_string(purgeInterval)
                                     + " seconds. Tracks may be formed, but the odds are low. To address this,"
                                     + " (1) decrease the mode's 'frame_time', (2) decrease the second value for 'hits_to_maintain_track', or"
                                     + " (3) increase the 'purge_interval'.");
                  message.SetSeverity(ScenarioAnalyzerMessage::cWARNING);
                  messages.push_back(message);
                  message.ClearDetails();
               }
            }
         }
      }
   }
   return passedCheck;
}

static bool checkCommInternallyLinkedHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      unsigned commCount = platform->GetComponentCount<wsf::comm::Comm>();
      for (unsigned j = 0; j < commCount; ++j)
      {
         wsf::comm::Comm* comm = platform->GetComponentEntry<wsf::comm::Comm>(j);
         if (!comm->HasInternalLinks())
         {
            passedCheck = false;

            message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
            message.AddScenarioFileLocation(ScenarioFileLocation("comms", comm->GetName()));
            message.SetDetails("Comm " + comm->GetName() + " on platform " + platform->GetName()
                               + " is not internally linked to any other platform component. "
                               + "Link the comm to a processor such as the track manager using the 'internal_link' command.");
            messages.push_back(message);
            message.ClearDetails();
         }
      }
   }
   return passedCheck;
}

static bool checkPlatformInCommandChainHasCommHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      if (platform->GetComponentCount<WsfCommandChain>() != 0 && platform->GetComponentCount<wsf::comm::Comm>() == 0)
      {
         // Ignore default command chains and command chains where the platform is the commander and has no subordinates
         unsigned numRealCommandChains = 0;
         unsigned chainCount = platform->GetComponentCount<WsfCommandChain>();
         for (unsigned j = 0; j < chainCount; ++j)
         {
            WsfCommandChain* commandChain = platform->GetComponentEntry<WsfCommandChain>(j);
            if (commandChain->GetNameId() != WsfCommandChain::GetDefaultNameId() ||
                commandChain->GetCommanderNameId() != platform->GetNameId() ||
                !commandChain->GetSubordinates().empty() ||
                !commandChain->GetPeers().empty())
            {
               numRealCommandChains += 1;
            }
         }

         if (numRealCommandChains > 0)
         {
            passedCheck = false;

            message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
            message.SetDetails("Platform " + platform->GetName() + " is part of a command chain but has no comm devices. "
                               + "Ensure the platform communicates with other members of its command chain.");
            messages.push_back(message);
            message.ClearDetails();
         }
      }
   }
   return passedCheck;
}

static bool checkLargeScenarioHasCommandChainHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   size_t threshold = 10;
   if (platformCount > threshold)
   {
      bool realCommandChainExists = false;

      for (size_t i = 0; i < platformCount; ++i)
      {
         WsfPlatform* platform = sim.GetPlatformEntry(i);
         unsigned chainCount = platform->GetComponentCount<WsfCommandChain>();
         for (unsigned j = 0; j < chainCount; ++j)
         {
            auto commandChain = platform->GetComponentEntry<WsfCommandChain>(j);
            if (commandChain->GetCommanderNameId() != platform->GetNameId() || !commandChain->GetSubordinates().empty() || !commandChain->GetPeers().empty())
            {
               realCommandChainExists = true;
               break;
            }
         }
      }

      if (!realCommandChainExists)
      {
         passedCheck = false;

         message.SetDetails("Simulation contains more than " + std::to_string(threshold) + " platforms, but no command chain of more than one platform exists. "
                            + "Consider organizing platforms into a command chain: use 'commander <platform-name>' to designate a platform's commander.");
         messages.push_back(message);
         message.ClearDetails();
      }
   }
   return passedCheck;
}

static bool checkScriptProcessorHasUpdateIntervalHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      unsigned processorCount = platform->GetComponentCount<WsfProcessor>();
      for (unsigned j = 0; j < processorCount; ++j)
      {
         WsfProcessor* processor = platform->GetComponentEntry<WsfProcessor>(j);
         if ((processor->IsA_TypeOf("WSF_SCRIPT_PROCESSOR") || processor->IsA_TypeOf("WSF_QUANTUM_TASKER_PROCESSOR"))
             && processor->GetUpdateInterval() <= 0)
         {
            passedCheck = false;

            message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
            message.AddScenarioFileLocation(ScenarioFileLocation("processors", processor->GetName()));
            message.SetDetails("Script processor " + processor->GetName() + " on platform " + platform->GetName()
                               + " has an update interval <= 0, so it will never be updated. "
                               + "Use the 'update_interval <time-reference>' command to set this value.");
            messages.push_back(message);
            message.ClearDetails();
         }
      }
   }
   return passedCheck;
}

static bool checkDeclaredCommandChainHasStructureHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      unsigned commandChainCount = platform->GetComponentCount<WsfCommandChain>();
      for (unsigned j = 0; j < commandChainCount; ++j)
      {
         WsfCommandChain* commandChain = platform->GetComponentEntry<WsfCommandChain>(j);
         // If command chain doesn't have the default name, it must have been explicitly declared as a command chain by the user,
         // so we need to check that it has some structure.
         if (commandChain->GetNameId() != commandChain->GetDefaultNameId())
         {
            // No structure at this node in command chain if platform has no distinct commander, no subs, and no peers. [TODO: don't think we need to check peers]
            if (commandChain->GetCommanderNameId() == platform->GetNameId() && commandChain->GetSubordinates().empty() && commandChain->GetPeers().empty())
            {
               passedCheck = false;

               message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
               message.SetDetails("Command chain " + commandChain->GetName() + " has no structure: Platform " + platform->GetName()
                                  + " is its own commander and has no subordinates or peers.");
               messages.push_back(message);
               message.ClearDetails();
            }
         }
      }
   }
   return passedCheck;
}

static bool checkCommanderInDeclaredCommandChainHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      unsigned commandChainCount = platform->GetComponentCount<WsfCommandChain>();
      for (unsigned j = 0; j < commandChainCount; ++j)
      {
         WsfCommandChain* commandChain = platform->GetComponentEntry<WsfCommandChain>(j);
         // If command chain doesn't have the default name, it must have been explicitly declared as a command chain by the user.
         if (commandChain->GetNameId() != commandChain->GetDefaultNameId())
         {
            // We need to check that the commander is also in the declared command chain
            if (commandChain->GetCommanderNameId() != platform->GetNameId())
            {
               bool commanderInChain = false;

               WsfStringId ccName = commandChain->GetNameId();
               WsfPlatform* commander = commandChain->GetCommander();
               unsigned count = commander->GetComponentCount<WsfCommandChain>();
               for (unsigned k = 0; k < count; ++k)
               {
                  if (commander->GetComponentEntry<WsfCommandChain>(k)->GetNameId() == ccName)
                  {
                     commanderInChain = true;
                     break;
                  }

               }

               if (!commanderInChain)
               {
                  passedCheck = false;

                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
                  message.AddScenarioFileLocation(ScenarioFileLocation("platform", commander->GetName()));
                  message.SetDetails("Platform " + platform->GetName() + " identifies " + commander->GetName() + " as its commander in command chain "
                                     + commandChain->GetName() + ", but " + commander->GetName() + " is not in that command chain.");
                  messages.push_back(message);
                  message.ClearDetails();
               }
            }
         }
      }
   }
   return passedCheck;
}

static bool checkPlatformHasRequiredSignaturesHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   using SensorChecksContainer = std::vector<std::function<bool(WsfSensor*)>>;

   bool passedCheck = true;

   // TODO: check is WsfMil is registered. If it isn't, signature types aren't added to the prototype sig list (or, as a result, platform sig lists).

   std::vector<int> signatureIndices
   {
      WsfAcousticSignature::cSIGNATURE_INDEX,
      WsfInfraredSignature::cSIGNATURE_INDEX,
      WsfInherentContrast::cSIGNATURE_INDEX,
      //WsfOpticalReflectivity::cSIGNATURE_INDEX, // TODO: not sure that we should give warning for omitting optical reflectivity since there's a default
      WsfOpticalSignature::cSIGNATURE_INDEX,
      WsfRadarSignature::cSIGNATURE_INDEX
   };

   SensorChecksContainer requiresAcousticSig;
   requiresAcousticSig.emplace_back([](WsfSensor * sensor) { return sensor->IsClassAcoustic(); });

   // TODO: more accurate to check if any sensor has a WsfEOIR_IRST_SensorMode
   SensorChecksContainer requiresInfraredSig;
   requiresInfraredSig.emplace_back([](WsfSensor * sensor) { return sensor->IsClassInfrared(); });
   requiresInfraredSig.emplace_back([](WsfSensor * sensor) { return sensor->IsA_TypeOf("WSF_EOIR_SENSOR"); });
   requiresInfraredSig.emplace_back([](WsfSensor * sensor) { return sensor->IsA_TypeOf("WSF_IRST_SENSOR"); });

   SensorChecksContainer requiresInherentContrast;
   requiresInherentContrast.emplace_back([](WsfSensor * sensor) { return sensor->IsClassVisual(); });
   requiresInherentContrast.emplace_back([](WsfSensor * sensor) { return sensor->IsA_TypeOf("WSF_IRST_SENSOR"); });

   /*   SensorChecksContainer requiresOpticalReflectivity;
      requiresOpticalReflectivity.emplace_back([](WsfSensor* sensor) { return sensor->IsA_TypeOf("WSF_LADAR_SENSOR");  })*/;

   SensorChecksContainer requiresOpticalSig;
   requiresOpticalSig.emplace_back([](WsfSensor * sensor) { return sensor->IsClassVisual(); });
   requiresOpticalSig.emplace_back([](WsfSensor * sensor) { return sensor->IsA_TypeOf("WSF_BEAM_DIRECTOR"); });
   requiresOpticalSig.emplace_back([](WsfSensor * sensor) { return sensor->IsA_TypeOf("WSF_LADAR_SENSOR"); });
   requiresOpticalSig.emplace_back([](WsfSensor * sensor) { return sensor->IsA_TypeOf("WSF_IRST_SENSOR");  });
   requiresOpticalSig.emplace_back([](WsfSensor * sensor) { return sensor->IsA_TypeOf("WSF_SAR_SENSOR"); });

   SensorChecksContainer requiresRadarSig;
   requiresRadarSig.emplace_back([](WsfSensor * sensor) { return sensor->IsClassRadio(); });

   std::vector<SensorChecksContainer> sensorChecks{ requiresAcousticSig,
                                                    requiresInfraredSig,
                                                    requiresInherentContrast, /*requiresOpticalReflectivity,*/
                                                    requiresOpticalSig,
                                                    requiresRadarSig };

   // Function to check that all platforms have the required signature
   auto checkPlatformsHaveSignature = [&](int aSignatureIndex, WsfPlatform* aTriggeringPlatform, WsfSensor* aTriggeringSensor)
   {
      // If signature is required, check that each platform has that signature
      bool passedCheck = true;
      for (unsigned int i = 0; i < sim.GetPlatformCount(); ++i)
      {
         WsfPlatform* platform = sim.GetPlatformEntry(i);
         WsfSignatureList& signatureList = platform->GetSignatureList();
         WsfSignatureInterface* sigInterface = signatureList.GetInterface(aSignatureIndex); // Index into signatureIndices be valid as long as WsfMil extension registered
         if (!sigInterface->GetSignature())
         {
            passedCheck = false;

            message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
            message.AddScenarioFileLocation(ScenarioFileLocation("platform", aTriggeringPlatform->GetName()));
            message.AddScenarioFileLocation(ScenarioFileLocation("sensors", aTriggeringSensor->GetName()));
            message.SetDetails("Sensor " + aTriggeringSensor->GetName() + " on platform " + aTriggeringPlatform->GetName()
                               + " relies on platforms having a signature of type " + sigIndexToString(aSignatureIndex) + ", but platform "
                               + platform->GetName() + " doesn't have a signature of that type.");
            messages.push_back(message);
            message.ClearDetails();
         }
      }
      return passedCheck;
   };

   // Function to find if a sensor requires a signature. If found, calls a callback to verify all platforms have the required signature
   auto checkSensorSignatures = [&](int aSignatureIndex, const SensorChecksContainer& aChecks, std::function<bool(int, WsfPlatform*, WsfSensor*)> aCheckCallback)
   {
      for (size_t i = 0; i < sim.GetPlatformCount(); ++i)
      {
         // Check if any sensor on the platform requires the signature (i.e., whether sensor triggers any of the relevant sensor checks)
         WsfPlatform* platform = sim.GetPlatformEntry(i);
         unsigned sensorCount = platform->GetComponentCount<WsfSensor>();
         for (unsigned j = 0; j < sensorCount; ++j)
         {
            auto sensor = platform->GetComponentEntry<WsfSensor>(j);
            for (auto& check : aChecks)
            {
               if (check(sensor))
               {
                  return aCheckCallback(aSignatureIndex, platform, sensor);
               }
            }
         }
      }
      return true;
   };

   // For each signature that might be required:
   for (size_t i = 0; i < signatureIndices.size(); ++i)
   {
      passedCheck &= checkSensorSignatures(signatureIndices[i], sensorChecks[i], checkPlatformsHaveSignature);
   }
   return passedCheck;
}

static bool checkSignaturesDetectableByEnemySensorHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   std::vector<int> signatureIndices
   {
      WsfAcousticSignature::cSIGNATURE_INDEX,
      WsfInfraredSignature::cSIGNATURE_INDEX,
      WsfInherentContrast::cSIGNATURE_INDEX,
      //WsfOpticalReflectivity::cSIGNATURE_INDEX, // TODO: not sure that we should give warning for omitting optical reflectivity since there's a default
      WsfOpticalSignature::cSIGNATURE_INDEX,
      WsfRadarSignature::cSIGNATURE_INDEX };

   using SensorChecksContainer = std::vector<std::function<bool(WsfSensor*)>>;

   SensorChecksContainer requiresAcousticSig;
   requiresAcousticSig.emplace_back([](WsfSensor * sensor) { return sensor->IsClassAcoustic(); });

   // TODO: more accurate to check if any sensor has a WsfEOIR_IRST_SensorMode
   SensorChecksContainer requiresInfraredSig;
   requiresInfraredSig.emplace_back([](WsfSensor * sensor) { return sensor->IsClassInfrared(); });
   requiresInfraredSig.emplace_back([](WsfSensor * sensor) { return sensor->IsA_TypeOf("WSF_EOIR_SENSOR"); });
   requiresInfraredSig.emplace_back([](WsfSensor * sensor) { return sensor->IsA_TypeOf("WSF_IRST_SENSOR"); });

   SensorChecksContainer requiresInherentContrast;
   requiresInherentContrast.emplace_back([](WsfSensor * sensor) { return sensor->IsClassVisual(); });
   requiresInherentContrast.emplace_back([](WsfSensor * sensor) { return sensor->IsA_TypeOf("WSF_IRST_SENSOR"); });

   /*   SensorChecksContainer requiresOpticalReflectivity;
   requiresOpticalReflectivity.emplace_back([](WsfSensor* sensor) { return sensor->IsA_TypeOf("WSF_LADAR_SENSOR");  })*/;

   SensorChecksContainer requiresOpticalSig;
   requiresOpticalSig.emplace_back([](WsfSensor * sensor) { return sensor->IsClassVisual(); });
   requiresOpticalSig.emplace_back([](WsfSensor * sensor) { return sensor->IsA_TypeOf("WSF_BEAM_DIRECTOR"); });
   requiresOpticalSig.emplace_back([](WsfSensor * sensor) { return sensor->IsA_TypeOf("WSF_LADAR_SENSOR"); });
   requiresOpticalSig.emplace_back([](WsfSensor * sensor) { return sensor->IsA_TypeOf("WSF_IRST_SENSOR");  });
   requiresOpticalSig.emplace_back([](WsfSensor * sensor) { return sensor->IsA_TypeOf("WSF_SAR_SENSOR"); });

   SensorChecksContainer requiresRadarSig;
   requiresRadarSig.emplace_back([](WsfSensor * sensor) { return sensor->IsClassRadio(); });

   std::vector<SensorChecksContainer> sensorChecks{ requiresAcousticSig,
                                                    requiresInfraredSig,
                                                    requiresInherentContrast, /*requiresOpticalReflectivity,*/
                                                    requiresOpticalSig,
                                                    requiresRadarSig };

   int maxSigIndex = *(std::max_element(signatureIndices.begin(), signatureIndices.end()));
   std::map<WsfStringId, std::vector<int>> signaturesUsedByEachSide;

   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      // Keep track of the signatures used by each of the platform's sensors
      unsigned sensorCount = platform->GetComponentCount<WsfSensor>();
      for (unsigned j = 0; j != sensorCount; ++j)
      {
         WsfSensor* sensor = platform->GetComponentEntry<WsfSensor>(j);
         for (size_t k = 0, sz = sensorChecks.size(); k != sz; ++k)
         {
            for (const auto& check : sensorChecks[k])
            {
               // We've identified a signature that this sensor uses
               if (check(sensor))
               {
                  WsfStringId sideId = platform->GetSideId();
                  // First time we've encountered a platform on this side: set up the side's signature indices vector
                  if (signaturesUsedByEachSide.find(sideId) == signaturesUsedByEachSide.end())
                  {
                     signaturesUsedByEachSide.emplace(sideId, std::vector<int>(maxSigIndex + 1));
                  }
                  // Mark that we've found a sensor on this side that uses the given signature
                  std::vector<int>& signaturesUsed = signaturesUsedByEachSide.at(sideId);
                  ++signaturesUsed[signatureIndices[k]];

                  break;
               }
            }
         }
      }
   }

   // For each platform, ensure that each of its signatures will be used by at least on enemy sensor:
   for (unsigned m = 0; m != platformCount; ++m)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(m);
      WsfStringId sideId = platform->GetSideId();
      WsfSignatureList& signatureList = platform->GetSignatureList();

      bool platformDetectable = false;
      bool hasUndetectableSig = false;
      for (auto sigIndex : signatureIndices)
      {
         // For each signature that the platform has...
         if (signatureList.GetInterface(sigIndex)->GetSignature())
         {
            bool signatureDetectable = false;
            for (const auto& entry : signaturesUsedByEachSide)
            {
               // ... ensure that an enemy side has at least one sensor that uses that signature
               if (entry.first != sideId && entry.second.at(sigIndex) > 0)
               {
                  signatureDetectable = true;
                  platformDetectable = true;
                  break;
               }
            }
            if (!signatureDetectable)
            {
               passedCheck = false;
               hasUndetectableSig = true;
               message.AddDetails("Platform " + platform->GetName() + " has a signature of type " + sigIndexToString(sigIndex) +
                                  ", but no enemy platform has a sensor that detects this kind of signature. ");
            }
         }
      }
      if (!platformDetectable)
      {
         passedCheck = false;
         message.AddDetails("Platform " + platform->GetName() + " has no signatures that are detectable by any enemy sensors. "
                            + "As a result, no enemy platforms will be able to detect this platform or form tracks on it.");
      }

      if (!platformDetectable || hasUndetectableSig)
      {
         message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
         messages.push_back(message);
         message.ClearDetails();
      }
   }
   return passedCheck;
}

static bool checkPlatformHasMeaningfulLocationHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      double lat, lon, alt;
      platform->GetLocationLLA(lat, lon, alt);
      if (lat == 0.0 && lon == 0.0 && alt == 0.0)
      {
         passedCheck = false;

         message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
         message.SetDetails("Platform " + platform->GetName() +
                            " is located at the default position of (0.0, 0.0, 0.0) LLA. " +
                            "Specify a meaningful location using the 'position' command.");
         messages.push_back(message);
         message.ClearDetails();
      }
   }
   return passedCheck;
}

static bool checkUserConfiguredSpeedsWithinMoverCapabilitiesHelper(WsfSimulation& sim, Messages& messages, ScenarioAnalyzerMessage& message)
{
   bool passedCheck = true;
   size_t platformCount = sim.GetPlatformCount();
   for (size_t i = 0; i < platformCount; ++i)
   {
      WsfPlatform* platform = sim.GetPlatformEntry(i);
      WsfMover* mover = platform->GetMover();
      // WsfWaypointMover is currently the only class derived from WsfRouteMover that has min/max speed constraints
      // and allows users to configure speeds at waypoints, so we're only interested in movers of this type.
      if (mover && dynamic_cast<WsfWaypointMover*>(mover))
      {
         auto waypointMover = static_cast<WsfWaypointMover*>(mover);
         double maxSpeed = waypointMover->GetConstraints().mMaxSpeed;
         double minSpeed = waypointMover->GetConstraints().mMinSpeed;

         const WsfRoute* route = waypointMover->GetRoute();
         unsigned waypointCount = route->GetSize();
         for (unsigned j = 0; j != waypointCount; ++j)
         {
            const WsfWaypoint& waypoint = route->GetWaypointAt(j);
            // Get user-configured speed at waypoint
            double speed = waypoint.GetSpeed();
            // Setting and accessing mover speed constraints and waypoints speeds involves converting
            // between different units of speed. Using epsilon when comparing user-configured speeds with
            // min and max speeds avoids false positives.
            double epsilon = 0.0001;
            // If user doesn't specify speed, mSpeed will be <= 0: Ensure that speed > 0
            // to limit this check to explicitly defined speeds. This means that the check will not
            // catch instances where user explicitly sets speed to 0 or a negative number.
            if (speed > 0 && (minSpeed - speed > epsilon))
            {
               passedCheck = false;

               message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
               message.SetDetails("The speed set for at least one of the waypoints in platform " + platform->GetName()
                                  + "'s route is less than the mover's minimum speed of " + std::to_string(minSpeed) + " meters/second.");
               messages.push_back(message);
               message.ClearDetails();
            }
            else if (speed - maxSpeed > epsilon)
            {
               passedCheck = false;

               message.AddScenarioFileLocation(ScenarioFileLocation("platform", platform->GetName()));
               message.SetDetails("The speed set for at least one of the waypoints in platform " + platform->GetName()
                                  + "'s route is greater than the mover's maximum speed of " + std::to_string(maxSpeed) + " meters/second.");
               messages.push_back(message);
               message.ClearDetails();
            }
         }
      }
   }
   return passedCheck;
}

}

// ****************************************************************************
//              Accessible Scenario Analyzer Methods
// ****************************************************************************
void notifyOfPlatformsNotPresentInSimulation(WsfSimulation& sim)
{
   const WsfScenario& scenario = sim.GetScenario();
   unsigned inputPlatformCount = scenario.GetInputPlatformCount();
   if (inputPlatformCount > sim.GetPlatformCount())
   {
      // At least some platforms that may be added to the simulation later are
      // not yet present. This may be because platform's creation time is later
      // that the sim time, or because the platform has already been deleted.
      for (unsigned i = 0; i != inputPlatformCount; ++i)
      {
         WsfPlatform* inputPlatform = scenario.GetInputPlatformEntry(i);
         if (inputPlatform)
         {
            WsfPlatform* activePlatform = sim.GetPlatformByName(inputPlatform->GetNameId());
            if (!activePlatform)
            {
               std::string detailedMessage = "Platform " + inputPlatform->GetName() + " is not present in the simulation at the time that the checks are being run: ";

               // The input platform is not currently active in the scenario, which means that
               // it will be invisible to any checks we run. If it's missing because it has a
               // later creation time or has already been destroyed, send user a message explaining.
               if (inputPlatform->GetCreationTime() >= sim.GetSimTime() && sim.GetSimTime() <= 1)
               {
                  detailedMessage += "This is likely because the platform's creation time is " + std::to_string(inputPlatform->GetCreationTime()) +
                                     " seconds, while tool's default behavior is to run all selected checks immediately after the simulation is initialized. " +
                                     "You may adjust the time at which checks are run to a time after this platform's creation time by entering a time to execute checks in the text box next to the 'Run Checks' button. " +
                                     "(Note that other platforms may get deleted if you advance the simulation time.)";
               }
               else if (inputPlatform->GetCreationTime() >= sim.GetSimTime())
               {
                  detailedMessage += "This is likely because the platform's creation time is " + std::to_string(inputPlatform->GetCreationTime()) +
                                     " seconds, while the tool is currently set to run all selected checks at simulation time " + std::to_string(sim.GetSimTime()) +
                                     " seconds. You may adjust the time at which checks are run to a time after this platform's creation time. " +
                                     "(Note that other platforms may get deleted if you advance the simulation time.)";
               }
               else if (sim.GetSimTime() > 0)
               {
                  detailedMessage += "This may be because the platform has already been deleted. The checks are currently being run at a sim time of " +
                                     std::to_string(sim.GetSimTime()) + ". You may adjust the time at which checks are run to an earlier time.";
               }
               else
               {
                  detailedMessage += "This is likely because the platform was found to be 'unavailable' when the simulation was set up. ";
               }

               ScenarioAnalyzerMessage message("Missing Platforms", detailedMessage);
               message.Write();
            }
         }
      }
   }
}

Messages checkWeaponsNonzeroQuantity(WsfSimulation& sim)
{
   return runSuiteCheck("Core",
                        "Deployed weapons should have quantity greater than zero",
                        ScenarioAnalyzerMessage::cWARNING,
                        sim,
                        checkWeaponsNonzeroQuantityHelper);
}

// This check works differently depending on whether or not IADS C2 is being used
// in the scenario. If a platform is not part of an IADS C2 system, the check will give
// a warning if a platform has one or more sensors and no sensor is turned on.
// If a platform is part of an IADS C2 system, the check will also check the
// categories of its sensors (TTR, TAR, EW, RWR) and will give a warning if the
// platform has one or more non-TTR sensors, all which are turned off. For the
// purposes of this check, a platform is "part of an IADS C2 system" if its
// sensors are managed by a sensors manager.
extern Messages checkSensorOn(WsfSimulation& sim)
{
   return runSuiteCheck("Core", "Sensor platforms should have at least one sensor turned on",
                        ScenarioAnalyzerMessage::cWARNING, sim, checkSensorOnHelper);
}

Messages checkSensorInternallyLinked(WsfSimulation& sim)
{
   return runSuiteCheck("Core", "Sensors must be internally linked",
                        ScenarioAnalyzerMessage::cERROR, sim, checkSensorInternallyLinkedHelper);
}

Messages checkSensorInternallyLinkedToTrackProcessor(WsfSimulation& sim)
{
   return runSuiteCheck("Core", "Sensors should be linked to track processor",
                        ScenarioAnalyzerMessage::cWARNING, sim, checkSensorInternallyLinkedToTrackProcessorHelper);
}

Messages checkTrackProcessorHasPurgeInterval(WsfSimulation& sim)
{
   return runSuiteCheck("Core", "Track processors should have purge interval defined",
                        ScenarioAnalyzerMessage::cWARNING, sim, checkTrackProcessorHasPurgeIntervalHelper);
}

// If track processor has fused_track_reporting enabled, check its external links.
// Warn if: track processor A and track processor B are mutually "linked and reachable"
// when ignoring external links on other track processors, and both A and B report
// fused tracks.
Messages checkTrackProcessorsDontReportFusedTracksToEachOther(WsfSimulation& sim)
{
   return runSuiteCheck("Core", "Track processors should not circularly report fused tracks",
                        ScenarioAnalyzerMessage::cWARNING, sim, checkTrackProcessorsDontReportFusedTracksToEachOtherHelper);
}

// For each sensor, find all track processors that the sensor is directly and indirectly linked to.
// For each of those linked track processors, check that the purge interval is not so short that it
// will prevent tracks from being maintained from data generated by that sensor. When checking the time
// needed by the sensor, consider each sensor mode.
Messages checkPurgeIntervalLongEnoughToMaintainTrack(WsfSimulation& sim)
{
   return runSuiteCheck("Core", "Track processor purging intervals must be long enough to maintain tracks",
                        ScenarioAnalyzerMessage::cERROR, sim, checkPurgeIntervalLongEnoughToMaintainTrackHelper);
}

// For each sensor, find all track processors that the sensor is directly and indirectly linked to.
// For each of those linked track processors, check that the purge interval is not so short that it
// will prevent tracks from being formed from data generated by that sensor. When checking the time
// needed by the sensor, consider each sensor mode.
Messages checkPurgeIntervalLongEnoughToEstablishTrack(WsfSimulation& sim)
{
   return runSuiteCheck("Core", "Track processor purging intervals must be long enough to form tracks",
                        ScenarioAnalyzerMessage::cERROR, sim, checkPurgeIntervalLongEnoughToEstablishTrackHelper);
}

Messages checkCommInternallyLinked(WsfSimulation& sim)
{
   return runSuiteCheck("Core", "Comm devices must have internal links",
                        ScenarioAnalyzerMessage::cERROR, sim, checkCommInternallyLinkedHelper);
}

Messages checkPlatformInCommandChainHasComm(WsfSimulation& sim)
{
   return runSuiteCheck("Core", "Platforms in command chains must have comm devices",
                        ScenarioAnalyzerMessage::cERROR, sim, checkPlatformInCommandChainHasCommHelper);
}

Messages checkLargeScenarioHasCommandChain(WsfSimulation& sim)
{
   return runSuiteCheck("Core", "Scenarios with many platforms should have a command chain",
                        ScenarioAnalyzerMessage::cWARNING, sim, checkLargeScenarioHasCommandChainHelper);
}

Messages checkScriptProcessorHasUpdateInterval(WsfSimulation& sim)
{
   return runSuiteCheck("Core", "Script processors must have update interval defined",
                        ScenarioAnalyzerMessage::cERROR, sim, checkScriptProcessorHasUpdateIntervalHelper);
}

Messages checkDeclaredCommandChainHasStructure(WsfSimulation& sim)
{
   return runSuiteCheck("Core", "Declared command chains should have structure",
                        ScenarioAnalyzerMessage::cWARNING, sim, checkDeclaredCommandChainHasStructureHelper);
}

Messages checkCommanderInDeclaredCommandChain(WsfSimulation& sim)
{
   return runSuiteCheck("Core", "Declared commander should be in that command chain",
                        ScenarioAnalyzerMessage::cWARNING, sim, checkCommanderInDeclaredCommandChainHelper);
}

Messages checkPlatformHasRequiredSignatures(WsfSimulation& sim)
{
   return runSuiteCheck("Core", "Platforms should have signatures required by sensors in scenario",
                        ScenarioAnalyzerMessage::cWARNING, sim, checkPlatformHasRequiredSignaturesHelper);
}

// Ensures that every signature a platform has is used by/detectable by at least one sensor on an enemy platform.
// Requires wsf_mil, which should be included in the list of dependencies for the "core" suite.
Messages checkSignaturesDetectableByEnemySensor(WsfSimulation& sim)
{
   return runSuiteCheck("Core", "All signatures should be detectable by an enemy sensor",
                        ScenarioAnalyzerMessage::cWARNING, sim, checkSignaturesDetectableByEnemySensorHelper);
}

// For now, checking that user has modified location from default of (0,0,0) in LLA.
Messages checkPlatformHasMeaningfulLocation(WsfSimulation& sim)
{
   return runSuiteCheck("Core", "Platforms should have meaningful locations defined",
                        ScenarioAnalyzerMessage::cWARNING, sim, checkPlatformHasMeaningfulLocationHelper);
}

Messages checkUserConfiguredSpeedsWithinMoverCapabilities(WsfSimulation& sim)
{
   return runSuiteCheck("Core", "User configured speeds should be within mover capabilities",
                        ScenarioAnalyzerMessage::cWARNING, sim, checkUserConfiguredSpeedsWithinMoverCapabilitiesHelper);
}

// ****************************************************************************
//                   Scenario Analyzer Script Methods
// ****************************************************************************
// this declaration is necessary because this name is referenced in the
// UT_DEFINE_SCRIPT_METHOD macros for ScenarioAnalyzerScriptClass
class ScenarioAnalyzer : public UtScriptAccessible
{
   public:
      const char* GetScriptClassName() const override { return "ScenarioAnalyzer"; }
};

class ScenarioAnalyzerScriptClass : public UtScriptClass
{
   public:
      UT_DECLARE_SCRIPT_METHOD(NotifyOfPlatformsNotPresentInSimulation);

      UT_DECLARE_SCRIPT_METHOD(CheckCommInternallyLinked);
      UT_DECLARE_SCRIPT_METHOD(CheckPlatformInCommandChainHasComm);
      UT_DECLARE_SCRIPT_METHOD(CheckDeclaredCommandChainHasStructure);
      UT_DECLARE_SCRIPT_METHOD(CheckCommanderInDeclaredCommandChain);
      UT_DECLARE_SCRIPT_METHOD(CheckLargeScenarioHasCommandChain);
      UT_DECLARE_SCRIPT_METHOD(CheckPlatformHasMeaningfulLocation);
      UT_DECLARE_SCRIPT_METHOD(CheckSensorInternallyLinked);
      UT_DECLARE_SCRIPT_METHOD(CheckSensorOn);
      UT_DECLARE_SCRIPT_METHOD(CheckSensorInternallyLinkedToTrackProcessor);
      UT_DECLARE_SCRIPT_METHOD(CheckSignaturesDetectableByEnemySensor);
      UT_DECLARE_SCRIPT_METHOD(CheckPlatformHasRequiredSignatures);
      UT_DECLARE_SCRIPT_METHOD(CheckPurgeIntervalLongEnoughToMaintainTrack);
      UT_DECLARE_SCRIPT_METHOD(CheckPurgeIntervalLongEnoughToEstablishTrack);
      UT_DECLARE_SCRIPT_METHOD(CheckScriptProcessorHasUpdateInterval);
      UT_DECLARE_SCRIPT_METHOD(CheckTrackProcessorHasPurgeInterval);
      UT_DECLARE_SCRIPT_METHOD(CheckTrackProcessorsDontReportFusedTracksToEachOther);
      UT_DECLARE_SCRIPT_METHOD(CheckUserConfiguredSpeedsWithinMoverCapabilities);
      UT_DECLARE_SCRIPT_METHOD(CheckWeaponsNonzeroQuantity);

      explicit ScenarioAnalyzerScriptClass(UtScriptTypes* types)
         : UtScriptClass("ScenarioAnalyzer", types)
      {
         SetClassName("ScenarioAnalyzer");

         AddStaticMethod(ut::make_unique<NotifyOfPlatformsNotPresentInSimulation>());

         AddStaticMethod(ut::make_unique<CheckWeaponsNonzeroQuantity>());
         AddStaticMethod(ut::make_unique<CheckSensorOn>());
         AddStaticMethod(ut::make_unique<CheckSensorInternallyLinked>());
         AddStaticMethod(ut::make_unique<CheckSensorInternallyLinkedToTrackProcessor>());
         AddStaticMethod(ut::make_unique<CheckTrackProcessorHasPurgeInterval>());
         AddStaticMethod(ut::make_unique<CheckCommInternallyLinked>());
         AddStaticMethod(ut::make_unique<CheckPlatformInCommandChainHasComm>());
         AddStaticMethod(ut::make_unique<CheckLargeScenarioHasCommandChain>());
         AddStaticMethod(ut::make_unique<CheckScriptProcessorHasUpdateInterval>());
         AddStaticMethod(ut::make_unique<CheckDeclaredCommandChainHasStructure>());
         AddStaticMethod(ut::make_unique<CheckCommanderInDeclaredCommandChain>());
         AddStaticMethod(ut::make_unique<CheckPlatformHasRequiredSignatures>());
         AddStaticMethod(ut::make_unique<CheckPurgeIntervalLongEnoughToMaintainTrack>());
         AddStaticMethod(ut::make_unique<CheckPurgeIntervalLongEnoughToEstablishTrack>());
         AddStaticMethod(ut::make_unique<CheckSignaturesDetectableByEnemySensor>());
         AddStaticMethod(ut::make_unique<CheckPlatformHasMeaningfulLocation>());
         AddStaticMethod(ut::make_unique<CheckTrackProcessorsDontReportFusedTracksToEachOther>());
         AddStaticMethod(ut::make_unique<CheckUserConfiguredSpeedsWithinMoverCapabilities>());
      }

      ~ScenarioAnalyzerScriptClass() override = default;
};

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerScriptClass, ScenarioAnalyzer, NotifyOfPlatformsNotPresentInSimulation, 0, "void", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);
   notifyOfPlatformsNotPresentInSimulation(*sim);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerScriptClass, ScenarioAnalyzer, CheckWeaponsNonzeroQuantity, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);

   Messages messages = checkWeaponsNonzeroQuantity(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerScriptClass, ScenarioAnalyzer, CheckSensorOn, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);

   Messages messages = checkSensorOn(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerScriptClass, ScenarioAnalyzer, CheckSensorInternallyLinked, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);

   Messages messages = checkSensorInternallyLinked(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerScriptClass, ScenarioAnalyzer, CheckSensorInternallyLinkedToTrackProcessor, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);

   Messages messages = checkSensorInternallyLinkedToTrackProcessor(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerScriptClass, ScenarioAnalyzer, CheckTrackProcessorHasPurgeInterval, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);

   Messages messages = checkTrackProcessorHasPurgeInterval(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerScriptClass, ScenarioAnalyzer, CheckCommInternallyLinked, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);

   Messages messages = checkCommInternallyLinked(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerScriptClass, ScenarioAnalyzer, CheckPlatformInCommandChainHasComm, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);

   Messages messages = checkPlatformInCommandChainHasComm(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerScriptClass, ScenarioAnalyzer, CheckLargeScenarioHasCommandChain, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);

   Messages messages = checkLargeScenarioHasCommandChain(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerScriptClass, ScenarioAnalyzer, CheckScriptProcessorHasUpdateInterval, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);

   Messages messages = checkScriptProcessorHasUpdateInterval(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerScriptClass, ScenarioAnalyzer, CheckDeclaredCommandChainHasStructure, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);

   Messages messages = checkDeclaredCommandChainHasStructure(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerScriptClass, ScenarioAnalyzer, CheckCommanderInDeclaredCommandChain, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);

   Messages messages = checkCommanderInDeclaredCommandChain(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerScriptClass, ScenarioAnalyzer, CheckPlatformHasRequiredSignatures, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);

   Messages messages = checkPlatformHasRequiredSignatures(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerScriptClass, ScenarioAnalyzer, CheckTrackProcessorsDontReportFusedTracksToEachOther, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);

   Messages messages = checkTrackProcessorsDontReportFusedTracksToEachOther(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerScriptClass, ScenarioAnalyzer, CheckPurgeIntervalLongEnoughToMaintainTrack, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);

   Messages messages = checkPurgeIntervalLongEnoughToMaintainTrack(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerScriptClass, ScenarioAnalyzer, CheckPurgeIntervalLongEnoughToEstablishTrack, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);

   Messages messages = checkPurgeIntervalLongEnoughToEstablishTrack(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerScriptClass, ScenarioAnalyzer, CheckSignaturesDetectableByEnemySensor, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);

   Messages messages = checkSignaturesDetectableByEnemySensor(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerScriptClass, ScenarioAnalyzer, CheckPlatformHasMeaningfulLocation, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);

   Messages messages = checkPlatformHasMeaningfulLocation(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

UT_DEFINE_SCRIPT_METHOD(ScenarioAnalyzerScriptClass, ScenarioAnalyzer, CheckUserConfiguredSpeedsWithinMoverCapabilities, 0, "Array<ScenarioAnalyzerMessage>", "")
{
   WsfSimulation* sim = WsfScriptContext::GetSIMULATION(aContext);

   Messages messages = checkUserConfiguredSpeedsWithinMoverCapabilities(*sim);
   ConvertMessagesToScript(messages, aReturnVal, aContext, aReturnClassPtr);
}

// Plugin interface
void ScenarioAnalyzerRegisterScriptTypes(WsfApplication& aApplication)
{
   UtScriptTypes* scriptTypes = aApplication.GetScriptTypes();
   scriptTypes->Register(ut::make_unique<ScenarioAnalyzerScriptClass>(scriptTypes));
   scriptTypes->Register(ut::make_unique<ScriptScenarioAnalyzerMessageClass>(scriptTypes));
   RegisterUtilsScriptTypes(aApplication);
}
