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
*   File: nullTerrainEngine.cpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name: Radiance Technologies
*   Address:         350 Wynn Drive
*                    Huntsville, AL 35805
*
*   Abstract:
*
*******************************************************************************/

#include <iadsLib/nullTerrainEngine.hpp>

namespace il
{

IADSLIB_EXPORT double nullTerrainEngine::GetElevation(const positionRecord& position)  /* const */
{
   return 0;
}

IADSLIB_EXPORT bool nullTerrainEngine::IsLOSMasked(const positionRecord& position1,
                                                   const positionRecord& position2,
                                                   const bool            ignore_features)  /* const */
{
   return false;
}

} // namespace il
