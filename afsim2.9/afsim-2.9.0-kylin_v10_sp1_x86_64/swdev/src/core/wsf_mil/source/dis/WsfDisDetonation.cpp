// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "dis/WsfDisDetonation.hpp"

#include <iostream>
#include <vector>

#include "Dis.hpp"
#include "DisIO.hpp"
#include "UtInput.hpp"
#include "UtLatPos.hpp"
#include "UtLonPos.hpp"
#include "UtMath.hpp"
#include "UtStringUtil.hpp"
#include "UtVec3.hpp"
#include "WsfExplicitWeaponEffects.hpp"
#include "WsfExtDetonation.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfWeaponEffects.hpp"
#include "WsfWeaponEngagement.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisUtil.hpp"
#include "ext/WsfExtEntityType.hpp"
#include "ext/WsfExtInterface.hpp"

UtCallbackListN<void(WsfDisDetonation&)> WsfDisDetonation::ProcessPDU;

WsfDisDetonation::WsfDisDetonation(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisDetonation(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
int WsfDisDetonation::Process()
{
   // Give observers a chance to react (or change! the pdu)
   ProcessPDU(*this);

   // Extract data from PDU, and execute a WsfExtDetonation
   WsfExtDetonation extDetonation(WsfDisUtil::GetExtInterface(mInterfacePtr));
   WsfDisPlatform*  disPlatformPtr = WsfDisUtil::FindDisPlatform(mInterfacePtr, GetFiringEntity());
   if (disPlatformPtr != nullptr)
   {
      extDetonation.SetFiringEntity(disPlatformPtr->GetPlatform());
   }
   disPlatformPtr = WsfDisUtil::FindDisPlatform(mInterfacePtr, GetTargetEntity());
   if (disPlatformPtr != nullptr)
   {
      extDetonation.SetTargetEntity(disPlatformPtr->GetPlatform());
   }
   disPlatformPtr = WsfDisUtil::FindDisPlatform(mInterfacePtr, GetWeaponEntity());
   if (disPlatformPtr != nullptr)
   {
      extDetonation.SetWeaponEntity(disPlatformPtr->GetPlatform());
   }
   else
   {
      // Set the weapon type, in case this is an implicit engagement
      DisEntityType    disWeaponType = GetWeaponType();
      WsfExtEntityType weaponType(disWeaponType.GetEntityKind(),
                                  disWeaponType.GetDomain(),
                                  disWeaponType.GetCountry(),
                                  disWeaponType.GetCategory(),
                                  disWeaponType.GetSubcategory(),
                                  disWeaponType.GetSpecific(),
                                  disWeaponType.GetExtra());
      WsfStringId      weaponTypeId = WsfDisUtil::GetExtInterface(mInterfacePtr)->GetStringId(weaponType);
      extDetonation.SetWeaponEntityType(weaponTypeId);
   }
   double detLocationWCS[3];
   GetLocation(detLocationWCS[0], detLocationWCS[1], detLocationWCS[2]);
   extDetonation.SetDetonateLocation(detLocationWCS);
   extDetonation.SetResult(GetDetonationResult());
   float impactX;
   float impactY;
   float impactZ;
   GetImpactLocation(impactX, impactY, impactZ);
   double impactXYZ[3] = {(double)impactX, (double)impactY, (double)impactZ};
   extDetonation.SetRelativeDetonationLocation(impactXYZ);
   WsfExtDetonation::Result result;
   extDetonation.Execute(WsfDisUtil::GetSimTime(mInterfacePtr), result);
   return 1;
}
