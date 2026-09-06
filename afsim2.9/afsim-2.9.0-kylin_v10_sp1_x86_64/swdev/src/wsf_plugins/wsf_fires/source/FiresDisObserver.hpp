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

#ifndef FIRES_DISOBSERVER_HPP
#define FIRES_DISOBSERVER_HPP

#include "wsf_fires_export.h"

#include <map>

#include "DisEntityId.hpp"
#include "DisEventId.hpp"
#include "dis/WsfDisInterface.hpp"
class DisDetonation;
class DisFire;
#include "UtCallbackHolder.hpp"
class WsfDisDetonation;
class WsfDisFire;
#include "WsfDraw.hpp"
class WsfPlatform;
#include "WsfStringId.hpp"

namespace Fires
{
//! An observer that monitors dis fire and detonate pdus
//! and performs custom processing on them.
class WSF_FIRES_EXPORT DisObserver
{
public:
   explicit DisObserver(WsfSimulation* aSimPtr);

   // void ProcessPDU(WsfDisFire& aFire);  // incoming

   void ProcessPDU(WsfDisDetonation& aDet); // incoming

   void ProcessFirePDU(DisFire& aFire); // outgoing

   void ProcessDetonationPDU(DisDetonation& aDet); // outgoing

   typedef std::map<WsfStringId, DisEventId> NameToEventMap;
   typedef std::map<DisEventId, WsfStringId> EventToNameMap;

   const NameToEventMap& GetNameToEventMap() const { return mNameToEventMap; }
   const EventToNameMap& GetEventToNameMap() const { return mEventToNameMap; }

private:
   void VisualizeDetonation(const DisEntityId& aFiringEntityId, const DisEntityId& aTargetEntityId);

   // void VisualizeFire(const DisEntityId& aFiringEntityId,
   //                    const DisEntityId& aTargetEntityId) const;

   struct EngagementData
   {
      size_t      mPlatformIndex;
      WsfStringId mWeaponName;
      int         mEngagementId;
   };

   WsfSimulation*   mSimPtr;
   WsfDisInterface* mDisInterfacePtr;
   WsfExtInterface* mExtInterfacePtr;

   typedef std::map<DisEventId, EngagementData> EngagementDataMap;
   EngagementDataMap                            mEngagementDataMap;

   NameToEventMap   mNameToEventMap;
   EventToNameMap   mEventToNameMap;
   WsfDraw          mDraw;
   UtCallbackHolder mCallbacks;
};
}; // namespace Fires

#endif
