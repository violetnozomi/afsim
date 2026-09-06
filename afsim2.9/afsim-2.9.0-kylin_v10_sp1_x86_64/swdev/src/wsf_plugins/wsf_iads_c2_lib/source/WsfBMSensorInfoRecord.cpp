// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#include <cassert>
#include <cmath>

#include "logger.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfBMAssetRecord.hpp"
#include "WsfBMSensorInfoRecord.hpp"
#include "WsfBMUtils.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "UtMath.hpp"
#include "iadsLib/util.hpp"

#include "WsfIADSC2ScenarioExtension.hpp"

WsfBMSensorInfoRecord::WsfBMSensorInfoRecord()
   : m_attached_plat(WSF_INVALID_PLAT_ID),
     m_sim_ptr(nullptr),
     restingAzimuth(0.0),
     coarseSlewRate(WsfArticulatedPart::cINFINITE_SLEW_RATE),
     fineSlewRate(WsfArticulatedPart::cINFINITE_SLEW_RATE)
{ }

WsfBMSensorInfoRecord::WsfBMSensorInfoRecord(const WsfBMSensorInfoRecord& aSrc)
   : sensorRecord(aSrc),
     m_attached_plat(aSrc.m_attached_plat),
     m_tracking_mode(aSrc.m_tracking_mode),
     restingAzimuth(aSrc.restingAzimuth),
     coarseSlewRate(aSrc.coarseSlewRate),
     fineSlewRate(aSrc.fineSlewRate)
{ }

WsfBMSensorInfoRecord::~WsfBMSensorInfoRecord()
{ }

// virtual
double WsfBMSensorInfoRecord::GetAz() const
{
   WsfSensor* sensor = GetSensorRef();
   if (sensor != nullptr)
   {
      double az, el;
      sensor->GetCurrentCuedOrientation(az, el);
      assert(az >= -UtMath::cPI && az <= UtMath::cPI);
      return az;
   }
   else
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(m_sim_ptr->GetScenario())) <<
                         "WsfBMSensorInfoRecord::GetAz(): Could not resolve AFSIM sensor: " << GetSsysID();
      return 0.0;
   }
}

// virtual
double WsfBMSensorInfoRecord::GetAzExtent() const
{
   std::weak_ptr<il::logger::GlobalLogger> globalLogger;
   WsfSensor* sensor = GetSensorRef();
   if (sensor != nullptr)
   {
      globalLogger = IADSC2ScenarioExtension::GetGlobalLogger(sensor->GetScenario());
      // If there are transmitters, assume an active sensor (e.g. a radar).
      // if there are receivers, assume a passive sensor; otherwise, we
      // cannot determine the azimuth extent.

      if (sensor->GetEM_XmtrCount() > 0)
      {
         if (sensor->GetEM_XmtrCount() > 1)
         {
            HCL_ERROR_LOGGER(globalLogger) <<
                               "WsfBMSensorInfoRecord::GetAzExtent(): Cannot determine azimuth field of view; sensor has multiple antennae: " <<
                               sensor->GetName();
         }

         WsfEM_Xmtr& tx = sensor->GetEM_Xmtr(0);
         double azExtent = tx.GetAzimuthBeamwidth(0.0, 0.0);
         assert(azExtent >= 0.0 && azExtent <= 2.0 * UtMath::cPI);
         return azExtent;
      }
      else if (sensor->GetEM_RcvrCount() > 0)
      {
         if (sensor->GetEM_RcvrCount() > 1)
         {
            HCL_ERROR_LOGGER(globalLogger) <<
                               "WsfBMSensorInfoRecord::GetAzExtent(): Cannot determine azimuth field of view; sensor has multiple antennae: " <<
                               GetSsysID();
         }

         WsfEM_Rcvr& rx = sensor->GetEM_Rcvr(0);
         double azExtent = rx.GetAzimuthBeamwidth(0.0, 0.0);
         assert(azExtent >= 0.0 && azExtent <= 2.0 * UtMath::cPI);
         return azExtent;
      }
      else
      {
         HCL_ERROR_LOGGER(globalLogger) <<
                            "WsfBMSensorInfoRecord::GetAzExtent(): Cannot determine azimuth field of view; sensor has no antennae: " <<
                            GetSsysID();
      }
   }
   else
   {
      globalLogger = IADSC2ScenarioExtension::GetGlobalLogger(m_sim_ptr->GetScenario());
      HCL_ERROR_LOGGER(globalLogger) <<
                         "WsfBMSensorInfoRecord::GetAzExtent(): Could not resolve AFSIM sensor: " << GetSsysID();
   }

   HCL_ERROR_LOGGER(globalLogger) << "Using default azimuth field of view of 0 degrees";
   return 0.0;
}

