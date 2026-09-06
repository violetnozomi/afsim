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
#include "RvCyberDataExtension.hpp"

#include "RvCyberEventPipeClasses.hpp"
#include "RvCyberEventPipeClassesRegister.hpp"
#include "RvCyberEventPipeSchema.hpp"
#include "RvEnvironment.hpp"
#include "RvInteractionDb.hpp"
#include "RvResultDb.hpp"

bool rv::CyberDataExtension::HandlesMessage(int aMessageId) const
{
   switch (aMessageId)
   {
   case MsgCyberAttackInitiated::cMESSAGE_ID:
   case (MsgCyberAttackSucceeded::cMESSAGE_ID):
   case (MsgCyberAttackFailed::cMESSAGE_ID):
   case (MsgCyberAttackDetected::cMESSAGE_ID):
   case (MsgCyberAttackAttributed::cMESSAGE_ID):
   case (MsgCyberAttackRecovery::cMESSAGE_ID):
   case (MsgCyberScanInitiated::cMESSAGE_ID):
   case (MsgCyberScanSucceeded::cMESSAGE_ID):
   case (MsgCyberScanFailed::cMESSAGE_ID):
   case (MsgCyberScanDetected::cMESSAGE_ID):
   case (MsgCyberScanAttributed::cMESSAGE_ID):
      return true;
   default:
      return false;
   }
}

QVariant rv::CyberDataExtension::HandleMessage(const MsgBase* aMsg, int aColumn, int aRole) const
{
   switch (aMsg->GetMessageId())
   {
   case (MsgCyberAttackInitiated::cMESSAGE_ID):
      return HandleCyberAttackInitiated(static_cast<const MsgCyberAttackInitiated*>(aMsg), aColumn, aRole);
   case (MsgCyberAttackSucceeded::cMESSAGE_ID):
      return HandleCyberAttackSucceeded(static_cast<const MsgCyberAttackSucceeded*>(aMsg), aColumn, aRole);
   case (MsgCyberAttackFailed::cMESSAGE_ID):
      return HandleCyberAttackFailed(static_cast<const MsgCyberAttackFailed*>(aMsg), aColumn, aRole);
   case (MsgCyberAttackDetected::cMESSAGE_ID):
      return HandleCyberAttackDetected(static_cast<const MsgCyberAttackDetected*>(aMsg), aColumn, aRole);
   case (MsgCyberAttackAttributed::cMESSAGE_ID):
      return HandleCyberAttackAttributed(static_cast<const MsgCyberAttackAttributed*>(aMsg), aColumn, aRole);
   case (MsgCyberAttackRecovery::cMESSAGE_ID):
      return HandleCyberAttackRecovery(static_cast<const MsgCyberAttackRecovery*>(aMsg), aColumn, aRole);
   case (MsgCyberScanInitiated::cMESSAGE_ID):
      return HandleCyberScanInitiated(static_cast<const MsgCyberScanInitiated*>(aMsg), aColumn, aRole);
   case (MsgCyberScanSucceeded::cMESSAGE_ID):
      return HandleCyberScanSucceeded(static_cast<const MsgCyberScanSucceeded*>(aMsg), aColumn, aRole);
   case (MsgCyberScanFailed::cMESSAGE_ID):
      return HandleCyberScanFailed(static_cast<const MsgCyberScanFailed*>(aMsg), aColumn, aRole);
   case (MsgCyberScanDetected::cMESSAGE_ID):
      return HandleCyberScanDetected(static_cast<const MsgCyberScanDetected*>(aMsg), aColumn, aRole);
   case (MsgCyberScanAttributed::cMESSAGE_ID):
      return HandleCyberScanAttributed(static_cast<const MsgCyberScanAttributed*>(aMsg), aColumn, aRole);
   default:
      return QVariant();
   }
}

