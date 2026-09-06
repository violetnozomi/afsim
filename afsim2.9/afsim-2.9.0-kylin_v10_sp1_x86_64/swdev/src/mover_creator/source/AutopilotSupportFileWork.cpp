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

#include "AutopilotSupportFileWork.hpp"

#include "MoverCreatorWidget.hpp"
#include "MoverCreatorEnvironment.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "PerformanceWidget.hpp"

#include <QFile>

namespace Designer
{

AutopilotSupportFileWork::AutopilotSupportFileWork(Vehicle* aVehiclePtr, const QDir& aTestDir)
{
   if (aVehiclePtr == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }

   // Setup working directory
   mTestDir = aTestDir;
}

void AutopilotSupportFileWork::BeginWork()
{
   emit WorkProgress(QString("<<LOADING>>"));

   mAFSIMProcessPtr = std::make_shared<QProcess>();
   qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");

   // Uncomment the following line to direct stdout to main program console -- doing so will disable the progress bar
   //mAFSIMProcessPtr->setProcessChannelMode(QProcess::ForwardedChannels);

   QString exePath(MoverCreatorEnvironment::GetEnvironment().GetAFSIM_Dir());
   QString shortenedFilePath("generate_autopilot_support_file.txt");

   mAFSIMProcessPtr->setWorkingDirectory(mTestDir.absolutePath());

   connect(mAFSIMProcessPtr.get(), static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &AutopilotSupportFileWork::HandleProcessFinished);
   connect(mAFSIMProcessPtr.get(), &QProcess::readyReadStandardOutput, this, &AutopilotSupportFileWork::ProcessStdOut);

   mAFSIMProcessPtr->start(exePath, QStringList(shortenedFilePath));

   if (!mAFSIMProcessPtr->waitForStarted())
   {
      AutopilotSupportFileWork::HandleProcessFinished(1, QProcess::ExitStatus::CrashExit);
      MoverCreatorMainWindow().CreateErrorBox("The Mission executable could not be launched. Autopilot tables have not been generated.");
   }

   mAFSIMProcessPtr->waitForFinished(-1);
}

void AutopilotSupportFileWork::HandleProcessFinished(int aExitCode, QProcess::ExitStatus aExitStatus)
{
   emit WorkFinished(mControllable);
}

void AutopilotSupportFileWork::ProcessStdOut()
{
   // Read a line at a time from stdOutputString
   while (mAFSIMProcessPtr.get()->canReadLine())
   {
      QByteArray byteArray = mAFSIMProcessPtr.get()->readLine(1024);
      emit WorkProgress(QString(byteArray));
   }
}

} // namespace Designer