// virtual
void WsfBMSensorInfoRecord::SetAzSlewRate(double slewRate)
{
   assert(slewRate >= 0.0);

   WsfSensor* sensor = GetSensorRef();
   if (sensor != nullptr)
   {
      sensor->SetSlewRates(slewRate, sensor->GetElSlewRate());
   }
   else
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(m_sim_ptr->GetScenario())) <<
                         "WsfBMSensorInfoRecord::SetAzSlewRate(): Could not resolve AFSIM sensor: " << GetSsysID();
   }
}

// virtual
void WsfBMSensorInfoRecord::CueToAzimuth(double az)
{
   assert(az >= -UtMath::cPI && az <= UtMath::cPI);

   WsfSensor* sensor = GetSensorRef();
   if (sensor != nullptr)
   {
      double junk, el;
      // retrieve the current cued elevation, or just the
      // current elevation if nothing is cued
      if (!sensor->GetCuedOrientation(junk, el))
      {
         sensor->GetCurrentCuedOrientation(junk, el);
      }
      sensor->SetCuedOrientation(az, el);
   }
   else
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(m_sim_ptr->GetScenario())) <<
                         "WsfBMSensorInfoRecord::CueToAzimuth(): Could not resolve AFSIM sensor: " << GetSsysID();
   }
}

// virtual
vm::Vector3<double> WsfBMSensorInfoRecord::WcsToEcs(const vm::Vector3<double>& wcs) const
{
   WsfSensor* sensor = GetSensorRef();
   if (sensor != nullptr)
   {
      double wcsArray[3];
      wcs.get(wcsArray[0], wcsArray[1], wcsArray[2]);
      double ecsArray[3];
      sensor->GetPlatform()->ConvertWCSToECS(wcsArray, ecsArray);
      return vm::Vector3<double>(ecsArray[0], ecsArray[1], ecsArray[2]);
   }
   else
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(m_sim_ptr->GetScenario())) <<
                         "WsfBMSensorInfoRecord::WcsToEcs(): Could not resolve AFSIM sensor: " << GetSsysID();
   }

   return wcs;
}

// virtual
vm::Vector3<double> WsfBMSensorInfoRecord::WcsToPcs(const vm::Vector3<double>& wcs) const
{
   WsfSensor* sensor = GetSensorRef();
   if (sensor != nullptr)
   {
      // transform from WCS to ECS
      double wcsArray[3];
      wcs.get(wcsArray[0], wcsArray[1], wcsArray[2]);
      double ecsArray[3];
      sensor->GetPlatform()->ConvertWCSToECS(wcsArray, ecsArray);

      // compute the translation and rotation for ECS to PCS
      double yaw, pitch, roll;
      sensor->GetOrientation(yaw, pitch, roll);
      double ecsToPcsRotation[3][3];
      UtEntity::ComputeRotationalTransform(yaw, pitch, roll, ecsToPcsRotation);
      double sensorOffset[3];
      sensor->GetLocation(sensorOffset);

      // transform from ECS to PCS
      double tmp[3];
      UtVec3d::Subtract(tmp, ecsArray, sensorOffset);
      double pcsArray[3];
      UtMat3d::Transform(pcsArray, ecsToPcsRotation, tmp);

      return vm::Vector3<double>(pcsArray[0], pcsArray[1], pcsArray[2]);
   }
   else
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(m_sim_ptr->GetScenario())) <<
                         "WsfBMSensorInfoRecord::WcsToPcs(): Could not resolve AFSIM sensor: " << GetSsysID();
   }

   return wcs;
}

void WsfBMSensorInfoRecord::Set(/* const */ WsfSimulation* sim_ptr, WsfBMAssetRecord* asset, tSensorSysID sensor_sys_id, il::AssetTypeEnum sensor_type)
{
   Set(sim_ptr, asset->GetRawRecord(), sensor_sys_id, sensor_type);
}

void WsfBMSensorInfoRecord::Set(/* const */ WsfSimulation* sim_ptr, const std::shared_ptr<il::assetRecord>& asset, tSensorSysID sensor_sys_id, il::AssetTypeEnum sensor_type)
{
   m_sim_ptr = sim_ptr;

   m_attached_plat = ut::safe_cast<tPlatformRef>(asset->getID().getID());
   m_sensor_id = WsfBMUtils::GetSensorID(m_sim_ptr, m_attached_plat, sensor_sys_id);

   m_parent_asset = asset;

   auto type_id = sensor_type;
   auto type_str = il::AssetTypeEnumToString(sensor_type);
   auto subtype_id = WsfStringInt(GetSensorRef()->GetNameId());
   auto subtype_str = GetSensorRef()->GetName();

   m_sensor_type.set(type_id, type_str, subtype_id, subtype_str);
}