void rv::CyberDataExtension::RegisterSerializer(UtPackSerializer& aSerializer) const
{
   UtPack_register_all_wsf_cyber_events_types(aSerializer);
}

const char* rv::CyberDataExtension::GetSchema() const
{
   return cRvCyberEventPipe_SCHEMA;
}

void rv::CyberDataExtension::AddMessage(ResultDb& aResultDb, MsgBase* aMsg, bool aAtBegin) const
{
   InteractionDb* idb = aResultDb.LockInteractionDb();
   switch (aMsg->GetMessageId())
   {
   case MsgCyberScanInitiated::cMESSAGE_ID:
   {
      auto* msg = static_cast<MsgCyberScanInitiated*>(aMsg);
      idb->AddPairedStartInteraction(msg->ownerIndex(),
                                     msg->ownerIndex(),
                                     msg->victimIndex(),
                                     msg->GetMessageIndex(),
                                     aAtBegin,
                                     "CyberScan",
                                     msg->simTime(),
                                     msg->attackType());
      if (msg->ownerIndex() != msg->victimIndex())
      {
         idb->AddPairedStartInteraction(msg->victimIndex(),
                                        msg->ownerIndex(),
                                        msg->victimIndex(),
                                        msg->GetMessageIndex(),
                                        aAtBegin,
                                        "CyberScan",
                                        msg->simTime(),
                                        msg->attackType());
      }
   }
   break;
   case MsgCyberScanDetected::cMESSAGE_ID:
   {
      auto* msg = static_cast<MsgCyberScanDetected*>(aMsg);
      idb->AddUnpairedInteraction(msg->ownerIndex(),
                                  msg->ownerIndex(),
                                  msg->victimIndex(),
                                  msg->GetMessageIndex(),
                                  aAtBegin,
                                  "CyberDetect",
                                  msg->simTime(),
                                  msg->attackType());
      if (msg->ownerIndex() != msg->victimIndex())
      {
         idb->AddUnpairedInteraction(msg->victimIndex(),
                                     msg->ownerIndex(),
                                     msg->victimIndex(),
                                     msg->GetMessageIndex(),
                                     aAtBegin,
                                     "CyberDetect",
                                     msg->simTime(),
                                     msg->attackType());
      }
   }
   break;
   case MsgCyberScanAttributed::cMESSAGE_ID:
   {
      auto* msg = static_cast<MsgCyberScanAttributed*>(aMsg);
      idb->AddUnpairedInteraction(msg->ownerIndex(),
                                  msg->victimIndex(),
                                  msg->ownerIndex(),
                                  msg->GetMessageIndex(),
                                  aAtBegin,
                                  "CyberAttributed",
                                  msg->simTime(),
                                  msg->attackType());
      if (msg->ownerIndex() != msg->victimIndex())
      {
         idb->AddUnpairedInteraction(msg->victimIndex(),
                                     msg->victimIndex(),
                                     msg->ownerIndex(),
                                     msg->GetMessageIndex(),
                                     aAtBegin,
                                     "CyberAttributed",
                                     msg->simTime(),
                                     msg->attackType());
      }
   }
   break;
   case MsgCyberScanFailed::cMESSAGE_ID:
   {
      auto* msg = static_cast<MsgCyberScanFailed*>(aMsg);
      idb->AddPairedStopInteraction(msg->ownerIndex(),
                                    msg->ownerIndex(),
                                    msg->victimIndex(),
                                    msg->GetMessageIndex(),
                                    aAtBegin,
                                    "CyberScan",
                                    msg->simTime(),
                                    msg->attackType());
      if (msg->ownerIndex() != msg->victimIndex())
      {
         idb->AddPairedStopInteraction(msg->victimIndex(),
                                       msg->ownerIndex(),
                                       msg->victimIndex(),
                                       msg->GetMessageIndex(),
                                       aAtBegin,
                                       "CyberScan",
                                       msg->simTime(),
                                       msg->attackType());
      }
   }
   break;
   case MsgCyberScanSucceeded::cMESSAGE_ID:
   {
      auto* msg = static_cast<MsgCyberScanSucceeded*>(aMsg);
      idb->AddPairedStopInteraction(msg->ownerIndex(),
                                    msg->ownerIndex(),
                                    msg->victimIndex(),
                                    msg->GetMessageIndex(),
                                    aAtBegin,
                                    "CyberScan",
                                    msg->simTime(),
                                    msg->attackType());
      idb->AddUnpairedInteraction(msg->ownerIndex(),
                                  msg->ownerIndex(),
                                  msg->victimIndex(),
                                  msg->GetMessageIndex(),
                                  aAtBegin,
                                  "CyberCompromise",
                                  msg->simTime(),
                                  msg->attackType());
      if (msg->ownerIndex() != msg->victimIndex())
      {
         idb->AddPairedStopInteraction(msg->victimIndex(),
                                       msg->ownerIndex(),
                                       msg->victimIndex(),
                                       msg->GetMessageIndex(),
                                       aAtBegin,
                                       "CyberScan",
                                       msg->simTime(),
                                       msg->attackType());
         idb->AddUnpairedInteraction(msg->victimIndex(),
                                     msg->ownerIndex(),
                                     msg->victimIndex(),
                                     msg->GetMessageIndex(),
                                     aAtBegin,
                                     "CyberCompromise",
                                     msg->simTime(),
                                     msg->attackType());
      }
   }
   break;
   case MsgCyberAttackInitiated::cMESSAGE_ID:
   {
      auto* msg = static_cast<MsgCyberAttackInitiated*>(aMsg);
      idb->AddPairedStartInteraction(msg->ownerIndex(),
                                     msg->ownerIndex(),
                                     msg->victimIndex(),
                                     msg->GetMessageIndex(),
                                     aAtBegin,
                                     "CyberAttack",
                                     msg->simTime(),
                                     msg->attackType());
      if (msg->ownerIndex() != msg->victimIndex())
      {
         idb->AddPairedStartInteraction(msg->victimIndex(),
                                        msg->ownerIndex(),
                                        msg->victimIndex(),
                                        msg->GetMessageIndex(),
                                        aAtBegin,
                                        "CyberAttack",
                                        msg->simTime(),
                                        msg->attackType());
      }
   }
   break;
   case MsgCyberAttackFailed::cMESSAGE_ID:
   {
      auto* msg = static_cast<MsgCyberAttackFailed*>(aMsg);
      idb->AddPairedStopInteraction(msg->ownerIndex(),
                                    msg->ownerIndex(),
                                    msg->victimIndex(),
                                    msg->GetMessageIndex(),
                                    aAtBegin,
                                    "CyberAttack",
                                    msg->simTime(),
                                    msg->attackType());
      if (msg->ownerIndex() != msg->victimIndex())
      {
         idb->AddPairedStopInteraction(msg->victimIndex(),
                                       msg->ownerIndex(),
                                       msg->victimIndex(),
                                       msg->GetMessageIndex(),
                                       aAtBegin,
                                       "CyberAttack",
                                       msg->simTime(),
                                       msg->attackType());
      }
   }
   break;
   case MsgCyberAttackSucceeded::cMESSAGE_ID:
   {
      auto* msg = static_cast<MsgCyberAttackSucceeded*>(aMsg);
      idb->AddPairedStopInteraction(msg->ownerIndex(),
                                    msg->ownerIndex(),
                                    msg->victimIndex(),
                                    msg->GetMessageIndex(),
                                    aAtBegin,
                                    "CyberAttack",
                                    msg->simTime(),
                                    msg->attackType());
      idb->AddUnpairedInteraction(msg->ownerIndex(),
                                  msg->ownerIndex(),
                                  msg->victimIndex(),
                                  msg->GetMessageIndex(),
                                  aAtBegin,
                                  "CyberDamage",
                                  msg->simTime(),
                                  msg->attackType());
      if (msg->ownerIndex() != msg->victimIndex())
      {
         idb->AddPairedStopInteraction(msg->victimIndex(),
                                       msg->ownerIndex(),
                                       msg->victimIndex(),
                                       msg->GetMessageIndex(),
                                       aAtBegin,
                                       "CyberAttack",
                                       msg->simTime(),
                                       msg->attackType());
         idb->AddUnpairedInteraction(msg->victimIndex(),
                                     msg->ownerIndex(),
                                     msg->victimIndex(),
                                     msg->GetMessageIndex(),
                                     aAtBegin,
                                     "CyberDamage",
                                     msg->simTime(),
                                     msg->attackType());
      }
   }
   break;
   case MsgCyberAttackDetected::cMESSAGE_ID:
   {
      auto* msg = static_cast<MsgCyberAttackDetected*>(aMsg);
      idb->AddUnpairedInteraction(msg->ownerIndex(),
                                  msg->ownerIndex(),
                                  msg->victimIndex(),
                                  msg->GetMessageIndex(),
                                  aAtBegin,
                                  "CyberDetect",
                                  msg->simTime(),
                                  msg->attackType());
      if (msg->ownerIndex() != msg->victimIndex())
      {
         idb->AddUnpairedInteraction(msg->victimIndex(),
                                     msg->ownerIndex(),
                                     msg->victimIndex(),
                                     msg->GetMessageIndex(),
                                     aAtBegin,
                                     "CyberDetect",
                                     msg->simTime(),
                                     msg->attackType());
      }
   }
   break;
   case MsgCyberAttackAttributed::cMESSAGE_ID:
   {
      auto* msg = static_cast<MsgCyberAttackAttributed*>(aMsg);
      idb->AddUnpairedInteraction(msg->ownerIndex(),
                                  msg->victimIndex(),
                                  msg->ownerIndex(),
                                  msg->GetMessageIndex(),
                                  aAtBegin,
                                  "CyberAttributed",
                                  msg->simTime(),
                                  msg->attackType());
      if (msg->ownerIndex() != msg->victimIndex())
      {
         idb->AddUnpairedInteraction(msg->victimIndex(),
                                     msg->victimIndex(),
                                     msg->ownerIndex(),
                                     msg->GetMessageIndex(),
                                     aAtBegin,
                                     "CyberAttributed",
                                     msg->simTime(),
                                     msg->attackType());
      }
   }
   break;
   case MsgCyberAttackRecovery::cMESSAGE_ID:
   {
      auto* msg = static_cast<MsgCyberAttackRecovery*>(aMsg);
      idb->AddUnpairedInteraction(msg->ownerIndex(),
                                  msg->victimIndex(),
                                  msg->ownerIndex(),
                                  msg->GetMessageIndex(),
                                  aAtBegin,
                                  "CyberImmune",
                                  msg->simTime(),
                                  msg->attackType());
      if (msg->ownerIndex() != msg->victimIndex())
      {
         idb->AddUnpairedInteraction(msg->victimIndex(),
                                     msg->victimIndex(),
                                     msg->ownerIndex(),
                                     msg->GetMessageIndex(),
                                     aAtBegin,
                                     "CyberImmune",
                                     msg->simTime(),
                                     msg->attackType());
      }
   }
   break;
   }
   aResultDb.UnlockInteractionDb();
}


