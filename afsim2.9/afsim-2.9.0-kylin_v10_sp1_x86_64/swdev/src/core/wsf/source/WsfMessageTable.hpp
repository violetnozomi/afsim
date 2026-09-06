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

#ifndef WSFMESSAGETABLE_HPP
#define WSFMESSAGETABLE_HPP

#include "wsf_export.h"

#include <map>

class UtInput;
class UtInputBlock;
#include "UtRandom.hpp"
class WsfMessage;
#include "WsfStringId.hpp"

//! WsfMessageTable is a singleton class that maintains default properties
//! of a message type relative to a specific comm type.  For example,
//! we can make an entry for a comm device named RADIO_TRANSCEIVER and
//! configure the default size of various message types that it will send.
//!
//! message_table
//!    comm_type RADIO_TRANSCEIVER
//!       type MY_MESSAGE_1 128 kb priority 5
//!          subtype MY_SUBTYPE_A 256 kb priority 6
//!          subtype MY_SUBTYPE_B 1024 kb
//!       type MY_MESSAGE_2 1024 bits
//!       default 1 byte
//!
//!    default_comm_type
//!       default 128 bits
//! end_message_table
//!
//! To configure default message properties for multiple comm types, simply repeat
//! the comm_type statement.  In addition, there is a default_comm_type statement
//! that allows you to configure default message properties without specifying
//! a comm type (see default_comm_type above).

class WSF_EXPORT WsfMessageTable
{
public:
   struct MsgProp
   {
      MsgProp()
         : mCommType(nullptr)
         , mType(nullptr)
         , mSubType(nullptr)
         , mSize(0)
         , mPriority(0)
      {
      }

      MsgProp(WsfStringId aCommType, WsfStringId aType, WsfStringId aSubType, int aSize, int aPriority)
         : mCommType(aCommType)
         , mType(aType)
         , mSubType(aSubType)
         , mSize(aSize)
         , mPriority(aPriority)
      {
      }

      WsfStringId mCommType;
      WsfStringId mType;
      WsfStringId mSubType;
      int         mSize;
      int         mPriority;
   };
   WsfMessageTable();

   bool ProcessInput(UtInput& aInput);

   bool Initialize();

   void Add(WsfStringId aCommType, WsfStringId aMessageType, WsfStringId aMessageSubType, int aMessageSize, int aMessagePriority);

   void Remove(WsfStringId aCommType, WsfStringId aMessageType, WsfStringId aMessageSubType);

   MsgProp Find(WsfStringId aCommType, WsfStringId aMessageType, WsfStringId aMessageSubType = nullptr) const;

   MsgProp DefaultProp() const;

   MsgProp RandomProp(ut::Random& aRandom) const;

   void SetMessageProp(WsfStringId aCommType, WsfMessage& aMessage) const;

private:
   bool ProcessMessageTypes(UtInputBlock& aInputBlock, WsfStringId aCommTypeId);

   bool ProcessMessageSubTypes(UtInputBlock& aInputBlock, WsfStringId aCommTypeId, WsfStringId aMessageTypeId);

   //! Represents a key into the message size map. It provides an operator<
   //! that will sort the entries according to their comm type, message type,
   //! and message sub-type.

   struct Key
   {
      Key(WsfStringId aCommType, WsfStringId aMessageType, WsfStringId aMessageSubType)
         : mCommType(aCommType)
         , mMessageType(aMessageType)
         , mMessageSubType(aMessageSubType)
      {
      }

      bool operator<(const Key& aRhs) const
      {
         if (mCommType < aRhs.mCommType)
         {
            return true;
         }
         else if (mCommType == aRhs.mCommType)
         {
            if (mMessageType < aRhs.mMessageType)
            {
               return true;
            }
            else if (mMessageType == aRhs.mMessageType)
            {
               if (mMessageSubType < aRhs.mMessageSubType)
               {
                  return true;
               }
            }
         }
         return false;
      }

      WsfStringId mCommType;
      WsfStringId mMessageType;
      WsfStringId mMessageSubType;
   };

   typedef std::map<Key, MsgProp> MessagePropMap;

   MessagePropMap mMessagePropMap;
   MsgProp        mDefaultProp;
};

#endif
