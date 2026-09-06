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
*   File: weaponRecordInterface.hpp
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

#ifndef IADSLIB_WEAPON_RECORD_INTERFACE
#define IADSLIB_WEAPON_RECORD_INTERFACE

#include <string>
#include <memory>

#include <iadsLib/idRecord.hpp>
#include <iadsLib/enum.hpp>
#include <iadsLib/unitTypeRecord.hpp>
#include <iadsLib/positionRecord.hpp>
#include <iadsLib/zoneRecordInterface.hpp>
#include <iadsLib/referencesProcessor.hpp>

namespace il
{
   class assetRecord;

   typedef idRecord tWeaponID;

   class IADSLIB_EXPORT weaponRecord : public ReferencesWeaponsManager
   {
   public:
      weaponRecord ();
      weaponRecord (const std::shared_ptr<assetRecord> &parent_asset, const tWeaponID &weapon_id, const unitTypeRecord &weapon_type);
      virtual ~weaponRecord() = default;
      virtual idRecord GetParentAssetID () const;
      virtual unitTypeRecord GetType () const;
      virtual tWeaponID GetWeaponID () const;
      virtual double GetAssignmentDelay () const;
      virtual positionRecord GetPosition () const;
      virtual std::shared_ptr<assetRecord> GetParentAsset () const;

      // Tracked locally typically off status perceptions - unless unit is assigned
      virtual void SetEstimatedMunitionsPrepped (const int value);
      virtual void SetEstimatedTotalMunitions (const int value);
      virtual int  GetEstimatedMunitionsPrepped () const;
      virtual int  GetEstimatedTotalMunitions () const;
      // fire channels
      virtual void SetTotalFireChannels (const int value);
      virtual int GetTotalFireChannels () const;
      virtual void SetEstimatedAllocatedFireChannels (const int value);
      virtual int GetEstimatedAllocatedFireChannels () const;
      virtual int GetEstimatedAvailableFireChannels () const;
	  virtual int GetTruthAllocatedFireChannels () const;

      // allocate/deallocate estimations for weapon assignment
      virtual void IncrementEstimations (const int assign_incr, const int munition_incr);
      virtual void DecrementEstimations (const int assign_incr, const int munition_incr);

      // Optionally Abstract Interface - intended to be overridden but default implementation provided since not all
      // sims will be able to satisfy this interface
      virtual int  GetTruthMunitionsPrepped () const;
      virtual int  GetTruthTotalMunitions () const;

      virtual void AddZone(const std::shared_ptr<zoneRecordInterface> &zone);
      virtual const std::vector<std::shared_ptr<zoneRecordInterface>> GetAttachedZones() const;
      virtual bool HasZones () const;

      // Pure Abstract interface
      virtual double GetTimeBetweenShots () const = 0;
      virtual double GetTimeToFire () const = 0;
      virtual bool   HasMinRangeAttribute() const = 0;
      virtual double GetMinRange () const = 0;
      virtual bool   HasMaxRangeAttribute() const = 0;
      virtual double GetMaxRange () const = 0;
      virtual double GetAvgSpeed() const = 0;
      virtual double GetEstPK() const = 0;
      virtual std::string GetName() const = 0;

      virtual int GetShotsFiredAtTrack (const idRecord &track_id) const = 0;
      virtual void SetShotsFiredAtTrack(const idRecord &track_id, const int fired) = 0;
      virtual int GetShotsPendingForTrack(const il::idRecord &track_id) const = 0;
      virtual void SetShotsPendingForTrack(const il::idRecord &track_id, const int fired) = 0;

   protected:
      std::shared_ptr<assetRecord> m_parent_asset;
      tWeaponID m_weapon_id;
      unitTypeRecord m_weapon_type;


      // Estimates are tracked locally by C2 systems based on current dynamic assignments, etc
      /// Estimated number of munitions prepped to fire
      int m_EstimatedMunitionsPrepped;

      /// Estimated total number of munitions (including prepped)
      int m_EstimatedTotalMunitions;

      /// Estimated allocated fire channels
      int m_EstimatedAllocatedFireChannels;

      /// Total fire channels for this weapon
      int m_TotalFireChannels;

      /// Zone data, if present.
      zoneContainer m_attached_zones;
   };

}


#endif

