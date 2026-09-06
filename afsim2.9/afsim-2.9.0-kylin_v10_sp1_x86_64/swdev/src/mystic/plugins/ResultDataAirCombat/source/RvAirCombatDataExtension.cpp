// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "RvAirCombatDataExtension.hpp"

#include "RvAirCombatEventPipeClasses.hpp"
#include "RvAirCombatEventPipeClassesRegister.hpp"
#include "RvAirCombatEventPipeSchema.hpp"
#include "RvEnvironment.hpp"
#include "RvResultDb.hpp"

bool rv::AirCombatDataExtension::HandlesMessage(int aMessageId) const
{
   switch (aMessageId)
   {
   case (MsgSA_EngagementSummaryData::cMESSAGE_ID):
   case (MsgSA_FlightKinematicsData::cMESSAGE_ID):
   case (MsgSA_FuelData::cMESSAGE_ID):
   case (MsgSA_NavData::cMESSAGE_ID):
   case (MsgSA_FlightControlsData::cMESSAGE_ID):
   case (MsgSA_WeaponsData::cMESSAGE_ID):
   case (MsgSA_TrackData::cMESSAGE_ID):
   case (MsgSA_PerceivedAssetsData::cMESSAGE_ID):
   case (MsgSA_PerceivedBogiesAndBanditsData::cMESSAGE_ID):
   case (MsgSA_GroupsData::cMESSAGE_ID):
   case (MsgSA_PrioritizedThreatsAndTargetsData::cMESSAGE_ID):
      return true;
   default:
      return false;
   }
}

QVariant rv::AirCombatDataExtension::HandleMessage(const MsgBase* aMsg, int aColumn, int aRole) const
{
   switch (aMsg->GetMessageId())
   {
   case (MsgSA_EngagementSummaryData::cMESSAGE_ID):
      HandleSA_EngagementSummaryData(static_cast<const MsgSA_EngagementSummaryData*>(aMsg), aColumn, aRole);
      break;

   case (MsgSA_FlightKinematicsData::cMESSAGE_ID):
      HandleSA_FlightKinematicsData(static_cast<const MsgSA_FlightKinematicsData*>(aMsg), aColumn, aRole);
      break;

   case (MsgSA_FuelData::cMESSAGE_ID):
      HandleSA_FuelData(static_cast<const MsgSA_FuelData*>(aMsg), aColumn, aRole);
      break;

   case (MsgSA_NavData::cMESSAGE_ID):
      HandleSA_NavData(static_cast<const MsgSA_NavData*>(aMsg), aColumn, aRole);
      break;

   case (MsgSA_FlightControlsData::cMESSAGE_ID):
      HandleSA_FlightControlsData(static_cast<const MsgSA_FlightControlsData*>(aMsg), aColumn, aRole);
      break;

   case (MsgSA_WeaponsData::cMESSAGE_ID):
      HandleSA_WeaponsData(static_cast<const MsgSA_WeaponsData*>(aMsg), aColumn, aRole);
      break;

   case (MsgSA_TrackData::cMESSAGE_ID):
      HandleSA_TrackData(static_cast<const MsgSA_TrackData*>(aMsg), aColumn, aRole);
      break;

   case (MsgSA_PerceivedAssetsData::cMESSAGE_ID):
      HandleSA_PerceivedAssetsData(static_cast<const MsgSA_PerceivedAssetsData*>(aMsg), aColumn, aRole);
      break;

   case (MsgSA_PerceivedBogiesAndBanditsData::cMESSAGE_ID):
      HandleSA_PerceivedBogiesAndBanditsData(static_cast<const MsgSA_PerceivedBogiesAndBanditsData*>(aMsg), aColumn, aRole);
      break;

   case (MsgSA_GroupsData::cMESSAGE_ID):
      HandleSA_GroupsData(static_cast<const MsgSA_GroupsData*>(aMsg), aColumn, aRole);
      break;

   case (MsgSA_PrioritizedThreatsAndTargetsData::cMESSAGE_ID):
      HandleSA_PrioritizedThreatsAndTargetsData(static_cast<const MsgSA_PrioritizedThreatsAndTargetsData*>(aMsg),
                                                aColumn,
                                                aRole);
      break;

   default:
      return QVariant();
   }

   return QVariant();
}

