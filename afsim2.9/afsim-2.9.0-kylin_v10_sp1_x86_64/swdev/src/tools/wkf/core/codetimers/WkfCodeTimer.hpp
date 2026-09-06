// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WkfCodeTimer_hpp
#define WkfCodeTimer_hpp

#include "wkf_export.h"

#include <QDockWidget>
#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QTimer>
#include <QTreeWidget>

#include "UtCodeTimer.hpp"
#include "WkfMainWindow.hpp"

namespace wkf
{
class WKF_EXPORT CodeTimerObject : public QObject
{
public:
   CodeTimerObject(wkf::MainWindow* aMainWindowPtr);
   ~CodeTimerObject();

private:
   QTimer mTimer;
};

class CodeTimerWidget : public UtCodeTimerObserver, public QDockWidget
{
public:
   CodeTimerWidget(QWidget* aParent, Qt::WindowFlags aFlags = Qt::WindowFlags());

   void Update(double aTime, UtCodeTimerNode* aNodePtr) override;
   void PauseToggle();
   void ReportChildren(QTreeWidgetItem* aParent, UtCodeTimerNode* aNode, double aElapsedTime);

private:
   QWidget*     mWidget      = nullptr;
   bool         mPaused      = false;
   QPushButton* mPauseButton = nullptr;
   QLabel*      mReportLabel = nullptr;
   QTreeWidget* mTreeWidget  = nullptr;
};
} // namespace wkf

#endif
