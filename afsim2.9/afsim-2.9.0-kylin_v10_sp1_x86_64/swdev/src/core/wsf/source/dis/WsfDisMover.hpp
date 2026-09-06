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

#ifndef WSFDISMOVER_HPP
#define WSFDISMOVER_HPP

#include "wsf_export.h"

#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
#include "ext/WsfExtEntityDeadReckon.hpp"

class DisEntityState;
class WsfDisInterface;
#include "WsfMover.hpp"
class WsfPlatform;
#include "WsfTypes.hpp"

//! A DIS mover is attached to platforms that are being controlled by DIS.
//!
//! It simply updates the kinematic state of the platform by extrapolating from
//! the received DIS Entity State data.

class WSF_EXPORT WsfDisMover : public WsfMover
{
public:
   WsfDisMover() = delete;
   WsfDisMover(WsfDisInterface* aInterfacePtr, WsfPlatform* aPlatformPtr, DisEntityState* aEntityStatePtr);
   ~WsfDisMover() override;

   WsfMover*        Clone() const override;
   void             Update(double aSimTime) override;
   WsfSpatialDomain GetSpatialDomain() const override { return mSpatialDomain; }

private:
   void             DeadReckonChanged(double aSimTime, WsfExtEntityDeadReckon::PlatformDR& aDR);
   WsfDisInterface* mInterfacePtr;
   WsfSpatialDomain mSpatialDomain;

   UtCallbackHolder mConditionalCallbacks;
};
#endif