void rv::CyberDataExtension::RemoveMessage(ResultDb& aResultDb, MsgBase* aMsg, bool aAtBegin) const
{
   InteractionDb* idb = aResultDb.LockInteractionDb();
   switch (aMsg->GetMessageId())
   {
   case MsgCyberScanInitiated::cMESSAGE_ID:
   {
      auto* msg = static_cast<MsgCyberScanInitiated*>(aMsg);
      idb->RemovePairedInteraction(msg->ownerIndex(), msg->victimIndex(), msg->GetMessageIndex(), aAtBegin);
   }
   break;
   case MsgCyberScanDetected::cMESSAGE_ID:
   {
      auto* msg = static_cast<MsgCyberScanDetected*>(aMsg);
      idb->RemoveUnpairedInteraction(msg->ownerIndex(), msg->victimIndex(), msg->GetMessageIndex(), aAtBegin);
   }
   break;
   case MsgCyberScanAttributed::cMESSAGE_ID:
   {
      auto* msg = static_cast<MsgCyberScanAttributed*>(aMsg);
      idb->RemoveUnpairedInteraction(msg->ownerIndex(), msg->victimIndex(), msg->GetMessageIndex(), aAtBegin);
   }
   break;
   case MsgCyberScanFailed::cMESSAGE_ID:
   {
      auto* msg = static_cast<MsgCyberScanFailed*>(aMsg);
      idb->RemovePairedInteraction(msg->ownerIndex(), msg->victimIndex(), msg->GetMessageIndex(), aAtBegin);
   }
   break;
   case MsgCyberScanSucceeded::cMESSAGE_ID:
   {
      auto* msg = static_cast<MsgCyberScanSucceeded*>(aMsg);
      idb->RemovePairedInteraction(msg->ownerIndex(), msg->victimIndex(), msg->GetMessageIndex(), aAtBegin);
      idb->RemoveUnpairedInteraction(msg->ownerIndex(), msg->victimIndex(), msg->GetMessageIndex(), aAtBegin);
   }
   break;
   case MsgCyberAttackInitiated::cMESSAGE_ID:
   {
      auto* msg = static_cast<MsgCyberAttackInitiated*>(aMsg);
      idb->RemovePairedInteraction(msg->ownerIndex(), msg->victimIndex(), msg->GetMessageIndex(), aAtBegin);
   }
   break;
   case MsgCyberAttackFailed::cMESSAGE_ID:
   {
      auto* msg = static_cast<MsgCyberAttackFailed*>(aMsg);
      idb->RemovePairedInteraction(msg->ownerIndex(), msg->victimIndex(), msg->GetMessageIndex(), aAtBegin);
   }
   break;
   case MsgCyberAttackSucceeded::cMESSAGE_ID:
   {
      auto* msg = static_cast<MsgCyberAttackSucceeded*>(aMsg);
      idb->RemovePairedInteraction(msg->ownerIndex(), msg->victimIndex(), msg->GetMessageIndex(), aAtBegin);
      idb->RemoveUnpairedInteraction(msg->ownerIndex(), msg->victimIndex(), msg->GetMessageIndex(), aAtBegin);
   }
   break;
   case MsgCyberAttackDetected::cMESSAGE_ID:
   {
      auto* msg = static_cast<MsgCyberAttackDetected*>(aMsg);
      idb->RemoveUnpairedInteraction(msg->ownerIndex(), msg->victimIndex(), msg->GetMessageIndex(), aAtBegin);
   }
   break;
   case MsgCyberAttackAttributed::cMESSAGE_ID:
   {
      auto* msg = static_cast<MsgCyberAttackAttributed*>(aMsg);
      idb->RemoveUnpairedInteraction(msg->ownerIndex(), msg->victimIndex(), msg->GetMessageIndex(), aAtBegin);
   }
   break;
   case MsgCyberAttackRecovery::cMESSAGE_ID:
   {
      auto* msg = static_cast<MsgCyberAttackRecovery*>(aMsg);
      if (msg->immunityStatus() > 0)
      {
         idb->RemoveUnpairedInteraction(msg->ownerIndex(), msg->victimIndex(), msg->GetMessageIndex(), aAtBegin);
      }
   }
   break;
   }
   aResultDb.UnlockInteractionDb();
}