void rv::AirCombatDataExtension::RegisterSerializer(UtPackSerializer& aSerializer) const
{
   UtPack_register_all_wsf_air_combat_events_types(aSerializer);
}

const char* rv::AirCombatDataExtension::GetSchema() const
{
   return cRvAirCombatEventPipe_SCHEMA;
}

bool rv::AirCombatDataExtension::IsOneTimeMessage(int aMessageId) const
{
   return false;
}

void rv::AirCombatDataExtension::AddMessage(ResultDb& aResultDb, MsgBase* aMsg, bool aAtBegin) const
{
   switch (aMsg->GetMessageId())
   {
   case MsgSA_EngagementSummaryData::cMESSAGE_ID:
   {
      auto            msgPtr  = static_cast<MsgSA_EngagementSummaryData*>(aMsg);
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
      break;
   }

   case MsgSA_FlightKinematicsData::cMESSAGE_ID:
   {
      auto            msgPtr  = static_cast<MsgSA_FlightKinematicsData*>(aMsg);
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
      break;
   }

   case MsgSA_FuelData::cMESSAGE_ID:
   {
      auto            msgPtr  = static_cast<MsgSA_FuelData*>(aMsg);
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
      break;
   }

   case MsgSA_NavData::cMESSAGE_ID:
   {
      auto            msgPtr  = static_cast<MsgSA_NavData*>(aMsg);
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
      break;
   }

   case MsgSA_FlightControlsData::cMESSAGE_ID:
   {
      auto            msgPtr  = static_cast<MsgSA_FlightControlsData*>(aMsg);
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
      break;
   }

   case MsgSA_WeaponsData::cMESSAGE_ID:
   {
      auto            msgPtr  = static_cast<MsgSA_WeaponsData*>(aMsg);
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
      break;
   }

   case MsgSA_TrackData::cMESSAGE_ID:
   {
      auto            msgPtr  = static_cast<MsgSA_TrackData*>(aMsg);
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
      break;
   }

   case MsgSA_PerceivedAssetsData::cMESSAGE_ID:
   {
      auto            msgPtr  = static_cast<MsgSA_PerceivedAssetsData*>(aMsg);
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
      break;
   }

   case MsgSA_PerceivedBogiesAndBanditsData::cMESSAGE_ID:
   {
      auto            msgPtr  = static_cast<MsgSA_PerceivedBogiesAndBanditsData*>(aMsg);
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
      break;
   }

   case MsgSA_GroupsData::cMESSAGE_ID:
   {
      auto            msgPtr  = static_cast<MsgSA_GroupsData*>(aMsg);
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
      break;
   }

   case MsgSA_PrioritizedThreatsAndTargetsData::cMESSAGE_ID:
   {
      auto            msgPtr  = static_cast<MsgSA_PrioritizedThreatsAndTargetsData*>(aMsg);
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
      break;
   }

   default:
      break;
   }
}