tSensorSysID WsfBMSensorInfoRecord::GetSsysID() const
{
   return m_sensor_id.getID();
}

il::unitTypeRecord WsfBMSensorInfoRecord::GetTypeRecord() const
{
   return m_sensor_type;
}

std::string WsfBMSensorInfoRecord::GetName() const
{
   return GetSensorRef()->GetName();
}

int WsfBMSensorInfoRecord::GetTruthActiveCues(std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger) const
{
   int active_request_count = 0;

   auto sensor = GetSensorRef();
   if (sensor)
   {
      active_request_count = sensor->GetActiveRequestCount(GetTrackingMode());
   }
   else
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(m_sim_ptr->GetScenario())) << "WsfBMSensorInfoRecord::GetTruthActiveCues(): Could not resolve AFSIM sensor: " << GetSsysID();
   }

   return active_request_count;
}

int WsfBMSensorInfoRecord::GetMaxActiveCues() const
{
   int max_request_count = 0;

   auto sensor = GetSensorRef();
   if (sensor)
   {
      max_request_count = sensor->GetMaximumRequestCount(GetTrackingMode());
   }
   else
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(m_sim_ptr->GetScenario())) << "WsfBMSensorInfoRecord::GetMaxActiveCues(): Could not resolve AFSIM sensor: " << GetSsysID();
   }

   return max_request_count;
}

bool WsfBMSensorInfoRecord::IsTurnedOn() const
{
   bool is_on = false;

   auto sensor = GetSensorRef();
   if (sensor)
   {
      is_on = sensor->IsTurnedOn();
   }
   else
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(m_sim_ptr->GetScenario())) << "WsfBMSensorInfoRecord::IsTurnedOn(): Could not resolve AFSIM sensor: " << GetSsysID();
   }

   return is_on;
}

bool WsfBMSensorInfoRecord::TurnOff(const double aSimTime)
{
   bool success = false;

   auto platform = GetPlatformRef();
   auto sensor = GetSensorRef();

   if (platform && sensor)
   {
      success = platform->GetSimulation()->TurnPartOff(aSimTime, sensor);

      if (success)
      {
         HCL_DEBUG_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(platform->GetScenario())) << aSimTime << ": Requested to turn off sensor: " << platform->GetName() << ":" << sensor->GetName();
      }
      else
      {
         HCL_WARN_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(platform->GetScenario())) << aSimTime << ": Attempted request to turn off sensor: " << platform->GetName() << ":" << sensor->GetName() << " but failed";
      }
   }
   else
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(m_sim_ptr->GetScenario())) << "WsfBMSensorInfoRecord::TurnOff(): Could not resolve AFSIM sensor: " << GetSsysID();
   }

   return success;
}

bool WsfBMSensorInfoRecord::TurnOn(const double aSimTime)
{
   bool success = false;

   auto platform = GetPlatformRef();
   auto sensor = GetSensorRef();

   if (platform && sensor)
   {
      success = platform->GetSimulation()->TurnPartOn(aSimTime, sensor);

      if (success)
      {
         HCL_DEBUG_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(platform->GetScenario())) << aSimTime << ": Requested to turn on sensor: " << platform->GetName() << ":" << sensor->GetName();
      }
      else
      {
         HCL_WARN_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(platform->GetScenario())) << aSimTime << ": Attempted request to turn on sensor: " << platform->GetName() << ":" << sensor->GetName() << " but failed";
      }
   }
   else
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(m_sim_ptr->GetScenario())) << "WsfBMSensorInfoRecord::TurnOn(): Could not resolve AFSIM sensor: " << GetSsysID();
   }

   return success;
}

bool WsfBMSensorInfoRecord::Cue(const double aSimTime, const il::idRecord& invoking_asset, const il::tTrackID& master_track_id)
{
   bool was_success = false;

   auto sensor = GetSensorRef();

   WsfPlatform* invoking_platform = nullptr;
   if (sensor)
   {
      invoking_platform = WsfBMUtils::GetPlatformFromUnitIDRecord(sensor->GetSimulation(), invoking_asset);
   }

   // locate master track
   if (invoking_platform)
   {
      auto local_track = invoking_platform->GetTrackManager().FindTrack(WsfBMUtils::ConvertTrackId(master_track_id));
      if (local_track)
      {
         was_success = sensor->StartTracking(aSimTime, *local_track, GetTrackingMode());
      }
   }

   return was_success;
}

