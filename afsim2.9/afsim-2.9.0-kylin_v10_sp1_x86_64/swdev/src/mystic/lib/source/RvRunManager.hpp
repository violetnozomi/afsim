// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef RVRUNMANAGER_HPP
#define RVRUNMANAGER_HPP

#include <QList>
#include <QString>

#include "RvExport.hpp"

namespace rv
{
class RV_EXPORT RunManager
{
public:
   static RunManager& GetInstance();

   static void Create();

   const QList<QString>& GetRecentRecordings() const { return mRecentRecordings; }
   void                  RemoveRecentRecording(const QString& aFile);
   void                  ClearRecentRecordings();
   QString               GetMostRecentDirectory();

   bool ReloadRecording();

   void Shutdown();

   void SetStartPaused(bool aStartPaused) { mStartPaused = aStartPaused; }

   // For window title and recent menu actions/links
   static QString GetRecordingDisplayName(const QString& aFile, bool aDisplayFullPath);

   void SetMostRecentRecording(const QString& aFile);

protected:
   RunManager();
   virtual ~RunManager();

   void ReadSettings();
   void WriteHistory();

   static QString StandardizeFilePath(const QString& aFile, bool* aFileExists = nullptr);

   int            mMaxRecentRecordings;
   QList<QString> mRecentRecordings;

   static RunManager* mInstancePtr;

   bool mStartPaused;
};
} // namespace rv
#endif