void rv::AirCombatDataExtension::RemoveMessage(ResultDb& aResultDb, MsgBase* aMsg, bool aAtBegin) const
{
   switch (aMsg->GetMessageId())
   {
   case MsgSA_EngagementSummaryData::cMESSAGE_ID:
   {
      auto            msgPtr  = static_cast<MsgSA_EngagementSummaryData*>(aMsg);
      ResultPlatform* platPtr = aResultDb.FindPlatform(msgPtr->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->RemoveBaseMessage(msgPtr, aAtBegin);
         platPtr->EndWrite();
      }
      break;
   }

   case MsgSA_FlightKinematicsData::cMESSAGE_ID:
   {
      auto            msgPtr  = static_cast<MsgSA_FlightKinematicsData*>(aMsg);
      ResultPlatform* platPtr = aResultDb.FindPlatform(msgPtr->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->RemoveBaseMessage(msgPtr, aAtBegin);
         platPtr->EndWrite();
      }
      break;
   }

   case MsgSA_FuelData::cMESSAGE_ID:
   {
      auto            msgPtr  = static_cast<MsgSA_FuelData*>(aMsg);
      ResultPlatform* platPtr = aResultDb.FindPlatform(msgPtr->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->RemoveBaseMessage(msgPtr, aAtBegin);
         platPtr->EndWrite();
      }
      break;
   }

   case MsgSA_NavData::cMESSAGE_ID:
   {
      auto            msgPtr  = static_cast<MsgSA_NavData*>(aMsg);
      ResultPlatform* platPtr = aResultDb.FindPlatform(msgPtr->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->RemoveBaseMessage(msgPtr, aAtBegin);
         platPtr->EndWrite();
      }
      break;
   }

   case MsgSA_FlightControlsData::cMESSAGE_ID:
   {
      auto            msgPtr  = static_cast<MsgSA_FlightControlsData*>(aMsg);
      ResultPlatform* platPtr = aResultDb.FindPlatform(msgPtr->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->RemoveBaseMessage(msgPtr, aAtBegin);
         platPtr->EndWrite();
      }
      break;
   }

   case MsgSA_WeaponsData::cMESSAGE_ID:
   {
      auto            msgPtr  = static_cast<MsgSA_WeaponsData*>(aMsg);
      ResultPlatform* platPtr = aResultDb.FindPlatform(msgPtr->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->RemoveBaseMessage(msgPtr, aAtBegin);
         platPtr->EndWrite();
      }
      break;
   }

   case MsgSA_TrackData::cMESSAGE_ID:
   {
      auto            msgPtr  = static_cast<MsgSA_TrackData*>(aMsg);
      ResultPlatform* platPtr = aResultDb.FindPlatform(msgPtr->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->RemoveBaseMessage(msgPtr, aAtBegin);
         platPtr->EndWrite();
      }
      break;
   }

   case MsgSA_PerceivedAssetsData::cMESSAGE_ID:
   {
      auto            msgPtr  = static_cast<MsgSA_PerceivedAssetsData*>(aMsg);
      ResultPlatform* platPtr = aResultDb.FindPlatform(msgPtr->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->RemoveBaseMessage(msgPtr, aAtBegin);
         platPtr->EndWrite();
      }
      break;
   }

   case MsgSA_PerceivedBogiesAndBanditsData::cMESSAGE_ID:
   {
      auto            msgPtr  = static_cast<MsgSA_PerceivedBogiesAndBanditsData*>(aMsg);
      ResultPlatform* platPtr = aResultDb.FindPlatform(msgPtr->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->RemoveBaseMessage(msgPtr, aAtBegin);
         platPtr->EndWrite();
      }
      break;
   }

   case MsgSA_GroupsData::cMESSAGE_ID:
   {
      auto            msgPtr  = static_cast<MsgSA_GroupsData*>(aMsg);
      ResultPlatform* platPtr = aResultDb.FindPlatform(msgPtr->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->RemoveBaseMessage(msgPtr, aAtBegin);
         platPtr->EndWrite();
      }
      break;
   }

   case MsgSA_PrioritizedThreatsAndTargetsData::cMESSAGE_ID:
   {
      auto            msgPtr  = static_cast<MsgSA_PrioritizedThreatsAndTargetsData*>(aMsg);
      ResultPlatform* platPtr = aResultDb.FindPlatform(msgPtr->platformIndex());
      if (platPtr)
      {
         platPtr->BeginWrite();
         platPtr->RemoveBaseMessage(msgPtr, aAtBegin);
         platPtr->EndWrite();
      }
      break;
   }

   default:
      break;
   }
}

