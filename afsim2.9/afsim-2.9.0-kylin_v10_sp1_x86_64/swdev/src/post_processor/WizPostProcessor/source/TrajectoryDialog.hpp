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

#ifndef TRAJECTORYDIALOG_HPP
#define TRAJECTORYDIALOG_HPP

#include <QDialog>
#include <QMainWindow>
#include <QObject>
#include <QTreeWidgetItem>

#include "ReportDialog.hpp"

namespace Ui
{
class ReportDialog;
}

namespace PostProcessor
{
// Trajectory Report Dialog
class TrajectoryDialog : public ReportDialog
{
      Q_OBJECT
   public:
      TrajectoryDialog(QMainWindow*    aMainWindowPtr,
                       ProxyInterface* aProxyInterface,
                       Qt::WindowFlags aFlags = Qt::WindowFlags());
      ~TrajectoryDialog() override = default;

   private:
      void Build();
};
}
#endif // !TRAJECTORYDIALOG_HPP