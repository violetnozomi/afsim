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

#ifndef WSFXIO_PACKET_HPP
#define WSFXIO_PACKET_HPP

#include "wsf_export.h"

#include "GenUniqueId.hpp"
#include "PakPacket.hpp"
#include "xio/WsfXIO_Defines.hpp"

// Libraries and applications registering new packets should reserve a range of IDs here.
// Other libraries should be written to handle a change in the packet ID range.

// Packets that support the unextended WSF
static const int cXIO_WSF_LAST_PACKET_ID = 74;
// Packets to support the standard extension to WSF.
static const int cXIO_WSF_EXTENDED_FIRST_PACKET_ID = 75;
static const int cXIO_WSF_EXTENDED_LAST_PACKET_ID  = 99;
// Packets assigned to application 1
static const int cXIO_APP_1_FIRST_PACKET_ID = 100;
static const int cXIO_APP_1_LAST_PACKET_ID  = 114;
// 115-199 unused
// Packets assigned to application 2
static const int cXIO_APP_2_FIRST_PACKET_ID = 200;
static const int cXIO_APP_2_LAST_PACKET_ID  = 349;
// 350+ unused

//! Class type registry for polymorphic serialization.
static const int cXIO_WSF_LAST_TYPE_ID           = 10;
static const int cXIO_WSF_SIM_LAST_TYPE_ID       = 20;
static const int cXIO_WSF_EXTENDED_FIRST_TYPE_ID = 21;
static const int cXIO_WSF_EXTENDED_LAST_TYPE_ID  = 30;

//! Base class for XIO Packets.  Provides the header information.
class WSF_EXPORT WsfXIO_Packet : public PakPacket
{
public:
   typedef bool     BaseType;
   static const int cPACKET_ID = -1;

   enum PacketFlags
   {
      //! Indicates the packet should be synchronized if possible
      cSYNCHRONIZED = 1
   };

   WsfXIO_Packet(int aPacketId)
      : PakPacket(aPacketId)
      , mFlags(0)
   {
   }

   //! Serializes the packet header.  Must be called first from derived class Serialize()
   template<typename T>
   void Serialize(T& aBuff)
   {
      aBuff& mApplicationId& mFlags& mTimeStamp;
   }

   float GetTimeStamp() const { return mTimeStamp; }
   void  SetTimeStamp(float aTimeStamp) { mTimeStamp = aTimeStamp; }

   void     SetFlags(uint32_t aFlags) { mFlags = aFlags; }
   void     AddFlags(uint32_t aFlags) { mFlags = mFlags | aFlags; }
   uint32_t GetFlags() const { return mFlags; }

   GenUniqueId mApplicationId;

protected:
   uint32_t mFlags;
   float    mTimeStamp;
};

//! @name Macros to simplify defining packets.
//@{
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

//@}

#endif
