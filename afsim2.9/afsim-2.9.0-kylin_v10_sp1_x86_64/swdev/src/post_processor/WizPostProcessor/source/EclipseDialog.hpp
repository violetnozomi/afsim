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

#ifndef ECLIPSEDIALOG_HPP
#define ECLIPSEDIALOG_HPP

// Qt
#include <QDialog>
#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QObject>

#include "ReportDialog.hpp"

#include "ProxyInterface.hpp"

namespace Ui
{
class Dialog;
}

namespace PostProcessor
{
class EclipsePlatforms;

// Eclipse Report Dialog
class EclipseDialog : public ReportDialog
{
      Q_OBJECT
   public:
      EclipseDialog(QMainWindow* aMainWindowPtr,
                    ProxyInterface* aProxyInterface,
                    Qt::WindowFlags aFlags = Qt::WindowFlags());
      ~EclipseDialog() override = default;

   private:
      void Build();
      void Connect();
      EclipsePlatforms * mEclipsePlatformsWidget;
};
}
#endif // !ECLIPSEDIALOG_HPP