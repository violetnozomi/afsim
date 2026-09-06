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

#ifndef ENGAGEMENTDIALOG_HPP
#define ENGAGEMENTDIALOG_HPP

// Qt
#include <QDialog>
#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QObject>

#include "ReportDialog.hpp"

namespace PostProcessor
{
class Participants;

// Engagement Report Dialog
class EngagementDialog : public ReportDialog
{
      Q_OBJECT
   public:
      EngagementDialog(QMainWindow*    aMainWindowPtr,
                       ProxyInterface* aProxyInterface,
                       Qt::WindowFlags aFlags = Qt::WindowFlags());
      ~EngagementDialog() override = default;

   private:
      void Build();
      void Connect();

      Participants* mActingPlatformsWidget;
      Participants* mAgainstPlatformsWidget;
};
}
#endif
