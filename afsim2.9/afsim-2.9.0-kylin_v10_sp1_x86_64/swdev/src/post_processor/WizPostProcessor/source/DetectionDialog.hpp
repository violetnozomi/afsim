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

#ifndef DETECTIONDIALOG_HPP
#define DETECTIONDIALOG_HPP

// Qt
#include <QDialog>
#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QObject>

#include "ProxyInterface.hpp"
#include "ReportDialog.hpp"

namespace PostProcessor
{
class DetectedPlatforms;
class DetectingSensors;

// Detection Report Dialog
class DetectionDialog : public ReportDialog
{
      Q_OBJECT
   public:
      DetectionDialog(QMainWindow*    aMainWindowPtr,
                      ProxyInterface* aProxyInterface,
                      Qt::WindowFlags aFlags = Qt::WindowFlags());
      ~DetectionDialog() override = default;

   private:
      void Build();
      void Connect();

      DetectingSensors* mDetectingSensorsWidget;
      DetectedPlatforms* mDetectedPlatformsWidget;
};
}
#endif // !DETECTIONDIALOG_HPP
