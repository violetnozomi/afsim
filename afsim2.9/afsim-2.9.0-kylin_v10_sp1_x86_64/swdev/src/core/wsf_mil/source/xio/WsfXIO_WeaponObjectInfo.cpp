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

#include "xio/WsfXIO_WeaponObjectInfo.hpp"

#include "xio/WsfXIO_PacketRegistry.hpp"
#include "xio/WsfXIO_WeaponPacketRegistry.hpp"

// =================================================================================================
// virtual
void WsfXIO_WeaponInfo::UpdateFromStateChange(WsfXIO_PartStateChangePkt& aPkt)
{
   WsfXIO_ModedPartInfo::UpdateFromStateChange(aPkt); // Base class
   mQuantityRemaining = aPkt.mQuantityRemaining;
}

// =================================================================================================
// virtual
void WsfXIO_WeaponInfo::UpdateFromPartState(WsfXIO_PlatformPartStatePkt& aPkt)
{
   WsfXIO_ModedPartInfo::UpdateFromPartState(aPkt); // Base class
}

// =================================================================================================
WsfXIO_JammerWeaponInfo::~WsfXIO_JammerWeaponInfo()
{
   // TODO_XIO - what about the master transmitters???? They weren't deleted in the old version???
   for (std::vector<WsfXIO_EM_XmtrInfo*>::const_iterator i = mTransmitters.begin(); i != mTransmitters.end(); ++i)
   {
      delete *i;
   }
}

// =================================================================================================
void WsfXIO_WeaponInfo::FireSalvo(const WsfTrackId& aTrackId, int aSalvoSize)
{
   WsfXIO_FireWeaponPkt pkt;
   pkt.mSalvoSize     = aSalvoSize;
   pkt.mWeaponNameId  = GetName();
   pkt.mPlatformIndex = static_cast<int32_t>(GetPlatformIndex());
   pkt.mTrackId       = aTrackId;
   pkt.mTrackPtr      = nullptr;
   GetPlatform()->GetHostConnection()->Send(pkt);
}

// =================================================================================================
void WsfXIO_WeaponInfo::FireSalvo(WsfStringId aTargetNameId, int aSalvoSize)
{
   WsfXIO_FireWeaponPkt pkt;
   pkt.mSalvoSize     = aSalvoSize;
   pkt.mWeaponNameId  = GetName();
   pkt.mPlatformIndex = static_cast<int32_t>(GetPlatformIndex());
   pkt.mTrackPtr      = nullptr;
   pkt.mTargetNameId  = aTargetNameId;
   GetPlatform()->GetHostConnection()->Send(pkt);
}
