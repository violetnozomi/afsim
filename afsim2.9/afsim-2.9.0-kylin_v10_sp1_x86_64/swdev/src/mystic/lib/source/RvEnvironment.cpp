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
#include "RvEnvironment.hpp"

#include <sstream>

#include <QAbstractButton>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "RvEventPipeClasses.hpp"
#include "RvMemoryPrefObject.hpp"
#include "RvResultData.hpp"
#include "RvResultPlatform.hpp"
#include "RvRunManager.hpp"
#include "WkfAction.hpp"
#include "WkfGeneralPrefObject.hpp"
#include "WkfKeyBindingsObject.hpp"
#include "WkfMainWindow.hpp"
#include "WkfResourceManager.hpp"
#include "WkfVtkEnvironment.hpp"

rv::Environment* rv::Environment::mInstancePtr = nullptr;

rv::Environment::Environment()
   : QObject()
   , mDataPtr(nullptr)
   , mTimer(nullptr)
   , mLastReportedTime(-1.0)
   , mWasOutside(false)
   , mFileInfo(QFileInfo())
   , mLooped(true)
   , mExtrapolateAtEnd(false)
{
   InitializeCallbacks();

   mInstancePtr = this;
   // build the data pointer here based on mFileInfo
}

rv::Environment::~Environment()
{
   if (nullptr != mDataPtr)
   {
      delete mDataPtr;
   }
   if (nullptr != mTimer)
   {
      delete mTimer;
   }

   mInstancePtr = nullptr;
}

void rv::Environment::RegisterDataExtension(std::unique_ptr<rv::DataExtension> aExtension)
{
   mDataExtensions.emplace_back(std::move(aExtension));
}

rv::Environment& rv::Environment::GetInstance()
{
   assert(mInstancePtr != nullptr);
   return *mInstancePtr;
}

void rv::Environment::Create()
{
   assert(mInstancePtr == nullptr);
   if (mInstancePtr == nullptr)
   {
      new Environment();
   }
}

void rv::Environment::Shutdown()
{
   if (mInstancePtr != nullptr)
   {
      delete mInstancePtr;
   }
}

void rv::Environment::UpdateMenus()
{
   QMenu* file = wkfEnv.GetMainWindow()->GetMenuByName("File");
   if (nullptr != file)
   {
      PopulateRecents();

      wkf::Action* open = new wkf::Action("Open...", file, QKeySequence("Ctrl+O"));
      connect(open, &QAction::triggered, this, &Environment::FileOpenSlot);
      file->insertAction(*file->actions().begin(), open);
      file->addSeparator();
      QList<wkf::Action*> list = {open};
      wkfEnv.GetPreferenceObject<wkf::KeyBindingsObject>()->AddActions(list, "Application");
   }
}

void rv::Environment::LaunchDataThread(const QFileInfo& aFileInfo)
{
   mFileInfo = aFileInfo;
   if (mDataPtr != nullptr)
   {
      delete mDataPtr;
      mLastReportedTime = -1.0;
      mWasOutside       = false;
   }
   mDataPtr = new rv::ResultData(aFileInfo, wkfEnv.GetPreferenceObject<MemoryPrefObject>()->GetPagesInCache());
   mDataPtr->SetLooped(mLooped);
   mDataPtr->SetExtrapolateAtEnd(mExtrapolateAtEnd);
   if (nullptr == mTimer)
   {
      mTimer = new QBasicTimer;
      mTimer->start(33, this);
      wkfEnv.SetFrameUpdateEnabled(true);
   }
}

