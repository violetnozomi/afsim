// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WsfSensorsManagerFovImpl.hpp"

#include "iadsLib/assetManagerInterface.hpp"
#include "iadsLib/assignmentContainers.hpp"
#include "iadsLib/assignmentMessage.hpp"
#include "iadsLib/cueMessage.hpp"
#include "iadsLib/sensorRecordInterface.hpp"
#include "iadsLib/trackRecord.hpp"

#include "WsfBMMOELogger.hpp"
#include "WsfIADSC2ScenarioExtension.hpp"

static void turnOffUnassignedSensor(il::moeLoggingInterface& aMOELogger,
                                    std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger,
                                    double simTime,
                                    il::sensorRecord& sensor,
                                    il::ResponsibleAssignments& assignments)
{
   if (sensor.IsTTR())
   {
      // the on/off state of TTRs is handled by WsfSensorsManagerFov
      return;
   }

   // check the active cues on the sensor directly in case other processors are
   // interacting with it - we do not want to turn the sensor off if someone
   // else is using it
   if (sensor.GetTruthActiveCues(aGlobalLogger) == 0 && sensor.IsTurnedOn())
   {
      il::tSensorID sensorId = sensor.GetSensorID();
      bool hasAssignments;
      if (sensor.IsTAR())
      {
         hasAssignments = false;
         auto processAssignments =
            [&hasAssignments, sensorId]
            (il::moeLoggingInterface& aMOELogger, const il::tTrackID& masterTrackId, il::ResponsibleRadarAssignment& assignment)
            -> void
         {
            if (assignment.HasTARResponsibility() &&
            assignment.GetResponsibleTAR()->GetSensorID() == sensorId)
            {
               hasAssignments = true;
            }
         };
         assignments.ForeachResponsibleAssignment(aMOELogger, processAssignments);
      }
      else
      {
         return;
      }

      if (!hasAssignments)
      {
         sensor.TurnOff(simTime);
      }
   }
}

static bool assignCue(il::moeLoggingInterface& aMOELogger,
                      const il::AssetManagerInterface& assetManager,
                      double simTime,
                      const il::trackRecord& masterTrack,
                      const il::assignmentMessage& assignment,
                      il::sensorRecord& sensor)
{
   il::assetRecord& asset = *(assetManager.getThisAsset());
   if (sensor.IsTTR())
   {
      // the on/off state of TTRs is handled by WsfSensorsManagerFov

      if (sensor.Cue(simTime, asset.getID(), masterTrack.getID()))
      {
         aMOELogger.SensorCue(
            simTime, &asset, &masterTrack, &assignment, &sensor);
         return true;
      }
   }
   else
   {
      if (!sensor.IsTurnedOn())
      {
         sensor.TurnOn(simTime);
      }
      if (sensor.IsTurnedOn())
      {
         aMOELogger.SensorCue(
            simTime, &asset, &masterTrack, &assignment, &sensor);
         return true;
      }
   }

   return false;
}

static bool dropCue(il::moeLoggingInterface& aMOELogger,
                    const il::AssetManagerInterface& assetManager,
                    bool turnOffUnassignedTar,
                    bool turnOffUnassignedTtr,
                    double simTime,
                    const il::trackRecord& masterTrack,
                    const il::assignmentMessage& assignment,
                    il::sensorRecord& sensor,
                    il::ResponsibleAssignments& responsibleAssignments)
{
   if (sensor.IsTTR())
   {
      // the on/off state of TTRs is handled by WsfSensorsManagerFov

      if (sensor.DropCue(simTime, masterTrack.getID()))
      {
         il::assetRecord& asset = *(assetManager.getThisAsset());
         aMOELogger.SensorDropCue(
            simTime, &asset, &masterTrack, &assignment, &sensor);
      }
      return true;
   }

   if (sensor.IsTAR() && turnOffUnassignedTar)
   {
      turnOffUnassignedSensor(aMOELogger, assetManager.getGlobalLogger(), simTime, sensor, responsibleAssignments);
   }

   aMOELogger.SensorDropCue(
      simTime, assetManager.getThisAsset().get(), &masterTrack, &assignment, &sensor);

   return true;
}

