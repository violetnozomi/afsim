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

#include "WsfEventPipePartUpdateEvent.hpp"

#include "WsfArticulatedPart.hpp"
#include "WsfEventPipeInterface.hpp"
#include "WsfPlatform.hpp"

wsf::eventpipe::PartUpdateEvent::PartUpdateEvent(WsfEventPipeInterface*    aInterfacePtr,
                                                 const WsfArticulatedPart* aPartPtr,
                                                 double                    aSimTime,
                                                 double                    aUpdateRate)
   : WsfEvent(aSimTime)
   , mUpdateRate(aUpdateRate)
   , mInterfacePtr(aInterfacePtr)
   , mDeleteLater(false)
{
   mPlatformId = aPartPtr->GetPlatform()->GetIndex();
   mPartNameId = aPartPtr->GetNameId();
}

WsfEvent::EventDisposition wsf::eventpipe::PartUpdateEvent::Execute()
{
   if (mDeleteLater)
   {
      return WsfEvent::cDELETE;
   }
   ExecutePrivate();
   SetTime(GetTime() + mUpdateRate);
   return WsfEvent::cRESCHEDULE;
}

void wsf::eventpipe::PartUpdateEvent::ExecutePrivate()
{
   mInterfacePtr->UpdatePartArticulation(GetTime(), mPlatformId, mPartNameId);
}
