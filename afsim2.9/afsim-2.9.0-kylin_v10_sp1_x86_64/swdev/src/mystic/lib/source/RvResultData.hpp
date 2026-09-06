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

#ifndef RVRESULTDATA_HPP
#define RVRESULTDATA_HPP

#include <QFileInfo>
#include <QObject>
#include <QTimer>

#include "RvExport.hpp"
#include "RvWsfPipe_FileStreamer.hpp"
#include "UtWallClock.hpp"

namespace rv
{
class ResultDb;
class ResultPlatform;

class RV_EXPORT ResultData : public QObject
{
   Q_OBJECT
public:
   ResultData(const QFileInfo& aDataFileInfo, const unsigned int aPageLimit);
   ~ResultData() override;

   ResultPlatform* FindPlatform(int aIndex) const;
   int             FindPlatformIndex(const std::string& aName) const;

   float                   GetSimTime() const { return mCurrentTime; }
   float                   GetEndTime() const;
   float                   GetClockRate() const { return mRate; }
   std::pair<float, float> GetCacheRange() const;


   void SetTime(float aTime);
   void SetRate(float aRate) { mRate = aRate; }
   void Pause();
   void Resume();
   void Reverse();
   void SetLooped(bool aState);
   void SetExtrapolateAtEnd(bool aState);
   void timerEvent(QTimerEvent* aEvent) override;

   void HaltTimeChanges();
   void ResumeTimeChanges();

   ResultDb* GetDb() const { return mDb; }
   QString   GetFilePath() const { return mFileInfo.path(); }

   std::string            DetermineFileType(const QFileInfo& aFileInfo);
   RvWsfPipe::StreamState GetStreamState();

private:
   void LoadDb(const std::string& aFileName, const unsigned int aPageLimit);
   bool LockReplayFile(const std::string& aFileName);

   QString GetDisplayName(const QFileInfo& aFile, bool aDisplayFullPath);

   QFileInfo    mFileInfo;
   ResultDb*    mDb;
   QBasicTimer* mTimer;
   float        mCurrentTime;
   float        mRate;
   UtWallClock  mClock;
   bool         mLooped;
   bool         mExtrapolateAtEnd;
   bool         mReverse;
   bool         mTimeChangesEnabled;
};
} // namespace rv
#endif
