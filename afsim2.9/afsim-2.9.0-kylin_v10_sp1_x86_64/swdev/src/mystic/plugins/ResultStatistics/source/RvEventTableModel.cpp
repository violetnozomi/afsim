// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "RvEventTableModel.hpp"

#include <bitset>

#include "RvEnvironment.hpp"
#include "RvEventPipeClasses.hpp"
#include "RvEventPipeUtil.hpp"
#include "RvMilEventPipeClasses.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"

RvStatistics::EventTableModel::EventTableModel(QObject* aParent)
   : QAbstractTableModel(aParent)
{
}

void RvStatistics::EventTableModel::SetData(rv::MessageBaseArray& aPagedData,
                                            rv::MessageBaseArray& aMaintainedData,
                                            rv::ResultDb*         aDbPtr)
{
   mData.clear();
   mDbPtr                                  = aDbPtr;
   rv::MessageBaseArray::const_iterator it = aPagedData.begin();
   rv::MessageBaseArray::const_iterator jt = aMaintainedData.begin();

   while (it != aPagedData.end())
   {
      if (jt != aMaintainedData.end())
      {
         if ((*jt)->simTime() <= (*it)->simTime())
         {
            mData.push_back(*jt);
            ++jt;
         }
         else
         {
            mData.push_back(*it);
            ++it;
         }
      }
      else
      {
         mData.push_back(*it);
         ++it;
      }
   }
   while (jt != aMaintainedData.end())
   {
      mData.push_back(*jt);
      ++jt;
   }

   mSimNames.clear();
   for (const auto& item : mData)
   {
      if (item->GetMessageId() == rv::MsgExecData::cMESSAGE_ID)
      {
         // The priority for data in the simulation column is:
         // 1) The given (non-empty) simulation_name.
         // 2) The DIS site and application.
         // 3) The string literal "local".

         const auto* execData = static_cast<rv::MsgExecData*>(item);
         QString     name     = QString::fromStdString(execData->simName());
         if (name.isEmpty())
         {
            if (execData->disSiteValid() &&
                execData->disApplicationValid()) // If the DIS site/app is not valid, the sim must be local.
            {
               name = QString::number(execData->disSite()) + ":" + QString::number(execData->disApplication());
            }
            else
            {
               name = "local";
            }
         }

         mSimNames[item->simIndex()] = name;
      }
   }
}

void RvStatistics::EventTableModel::Clear()
{
   mData.clear();
   mDbPtr = nullptr;
}

int RvStatistics::EventTableModel::rowCount(const QModelIndex& aIndex) const
{
   return (int)mData.size();
}

int RvStatistics::EventTableModel::columnCount(const QModelIndex& aIndex) const
{
   return rv::MessageBase::cFIELD_DATA + 30;
}

QString RvStatistics::EventTableModel::LookupSimName(int aIndex) const
{
   auto it = mSimNames.find(aIndex);

   if (it != mSimNames.end())
   {
      return it->second;
   }

   return "external";
}

QVariant RvStatistics::EventTableModel::HandlePartType(const unsigned int aEnum) const
{
   QString partType = rv::PartType::ToString(aEnum);
   if (partType.isEmpty())
   {
      partType = rv::MilPartType::ToString(aEnum);
   }
   return partType;
}

