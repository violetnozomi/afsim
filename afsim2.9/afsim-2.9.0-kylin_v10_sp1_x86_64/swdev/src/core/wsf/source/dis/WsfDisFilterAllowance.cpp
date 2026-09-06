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

#include "dis/WsfDisFilterAllowance.hpp"

#include "DisPdu.hpp"
#include "DisPduEnums.hpp"
#include "DisTypes.hpp"
#include "dis/WsfDisEntityState.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisPlatform.hpp"


WsfDisFilterAllowance::WsfDisFilterAllowance()
   : mPduType(DisEnum::Pdu::Type::EntityState)
   , mFilterType(cENTITYTYPE)
   , mTrackedFlag(cALL)
{
}

//! virtual
WsfDisFilterAllowance::~WsfDisFilterAllowance() {}

bool WsfDisFilterAllowance::PduAllowed(DisPdu& aPdu, WsfDisInterface* aInterfacePtr)
{
   DisEnum8 pduType = aPdu.GetPduType();
   if (pduType != mPduType)
   {
      return false;
   }

   bool allowed = true;
   switch (pduType)
   {
   case DisEnum::Pdu::Type::EntityState:
   {
      WsfDisEntityState* entityStatePtr = (WsfDisEntityState*)&aPdu;

      switch (mFilterType)
      {
      case cENTITYTYPE:
      {
         allowed &= (entityStatePtr->GetEntityType() == mEntityType);
      }
      break;
      case cENTITYFORCE:
      {
         allowed &= (entityStatePtr->GetForceId() == mEntityForceId);
      }
      break;
      }

      if (mTrackedFlag == cTRACKED)
      {
         bool        isBeingTracked = false; // how do we figure out if this guy is being tracked?
         size_t      N              = aInterfacePtr->GetDisPlatformCount();
         DisEntityId entityId       = entityStatePtr->GetEntityId();
         for (size_t i = 1; i < N; ++i)
         {
            WsfDisPlatform* disPlatformPtr = aInterfacePtr->GetDisPlatform(i);
            if (disPlatformPtr != nullptr)
            {
               // only want to know if the given entity (entityStatePtr) is being tracked by a certain force
               if (disPlatformPtr->GetForceId() == mTrackingForceId)
               {
                  if (disPlatformPtr->PlatformBeingTracked(entityId))
                  {
                     // disPlatformPtr is the proper force
                     // disPlatformPtr is tracking entityStatePtr
                     isBeingTracked = true;
                     break;
                  }
               }
            }
         }
         allowed &= isBeingTracked;
      }
   }
   break;

   case DisEnum::Pdu::Type::IffAtcNavaids:
   {
      allowed &= true;
   }
   break;

   default:
   {
      allowed &= false; // no filter supports yet
   }
   break;
   }

   return allowed;
}
