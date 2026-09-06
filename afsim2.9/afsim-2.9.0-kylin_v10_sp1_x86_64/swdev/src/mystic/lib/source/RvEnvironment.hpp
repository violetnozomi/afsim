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
#ifndef RVENVIRONMENT_HPP
#define RVENVIRONMENT_HPP

class QBasicTimer;
class QFileInfo;
#include <QMutex>

#include "RvDataExtension.hpp"
#include "UtCalendar.hpp"
#include "UtCallbackHolder.hpp"
#include "UtWallClock.hpp"
#include "WkfEnvironment.hpp"
class WsfPlatform;
#include "RvExport.hpp"

#if defined(rvEnv)
#undef rvEnv
#endif
#define rvEnv static_cast<rv::Environment&>(rv::Environment::GetInstance())

namespace rv
{
class MsgBase;
class MsgComment;
class MsgCommInfo;
class MsgNetworkInfo;
class MsgRouterInfo;
class MsgDrawCommand;
class MsgExecData;
class MsgPlatformInfo;
class MsgPlatformStatus;
class MsgEmitterModeDefinition;
class MsgWeaponFired;
class MsgWeaponTerminated;
class MsgSetDate;
class MsgResource;
class ResultData;
class ResultDb;
class MsgBookmark;

enum class PlaybackStatus
{
   cPAUSE,
   cPLAY,
   cREVERSE,
   cRESTART
};

class RV_EXPORT Environment : public QObject
{
   Q_OBJECT

public:
   Environment();
   ~Environment() override;

   static Environment& GetInstance();

   static void Create();
   static bool Exists() { return mInstancePtr != nullptr; }

   void StartUp();
   void Shutdown();
   void LaunchDataThread(const QFileInfo& aFileInfo);

   void SetTime(float aTime); // time updated from widget
   void SetRate(float aRate);
   void SetLooped(bool aState);
   void SetExtrapolateAtEnd(bool aState);
   void SendPauseCommand();
   void SendResumeCommand();
   void SendReverseCommand();
   void SendRestartCommmand();

   void timerEvent(QTimerEvent* aEvent) override;

   void AddPlatform(ResultDb& aResultData, const MsgPlatformInfo& aPlatform);
   void AddComment(ResultDb& aResultData, const MsgComment& aComment);
   void AddComm(ResultDb& aResultData, const MsgCommInfo& aCommInfo);
   void AddNetwork(ResultDb& aResultData, const MsgNetworkInfo& aNetworkInfo);
   void AddRouter(ResultDb& aResultData, const MsgRouterInfo& aRouterInfo);
   void AddDrawMessage(ResultDb& aResultData, const MsgDrawCommand& aDrawCommand);
   void AddMetaData(const MsgExecData& aMetaData);
   void AddSensorModeDefinition(const MsgEmitterModeDefinition& aMode);
   void AddWeaponModeDefinition(const MsgEmitterModeDefinition& aMode);
   void AddWeaponFiredEvent(const MsgWeaponFired& aEvent);
   void AddWeaponTerminatedEvent(const MsgWeaponTerminated& aEvent);
   void AddPlatformStatusEvent(const MsgPlatformStatus& aEvent);
   void SetDate(const MsgSetDate& aEvent);
   void AddResource(const MsgResource& aResource);
   void AddAnnotationMessage(MsgBase& aMessage);
   void AddBookmark(const MsgBookmark& aBookmark);
   void AddZone(MsgBase& aMessage);

   ResultData* GetData() { return mDataPtr; }
   void        UpdateMenus();
   void        OpenEventRecording(const QString& aFilename);

   void              PopulateRecents();
   const UtCalendar& GetEpoch();

   const std::vector<std::unique_ptr<DataExtension>>& GetExtensions() { return mDataExtensions; }

   void RegisterDataExtension(std::unique_ptr<DataExtension> aExtension);

   QVariant LookupPlatformVariant(const unsigned int aIdx, int aRole, bool aValid = true) const;
   QString  LookupPlatformName(int aIndex) const;

signals:
   //      void PlatformAdded(double aSimTime, const WsfPlatform& aPlatformName);
   void PlayStatusUpdate(PlaybackStatus aStatus);
   void AdvanceTimeRead(const ResultData& aResultData);
   void RegularRead(const ResultData& aResultData);
   void AdvanceTimeWrite(ResultData& aResultData);
   void SensorModeDefinition(const MsgEmitterModeDefinition& aMode);
   void WeaponModeDefinition(const MsgEmitterModeDefinition& aMode);
   void ClearScenario(bool aFullReset);
   void WeaponFiredRead(const MsgWeaponFired& aEvent);
   void WeaponTerminatedRead(const MsgWeaponTerminated& aEvent);
   void PlatformAddedRead(double aSimTime, ResultDb& aResultData, const MsgPlatformInfo& aPlatform);
   void CommentRead(ResultDb& aResultData, const MsgComment& aComment);
   void CommInfoRead(ResultDb& aResultData, const MsgCommInfo& aCommInfo);
   void NetworkInfoRead(ResultDb& aResultData, const MsgNetworkInfo& aNetworkInfo);
   void RouterInfoRead(ResultDb& aResultData, const MsgRouterInfo& aRouterInfo);
   void DrawCommandRead(ResultDb& aResultData, const MsgDrawCommand& aCommand);
   void PlatformStatusRead(const MsgPlatformStatus& aEvent);

   void AnnotationRead(MsgBase& aMessage);
   void ZoneRead(MsgBase& aMessage);
   void MetaDataRead(const MsgExecData& aMetaData);
   void SetEpoch(const UtCalendar& aEpoch);
   void BookmarkRead(const MsgBookmark& aBookmark);
   /*
         void Pausing();
         void Resuming();
         void ClockRateChange(double aClockRate);*/
private slots:
   void FileOpenSlot();

private:
   void InitializeCallbacks();

   static Environment* mInstancePtr;

   ResultData*  mDataPtr;
   QBasicTimer* mTimer;
   UtWallClock  mRegularReadTimer;
   double       mLastReportedTime;
   bool         mWasOutside;
   QFileInfo    mFileInfo;
   bool         mLooped;
   bool         mExtrapolateAtEnd;
   QMutex       mMutex;
   UtCalendar   mEpoch;

   std::vector<std::unique_ptr<DataExtension>> mDataExtensions;
};
} // namespace rv
#endif // RVENVIRONMENT_HPP