bool WsfBMSensorInfoRecord::CueTrackUpdated(const double aSimTime, const il::idRecord& invoking_asset, const il::tTrackID& master_track_id)
{
   // note: the sensor doesn't have a simple way to update the track directly for an existing cue. If you call start tracking against an existing cue, it will effectivley
   // just update the request information with the new track information.
   bool was_success = false;

   auto sensor = GetSensorRef();

   WsfPlatform* invoking_platform = nullptr;
   if (sensor)
   {
      invoking_platform = WsfBMUtils::GetPlatformFromUnitIDRecord(sensor->GetSimulation(), invoking_asset);
   }

   // locate master track
   if (invoking_platform)
   {
      auto local_track = invoking_platform->GetTrackManager().FindTrack(WsfBMUtils::ConvertTrackId(master_track_id));
      if (local_track && sensor->HaveRequestFor(local_track->GetTrackId()))
      {
         was_success = sensor->StartTracking(aSimTime, *local_track, GetTrackingMode());
      }
   }

   return was_success;
}

bool WsfBMSensorInfoRecord::DropCue(const double aSimTime, const il::tTrackID& master_track_id)
{
   bool was_success = false;

   auto sensor = GetSensorRef();
   if (sensor)
   {
      was_success = sensor->StopTracking(aSimTime, WsfBMUtils::ConvertTrackId(master_track_id));
   }
   else
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(m_sim_ptr->GetScenario())) << "WsfBMSensorInfoRecord::DropCue(): Could not resolve AFSIM sensor: " << GetSsysID();
   }

   return was_success;
}

void WsfBMSensorInfoRecord::SetTrackingMode(const std::string& opt_tracking_mode)
{
   m_tracking_mode = opt_tracking_mode;
}

WsfPlatform* WsfBMSensorInfoRecord::GetPlatformRef() const
{
   return m_sim_ptr->GetPlatformByIndex(m_attached_plat);
}

WsfSensor* WsfBMSensorInfoRecord::GetSensorRef() const
{
   auto platform = GetPlatformRef();
   WsfSensor* sensor = nullptr;

   if (platform)
   {
      sensor = platform->GetComponentEntry<WsfSensor>(m_sensor_id.getID());
   }

   return sensor;
}

WsfStringId WsfBMSensorInfoRecord::GetTrackingMode() const
{
   return m_tracking_mode.size() ? WsfStringId(m_tracking_mode) : 0;
}

///////////////
// EW Sensor //
///////////////
WsfBMEWSensorInfoRecord* WsfBMEWSensorInfoRecord::Clone() const
{
   return new WsfBMEWSensorInfoRecord(*this);
}

il::SensorEnum WsfBMEWSensorInfoRecord::GetSensorType() const
{
   return il::SENSOR_EW;
}

bool WsfBMEWSensorInfoRecord::IsEW() const
{
   return true;
}

////////////////
// TAR Sensor //
////////////////
WsfBMTARSensorInfoRecord* WsfBMTARSensorInfoRecord::Clone() const
{
   return new WsfBMTARSensorInfoRecord(*this);
}

il::SensorEnum WsfBMTARSensorInfoRecord::GetSensorType() const
{
   return il::SENSOR_TAR;
}

bool WsfBMTARSensorInfoRecord::IsTAR() const
{
   return true;
}

////////////////
// TTR Sensor //
////////////////
WsfBMTTRSensorInfoRecord* WsfBMTTRSensorInfoRecord::Clone() const
{
   return new WsfBMTTRSensorInfoRecord(*this);
}

il::SensorEnum WsfBMTTRSensorInfoRecord::GetSensorType() const
{
   return il::SENSOR_TTR;
}

bool WsfBMTTRSensorInfoRecord::IsTTR() const
{
   return true;
}

////////////////
// RWR Sensor //
////////////////
WsfBMRWRSensorInfoRecord* WsfBMRWRSensorInfoRecord::Clone() const
{
   return new WsfBMRWRSensorInfoRecord(*this);
}

il::SensorEnum WsfBMRWRSensorInfoRecord::GetSensorType() const
{
   return il::SENSOR_RWR;
}

bool WsfBMRWRSensorInfoRecord::IsRWR() const
{
   return true;
}

WsfStringId WsfBMRWRSensorInfoRecord::GetNameId() const
{
   WsfSensor* sensor = GetSensorRef();
   if (sensor)
   {
      return sensor->GetNameId();
   }
   return WsfStringId();
}

int WsfBMRWRSensorInfoRecord::GetRWRActiveTrackCount() const
{
   int trackCount = 0;
   WsfSensor* sensor = GetSensorRef();
   if (sensor)
   {
      trackCount = sensor->GetActiveTrackCount();
   }
   return trackCount;
}

void WsfBMRWRSensorInfoRecord::GetRWRActiveTrackList(WsfTrackList& trackList)
{
   WsfSensor* sensor = GetSensorRef();
   if (sensor)
   {
      sensor->GetActiveTrackList(trackList);
   }
   return;
}
