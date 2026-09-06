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
*   File: nullTerrainEngine.hpp
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

#ifndef IADSLIB_NULL_TERRAIN_ENGINE_HPP
#define IADSLIB_NULL_TERRAIN_ENGINE_HPP

#include <iadsLib/terrainInterface.hpp>

namespace il
{

   class nullTerrainEngine : public terrainInterface
   {
   public:
      /**
       @brief Get terrain elevation at a specified point.
       @param positionRecord Position to get terrain elevation at.
       @returns double Terrain elevation in meters
      */
      virtual ~nullTerrainEngine() = default;
      IADSLIB_EXPORT virtual double GetElevation (const positionRecord &position) /* const */;

      /**
       @brief Determine if there is terrain obscuration between two points.
       @param positionRecord First position.
       @param positionRecord Second position.
       @returns bool true if terrain is obscured, otherwise false.
      */
      IADSLIB_EXPORT virtual bool IsLOSMasked (const positionRecord &position1, const positionRecord &position2, const bool ignore_features = true) /* const */;
   };
}


#endif

