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

#ifndef RVEVENTTABLEMODEL_HPP
#define RVEVENTTABLEMODEL_HPP

#include <QAbstractTableModel>


namespace rv
{
class MsgAuxData;
class MsgBase;
class MessageBaseArray;
class MsgEntityState;
class MsgExecData;
class MsgPlatformInfo;
class MsgLocalTrackCreated;
class MsgLocalTrackUpdate;
class MsgLocalTrackDrop;
class MsgLocalTrackCorrelation;
class MsgLocalTrackDecorrelation;
class MsgSensorModeChange;
class MsgPlatformStatus;
class MsgPartStatus;
class MsgPlatformAppearanceChanged;
class MsgComment;
class MsgDetectAttempt;
class MsgDrawCommand;
class MsgMessageReceived;
class MsgMessageTransmitted;
class MsgMessageHop;
class MsgScenarioData;
class MsgScriptData;
class MsgSensorDetectionChange;
class MsgSensorTrackCreated;
class MsgSensorTrackUpdate;
class MsgSensorTrackDrop;
class MsgEmitterModeDefinition;
class MsgPartArticulationStatus;
class MsgVisualPartDefinition;
class MsgWeaponTerminated;
class MsgWeaponFired;
class MsgWeaponModeChange;
class MsgWeaponQuantityChange;
class MsgTaskUpdate;
class MsgQuantumTaskerUpdate;
class MsgJammingRequestInitiated;
class MsgJammingRequestCanceled;
class MsgJammingRequestUpdated;
class MsgSetDate;
class MsgResource;
class MsgBookmark;
class MsgRouteChanged;
class MsgCircularZone;
class MsgEllipticalZone;
class MsgSphericalZone;
class MsgPolygonalZone;
class MsgZoneSet;
class MsgCommInfo;
class MsgNetworkInfo;
class MsgRouterInfo;
class ResultDb;
} // namespace rv
namespace RvStatistics
{
class EventTableModel : public QAbstractTableModel
{
public:
   explicit EventTableModel(QObject* aParent);

   ~EventTableModel() override = default;

   void SetData(rv::MessageBaseArray& aPagedData, rv::MessageBaseArray& aMaintainedData, rv::ResultDb* aDbPtr);

   void Clear();

   int      rowCount(const QModelIndex& aIndex) const override;
   int      columnCount(const QModelIndex& aIndex) const override;
   QVariant data(const QModelIndex& aIndex, int aRole) const override;
   QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
   QString LookupSimName(int aIndex) const;

