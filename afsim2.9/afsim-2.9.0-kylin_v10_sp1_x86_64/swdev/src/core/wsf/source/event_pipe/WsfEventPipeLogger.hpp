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

#ifndef WSFEVENTPIPELOGGER_HPP
#define WSFEVENTPIPELOGGER_HPP

#include "WsfEventPipeOptions.hpp"

//! The event pipe interface may be extended.  Implementing this interface, and using WsfEventPipeInterface::AddLogger()
//! allows the event pipe extension to receive information about which events are enabled
class WsfEventPipeLogger
{
public:
   virtual ~WsfEventPipeLogger() = default;

   // Called when the set of active options has changed
   // Note that an event's enable state should be checked for each platform when outputting an event
   // if (!GetPlatformDetailLevel(PLATFORM).IsOptionEnabled(EVENT_ID)) return; // don't output event...
   virtual void UpdateSubscriptions(const WsfEventPipeOptions& aNewCombinedOptions,
                                    const WsfEventPipeOptions& aPreviousCombinedOptions) = 0;
};

#endif
