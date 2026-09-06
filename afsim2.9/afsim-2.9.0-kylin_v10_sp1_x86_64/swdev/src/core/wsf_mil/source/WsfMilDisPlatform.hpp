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

#ifndef WSFMILDISPLATFORM_HPP
#define WSFMILDISPLATFORM_HPP

class DisSystem;
#include "wsf_mil_export.h"

#include "WsfComponent.hpp"
#include "WsfMilComponentRoles.hpp"
#include "dis/WsfDisPlatform.hpp"
class WsfMilDisInterface;
class WsfWeapon;

class WSF_MIL_EXPORT WsfMilDisPlatform : public WsfDisPlatformComponent
{
public:
   static WsfMilDisPlatform* Find(const WsfDisPlatform& aPlatform);

   WsfMilDisPlatform(WsfDisPlatform* aPlatformPtr);
   ~WsfMilDisPlatform() override;

   //! @name Component infrastructure methods
   //@{
   WsfComponent* CloneComponent() const override;
   WsfStringId   GetComponentName() const override;
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   //@}

   void SendingEmission(double aSimTime, DisEmission* aEmissionPtr, DisEventId& aPreviousNumber) override;

   void AllocateEmissionSystems(DisUint8& emitterId) override;

   void TurnOffEmission(DisEmission* aEmissionPtr) override;

   // ============================================================================
   //! Given a WSF weapon, locate its corresponding DIS system.
   DisSystem* GetJammerSystem(WsfWeapon* aWeaponPtr);

   //! A class used to hold data needed by the interface that is related to a jammer
   class JammerData
   {
   public:
      JammerData(WsfWeapon* aWeaponPtr, DisSystem* aSystemPtr)
         : mWeaponPtr(aWeaponPtr)
         , mSystemPtr(aSystemPtr)
         , mLastBeamCountSent(0)
      {
      }

      WsfWeapon* mWeaponPtr;
      DisSystem* mSystemPtr;

      //! The number of beams sent in the last DIS system record.
      DisUint8 mLastBeamCountSent;
   };

   JammerData* GetJammerData(WsfWeapon* aWeaponPtr);

   void UpdateJammerSystem(double aSimTime, WsfWeapon* aWeaponPtr, DisSystem* aSystemPtr, size_t aTargetIndex);

   void WeaponTurnedOff(double aSimTime, WsfWeapon* aWeaponPtr);
   void WeaponTurnedOn(double aSimTime, WsfWeapon* aWeaponPtr);
   void JammingRequestCanceled(double aSimTime, WsfWeapon* aWeaponPtr, size_t aTargetIndex);
   void JammingRequestInitiated(double aSimTime, WsfWeapon* aWeaponPtr, size_t aTargetIndex);
   void JammingRequestUpdated(double aSimTime, WsfWeapon* aWeaponPtr, size_t aTargetIndex);

private:
   WsfMilDisInterface* mMilInterfacePtr;
   WsfDisPlatform*     mPlatformPtr;

   std::vector<JammerData> mJammers;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfMilDisPlatform, cWSF_COMPONENT_MIL_DIS_PLATFORM)

#endif