   QVariant HandlePartType(const unsigned int aEnum) const;
   QVariant HandleAuxData(const rv::MsgAuxData* aMsg, int aColumn, int aRole) const;
   QVariant HandleEntityState(const rv::MsgEntityState* aMsg, int aColumn, int aRole) const;
   QVariant HandlePlatformInfo(const rv::MsgPlatformInfo* aMsg, int aColumn, int aRole) const;
   QVariant HandleLocalTrackCreated(const rv::MsgLocalTrackCreated* aMsg, int aColumn, int aRole) const;
   QVariant HandleLocalTrackUpdate(const rv::MsgLocalTrackUpdate* aMsg, int aColumn, int aRole) const;
   QVariant HandleLocalTrackDrop(const rv::MsgLocalTrackDrop* aMsg, int aColumn, int aRole) const;
   QVariant HandleLocalTrackCorrelation(const rv::MsgLocalTrackCorrelation* aMsg, int aColumn, int aRole) const;
   QVariant HandleLocalTrackDecorrelation(const rv::MsgLocalTrackDecorrelation* aMsg, int aColumn, int aRole) const;
   QVariant HandleSensorModeChange(const rv::MsgSensorModeChange* aMsg, int aColumn, int aRole) const;
   QVariant HandlePlatformStatus(const rv::MsgPlatformStatus* aMsg, int aColumn, int aRole) const;
   QVariant HandlePlatformAppearanceChanged(const rv::MsgPlatformAppearanceChanged* aMsg, int aColumn, int aRole) const;
   QVariant HandlePartStatus(const rv::MsgPartStatus* aMsg, int aColumn, int aRole) const;
   QVariant HandleComment(const rv::MsgComment* aMsg, int aColumn, int aRole) const;
   QVariant HandleDetectAttempt(const rv::MsgDetectAttempt* aMsg, int aColumn, int aRole) const;
   QVariant HandleDrawCommand(const rv::MsgDrawCommand* aMsg, size_t aColumn, int aRole) const;
   QVariant HandleMessageReceived(const rv::MsgMessageReceived* aMsg, int aColumn, int aRole) const;
   QVariant HandleMessageHop(const rv::MsgMessageHop* aMsg, int aColumn, int aRole) const;
   QVariant HandleMessageTransmitted(const rv::MsgMessageTransmitted* aMsg, int aColumn, int aRole) const;
   QVariant HandleScenarioData(const rv::MsgScenarioData* aMsg, int aColumn, int aRole) const;
   QVariant HandleScriptData(const rv::MsgScriptData* aMsg, int aColumn, int aRole) const;
   QVariant HandleSensorDetectionChange(const rv::MsgSensorDetectionChange* aMsg, int aColumn, int aRole) const;
   QVariant HandleSensorTrackCreated(const rv::MsgSensorTrackCreated* aMsg, int aColumn, int aRole) const;
   QVariant HandleSensorTrackUpdate(const rv::MsgSensorTrackUpdate* aMsg, int aColumn, int aRole) const;
   QVariant HandleSensorTrackDrop(const rv::MsgSensorTrackDrop* aMsg, int aColumn, int aRole) const;
   QVariant HandleEmitterModeDefinition(const rv::MsgEmitterModeDefinition* aMsg, size_t aColumn, int aRole) const;
   QVariant HandlePartArticulationStatus(const rv::MsgPartArticulationStatus* aMsg, int aColumn, int aRole) const;
   QVariant HandleVisualPartDefinition(const rv::MsgVisualPartDefinition* aMsg, int aColumn, int aRole) const;
   QVariant HandleExecData(const rv::MsgExecData* aMsg, int aColumn, int aRole) const;
   QVariant HandleWeaponTerminated(const rv::MsgWeaponTerminated* aMsg, int aColumn, int aRole) const;
   QVariant HandleWeaponFired(const rv::MsgWeaponFired* aMsg, int aColumn, int aRole) const;
   QVariant HandleWeaponModeChange(const rv::MsgWeaponModeChange* aMsg, int aColumn, int aRole) const;
   QVariant HandleWeaponQuantityChange(const rv::MsgWeaponQuantityChange* aMsg, int aColumn, int aRole) const;
   QVariant HandleTaskUpdate(const rv::MsgTaskUpdate* aMsg, int aColumn, int aRole) const;
   QVariant HandleQuantumTaskerUpdate(const rv::MsgQuantumTaskerUpdate* aMsg, int aColumn, int aRole) const;
   QVariant HandleJammingRequestInitiated(const rv::MsgJammingRequestInitiated* aMsg, int aColumn, int aRole) const;
   QVariant HandleJammingRequestCanceled(const rv::MsgJammingRequestCanceled* aMsg, int aColumn, int aRole) const;
   QVariant HandleJammingRequestUpdated(const rv::MsgJammingRequestUpdated* aMsg, int aColumn, int aRole) const;
   QVariant HandleSetDate(const rv::MsgSetDate* aMsg, int aColumn, int aRole) const;
   QVariant HandleResource(const rv::MsgResource* aMsg, int aColumn, int aRole) const;
   QVariant HandleBookmark(const rv::MsgBookmark* aMsg, int aColumn, int aRole) const;
   QVariant HandleRouteChanged(const rv::MsgRouteChanged* aMsg, int aColumn, int aRole) const;
   QVariant HandleCircularZone(const rv::MsgCircularZone* aMsg, int aColumn, int aRole) const;
   QVariant HandleEllipticalZone(const rv::MsgEllipticalZone* aMsg, int aColumn, int aRole) const;
   QVariant HandleSphericalZone(const rv::MsgSphericalZone* aMsg, int aColumn, int aRole) const;
   QVariant HandlePolygonalZone(const rv::MsgPolygonalZone* aMsg, int aColumn, int aRole) const;
   QVariant HandleZoneSet(const rv::MsgZoneSet* aMsg, int aColumn, int aRole) const;
   QVariant HandleCommInfo(const rv::MsgCommInfo* aMsg, int aColumn, int aRole) const;
   QVariant HandleNetworkInfo(const rv::MsgNetworkInfo* aMsg, int aColumn, int aRole) const;
   QVariant HandleRouterInfo(const rv::MsgRouterInfo* aMsg, int aColumn, int aRole) const;

   std::vector<rv::MsgBase*> mData;
   std::map<int, QString>    mSimNames;
   rv::ResultDb*             mDbPtr;
};
} // namespace RvStatistics
#endif
