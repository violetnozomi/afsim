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

#ifndef WSFIMPLICITWEAPON_HPP
#define WSFIMPLICITWEAPON_HPP

#include "wsf_mil_export.h"

#include <map>

#include "WsfEvent.hpp"
#include "WsfStringId.hpp"
#include "WsfWeapon.hpp"

//! A representation of a WsfWeapon that does not require an independent flyout.
//! There is no weapon platform created for implicit weapons.  Often,
//! they are simply fired directly at a particular target rather than using a
//! track to provide other information such as target coordinates.
//! The current weapon engagement is maintained by the implicit weapon, whereas
//! in the case of an explicit weapon, it is maintained by the platform
//! instance of the weapon that is flown out.
class WSF_MIL_EXPORT WsfImplicitWeapon : public WsfWeapon
{
public:
   WsfImplicitWeapon(const WsfScenario& aScenario);
   ~WsfImplicitWeapon() override;

   WsfWeapon* Clone() const override;

   bool Initialize(double aSimTime) override;

   void Update(double aSimTime) override;

   const char* GetScriptClassName() const override;

   WsfStringId GetTargetName() const;

   WsfStringId GetTargetOffset() const;

   WsfWeaponEngagement* GetCurrentEngagement() const;

   void ClearCurrentEngagement(double aSimTime);

   WsfWeaponEngagement* GetEngagement(int aEngagementId = 0) const; // Default argument provided to maintain backward compatibility.

   void ClearEngagement(double aSimTime,
                        int    aEngagementId = 0); // Default argument provided to maintain backward compatibility.

   FireResult Fire(double aSimTime, const FireTarget& aTarget, const FireOptions& aSettings) override;

   void CeaseFire(double aSimTime) override;

   //! A nested class containing information needed to keep track
   //! of implicit engagements.  These can be either based on
   //! truth target name and offset, or track data and offset
   //! (the offset need not be specified).  The engagement pointer is stored,
   //! along with the engagement id and a flag indicating whether we've set up
   //! an event to terminate the engagement.
   class EngagementData
   {
   public:
      EngagementData() = default;
      EngagementData(const EngagementData& aSrc);
      EngagementData& operator=(const EngagementData&) = delete;
      ~EngagementData();

      WsfStringId               mTargetName{""};
      size_t                    mTargetIndex{0};
      WsfStringId               mTargetOffset{""};
      std::unique_ptr<WsfTrack> mTargetTrackPtr{nullptr};
      WsfWeaponEngagement*      mEngagementPtr{nullptr};
      int                       mEngagementId{0};
      bool                      mTerminateSet{false};
   };

   class TerminateEngagementEvent : public WsfEvent
   {
   public:
      TerminateEngagementEvent() = default;
      TerminateEngagementEvent(double             aSimTime,
                               size_t             aWeaponPlatformIndex,
                               WsfImplicitWeapon* aImplicitWeaponPtr,
                               EngagementData*&   aEngagementDataPtr);

      EventDisposition Execute() override;

   private:
      size_t             mWeaponPlatformIndex{0};
      WsfImplicitWeapon* mWeaponPtr{nullptr}; // only valid if the firing platform,
                                              // retrieved from mWeaponPlatformIndex, is valid.
      EngagementData* mEngagementDataPtr{nullptr};
   };

protected:
   WsfImplicitWeapon(const WsfImplicitWeapon& aSrc);
   WsfImplicitWeapon& operator=(const WsfImplicitWeapon&) = delete;

   void DeleteEngagementData(EngagementData* aEngagementDataPtr);

   bool BeginEngagement(double aSimTime, EngagementData* aEngagementDataPtr);

   void EngagementComplete(double aSimTime, WsfWeaponEngagement* aEngagementPtr) override;

   friend class TerminateEngagementEvent;

private:
   using EngagementDataMap = std::map<int, EngagementData*>;

   EngagementDataMap mEngagementDataMap;        // Map of engagement serial ids to engagement data.
   EngagementData*   mCurrentEngagementDataPtr; // In case only one engagement occurs at a time.
};
#endif