QVariant rv::AirCombatDataExtension::HandleSA_EngagementSummaryData(const MsgSA_EngagementSummaryData* aMsg,
                                                                    int                                aColumn,
                                                                    int                                aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "owner";
      }
      return rvEnv.LookupPlatformVariant(aMsg->platformIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "radar emitting";
      }
      return aMsg->radarEmitting();
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "jammer emitting";
      }
      return aMsg->jammerEmitting();
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "other emitting";
      }
      return aMsg->otherEmitting();
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "afterburner";
      }
      return aMsg->afterburner();
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "contrails";
      }
      return aMsg->contrails();
   }
   case (rv::MessageBase::cFIELD_DATA + 5):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "mission task";
      }
      return QString::fromStdString(aMsg->missionTask());
   }
   case (rv::MessageBase::cFIELD_DATA + 6):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "risk acceptable";
      }
      return aMsg->riskAcceptable();
   }
   case (rv::MessageBase::cFIELD_DATA + 7):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "risk current";
      }
      return aMsg->riskCurrent();
   }
   case (rv::MessageBase::cFIELD_DATA + 8):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "defensiveness";
      }
      return aMsg->defensiveness();
   }
   case (rv::MessageBase::cFIELD_DATA + 9):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "urgency";
      }
      return aMsg->urgency();
   }
   case (rv::MessageBase::cFIELD_DATA + 10):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "self risk";
      }
      return aMsg->selfRisk();
   }
   case (rv::MessageBase::cFIELD_DATA + 11):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "flight risk";
      }
      return aMsg->flightRisk();
   }
   case (rv::MessageBase::cFIELD_DATA + 12):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "package risk";
      }
      return aMsg->packageRisk();
   }
   case (rv::MessageBase::cFIELD_DATA + 13):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "mission risk";
      }
      return aMsg->missionRisk();
   }
   default:
      return QVariant();
   }
}

QVariant rv::AirCombatDataExtension::HandleSA_FlightKinematicsData(const MsgSA_FlightKinematicsData* aMsg,
                                                                   int                               aColumn,
                                                                   int                               aRole) const
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

QVariant rv::AirCombatDataExtension::HandleSA_FuelData(const MsgSA_FuelData* aMsg, int aColumn, int aRole) const
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

QVariant rv::AirCombatDataExtension::HandleSA_NavData(const MsgSA_NavData* aMsg, int aColumn, int aRole) const
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

QVariant rv::AirCombatDataExtension::HandleSA_FlightControlsData(const MsgSA_FlightControlsData* aMsg,
                                                                 int                             aColumn,
                                                                 int                             aRole) const
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

QVariant rv::AirCombatDataExtension::HandleSA_WeaponsData(const MsgSA_WeaponsData* aMsg, int aColumn, int aRole) const
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

QVariant rv::AirCombatDataExtension::HandleSA_TrackData(const MsgSA_TrackData* aMsg, int aColumn, int aRole) const
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

QVariant rv::AirCombatDataExtension::HandleSA_PerceivedAssetsData(const MsgSA_PerceivedAssetsData* aMsg,
                                                                  int                              aColumn,
                                                                  int                              aRole) const
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

QVariant rv::AirCombatDataExtension::HandleSA_PerceivedBogiesAndBanditsData(const MsgSA_PerceivedBogiesAndBanditsData* aMsg,
                                                                            int aColumn,
                                                                            int aRole) const
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

QVariant rv::AirCombatDataExtension::HandleSA_GroupsData(const MsgSA_GroupsData* aMsg, int aColumn, int aRole) const
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

QVariant rv::AirCombatDataExtension::HandleSA_PrioritizedThreatsAndTargetsData(const MsgSA_PrioritizedThreatsAndTargetsData* aMsg,
                                                                               int aColumn,
                                                                               int aRole) const
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