QVariant RvStatistics::EventTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role == Qt::DisplayRole)
   {
      if (orientation == Qt::Horizontal)
      {
         switch (section)
         {
         case (rv::MessageBase::cFIELD_TIME):
            return "time";
         case (rv::MessageBase::cFIELD_TYPE):
            return "type";
         case (rv::MessageBase::cFIELD_OWNER):
            return "platform";
         case (rv::MessageBase::cFIELD_TARGET):
            return "interactor";
         case (rv::MessageBase::cFIELD_TRACK_ID):
            return "track id";
         case (rv::MessageBase::cFIELD_COMPONENT):
            return "component";
         case (rv::MessageBase::cFIELD_SIM):
            return "simulation";
         }
      }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::data(const QModelIndex& aIndex, int aRole) const
{
   if (aIndex.isValid() && aIndex.row() < static_cast<int>(mData.size()))
   {
      if ((aRole == Qt::DisplayRole) || (aRole == Qt::ToolTipRole) || (aRole == Qt::UserRole))
      {
         const int c = aIndex.column();

         const rv::MsgBase* da = mData[aIndex.row()];

         if (c == rv::MessageBase::cFIELD_TIME)
         {
            if (aRole == Qt::ToolTipRole)
            {
               return "time";
            }
            return da->simTime();
         }
         else if (c == rv::MessageBase::cFIELD_TYPE)
         {
            if (aRole == Qt::ToolTipRole)
            {
               return "type";
            }
            return da->GetMessageName();
         }
         else if (c == rv::MessageBase::cFIELD_SIM)
         {
            if (aRole == Qt::ToolTipRole)
            {
               return "simulation";
            }

            return LookupSimName(da->simIndex());
         }
         else
         {
            switch (da->GetMessageId())
            {
            case (rv::MsgAuxData::cMESSAGE_ID):
            {
               return HandleAuxData((rv::MsgAuxData*)da, c, aRole);
            }
            case (rv::MsgEntityState::cMESSAGE_ID):
            {
               return HandleEntityState((rv::MsgEntityState*)da, c, aRole);
            }
            case (rv::MsgPlatformInfo::cMESSAGE_ID):
            {
               return HandlePlatformInfo((rv::MsgPlatformInfo*)da, c, aRole);
            }
            case (rv::MsgLocalTrackCreated::cMESSAGE_ID):
            {
               return HandleLocalTrackCreated((rv::MsgLocalTrackCreated*)da, c, aRole);
            }
            case (rv::MsgLocalTrackUpdate::cMESSAGE_ID):
            {
               return HandleLocalTrackUpdate((rv::MsgLocalTrackUpdate*)da, c, aRole);
            }
            case (rv::MsgLocalTrackDrop::cMESSAGE_ID):
            {
               return HandleLocalTrackDrop((rv::MsgLocalTrackDrop*)da, c, aRole);
            }
            case (rv::MsgLocalTrackCorrelation::cMESSAGE_ID):
            {
               return HandleLocalTrackCorrelation((rv::MsgLocalTrackCorrelation*)da, c, aRole);
            }
            case (rv::MsgLocalTrackDecorrelation::cMESSAGE_ID):
            {
               return HandleLocalTrackDecorrelation((rv::MsgLocalTrackDecorrelation*)da, c, aRole);
            }
            case (rv::MsgSensorModeChange::cMESSAGE_ID):
            {
               return HandleSensorModeChange((rv::MsgSensorModeChange*)da, c, aRole);
            }
            case (rv::MsgPlatformStatus::cMESSAGE_ID):
            {
               return HandlePlatformStatus((rv::MsgPlatformStatus*)da, c, aRole);
            }
            case (rv::MsgPlatformAppearanceChanged::cMESSAGE_ID):
            {
               return HandlePlatformAppearanceChanged((rv::MsgPlatformAppearanceChanged*)da, c, aRole);
            }
            case (rv::MsgPartStatus::cMESSAGE_ID):
            {
               return HandlePartStatus((rv::MsgPartStatus*)da, c, aRole);
            }
            case (rv::MsgComment::cMESSAGE_ID):
            {
               return HandleComment((rv::MsgComment*)da, c, aRole);
            }
            case (rv::MsgDetectAttempt::cMESSAGE_ID):
            {
               return HandleDetectAttempt((rv::MsgDetectAttempt*)da, c, aRole);
            }
            case (rv::MsgDrawCommand::cMESSAGE_ID):
            {
               return HandleDrawCommand((rv::MsgDrawCommand*)da, c, aRole);
            }
            case (rv::MsgMessageReceived::cMESSAGE_ID):
            {
               return HandleMessageReceived((rv::MsgMessageReceived*)da, c, aRole);
            }
            case (rv::MsgMessageTransmitted::cMESSAGE_ID):
            {
               return HandleMessageTransmitted((rv::MsgMessageTransmitted*)da, c, aRole);
            }
            case (rv::MsgMessageHop::cMESSAGE_ID):
            {
               return HandleMessageHop(static_cast<const rv::MsgMessageHop*>(da), c, aRole);
            }
            case (rv::MsgScriptData::cMESSAGE_ID):
            {
               return HandleScriptData((rv::MsgScriptData*)da, c, aRole);
            }
            case (rv::MsgSensorDetectionChange::cMESSAGE_ID):
            {
               return HandleSensorDetectionChange((rv::MsgSensorDetectionChange*)da, c, aRole);
            }
            case (rv::MsgSensorTrackCreated::cMESSAGE_ID):
            {
               return HandleSensorTrackCreated((rv::MsgSensorTrackCreated*)da, c, aRole);
            }
            case (rv::MsgSensorTrackUpdate::cMESSAGE_ID):
            {
               return HandleSensorTrackUpdate((rv::MsgSensorTrackUpdate*)da, c, aRole);
            }
            case (rv::MsgSensorTrackDrop::cMESSAGE_ID):
            {
               return HandleSensorTrackDrop((rv::MsgSensorTrackDrop*)da, c, aRole);
            }
            case (rv::MsgEmitterModeDefinition::cMESSAGE_ID):
            {
               return HandleEmitterModeDefinition((rv::MsgEmitterModeDefinition*)da, c, aRole);
            }
            case (rv::MsgPartArticulationStatus::cMESSAGE_ID):
            {
               return HandlePartArticulationStatus((rv::MsgPartArticulationStatus*)da, c, aRole);
            }
            case (rv::MsgVisualPartDefinition::cMESSAGE_ID):
            {
               return HandleVisualPartDefinition((rv::MsgVisualPartDefinition*)da, c, aRole);
            }
            case (rv::MsgExecData::cMESSAGE_ID):
            {
               return HandleExecData((rv::MsgExecData*)da, c, aRole);
            }
            case (rv::MsgScenarioData::cMESSAGE_ID):
            {
               return HandleScenarioData((rv::MsgScenarioData*)da, c, aRole);
            }
            case (rv::MsgWeaponTerminated::cMESSAGE_ID):
            {
               return HandleWeaponTerminated((rv::MsgWeaponTerminated*)da, c, aRole);
            }
            case (rv::MsgWeaponFired::cMESSAGE_ID):
            {
               return HandleWeaponFired((rv::MsgWeaponFired*)da, c, aRole);
            }
            case (rv::MsgWeaponModeChange::cMESSAGE_ID):
            {
               return HandleWeaponModeChange((rv::MsgWeaponModeChange*)da, c, aRole);
            }
            case (rv::MsgWeaponQuantityChange::cMESSAGE_ID):
            {
               return HandleWeaponQuantityChange((rv::MsgWeaponQuantityChange*)da, c, aRole);
            }
            case (rv::MsgTaskUpdate::cMESSAGE_ID):
            {
               return HandleTaskUpdate((rv::MsgTaskUpdate*)da, c, aRole);
            }
            case (rv::MsgQuantumTaskerUpdate::cMESSAGE_ID):
            {
               return HandleQuantumTaskerUpdate((rv::MsgQuantumTaskerUpdate*)da, c, aRole);
            }
            case (rv::MsgJammingRequestInitiated::cMESSAGE_ID):
            {
               return HandleJammingRequestInitiated((rv::MsgJammingRequestInitiated*)da, c, aRole);
            }
            case (rv::MsgJammingRequestCanceled::cMESSAGE_ID):
            {
               return HandleJammingRequestCanceled((rv::MsgJammingRequestCanceled*)da, c, aRole);
            }
            case (rv::MsgJammingRequestUpdated::cMESSAGE_ID):
            {
               return HandleJammingRequestUpdated((rv::MsgJammingRequestUpdated*)da, c, aRole);
            }
            case (rv::MsgSetDate::cMESSAGE_ID):
            {
               return HandleSetDate((rv::MsgSetDate*)da, c, aRole);
            }
            case (rv::MsgResource::cMESSAGE_ID):
            {
               return HandleResource((rv::MsgResource*)da, c, aRole);
            }
            case (rv::MsgBookmark::cMESSAGE_ID):
            {
               return HandleBookmark((rv::MsgBookmark*)da, c, aRole);
            }
            case (rv::MsgRouteChanged::cMESSAGE_ID):
            {
               return HandleRouteChanged(static_cast<const rv::MsgRouteChanged*>(da), c, aRole);
            }
            case (rv::MsgCircularZone::cMESSAGE_ID):
            {
               return HandleCircularZone(static_cast<const rv::MsgCircularZone*>(da), c, aRole);
            }
            case (rv::MsgEllipticalZone::cMESSAGE_ID):
            {
               return HandleEllipticalZone(static_cast<const rv::MsgEllipticalZone*>(da), c, aRole);
            }
            case (rv::MsgSphericalZone::cMESSAGE_ID):
            {
               return HandleSphericalZone(static_cast<const rv::MsgSphericalZone*>(da), c, aRole);
            }
            case (rv::MsgPolygonalZone::cMESSAGE_ID):
            {
               return HandlePolygonalZone(static_cast<const rv::MsgPolygonalZone*>(da), c, aRole);
            }
            case (rv::MsgZoneSet::cMESSAGE_ID):
            {
               return HandleZoneSet(static_cast<const rv::MsgZoneSet*>(da), c, aRole);
            }
            case (rv::MsgCommInfo::cMESSAGE_ID):
            {
               return HandleCommInfo(static_cast<const rv::MsgCommInfo*>(da), c, aRole);
            }
            case (rv::MsgNetworkInfo::cMESSAGE_ID):
            {
               return HandleNetworkInfo(static_cast<const rv::MsgNetworkInfo*>(da), c, aRole);
            }
            case (rv::MsgRouterInfo::cMESSAGE_ID):
            {
               return HandleRouterInfo(static_cast<const rv::MsgRouterInfo*>(da), c, aRole);
            }
            default:
            {
               for (auto& ext : rvEnv.GetExtensions())
               {
                  if (ext->HandlesMessage(da->GetMessageId()))
                  {
                     return ext->HandleMessage(da, c, aRole);
                  }
               }
            }
               // we need to dig into the particular message type
            }
         }
      }
   }
   return QVariant();
}

static QString AuxDataValueToString(const rv::AuxDataValue& aValue)
{
   switch (aValue.type())
   {
   case rv::AuxDataType::Boolean:
      return aValue.boolean() ? "true" : "false";
      break;
   case rv::AuxDataType::Integer:
      return QString::number(aValue.integer());
      break;
   case rv::AuxDataType::Real:
      return QString::number(aValue.real());
      break;
   case rv::AuxDataType::Text:
      return QString::fromStdString(aValue.text());
      break;
   default:
      return "(unknown type)";
   }
}

QVariant RvStatistics::EventTableModel::HandleAuxData(const rv::MsgAuxData* aMsg, int aColumn, int aRole) const
{
   if (aColumn == rv::MessageBase::cFIELD_OWNER)
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->platformIndex(), aRole);
   }
   else if (aColumn >= rv::MessageBase::cFIELD_DATA)
   {
      const int index = aColumn - rv::MessageBase::cFIELD_DATA;
      if (index < static_cast<int>(aMsg->auxData().size()))
      {
         const auto& value = aMsg->auxData()[index];
         auto        name  = QString::fromStdString(value.name());
         if (aRole == Qt::ToolTipRole)
         {
            return name;
         }

         return name + ": " + ::AuxDataValueToString(value);
      }
   }

   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleEntityState(const rv::MsgEntityState* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->state().platformIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "Damage Factor";
      }
      return aMsg->state().damageFactor();
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "Fuel";
      }
      return aMsg->state().fuelCurrent();
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "WCS location X";
      }
      return aMsg->state().locationWCS().x();
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "WCS location Y";
      }
      return aMsg->state().locationWCS().y();
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "WCS location Z";
      }
      return aMsg->state().locationWCS().z();
   }
   case (rv::MessageBase::cFIELD_DATA + 5):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "WCS velocity X";
      }
      if (aMsg->state().velocityWCSValid())
      {
         return aMsg->state().velocityWCS().x();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 6):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "WCS velocity Y";
      }
      if (aMsg->state().velocityWCSValid())
      {
         return aMsg->state().velocityWCS().y();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 7):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "WCS velocity Z";
      }
      if (aMsg->state().velocityWCSValid())
      {
         return aMsg->state().velocityWCS().z();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 8):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "WCS acceleration X";
      }
      if (aMsg->state().accelerationWCSValid())
      {
         return aMsg->state().accelerationWCS().x();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 9):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "WCS acceleration Y";
      }
      if (aMsg->state().accelerationWCSValid())
      {
         return aMsg->state().accelerationWCS().y();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 10):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "WCS acceleration Z";
      }
      if (aMsg->state().accelerationWCSValid())
      {
         return aMsg->state().accelerationWCS().z();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 11):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "WCS orientation X";
      }
      if (aMsg->state().orientationWCSValid())
      {
         return aMsg->state().orientationWCS().x();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 12):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "WCS orientation Y";
      }
      if (aMsg->state().orientationWCSValid())
      {
         return aMsg->state().orientationWCS().y();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 13):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "WCS orientation Z";
      }
      if (aMsg->state().orientationWCSValid())
      {
         return aMsg->state().orientationWCS().z();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 14):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "mach";
      }
      if (aMsg->state().machNumberValid())
      {
         return aMsg->state().machNumber();
      }
      break;
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandlePlatformInfo(const rv::MsgPlatformInfo* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "platform";
      }
      return aMsg->platformIndex();
   }
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "name";
      }
      return rvEnv.LookupPlatformVariant(aMsg->platformIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "types";
      }
      if (aMsg->typesValid())
      {
         QString list;
         bool    first = true;
         for (auto&& it : aMsg->types())
         {
            if (first)
            {
               first = false;
            }
            else
            {
               list += "|";
            }
            list += it.c_str();
         }
         return list;
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "side";
      }
      if (aMsg->sideValid())
      {
         return aMsg->side().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "icon";
      }
      if (aMsg->iconValid())
      {
         return aMsg->icon().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "marking";
      }
      if (aMsg->markingValid())
      {
         return aMsg->marking().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 5):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "spatial domain";
      }
      if (aMsg->spatialDomainValid())
      {
         return rv::SpatialDomain::ToString(aMsg->spatialDomain());
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 6):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "command chains";
      }
      if (aMsg->commandChainsValid())
      {
         QString list;
         bool    first = true;
         for (auto&& it : aMsg->commandChains())
         {
            if (first)
            {
               first = false;
            }
            else
            {
               list += "|";
            }
            list += it.name().c_str();
            list += ":";
            list += it.commander().c_str();
         }
         return list;
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 7):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "categories";
      }
      if (aMsg->categoriesValid())
      {
         QString list;
         bool    first = true;
         for (auto&& it : aMsg->categories())
         {
            if (first)
            {
               first = false;
            }
            else
            {
               list += "|";
            }
            list += it.c_str();
         }
         return list;
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 8):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "DIS_ID";
      }
      else
      {
         return QString(QString::number(aMsg->disId().site()) + ":" + QString::number(aMsg->disId().application()) +
                        ":" + QString::number(aMsg->disId().entity()));
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 9):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "Fuel Capacity";
      }
      else
      {
         return aMsg->fuelCapacity();
      }
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleLocalTrackCreated(const rv::MsgLocalTrackCreated* aMsg,
                                                                int                             aColumn,
                                                                int                             aRole) const
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
         return "target";
      }
      return rvEnv.LookupPlatformVariant(aMsg->targetIndex(), aRole, aMsg->targetIndexValid());
   }
   case (rv::MessageBase::cFIELD_TRACK_ID):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "track id";
      }
      return QString(aMsg->trackId().owner().c_str()) + QString(":") + QString::number(aMsg->trackId().localTrackNumber());
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "master";
      }
      return QString(aMsg->masterProcessor() ? "true" : "false");
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleLocalTrackUpdate(const rv::MsgLocalTrackUpdate* aMsg, int aColumn, int aRole) const
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
         return "target";
      }
      return rvEnv.LookupPlatformVariant(aMsg->track().targetIndex(), aRole, aMsg->track().targetIndexValid());
   }
   case (rv::MessageBase::cFIELD_TRACK_ID):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "track_id";
      }
      return QString(aMsg->track().trackId().owner().c_str()) + QString(":") +
             QString::number(aMsg->track().trackId().localTrackNumber());
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "start time";
      }
      if (aMsg->track().startTimeValid())
      {
         return aMsg->track().startTime();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "update time";
      }
      if (aMsg->track().updateTimeValid())
      {
         return aMsg->track().updateTime();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "originator";
      }
      if (aMsg->track().originatorIndexValid())
      {
         return rvEnv.LookupPlatformName(aMsg->track().originatorIndex());
      }
   }
   case (rv::MessageBase::cFIELD_COMPONENT):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "sensor name";
      }
      if (aMsg->track().sensorNameValid())
      {
         return aMsg->track().sensorName().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "sensor mode";
      }
      if (aMsg->track().sensorModeValid())
      {
         return aMsg->track().sensorMode().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "update count";
      }
      if (aMsg->track().updateCountValid())
      {
         return aMsg->track().updateCount();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 5):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "originator WCS x";
      }
      if (aMsg->track().originatorWCSValid())
      {
         return aMsg->track().originatorWCS().x();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 6):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "originator WCS y";
      }
      if (aMsg->track().originatorWCSValid())
      {
         return aMsg->track().originatorWCS().y();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 7):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "originator WCS z";
      }
      if (aMsg->track().originatorWCSValid())
      {
         return aMsg->track().originatorWCS().z();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 8):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "location WCS x";
      }
      if (aMsg->track().locationWCSValid())
      {
         return aMsg->track().locationWCS().x();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 9):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "location WCS y";
      }
      if (aMsg->track().locationWCSValid())
      {
         return aMsg->track().locationWCS().y();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 10):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "location WCS z";
      }
      if (aMsg->track().locationWCSValid())
      {
         return aMsg->track().locationWCS().z();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 11):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "velocity WCS dx";
      }
      if (aMsg->track().velocityWCSValid())
      {
         return aMsg->track().velocityWCS().x();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 12):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "velocity WCS dy";
      }
      if (aMsg->track().velocityWCSValid())
      {
         return aMsg->track().velocityWCS().y();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 13):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "velocity WCS dz";
      }
      if (aMsg->track().velocityWCSValid())
      {
         return aMsg->track().velocityWCS().z();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 14):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "range";
      }
      if (aMsg->track().rangeValid())
      {
         return aMsg->track().range();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 15):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "range error";
      }
      if (aMsg->track().rangeErrorValid())
      {
         return aMsg->track().rangeError();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 16):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "bearing";
      }
      if (aMsg->track().bearingValid())
      {
         return aMsg->track().bearing();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 17):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "bearing error";
      }
      if (aMsg->track().bearingErrorValid())
      {
         return aMsg->track().bearingError();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 18):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "elevation";
      }
      if (aMsg->track().elevationValid())
      {
         return aMsg->track().elevation();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 19):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "elevation error";
      }
      if (aMsg->track().elevationErrorValid())
      {
         return aMsg->track().elevationError();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 20):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "range rate";
      }
      if (aMsg->track().rangeRateValid())
      {
         return aMsg->track().rangeRate();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 21):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "range rate error";
      }
      if (aMsg->track().rangeRateErrorValid())
      {
         return aMsg->track().rangeRateError();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 22):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "track quality";
      }
      if (aMsg->track().trackQualityValid())
      {
         return aMsg->track().trackQuality();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 23):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "signal to noise";
      }
      if (aMsg->track().signalToNoiseValid())
      {
         return aMsg->track().signalToNoise();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 24):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "pixel count";
      }
      if (aMsg->track().pixelCountValid())
      {
         return aMsg->track().pixelCount();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 25):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "spatial domain";
      }
      if (aMsg->track().spatialDomainValid())
      {
         return rv::SpatialDomain::ToString(aMsg->track().spatialDomain());
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 26):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "state covariance";
      }
      if (aMsg->track().stateCovarianceValid())
      {
         return "state covariance"; // TODO what?
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 27):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "side";
      }
      if (aMsg->track().sideValid())
      {
         return aMsg->track().side().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 28):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "type";
      }
      if (aMsg->track().typeValid())
      {
         return aMsg->track().type().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 29):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "IFF Status";
      }
      if (aMsg->track().IFFValid())
      {
         return rv::IFFStatus::ToString(aMsg->track().IFF());
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 30):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "icon";
      }
      if (aMsg->track().iconValid())
      {
         return aMsg->track().icon().c_str();
      }
      break;
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleLocalTrackDrop(const rv::MsgLocalTrackDrop* aMsg, int aColumn, int aRole) const
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
         return "target";
      }
      return rvEnv.LookupPlatformVariant(aMsg->targetIndex(), aRole, aMsg->targetIndexValid());
   }
   case (rv::MessageBase::cFIELD_TRACK_ID):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "track id";
      }
      return QString(aMsg->trackId().owner().c_str()) + QString(":") + QString::number(aMsg->trackId().localTrackNumber());
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "master";
      }
      return QString(aMsg->masterProcessor() ? "true" : "false");
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleLocalTrackCorrelation(const rv::MsgLocalTrackCorrelation* aMsg,
                                                                    int                                 aColumn,
                                                                    int                                 aRole) const
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
   case (rv::MessageBase::cFIELD_TRACK_ID):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "track id";
      }
      return QString(aMsg->trackId().owner().c_str()) + QString(":") + QString::number(aMsg->trackId().localTrackNumber());
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "add track";
      }
      return QString(aMsg->addTrackId().owner().c_str()) + QString(":") +
             QString::number(aMsg->addTrackId().localTrackNumber());
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleLocalTrackDecorrelation(const rv::MsgLocalTrackDecorrelation* aMsg,
                                                                      int                                   aColumn,
                                                                      int                                   aRole) const
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
   case (rv::MessageBase::cFIELD_TRACK_ID):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "track id";
      }
      return QString(aMsg->trackId().owner().c_str()) + QString(":") + QString::number(aMsg->trackId().localTrackNumber());
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "add track";
      }
      return QString(aMsg->remTrackId().owner().c_str()) + QString(":") +
             QString::number(aMsg->remTrackId().localTrackNumber());
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleSensorModeChange(const rv::MsgSensorModeChange* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->platformIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_COMPONENT):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "sensor name";
      }
      return aMsg->sensorName().c_str();
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "mode name";
      }
      return aMsg->modeName().c_str();
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "activate";
      }
      return aMsg->activate();
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandlePlatformStatus(const rv::MsgPlatformStatus* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->platformIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "broken";
      }
      return aMsg->broken();
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "removed";
      }
      return aMsg->removed();
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandlePlatformAppearanceChanged(const rv::MsgPlatformAppearanceChanged* aMsg,
                                                                        int                                     aColumn,
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
      return rvEnv.LookupPlatformVariant(aMsg->ownerIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "appearance word";
      }
      return std::bitset<sizeof(aMsg->appearance()) * 8>(aMsg->appearance()).to_string().c_str();
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandlePartStatus(const rv::MsgPartStatus* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->platformIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_COMPONENT):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "part name";
      }
      return aMsg->partName().c_str();
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "type";
      }
      return HandlePartType(aMsg->partType());
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "on";
      }
      return aMsg->on();
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "broken";
      }
      return aMsg->broken();
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "disabled";
      }
      return aMsg->broken();
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleComment(const rv::MsgComment* aMsg, int aColumn, int aRole) const
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