void rv::Environment::timerEvent(QTimerEvent* aEvent)
{
   if (mDataPtr != nullptr)
   {
      mMutex.lock();
      bool inside = ((mDataPtr->GetSimTime() >= mDataPtr->GetCacheRange().first) &&
                     (mDataPtr->GetSimTime() <= mDataPtr->GetCacheRange().second));
      if (mDataPtr->GetSimTime() != mLastReportedTime)
      {
         emit AdvanceTimeRead(*mDataPtr);
         mLastReportedTime = mDataPtr->GetSimTime();
      }
      else if ((inside && mWasOutside))
      {
         emit AdvanceTimeRead(
            *mDataPtr); // we need to update even if we set the correct time earlier if we didn't have data available.
      }
      else if (mRegularReadTimer.GetClock() > 0.5)
      {
         emit RegularRead(*mDataPtr);
         mRegularReadTimer.ResetClock();
      }
      mWasOutside = !inside;
      mMutex.unlock();
      emit                   AdvanceTimeWrite(*mDataPtr); // this is really just for time controls
      RvWsfPipe::StreamState ss = mDataPtr->GetStreamState();
      if (ss != RvWsfPipe::cOK)
      {
         float rate = mDataPtr->GetClockRate();
         float time = mDataPtr->GetSimTime();
         if (ss == RvWsfPipe::cRESET_REQUIRED)
         {
            QMessageBox::warning(wkfEnv.GetMainWindow(),
                                 "Reset required",
                                 "The recording file has changed, a reload will be performed.");
            emit ClearScenario(false);
            LaunchDataThread(mFileInfo);
            mDataPtr->SetRate(rate);
            mDataPtr->SetTime(time);
         }
         else if (ss == RvWsfPipe::cTIME_TYPE_MISMATCH)
         {
            QString msg =
               QString("The recording's schema is incompatible with Mystic's schema due to a time-type mismatch.<br>") +
               "Please run your scenario with a version of an AFSIM simulation that matches Mystic's version.";

            QMessageBox::warning(wkfEnv.GetMainWindow(), "Failed to load!", msg, QMessageBox::Abort);

            delete mDataPtr;
            mDataPtr          = nullptr;
            mLastReportedTime = -1.0;
            mWasOutside       = false;
            emit ClearScenario(true);
         }
         else
         {
            int retval = QMessageBox::warning(
               wkfEnv.GetMainWindow(),
               "Reset maybe required",
               "An error was encountered when reading the file, select Yes to reload, or Abort to close the file.",
               QMessageBox::Yes,
               QMessageBox::Abort);
            if (retval == QMessageBox::Yes)
            {
               emit ClearScenario(false);
               LaunchDataThread(mFileInfo);
               mDataPtr->SetRate(rate);
               mDataPtr->SetTime(time);
            }
            else
            {
               delete mDataPtr;
               mDataPtr          = nullptr;
               mLastReportedTime = -1.0;
               mWasOutside       = false;
               emit ClearScenario(true);
            }
         }
         // get the time and rate from the previous session.
      }
   }
}

// This is needed for the queued connection in InitializeCallbacks.
Q_DECLARE_METATYPE(rv::MsgExecData);

void rv::Environment::InitializeCallbacks()
{
   connect(
      this,
      &rv::Environment::ClearScenario,
      this,
      [this](bool aDummyArg)
      {
         auto prefs = wkfEnv.GetPreferenceObject<wkf::GeneralPrefObject>();
         if (prefs)
         {
            prefs->SetSimulationName(std::string());
         }
      },
      Qt::QueuedConnection);

   connect(
      this,
      &rv::Environment::MetaDataRead,
      this,
      [this](const rv::MsgExecData& aData)
      {
         auto prefs = wkfEnv.GetPreferenceObject<wkf::GeneralPrefObject>();
         if (prefs)
         {
            std::string newName;
            if (aData.simIndex() == 0)
            {
               newName = aData.simName() + prefs->GetSimulationName();
            }
            else
            {
               newName = prefs->GetSimulationName() + " | " + aData.simName();
            }
            prefs->SetSimulationName(std::move(newName));
         }
      },
      Qt::QueuedConnection); // Queued connection because slot needs to run on the gui thread.

   /*   connect(mDataPtr, &rv::ResultData::PlatformAdded, this, &Environment::PlatformAddedSlot);
   connect(mDataPtr, &rv::ResultData::TimeInCacheChanged, this, &Environment::TimeInCacheChangedSlot);
   connect(mDataPtr, rv::ResultData::MaxTimeChanged, this, &Environment::MaxTimeChangedSlot);
   connect(mDataPtr, SIGNAL(AdvanceTime(double)), this, SLOT(AdvanceTimeRead(double)));
   connect(mDataPtr, SIGNAL(Pausing()), this, SLOT(Pausing()));
   connect(mDataPtr, SIGNAL(Resuming()), this, SLOT(Resuming()));
   connect(mDataPtr, SIGNAL(ClockRateChange(double)), this, SLOT(ClockRateChange(double)));*/
}

