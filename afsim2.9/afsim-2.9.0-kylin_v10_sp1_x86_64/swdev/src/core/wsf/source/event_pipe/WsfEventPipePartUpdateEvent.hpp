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

#ifndef WSFEVENTPIPEPARTUPDATEEVENT_HPP
#define WSFEVENTPIPEPARTUPDATEEVENT_HPP

#include "WsfEvent.hpp"

class WsfArticulatedPart;
class WsfEventPipeInterface;

#include "WsfStringId.hpp"

// ================================================================================================
// ================================================================================================
namespace wsf
{
namespace eventpipe
{
// this is to update the articulation at some rate
class PartUpdateEvent : public WsfEvent
{
public:
   PartUpdateEvent(WsfEventPipeInterface* aInterfacePtr, const WsfArticulatedPart* aPartPtr, double aSimTime, double aUpdateRate);

   EventDisposition Execute() override;

   void DeleteLater() { mDeleteLater = true; }

private:
   void ExecutePrivate();

   double                 mUpdateRate;
   WsfEventPipeInterface* mInterfacePtr;
   size_t                 mPlatformId;
   WsfStringId            mPartNameId;
   bool                   mDeleteLater;
};
} // namespace eventpipe
} // namespace wsf


#endif