QVariant rv::CyberDataExtension::HandleCyberAttackInitiated(const MsgCyberAttackInitiated* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "owner";
      }
      return rvEnv.LookupPlatformVariant(aMsg->ownerIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_TARGET):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "victim";
      }
      return rvEnv.LookupPlatformVariant(aMsg->victimIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "attack type";
      }
      return QString::fromStdString(aMsg->attackType());
   }
   }
   return QVariant();
}

QVariant rv::CyberDataExtension::HandleCyberAttackSucceeded(const MsgCyberAttackSucceeded* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "owner";
      }
      return rvEnv.LookupPlatformVariant(aMsg->ownerIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_TARGET):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "victim";
      }
      return rvEnv.LookupPlatformVariant(aMsg->victimIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "attack type";
      }
      return QString::fromStdString(aMsg->attackType());
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "success odds";
      }
      return QString::number(aMsg->successOdds().draw()) + "/" + QString::number(aMsg->successOdds().threshold());
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "report status";
      }
      return QString::fromStdString(aMsg->reportStatus());
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "report odds";
      }
      return QString::number(aMsg->reportOdds().draw()) + "/" + QString::number(aMsg->reportOdds().threshold());
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "detect odds";
      }
      return QString::number(aMsg->detectOdds().draw()) + "/" + QString::number(aMsg->detectOdds().threshold());
   }
   }
   return QVariant();
}

