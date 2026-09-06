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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "WsfMessage.hpp"

#include "UtStringIdLiteral.hpp"
#include "WsfComm.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

// =================================================================================================
//! Constructor for XIO (de)serialization.
WsfMessage::WsfMessage(WsfStringId aType)
   : mSimulationPtr(nullptr)
   , mStaticType(aType)
   , mType(aType)
   , mSubType(nullptr)
   , mDataTag(0.0)
   , mSerialNumber(0)
   , mSizeBits(0)
   , mPriority(0)
   , mOriginator(nullptr)
   , mOriginatorIndex(0)
   , mSrcAddr()
   , mDstAddr()
   , mNextHopAddr()
   , mGroup()
{
}

// =================================================================================================
//! Constructor
//! @param aType    The message type as a string ID.
//! @param aSrcAddr The address of the interface that originated the message.
//! @param aSimulation The simulation
WsfMessage::WsfMessage(WsfStringId aType, const wsf::comm::Address& aSrcAddr, WsfSimulation& aSimulation)
   : mSimulationPtr(&aSimulation)
   , mStaticType(aType)
   , mType(aType)
   , mSubType(nullptr)
   , mDataTag(0.0)
   , mSerialNumber(aSimulation.NextMessageSerialNumber())
   , mSizeBits(0)
   , mPriority(0)
   , mOriginator()
   , mSrcAddr(aSrcAddr)
   , mDstAddr()
   , mNextHopAddr()
   , mGroup()
{
   auto networkManagerPtr = aSimulation.GetCommNetworkManager();
   if (networkManagerPtr)
   {
      mOriginator = networkManagerPtr->GetComm(aSrcAddr)->GetPlatform()->GetNameId();
   }

   WsfPlatform* platformPtr = aSimulation.GetPlatformByName(mOriginator);
   if (platformPtr != nullptr)
   {
      mOriginatorIndex = platformPtr->GetIndex();
   }
}

// =================================================================================================
//! Constructor.
//! @param aPlatformPtr The originator of the message.
WsfMessage::WsfMessage(WsfPlatform* aPlatformPtr)
   : WsfMessage(WsfStringId{}, aPlatformPtr)
{
}

// =================================================================================================
//! Constructor.
//! @param aType The message type as a string ID.
//! @param aPlatformPtr The originator of the message.
WsfMessage::WsfMessage(WsfStringId aType, WsfPlatform* aPlatformPtr)
   : WsfMessage(aType, WsfStringId(), aPlatformPtr)
{
}

// =================================================================================================
//! Constructor.
//! @param aType The message type as a string ID.
//! @param aSubType The message sub-type as a string ID.
//! @param aPlatformPtr The originator of the message.
WsfMessage::WsfMessage(WsfStringId aType, WsfStringId aSubType, WsfPlatform* aPlatformPtr)
   : mSimulationPtr(aPlatformPtr ? aPlatformPtr->GetSimulation() :
                                   throw UtException("Invalid WsfMessage platform reference."))
   , mStaticType(aType)
   , mType(aType)
   , mSubType(aSubType)
   , mDataTag(0.0)
   , mSerialNumber(mSimulationPtr ? mSimulationPtr->NextMessageSerialNumber() :
                                    throw UtException("Invalid WsfMessage sim reference."))
   , mSizeBits(0)
   , mPriority(0)
   , mOriginator(aPlatformPtr->GetNameId())
   , mOriginatorIndex(aPlatformPtr->GetIndex())
   , mSrcAddr()
   , mDstAddr()
   , mNextHopAddr()
   , mGroup()
{
}

// =================================================================================================
//! Constructor.
//! //! @param aSimulation The simulation
WsfMessage::WsfMessage(WsfSimulation& aSimulation)
   : mSimulationPtr(&aSimulation)
   , mStaticType()
   , mType(nullptr)
   , mSubType(nullptr)
   , mDataTag(0.0)
   , mSerialNumber(aSimulation.NextMessageSerialNumber())
   , mSizeBits(0)
   , mPriority(0)
   , mOriginator()
   , mOriginatorIndex()
   , mSrcAddr()
   , mDstAddr()
   , mNextHopAddr()
   , mGroup()
{
}

// =================================================================================================
// virtual
WsfMessage::~WsfMessage()
{
   UtReferenceTracked::FreeReference();
}

// =================================================================================================
// virtual
//! Clone this object by creating an identical copy and returning a pointer to it.
//! @returns A pointer to the cloned object.
WsfMessage* WsfMessage::Clone() const
{
   return new WsfMessage(*this);
}

// =================================================================================================
// static
//! Get the message type ID for messages of this class.
//! @returns The integer message type ID for messages of this type.
WsfStringId WsfMessage::GetTypeId()
{
   return UtStringIdLiteral("WSF_MESSAGE");
}

// =================================================================================================
// friend
std::ostream& operator<<(std::ostream& aOut, const WsfMessage& aMessage)
{
   aOut << aMessage.GetSerialNumber();
   aOut << " (dest " << aMessage.GetDstAddr();
   aOut << "; size " << aMessage.GetSizeBits();
   aOut << ')';
   return aOut;
}

// =================================================================================================
//! Can this message be replaced by the new message.
//!
//! This routine can be used by communications devices to determine if a message that is to be sent should
//! replace a message is in the queue for transmission. For example, a new track update message should replace
//! any queued track update message for the same track ID.
//!
//! @param aMessage The new message.
//! @returns 'true' if the supplied message is a replacement for this message.
//!
//! @note The default implementation returns 'false' (the message cannot replace the current message).
// virtual
bool WsfMessage::CanBeReplacedBy(const WsfMessage& aMessage) const
{
   return false;
}

// =================================================================================================
void WsfMessage::SetOriginator(WsfPlatform* aPlatformPtr)
{
   mOriginator      = aPlatformPtr->GetNameId();
   mOriginatorIndex = aPlatformPtr->GetIndex();
}

// =================================================================================================
void WsfMessage::SetSrcAddr(const wsf::comm::Address& aSrcAddr)
{
   mSrcAddr = aSrcAddr;

   auto networkManagerPtr = mSimulationPtr->GetCommNetworkManager();
   if (networkManagerPtr)
   {
      mOriginator = networkManagerPtr->GetComm(aSrcAddr)->GetPlatform()->GetNameId();
   }


   WsfPlatform* platformPtr = mSimulationPtr->GetPlatformByName(mOriginator);
   if (platformPtr != nullptr)
   {
      mOriginatorIndex = platformPtr->GetIndex();
   }
}

// =================================================================================================
void WsfMessage::SetNextHopAddr(const wsf::comm::Address& aNextHopAddr)
{
   mNextHopAddr = aNextHopAddr;
}

// =================================================================================================
void WsfMessage::PrintDataTagPrecise(std::ostream& aStream, std::streamsize aPrecision) const
{
   std::ios::fmtflags oldFlags     = aStream.flags();
   std::streamsize    oldPrecision = aStream.precision(aPrecision);
   aStream.setf(std::ios::fixed, std::ios::floatfield);
   aStream << GetDataTag();
   aStream.flags(oldFlags);
   aStream.precision(oldPrecision);
}

const char* WsfMessage::GetScriptClassName() const
{
   return "WsfMessage";
}
