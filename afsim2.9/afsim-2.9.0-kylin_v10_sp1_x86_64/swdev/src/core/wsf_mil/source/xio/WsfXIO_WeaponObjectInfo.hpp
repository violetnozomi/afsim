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

#ifndef WSFXIO_WEAPONOBJECTINFO_HPP
#define WSFXIO_WEAPONOBJECTINFO_HPP

#include "wsf_mil_export.h"

#include <cassert>
#include <vector>

class WsfEW_EA_EP;
class WsfExplicitWeapon;
#include "WsfMilComponentRoles.hpp"
class WsfRF_Jammer;
class WsfWeapon;
#include "xio/WsfXIO_ObjectInfo.hpp"

//! The parallel of WsfXIO_ObjectInfo for weapons.

//! Defines an EW (EA or EP) technique.
class WsfXIO_EW_Technique
{
public:
   bool                     mInUse;
   bool                     mExternallyControlled;
   std::vector<WsfStringId> mEffects;

   template<typename T>
   void Serialize(T& aBuff)
   {
      aBuff& mInUse& mExternallyControlled& mEffects;
   }
};

//! The set of EW techniques attached to a transmitter/receiver
class WsfXIO_EW_Techniques : public WsfXIO_ExtraInfo
{
public:
   template<typename T>
   void Serialize(T& aBuff)
   {
      aBuff& mEW_Techniques;
   }

   typedef std::map<WsfStringId, WsfXIO_EW_Technique> EW_TechniquesType;
   EW_TechniquesType                                  mEW_Techniques;
};

//! Provides information about a weapon.
class WSF_MIL_EXPORT WsfXIO_WeaponInfo : public WsfXIO_ModedPartInfo
{
public:
   static const ObjectType cPART_INFO_ID = cARTICULATED_PART | cWSF_COMPONENT_WEAPON;

   WsfXIO_WeaponInfo(ObjectType aType = cPART_INFO_ID)
      : WsfXIO_ModedPartInfo(aType)
   {
   }

   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfXIO_ModedPartInfo::Serialize(aBuff);
      aBuff& mQuantityRemaining;
   }

   void UpdateFromStateChange(WsfXIO_PartStateChangePkt& aPkt) override;
   void UpdateFromPartState(WsfXIO_PlatformPartStatePkt& aPkt) override;

   void FireSalvo(const WsfTrackId& aTrackId, int aSalvoSize = 1);
   void FireSalvo(WsfStringId aTargetNameId, int aSalvoSize = 1);

   double mQuantityRemaining;

private:
   WsfXIO_WeaponInfo(const WsfXIO_WeaponInfo&);
};

//! Provides information about a WsfExplicitWeapon
class WSF_MIL_EXPORT WsfXIO_ExplicitWeaponInfo : public WsfXIO_WeaponInfo
{
public:
   static const ObjectType cPART_INFO_ID = cARTICULATED_PART | cWSF_COMPONENT_WEAPON | cDERIVED_TYPE_BEGIN;
   WsfXIO_ExplicitWeaponInfo(ObjectType aType = cPART_INFO_ID)
      : WsfXIO_WeaponInfo(aType)
   {
   }

   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfXIO_WeaponInfo::Serialize(aBuff);
      aBuff& mLaunchedPlatformType;
   }

   //! TypeID of the launched platform type
   WsfStringId mLaunchedPlatformType;

private:
   WsfXIO_ExplicitWeaponInfo(const WsfXIO_ExplicitWeaponInfo&);
};

//! Provides information about a WsfRF_Jammer
class WSF_MIL_EXPORT WsfXIO_JammerWeaponInfo : public WsfXIO_WeaponInfo
{
public:
   static const ObjectType cPART_INFO_ID = cARTICULATED_PART | cWSF_COMPONENT_WEAPON | (2 * cDERIVED_TYPE_BEGIN);
   WsfXIO_JammerWeaponInfo(ObjectType aType = cPART_INFO_ID)
      : WsfXIO_WeaponInfo(aType)
   {
   }
   ~WsfXIO_JammerWeaponInfo() override;

   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfXIO_WeaponInfo::Serialize(aBuff);
      aBuff& mTransmitters& mMasterXmtrs;
   }

   //! Active transmitter spot information
   std::vector<WsfXIO_EM_XmtrInfo*> mTransmitters;

   //! Master transmitter information for each jammer mode
   std::vector<WsfXIO_EM_XmtrInfo*> mMasterXmtrs;

private:
   WsfXIO_JammerWeaponInfo(const WsfXIO_JammerWeaponInfo&);
};

#endif
