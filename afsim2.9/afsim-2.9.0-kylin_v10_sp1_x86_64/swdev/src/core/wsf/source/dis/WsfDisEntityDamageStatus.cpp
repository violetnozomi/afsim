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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************


#include "dis/WsfDisEntityDamageStatus.hpp"

#include "DisDEDamageDescriptionRecord.hpp"
#include "DisIO.hpp"
#include "UtLog.hpp"
#include "WsfPlatform.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisEntityDamageStatus::WsfDisEntityDamageStatus(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisEntityDamageStatus(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
int WsfDisEntityDamageStatus::Process()
{
   // What is the damaged entity
   const DisEntityId& entityId = GetDamagedEntity();

   auto out = ut::log::info() << "WsfDisEntityDamageStatus: Processing.";
   out.AddNote() << "T = " << WsfDisUtil::GetSimTime(mInterfacePtr);
   out.AddNote() << "Entity: " << entityId;

   WsfDisPlatform* disPlatformPtr = WsfDisUtil::FindDisPlatform(mInterfacePtr, entityId);
   if (disPlatformPtr != nullptr)
   {
      WsfPlatform* platformPtr = disPlatformPtr->GetPlatform();
      if (platformPtr != nullptr)
      {
         out.AddNote() << "Platform: " << platformPtr->GetName();

         // Basically all we can do is set damage appropriately.
         // Once WSF incorporates damaged states this code should be expanded.
         for (auto ddRecIter = GetDDRecordBegin(); ddRecIter != GetDDRecordEnd(); ++ddRecIter)
         {
            DisDDRecord* ddRecPtr   = *ddRecIter;
            DisEnum32    recordType = ddRecPtr->GetRecordType();
            DisEnum8     damage     = 0;

            if (recordType == DisDDRecord::DAMAGE_DESCRIPTION_DD_RECORD_TYPE)
            {
               DisDEDamageDescriptionRecord* damagePtr = dynamic_cast<DisDEDamageDescriptionRecord*>(ddRecPtr);
               damage                                  = damagePtr->GetComponentDamageStatus();
            }

            out.AddNote() << "Damage: " << damage;
            if (damage == 1)
            {
               platformPtr->SetDamageFactor(0.2);
            }
            else if (damage == 2)
            {
               platformPtr->SetDamageFactor(0.5);
            }
            else if (damage == 3)
            {
               platformPtr->SetDamageFactor(0.7);
            }
            else if (damage == 4) // Broken
            {
               platformPtr->SetDamageFactor(1.0);
            }
         }
      }
   }
   return 1;
}
