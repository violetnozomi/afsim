// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#ifndef WSF_BM_WEAPON_INFO_RECORD_HPP
#define WSF_BM_WEAPON_INFO_RECORD_HPP

#include <WsfObject.hpp>
#include <WsfPlatform.hpp>
#include "WsfBMCommon.hpp"
#include <iadsLib/weaponRecordInterface.hpp>
#include <iadsLib/trackRecord.hpp>

class WsfBMAssetRecord;
class WsfWeapon;

class WSF_IADS_C2_LIB_EXPORT WsfBMWeaponInfoRecord : public WsfObject, public il::weaponRecord
{
   public:

      WsfBMWeaponInfoRecord();
      WsfBMWeaponInfoRecord(const WsfBMWeaponInfoRecord& aSrc);
      virtual ~WsfBMWeaponInfoRecord();

      //! The standard Clone() method (virtual copy constructor).
      virtual WsfBMWeaponInfoRecord* Clone() const;

      void   Set(/* const */ WsfSimulation* sim_ptr, WsfBMAssetRecord* asset, tWeaponSysID weapon_sys_id, il::AssetTypeEnum weapon_type); // TTD: We should look at taking in the weapon reference although that is wrought with potential errors like if the platform is deleted, we may have an invalid reference lifetime is an issue for sure.
      void   Set(/* const */ WsfSimulation* sim_ptr, const std::shared_ptr<il::assetRecord>& asset, tWeaponSysID weapon_sys_id, il::AssetTypeEnum weapon_type);

      int    GetNumMunitions() const;
      int    GetMaxEngagements();
      double GetInterceptSpeed();
      tWeaponSysID GetWsysID() const;
      il::unitTypeRecord GetTypeRecord() const;

      // Optionally Abstract interface
      virtual int  GetTruthMunitionsPrepped() const;
      virtual int  GetTruthTotalMunitions() const;
      virtual int  GetTruthAllocatedFireChannels() const;

      // Pure Abstract interface
      virtual double GetTimeBetweenShots() const;
      virtual double GetTimeToFire() const;
      virtual bool   HasMinRangeAttribute() const;
      virtual double GetMinRange() const;
      virtual bool   HasMaxRangeAttribute() const;
      virtual double GetMaxRange() const;
      virtual double GetAvgSpeed() const;
      virtual double GetEstPK() const;
      virtual std::string GetName() const;

      virtual int GetShotsFiredAtTrack(const il::idRecord& track_id) const;
      virtual void SetShotsFiredAtTrack(const il::idRecord& track_id, const int fired);
      virtual int GetShotsPendingForTrack(const il::idRecord& track_id) const;
      virtual void SetShotsPendingForTrack(const il::idRecord& track_id, const int fired);

   protected:
      const WsfWeapon* GetWeaponRef() const;

      tPlatformRef m_attached_plat;
      WsfSimulation* m_sim_ptr; // we don't own this
};

#endif
