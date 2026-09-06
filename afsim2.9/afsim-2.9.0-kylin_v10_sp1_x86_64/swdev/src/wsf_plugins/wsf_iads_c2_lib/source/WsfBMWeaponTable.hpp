// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#ifndef WSF_BM_WEAPON_TABLE_HPP
#define WSF_BM_WEAPON_TABLE_HPP

#include <string>
#include <map>

#include <UtInput.hpp>
#include <UtInputBlock.hpp>

#include "WsfBMGenericTypeWrapper.hpp"
#include <iadsLib/weaponTable.hpp>

class WSF_IADS_C2_LIB_EXPORT WsfBMWeaponTable : public WsfBMUnmanagedTypeWrapper<il::weaponTable>
{
   public:
      WsfBMWeaponTable();
      WsfBMWeaponTable(il::weaponTable* table);
      virtual WsfBMWeaponTable* Clone() const;

      virtual bool ProcessInput(UtInput& aInput, bool has_priority = false, bool has_pk_percent = false);

   protected:

      typedef std::map <std::string, il::genericAttribute> tAttributeCache;

      tAttributeCache m_speed_attributes;
      tAttributeCache m_altitude_attributes;

      virtual bool ParseSpeedAttributes(UtInput& aInput, const std::string& end_block_str);
      virtual bool ParseAltitudeAttributes(UtInput& aInput, const std::string& end_block_str);

      virtual bool ParseWeaponRowsBlock(UtInput& aInput, const std::string& end_block_str, bool has_priority = false, bool has_pk_percent = false);
      virtual bool ParseAISubtypePrefs(UtInput& aInput, const std::string& end_block_str, std::shared_ptr<il::weaponTableRow>& wtr);
      virtual bool ParseSAMSubtypePrefs(UtInput& aInput, const std::string& end_block_str, std::shared_ptr<il::weaponTableRow>& wtr);
      virtual bool ParseOtherSubtypePrefs(UtInput& aInput, const std::string& end_block_str, std::shared_ptr<il::weaponTableRow>& wtr);
};

#endif
