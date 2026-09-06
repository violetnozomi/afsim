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

#ifndef WSFMOVERUPDATEEVENT_HPP
#define WSFMOVERUPDATEEVENT_HPP

#include "wsf_export.h"

#include <cstddef> // for size_t

#include "WsfEvent.hpp"
class WsfPlatform;

/** An event to force automatic position updates. */

class WSF_EXPORT WsfMoverUpdateEvent : public WsfEvent
{
public:
   WsfMoverUpdateEvent(double aSimTime, WsfPlatform* aPlatformPtr);

   EventDisposition Execute() override;

private:
   size_t mPlatformIndex;
};

#endif
