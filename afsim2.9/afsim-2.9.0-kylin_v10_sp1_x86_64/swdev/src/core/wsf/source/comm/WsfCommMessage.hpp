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

#ifndef WSFCOMMMESSAGE_HPP
#define WSFCOMMMESSAGE_HPP

#include "wsf_export.h"

#include <memory>
#include <vector>

#include "UtCloneablePtr.hpp"
#include "UtScriptClassDefine.hpp"
#include "WsfAuxDataEnabled.hpp"
#include "WsfCommAddress.hpp"
#include "WsfCommResult.hpp"
#include "WsfMessage.hpp"
#include "script/WsfScriptAuxDataUtil.hpp"

namespace wsf
{
namespace comm
{

//!
class WSF_EXPORT MessageHeader
{
public:
   virtual ~MessageHeader()             = default;
   virtual MessageHeader* Clone() const = 0;
};

//!
class WSF_EXPORT MessageTrailer
{
public:
   virtual ~MessageTrailer()             = default;
   virtual MessageTrailer* Clone() const = 0;
};

//!
class WSF_EXPORT Message : public UtScriptAccessible, public WsfAuxDataEnabled
{
public:
   Message(std::unique_ptr<WsfMessage> aMessagePtr);
   WSF_DEPRECATED Message(const WsfMessage& aMessage);
   virtual ~Message();
   Message(const Message& aSrc);
   Message(Message&&)  = default;
   Message&    operator=(const Message& aRhs);
   Message&    operator=(Message&&) = default;
   const char* GetScriptClassName() const override { return "WsfCommMessage"; }

   virtual Message* Clone() const;

   ut::CloneablePtr<WsfMessage>&       SourceMessage() { return mSrcMessagePtr; }
   const ut::CloneablePtr<WsfMessage>& SourceMessage() const { return mSrcMessagePtr; }
   virtual void                        SetSourceMessage(std::unique_ptr<WsfMessage> aMessagePtr);

   MessageHeader* PopHeader();
   void           PushHeader(MessageHeader* aHeader);

   MessageTrailer* PopTrailer();
   void            PushTrailer(MessageTrailer* aTrailer);

   std::vector<Address>&       GetTraceRoute() { return mTraceRoute; }
   const std::vector<Address>& GetTraceRoute() const { return mTraceRoute; }

   //! Return by reference accessor/mutator.
   unsigned int& GetTransportFeatures() { return mTransportFeature; }

   size_t       GetTTL() const { return mTTL; }
   virtual void SetTTL(size_t aTTL) { mTTL = aTTL; }

   Result& GetResult() { return mCommResult; }

   //! The Identifier class provides an object that can
   //! be constructed via messages and used as a unique
   //! handle for specifying a message without requiring
   //! the use of an entire message object. Serial numbers
   //! are not solely sufficient to identify a message in
   //! core due to widespread variances in how they have
   //! generated. Equivalent messages are determined based
   //! on a combination of the serial number, source,
   //! and destination.
   class Identifier
   {
   public:
      Identifier() = default;
      Identifier(const Message& aMessage);
      Identifier(const WsfMessage& aMessage); // No source address via WsfMessage - use SetSource()
      virtual ~Identifier() = default;

      void           SetSerialNumber(unsigned int aSerialNumber) { mSerialNumber = aSerialNumber; }
      void           SetSource(const Address& aAddress) { mSource = aAddress; }
      void           SetDestination(const Address& aAddress) { mDestination = aAddress; }
      unsigned int   GetSerialNumber() const { return mSerialNumber; }
      const Address& GetSource() const { return mSource; }
      const Address& GetDestination() const { return mDestination; }

      bool operator==(const Identifier& aRhs) const;
      bool operator!=(const Identifier& aRhs) const;

   private:
      unsigned int mSerialNumber{};
      Address      mSource{};
      Address      mDestination{};
   };

private:
   ut::CloneablePtr<WsfMessage> mSrcMessagePtr; //!< Pointer to the cloned WsfMessage

   // size of sent message - headers + message + trailers
   // ability to split up source message into multiple packets but still reference the same source message

   std::vector<MessageHeader*>  mHeaders;
   std::vector<MessageTrailer*> mTrailers;

   //! Contains a list of addresses of nodes (comms) that this message has passed through
   //! to its destination.
   std::vector<wsf::comm::Address> mTraceRoute;

   //! The transport feature is a bit mask, which mirrors the capabilities defined in the
   //! transport layer of message processing. This is annotated to the message for correct
   //! handling of a message on other layers, without having to query the layer itself for
   //! these settings. It also allows future usage of setting transport features on a message
   //! by message basis.
   unsigned int mTransportFeature;

   //! The TTL (time to live, max hops, etc.) allowable for this message. This is set by user definition of router
   //! settings from the comm device from which the message originated. It will compared to the size
   //! of mTraceRoute at each hop to ensure the limit hasn't been exceeded. This avoids circular
   //! routing, especially with gateway usage.
   size_t mTTL;

   //! Contains the result of a communication interaction. It is carried along with the message allowing
   //! comm devices to fill in data on both the send and receive.
   Result mCommResult;
};

class WSF_EXPORT ScriptMessageClass : public UtScriptClass
{
public:
   ScriptMessageClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(GetTraceRoute);
   UT_DECLARE_SCRIPT_METHOD(GetTTL);
   UT_DECLARE_SCRIPT_METHOD(GetSourceMessage);

   UT_DECLARE_AUX_DATA_SCRIPT_METHODS();
};

} // namespace comm
} // namespace wsf

//! Template specialization for wsf::comm::Message::Identifier. Allows usage of Address in std::unordered_map/set.
namespace std
{
template<>
struct hash<wsf::comm::Message::Identifier>
{
   size_t operator()(const wsf::comm::Message::Identifier& aId) const
   {
      return (aId.GetSerialNumber() * hash<wsf::comm::Address>{}(aId.GetDestination())) ^
             hash<wsf::comm::Address>{}(aId.GetSource());
   }
};

} // namespace std
#endif
