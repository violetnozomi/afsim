// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: sensorRecordInterface.hpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name:	Radiance Technologies
*   Address:			350 Wynn Drive
*   					   Huntsville, AL 35805
*
*   Abstract:
*
*******************************************************************************/

#ifndef IADSLIB_SENSOR_RECORD_INTERFACE
#define IADSLIB_SENSOR_RECORD_INTERFACE

#include <string>
#include <memory>

#include <iadsLib/idRecord.hpp>
#include <iadsLib/enum.hpp>
#include <iadsLib/unitTypeRecord.hpp>
#include <iadsLib/positionRecord.hpp>
#include <iadsLib/zoneRecordInterface.hpp>
#include <iadsLib/trackRecord.hpp>

namespace il
{
   class assetRecord;

   typedef idRecord tSensorID;

   class IADSLIB_EXPORT sensorRecord
   {
   public:
      sensorRecord ();
      sensorRecord (const std::shared_ptr<assetRecord> &parent_asset, const tSensorID &sensor_id, const unitTypeRecord &sensor_type);
      ~sensorRecord() = default;

      virtual idRecord GetParentAssetID () const;
      virtual unitTypeRecord GetType () const;
      virtual tSensorID GetSensorID () const;
      virtual double GetAssignmentDelay () const;
      virtual positionRecord GetPosition () const;
      virtual std::shared_ptr<assetRecord> GetParentAsset () const;

      // gets the current PCS (Part Coordinate System) azimuth of the sensor beam in radians;
      // in the range [0, 2 * pi]
      virtual double GetAz() const = 0;

      // gets the width of the sensor beam, in radians; in the range [0, 2 * pi]
      virtual double GetAzExtent() const = 0;

      // sets the slew rate of the sensor, in radians/sec
      virtual void SetAzSlewRate(double slewRate) = 0;

      // cues the sensor to an azimuth in PCS (Part Coordinate System), in radians
      virtual void CueToAzimuth(double azimuth) = 0;

      // Transforms a position from WCS coordinates to ECS
      // coordinates of the sensor's platform.
      virtual vm::Vector3<double> WcsToEcs(const vm::Vector3<double>& p) const = 0;

      // Transforms a position from WCS coordinates to PCS
      // coordinates of the sensor.
      virtual vm::Vector3<double> WcsToPcs(const vm::Vector3<double>& p) const = 0;

      // Gets the resting azimuth, in radians. This value is used by the FOV sensor
      // manager, to cue TTRs when they have no assigned targets.
      virtual double GetRestingAzimuth() = 0;

      // Gets the course and fine slew rates, in radians/second. The coarse slew
      // rate is used when the sensor needs to slew a large amount, and the fine
      // slew rate when it needs to slew a small amount.
      virtual double GetCoarseSlewRate() = 0;
      virtual double GetFineSlewRate() = 0;

      // based off status perceptions
      virtual void SetMaxActiveCues (const int max_cues);
      virtual int GetMaxActiveCues () const;
      virtual void SetEstimatedActiveCues (const int num_cues);
      virtual int GetEstimatedActiveCues() const;
      virtual bool HasEstimatedAvailableCues() const;
      virtual void IncrementAvailableCues(const int by_num);
      virtual void DecrementAvailableCues(const int by_num);

      // Optionally Abstract Interface - not useful unless overridden because the defult implementations
      // lead to a sensor of type 'Unknown'
      virtual bool IsEW () const;
      virtual bool IsTAR() const;
      virtual bool IsTTR() const;
      virtual bool IsRWR() const;
      virtual bool IsCueable () const;
      virtual int GetTruthActiveCues(std::weak_ptr<logger::GlobalLogger> aGlobalLogger) const;

      // Pure Abstract interface
      virtual SensorEnum GetSensorType () const = 0;
      virtual bool IsTurnedOn () const = 0;
      virtual bool TurnOff(const double aSimTime) = 0;
      virtual bool TurnOn(const double aSimTime) = 0;
      virtual bool Cue(const double aSimTime, const il::idRecord &invoking_asset, const il::tTrackID &master_track_id) = 0;
      virtual bool CueTrackUpdated(const double aSimTime, const il::idRecord &invoking_asset, const il::tTrackID &master_track_id) = 0;
      virtual bool DropCue(const double aSimTime, const il::tTrackID &master_track_id) = 0;

   protected:
      std::shared_ptr<assetRecord> m_parent_asset;
      tSensorID m_sensor_id;
      unitTypeRecord m_sensor_type;

      int m_estimated_active_cues;
      int m_max_cues;
   };
}

#endif