QVariant rv::CyberDataExtension::HandleCyberAttackFailed(const MsgCyberAttackFailed* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "owner";
      }
      return rvEnv.LookupPlatformVariant(aMsg->ownerIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_TARGET):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "victim";
      }
      return rvEnv.LookupPlatformVariant(aMsg->victimIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "attack type";
      }
      return QString::fromStdString(aMsg->attackType());
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "success odds";
      }
      return QString::number(aMsg->successOdds().draw()) + "/" + QString::number(aMsg->successOdds().threshold());
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "report status";
      }
      return QString::fromStdString(aMsg->reportStatus());
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "report odds";
      }
      return QString::number(aMsg->reportOdds().draw()) + "/" + QString::number(aMsg->reportOdds().threshold());
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "reason";
      }
      return rv::AttackFailReason::ToString(aMsg->reason());
   }
   }
   return QVariant();
}

QVariant rv::CyberDataExtension::HandleCyberAttackDetected(const MsgCyberAttackDetected* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "owner";
      }
      return rvEnv.LookupPlatformVariant(aMsg->ownerIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_TARGET):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "victim";
      }
      return rvEnv.LookupPlatformVariant(aMsg->victimIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "attack type";
      }
      return QString::fromStdString(aMsg->attackType());
   }
   }
   return QVariant();
}

