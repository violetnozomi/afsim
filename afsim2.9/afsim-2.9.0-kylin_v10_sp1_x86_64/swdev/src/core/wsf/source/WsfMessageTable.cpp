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

#include "WsfMessageTable.hpp"

#include <string>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtRandom.hpp"
#include "WsfMessage.hpp"
#include "WsfStringId.hpp"

// =================================================================================================
// private
WsfMessageTable::WsfMessageTable()
   : mMessagePropMap()
   , mDefaultProp()
{
}

// =================================================================================================
//! Parses the message_table ... end_message_table syntax
//! to build up a message table from an input stream.
//! @param aInput The input stream.
//! @return Returns true if the command was handled.
bool WsfMessageTable::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());
   if (command == "message_table")
   {
      myCommand = true;
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if ((command == "comm_type") || (command == "default_comm_type"))
         {
            // If this the comm_type command, read the comm type.

            WsfStringId commTypeId;
            if (command == "comm_type")
            {
               std::string commType;
               aInput.ReadValue(commType);
               commTypeId = commType;
            }

            // Process and message types that are relative to this comm type.

            bool endOfBlock = ProcessMessageTypes(inputBlock, commTypeId);
            if (endOfBlock)
            {
               break;
            }
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   return myCommand;
}

// =================================================================================================
bool WsfMessageTable::Initialize()
{
   Key                            key(nullptr, nullptr, nullptr);
   MessagePropMap::const_iterator iter = mMessagePropMap.find(key);
   if (iter != mMessagePropMap.end())
   {
      mDefaultProp = iter->second;
   }
   return true;
}

// =================================================================================================
//! Adds an entry to the message table.
//!
//! @param aCommType        The string id of the comm type
//!                         (0 represents the default comm type).
//! @param aMessageType     The string id of the message type
//!                         (0 represents the default message type).
//! @param aMessageSubType  The string id of the message sub type
//!                         (0 represents the default message sub type).
//! @param aMessageSize     The message size in bits.
//! @param aMessagePriority The message priority.
void WsfMessageTable::Add(WsfStringId aCommType,
                          WsfStringId aMessageType,
                          WsfStringId aMessageSubType,
                          int         aMessageSize,
                          int         aMessagePriority)
{
   Key     key(aCommType, aMessageType, aMessageSubType);
   MsgProp prop(aCommType, aMessageType, aMessageSubType, aMessageSize, aMessagePriority);
   // The original code had
   // ' mMessagePropMap.insert(MessagePropMap::value_type(key, prop));' which
   // fails if data had already been provided for the supplied key
   // (see CR 8672). Remember, the WSF convention is that
   // 'last in wins!'
   mMessagePropMap[key] = prop;
}

// =================================================================================================
//! Removes an entry from the message table.
//!
//! @param aCommType       The string id of the comm type
//!                        (0 represents the default comm type).
//! @param aMessageType    The string id of the message type
//!                        (0 represents the default message type).
//! @param aMessageSubType The string id of the message sub type
//!                        (0 represents the default message sub type).
void WsfMessageTable::Remove(WsfStringId aCommType, WsfStringId aMessageType, WsfStringId aMessageSubType)
{
   Key                      key(aCommType, aMessageType, aMessageSubType);
   MessagePropMap::iterator iter = mMessagePropMap.find(key);
   if (iter != mMessagePropMap.end())
   {
      mMessagePropMap.erase(iter);
   }
}

// =================================================================================================
//! Finds the message property for the specified comm type/message type/message sub-type.
//!
//! @param aCommType       The string id of the comm type
//!                        (0 represents the default comm type).
//! @param aMessageType    The string id of the message type
//!                        (0 represents the default message type).
//! @param aMessageSubType The string id of the message sub type
//!                        (0 represents the default message sub type).
//! @return Returns the message property.
//!
//! For example,
//!
//! If you want the global default message size you would call:
//! int size = Find(0, 0, 0);
//!
//! If you want the default message size for a specific message type you would call:
//! int size = Find(0, "WSF_COMM_MESSAGE", 0);
//!
//! If you want the default message size for a specific message type relative to a
//! specific comm type, you would call:
//!
//! int size = Find("COMM_TYPE",
//!                 "WSF_COMM_MESSAGE", 0);
WsfMessageTable::MsgProp WsfMessageTable::Find(WsfStringId aCommType, WsfStringId aMessageType, WsfStringId aMessageSubType) const
{
   // Try exact match (comm_type, type, sub_type).

   Key                            key0(aCommType, aMessageType, aMessageSubType);
   MessagePropMap::const_iterator iter = mMessagePropMap.find(key0);
   if (iter != mMessagePropMap.end())
   {
      return iter->second;
   }

   // If we failed to find an exact, then strip off the sub-type (comm_type, type).
   if (aMessageSubType != 0)
   {
      Key key(aCommType, aMessageType, nullptr);
      iter = mMessagePropMap.find(key);
      if (iter != mMessagePropMap.end())
      {
         return iter->second;
      }
   }

   // If we failed to find a match, the strip off type (comm_type, default)
   if (aMessageType != 0)
   {
      Key key(aCommType, nullptr, nullptr);
      iter = mMessagePropMap.find(key);
      if (iter != mMessagePropMap.end())
      {
         return iter->second;
      }
   }

   // If we still haven't found a match, search based on just the type and sub-type (default_comm_type, type, sub_type)
   if (aCommType != 0)
   {
      Key key(nullptr, aMessageType, aMessageSubType);
      iter = mMessagePropMap.find(key);
      if (iter != mMessagePropMap.end())
      {
         return iter->second;
      }
   }

   // If no match has been found, search based strictly on message type (default_comm_type, type)
   if (aMessageSubType != 0)
   {
      Key key(nullptr, aMessageType, nullptr);
      iter = mMessagePropMap.find(key);
      if (iter != mMessagePropMap.end())
      {
         return iter->second;
      }
   }

   // Finally, return the default size (default_comm_type, default), or an internal default if that wasn't provided.
   return DefaultProp();
}

// =================================================================================================
WsfMessageTable::MsgProp WsfMessageTable::DefaultProp() const
{
   return mDefaultProp;
}

// =================================================================================================
WsfMessageTable::MsgProp WsfMessageTable::RandomProp(ut::Random& aRandom) const
{
   MsgProp prop(DefaultProp());

   // Pick a random message property.
   int randomProp = static_cast<int>(aRandom.Uniform(0.0, static_cast<double>(mMessagePropMap.size())));

   // Find the random message property.
   int count = 0;
   for (MessagePropMap::const_iterator iter = mMessagePropMap.begin(); iter != mMessagePropMap.end(); ++iter)
   {
      if (randomProp == count++)
      {
         prop = iter->second;
         break;
      }
   }
   return prop;
}

// =================================================================================================
//! A static convenience method that sets a message's properties based on
//! entries made in the message table.  The message's properties are only
//! set if they are currently 0.
//!
//! @param aCommType The specific comm type.
//! @param aMessage The message whose properties will be set.
void WsfMessageTable::SetMessageProp(WsfStringId aCommType, WsfMessage& aMessage) const
{
   MsgProp prop(Find(aCommType, aMessage.GetType(), aMessage.GetSubType()));
   if (aMessage.GetSizeBits() == 0)
   {
      aMessage.SetSizeBits(prop.mSize);
   }
   if (aMessage.GetPriority() == 0)
   {
      aMessage.SetPriority(prop.mPriority);
   }
}

// =================================================================================================
//! A helper method called by ProcessInput that parses the
//! 'type' and 'default' statements, which are contained in the
//! 'comm_type' statement.
//! @param aInputBlock The current input stream block.
//! @param aCommTypeId The current string id of the comm type.
// private
bool WsfMessageTable::ProcessMessageTypes(UtInputBlock& aInputBlock, WsfStringId aCommTypeId)
{
   bool endOfBlock = true;
   while (aInputBlock.ReadCommand())
   {
      std::string command(aInputBlock.GetInput().GetCommand());
      if ((command == "type") || (command == "default"))
      {
         WsfStringId messageTypeId;
         if (command == "type")
         {
            std::string messageType;
            aInputBlock.GetInput().ReadValue(messageType);
            messageTypeId = messageType;
         }

         int messageSize;
         aInputBlock.GetInput().ReadValueOfType(messageSize, UtInput::cDATA_SIZE);

         int         messagePriority = 0;
         std::string priority;
         aInputBlock.GetInput().ReadCommand(priority);
         if (priority == "priority")
         {
            aInputBlock.GetInput().ReadValue(messagePriority);
         }
         else
         {
            aInputBlock.GetInput().PushBack(priority);
         }

         // Add the default message size for this message type (or the 'default' message type).
         Add(aCommTypeId, messageTypeId, nullptr, messageSize, messagePriority);

         // Process and subtypes of this message type ('default' type doesn't have subtypes, but they are allowed
         // for compatibility with old input files.)
         endOfBlock = ProcessMessageSubTypes(aInputBlock, aCommTypeId, messageTypeId);
         if (endOfBlock)
         {
            break;
         }
      }
      else if ((command == "comm_type") || (command == "default_comm_type"))
      {
         aInputBlock.GetInput().PushBack(command);
         endOfBlock = false;
         break;
      }
      else
      {
         throw UtInput::UnknownCommand(aInputBlock.GetInput());
      }
   }
   return endOfBlock;
}

// =================================================================================================
//! A helper method called by ProcessMessageTypes that parses the
//! 'subtype' statements, which are contained in the 'type' and
//! 'default' statement.
//!
//! @param aInputBlock The current input stream block.
//! @param aCommTypeId The current comm type string id.
//! @param aMessageTypeId The current message type string id.
// private
bool WsfMessageTable::ProcessMessageSubTypes(UtInputBlock& aInputBlock, WsfStringId aCommTypeId, WsfStringId aMessageTypeId)
{
   bool endOfBlock = true;
   while (aInputBlock.ReadCommand())
   {
      std::string command(aInputBlock.GetInput().GetCommand());
      if (command == "subtype")
      {
         std::string messageSubType;
         aInputBlock.GetInput().ReadValue(messageSubType);
         WsfStringId messageSubTypeId = messageSubType;

         int messageSize;
         aInputBlock.GetInput().ReadValueOfType(messageSize, UtInput::cDATA_SIZE);

         int         messagePriority = 0;
         std::string priority;
         aInputBlock.GetInput().ReadValue(priority);
         if (priority == "priority")
         {
            aInputBlock.GetInput().ReadValue(messagePriority);
         }
         else
         {
            aInputBlock.GetInput().PushBack(priority);
         }

         // Add the message size for this message subtype.
         if (aMessageTypeId != 0)
         {
            Add(aCommTypeId, aMessageTypeId, messageSubTypeId, messageSize, messagePriority);
         }
         else
         {
            // 'sub_type' definitions were allowed in old input files but they had no effect in the code.
            // (The Find method never looked for the default type with a non-null sub-type, as well it shouldn't).
            // Write out a warning message and continue.
            auto out = ut::log::warning()
                       << "'subtype' definitions are not applicable for the 'default' type and will be ignored.";
            out.AddNote() << "In File: " << aInputBlock.GetInput().GetLocation();
         }
      }
      else if ((command == "type") || (command == "default") || (command == "comm_type") ||
               (command == "default_comm_type"))
      {
         aInputBlock.GetInput().PushBack(command);
         endOfBlock = false;
         break;
      }
      else
      {
         throw UtInput::UnknownCommand(aInputBlock.GetInput());
      }
   }
   return endOfBlock;
}
