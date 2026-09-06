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

#ifndef DSV_DIALOG_HPP
#define DSV_DIALOG_HPP

// Qt
#include <QDialog>
#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QObject>

#include "ReportDialog.hpp"

namespace PostProcessor
{
// DSV Output Report Dialog
class DSV_Dialog : public ReportDialog
{
      Q_OBJECT
   public:
      DSV_Dialog(QMainWindow*    aMainWindowPtr,
                 ProxyInterface* aProxyInterface,
                 Qt::WindowFlags aFlags = Qt::WindowFlags());
      ~DSV_Dialog() override = default;

   private:
      void Build();
};
}
#endif
