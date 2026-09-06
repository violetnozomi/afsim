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

#ifndef WSFMILDISINTERFACE_HPP
#define WSFMILDISINTERFACE_HPP

#include "wsf_mil_export.h"

#include <memory>
#include <vector>

#include "DisEntityId.hpp"
#include "DisEntityType.hpp"
#include "DisEventId.hpp"
#include "UtCallbackHolder.hpp"
#include "WsfDirectedEnergyWeapon.hpp"
#include "WsfMilComponentRoles.hpp"
#include "WsfStringId.hpp"
#include "dis/WsfDisInput.hpp"

class DisBeam;
class DisSystem;
class WsfDisLaserDesignations;
class WsfDisWeaponEvents;
class WsfEM_Xmtr;
class WsfImplicitWeapon;
class WsfMilExtInterface;
class WsfTrack;
class WsfWeapon;
class WsfWeaponEngagement;
class WsfMilExtension;

class WSF_MIL_EXPORT WsfMilDisInterface : public WsfDisComponent
{
public:
   static WsfMilDisInterface* Find(const WsfDisInterface& aDisInterface);

   WsfMilDisInterface(WsfMilExtension* aMilExtensionPtr);
   WsfMilDisInterface(const WsfMilDisInterface& aSrc);
   WsfMilDisInterface(WsfMilDisInterface&&) = default;
   WsfMilDisInterface& operator=(const WsfMilDisInterface&) = delete;
   WsfMilDisInterface& operator=(WsfMilDisInterface&&) = default;
   ~WsfMilDisInterface() override;

   WsfDisWeaponEvents* GetWeaponEvents() const { return mWeaponEventsPtr; }

   //! @name Component infrastructure methods
   //@{
   WsfComponent* CloneComponent() const override;
   WsfStringId   GetComponentName() const override { return "#WsfMilDisInterface"; }
   const int*    GetComponentRoles() const override
   {
      static const int roles[] = {cWSF_COMPONENT_MIL_DIS_EXTENSION, 0};
      return roles;
   }
   void* QueryInterface(int aRole) override;
   //@}

   void AddedToDisInterface(WsfDisInterface* aDisInterfacePtr) override;

