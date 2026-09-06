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

#ifndef WSFDISWEAPONEVENTS_HPP
#define WSFDISWEAPONEVENTS_HPP

#include "wsf_mil_export.h"

#include <list>
#include <map>

class DisDirectedEnergyFire;
class DisFire;
class DisDetonation;
#include "DisEntityId.hpp"
#include "DisEntityType.hpp"
#include "DisEventId.hpp"
class DisFire;

#include "UtCallback.hpp"
class UtInput;

#include "WsfDirectedEnergyWeapon.hpp"
class WsfDisInterface;
class WsfDisPlatform;
class WsfGeoPoint;
class WsfImplicitWeapon;
class WsfMilDisInterface;
class WsfMilExtInterface;
class WsfPlatform;
class WsfTrack;
#include "WsfTrackId.hpp"
class WsfWeapon;
class WsfWeaponEngagement;

//! A Helper class for WsfDisInterface to process weapon launch and termination events.
class WSF_MIL_EXPORT WsfDisWeaponEvents
{
public:
   WsfDisWeaponEvents(WsfMilDisInterface* aInterfacePtr);
   virtual ~WsfDisWeaponEvents() = default;

   void WeaponFired(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTrackPtr);

   void DirectedEnergyWeaponFired(double                         aSimTime,
                                  const WsfDirectedEnergyWeapon* aWeaponPtr,
                                  const WsfWeaponEngagement*     aEngagementPtr);

   void DirectedEnergyWeaponHit(double                         aSimTime,
                                const WsfDirectedEnergyWeapon* aWeaponPtr,
                                const WsfWeaponEngagement*     aEngagementPtr);

   void ImplicitWeaponEndEngagement(double                     aSimTime,
                                    WsfImplicitWeapon*         aImplicitWeapon,
                                    const WsfWeaponEngagement* aEngagementPtr);

   void WeaponTerminated(double aSimTime, const WsfWeaponEngagement* aEngagementPtr);

   bool InitiateWeaponTransfer(double aSimTime, DisFire* aPduPtr);

   bool TransferPlatformToExternalControl(double aSimTime, WsfPlatform* aPlatformPtr);

   bool TransferPlatformToInternalControl(double         aSimTime,
                                          WsfPlatform*   aPlatformPtr,
                                          DisEntityId&   aEntityId,
                                          DisEntityType& aEntityType);

   bool IsOutgoingWeaponTransfer(double aSimTime, WsfPlatform* aPlatformPtr);

   UtCallbackListN<void(DisFire&)> ProcessFirePDU;

   UtCallbackListN<void(DisDetonation&)> ProcessDetonationPDU;

   UtCallbackListN<void(const WsfWeapon*, WsfPlatform*, WsfDisPlatform*, DisEntityId&, bool&)> ProcessWeaponPrelaunchStatus;

   UtCallbackListN<void(WsfDisPlatform*, DisEntityId&)> ProcessWeaponTerminated;

   UtCallbackListN<void(std::string)> ReportErrorMessage;

   UtCallbackListN<void(WsfGeoPoint&, int)> SetTrackLocationData;

   UtCallbackListN<void(WsfPlatform*, const WsfWeapon*, const WsfWeaponEngagement*, DisEntityId&)> UpdateFiringPlatformSystems;
   WsfSimulation* GetSimulation() const;

private:
   void WeaponFiredP(double aSimTime, const WsfWeaponEngagement* aEngagementPtr);

   int GetWarheadEnum(const WsfWeapon* aWeaponPtr);

   //! A map of category ids to DIS enumerations for output of warhead info.
   using CategoryToWarheadMap = std::map<int, int>;

   //! Outstanding weapon event definition.
   struct WeaponEvent
   {
      DisEventId    mDisEventId;
      DisEntityId   mFiringDisEntityId;
      DisEntityId   mWeaponDisEntityId;
      DisEntityType mWeaponDisEntityType;
      DisEntityId   mTargetDisEntityId;
   };

   using WeaponEventList = std::map<unsigned int, WeaponEvent>;

   //! Pending incoming weapon transfers.
   struct PendingTransfer
   {
      double        mFiringStartTime;
      size_t        mFiringPlatformIndex;
      WsfTrackId    mTargetTrackId;
      WsfStringId   mWeaponSystemName;
      DisEntityId   mWeaponDisEntityId;
      DisEntityType mWeaponDisEntityType;
   };

   using PendingTransferList = std::list<PendingTransfer>;

   WsfDisInterface*    mInterfacePtr;
   WsfMilDisInterface* mMilPtr;
   WsfMilExtInterface* mExtMilPtr;

   //! A list of weapon events indexed by the WSF engagement serial number
   WeaponEventList mWeaponEventList;

   //! A list of pending (in-process) incoming weapon transfers.
   PendingTransferList mPendingTransferList;
};

#endif
