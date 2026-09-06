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

#ifndef WsfXIO_WEAPONCOMPONENT_HPP
#define WsfXIO_WEAPONCOMPONENT_HPP

#include "wsf_mil_export.h"

#include <vector>

#include "UtCallbackHolder.hpp"
class WsfApplication;
class WsfEW_EA_EP;
class WsfPlatformPart;
class WsfSimulation;
#include "WsfWeapon.hpp"
class WsfWeaponEngagement;
#include "xio_sim/WsfXIO_Component.hpp"
class WsfXIO_EW_TechniqueStateChangeRequestPkt;
class WsfXIO_FireWeaponPkt;
#include "xio/WsfXIO_PacketRegistry.hpp"
class WsfXIO_PlatformInfoService;
class WsfXIO_PlatformPartInfo;
class WsfXIO_WeaponInfo;

class WSF_MIL_EXPORT WsfXIO_WeaponComponent : public WsfXIO_Component
{
public:
   //! Equate generic definitions in the packet registry to more meaningful definitions;
   enum
   {
      cWEAPON_INFO = WsfXIO_RequestPlatformInfoPkt::cPART_1_INFO,
      cABORT_FIRE  = WsfXIO_TaskCommandPkt::cTASK_COMMAND_1,
   };

   WsfXIO_WeaponComponent();
   ~WsfXIO_WeaponComponent() override = default;

   //! @name Component interface.
   //@{
   WsfComponent* CloneComponent() const override { return new WsfXIO_WeaponComponent(*this); }
   void*         QueryInterface(int aRole) override { return (aRole == mRoles[0]) ? this : nullptr; }
   void          ComponentParentChanged(WsfXIO_Simulation* aXIO_SimulationPtr) override;
   //@}

   //! @name Miscellaneous methods.
   //@{
   WsfXIO_Simulation* GetXIO_Simulation() const { return GetComponentParent(); }
   WsfSimulation&     GetSimulation() const;
   double             GetSimTime() const;
   //@}

   //! @name WsfXIO_Component interface.
   //@{
   void EM_RcvrInfo_Load(WsfXIO_EM_RcvrInfo& aInfo, const WsfEM_Rcvr& aRcvr) override;

   void EM_XmtrInfo_Load(WsfXIO_EM_XmtrInfo& aInfo, const WsfEM_Xmtr& aXmtr) override;

   WsfXIO_PlatformPartInfo* CreatePlatformPartInfo(WsfPlatformPart& aPart) override;

   void RegisterSimulationObservers(WsfSimulation& aSimulation, UtCallbackHolder& aCallbacks) override;

   bool FillPartState(const WsfPlatformPart& aPart, WsfXIO_PartStateChangePkt& aPkt) override;

   WsfObject* FindPartType(int aPartType, WsfStringId aPartTypeName) override;

   WsfObject* FindPart(const WsfPlatform& aPlatform, int aPartType, WsfStringId aPartName) override;

   bool HandleStateChangeRequest(double                            aSimTime,
                                 WsfXIO_PartStateChangeRequestPkt& aPkt,
                                 WsfPlatform*                      aPlatformPtr,
                                 WsfEM_Xmtr*&                      aXmtrPtr,
                                 WsfEM_Rcvr*&                      aRcvrPtr) override;

   bool HandleTaskCommand(double                 aSimTime,
                          WsfXIO_TaskCommandPkt& aPkt,
                          WsfTaskProcessor*      aProcA_Ptr,
                          WsfTaskProcessor*      aProcB_Ptr,
                          WsfTrack*              aTrackPtr,
                          WsfTaskResource*       aResourcePtr) override;
   //@}

private:
   //! @name Handlers for our packet types.
   //@{
   void HandleFireWeaponRequest(WsfXIO_FireWeaponPkt& aPkt);
   void HandleEW_TechniqueStateChangeRequest(WsfXIO_EW_TechniqueStateChangeRequestPkt& aPkt);
   //@}

   //! @name Callbacks from simulation observers.
   //@{
   void WeaponStateChange(double aSimTime, WsfWeapon* aWeaponPtr);

   void WeaponModeChange(double aSimTime, WsfWeapon* aWeaponPtr, WsfWeapon::WsfWeaponMode* aModePtr);

   void WeaponFired(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTrackPtr);

   void AfterWeaponFired();
   //@}

   void LoadTechniques(WsfXIO_EM_XmtrRcvrInfo& aInfo, WsfEW_EA_EP* aEWPtr);

   WsfXIO_PlatformInfoService* mPlatformInfoServicePtr;
   UtCallbackHolder            mCallbacks;

   //! A hack to get around ordering issue of WeaponFired callback to send out updated weapon quantities
   std::vector<std::pair<size_t, WsfStringId>> mWeaponsFired;
};

#endif
