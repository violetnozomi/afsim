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

#ifndef AUTOPILOT_SUPPORT_FILE_WORK_HPP
#define AUTOPILOT_SUPPORT_FILE_WORK_HPP

#include <memory>
#include <QDir>
#include <QProcess>

#include "Vehicle.hpp"

namespace Designer
{
   class AutopilotSupportFileWork : public QObject
   {
         Q_OBJECT
      public:
         AutopilotSupportFileWork(Vehicle* aVehiclePtr, const QDir& aTestDir);
         ~AutopilotSupportFileWork() = default;

         void BeginWork();

         void HandleProcessFinished(int aExitCode, QProcess::ExitStatus aExitStatus);
         void ProcessStdOut();
         void SetControllable(bool aControllable) { mControllable = aControllable; }

      signals:
         void WorkFinished(bool aControllable);
         void WorkProgress(const QString& aProgressLine);

      private:
         std::shared_ptr<QProcess> mAFSIMProcessPtr = nullptr;

         QDir     mTestDir;
         bool     mControllable = true;
   };
}

#endif // AUTOPILOT_SUPPORT_FILE_WORK_HPP
