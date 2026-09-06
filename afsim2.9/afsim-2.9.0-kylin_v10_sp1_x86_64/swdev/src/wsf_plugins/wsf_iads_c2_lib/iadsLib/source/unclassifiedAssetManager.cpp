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
*   File: unclassifiedAssetManager.cpp
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


#include <iadsLib/unclassifiedAssetManager.hpp>
#include <iadsLib/assetManagerInterface.hpp>
#include <iadsLib/BattleManagerIface.hpp>

namespace il
{

unclassifiedAssetManager::unclassifiedAssetManager(std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
   : AssetManagerInterface(aGlobalLogger)
{

}

AssetManagerInterface *unclassifiedAssetManager::Clone ()
{
   return new unclassifiedAssetManager (*this);
}

unclassifiedAssetManager::unclassifiedAssetManager (const unclassifiedAssetManager &from)
   : AssetManagerInterface (from)
{

}

} // namespace il