QVariant rv::CyberDataExtension::HandleCyberAttackAttributed(const MsgCyberAttackAttributed* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "owner";
      }
      return rvEnv.LookupPlatformVariant(aMsg->ownerIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_TARGET):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "victim";
      }
      return rvEnv.LookupPlatformVariant(aMsg->victimIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "attack type";
      }
      return QString::fromStdString(aMsg->attackType());
   }
   }
   return QVariant();
}

QVariant rv::CyberDataExtension::HandleCyberAttackRecovery(const MsgCyberAttackRecovery* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "owner";
      }
      return rvEnv.LookupPlatformVariant(aMsg->ownerIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_TARGET):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "victim";
      }
      return rvEnv.LookupPlatformVariant(aMsg->victimIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "attack type";
      }
      return QString::fromStdString(aMsg->attackType());
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "immunity status";
      }
      return aMsg->immunityStatus();
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "immunity odds";
      }
      return QString::number(aMsg->immunityOdds().draw()) + "/" + QString::number(aMsg->immunityOdds().threshold());
   }
   }
   return QVariant();
}

QVariant rv::CyberDataExtension::HandleCyberScanInitiated(const MsgCyberScanInitiated* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "owner";
      }
      return rvEnv.LookupPlatformVariant(aMsg->ownerIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_TARGET):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "victim";
      }
      return rvEnv.LookupPlatformVariant(aMsg->victimIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "attack type";
      }
      return QString::fromStdString(aMsg->attackType());
   }
   }
   return QVariant();
}

