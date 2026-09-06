// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "RvWkDataExtension.hpp"

#include "RvEnvironment.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "RvWkEventPipeClasses.hpp"
#include "RvWkEventPipeClassesRegister.hpp"
#include "RvWkEventPipeSchema.hpp"
#include "UtEllipsoidalEarth.hpp"

bool rv::WkDataExtension::HandlesMessage(int aMessageId) const
{
   switch (aMessageId)
   {
   case MsgUserAction::cMESSAGE_ID:
   case MsgHUD_Data::cMESSAGE_ID:
   case MsgChatMessage::cMESSAGE_ID:
      return true;
   default:
      return false;
   }
}

QVariant rv::WkDataExtension::HandleMessage(const MsgBase* aMsg, int aColumn, int aRole) const
{
   switch (aMsg->GetMessageId())
   {
   case (MsgUserAction::cMESSAGE_ID):
      return HandleUserAction(static_cast<const MsgUserAction*>(aMsg), aColumn, aRole);
   case (MsgHUD_Data::cMESSAGE_ID):
      return HandleHUD_Data(static_cast<const MsgHUD_Data*>(aMsg), aColumn, aRole);
   case (MsgChatMessage::cMESSAGE_ID):
      return HandleChatMessage(static_cast<const MsgChatMessage*>(aMsg), aColumn, aRole);
   }
   return QVariant();
}

void rv::WkDataExtension::RegisterSerializer(UtPackSerializer& aSerializer) const
{
   UtPack_register_all_wk_events_types(aSerializer);
}

const char* rv::WkDataExtension::GetSchema() const
{
   return cRvWkEventPipe_SCHEMA;
}

bool rv::WkDataExtension::IsOneTimeMessage(int aMessageId) const
{
   return false;
}

bool rv::WkDataExtension::IsEventMessage(int aMessageId) const
{
   return false;
}

void rv::WkDataExtension::AddMessage(ResultDb& aResultDb, MsgBase* aMsg, bool aAtBegin) const
{
   if (aMsg->GetMessageId() == rv::MsgHUD_Data::cMESSAGE_ID)
   {
      auto            msgPtr  = static_cast<rv::MsgHUD_Data*>(aMsg);
      ResultPlatform* platPtr = aResultDb.FindPlatform(msgPtr->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->AddBaseMessage(msgPtr, aAtBegin);
         platPtr->EndWrite();
      }
      else
      {
         msgPtr->platformIndex(0);
      }
   }
}

void rv::WkDataExtension::RemoveMessage(ResultDb& aResultDb, MsgBase* aMsg, bool aAtBegin) const
{
   if (aMsg->GetMessageId() == rv::MsgHUD_Data::cMESSAGE_ID)
   {
      auto            msgPtr  = static_cast<MsgHUD_Data*>(aMsg);
      ResultPlatform* platPtr = aResultDb.FindPlatform(msgPtr->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->RemoveBaseMessage(msgPtr, aAtBegin);
         platPtr->EndWrite();
      }
   }
}

QVariant rv::WkDataExtension::HandleUserAction(const MsgUserAction* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->platformIndex(), aRole, aMsg->platformIndexValid());
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "text";
      }
      return aMsg->text().c_str();
   }
   }
   return QVariant();
}

QVariant rv::WkDataExtension::HandleHUD_Data(const MsgHUD_Data* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->platformIndex(), aRole, aMsg->platformIndex());
   }
   }
   return QVariant();
}

QVariant rv::WkDataExtension::HandleChatMessage(const MsgChatMessage* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "sender";
      }
      return QString::fromStdString(aMsg->name());
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "channel";
      }
      return QString::fromStdString(aMsg->channel());
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "text";
      }
      return QString::fromStdString(aMsg->text());
   }
   }
   return QVariant();
}