QVariant RvStatistics::EventTableModel::HandleDetectAttempt(const rv::MsgDetectAttempt* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->sensorPlatformIndex(), aRole, aMsg->sensorPlatformIndexValid());
   }
   case (rv::MessageBase::cFIELD_TARGET):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "target";
      }
      return rvEnv.LookupPlatformVariant(aMsg->targetPlatformIndex(), aRole, aMsg->targetPlatformIndexValid());
   }
   case (rv::MessageBase::cFIELD_COMPONENT):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "sensor name";
      }
      if (aMsg->sensorNameValid())
      {
         return aMsg->sensorName().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "Pd";
      }
      if (aMsg->pdValid())
      {
         return aMsg->pd();
      }
      break;
   }
      // TODO this is covered in the sensor detection report, I don't think we necessarily need it here, too
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleDrawCommand(const rv::MsgDrawCommand* aMsg, size_t aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "layer";
      }
      if (aMsg->layerValid())
      {
         return aMsg->layer().c_str();
      }
      break;
   }
   }
   size_t itemCount = aMsg->items().size();
   if ((aColumn > rv::MessageBase::cFIELD_DATA) && (aColumn < rv::MessageBase::cFIELD_DATA + 1 + itemCount * 2))
   {
      size_t item     = (aColumn - rv::MessageBase::cFIELD_DATA - 1) / 2;
      int    datatype = (aColumn - rv::MessageBase::cFIELD_DATA - 1) % 2;
      if (aRole == Qt::ToolTipRole)
      {
         return (datatype == 0) ? "Type" : "Draw ID";
      }
      if (datatype == 0)
      {
         return rv::MsgDrawCommand_DrawType::ToString(aMsg->items()[item].drawType());
      }
      else
      {
         return aMsg->items()[item].id();
      }
   }


   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleMessageReceived(const rv::MsgMessageReceived* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "transmitting platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->xmtrPlatformIndex(), aRole, aMsg->xmtrPlatformIndexValid());
   }
   case (rv::MessageBase::cFIELD_TARGET):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "receiving platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->rcvrPlatformIndex(), aRole, aMsg->rcvrPlatformIndexValid());
   }
   case (rv::MessageBase::cFIELD_COMPONENT):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "transmitting comm";
      }
      if (aMsg->xmtrCommNameValid())
      {
         return aMsg->xmtrCommName().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "receiving comm";
      }
      if (aMsg->rcvrCommNameValid())
      {
         return aMsg->rcvrCommName().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "message type";
      }
      if (aMsg->messageTypeValid())
      {
         return aMsg->messageType().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "message sub-type";
      }
      if (aMsg->messageSubTypeValid())
      {
         return aMsg->messageSubType().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "message size";
      }
      if (aMsg->messageSizeValid())
      {
         return aMsg->messageSize();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "serial number";
      }
      if (aMsg->serialNumberValid())
      {
         return aMsg->serialNumber();
      }
      break;
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleMessageHop(const rv::MsgMessageHop* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "transmitting platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->xmtrPlatformIndex(), aRole, aMsg->xmtrPlatformIndexValid());
   }
   case (rv::MessageBase::cFIELD_TARGET):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "receiving platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->rcvrPlatformIndex(), aRole, aMsg->rcvrPlatformIndexValid());
   }
   case (rv::MessageBase::cFIELD_COMPONENT):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "transmitting comm";
      }
      if (aMsg->xmtrCommNameValid())
      {
         return aMsg->xmtrCommName().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "receiving comm";
      }
      if (aMsg->rcvrCommNameValid())
      {
         return aMsg->rcvrCommName().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "message type";
      }
      if (aMsg->messageTypeValid())
      {
         return aMsg->messageType().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "message sub-type";
      }
      if (aMsg->messageSubTypeValid())
      {
         return aMsg->messageSubType().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "message size";
      }
      if (aMsg->messageSizeValid())
      {
         return aMsg->messageSize();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "serial number";
      }
      if (aMsg->serialNumberValid())
      {
         return aMsg->serialNumber();
      }
      break;
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleMessageTransmitted(const rv::MsgMessageTransmitted* aMsg,
                                                                 int                              aColumn,
                                                                 int                              aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "transmitting platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->xmtrPlatformIndex(), aRole, aMsg->xmtrPlatformIndexValid());
   }
   case (rv::MessageBase::cFIELD_COMPONENT):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "transmitting comm";
      }
      if (aMsg->xmtrCommNameValid())
      {
         return aMsg->xmtrCommName().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 1): // we skip 0 for alignment with the receive messages
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "message type";
      }
      if (aMsg->messageTypeValid())
      {
         return aMsg->messageType().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "message sub-type";
      }
      if (aMsg->messageSubTypeValid())
      {
         return aMsg->messageSubType().c_str();
      }
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "message size";
      }
      if (aMsg->messageSizeValid())
      {
         return aMsg->messageSize();
      }
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "serial number";
      }
      if (aMsg->serialNumberValid())
      {
         return aMsg->serialNumber();
      }
      break;
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleScenarioData(const rv::MsgScenarioData* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "classification";
      }
      return aMsg->classification().c_str();
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleScriptData(const rv::MsgScriptData* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->platformIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "key";
      }
      return aMsg->key().c_str();
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "value";
      }
      switch (aMsg->value().GetFieldType())
      {
      case (rv::MsgScriptData_Value::cFT_floating):
         return aMsg->value().floating();
      case (rv::MsgScriptData_Value::cFT_integer):
         return aMsg->value().integer();
      case (rv::MsgScriptData_Value::cFT_boolean):
         return aMsg->value().boolean() == 0 ? "false" : "true";
      case (rv::MsgScriptData_Value::cFT_text):
         return QString::fromStdString(aMsg->value().text());
      case (rv::MsgScriptData_Value::cFT_NONE):
      default:
         break;
      }
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleSensorDetectionChange(const rv::MsgSensorDetectionChange* aMsg,
                                                                    int                                 aColumn,
                                                                    int                                 aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->ownerIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_TARGET):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "target";
      }
      return rvEnv.LookupPlatformVariant(aMsg->targetIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_COMPONENT):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "sensor";
      }
      return aMsg->sensorName().c_str();
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "detected";
      }
      return aMsg->detected();
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleSensorTrackCreated(const rv::MsgSensorTrackCreated* aMsg,
                                                                 int                              aColumn,
                                                                 int                              aRole) const
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
         return "target";
      }
      return rvEnv.LookupPlatformVariant(aMsg->targetIndex(), aRole, aMsg->targetIndexValid());
   }
   case (rv::MessageBase::cFIELD_TRACK_ID):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "track id";
      }
      return QString(aMsg->trackId().owner().c_str()) + QString(":") + QString::number(aMsg->trackId().localTrackNumber());
   }
   case (rv::MessageBase::cFIELD_COMPONENT):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "sensor";
      }
      return QString(aMsg->sensorName().c_str());
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleSensorTrackUpdate(const rv::MsgSensorTrackUpdate* aMsg,
                                                                int                             aColumn,
                                                                int                             aRole) const
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
         return "target";
      }
      return rvEnv.LookupPlatformVariant(aMsg->track().targetIndex(), aRole, aMsg->track().targetIndexValid());
   }
   case (rv::MessageBase::cFIELD_TRACK_ID):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "track_id";
      }
      return QString(aMsg->track().trackId().owner().c_str()) + QString(":") +
             QString::number(aMsg->track().trackId().localTrackNumber());
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "start time";
      }
      if (aMsg->track().startTimeValid())
      {
         return aMsg->track().startTime();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "update time";
      }
      if (aMsg->track().updateTimeValid())
      {
         return aMsg->track().updateTime();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "originator";
      }
      if (aMsg->track().originatorIndexValid())
      {
         return rvEnv.LookupPlatformName(aMsg->track().originatorIndex());
      }
   }
   case (rv::MessageBase::cFIELD_COMPONENT):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "sensor name";
      }
      if (aMsg->track().sensorNameValid())
      {
         return aMsg->track().sensorName().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "sensor mode";
      }
      if (aMsg->track().sensorModeValid())
      {
         return aMsg->track().sensorMode().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "update count";
      }
      if (aMsg->track().updateCountValid())
      {
         return aMsg->track().updateCount();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 5):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "originator WCS x";
      }
      if (aMsg->track().originatorWCSValid())
      {
         return aMsg->track().originatorWCS().x();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 6):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "originator WCS y";
      }
      if (aMsg->track().originatorWCSValid())
      {
         return aMsg->track().originatorWCS().y();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 7):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "originator WCS z";
      }
      if (aMsg->track().originatorWCSValid())
      {
         return aMsg->track().originatorWCS().z();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 8):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "location WCS x";
      }
      if (aMsg->track().locationWCSValid())
      {
         return aMsg->track().locationWCS().x();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 9):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "location WCS y";
      }
      if (aMsg->track().locationWCSValid())
      {
         return aMsg->track().locationWCS().y();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 10):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "location WCS z";
      }
      if (aMsg->track().locationWCSValid())
      {
         return aMsg->track().locationWCS().z();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 11):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "velocity WCS dx";
      }
      if (aMsg->track().velocityWCSValid())
      {
         return aMsg->track().velocityWCS().x();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 12):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "velocity WCS dy";
      }
      if (aMsg->track().velocityWCSValid())
      {
         return aMsg->track().velocityWCS().y();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 13):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "velocity WCS dz";
      }
      if (aMsg->track().velocityWCSValid())
      {
         return aMsg->track().velocityWCS().z();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 14):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "range";
      }
      if (aMsg->track().rangeValid())
      {
         return aMsg->track().range();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 15):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "range error";
      }
      if (aMsg->track().rangeErrorValid())
      {
         return aMsg->track().rangeError();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 16):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "bearing";
      }
      if (aMsg->track().bearingValid())
      {
         return aMsg->track().bearing();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 17):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "bearing error";
      }
      if (aMsg->track().bearingErrorValid())
      {
         return aMsg->track().bearingError();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 18):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "elevation";
      }
      if (aMsg->track().elevationValid())
      {
         return aMsg->track().elevation();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 19):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "elevation error";
      }
      if (aMsg->track().elevationErrorValid())
      {
         return aMsg->track().elevationError();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 20):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "range rate";
      }
      if (aMsg->track().rangeRateValid())
      {
         return aMsg->track().rangeRate();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 21):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "range rate error";
      }
      if (aMsg->track().rangeRateErrorValid())
      {
         return aMsg->track().rangeRateError();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 22):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "track quality";
      }
      if (aMsg->track().trackQualityValid())
      {
         return aMsg->track().trackQuality();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 23):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "signal to noise";
      }
      if (aMsg->track().signalToNoiseValid())
      {
         return aMsg->track().signalToNoise();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 24):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "pixel count";
      }
      if (aMsg->track().pixelCountValid())
      {
         return aMsg->track().pixelCount();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 25):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "spatial domain";
      }
      if (aMsg->track().spatialDomainValid())
      {
         return rv::SpatialDomain::ToString(aMsg->track().spatialDomain());
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 26):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "state covariance";
      }
      if (aMsg->track().stateCovarianceValid())
      {
         return "state covariance"; // TODO what?
      }
      break;
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleSensorTrackDrop(const rv::MsgSensorTrackDrop* aMsg, int aColumn, int aRole) const
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
         return "target";
      }
      return rvEnv.LookupPlatformVariant(aMsg->targetIndex(), aRole, aMsg->targetIndexValid());
   }
   case (rv::MessageBase::cFIELD_TRACK_ID):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "track id";
      }
      return QString(aMsg->trackId().owner().c_str()) + QString(":") + QString::number(aMsg->trackId().localTrackNumber());
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleEmitterModeDefinition(const rv::MsgEmitterModeDefinition* aMsg,
                                                                    size_t                              aColumn,
                                                                    int                                 aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->ownerIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_COMPONENT):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "part";
      }
      return QString::fromStdString(aMsg->partName());
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "type";
      }
      return HandlePartType(aMsg->partType());
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "mode name";
      }
      return aMsg->modeName().c_str();
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "slew mode";
      }
      return QString::fromStdString(rv::AngleMode::ToString(aMsg->slewMode()));
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "slew limit";
      }
      return rv::toString(aMsg->slewLimit());
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "cue limit";
      }
      return rv::toString(aMsg->cueLimit());
   }
   }
   if ((aColumn > rv::MessageBase::cFIELD_DATA) &&
       (aColumn < rv::MessageBase::cFIELD_DATA + 5 + aMsg->beamList().size() * 10))
   {
      size_t beam  = (aColumn - (rv::MessageBase::cFIELD_DATA + 5)) / 10;
      int    field = (aColumn - (rv::MessageBase::cFIELD_DATA + 5)) % 10;
      switch (field)
      {
      case (0): // beam id
      {
         if (aRole == Qt::ToolTipRole)
         {
            return QString::number(beam) + " beam id";
         }
         return aMsg->beamList()[beam].beamId();
      }
      case (1): // r0 range
      {
         if (aRole == Qt::ToolTipRole)
         {
            return QString::number(beam) + " r0 range";
         }
         return aMsg->beamList()[beam].r0_range();
      }
      case (2): // r0 range type
      {
         if (aRole == Qt::ToolTipRole)
         {
            return QString::number(beam) + " r0 range type";
         }
         return aMsg->beamList()[beam].r0_rangeType();
      }
      case (3): // min range
      {
         if (aRole == Qt::ToolTipRole)
         {
            return QString::number(beam) + " range";
         }
         return QString::number(aMsg->beamList()[beam].minRange()) + "|" +
                QString::number(aMsg->beamList()[beam].maxRange());
      }
      case (4): // stabilized
      {
         if (aRole == Qt::ToolTipRole)
         {
            return QString::number(beam) + " stabilized";
         }
         return aMsg->beamList()[beam].stabilized();
      }
      case (5): // tilt
      {
         if (aRole == Qt::ToolTipRole)
         {
            return QString::number(beam) + " tilt";
         }
         return aMsg->beamList()[beam].antennaTilt();
      }
      case (6): // scan mode
      {
         if (aRole == Qt::ToolTipRole)
         {
            return QString::number(beam) + " scan mode";
         }
         return QString::fromStdString(rv::AngleMode::ToString(aMsg->beamList()[beam].scanMode()));
      }
      case (7): // scan limits
      {
         if (aRole == Qt::ToolTipRole)
         {
            return QString::number(beam) + " scan limit";
         }
         return rv::toString(aMsg->beamList()[beam].scanLimit());
      }
      case (8): // beam width
      {
         if (aRole == Qt::ToolTipRole)
         {
            return QString::number(beam) + " beam width";
         }
         const rv::BeamDefinition& bd = aMsg->beamList()[beam];

         QString azWidth = bd.beamWidthAzValid() ? QString::number(bd.beamWidthAz()) : "undefined";
         QString elWidth = bd.beamWidthElValid() ? QString::number(bd.beamWidthEl()) : "undefined";
         return azWidth + "|" + elWidth;
      }
      case (9): // fov
      {
         if (aRole == Qt::ToolTipRole)
         {
            return QString::number(beam) + " fov";
         }
         if (aMsg->beamList()[beam].fov().points().empty())
         {
            return "undefined";
         }
         else
         {
            QString ret = QString::number(aMsg->beamList()[beam].fov().shape()) + ": ";
            for (auto it : aMsg->beamList()[beam].fov().points())
            {
               ret += "(" + QString::number(it.x()) + "," + QString::number(it.y()) + ")";
            }
            return ret;
         }
      }
      }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandlePartArticulationStatus(const rv::MsgPartArticulationStatus* aMsg,
                                                                     int                                  aColumn,
                                                                     int                                  aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->ownerIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_COMPONENT):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "part name";
      }
      return aMsg->partName().c_str();
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "type";
      }
      return HandlePartType(aMsg->partType());
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "ecs x";
      }
      return aMsg->locationECS().x();
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "ecs y";
      }
      return aMsg->locationECS().y();
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "ecs z";
      }
      return aMsg->locationECS().z();
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "slew phi";
      }
      return aMsg->slewECS().x();
   }
   case (rv::MessageBase::cFIELD_DATA + 5):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "slew theta";
      }
      return aMsg->slewECS().y();
   }
   case (rv::MessageBase::cFIELD_DATA + 6):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "slew psi";
      }
      return aMsg->slewECS().z();
   }
   case (rv::MessageBase::cFIELD_DATA + 7):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "cue phi";
      }
      return aMsg->cuePCS().x();
   }
   case (rv::MessageBase::cFIELD_DATA + 8):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "cue theta";
      }
      return aMsg->cuePCS().y();
   }
   case (rv::MessageBase::cFIELD_DATA + 9):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "cue psi";
      }
      return aMsg->cuePCS().z();
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleVisualPartDefinition(const rv::MsgVisualPartDefinition* aMsg,
                                                                   int                                aColumn,
                                                                   int                                aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->ownerIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_COMPONENT): // part name
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "name";
      }
      return QString(aMsg->partName().c_str());
   }
   case (rv::MessageBase::cFIELD_DATA): // part type
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "type";
      }
      return HandlePartType(aMsg->partType());
   }
   case (rv::MessageBase::cFIELD_DATA + 1): // part icon
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "icon";
      }
      return QString(aMsg->partIcon().c_str());
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleExecData(const rv::MsgExecData* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "path";
      }
      return QString(aMsg->execPath().c_str());
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "cmdLine";
      }
      return QString(aMsg->cmdLine().c_str());
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "version";
      }
      return QString(aMsg->version().c_str());
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "timeStamp";
      }
      return QString(aMsg->timeStamp().c_str());
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "features";
      }
      return QString(aMsg->features().c_str());
   }
   case (rv::MessageBase::cFIELD_DATA + 5):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "extensions";
      }
      return QString(aMsg->extensions().c_str());
   }
   case (rv::MessageBase::cFIELD_DATA + 6):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "name";
      }
      return QString::fromStdString(aMsg->simName());
   }
   case (rv::MessageBase::cFIELD_DATA + 7):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "disApplication";
      }
      return QString::number(aMsg->disSite()) + ":" + QString::number(aMsg->disApplication());
   }
   case (rv::MessageBase::cFIELD_DATA + 8):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "runNumber";
      }
      return aMsg->runNumber();
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleWeaponTerminated(const rv::MsgWeaponTerminated* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->firingPlatformIndex(), aRole, aMsg->firingPlatformIndexValid());
   }
   case (rv::MessageBase::cFIELD_TARGET):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "target";
      }
      return rvEnv.LookupPlatformVariant(aMsg->targetPlatformIndex(), aRole, aMsg->targetPlatformIndexValid());
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "munition platform";
      }
      if (aMsg->weaponPlatformIndexValid())
      {
         return rvEnv.LookupPlatformName(aMsg->weaponPlatformIndex());
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "intended target";
      }
      if (aMsg->intendedTargetNameValid())
      {
         return aMsg->intendedTargetName().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "result";
      }
      if (aMsg->geometryResultValid())
      {
         return rv::MsgWeaponTerminated_GeometryResult::ToString(aMsg->geometryResult());
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "extended result";
      }
      if (aMsg->extendedResultValid())
      {
         return aMsg->extendedResult().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "miss distance";
      }
      if (aMsg->missDistanceValid())
      {
         return aMsg->missDistance();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 5):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "intercept pk";
      }
      if (aMsg->interceptPkValid())
      {
         return aMsg->interceptPk();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 6):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "pk drawn";
      }
      if (aMsg->pkDrawnValid())
      {
         if (aMsg->pkDrawn() < 0)
         {
            return "indestructible";
         }
         return aMsg->pkDrawn();
      }
      break;
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleWeaponFired(const rv::MsgWeaponFired* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->firingPlatformIndex(), aRole, aMsg->firingPlatformIndexValid());
   }
   case (rv::MessageBase::cFIELD_TARGET):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "target";
      }
      return rvEnv.LookupPlatformVariant(aMsg->targetPlatformIndex(), aRole, aMsg->targetPlatformIndexValid());
   }
   case (rv::MessageBase::cFIELD_TRACK_ID):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "track id";
      }
      if (aMsg->targetTrackIdValid())
      {
         return QString(aMsg->targetTrackId().owner().c_str()) + QString(":") +
                QString::number(aMsg->targetTrackId().localTrackNumber());
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "munition platform";
      }
      if (aMsg->weaponPlatformIndexValid())
      {
         return rvEnv.LookupPlatformName(aMsg->weaponPlatformIndex());
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "intended target";
      }
      if (aMsg->intendedTargetNameValid())
      {
         return aMsg->intendedTargetName().c_str();
      }
      break;
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleWeaponModeChange(const rv::MsgWeaponModeChange* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->platformIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_COMPONENT):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "weapon";
      }
      return aMsg->weaponName().c_str();
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "mode";
      }
      return aMsg->modeName().c_str();
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "activate";
      }
      return aMsg->activate();
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleWeaponQuantityChange(const rv::MsgWeaponQuantityChange* aMsg,
                                                                   int                                aColumn,
                                                                   int                                aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->platformIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_COMPONENT):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "weapon";
      }
      return aMsg->weaponName().c_str();
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "quantity";
      }
      return std::to_string(aMsg->weaponQuantity()).c_str();
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleTaskUpdate(const rv::MsgTaskUpdate* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "assigner";
      }
      return rvEnv.LookupPlatformVariant(aMsg->assignerPlatform(), aRole, aMsg->assignerPlatformValid());
   }
   case (rv::MessageBase::cFIELD_TARGET):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "assignee";
      }
      return rvEnv.LookupPlatformVariant(aMsg->assigneePlatform(), aRole, aMsg->assigneePlatformValid());
   }
   case (rv::MessageBase::cFIELD_TRACK_ID):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "track id";
      }
      return QString(aMsg->trackId().owner().c_str()) + QString(":") + QString::number(aMsg->trackId().localTrackNumber());
   }
   case (rv::MessageBase::cFIELD_COMPONENT):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "assigner processor";
      }
      if (aMsg->assignerProcessorValid())
      {
         return aMsg->assignerProcessor().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "state";
      }
      return rv::MsgTaskUpdate_State::ToString(aMsg->state());
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "task id";
      }
      return aMsg->taskId();
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "task type";
      }
      if (aMsg->taskTypeValid())
      {
         return aMsg->taskType().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "assignee processor";
      }
      if (aMsg->assigneeProcessorValid())
      {
         return aMsg->assigneeProcessor().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "assign time";
      }
      return aMsg->assignTime();
   }
   case (rv::MessageBase::cFIELD_DATA + 5):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "status";
      }
      if (aMsg->statusValid())
      {
         return aMsg->status().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 6):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "sub-status";
      }
      if (aMsg->subStatusValid())
      {
         return aMsg->subStatus().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 7):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "target";
      }
      if (aMsg->targetIndexValid())
      {
         return rvEnv.LookupPlatformName(aMsg->targetIndex());
      }
      break;
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleQuantumTaskerUpdate(const rv::MsgQuantumTaskerUpdate* aMsg,
                                                                  int                               aColumn,
                                                                  int                               aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "column";
      }
      if (aMsg->columnValid())
      {
         return aMsg->column();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "row";
      }
      if (aMsg->rowValid())
      {
         return aMsg->row();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "value";
      }
      if (aMsg->valueValid())
      {
         return aMsg->value();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "task name";
      }
      if (aMsg->taskNameValid())
      {
         return aMsg->taskName().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "asset name";
      }
      if (aMsg->assetNameValid())
      {
         return aMsg->assetName().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 5):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "resource name";
      }
      if (aMsg->resourceNameValid())
      {
         return aMsg->resourceName().c_str();
      }
      break;
   }
   case (rv::MessageBase::cFIELD_DATA + 6):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "winner";
      }
      if (aMsg->taskWinnerValid())
      {
         return aMsg->taskWinner();
      }
      break;
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleJammingRequestInitiated(const rv::MsgJammingRequestInitiated* aMsg,
                                                                      int                                   aColumn,
                                                                      int                                   aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "jamming platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->srcPlatform(), aRole);
   }
   case (rv::MessageBase::cFIELD_TARGET):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "target";
      }
      return rvEnv.LookupPlatformVariant(aMsg->target(), aRole);
   }
   case (rv::MessageBase::cFIELD_COMPONENT):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "weapon";
      }
      return aMsg->weaponName().c_str();
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "frequency";
      }
      return aMsg->frequency();
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "bandwidth";
      }
      return aMsg->bandwidth();
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "technique";
      }
      return aMsg->technique().c_str();
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleJammingRequestCanceled(const rv::MsgJammingRequestCanceled* aMsg,
                                                                     int                                  aColumn,
                                                                     int                                  aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "jamming platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->srcPlatform(), aRole);
   }
   case (rv::MessageBase::cFIELD_TARGET):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "target";
      }
      return rvEnv.LookupPlatformVariant(aMsg->target(), aRole);
   }
   case (rv::MessageBase::cFIELD_COMPONENT):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "weapon";
      }
      return aMsg->weaponName().c_str();
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "frequency";
      }
      return aMsg->frequency();
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "bandwidth";
      }
      return aMsg->bandwidth();
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleJammingRequestUpdated(const rv::MsgJammingRequestUpdated* aMsg,
                                                                    int                                 aColumn,
                                                                    int                                 aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "jamming platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->srcPlatform(), aRole);
   }
   case (rv::MessageBase::cFIELD_TARGET):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "target";
      }
      return rvEnv.LookupPlatformVariant(aMsg->target(), aRole);
   }
   case (rv::MessageBase::cFIELD_COMPONENT):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "weapon";
      }
      return aMsg->weaponName().c_str();
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "frequency";
      }
      return aMsg->frequency();
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "bandwidth";
      }
      return aMsg->bandwidth();
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "technique";
      }
      return aMsg->technique().c_str();
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleSetDate(const rv::MsgSetDate* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "time";
      }
      int     hours     = aMsg->time() / 3600;
      int     mins      = (aMsg->time() - 3600 * hours) / 60;
      QString minSpacer = ":";
      if (mins < 10)
      {
         minSpacer = ":0";
      }
      QString secSpacer = ":";
      float   seconds   = aMsg->time() - 3600 * hours - 60 * mins;
      if (seconds < 10)
      {
         secSpacer = ":0";
      }
      QString retval = QString::number(hours) + minSpacer + QString::number(mins) + secSpacer + QString::number(seconds);
      return retval;
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "month";
      }
      return aMsg->month();
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "day";
      }
      return aMsg->day();
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "year";
      }
      return aMsg->year();
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleResource(const rv::MsgResource* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "resource type";
      }
      return rv::MsgResource_ResourceType::ToString(aMsg->resourceType());
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "resource id";
      }
      return aMsg->resourceId().c_str();
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "paths";
      }
      QStringList paths;
      for (auto&& path : aMsg->resourceLocation())
      {
         paths += path.c_str();
      }
      return paths.join('|');
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleBookmark(const rv::MsgBookmark* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "time";
      }
      return aMsg->time();
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "type";
      }
      return QString::fromStdString(aMsg->msgType());
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "text";
      }
      return QString::fromStdString(aMsg->msgText());
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleRouteChanged(const rv::MsgRouteChanged* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_OWNER):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "platform";
      }
      return rvEnv.LookupPlatformVariant(aMsg->platformIndex(), aRole);
   }
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "WCS-X";
      }
      return aMsg->location().x();
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "WCS-Y";
      }
      return aMsg->location().y();
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "WCS-Z";
      }
      return aMsg->location().z();
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "NED Heading";
      }
      return aMsg->heading();
   }
   }
   // there are 6 fields per waypoint, we allow up to 10 waypoints to be displayed
   if (aColumn < rv::MessageBase::cFIELD_DATA + 3 + 10 * 6)
   {
      size_t wpIdx = (aColumn - rv::MessageBase::cFIELD_DATA - 4) / 6;
      if (wpIdx < aMsg->route().size())
      {
         int   wpDatum = (aColumn - rv::MessageBase::cFIELD_DATA - 4) % 6;
         auto& wp      = aMsg->route()[wpIdx];
         switch (wpDatum)
         {
         case (0):
         {
            if (aRole == Qt::ToolTipRole)
            {
               return "wp" + QString::number(wpIdx + 1) + ": label";
            }
            return QString::fromStdString(wp.label());
         }
         case (1):
         {
            if (aRole == Qt::ToolTipRole)
            {
               QString locType = "lat";
               if (wp.locationType() != rv::WaypointLocationType::LatitiudeAndLongitude)
               {
                  locType = "x-offset";
               }
               return "wp" + QString::number(wpIdx + 1) + ": " + locType;
            }
            return wp.locationX();
         }
         case (2):
         {
            if (aRole == Qt::ToolTipRole)
            {
               QString locType = "lon";
               if (wp.locationType() != rv::WaypointLocationType::LatitiudeAndLongitude)
               {
                  locType = "y-offset";
               }
               return "wp" + QString::number(wpIdx + 1) + ": " + locType;
            }
            return wp.locationY();
         }
         case (3):
         {
            if (aRole == Qt::ToolTipRole)
            {
               return "wp" + QString::number(wpIdx + 1) + ": altitude";
            }
            if (wp.altitudeValid())
            {
               return wp.altitude();
            }
            else
            {
               return "use_prev";
            }
         }
         case (4):
         {
            if (aRole == Qt::ToolTipRole)
            {
               return "wp" + QString::number(wpIdx + 1) + ": heading";
            }
            if (wp.headingValid())
            {
               return wp.heading();
            }
            else
            {
               return "use_prev";
            }
         }
         case (5):
         {
            if (aRole == Qt::ToolTipRole)
            {
               return "wp" + QString::number(wpIdx + 1) + ": goto";
            }
            if (wp.gotoIdValid())
            {
               return QString::fromStdString(wp.gotoId());
            }
            else
            {
               return QVariant();
            }
         }
         }
      }
   }
   return QVariant();
}

