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
#include "RvResultData.hpp"

#include <QFileInfo>

#include "RvResultDb.hpp"
#include "UtPath.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"

rv::ResultData::ResultData(const QFileInfo& aFileInfo, const unsigned int aPageLimit)
   : mFileInfo(aFileInfo)
   , mDb(nullptr)
   , mCurrentTime(0.0f)
   , mRate(1.0f)
   , mLooped(true)
   , mExtrapolateAtEnd(false)
   , mReverse(false)
   , mTimeChangesEnabled(true)
{
   mTimer = new QBasicTimer;
   // we want to hold onto a pointer to the proxy
   // and load the event-pipe file
   if (DetermineFileType(aFileInfo) == "wsf_pipe")
   {
      UtPath path = aFileInfo.absoluteFilePath().toStdString();
      // need to make the map window visible before some things are created

      //      mEventPipeDataPointer = new VtScenarioWsfResult(path);
      if (!mDb)
      {
         UtPath path(aFileInfo.filePath().toStdString());
         if (LockReplayFile(path.GetSystemPath()))
         {
            LoadDb(aFileInfo.filePath().toStdString(), aPageLimit);
            wkfEnv.GetMainWindow()->setWindowTitle(QString("[ %1 ]").arg(GetDisplayName(aFileInfo.filePath(), true)));
         }
      }
   }
}

rv::ResultData::~ResultData()
{
   delete mTimer;
   if (nullptr != mDb)
   {
      delete mDb;
   }
}

QString rv::ResultData::GetDisplayName(const QFileInfo& aFile, bool aDisplayFullPath)
{
   QString text;

   text = aDisplayFullPath ? aFile.absoluteFilePath() : aFile.fileName();

   return text;
}


bool rv::ResultData::LockReplayFile(const std::string& aFileName)
{
   /*   UtPath replayPath(aFileName);
      if (replayPath.Stat() != UtPath::cSTAT_ERROR)
      {
         UtPath replayPathBase = replayPath;
         replayPathBase.Up();
         UtPath lockPath = replayPathBase + (replayPath.GetFileName() + ".rdlock");
         mLockFileHandle = (void*)AcquireFileLock(lockPath.GetSystemPath());
         if (mLockFileHandle != cINVALID_FILE_HANDLE)
         {
            mLockSignalPath = replayPathBase + (replayPath.GetFileName() + ".signal");
            mLockFilePath = lockPath;
            mLockCheckTimer.start();
            return true;
         }
      }
      return false;*/
   // JEM we are skipping this for now, until we have a better understanding for the reasoning behind it
   return true;
}

void rv::ResultData::LoadDb(const std::string& aFileName, const unsigned int aPageLimit)
{
   mDb = new rv::ResultDb(aFileName, aPageLimit);
}

void rv::ResultData::SetTime(float aTime)
{
   mCurrentTime = aTime;
   if (nullptr != mDb)
   {
      mDb->SetCurrentTime(aTime);
   }
}

void rv::ResultData::Pause()
{
   // kill the timer
   mTimer->stop();
}

void rv::ResultData::Resume()
{
   mReverse = false;
   mClock.ResetClock();
   mTimer->stop();
   mTimer->start(33, this);
   // start the timer
}

void rv::ResultData::Reverse()
{
   mReverse = true;
   mClock.ResetClock();
   mTimer->stop();
   mTimer->start(33, this);
}

void rv::ResultData::SetLooped(bool aState)
{
   if (aState)
   {
      mExtrapolateAtEnd = false;
   }
   mLooped = aState;
}

void rv::ResultData::SetExtrapolateAtEnd(bool aState)
{
   if (aState)
   {
      mLooped = false;
   }
   mExtrapolateAtEnd = aState;
}

void rv::ResultData::timerEvent(QTimerEvent* aEvent)
{
   if (mTimeChangesEnabled)
   {
      float elapsed = mClock.GetClock();
      mClock.ResetClock();
      float revFactor = mReverse ? -1 : 1;
      mCurrentTime += mRate * revFactor * elapsed;
      if (mCurrentTime > mDb->GetMaxTime())
      {
         if (mLooped)
         {
            mCurrentTime = 0.0;
         }
         else if (mExtrapolateAtEnd)
         {
            if (mCurrentTime >= mDb->GetMaxRealTime())
            {
               mCurrentTime = mDb->GetMaxRealTime();
               mRate        = 1.0;
            }
         }
         else
         {
            mCurrentTime = mDb->GetMaxTime();
            Pause();
         }
      }
      else if (mCurrentTime < 0)
      {
         mCurrentTime = 0;
         Pause();
      }

      mDb->SetCurrentTime(mCurrentTime);
      //   wkfEnv.UpdateTime(mCurrentTime);
      // inform the plugins
   }
   mClock.ResetClock();
}

void rv::ResultData::HaltTimeChanges()
{
   mTimeChangesEnabled = false;
}

void rv::ResultData::ResumeTimeChanges()
{
   mTimeChangesEnabled = true;
}

float rv::ResultData::GetEndTime() const
{
   if (mDb != nullptr)
   {
      return mDb->GetMaxTime();
   }
   return 0.0f;
}

std::pair<float, float> rv::ResultData::GetCacheRange() const
{
   if (mDb != nullptr)
   {
      double mint;
      double maxt;
      mDb->GetTimeRangeInCache(mint, maxt);
      return std::pair<float, float>((float)mint, (float)maxt);
   }
   return std::pair<float, float>(0.0f, 0.0f);
}

rv::ResultPlatform* rv::ResultData::FindPlatform(int aIndex) const
{
   if (mDb != nullptr)
   {
      return mDb->FindPlatform(aIndex);
   }
   return nullptr;
}

// this is slower than lookup by index... only use this to find an index
// return of 0 means the name wasn't found
int rv::ResultData::FindPlatformIndex(const std::string& aName) const
{
   if (mDb != nullptr)
   {
      return mDb->FindPlatformIndexByName(aName);
   }
   return 0;
}


std::string rv::ResultData::DetermineFileType(const QFileInfo& aFileInfo)
{
   std::string fileType;
   QFile       file(aFileInfo.absoluteFilePath());
   if (file.open(QFile::ReadOnly))
   {
      const int         readLen = 100; // Needs to be >= the size of the largest header.
      const QByteArray& header  = file.read(readLen);

      std::string hdr(header.begin(), header.end());

      std::string headerString("\0\0WSF_PIPE\n", 11);

      if (hdr.size() > headerString.size() && hdr.compare(0, headerString.size(), headerString) == 0)
         return "wsf_pipe";
   }

   return fileType;
}

RvWsfPipe::StreamState rv::ResultData::GetStreamState()
{
   return mDb->GetStreamState();
}
