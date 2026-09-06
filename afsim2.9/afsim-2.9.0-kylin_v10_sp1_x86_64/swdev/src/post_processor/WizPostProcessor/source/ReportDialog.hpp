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

#ifndef REPORTDIALOG_HPP
#define REPORTDIALOG_HPP

// Qt
#include <QFile>
#include <QDialog>
#include <QMainWindow>
#include <QObject>
#include <QThread>
#include <QTreeWidgetItem>
#include <QXmlStreamWriter>

#include "Configuration.hpp"
#include "ProxyInterface.hpp"

#include "ui_ReportDialog.h"

namespace PostProcessor
{
class ProxyInterface;
class GeneralTime;
class GeneralOutput;
class ReportWidget;

class ReportThread : public QThread
{
      Q_OBJECT
   public:
      explicit ReportThread(const Configuration& aConfig);
      void run() override;

   private:
      Configuration mConfig;
};


// base class for all report dialogs
class ReportDialog : public QDialog
{
      Q_OBJECT
   public:
      enum ReportType
      {
         cENGAGEMENT,
         cCOMMUNICATION,
         cDETECTION,
         cDSV,
         cTRAJECTORY,
         cECLIPSE
      };

      ReportDialog(QMainWindow*    aMainWindowPtr,
                   ReportType      aReportType,
                   ProxyInterface* aProxyInterface,
                   Qt::WindowFlags aFlags = Qt::WindowFlags());
      ~ReportDialog() override = default;

      void UpdateEndTime(const WsfProxy::Time& aEndTime);

   protected:
      void AddItem(QTreeWidgetItem* aItem,
                   const QString&   aToolTip = "",
                   QTreeWidgetItem* aParentItem = nullptr);
      void AddWidget(const std::string& aName,
                     ReportWidget*      aWidgetPtr); // add aWidgetPtr to the stackedWidget, and maps it to aName

      void ItemSelected(QTreeWidgetItem* aTreeItem,
                        int              aColumn);
      void SetWidget(const std::string& aName); // sets the stacked widget to the widget named aName

      ReportWidget* GetWidget(const std::string& aName);
      void GetPrimaryWidgets(std::vector<ReportWidget*>& aWidgets); // gets all widgets but the option widgets

      std::map<std::string, int> mWidgetIndexMap; // map from option name to index of widget in mWidgetVec;
      std::vector<ReportWidget*> mWidgetVec; // widgets in the stackedWidget

      ProxyInterface*            mProxyInterface;

   private:
      void Build();
      void Connect();
      void GenerateClickedHandler();
      void RestoreDefaults();

      Ui::ReportDialog mUI;
      ReportType       mReportType;
      GeneralTime*     mGeneralTime;
      GeneralOutput*   mGeneralDirectories;

};
}
#endif
