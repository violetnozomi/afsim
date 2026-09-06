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

#include "UtQtCodeTimer.hpp"

#include <QLabel>
#include <QPushButton>
#include <QStyle>
#include <QTimer>
#include <QTreeWidget>
#include <QVBoxLayout>

QTimer*                UtQtCodeTimer::msTimer    = nullptr;
UtQtCodeTimerObserver* UtQtCodeTimer::msObserver = nullptr;

// static
// this will keep a rolling real-time event at the provided rate
void UtQtCodeTimer::Initialize(double aRate, bool aGUI)
{
   if (!msTimer)
   {
      msTimer = new QTimer();

      QObject::connect(msTimer,
                       &QTimer::timeout,
                       []()
                       {
                          UtCodeTimer::Update();
                          msTimer->start();
                       });
      msTimer->setTimerType(Qt::PreciseTimer);
      msTimer->setInterval(1000 * aRate);
      msTimer->start();
      if (aGUI)
      {
         msObserver = new UtQtCodeTimerObserver();
         msObserver->show();
      }
      UtCodeTimer::Initialize(msObserver); // start up the UtCodeTimer
   }
}

// static
void UtQtCodeTimer::Shutdown()
{
   if (msTimer)
   {
      delete msTimer;
      msTimer = nullptr;
   }
   if (msObserver)
   {
      delete msObserver;
      msObserver = nullptr;
   }
   UtCodeTimer::Shutdown();
}

UtQtCodeTimerObserver::UtQtCodeTimerObserver()
   : QDialog()
   , mPaused(false)
{
   setMinimumSize(800, 600);
   setWindowTitle("UtQtCodeTimer Reports");
   QVBoxLayout* vbox = new QVBoxLayout(this);
   // setLayout(vbox);
   QHBoxLayout* hbox = new QHBoxLayout();
   vbox->addLayout(hbox);
   mReportLabel = new QLabel(this);
   hbox->addWidget(mReportLabel);
   mPauseButton = new QPushButton(this);
   mPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
   mPauseButton->setMaximumWidth(40);
   hbox->addWidget(mPauseButton);
   connect(mPauseButton, &QPushButton::pressed, this, &UtQtCodeTimerObserver::PauseToggle);
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

void UtQtCodeTimerObserver::Update(double aTime, UtCodeTimerNode* aNodePtr)
{
   if (!mPaused)
   {
      mReportLabel->setText("UtCodeTimer Report(" + QString::number(aTime) + " seconds):");
      mTreeWidget->clear();

      ReportChildren(mTreeWidget->invisibleRootItem(), aNodePtr, aTime);
      mTreeWidget->expandAll();
      mTreeWidget->resizeColumnToContents(0);
   }
}

void UtQtCodeTimerObserver::PauseToggle()
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

void UtQtCodeTimerObserver::ReportChildren(QTreeWidgetItem* aParent, UtCodeTimerNode* aNode, double aElapsedTime)
{
   for (auto&& it : aNode->GetChildMap())
   {
      QStringList sl;
      sl.push_back(it.first.c_str());
      sl.push_back(QString::number(100.0 * it.second->GetTotalTime() / aElapsedTime, 'f', 2));
      sl.push_back(QString::number(it.second->GetTotalTime()));
      sl.push_back(QString::number(it.second->GetInstances()));
      QTreeWidgetItem* item = new QTreeWidgetItem(sl);
      aParent->addChild(item);
      ReportChildren(item, it.second, it.second->GetTotalTime());
   }
}
