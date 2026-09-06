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

#ifndef COMMUNICATIONDIALOG_HPP
#define COMMUNICATIONDIALOG_HPP

// Qt
#include <QDialog>
#include <QMainWindow>
#include <QObject>
#include <QTreeWidgetItem>

#include "ReportDialog.hpp"

namespace PostProcessor
{
class Transmitters;
class Receivers;
class Networks;

// Communication Report Dialog
class CommunicationDialog : public ReportDialog
{
      Q_OBJECT
   public:
      CommunicationDialog(QMainWindow*    aMainWindowPtr,
                          ProxyInterface* aProxyInterface,
                          Qt::WindowFlags aFlags = Qt::WindowFlags());
      ~CommunicationDialog() override = default;

   private:
      void Build();
      void Connect();

      Transmitters* mTransmittersWidget;
      Receivers*    mReceiversWidget;
      Networks*     mNetworksWidget;
};
}
#endif