WsfSensorsManagerFovImpl::WsfSensorsManagerFovImpl()
   : SensorsManagerInterface()
{ }

WsfSensorsManagerFovImpl::WsfSensorsManagerFovImpl(const WsfSensorsManagerFovImpl& src)
   : SensorsManagerInterface(src)
{ }

WsfSensorsManagerFovImpl* WsfSensorsManagerFovImpl::Clone()
{
   return new WsfSensorsManagerFovImpl(*this);
}

WsfSensorsManagerFovImpl::~WsfSensorsManagerFovImpl()
{ }

// virtual
bool WsfSensorsManagerFovImpl::SendSensorCueMessage(il::moeLoggingInterface& aMOELogger,
                                                    const double simTime,
                                                    const std::shared_ptr<const il::trackRecord>& masterTrack,
                                                    const std::shared_ptr<const il::assignmentMessage>& assignment,
                                                    const std::shared_ptr<il::sensorRecord>& sensor,
                                                    const il::cueMessage::CueReason& reason)
{
   // don't actually send out the cue, just bookkeep it internally
   // and treat it as if we are performing the local Asset Manager
   // sensor manager cue handling code
   il::AssetManagerInterface& assetManager = *(GetAssetManager());
   switch (reason)
   {
      case il::cueMessage::NEW_CUE:
         return assignCue(aMOELogger, assetManager, simTime, *masterTrack, *assignment, *sensor);
      case il::cueMessage::CANCEL:
         return dropCue(
                   aMOELogger,
                   assetManager,
                   GetTurnOffTARNoTracks(),
                   GetTurnOffTTRNoTracks(),
                   simTime,
                   *masterTrack,
                   *assignment,
                   *sensor,
                   GetResponsibleAssignments());
      default:
         HCL_FATAL_LOGGER(assetManager.getGlobalLogger()) <<
                            "WsfSensorsManagerFovImpl::SendSensorCueMessage(): unknown cue type, not cancel or new";
         return false;
   }
}

// virtual
bool WsfSensorsManagerFovImpl::ProcessCurrentCueAssignment(il::moeLoggingInterface& aMOELogger,
                                                           const double simTime,
                                                           const std::shared_ptr<const il::trackRecord>& masterTrack,
                                                           const std::shared_ptr<const il::assignmentMessage>& assignment,
                                                           const std::shared_ptr<il::sensorRecord>& sensor)
{
   return assignCue(aMOELogger, *(GetAssetManager()), simTime, *masterTrack, *assignment, *sensor);
}

// virtual
bool WsfSensorsManagerFovImpl::ProcessDropCue(il::moeLoggingInterface& aMOELogger,
                                              const double simTime,
                                              const std::shared_ptr<const il::trackRecord>& masterTrack,
                                              const std::shared_ptr<const il::assignmentMessage>& assignment,
                                              const std::shared_ptr<il::sensorRecord>& sensor)
{
   return dropCue(aMOELogger,
                  *(GetAssetManager()),
                  GetTurnOffTARNoTracks(),
                  GetTurnOffTTRNoTracks(),
                  simTime,
                  *masterTrack,
                  *assignment,
                  *sensor,
                  GetResponsibleAssignments());
}

// virtual
void WsfSensorsManagerFovImpl::TurnOffSensorIfNoTracks(il::moeLoggingInterface& aMOELogger,
                                                       const double simTime,
                                                       const std::shared_ptr<il::sensorRecord>& sensor)
{
   turnOffUnassignedSensor(aMOELogger, GetAssetManager()->getGlobalLogger(), simTime, *sensor, GetResponsibleAssignments());
}
