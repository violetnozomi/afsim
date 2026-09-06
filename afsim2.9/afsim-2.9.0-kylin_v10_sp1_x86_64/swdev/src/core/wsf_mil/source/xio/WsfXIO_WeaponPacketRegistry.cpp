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

#include "xio/WsfXIO_WeaponPacketRegistry.hpp"

#include <cassert>

#include "PakProcessor.hpp"
#include "PakSerializeImpl.hpp"
#include "PakTypeDictionary.hpp"
#include "WsfWeaponTaskResource.hpp"
#include "xio/WsfXIO_WeaponObjectInfo.hpp"

// Helper macro to ease the pain of registering packets
#define VALID_ID_RANGE(ID) ((ID >= 1) && (ID <= cXIO_WSF_EXTENDED_LAST_PACKET_ID))
#define REGISTER_PACKET(Z, N)            \
   aProcessor.RegisterPacket(#Z, new Z); \
   assert(N == Z::cPACKET_ID && VALID_ID_RANGE(N));

//! Registers all packets used by XIO.
//! Unregistered packets cannot be sent or received.
void WsfXIO_WeaponPacketRegistry::RegisterPackets(PakProcessor& aProcessor)
{
   RegisterClasses();
   REGISTER_PACKET(WsfXIO_FireWeaponPkt, 75);
   REGISTER_PACKET(WsfXIO_EW_TechniqueStateChangeRequestPkt, 76);
}

//! Register class types to support polymorphic serialization.
void WsfXIO_WeaponPacketRegistry::RegisterClasses()
{
   static bool isRegistered(false);
   if (!isRegistered)
   {
      isRegistered = true;
      int typeId   = cXIO_WSF_EXTENDED_FIRST_TYPE_ID;
      // Register the task resource types for weapon-related tasks.
      PakTypeDictionary::GetInstance().RegisterType<WsfWeaponTaskResource>(++typeId);
      PakTypeDictionary::GetInstance().RegisterType<WsfJammerTaskResource>(++typeId);
      PakTypeDictionary::GetInstance().RegisterType<WsfUplinkTaskResource>(++typeId);
      PakTypeDictionary::GetInstance().RegisterType<WsfXIO_EW_Techniques>(++typeId);
      PakTypeDictionary::GetInstance().RegisterType<WsfXIO_WeaponInfo>(++typeId);
      PakTypeDictionary::GetInstance().RegisterType<WsfXIO_ExplicitWeaponInfo>(++typeId);
      PakTypeDictionary::GetInstance().RegisterType<WsfXIO_JammerWeaponInfo>(++typeId);
      assert(cXIO_WSF_EXTENDED_LAST_TYPE_ID >= typeId);
   }
}
