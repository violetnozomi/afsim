// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "PerformanceTestThread.hpp"

#include "MoverCreatorWidget.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorEnvironment.hpp"
#include "UtLog.hpp"
#include <QFile>

namespace Designer
{

void PerformanceTestThread::run()
{
   mProcessNumber = 0;
   mClean         = true;

   qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
   mAFSIMProcesses.clear();

   // Create a MissionProcess for each test file
   std::transform(mTestFiles.begin(), mTestFiles.end(), std::back_inserter(mAFSIMProcesses), [](const QFileInfo& file) 
   {
      return std::make_shared<MissionProcess>(MoverCreatorEnvironment::GetEnvironment().GetAFSIM_Dir(), file); 
   });

   for (const auto & process : mAFSIMProcesses)
   {
      connect(process.get(), static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &PerformanceTestThread::HandleProcessFinished);
      process->Start();
   }
   exec();
}


void PerformanceTestThread::HandleProcessFinished(int aExitCode, QProcess::ExitStatus aExitStatus)
{
   ++mProcessNumber;

   if (aExitCode != 0)
   {
      mClean = false;
      exit(aExitCode);
   }

   if (mProcessNumber == mTestFiles.size())
   {
      quit();
   }
}

MissionProcess::MissionProcess(const QString& aCommand, const QFileInfo& aTestFile)
   : QProcess()
   , mCommand(aCommand)
   , mTestFile(aTestFile)
{
}

void MissionProcess::Start()
{
   start(mCommand, QStringList(mTestFile.absoluteFilePath()));
}

} // namespace Designer
