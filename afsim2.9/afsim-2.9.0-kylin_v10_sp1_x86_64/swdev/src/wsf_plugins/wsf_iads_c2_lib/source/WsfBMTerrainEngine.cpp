// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#include "WsfBMTerrainEngine.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"

#include "WsfSimulation.hpp"

WsfBMTerrainInterface::WsfBMTerrainInterface()
   : m_sim_ptr(nullptr)
{
}

WsfBMTerrainInterface::WsfBMTerrainInterface(const WsfBMTerrainInterface& aSrc)
{
   SetSimulation(aSrc.m_sim_ptr);
}

bool WsfBMTerrainInterface::SetSimulation(WsfSimulation* sim_ptr)
{
   m_sim_ptr = sim_ptr;
   auto terrain_iface = m_sim_ptr ? m_sim_ptr->GetTerrainInterface() : nullptr;
   m_terrain = ut::make_unique<wsf::Terrain>(terrain_iface);

   return m_sim_ptr != nullptr;
}

double WsfBMTerrainInterface::GetElevation(const il::positionRecord& position) /* const */
{
   float terrain_ht_m = 0;

   m_terrain->GetElevApprox(position.getLatDegs(), position.getLonDegs(), terrain_ht_m);

   return terrain_ht_m;
}

bool WsfBMTerrainInterface::IsLOSMasked(const il::positionRecord& position1, const il::positionRecord& position2, const bool ignore_features) /* const */
{
   if (ignore_features)
   {
      return m_terrain->MaskedByTerrain(position1.getLatDegs(), position1.getLonDegs(), position1.getAltM(),
                                        position2.getLatDegs(), position2.getLonDegs(), position2.getAltM(),
                                        -1);
   }
   else
   {
      return m_terrain->MaskedByVegetation(position1.getLatDegs(), position1.getLonDegs(), position1.getAltM(),
                                           position2.getLatDegs(), position2.getLonDegs(), position2.getAltM(),
                                           -1);
   }
}