namespace
{
QString GetRgbaString(const rv::ColorF& aColor)
{
   QColor color;
   color.setRgbF(aColor.red(), aColor.green(), aColor.blue(), aColor.alpha());
   QString r = QString::number(color.red());
   QString g = QString::number(color.green());
   QString b = QString::number(color.blue());
   QString a = QString::number(color.alpha());
   return ("rgba(" + r + "," + g + "," + b + "," + a + ")");
}

QVariant CommonZoneDataSwitch(const rv::CommonZoneData& aData, int aColumn, int aRole)
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "zone name";
      }
      return QString::fromStdString(aData.zoneName());
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "parent platform";
      }
      return QString::fromStdString(aData.parentPlatform());
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "reference platform";
      }
      return QString::fromStdString(aData.referencePlatform());
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "global";
      }
      return QString::fromStdString(aData.isGlobal() ? "global" : "not global");
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "line color";
      }
      return GetRgbaString(aData.lineColor());
   }
   case (rv::MessageBase::cFIELD_DATA + 5):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "fill color";
      }
      return GetRgbaString(aData.fillColor());
   }
   case (rv::MessageBase::cFIELD_DATA + 6):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "max altitude";
      }
      return aData.maxAltitude();
   }
   case (rv::MessageBase::cFIELD_DATA + 7):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "min altitude";
      }
      return aData.minAltitude();
   }
   case (rv::MessageBase::cFIELD_DATA + 8):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "reference lat";
      }
      return aData.referenceLat();
   }
   case (rv::MessageBase::cFIELD_DATA + 9):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "reference lon";
      }
      return aData.referenceLon();
   }
   case (rv::MessageBase::cFIELD_DATA + 10):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "use location";
      }
      return QString::fromStdString(aData.locationDefined() ? "use lat/lon" : "use platform");
   }
   case (rv::MessageBase::cFIELD_DATA + 11):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "heading";
      }
      return aData.heading();
   }
   }
   return QVariant();
}
} // namespace

