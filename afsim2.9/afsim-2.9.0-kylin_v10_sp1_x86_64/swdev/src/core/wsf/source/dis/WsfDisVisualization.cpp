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

#include "dis/WsfDisVisualization.hpp"

#include "DisOther.hpp"
#include "GenMemO.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfVisualization.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisUtil.hpp"
#include "ext/WsfExtInterface.hpp"

WsfDisVisualization::WsfDisVisualization(WsfDisInterface* aInterfacePtr)
   : mInterfacePtr(aInterfacePtr)
{
   mCallbacks +=
      WsfDisUtil::GetExtInterface(mInterfacePtr)->ConnectionComplete.Connect(&WsfDisVisualization::Connected, this);
}

void WsfDisVisualization::Connected(double aSimTime)
{
   const WsfVisualization::BehaviorMap& behaviors = WsfVisualization::GetInstance().GetBehaviors();
   for (WsfVisualization::BehaviorMap::const_iterator i = behaviors.begin(); i != behaviors.end(); ++i)
   {
      WsfPlatform* platformPtr = WsfDisUtil::GetSimulation(mInterfacePtr).GetPlatformByIndex(i->first);
      if (platformPtr != nullptr)
      {
         BehaviorChange(aSimTime, i->second);
      }
   }
   mCallbacks += WsfVisualization::VisibleChange.Connect(&WsfDisVisualization::VisibleChange, this);
   mCallbacks += WsfVisualization::BehaviorChange.Connect(&WsfDisVisualization::BehaviorChange, this);
}

void WsfDisVisualization::VisibleChange(double aSimTime, WsfPlatform* aPlatformPtr, bool aIsVisible)
{
   WsfDisPlatform* disPlatformPtr = WsfDisUtil::GetDisPlatform(mInterfacePtr, aPlatformPtr->GetIndex());
   if (disPlatformPtr != nullptr)
   {
      const DisUint32 cRTS_HEADER             = 0x12345678;
      const DisUint32 cCHANGE_VISIBLE_COMMAND = 2029;
      const DisUint32 cVERSION                = 1;
      GenMemO         data(GenBuf::BigEndian, 0);
      data << cRTS_HEADER << cCHANGE_VISIBLE_COMMAND << cVERSION;
      data << disPlatformPtr->GetEntityId() << (DisInt8&)aIsVisible;
      auto pduPtr = ut::make_unique<DisOther>();
      pduPtr->SetUserData((unsigned char*)data.GetBuffer(), (DisUint16)data.GetPutOffset());
      WsfDisUtil::PutPdu(mInterfacePtr, aSimTime, std::move(pduPtr));
   }
}

void WsfDisVisualization::BehaviorChange(double aSimTime, WsfVisualization::Behavior aBehavior)
{
   WsfDisPlatform* disPlatformPtr = WsfDisUtil::GetDisPlatform(mInterfacePtr, aBehavior.mPlatformPtr->GetIndex());
   if (disPlatformPtr != nullptr)
   {
      // Force a send of the entity state, to help behaviors match movement.
      WsfExtEntityDeadReckon* drPtr = WsfDisUtil::GetExtInterface(mInterfacePtr)->GetEntityDeadReckon();
      if (drPtr != nullptr)
      {
         drPtr->ForceUpdate(aSimTime, aBehavior.mPlatformPtr, true);
      }
      // Send a DisOther PDU with the new behavior
      const DisUint32 cRTS_HEADER             = 0x12345678;
      const DisUint32 cCHANGE_VISIBLE_COMMAND = 2030;
      const DisUint32 cVERSION                = 1;

      GenMemO data(GenBuf::BigEndian, 0);
      data << cRTS_HEADER << cCHANGE_VISIBLE_COMMAND << cVERSION;
      data << disPlatformPtr->GetEntityId();
      data << (DisInt32)aBehavior.mBehaviorType;
      data << (DisFloat32)aBehavior.mAnimationOffsetTime;
      data << (DisFloat32)aBehavior.mAnimationSpeed;

      auto pduPtr = ut::make_unique<DisOther>();
      pduPtr->SetUserData((unsigned char*)data.GetBuffer(), (DisUint16)data.GetPutOffset());
      WsfDisUtil::PutPdu(mInterfacePtr, aSimTime, std::move(pduPtr));
   }
}
