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
*   File: terrainInterface.hpp
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

#ifndef IADSLIB_TERRAIN_INTERFACE_HPP
#define IADSLIB_TERRAIN_INTERFACE_HPP

#include <iadsLib/positionRecord.hpp>

namespace il
{

   class IADSLIB_EXPORT terrainInterface
   {
   public:
      /**
       @brief Get terrain elevation at a specified point.
       @param positionRecord Position to get terrain elevation at.
       @returns double Terrain elevation in meters
      */
      virtual ~terrainInterface() = default;
      virtual double GetElevation (const positionRecord &position) /* const */ = 0;

      /**
       @brief Determine if there is terrain obscuration between two points.
       @param positionRecord First position.
       @param positionRecord Second position.
       @returns bool true if terrain is obscured, otherwise false.
      */
      virtual bool IsLOSMasked (const positionRecord &position1, const positionRecord &position2, const bool ignore_features = true) /* const */ = 0;
   };
}


#endif

