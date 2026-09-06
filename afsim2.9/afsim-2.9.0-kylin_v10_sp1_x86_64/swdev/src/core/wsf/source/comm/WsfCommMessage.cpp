// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCommMessage.hpp"

#include "UtCast.hpp"
#include "UtMemory.hpp"
#include "script/WsfScriptCommAddress.hpp"

namespace wsf
{
namespace comm
{

// ============================================================================
Message::Identifier::Identifier(const Message& aMessage)
{
   if (!aMessage.SourceMessage())
   {
      throw UtException("Invalid construction of wsf::comm::UniqueMessage - Missing WsfMessage source.");
   }

   mSerialNumber = aMessage.SourceMessage()->GetSerialNumber();

   if (!aMessage.GetTraceRoute().empty())
   {
      mSource = aMessage.GetTraceRoute().front();
   }

   mDestination = aMessage.SourceMessage()->GetDstAddr();
}

// ============================================================================
Message::Identifier::Identifier(const WsfMessage& aMessage)
   : mSerialNumber(aMessage.GetSerialNumber())
   , mDestination(aMessage.GetDstAddr())
{
}

// ============================================================================
bool Message::Identifier::operator==(const Identifier& aRhs) const
{
   return ((mSerialNumber == aRhs.mSerialNumber) && (mSource == aRhs.mSource) && (mDestination == aRhs.mDestination));
}

// ============================================================================
bool Message::Identifier::operator!=(const Identifier& aRhs) const
{
   return !(*this == aRhs);
}

// ============================================================================
Message::Message(std::unique_ptr<WsfMessage> aMessagePtr)
   : mSrcMessagePtr(std::move(aMessagePtr))
   , mHeaders()
   , mTrailers()
   , mTraceRoute()
   , mTransportFeature(0)
   , mTTL(64) //! Default TCP standard TTL
   , mCommResult()
{
}

// ============================================================================
Message::Message(const WsfMessage& aMessage)
   : Message(ut::clone(aMessage))
{
}

// ============================================================================
Message::~Message()
{
   // Delete headers
   for (auto header : mHeaders)
   {
      delete header;
   }
   // Delete trailers
   for (auto trailer : mTrailers)
   {
      delete trailer;
   }
}
// ============================================================================
Message* Message::Clone() const
{
   return new Message(*this);
}

// ============================================================================
MessageHeader* Message::PopHeader()
{
   MessageHeader* lastHeader = mHeaders.back();
   mHeaders.pop_back();
   return lastHeader;
}

// ============================================================================
void Message::PushHeader(MessageHeader* aHeader)
{
   mHeaders.push_back(aHeader);
}

// ============================================================================
void Message::SetSourceMessage(std::unique_ptr<WsfMessage> aMessagePtr)
{
   mSrcMessagePtr = std::move(aMessagePtr);
}

// ============================================================================
MessageTrailer* Message::PopTrailer()
{
   MessageTrailer* lastTrailer = mTrailers.back();
   mTrailers.pop_back();
   return lastTrailer;
}

// ============================================================================
void Message::PushTrailer(MessageTrailer* aTrailer)
{
   mTrailers.push_back(aTrailer);
}

// ============================================================================
//! Copy constructor (for Clone())
Message::Message(const Message& aSrc)
   : WsfAuxDataEnabled(aSrc)
   , mSrcMessagePtr(aSrc.mSrcMessagePtr)
   , mTraceRoute(aSrc.mTraceRoute)
   , mTransportFeature(aSrc.mTransportFeature)
   , mTTL(aSrc.mTTL)
   , mCommResult(aSrc.mCommResult)
{
   // Copy headers
   for (auto header : aSrc.mHeaders)
   {
      PushHeader(header->Clone());
   }
   // Copy trailers
   for (auto trailer : aSrc.mTrailers)
   {
      PushTrailer(trailer->Clone());
   }
}

// ============================================================================
Message& Message::operator=(const Message& aRhs)
{
   if (this != &aRhs)
   {
      WsfAuxDataEnabled::operator=(aRhs);
      mSrcMessagePtr             = aRhs.mSrcMessagePtr;
      mTraceRoute                = aRhs.mTraceRoute;
      mTransportFeature          = aRhs.mTransportFeature;
      mTTL                       = aRhs.mTTL;
      mCommResult                = aRhs.mCommResult;

      // Copy headers
      for (auto header : aRhs.mHeaders)
      {
         PushHeader(header->Clone());
      }
      // Copy trailers
      for (auto trailer : aRhs.mTrailers)
      {
         PushTrailer(trailer->Clone());
      }
   }
   return *this;
}

// ============================================================================
ScriptMessageClass::ScriptMessageClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfCommMessage");

   mIsScriptAccessible = true;

   AddMethod(ut::make_unique<GetTraceRoute>());
   AddMethod(ut::make_unique<GetTTL>());
   AddMethod(ut::make_unique<GetSourceMessage>());
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMessageClass, Message, GetTraceRoute, 0, "Array<WsfAddress>", "")
{
   auto& traceRoute = aObjectPtr->GetTraceRoute();
   auto  data       = ut::make_unique<std::vector<UtScriptData>>();
   for (const auto& address : traceRoute)
   {
      data->emplace_back(WsfScriptCommAddressClass::Create(address));
   }

   aReturnVal.SetPointer(new UtScriptRef(data.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMessageClass, Message, GetTTL, 0, "int", "")
{
   aReturnVal.SetInt(ut::cast_to_int(aObjectPtr->GetTTL()));
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMessageClass, Message, GetSourceMessage, 0, "WsfMessage", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->SourceMessage()->Clone(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_AUX_DATA_SCRIPT_METHODS(ScriptMessageClass, Message)

} // namespace comm
} // namespace wsf