   bool Initialize(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;

   bool TransferPlatformToInternalControl(double         aSimTime,
                                          WsfPlatform*   aPlatformPtr,
                                          DisEntityId&   aEntityId,
                                          DisEntityType& aEntityType) override;

   bool TransferPlatformToExternalControl(double aSimTime, WsfPlatform* aPlatformPtr) override;

   void PrepareComponent(double aSimTime) override;

   void PlatformCreated(WsfDisPlatform* aPlatformPtr) override;

   bool UpdateEmissionSystem(WsfPlatform* aPlatformPtr, WsfDisEmission* aEmissionPtr, DisSystem* aSystemPtr) override;

   bool AddEmissionSystem(WsfPlatform* aPlatformPtr, WsfDisEmission* aEmissionPtr, DisSystem* aSystemPtr) override;

   bool UpdateXmtrFromBeam(WsfDisEmission* aEmissionPtr, DisBeam* aBeamPtr, WsfEM_Xmtr* aXmtrPtr, bool& aBeamOn) override;

   void UpdateWeapon(WsfDisEmission* aEmissionPtr, DisSystem* aSystemPtr, WsfWeapon* aWeaponPtr);

   void RemoveXmtrs(WsfDisEmission* aEmissionPtr, DisSystem* aSystemPtr, WsfWeapon* aWeaponPtr);

   //! Incoming weapon transfer definition.
   struct IncomingTransfer
   {
   public:
      DisEntityId   mFiringDisEntityId;
      DisEntityType mWeaponDisEntityType;
      WsfStringId   mWeaponSystemType;
   };
   using IncomingTransferList = std::vector<IncomingTransfer>;
   struct OutgoingTransfer
   {
   public:
      WsfStringId mWeaponSystemType;
   };
   using OutgoingTransferList = std::vector<OutgoingTransfer>;

   const IncomingTransferList& GetIncomingTransferList() const { return sIncomingTransferList; }

   const OutgoingTransferList& GetOutgoingTransferList() const { return sOutgoingTransferList; }

   WsfDisInterface*   GetDis() const { return mDisInterfacePtr; }
   WsfMilExtension*   GetMilExtension() const { return mMilExtensionPtr; }
   const WsfScenario& GetScenario() const;

   void UpdateSystemFromWeapon(WsfWeapon* aWeaponPtr, DisSystem* aSystemPtr);
   void UpdateBeamFromXmtr(WsfEM_Xmtr& aXmtr, DisBeam* aBeamPtr, double aMinAz, double aMaxAz, double aMinEl, double aMaxEl);

   void AddImplicitWeaponEngagement(WsfWeaponEngagement* aEngagement, const DisEventId& aId);

   WsfWeaponEngagement* FindImpicitWeaponEngagement(const DisEventId& aId) const;

   void AddDirectedEnergyWeapon(std::unique_ptr<WsfDirectedEnergyWeapon> aWeaponPtr, const DisEventId& aId);

   WsfDirectedEnergyWeapon* FindDirectedEnergyWeapon(const DisEventId& aId) const;

   void DeleteDirectedEnergyWeapon(const DisEventId& aId);

private:
   void WeaponFired(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTrackPtr);

   void DirectedEnergyWeaponBeginShot(double                     aSimTime,
                                      WsfDirectedEnergyWeapon*   aWeaponPtr,
                                      const WsfWeaponEngagement* aEngagementPtr);

   void DirectedEnergyWeaponUpdateShot(double                     aSimTime,
                                       WsfDirectedEnergyWeapon*   aWeaponPtr,
                                       const WsfWeaponEngagement* aEngagementPtr);

   void DirectedEnergyWeaponEndShot(double                     aSimTime,
                                    WsfDirectedEnergyWeapon*   aWeaponPtr,
                                    const WsfWeaponEngagement* aEngagementPtr);

   void ImplicitWeaponEndEngagement(double                     aSimTime,
                                    WsfImplicitWeapon*         aImplicitWeapon,
                                    const WsfWeaponEngagement* aEngagementPtr);

   void WeaponTerminated(double aSimTime, const WsfWeaponEngagement* aEngagementPtr);

   void WeaponTurnedOff(double aSimTime, WsfWeapon* aWeaponPtr);

   void WeaponTurnedOn(double aSimTime, WsfWeapon* aWeaponPtr);

   void JammingRequestCanceled(double aSimTime, WsfWeapon* aWeaponPtr, double aFrequency, double aBandwidth, size_t aTargetIndex);

   void JammingRequestInitiated(double      aSimTime,
                                WsfWeapon*  aWeaponPtr,
                                double      aFrequency,
                                double      aBandwidth,
                                WsfStringId aTechniqueId,
                                size_t      aTargetIndex);

   void JammingRequestUpdated(double      aSimTime,
                              WsfWeapon*  aWeaponPtr,
                              double      aFrequency,
                              double      aBandwidth,
                              WsfStringId aTechniqueId,
                              size_t      aTargetIndex);
   // Inputs:
   bool                     mSuppressDirectedEnergyData;
   WsfDisInterface*         mDisInterfacePtr;
   WsfMilExtInterface*      mExtMilPtr;
   WsfDisLaserDesignations* mLaserDesignationsPtr;
   WsfDisWeaponEvents*      mWeaponEventsPtr;
   WsfMilExtension*         mMilExtensionPtr;

   //! A list of allowable incoming weapon transfers.
   IncomingTransferList sIncomingTransferList;

   //! A list of allowable outgoing weapon transfers.
   OutgoingTransferList sOutgoingTransferList;

   using ImplicitWeaponEngagementMap = std::map<DisEventId, WsfWeaponEngagement*>;
   using DirectedEnergyWeaponMap     = std::map<DisEventId, std::unique_ptr<WsfDirectedEnergyWeapon>>;

   ImplicitWeaponEngagementMap mImplicitWeaponEngagementMap;
   DirectedEnergyWeaponMap     mDirectedEnergyWeaponMap;

   UtCallbackHolder mCallbacks;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfMilDisInterface, cWSF_COMPONENT_MIL_DIS_EXTENSION)

#endif