QVariant rv::CyberDataExtension::HandleCyberScanSucceeded(const MsgCyberScanSucceeded* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "owner";
      }
      return rvEnv.LookupPlatformVariant(aMsg->ownerIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_TARGET):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "victim";
      }
      return rvEnv.LookupPlatformVariant(aMsg->victimIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "attack type";
      }
      return QString::fromStdString(aMsg->attackType());
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "detection odds";
      }
      return QString::number(aMsg->detectOdds().draw()) + "/" + QString::number(aMsg->detectOdds().threshold());
   }
   }
   return QVariant();
}

QVariant rv::CyberDataExtension::HandleCyberScanFailed(const MsgCyberScanFailed* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "owner";
      }
      return rvEnv.LookupPlatformVariant(aMsg->ownerIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_TARGET):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "victim";
      }
      return rvEnv.LookupPlatformVariant(aMsg->victimIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "attack type";
      }
      return QString::fromStdString(aMsg->attackType());
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "detection odds";
      }
      return QString::number(aMsg->detectOdds().draw()) + "/" + QString::number(aMsg->detectOdds().threshold());
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "reason";
      }
      return ScanFailReason::ToString(aMsg->reason());
   }
   }
   return QVariant();
}

QVariant rv::CyberDataExtension::HandleCyberScanDetected(const MsgCyberScanDetected* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "owner";
      }
      return rvEnv.LookupPlatformVariant(aMsg->ownerIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_TARGET):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "victim";
      }
      return rvEnv.LookupPlatformVariant(aMsg->victimIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "attack type";
      }
      return QString::fromStdString(aMsg->attackType());
   }
   }
   return QVariant();
}

QVariant rv::CyberDataExtension::HandleCyberScanAttributed(const MsgCyberScanAttributed* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "owner";
      }
      return rvEnv.LookupPlatformVariant(aMsg->ownerIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_TARGET):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "victim";
      }
      return rvEnv.LookupPlatformVariant(aMsg->victimIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "attack type";
      }
      return QString::fromStdString(aMsg->attackType());
   }
   }
   return QVariant();
}


std::vector<std::unique_ptr<rv::DataExtension::StateCard>> rv::CyberDataExtension::GetStateCardList() const
{
   std::vector<std::unique_ptr<rv::DataExtension::StateCard>> list;
   list.emplace_back(ut::make_unique<rv::DataExtension::StateCard>("CyberDetect", ":/icons/detect.png", "Cyber"));
   list.emplace_back(ut::make_unique<rv::DataExtension::StateCard>("CyberImmune", ":/icons/immune.png", "Cyber"));
   list.emplace_back(ut::make_unique<rv::DataExtension::StateCard>("CyberCompromise", ":/icons/vulnerable.png", "Cyber"));
   list.emplace_back(ut::make_unique<rv::DataExtension::StateCard>("CyberDamage", ":/icons/damage.png", "Cyber"));
   return list;
}

std::vector<std::unique_ptr<rv::DataExtension::Interaction>> rv::CyberDataExtension::GetInteractionList() const
{
   std::vector<std::unique_ptr<rv::DataExtension::Interaction>> list;
   list.emplace_back(
      ut::make_unique<rv::DataExtension::Interaction>("CyberAttributed", "Cyber", "cyber-attributed", QColor(0, 255, 255)));
   list.emplace_back(
      ut::make_unique<rv::DataExtension::Interaction>("CyberScan", "Cyber", "cyber-scans", QColor(0, 255, 192)));
   list.emplace_back(
      ut::make_unique<rv::DataExtension::Interaction>("CyberAttack", "Cyber", "cyber-attacks", QColor(255, 143, 143)));
   return list;
}
