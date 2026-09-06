// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#ifndef WSF_BM_SENSOR_INFO_RECORD_HPP
#define WSF_BM_SENSOR_INFO_RECORD_HPP

#include <WsfObject.hpp>
#include <WsfPlatform.hpp>
#include "WsfBMCommon.hpp"
#include <iadsLib/sensorRecordInterface.hpp>
#include <iadsLib/trackRecord.hpp>

class WsfBMAssetRecord;
class WsfSensor;

class WSF_IADS_C2_LIB_EXPORT WsfBMSensorInfoRecord : public WsfObject, public il::sensorRecord
{
   public:

      WsfBMSensorInfoRecord();
      WsfBMSensorInfoRecord(const WsfBMSensorInfoRecord& aSrc);
      virtual ~WsfBMSensorInfoRecord();

      //! The standard Clone() method (virtual copy constructor).
      virtual WsfBMSensorInfoRecord* Clone() const = 0;

      double GetAz() const override;
      double GetAzExtent() const override;
      void SetAzSlewRate(double slewRate) override;
      void CueToAzimuth(double azimuth) override;

      vm::Vector3<double> WcsToEcs(const vm::Vector3<double>& p) const override;
      vm::Vector3<double> WcsToPcs(const vm::Vector3<double>& p) const override;

      double GetRestingAzimuth() override { return restingAzimuth; }
      void SetRestingAzimuth(double azimuth) { restingAzimuth = azimuth; }

      double GetCoarseSlewRate() override { return coarseSlewRate; }
      void SetCoarseSlewRate(double slewRate) { coarseSlewRate = slewRate; }

      double GetFineSlewRate() override { return fineSlewRate; }
      void SetFineSlewRate(double slewRate) { fineSlewRate = slewRate; }

      void Set(/* const */ WsfSimulation* sim_ptr, WsfBMAssetRecord* asset, tSensorSysID sensor_sys_id, il::AssetTypeEnum weapon_type);
      void Set(/* const */ WsfSimulation* sim_ptr, const std::shared_ptr<il::assetRecord>& asset, tSensorSysID sensor_sys_id, il::AssetTypeEnum sensor_type);

      tSensorSysID GetSsysID() const;
      il::unitTypeRecord GetTypeRecord() const;
      virtual std::string GetName() const;

      // for AFSIM, we never send cues out so the sensors don't send sensor status in their messages relating to cues, override the max and active cues
      // so our sensor perception can work correctly with modes
      int GetTruthActiveCues(std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger) const override;
      int GetMaxActiveCues() const override;

      bool IsTurnedOn() const override;
      bool TurnOff(const double aSimTime) override;
      bool TurnOn(const double aSimTime) override;
      bool Cue(const double aSimTime, const il::idRecord& invoking_asset, const il::tTrackID& master_track_id) override;
      bool CueTrackUpdated(const double aSimTime, const il::idRecord& invoking_asset, const il::tTrackID& master_track_id) override;
      bool DropCue(const double aSimTime, const il::tTrackID& master_track_id) override;

      void
      SetTrackingMode(const std::string& opt_tracking_mode);

   protected:
      WsfSensor* GetSensorRef() const;

   private:
      WsfPlatform* GetPlatformRef() const;

      tPlatformRef m_attached_plat;
      WsfSimulation* m_sim_ptr; // we don't own this

      std::string m_tracking_mode;

      WsfStringId GetTrackingMode() const;

      double restingAzimuth;
      double coarseSlewRate;
      double fineSlewRate;
};

class WsfBMEWSensorInfoRecord : public WsfBMSensorInfoRecord
{
   public:
      WsfBMEWSensorInfoRecord* Clone() const override;
      il::SensorEnum GetSensorType() const override;
      bool IsEW() const override;

      bool HasEstimatedAvailableCues() const { return true; } // by default, non-tracking radars will have infinite cues
};

class WsfBMTARSensorInfoRecord : public WsfBMSensorInfoRecord
{
   public:
      WsfBMTARSensorInfoRecord* Clone() const override;
      il::SensorEnum GetSensorType() const override;
      bool IsTAR() const override;
      bool HasEstimatedAvailableCues() const override { return true; } // by default, non-tracking radars will have infinite cues
};

class WsfBMTTRSensorInfoRecord : public WsfBMSensorInfoRecord
{
   public:
      WsfBMTTRSensorInfoRecord* Clone() const override;
      il::SensorEnum GetSensorType() const override;
      bool IsTTR() const override;
};

class WsfBMRWRSensorInfoRecord : public WsfBMSensorInfoRecord
{
   public:
      WsfBMRWRSensorInfoRecord* Clone() const override;
      il::SensorEnum GetSensorType() const override;
      bool IsRWR() const override;
      bool HasEstimatedAvailableCues() const override { return true; } // by default, non-tracking radars will have infinite cues

      bool IsValid() const;
      WsfStringId GetNameId() const;
      int GetRWRActiveTrackCount() const;
      void GetRWRActiveTrackList(WsfTrackList& trackList);
};

#endif
