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
*   File: vclWeaponRecord.hpp
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

#ifndef IADSLIB_VCL_WEAPON_RECORD_HPP
#define IADSLIB_VCL_WEAPON_RECORD_HPP

#include <map>
#include <iadsLib/weaponRecordInterface.hpp>
#include <iadsLib/unitTypeRecord.hpp>
namespace il
{
   class assetRecord;

   class vclWeaponRecord : public weaponRecord
   {
   public:
      vclWeaponRecord (const std::shared_ptr<assetRecord> &parent_asset, const tWeaponID &weapon_id, const unitTypeRecord &weapon_Type);

      virtual double GetInterceptSpeed () const;

      // Abstract interface
      virtual double GetTimeBetweenShots () const;
      virtual double GetTimeToFire () const;

      virtual bool   HasMinRangeAttribute() const;
      virtual double GetMinRange () const;
      virtual bool   HasMaxRangeAttribute() const;
      virtual double GetMaxRange () const;

      virtual int GetShotsFiredAtTrack (const idRecord &track_id) const;
      virtual void SetShotsFiredAtTrack (const idRecord &track_id, const int fired);

   protected:
      /// Time delay between consecutive shots
      double m_TimeBetweenShots;

      /// Time from accepted fire command until weapon is actually fired
      double m_TimeToFire;

      /// Minimum range in meters
      double m_MinRange;

      /// Maximum range in meters
      double m_MaxRange;

      /// Intercept speed in m/s.
      double m_InterceptSpeed;

      std::map<idRecord, int> m_shots_fired;
   };

}

#endif