QVariant RvStatistics::EventTableModel::HandleCircularZone(const rv::MsgCircularZone* aMsg, int aColumn, int aRole) const
{
   if (aColumn >= rv::MessageBase::cFIELD_DATA && aColumn <= rv::MessageBase::cFIELD_DATA + 11)
   {
      return CommonZoneDataSwitch(aMsg->zoneInfo().commonInfo(), aColumn, aRole);
   }
   else
   {
      switch (aColumn)
      {
      case (rv::MessageBase::cFIELD_DATA + 12):
      {
         if (aRole == Qt::ToolTipRole)
         {
            return "radius";
         }
         return aMsg->zoneInfo().radius();
      }
      case (rv::MessageBase::cFIELD_DATA + 13):
      {
         if (aRole == Qt::ToolTipRole)
         {
            return "start angle";
         }
         return aMsg->zoneInfo().startAngle();
      }
      case (rv::MessageBase::cFIELD_DATA + 14):
      {
         if (aRole == Qt::ToolTipRole)
         {
            return "stop angle";
         }
         return aMsg->zoneInfo().stopAngle();
      }
      }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleEllipticalZone(const rv::MsgEllipticalZone* aMsg, int aColumn, int aRole) const
{
   if (aColumn >= rv::MessageBase::cFIELD_DATA && aColumn <= rv::MessageBase::cFIELD_DATA + 11)
   {
      return CommonZoneDataSwitch(aMsg->zoneInfo().commonInfo(), aColumn, aRole);
   }
   else
   {
      switch (aColumn)
      {
      case (rv::MessageBase::cFIELD_DATA + 12):
      {
         if (aRole == Qt::ToolTipRole)
         {
            return "lat axis";
         }
         return aMsg->zoneInfo().latAxis();
      }
      case (rv::MessageBase::cFIELD_DATA + 13):
      {
         if (aRole == Qt::ToolTipRole)
         {
            return "lon axis";
         }
         return aMsg->zoneInfo().lonAxis();
      }
      case (rv::MessageBase::cFIELD_DATA + 14):
      {
         if (aRole == Qt::ToolTipRole)
         {
            return "start angle";
         }
         return aMsg->zoneInfo().startAngle();
      }
      case (rv::MessageBase::cFIELD_DATA + 15):
      {
         if (aRole == Qt::ToolTipRole)
         {
            return "stop angle";
         }
         return aMsg->zoneInfo().stopAngle();
      }
      }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleSphericalZone(const rv::MsgSphericalZone* aMsg, int aColumn, int aRole) const
{
   if (aColumn >= rv::MessageBase::cFIELD_DATA && aColumn <= rv::MessageBase::cFIELD_DATA + 11)
   {
      return CommonZoneDataSwitch(aMsg->zoneInfo().commonInfo(), aColumn, aRole);
   }
   else
   {
      switch (aColumn)
      {
      case (rv::MessageBase::cFIELD_DATA + 12):
      {
         if (aRole == Qt::ToolTipRole)
         {
            return "radius";
         }
         return aMsg->zoneInfo().radius();
      }
      case (rv::MessageBase::cFIELD_DATA + 13):
      {
         if (aRole == Qt::ToolTipRole)
         {
            return "start angle";
         }
         return aMsg->zoneInfo().startAngle();
      }
      case (rv::MessageBase::cFIELD_DATA + 14):
      {
         if (aRole == Qt::ToolTipRole)
         {
            return "stop angle";
         }
         return aMsg->zoneInfo().stopAngle();
      }
      }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandlePolygonalZone(const rv::MsgPolygonalZone* aMsg, int aColumn, int aRole) const
{
   if (aColumn >= rv::MessageBase::cFIELD_DATA && aColumn <= rv::MessageBase::cFIELD_DATA + 11)
   {
      return CommonZoneDataSwitch(aMsg->zoneInfo().commonInfo(), aColumn, aRole);
   }
   else if (aColumn == rv::MessageBase::cFIELD_DATA + 12)
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "use lat/lon";
      }
      return (aMsg->zoneInfo().useLatLon() ? "use lat/lon" : "use relative");
   }
   // Limit to a maximum of 5 points
   else if (aColumn >= rv::MessageBase::cFIELD_DATA + 13 && aColumn <= rv::MessageBase::cFIELD_DATA + 17)
   {
      int  index     = aColumn - (rv::MessageBase::cFIELD_DATA + 13);
      auto pointList = aMsg->zoneInfo().Points();
      if (index >= 0 && index < static_cast<int>(pointList.size()))
      {
         if (aRole == Qt::ToolTipRole)
         {
            return "point " + QString::number(index + 1);
         }
         return "(" + QString::number(pointList[index].x()) + "," + QString::number(pointList[index].y()) + ")";
      }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleZoneSet(const rv::MsgZoneSet* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "zone set name";
      }
      return QString::fromStdString(aMsg->zoneSetName());
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "parent platform";
      }
      return QString::fromStdString(aMsg->parentPlatform());
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "is global";
      }
      return (aMsg->isGlobal() ? "is global" : "not global");
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "line color";
      }
      return GetRgbaString(aMsg->lineColor());
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "fill color";
      }
      return GetRgbaString(aMsg->fillColor());
   }
   case (rv::MessageBase::cFIELD_DATA + 5):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "circular quantity";
      }
      return aMsg->circularZones().GetSize();
   }
   case (rv::MessageBase::cFIELD_DATA + 6):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "elliptical quantity";
      }
      return aMsg->ellipticalZones().GetSize();
   }
   case (rv::MessageBase::cFIELD_DATA + 7):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "spherical quantity";
      }
      return aMsg->sphericalZones().GetSize();
   }
   case (rv::MessageBase::cFIELD_DATA + 8):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "polygonal quantity";
      }
      return aMsg->polygonalZones().GetSize();
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleCommInfo(const rv::MsgCommInfo* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "comm name";
      }
      return QString::fromStdString(aMsg->commName());
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "parent platform";
      }
      return QString::fromStdString(aMsg->parentPlatform());
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "comm address";
      }
      return QString::fromStdString(aMsg->commAddress());
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "comm type";
      }
      return QString::fromStdString(aMsg->commType());
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "network name";
      }
      return QString::fromStdString(aMsg->networkName());
   }
   case (rv::MessageBase::cFIELD_DATA + 5):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "network address";
      }
      return QString::fromStdString(aMsg->networkAddress());
   }
   case (rv::MessageBase::cFIELD_DATA + 6):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "router name";
      }
      return QString::fromStdString(aMsg->routerName());
   }
   case (rv::MessageBase::cFIELD_DATA + 7):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "gateway address";
      }
      return QString::fromStdString(aMsg->gatewayAddress());
   }
   case (rv::MessageBase::cFIELD_DATA + 8):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "gateway comm";
      }
      return QString::fromStdString(aMsg->gatewayCommName());
   }
   case (rv::MessageBase::cFIELD_DATA + 9):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "gateway platform";
      }
      return QString::fromStdString(aMsg->gatewayPlatformName());
   }
   }
   // Limit to a maximum of 5 links or link addresses
   if (aColumn >= rv::MessageBase::cFIELD_DATA + 10 && aColumn <= rv::MessageBase::cFIELD_DATA + 14)
   {
      int  index           = aColumn - (rv::MessageBase::cFIELD_DATA + 10);
      auto linksList       = aMsg->links();
      auto linkAddressList = aMsg->linkAddresses();
      // prioritize links first, then  add addresses if there are any and there is room to display them
      if (index >= 0 && index < static_cast<int>(linksList.size()))
      {
         if (aRole == Qt::ToolTipRole)
         {
            return "link " + QString::number(index + 1) + "platform/comm";
         }
         return QString::fromStdString(linksList[index].platformName()) + "/" +
                QString::fromStdString(linksList[index].commName());
      }
      else
      {
         index = index - linksList.size();
         if (index >= 0 && index < static_cast<int>(linkAddressList.size()))
         {
            if (aRole == Qt::ToolTipRole)
            {
               return "link address";
            }
            return QString::fromStdString(linkAddressList[index]);
         }
      }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleNetworkInfo(const rv::MsgNetworkInfo* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "network name";
      }
      return QString::fromStdString(aMsg->networkName());
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "network address";
      }
      return QString::fromStdString(aMsg->networkAddress());
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "network type";
      }
      return QString::fromStdString(aMsg->networkType());
   }
   }
   return QVariant();
}

