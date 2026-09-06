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
#ifndef RVWKDATAEXTENSION_HPP
#define RVWKDATAEXTENSION_HPP

#include <QVariant>

#include "RvDataExtension.hpp"

namespace rv
{
class MsgUserAction;
class MsgHUD_Data;
class MsgChatMessage;

class WkDataExtension : public DataExtension
{
public:
   bool        HandlesMessage(int aMessageId) const override;
   QVariant    HandleMessage(const MsgBase* aMsg, int aColumn, int aRole) const override;
   void        RegisterSerializer(UtPackSerializer& aSerializer) const override;
   const char* GetSchema() const override;
   bool        IsOneTimeMessage(int aMessageId) const override;
   bool        IsEventMessage(int aMessageId) const override;
   void        AddMessage(ResultDb& aResultDb, MsgBase* aMsg, bool aAtBeginning) const override;
   void        RemoveMessage(ResultDb& aResultDb, MsgBase* aMsg, bool aAtBeginning) const override;

private:
   QVariant HandleUserAction(const MsgUserAction* aMsg, int aColumn, int aRole) const;
   QVariant HandleHUD_Data(const MsgHUD_Data* aMsg, int aColumn, int aRole) const;
   QVariant HandleChatMessage(const MsgChatMessage* aMsg, int aColumn, int aRole) const;
};
} // namespace rv

#endif
