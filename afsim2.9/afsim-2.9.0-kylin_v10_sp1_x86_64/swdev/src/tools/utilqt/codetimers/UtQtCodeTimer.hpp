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

#ifndef UtQtCodeTimer_hpp
#define UtQtCodeTimer_hpp

#include "utilqt_export.h"

#include <QDialog>

#include "UtCodeTimer.hpp"

class QLabel;
class QTimer;
class QTreeWidget;
class QTreeWidgetItem;

class UtQtCodeTimerObserver : public UtCodeTimerObserver, public QDialog
{
public:
   UtQtCodeTimerObserver();
   ~UtQtCodeTimerObserver() override = default;
   void Update(double aTime, UtCodeTimerNode* aNodePtr) override;
   void PauseToggle();
   void ReportChildren(QTreeWidgetItem* aParent, UtCodeTimerNode* aNode, double aElapsedTime);

private:
   bool         mPaused;
   QPushButton* mPauseButton;
   QLabel*      mReportLabel;
   QTreeWidget* mTreeWidget;
};

// Calling initialize and terminate around code will allow for the use of UtCodeTimers in a qt application.
// this is best done at startup (after creating a QtApplication) and at shutdown
class UTILQT_EXPORT UtQtCodeTimer
{
public:
   // aRate sets the reporting rate for the timers
   // the report is a summary, so there is no need to make this high-frequency
   // aGUI, if set, will enable the GUI report dialog, otherwise reports go to the command-line
   static void Initialize(double aRate = 10.0, bool aGUI = true);
   static void Shutdown();

private:
   static QTimer*                msTimer;
   static UtQtCodeTimerObserver* msObserver;
};

#endif
