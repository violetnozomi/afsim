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

#ifndef PERFORMANCE_TEST_THREAD_HPP
#define PERFORMANCE_TEST_THREAD_HPP

#include <memory>
#include <QFileInfo>
#include <QProcess>
#include <QThread>

namespace Designer
{
   class MissionProcess : public QProcess
   {
      public:
         MissionProcess(const QString& aCommand, const QFileInfo& aTestFile);
         ~MissionProcess() = default;

         void Start();

      private:
         QString mCommand;
         QFileInfo mTestFile;

   };
   class PerformanceTestThread : public QThread
   {
      public:
         PerformanceTestThread() = default;
         ~PerformanceTestThread() = default;

         void run() override;
         void SetTestFiles(const std::vector<QFileInfo>& aTestFiles) { mTestFiles = aTestFiles; }

         void HandleProcessFinished(int aExitCode, QProcess::ExitStatus aExitStatus);

         bool ExecutedCleanly() { return mClean; }

      private:
         std::vector<QFileInfo>    mTestFiles;
         size_t                    mProcessNumber = 0;
         bool                      mClean = true;

         std::vector<std::shared_ptr<MissionProcess>> mAFSIMProcesses;
   };

}
#endif //PERFORMANCE_TEST_THREAD_HPP
