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
*   File: weaponsManagerSAM.hpp
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

#ifndef IADSLIB_WEAPONS_MANAGER_SAM
#define IADSLIB_WEAPONS_MANAGER_SAM

namespace il
{
   class AssetManagerInterface;
}

#include <iadsLib/weaponsManagerInterface.hpp>

namespace il
{

   class IADSLIB_EXPORT weaponsManagerSAM : public WeaponsManagerInterface
   {
      // Abstract interface
   public:
      weaponsManagerSAM ();
      virtual ~weaponsManagerSAM() = default;
      virtual WeaponsManagerInterface *Clone ();

   protected:
      weaponsManagerSAM (const weaponsManagerSAM &from);
   };

}

#endif

