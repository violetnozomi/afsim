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
*   File: weaponsManagerSAM.cpp
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

#include <iadsLib/weaponsManagerSAM.hpp>

namespace il
{

weaponsManagerSAM::weaponsManagerSAM ()
   : WeaponsManagerInterface()
{

}

WeaponsManagerInterface *weaponsManagerSAM::Clone ()
{
   return new weaponsManagerSAM (*this);
}

weaponsManagerSAM::weaponsManagerSAM (const weaponsManagerSAM &from)
   : WeaponsManagerInterface (from)
{

}

} // namespace il
