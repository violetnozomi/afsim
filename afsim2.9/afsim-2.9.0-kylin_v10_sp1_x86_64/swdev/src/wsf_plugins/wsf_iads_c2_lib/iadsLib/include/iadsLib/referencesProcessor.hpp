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
*   File: zoneRecordInterface.hpp
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

#ifndef IADSLIB_REFERENCES_PROCESSOR_HPP
#define IADSLIB_REFERENCES_PROCESSOR_HPP

#include <memory>

namespace il
{

// forward declarations
class BattleManagerInterface;
class AssetManagerInterface;
class DisseminateC2Interface;
class WeaponsManagerInterface;
class SensorsManagerInterface;

class IADSLIB_EXPORT ReferencesBattleManager
{
public:
          virtual ~ReferencesBattleManager() = default;
   inline virtual void SetBattleManager (const std::shared_ptr<BattleManagerInterface> &bm) { m_battle_manager = bm; }
   inline virtual std::shared_ptr<BattleManagerInterface> GetBattleManager() const { return m_battle_manager; }
protected:
   std::shared_ptr<BattleManagerInterface> m_battle_manager;
};

class IADSLIB_EXPORT ReferencesAssetManager
{
public:

          virtual ~ReferencesAssetManager() = default;
   inline virtual void SetAssetManager (const std::shared_ptr<AssetManagerInterface> &am) { m_asset_manager = am; }
   inline virtual std::shared_ptr<AssetManagerInterface> GetAssetManager() const { return m_asset_manager; }
protected:
   std::shared_ptr<AssetManagerInterface> m_asset_manager;
};

class IADSLIB_EXPORT ReferencesC2Dissemination
{
public:
          virtual ~ReferencesC2Dissemination() = default ;
   inline virtual void SetC2Dissemination (const std::shared_ptr<DisseminateC2Interface> &dc2) { m_c2_dissemination = dc2; }
   inline virtual std::shared_ptr<DisseminateC2Interface> GetC2Dissemination() const { return m_c2_dissemination; }
protected:
   std::shared_ptr<DisseminateC2Interface> m_c2_dissemination;
};

class IADSLIB_EXPORT ReferencesWeaponsManager
{
public:
          virtual ~ReferencesWeaponsManager() = default;
   inline virtual void SetWeaponsManager (const std::shared_ptr<WeaponsManagerInterface> &wm) { m_weapons_manager = wm; }
   inline virtual std::shared_ptr<WeaponsManagerInterface> GetWeaponsManager() const { return m_weapons_manager; }
protected:
   std::shared_ptr<WeaponsManagerInterface> m_weapons_manager;
};

class IADSLIB_EXPORT ReferencesSensorsManager
{
public:
          virtual ~ReferencesSensorsManager() = default;
	inline virtual void SetSensorsManager (const std::shared_ptr<SensorsManagerInterface> &sm) { m_sensors_manager = sm; }
	inline virtual std::shared_ptr<SensorsManagerInterface> GetSensorsManager() const { return m_sensors_manager; }
protected:
	std::shared_ptr<SensorsManagerInterface> m_sensors_manager;
};

}

#endif
