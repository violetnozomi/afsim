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

#ifndef WSFXIO_WEAPONPACKETREGISTRY_HPP
#define WSFXIO_WEAPONPACKETREGISTRY_HPP

#include "wsf_mil_export.h"

class PakProcessor;
#include "WsfTrack.hpp"
#include "xio/WsfXIO_Data.hpp"
#include "xio/WsfXIO_Defines.hpp"
#include "xio/WsfXIO_Packet.hpp"
#include "xio/WsfXIO_SerializeTypes.hpp"

// See WsfXIO_Packet.hpp for packet ID reservations

//! Performs the registration of XIO packets and classes
class WSF_MIL_EXPORT WsfXIO_WeaponPacketRegistry
{
public:
   static void RegisterPackets(PakProcessor& aProcessor);
   static void RegisterClasses();

private:
};

#define XIO_DEFINE_PACKET_CTOR(PACKET_NAME__, BASE_TYPE__, PACKET_ID__) \
   typedef BASE_TYPE__ BaseType;                                        \
   static const int    cPACKET_ID = PACKET_ID__;                        \
   PACKET_NAME__(int aPacketId = cPACKET_ID)                            \
      : BASE_TYPE__(aPacketId)

#define XIO_DEFINE_PACKET_SERIALIZE() \
   template<typename T>               \
   void Serialize(T& aBuff)           \
   {                                  \
      BaseType::Serialize(aBuff);     \
      DoSerialize(aBuff);             \
   }                                  \
   template<typename T>               \
   void DoSerialize(T& aBuff)

// Macro to implement some boiler plate code for WsfXIO_Packet types, including:
// defining the packet ID and serializing the base class
// PACKET_NAME__ - Name of the packet class
// BASE_TYPE__ - Name of the base class
// PACKET_ID__ - Unique ID of the packet
#define XIO_DEFINE_PACKET(PACKET_NAME__, BASE_TYPE__, PACKET_ID__)    \
   XIO_DEFINE_PACKET_CTOR(PACKET_NAME__, BASE_TYPE__, PACKET_ID__) {} \
   XIO_DEFINE_PACKET_SERIALIZE()

//! Triggers a weapon to fire on a track
class WSF_MIL_EXPORT WsfXIO_FireWeaponPkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_FireWeaponPkt, WsfXIO_Packet, 75)
   {
      using namespace PakSerialization;
      aBuff& mPlatformIndex& mWeaponNameId& mTrackPtr& mTrackId& mTargetNameId& mSalvoSize;
   }
   void ReceiveCleanup() override { delete mTrackPtr; }
   //! Index of the firing platform.
   int32_t mPlatformIndex;
   //! Name of the weapon on the firing platform that is being fired.
   WsfStringId mWeaponNameId;
   //! If mTrackPtr is non-null it will be used
   WsfTrack* mTrackPtr;
   //! If mTrackPtr is null, mTrackId is used to look up a track on the launching platform.
   WsfTrackId mTrackId;
   //! If mTrackId cannot be found, mTargetNameId is used to look up a track.
   //! Auto dis mapping must be on.
   WsfStringId mTargetNameId;
   //! The number of rounds to be fired.
   int mSalvoSize;
};

//! Requests an application to turn on or off an EA/EP technique
class WSF_MIL_EXPORT WsfXIO_EW_TechniqueStateChangeRequestPkt : public WsfXIO_Packet
{
public:
   enum TechniqueType
   {
      cEA,
      cEP
   };

   XIO_DEFINE_PACKET(WsfXIO_EW_TechniqueStateChangeRequestPkt, WsfXIO_Packet, 76)
   {
      aBuff& PakSerialization::Enum(mTechniqueType) & mPlatformIndex& mPartName& mXmtrRcvrIndex& mTechniqueId& mOnOff;
   }

   //! EA or EP technique
   TechniqueType mTechniqueType;
   //! Index of the owning platform
   int32_t mPlatformIndex;
   //! NameID of the sensor or weapon
   WsfStringId mPartName;
   //! Index of the xmtr or rcvr
   int32_t mXmtrRcvrIndex;

   //! ID of the EA/EP technique
   WsfStringId mTechniqueId;
   //! Select (true) or deselect (false) the technique
   bool mOnOff;
};

#endif