void rv::Environment::SetTime(float aTime)
{
   if (nullptr != mDataPtr)
   {
      mDataPtr->SetTime(aTime);
   }
}

void rv::Environment::SetRate(float aRate)
{
   if (nullptr != mDataPtr)
   {
      mDataPtr->SetRate(aRate);
   }
}

void rv::Environment::SetLooped(bool aState)
{
   mLooped = aState;
   if (nullptr != mDataPtr)
   {
      mDataPtr->SetLooped(aState);
   }
}

void rv::Environment::SetExtrapolateAtEnd(bool aState)
{
   mExtrapolateAtEnd = aState;
   if (nullptr != mDataPtr)
   {
      mDataPtr->SetExtrapolateAtEnd(aState);
   }
}

void rv::Environment::SendPauseCommand()
{
   if (nullptr != mDataPtr)
   {
      mDataPtr->Pause();
      PlayStatusUpdate(PlaybackStatus::cPAUSE);
   }
}

void rv::Environment::SendResumeCommand()
{
   if (nullptr != mDataPtr)
   {
      mDataPtr->Resume();
      PlayStatusUpdate(PlaybackStatus::cPLAY);
   }
}

void rv::Environment::SendRestartCommmand()
{
   if (nullptr != mDataPtr)
   {
      mDataPtr->SetTime(0.0f);
      PlayStatusUpdate(PlaybackStatus::cRESTART);
   }
}

void rv::Environment::SendReverseCommand()
{
   if (nullptr != mDataPtr)
   {
      mDataPtr->Reverse();
      PlayStatusUpdate(PlaybackStatus::cREVERSE);
   }
}

void rv::Environment::AddPlatform(rv::ResultDb& aResultData, const rv::MsgPlatformInfo& aPlatform)
{
   mMutex.lock();
   emit PlatformAddedRead(mDataPtr->GetSimTime(), aResultData, aPlatform);
   mMutex.unlock();
}

void rv::Environment::AddComment(ResultDb& aResultData, const MsgComment& aComment)
{
   mMutex.lock();
   emit CommentRead(aResultData, aComment);
   mMutex.unlock();
}

void rv::Environment::AddNetwork(ResultDb& aResultData, const MsgNetworkInfo& aNetworkInfo)
{
   mMutex.lock();
   emit NetworkInfoRead(aResultData, aNetworkInfo);
   mMutex.unlock();
}

void rv::Environment::AddComm(ResultDb& aResultData, const MsgCommInfo& aCommInfo)
{
   mMutex.lock();
   emit CommInfoRead(aResultData, aCommInfo);
   mMutex.unlock();
}

void rv::Environment::AddRouter(ResultDb& aResultData, const MsgRouterInfo& aRouterInfo)
{
   mMutex.lock();
   emit RouterInfoRead(aResultData, aRouterInfo);
   mMutex.unlock();
}

void rv::Environment::AddDrawMessage(ResultDb& aResultData, const MsgDrawCommand& aDrawCommand)
{
   mMutex.lock();
   emit DrawCommandRead(aResultData, aDrawCommand);
   mMutex.unlock();
}

