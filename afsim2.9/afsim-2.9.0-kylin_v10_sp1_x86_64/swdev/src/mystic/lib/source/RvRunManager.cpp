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

#include "RvRunManager.hpp"

#include <cassert>
#include <iostream>
#include <sstream>

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>
#include <QStatusBar>
#include <QTextDocument>

#include "RvEnvironment.hpp"
#include "UtInput.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfQueueableMessageObject.hpp"

rv::RunManager* rv::RunManager::mInstancePtr = nullptr;

rv::RunManager::RunManager()
   : mMaxRecentRecordings(12)
   , mStartPaused(false)
{
   mInstancePtr = this;
   ReadSettings();
}

rv::RunManager::~RunManager()
{
   mInstancePtr = nullptr;
}

rv::RunManager& rv::RunManager::GetInstance()
{
   assert(mInstancePtr);
   return *mInstancePtr;
}

void rv::RunManager::Create()
{
   assert(!mInstancePtr);
   if (!mInstancePtr)
   {
      new RunManager();
   }
}

void rv::RunManager::RemoveRecentRecording(const QString& aFile)
{
   QString recording = StandardizeFilePath(aFile);
   if (mRecentRecordings.removeAll(recording) > 0)
   {
      WriteHistory();
   }
}

void rv::RunManager::ClearRecentRecordings()
{
   mRecentRecordings.clear();
   // Clear the "Open Recent" menu
   rvEnv.PopulateRecents();
   WriteHistory();
}

QString rv::RunManager::GetMostRecentDirectory()
{
   QString retVal;
   if (!mRecentRecordings.empty())
   {
      QFileInfo info(mRecentRecordings.front());
      if (info.exists())
      {
         retVal = info.absolutePath();
      }
   }
   return retVal;
}

void rv::RunManager::Shutdown()
{
   if (mInstancePtr)
   {
      delete mInstancePtr;
   }
}

QString rv::RunManager::GetRecordingDisplayName(const QString& aFile, bool aDisplayFullPath)
{
   QString text;

   QFileInfo fi(aFile);
   text = aDisplayFullPath ? fi.absoluteFilePath() : fi.fileName();

   return text;
}

void rv::RunManager::ReadSettings()
{
   QSettings& settings = wkfEnv.GetAppDataSettingsFile();

   settings.beginGroup("RunManager");

   int size = settings.beginReadArray("History");
   for (int i = 0; i < size; ++i)
   {
      settings.setArrayIndex(i);
      QString recording = settings.value("recording").toString();
      if (!recording.isEmpty())
      {
         mRecentRecordings.push_back(recording);
      }
   }
   while (mRecentRecordings.size() > mMaxRecentRecordings)
   {
      mRecentRecordings.removeLast();
   }
   settings.endArray();
   settings.endGroup();
}

void rv::RunManager::WriteHistory()
{
   int        count    = 0;
   QSettings& settings = wkfEnv.GetAppDataSettingsFile();

   settings.beginGroup("RunManager");
   settings.remove("History");
   settings.beginWriteArray("History", mRecentRecordings.size());
   for (const QString& recording : mRecentRecordings)
   {
      settings.setArrayIndex(count);
      settings.setValue("recording", recording);
      count++;
   }
   settings.endArray();
   settings.endGroup();
}

void rv::RunManager::SetMostRecentRecording(const QString& aFile)
{
   // Remove it from the list of recent scenarios, if it was already in the list.
   RemoveRecentRecording(aFile);

   // Insert at the beginning of the list
   bool    fileExists;
   QString recording = StandardizeFilePath(aFile, &fileExists);
   if (fileExists)
   {
      mRecentRecordings.push_front(recording);
      if (mRecentRecordings.size() > mMaxRecentRecordings)
      {
         mRecentRecordings.removeLast();
      }
      WriteHistory();
   }
}

QString rv::RunManager::StandardizeFilePath(const QString& aFile, bool* aFileExists)
{
   // Standardize all file paths through QFileInfo for exact matching
   QString file = aFile;
   if (aFileExists)
   {
      *aFileExists = true;
   }
   QFileInfo fi(file);
   if (!fi.exists())
   {
      if (aFileExists)
      {
         *aFileExists = false;
      }
   }
   file = fi.absoluteFilePath();
   return file;
}
