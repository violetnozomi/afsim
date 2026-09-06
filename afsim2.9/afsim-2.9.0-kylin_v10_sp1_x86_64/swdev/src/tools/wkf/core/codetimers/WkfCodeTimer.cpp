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

#include "WkfCodeTimer.hpp"

#include <QVBoxLayout>

wkf::CodeTimerObject::CodeTimerObject(wkf::MainWindow* aMainWindowPtr)
   : QObject(aMainWindowPtr)
{
   connect(&mTimer,
           &QTimer::timeout,
           [this]()
           {
              UtCodeTimer::Update();
              mTimer.start();
           });
   mTimer.setTimerType(Qt::PreciseTimer);
   mTimer.setInterval(10000); // 10 seconds
   mTimer.start();

   auto observer = new CodeTimerWidget(aMainWindowPtr);
   observer->setObjectName("Code Timers");
   observer->setWindowTitle("Code Timers");
   aMainWindowPtr->addDockWidget(Qt::RightDockWidgetArea, observer);
   observer->hide();
   UtCodeTimer::Initialize(observer); // start up the UtCodeTimer
}

wkf::CodeTimerObject::~CodeTimerObject()
{
   UtCodeTimer::Shutdown();
}

wkf::CodeTimerWidget::CodeTimerWidget(QWidget* aParent, Qt::WindowFlags aFlags)
   : UtCodeTimerObserver()
   , QDockWidget(aParent, aFlags)
{
   mWidget = new QWidget(this);
   setWidget(mWidget);

   QVBoxLayout* vbox = new QVBoxLayout(mWidget);
   QHBoxLayout* hbox = new QHBoxLayout();
   vbox->addLayout(hbox);
   mReportLabel = new QLabel(this);
   hbox->addWidget(mReportLabel);
   mPauseButton = new QPushButton(this);
   mPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
   mPauseButton->setMaximumWidth(40);
   hbox->addWidget(mPauseButton);
   connect(mPauseButton, &QPushButton::pressed, this, &CodeTimerWidget::PauseToggle);
   mTreeWidget = new QTreeWidget(this);
   mTreeWidget->setColumnCount(4);
   if (mTreeWidget->headerItem())
   {
      mTreeWidget->headerItem()->setText(0, "UtCodeTimer");
      mTreeWidget->headerItem()->setText(1, "Percent");
      mTreeWidget->headerItem()->setText(2, "Time (s)");
      mTreeWidget->headerItem()->setText(3, "Instances");
   }
   vbox->addWidget(mTreeWidget);
}

void wkf::CodeTimerWidget::Update(double aTime, UtCodeTimerNode* aNodePtr)
{
   if (!mPaused)
   {
      mReportLabel->setText("UtCodeTimer Report(" + QString::number(aTime) + " seconds):");
      mTreeWidget->clear();

      ReportChildren(mTreeWidget->invisibleRootItem(), aNodePtr, aTime);
      mTreeWidget->resizeColumnToContents(0);
   }
}

void wkf::CodeTimerWidget::PauseToggle()
{
   if (mPaused)
   {
      mPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
   }
   else
   {
      mPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
   }
   mPaused = !mPaused;
}

void wkf::CodeTimerWidget::ReportChildren(QTreeWidgetItem* aParent, UtCodeTimerNode* aNode, double aElapsedTime)
{
   for (auto&& it : aNode->GetChildMap())
   {
      QStringList sl;
      float       pct = it.second->GetTotalTime() / aElapsedTime;
      sl.push_back(it.first.c_str());
      sl.push_back(QString::number(100.0 * pct, 'f', 2));
      sl.push_back(QString::number(it.second->GetTotalTime()));
      sl.push_back(QString::number(it.second->GetInstances()));
      QTreeWidgetItem* item = new QTreeWidgetItem(sl);
      aParent->addChild(item);
      ReportChildren(item, it.second, it.second->GetTotalTime());

      item->setExpanded(pct > 0.1); // auto expand 10%'ers
      if (pct > 0.5)
      {
         item->setTextColor(0, Qt::red); // make 50%'ers red
      }
   }
}