void rv::Environment::AddMetaData(const MsgExecData& aMetaData)
{
   mMutex.lock();
   emit MetaDataRead(aMetaData);
   mMutex.unlock();
}

void rv::Environment::AddSensorModeDefinition(const MsgEmitterModeDefinition& aMode)
{
   mMutex.lock();
   emit SensorModeDefinition(aMode);
   mMutex.unlock();
}

void rv::Environment::AddWeaponModeDefinition(const MsgEmitterModeDefinition& aMode)
{
   mMutex.lock();
   emit WeaponModeDefinition(aMode);
   mMutex.unlock();
}

void rv::Environment::AddWeaponFiredEvent(const MsgWeaponFired& aEvent)
{
   mMutex.lock();
   emit WeaponFiredRead(aEvent);
   mMutex.unlock();
}

void rv::Environment::AddWeaponTerminatedEvent(const MsgWeaponTerminated& aEvent)
{
   mMutex.lock();
   emit WeaponTerminatedRead(aEvent);
   mMutex.unlock();
}

void rv::Environment::AddPlatformStatusEvent(const MsgPlatformStatus& aEvent)
{
   mMutex.lock();
   emit PlatformStatusRead(aEvent);
   mMutex.unlock();
}

void rv::Environment::SetDate(const rv::MsgSetDate& aEvent)
{
   mMutex.lock();
   int second = aEvent.time();
   int hour   = second / 3600;
   second -= hour * 3600;
   int minute = second / 60;
   second -= minute * 60;
   mEpoch.SetDate(aEvent.year(), aEvent.month(), aEvent.day());
   mEpoch.SetTime(aEvent.time());
   mEpoch.AdvanceTimeBy(-aEvent.simTime());
   emit SetEpoch(mEpoch);
   vaEnv.SetStartTime(mEpoch);
   mMutex.unlock();
}

void rv::Environment::AddResource(const MsgResource& aResource)
{
   if (aResource.resourceType() == MsgResource_ResourceType::dted)
   {
      for (auto&& path : aResource.resourceLocation())
      {
         std::stringstream ss(path);
         int               level;
         std::string       pathStr;
         ss >> level;
         size_t start = path.find_first_of('\"');
         size_t end   = path.find_last_of('\"');
         pathStr      = path.substr(start + 1, end - start - 1);

         wkfEnv.GetResourceManager().AddDTED_Source(QString::fromStdString(pathStr), level);
      }
   }
   else if (aResource.resourceType() == MsgResource_ResourceType::raw_dted)
   {
      for (auto&& path : aResource.resourceLocation())
      {
         std::stringstream ss(path);
         int               level;
         std::string       pathStr;
         ss >> level;
         int south;
         ss >> south;
         int west;
         ss >> west;
         int north;
         ss >> north;
         int east;
         ss >> east;
         size_t start = path.find_first_of('\"');
         size_t end   = path.find_last_of('\"');
         pathStr      = path.substr(start + 1, end - start - 1);

         wkfEnv.GetResourceManager().AddRawDTED_Source(QString::fromStdString(pathStr), level, south, west, north, east);
      }
   }
   else if (aResource.resourceType() == MsgResource_ResourceType::geotiff_dem)
   {
      for (auto&& path : aResource.resourceLocation())
      {
         wkfEnv.GetResourceManager().AddGeotiffSource(QString::fromStdString(path));
      }
   }
}

void rv::Environment::AddBookmark(const MsgBookmark& aBookmark)
{
   mMutex.lock();
   emit BookmarkRead(aBookmark);
   mMutex.unlock();
}

void rv::Environment::AddZone(MsgBase& aMessage)
{
   mMutex.lock();
   emit ZoneRead(aMessage);
   mMutex.unlock();
}

void rv::Environment::AddAnnotationMessage(rv::MsgBase& aMessage)
{
   emit AnnotationRead(aMessage);
}

