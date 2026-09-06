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

#include "xio/WsfXIO_PacketRegistry.hpp"

#include <cassert>

#include "PakProcessor.hpp"
#include "PakSerializeImpl.hpp"
#include "PakTypeDictionary.hpp"

//! The packet version should be incremented whenever the XIO protocol is modified.
//! This will prevent incompatible applications from communicating.
//! NOTE: This should be incremented if ANY of the information that gets serialized is changed.
//!       This includes ANY object contained within a packet (e.g.: track, message, etc.)
int WsfXIO_PacketRegistry::sPacketVersion = 38;

// Helper macro to ease the pain of registering packets
#define VALID_ID_RANGE(ID) ((ID >= 1) && (ID <= cXIO_WSF_LAST_PACKET_ID))
#define REGISTER_PACKET(Z, N)            \
   aProcessor.RegisterPacket(#Z, new Z); \
   assert(N == Z::cPACKET_ID && VALID_ID_RANGE(N));

//! Registers all packets used by XIO.
//! Unregistered packets cannot be sent or received.
void WsfXIO_PacketRegistry::RegisterPackets(PakProcessor& aProcessor)
{
   RegisterClasses();
   REGISTER_PACKET(WsfXIO_HeartbeatPkt, 1);
   REGISTER_PACKET(WsfXIO_InitializePkt, 2);
   REGISTER_PACKET(WsfXIO_SubscriptionCancelPkt, 3);
   REGISTER_PACKET(WsfXIO_RequestPkt, 4);
   REGISTER_PACKET(WsfXIO_ResponsePkt, 5);
   REGISTER_PACKET(WsfXIO_PlatformListUpdatePkt, 6);
   REGISTER_PACKET(WsfXIO_DeletePlatformPkt, 7);
   REGISTER_PACKET(WsfXIO_RequestTracksPkt, 8);
   REGISTER_PACKET(WsfXIO_LocalTrackPkt, 9);
   REGISTER_PACKET(WsfXIO_RawTrackPkt, 10);
   REGISTER_PACKET(WsfXIO_TrackCoastPkt, 11);
   REGISTER_PACKET(WsfXIO_TrackDropPkt, 12);
   REGISTER_PACKET(WsfXIO_InformationRequestPkt, 13);
   REGISTER_PACKET(WsfXIO_PlatformListPkt, 14);
   // Packet 15 is no longer assigned and can be reused.
   // Packet 16 is no longer assigned and can be reused.
   // Packet 17 is no longer assigned and can be reused.
   // Packet 18 is no longer assigned and can be reused.
   // Packet 19 is no longer assigned and can be reused.
   // Packet 20 is no longer assigned and can be reused.
   REGISTER_PACKET(WsfXIO_QueryResolutionPkt, 21);
   REGISTER_PACKET(WsfXIO_PlatformCommandChainPkt, 22);
   REGISTER_PACKET(WsfXIO_PlatformPartInfoPkt, 23);
   REGISTER_PACKET(WsfXIO_RequestPlatformInfoPkt, 24);
   REGISTER_PACKET(WsfXIO_PartStateChangePkt, 25);
   REGISTER_PACKET(WsfXIO_PartStateChangeRequestPkt, 26);
   REGISTER_PACKET(WsfXIO_ReRoutePlatformPkt, 27);
   // Packet 28 is assigned in WsfXIO_SimPacketRegistry.
   REGISTER_PACKET(WsfXIO_RequestDisDataPkt, 29);
   REGISTER_PACKET(WsfXIO_DisPlatformInfoPkt, 30);
   // Packet 31 is assigned in WsfXIO_SimPacketRegistry.
   REGISTER_PACKET(WsfXIO_TaskRequestPkt, 32);
   REGISTER_PACKET(WsfXIO_TaskInitPkt, 33);
   REGISTER_PACKET(WsfXIO_TaskUpdatePkt, 34);
   REGISTER_PACKET(WsfXIO_TaskCommandPkt, 35);
   REGISTER_PACKET(WsfXIO_TypeQueryPkt, 36);
   REGISTER_PACKET(WsfXIO_TypeQueryResolutionPkt, 37);
   REGISTER_PACKET(WsfXIO_TypeListQueryPkt, 38);
   REGISTER_PACKET(WsfXIO_TypeListQueryResolutionPkt, 39);
   REGISTER_PACKET(WsfXIO_MessageRequestPkt, 40);
   // Packet 41 is assigned in WsfXIO_SimPacketRegistry.
   REGISTER_PACKET(WsfXIO_TimeBehindPkt, 42);
   // Packet 43 is no longer assigned and can be reused
   // Packet 44 is no longer assigned and can be reused.
   REGISTER_PACKET(WsfXIO_RequestSimTimePkt, 45);
   REGISTER_PACKET(WsfXIO_SimTimePkt, 46);
   REGISTER_PACKET(WsfXIO_EntityStatePkt, 47);
   REGISTER_PACKET(WsfXIO_SubscriptionPkt, 48);
   REGISTER_PACKET(WsfXIO_PublicationPkt, 49);
   REGISTER_PACKET(WsfXIO_SimTimeCommandPkt, 50);
   REGISTER_PACKET(WsfXIO_DataResponsePkt, 51);
   REGISTER_PACKET(WsfXIO_TrackManagerStateChangeRequestPkt, 52);
   REGISTER_PACKET(WsfXIO_TrackManagerStateChangePkt, 53);
   // Packet 54 is no longer assigned and can be reused.
   REGISTER_PACKET(WsfXIO_ServiceAvailableQuery, 55);
   REGISTER_PACKET(WsfXIO_EntityStateRawPkt, 56);
   REGISTER_PACKET(WsfXIO_RouteUpdatePkt, 57);
   REGISTER_PACKET(WsfXIO_RequestPlatformPartInfoPkt, 58);
   REGISTER_PACKET(WsfXIO_PlatformPartStatePkt, 59);
   REGISTER_PACKET(WsfXIO_ScriptListPkt, 60);
   // Packet 61 is assigned in WsfXIO_SimPacketRegistry.
   REGISTER_PACKET(WsfXIO_DrawPkt, 62);
   REGISTER_PACKET(WsfXIO_PlatformTypeListPkt, 63);
   REGISTER_PACKET(WsfXIO_ExtendedTrackInfoRequestPkt, 64);
   REGISTER_PACKET(WsfXIO_ExtendedTrackInfoPkt, 65);
   REGISTER_PACKET(WsfXIO_RequestSensorDataPkt, 66);
   REGISTER_PACKET(WsfXIO_SensorTrackingRequestUpdatePkt, 67);
   REGISTER_PACKET(WsfXIO_RequestScriptDataPkt, 68);
   REGISTER_PACKET(WsfXIO_RequestScriptDataResponsePkt, 69);
   REGISTER_PACKET(WsfXIO_ScriptCommandPkt, 70);
   REGISTER_PACKET(WsfXIO_RequestScriptSessionPkt, 71);
   REGISTER_PACKET(WsfXIO_RequestScriptSessionResponsePkt, 72);
   // Packet 73 is assigned in WsfXIO_SimPacketRegistry.
}

void WsfXIO_PacketRegistry::RegisterClasses()
{
   static bool isRegistered(false);
   if (!isRegistered)
   {
      isRegistered = true;
      int typeId   = 0;
      PakTypeDictionary::GetInstance().RegisterType<WsfTaskResource>(++typeId);
      PakTypeDictionary::GetInstance().RegisterType<WsfXIO_ExtraInfo>(++typeId);
      PakTypeDictionary::GetInstance().RegisterType<WsfXIO_PlatformPartInfo>(++typeId);
      PakTypeDictionary::GetInstance().RegisterType<WsfXIO_CommInfo>(++typeId);
      PakTypeDictionary::GetInstance().RegisterType<WsfXIO_ProcessorInfo>(++typeId);
      PakTypeDictionary::GetInstance().RegisterType<WsfXIO_SensorInfo>(++typeId);
      assert(cXIO_WSF_LAST_TYPE_ID >= typeId);
   }
}
