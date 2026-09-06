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
#ifndef RVCYBERDATAEXTENSION_HPP
#define RVCYBERDATAEXTENSION_HPP

#include <QVariant>

#include "RvDataExtension.hpp"

namespace rv
{
class MsgCyberAttackInitiated;
class MsgCyberAttackSucceeded;
class MsgCyberAttackDetected;
class MsgCyberAttackAttributed;
class MsgCyberAttackRecovery;
class MsgCyberScanInitiated;
class MsgCyberScanSucceeded;
class MsgCyberAttackFailed;
class MsgCyberScanFailed;
class MsgCyberScanDetected;
class MsgCyberScanAttributed;

class CyberDataExtension : public DataExtension
{
public:
   bool        HandlesMessage(int aMessageId) const override;
   QVariant    HandleMessage(const rv::MsgBase* aMsg, int aColumn, int aRole) const override;
   void        RegisterSerializer(UtPackSerializer& aSerializer) const override;
   const char* GetSchema() const override;
   bool        IsOneTimeMessage(int aMessageId) const override { return false; }
   bool        IsEventMessage(int aMessageId) const { return false; }
   void        AddMessage(ResultDb& aResultDb, MsgBase* aMsg, bool aAtBegin) const override;
   void        RemoveMessage(ResultDb& aResultDb, MsgBase* aMsg, bool aAtBegin) const override;

private:
   QVariant HandleCyberAttackInitiated(const MsgCyberAttackInitiated* aMsg, int aColumn, int aRole) const;
   QVariant HandleCyberAttackSucceeded(const MsgCyberAttackSucceeded* aMsg, int aColumn, int aRole) const;
   QVariant HandleCyberAttackDetected(const MsgCyberAttackDetected* aMsg, int aColumn, int aRole) const;
   QVariant HandleCyberAttackAttributed(const MsgCyberAttackAttributed* aMsg, int aColumn, int aRole) const;
   QVariant HandleCyberAttackRecovery(const MsgCyberAttackRecovery* aMsg, int aColumn, int aRole) const;
   QVariant HandleCyberScanInitiated(const MsgCyberScanInitiated* aMsg, int aColumn, int aRole) const;
   QVariant HandleCyberScanSucceeded(const MsgCyberScanSucceeded* aMsg, int aColumn, int aRole) const;
   QVariant HandleCyberAttackFailed(const MsgCyberAttackFailed* aMsg, int aColumn, int aRole) const;
   QVariant HandleCyberScanFailed(const MsgCyberScanFailed* aMsg, int aColumn, int aRole) const;
   QVariant HandleCyberScanDetected(const MsgCyberScanDetected* aMsg, int aColumn, int aRole) const;
   QVariant HandleCyberScanAttributed(const MsgCyberScanAttributed* aMsg, int aColumn, int aRole) const;

   std::vector<std::unique_ptr<StateCard>> GetStateCardList() const override;

   std::vector<std::unique_ptr<Interaction>> GetInteractionList() const override;
};

} // namespace rv

#endif