const UtCalendar& rv::Environment::GetEpoch()
{
   return mEpoch;
}

void rv::Environment::FileOpenSlot()
{
   QAction* sender = dynamic_cast<QAction*>(QObject::sender());
   if ((nullptr == sender) || (sender->text() == "Open..."))
   {
      QString fullFilePath = QFileDialog::getOpenFileName(wkfEnv.GetMainWindow(),
                                                          "Select File",
                                                          "",
                                                          "AFSIM EVENT RECORDING(*.aer);; All Files(*.*)");
      if (fullFilePath != "")
      {
         OpenEventRecording(fullFilePath);
      }
   }
   else
   {
      OpenEventRecording(sender->text());
   }
}

void rv::Environment::OpenEventRecording(const QString& aFilename)
{
   QFileInfo fileInfo = QFileInfo(aFilename);
   QString   filename = fileInfo.absoluteFilePath();
   if (!filename.endsWith(".aer", Qt::CaseInsensitive))
   {
      QMessageBox::warning(wkfEnv.GetMainWindow(),
                           "Invalid file extension",
                           filename + " is not an AFSIM event recording (.aer) file.");
      return;
   }
   else if (!fileInfo.exists())
   {
      QList<QString> rlist = RunManager::GetInstance().GetRecentRecordings();
      if (rlist.contains(filename))
      {
         QMessageBox::StandardButton button =
            QMessageBox::warning(nullptr,
                                 "File not found",
                                 QString("File %1\ndoes not exist. Remove from recent list?").arg(filename),
                                 QMessageBox::Yes | QMessageBox::No);
         if (button == QMessageBox::Yes)
         {
            RunManager::GetInstance().RemoveRecentRecording(filename);
            PopulateRecents();
         }
         return;
      }
      else
      {
         QMessageBox::warning(wkfEnv.GetMainWindow(), "File not found", "Could not find file: " + filename);
      }
      return;
   }
   emit ClearScenario(true);
   LaunchDataThread(fileInfo);
   QDir::setCurrent(fileInfo.dir().absolutePath());

   RunManager::GetInstance().SetMostRecentRecording(filename);
   PopulateRecents();
}

void rv::Environment::PopulateRecents()
{
   QMenu* file = wkfEnv.GetMainWindow()->GetMenuByName("File");

   if (file)
   {
      QMenu* recent = file->findChild<QMenu*>("Open Recent");
      if (!recent)
      {
         recent = new QMenu("Open Recent", file);
         recent->setObjectName("Open Recent");
         file->insertMenu(*file->actions().begin(), recent);
      }
      else
      {
         recent->clear();
      }

      QList<QString> rlist = RunManager::GetInstance().GetRecentRecordings();
      for (auto&& it : rlist)
      {
         QAction* recentAct = new QAction(it, recent);
         connect(recentAct, &QAction::triggered, this, &Environment::FileOpenSlot);
         recent->addAction(recentAct);
      }
      if (rlist.isEmpty())
      {
         auto* action = recent->addAction("(no recent replays)");
         action->setEnabled(false);
      }
   }
}

QString rv::Environment::LookupPlatformName(int aIndex) const
{
   if (mDataPtr != nullptr)
   {
      rv::ResultPlatform* plat = mDataPtr->FindPlatform(aIndex);
      if (plat != nullptr)
      {
         return QString::fromStdString(plat->GetName());
      }
   }
   return "???";
}

QVariant rv::Environment::LookupPlatformVariant(const unsigned int aIdx, int aRole, bool aValid) const
{
   if (aValid)
   {
      if (aRole == Qt::UserRole)
      {
         return aIdx;
      }
      else
      {
#ifdef _DEBUG
         return LookupPlatformName(aIdx) + " (" + QString::number(aIdx) + ")";
#else
         return LookupPlatformName(aIdx);
#endif
      }
   }
   return QVariant();
}
