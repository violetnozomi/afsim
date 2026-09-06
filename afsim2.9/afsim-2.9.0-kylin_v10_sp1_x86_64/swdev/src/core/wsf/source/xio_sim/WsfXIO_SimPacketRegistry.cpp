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

#include "xio_sim/WsfXIO_SimPacketRegistry.hpp"

#include <cassert>

#include "PakProcessor.hpp"
#include "PakSerializeImpl.hpp"
#include "PakTypeDictionary.hpp"
#include "WsfAssociationMessage.hpp"
#include "WsfControlMessage.hpp"
#include "WsfStatusMessage.hpp"
#include "WsfTaskAssignMessage.hpp"
#include "WsfTaskCancelMessage.hpp"
#include "WsfTaskControlMessage.hpp"
#include "WsfTaskStatusMessage.hpp"
#include "WsfTrackDropMessage.hpp"
#include "WsfTrackMessage.hpp"
#include "xio/WsfXIO_SerializeTypes.hpp"

// Helper macro to ease the pain of registering packets
#define VALID_ID_RANGE(ID) ((ID >= 1) && (ID <= cXIO_WSF_LAST_PACKET_ID))
#define REGISTER_PACKET(Z, N)            \
   aProcessor.RegisterPacket(#Z, new Z); \
   assert(N == Z::cPACKET_ID && VALID_ID_RANGE(N));

//! Registers all packets used by XIO simulation.
void WsfXIO_SimPacketRegistry::RegisterPackets(PakProcessor& aProcessor)
{
   RegisterClasses();
   REGISTER_PACKET(WsfXIO_SendMessagePkt, 28);
   REGISTER_PACKET(WsfXIO_DisMessagePkt, 31);
   REGISTER_PACKET(WsfXIO_MessagePkt, 41);
   REGISTER_PACKET(WsfXIO_ScriptExecutePkt, 61);
   REGISTER_PACKET(WsfXIO_AuxDataPkt, 73);
}

void WsfXIO_SimPacketRegistry::RegisterClasses()
{
   static bool isRegistered(false);
   if (!isRegistered)
   {
      isRegistered = true;
      int typeId   = cXIO_WSF_LAST_TYPE_ID + 1;
      PakTypeDictionary::GetInstance().RegisterType<WsfTrackMessage>(++typeId);
      PakTypeDictionary::GetInstance().RegisterType<WsfTrackDropMessage>(++typeId);
      PakTypeDictionary::GetInstance().RegisterType<WsfTaskAssignMessage>(++typeId);
      PakTypeDictionary::GetInstance().RegisterType<WsfTaskCancelMessage>(++typeId);
      PakTypeDictionary::GetInstance().RegisterType<WsfTaskControlMessage>(++typeId);
      PakTypeDictionary::GetInstance().RegisterType<WsfTaskStatusMessage>(++typeId);
      PakTypeDictionary::GetInstance().RegisterType<WsfAssociationMessage>(++typeId);
      PakTypeDictionary::GetInstance().RegisterType<WsfControlMessage>(++typeId);
      PakTypeDictionary::GetInstance().RegisterType<WsfStatusMessage>(++typeId);
      assert(typeId <= cXIO_WSF_SIM_LAST_TYPE_ID);
   }
}
