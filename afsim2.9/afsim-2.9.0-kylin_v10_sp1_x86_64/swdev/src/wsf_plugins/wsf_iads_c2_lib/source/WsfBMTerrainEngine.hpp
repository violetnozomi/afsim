// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#ifndef WSF_BM_TERRAIN_ENGINE_HPP
#define WSF_BM_TERRAIN_ENGINE_HPP

#include <memory>
#include <WsfBMCommon.hpp>
#include "WsfTerrain.hpp"
#include <iadsLib/terrainInterface.hpp>

class WsfSimulation;

class WSF_IADS_C2_LIB_EXPORT WsfBMTerrainInterface : public il::terrainInterface
{
   public:
      WsfBMTerrainInterface();
      WsfBMTerrainInterface(const WsfBMTerrainInterface& aSrc);

      // delayed initialization requirement - we need to wait until we can obtain a valid simulation pointer
      bool SetSimulation(WsfSimulation* sim_ptr);

      /**
       @brief Get terrain elevation at a specified point.
       @param position Position to get terrain elevation at.
       @returns double Terrain elevation in meters
      */
      double GetElevation(const il::positionRecord& position) override /* const */;

      /**
       @brief Determine if there is terrain obscuration between two points.
       @param position1 First position.
       @param position2 Second position.
       @param ignore_features If true, check terrain masking only. If false, vegetation is considered.
       @returns bool true if terrain is obscured, otherwise false.
      */
      bool IsLOSMasked(const il::positionRecord& position1, const il::positionRecord& position2, const bool ignore_features = true) override /* const */;

   protected:
      WsfSimulation* m_sim_ptr; // not owned by us, must slice
      std::unique_ptr<wsf::Terrain> m_terrain; // owned by us, must maintain
};

#endif