QVariant RvStatistics::EventTableModel::HandleRouterInfo(const rv::MsgRouterInfo* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "router name";
      }
      return QString::fromStdString(aMsg->routerName());
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "router type";
      }
      return QString::fromStdString(aMsg->routerType());
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "parent platform";
      }
      return QString::fromStdString(aMsg->parentPlatform());
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "comm name";
      }
      return QString::fromStdString(aMsg->commName());
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "comm address";
      }
      return QString::fromStdString(aMsg->commAddress());
   }
   case (rv::MessageBase::cFIELD_DATA + 5):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "gateway address";
      }
      return QString::fromStdString(aMsg->gatewayAddress());
   }
   case (rv::MessageBase::cFIELD_DATA + 6):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "gateway comm name";
      }
      return QString::fromStdString(aMsg->gatewayCommName());
   }
   case (rv::MessageBase::cFIELD_DATA + 7):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "hop limit";
      }
      return aMsg->hopLimit();
   }
   case (rv::MessageBase::cFIELD_DATA + 8):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "use default protocol";
      }
      return aMsg->useDefaultProtocol();
   }
   case (rv::MessageBase::cFIELD_DATA + 9):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "use multicast protocol";
      }
      return aMsg->useMulticastProtocol();
   }
   }
   return QVariant();
}
