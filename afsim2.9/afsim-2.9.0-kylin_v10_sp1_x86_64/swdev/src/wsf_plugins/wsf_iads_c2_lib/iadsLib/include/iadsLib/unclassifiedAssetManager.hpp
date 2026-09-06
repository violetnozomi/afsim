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
*   File: unclassifiedAssetManager.hpp
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

#ifndef IADSLIB_UNCLASSIFIED_ASSET_MANAGER
#define IADSLIB_UNCLASSIFIED_ASSET_MANAGER

namespace il
{
   class BattleManagerInterface;
}

#include <iadsLib/assetManagerInterface.hpp>

namespace il
{


   class IADSLIB_EXPORT unclassifiedAssetManager : public AssetManagerInterface
   {
      // Abstract interface
   public:
      unclassifiedAssetManager(std::weak_ptr<logger::GlobalLogger> aGlobalLogger);
      virtual ~unclassifiedAssetManager() = default;
      virtual AssetManagerInterface *Clone ();

   protected:
      unclassifiedAssetManager (const unclassifiedAssetManager &from);

   private:
      unclassifiedAssetManager & operator=(const unclassifiedAssetManager &rhs);
   };

}

#endif

