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

#ifndef WSF_RIPR_OBSERVER_HPP
#define WSF_RIPR_OBSERVER_HPP

#include "UtCallback.hpp"
#include "WsfRIPRCommon.hpp"
#include "WsfStringId.hpp"

class WsfRIPRJob;

//! A collection of callback objects.
//!
//! @see WsfObserver for conceptual overview.
struct WsfRIPRObserver
{
   using JobCallback                  = UtCallbackListN<void(double, const WsfRIPRJob*)>;
   using JobBidChangedCallback        = UtCallbackListN<void(double, const WsfRIPRJob*)>;
   using JobPriorityChangedCallback   = UtCallbackListN<void(double, const WsfRIPRJob*)>;
   using JobMaxWinnersChangedCallback = UtCallbackListN<void(double, const WsfRIPRJob*)>;
   using JobAddedCallback             = UtCallbackListN<void(double, const WsfRIPRJob*)>;
   using JobWonCallback               = UtCallbackListN<void(double, const unsigned int, const WsfRIPRJob*)>;
   using ChannelIdListChangedCallback = UtCallbackListN<void(double,
                                                             const size_t, // Platform Index
                                                             WsfStringId,  // processor name
                                                             const ripr::JobChannelIdList&)>;
   using WinnersReallocatedCallback   = UtCallbackListN<void(double,
                                                           const size_t, // Platform Index
                                                           WsfStringId,  // processor name
                                                           const ripr::RIPRWinnerMap&)>;
   using RIPRManagerChangedCallback   = UtCallbackListN<void()>;
   using BidControlChangedCallback    = UtCallbackListN<void(double,
                                                          const size_t, // Platform Index
                                                          WsfStringId,  // processor name
                                                          const bool)>;

   JobAddedCallback             JobAdded;
   JobCallback                  JobRemoved;
   JobWonCallback               JobWon;
   JobCallback                  JobCanceled;
   JobCallback                  JobCompleted;
   JobBidChangedCallback        JobBidChanged;
   JobPriorityChangedCallback   JobPriorityChanged;
   JobMaxWinnersChangedCallback JobMaxWinnersChanged;
   ChannelIdListChangedCallback ChannelIdListChanged;
   RIPRManagerChangedCallback   RIPRManagerChanged;
   BidControlChangedCallback    BidControlChanged;
   WinnersReallocatedCallback   WinnersReallocated;
};

#endif
