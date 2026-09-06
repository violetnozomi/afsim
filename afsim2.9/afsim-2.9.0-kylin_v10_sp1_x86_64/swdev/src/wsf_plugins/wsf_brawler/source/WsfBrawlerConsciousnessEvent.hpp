// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFBRAWLERCONSCIOUSNESSEVENT_HPP
#define WSFBRAWLERCONSCIOUSNESSEVENT_HPP

#include <cstddef> // for size_t

#include "WsfEvent.hpp"

//! A BRAWLER Consiousness Event
//! Used by WsfBrawlerPlatform to schedule updates
//! This class replicates the functionality of conevt.f
//! in the BRAWLER v7.5 code
class WsfBrawlerConsicousnessEvent : public WsfEvent
{
public:
   //! Defines the types of consciousness events that can be represented.
   //! This enumeration replicates the values used in icetyp defined on
   //! line 35 of edata.fi in the BRAWLER v7.5 code
   enum ConsicousnessEventType
   {
      SELF_CALLED = 1,
      MESSAGE_RECEIPT,
      NEARBY_UPDATE, // unused in BRAWLER
      PREPLANTED,    // currently unused in WSF
      INFERRED_DETECTION,
      MWS_ALARM
   };

   //! Creates a new BRAWLER Consciousness Event.
   //! @param aSimTime The current simulation time in seconds
   //! @param aPlatformIndex The ID of the platform creating the event
   //! @param aType The type of consciousness event the new event will represent
   WsfBrawlerConsicousnessEvent(double aSimTime, size_t aPlatformIndex, ConsicousnessEventType aType);

   //! Runs the event at the time it was scheduled to occur.
   //! @return The disposition status of the event after it has run
   EventDisposition Execute() override;

private:
   size_t                 mPlatformIndex; //!< Index of the platform associated with this event
   ConsicousnessEventType mType;          //<! The type of consciousness event represented by this event
};

#endif
