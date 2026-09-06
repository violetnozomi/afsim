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

#ifndef WSFXIO_SIMPACKETREGISTRY_HPP
#define WSFXIO_SIMPACKETREGISTRY_HPP

class PakProcessor;
#include "UtScriptData.hpp"
#include "WsfMessage.hpp"
#include "xio/WsfXIO_Packet.hpp"
#include "xio_sim/WsfXIO_ScriptSerialize.hpp"

//! Performs the registration of XIO messages
class WSF_EXPORT WsfXIO_SimPacketRegistry
{
public:
   static void RegisterClasses();
   static void RegisterPackets(PakProcessor& aProcessor);
};

class WSF_EXPORT WsfXIO_SendMessagePkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_SendMessagePkt, WsfXIO_Packet, 28)
   {
      using namespace PakSerialization;
      aBuff& Polymorphic(mMessagePtr);
   }
   void        ReceiveCleanup() override { delete mMessagePtr; }
   WsfMessage* mMessagePtr;
};

class WSF_EXPORT WsfXIO_DisMessagePkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_DisMessagePkt, WsfXIO_Packet, 31)
   {
      aBuff& mSimTime& mPlatformIndex& mCommName& PakSerialization::Polymorphic(mMessagePtr);
   }
   void ReceiveCleanup() override { delete mMessagePtr; }
   //! Simulation time when the message is sent
   double mSimTime;
   //! Index of the platform sending the message
   int32_t mPlatformIndex;
   //! NameID of the comm device transmitting the message
   WsfStringId mCommName;
   //! Message that is sent
   const WsfMessage* mMessagePtr;
};

//! Provides messages associated with a WsfXIO_MessageRequestPkt
class WSF_EXPORT WsfXIO_MessagePkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_MessagePkt, WsfXIO_Packet, 41)
   {
      using namespace PakSerialization;
      aBuff& mPlatformIndex& Enum(mMessageType) & Polymorphic(mMessagePtr);
   }
   void ReceiveCleanup() override { delete mMessagePtr; }
   enum MessageType
   {
      cRECEIVED,
      cSENT
   };

   //! Index of the platform sending or receiving the message
   int mPlatformIndex;
   //! Indicates if the message received or transmitted
   MessageType mMessageType;
   //! The message received or transmitted.
   const WsfMessage* mMessagePtr;
};


//! Requests that an application begin sending state information for a specified platform part
class WSF_EXPORT WsfXIO_ScriptExecutePkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_ScriptExecutePkt, WsfXIO_Packet, 61)
   {
      using namespace PakSerialization;
      aBuff& mIsApplicationMethod& mPlatformIndex& mPartName& mPartType& mScriptName& mArgumentList& mBaseObject& mScriptText;
   }
   void ReceiveCleanup() override { delete mBaseObject; }
   bool mIsApplicationMethod;
   //! Index of the platform where the script resides
   int32_t mPlatformIndex;
   //! Name of part if addressing a script on a platform part
   WsfStringId mPartName;
   //! Type of platform part.  Only used if mPartName is non-null
   int32_t mPartType;
   //! Name of the script to execute
   WsfStringId mScriptName;
   //! List of arguments
   std::vector<UtScriptData> mArgumentList;
   //! For non-static application methods
   UtScriptData* mBaseObject;
   //! For executing a new script function.  When using this form, mScriptName should be empty.
   // The script must be of the form:
   //   void dostuff(int x)\n
   //      writeln("Hello", x); \n
   //   end_script
   std::string mScriptText;
};

//! WsfXIO_AuxDataPkt is sent shortly after a platform's aux data is accessed in a mutable way.
//! On platform initialization, the aux data is sent in WsfXIO_PlatformPartInfoPkt.
class WSF_EXPORT WsfXIO_AuxDataPkt : public WsfXIO_Packet
{
public:
   XIO_DEFINE_PACKET(WsfXIO_AuxDataPkt, WsfXIO_Packet, 73) { aBuff& mPlatformIndex& mAuxData; }

   std::int32_t          mPlatformIndex;
   WsfAttributeContainer mAuxData;
};
#endif
