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
#include "RvPluginEngagement.hpp"

#include <QAction>
#include <QApplication>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QMenu>
#include <QMessageBox>
#include <QTableView>
#include <QTextBrowser>

#include "RvEnvironment.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "UtPackSchema.hpp"
#include "UtQtPieChart.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "statistics/WkfEngagementStatistics.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(RvEngagement::Plugin, "Engagement Analysis", "Displays engagements found in the event-pipe.", "mystic")

RvEngagement::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
   , mEngagementStatistics(nullptr)
   , mMutex()
{
   wkf::MainWindow* mainWindowPtr = wkfEnv.GetMainWindow();
   if (nullptr != mainWindowPtr)
   {
      QMenu* toolsMenu = mainWindowPtr->GetMenuByName("Tools");
      if (nullptr != toolsMenu)
      {
         QAction* showEngage = new QAction("Show Engagement Statistics", toolsMenu);
         toolsMenu->insertAction(*toolsMenu->actions().begin(), showEngage);
         connect(showEngage, &QAction::triggered, this, &RvEngagement::Plugin::ShowEngagements);
      }
   }
}

void RvEngagement::Plugin::ShowEngagements()
{
   rv::ResultData* results = rvEnv.GetData();
   if (nullptr != results)
   {
      mMutex.lock();
      if (nullptr != mEngagementStatistics)
      {
         delete mEngagementStatistics;
      }
      mEngagementStatistics = new wkf::EngagementStatistics(wkfEnv.GetMainWindow());
      connect(mEngagementStatistics,
              &wkf::EngagementStatistics::FilterChanged,
              this,
              &RvEngagement::Plugin::EngagementFilterChanged);
      connect(mEngagementStatistics,
              &wkf::EngagementStatistics::BuildRowContextMenu,
              this,
              &RvEngagement::Plugin::BuildRowContextMenu);

      mEngagementStatistics->show();
      EngagementFilterChangedPrivate();
      mMutex.unlock();
   }
}

void RvEngagement::Plugin::ClearScenario(bool aFullReset)
{
   mMutex.lock();
   delete mEngagementStatistics;
   mEngagementStatistics = nullptr;
   mEngagementEvents.clear();
   mMutex.unlock();
}

void RvEngagement::Plugin::WeaponFiredEventRead(const rv::MsgWeaponFired& aEvent)
{
   mMutex.lock();
   mEngagementEvents.push_back(&aEvent);
   if (mEngagementStatistics != nullptr)
   {
      ProcessWeaponFired(&aEvent);
   }
   mMutex.unlock();
}

void RvEngagement::Plugin::WeaponTerminatedEventRead(const rv::MsgWeaponTerminated& aEvent)
{
   mMutex.lock();
   mEngagementEvents.push_back(&aEvent);
   if (mEngagementStatistics != nullptr)
   {
      ProcessWeaponTerminated(&aEvent);
   }
   mMutex.unlock();
}

void RvEngagement::Plugin::ProcessWeaponFired(const rv::MsgWeaponFired* aEvent)
{
   if (mEngagementStatistics->InterestedIn(aEvent->firingPlatformIndex(), aEvent->targetPlatformIndex()))
   {
      QString         aname;
      QString         tname;
      QString         wname;
      rv::ResultData* results = rvEnv.GetData();
      if (nullptr != results)
      {
         rv::ResultPlatform* aplat = results->GetDb()->FindPlatform(aEvent->firingPlatformIndex());
         if (nullptr != aplat)
         {
            aname = aplat->GetName().c_str();
         }
         rv::ResultPlatform* tplat = results->GetDb()->FindPlatform(aEvent->targetPlatformIndex());
         if (nullptr != aplat)
         {
            tname = tplat->GetName().c_str();
         }
         rv::ResultPlatform* wplat = results->GetDb()->FindPlatform(aEvent->weaponPlatformIndex());
         if (nullptr != wplat)
         {
            wname = wplat->GetName().c_str();
         }
      }
      mEngagementStatistics->AddWeaponFire(aEvent->simTime(),
                                           aEvent->firingPlatformIndex(),
                                           aEvent->targetPlatformIndex(),
                                           aEvent->intendedTargetName(),
                                           aEvent->weaponPlatformIndex(),
                                           aEvent->targetTrackId().localTrackNumber(),
                                           aEvent->targetTrackId().owner(),
                                           aname,
                                           tname,
                                           wname);
   }
}

void RvEngagement::Plugin::ProcessWeaponTerminated(const rv::MsgWeaponTerminated* aEvent)
{
   if (mEngagementStatistics->InterestedIn(aEvent->firingPlatformIndex(), aEvent->targetPlatformIndex()))
   {
      QString         aname;
      QString         tname;
      QString         wname;
      rv::ResultData* results = rvEnv.GetData();
      if (nullptr != results)
      {
         rv::ResultPlatform* aplat = results->GetDb()->FindPlatform(aEvent->firingPlatformIndex());
         if (nullptr != aplat)
         {
            aname = aplat->GetName().c_str();
         }
         rv::ResultPlatform* tplat = results->GetDb()->FindPlatform(aEvent->targetPlatformIndex());
         if (nullptr != aplat)
         {
            tname = tplat->GetName().c_str();
         }
         rv::ResultPlatform* wplat = results->GetDb()->FindPlatform(aEvent->weaponPlatformIndex());
         if (nullptr != wplat)
         {
            wname = wplat->GetName().c_str();
         }
      }
      mEngagementStatistics->AddWeaponTerminate(aEvent->simTime(),
                                                aEvent->firingPlatformIndex(),
                                                aEvent->targetPlatformIndex(),
                                                aEvent->intendedTargetName(),
                                                aEvent->weaponPlatformIndex(),
                                                aEvent->geometryResult(),
                                                aEvent->extendedResult(),
                                                aEvent->missDistance(),
                                                aname,
                                                tname,
                                                wname);
   }
}

void RvEngagement::Plugin::EngagementFilterChanged()
{
   mMutex.lock();
   EngagementFilterChangedPrivate();
   mMutex.unlock();
}

void RvEngagement::Plugin::EngagementFilterChangedPrivate()
{
   for (auto&& it : mEngagementEvents)
   {
      if (it->GetMessageId() == rv::MsgWeaponFired::cMESSAGE_ID)
      {
         ProcessWeaponFired((rv::MsgWeaponFired*)it);
      }
      else
      {
         ProcessWeaponTerminated((rv::MsgWeaponTerminated*)it);
      }
   }
   // updateEngagements
}

void RvEngagement::Plugin::BuildRowContextMenu(QMenu* aMenuPtr, const QStringList& aStrList)
{
   QAction* traceAction = new QAction("Trace Event...", aMenuPtr);
   connect(traceAction, &QAction::triggered, [this, aStrList]() { TraceEvent(aStrList); });
   aMenuPtr->addAction(traceAction);
}

void RvEngagement::Plugin::TraceEvent(const QStringList& aWeaponData)
{
   QString                                            weaponName   = aWeaponData[5];
   QString                                            trackId      = aWeaponData[6];
   int                                                index        = trackId.lastIndexOf(":");
   QString                                            platformName = trackId.left(index);
   int                                                trackIdx = trackId.right(trackId.length() - index - 1).toInt();
   std::vector<std::pair<rv::MsgBase*, unsigned int>> eventList;

   rv::ResultData* results = rvEnv.GetData();
   if (nullptr != results)
   {
      rv::ResultDb* db = results->GetDb();
      if (nullptr != db)
      {
         rv::MsgWeaponFired* wfPtr = nullptr;
         float               time  = FLT_MAX;
         QString             message;
         // 1st collect weaponfire and weaponterminated events based on trackid and weaponidx
         rv::ResultPlatform* ep_platform = db->FindPlatformByName(weaponName);
         if (nullptr != ep_platform)
         {
            int                      weaponIndex = ep_platform->GetPlatformIndex();
            rv::MsgWeaponTerminated* wtPtr       = nullptr;
            db->FindWeaponTerminationByWeaponId(weaponIndex, wtPtr);
            // wtId == 0 indicates we could not find a termination for this weapon id
            if (wtPtr)
            {
               eventList.push_back(std::pair<rv::MsgBase*, unsigned int>(wtPtr, 0));
            }
            db->FindWeaponFireByWeaponId(weaponIndex, time, wfPtr);
            // wfId == 0 indicates we could not find a fire for this weapon... how did we get here?
            int wtIdx = 0;
            if (wtPtr)
            {
               wtIdx = wtPtr->GetMessageIndex();
            }
            eventList.push_back(std::pair<rv::MsgBase*, unsigned int>(wfPtr, wtIdx));
         }
         // 2nd follow the trackid to its origins (local created, sensor created, correlation, decorrelation, engage tasks)
         rv::Track_Id tid;
         tid.owner(platformName.toStdString());
         tid.localTrackNumber(trackIdx);

         unsigned int messageIndex = 0;
         if (wfPtr)
         {
            messageIndex = wfPtr->GetMessageIndex();
         }
         db->TraceTrackId(tid, messageIndex, eventList, time);

         QColor fc = qApp->palette().text().color();
         qreal  hsv[3];
         fc.getHsvF(&(hsv[0]), &(hsv[1]), &(hsv[2]));
         bool darkText = hsv[2] < 0.5;

         std::sort(eventList.begin(), eventList.end(), EngageSort);
         //         std::sort(eventList.begin(), eventList.end());
         int w = 0;
         for (auto&& it : eventList)
         {
            QString line = "";
            QString time = QString::number(it.first->simTime(), 'f', 4) + ": ";
            QString pre;
            QString post = "</font><br>";
            // build a message
            switch (it.first->GetMessageId())
            {
            case (rv::MsgWeaponTerminated::cMESSAGE_ID):
            {
               rv::MsgWeaponTerminated* wt = static_cast<rv::MsgWeaponTerminated*>(it.first);
               if (darkText)
               {
                  pre = "<font color=\"#CC0000\">";
               }
               else
               {
                  pre = "<font color=\"#FFAAAA\">";
               }
               line = "Weapon Terminates: " +
                      QString(rv::MsgWeaponTerminated_GeometryResult::ToString(wt->geometryResult()));
               break;
            }
            case (rv::MsgWeaponFired::cMESSAGE_ID):
            {
               rv::MsgWeaponFired* wf           = static_cast<rv::MsgWeaponFired*>(it.first);
               QString             platformName = db->FindPlatform(wf->firingPlatformIndex())->GetName().c_str();
               if (darkText)
               {
                  pre = "<font color=\"#CC0000\">";
               }
               else
               {
                  pre = "<font color=\"#FFAAAA\">";
               }
               line = platformName + " fires on " + wf->targetTrackId().owner().c_str() + ":" +
                      QString::number(wf->targetTrackId().localTrackNumber());
               break;
            }
            case (rv::MsgTaskUpdate::cMESSAGE_ID):
            {
               rv::MsgTaskUpdate* tu       = static_cast<rv::MsgTaskUpdate*>(it.first);
               QString            assigner = db->FindPlatform(tu->assignerPlatform())->GetName().c_str();
               QString            assignee = db->FindPlatform(tu->assigneePlatform())->GetName().c_str();
               if (darkText)
               {
                  pre = "<font color=\"#AA00CC\">";
               }
               else
               {
                  pre = "<font color=\"#DDAAFF\">";
               }

               if (tu->state() == rv::MsgTaskUpdate_State::assigned)
               {
                  line = assigner + " tasks " + assignee + " to " + tu->taskType().c_str() + " against " +
                         tu->trackId().owner().c_str() + ":" + QString::number(tu->trackId().localTrackNumber());
               }
               else if (tu->state() == rv::MsgTaskUpdate_State::canceled)
               {
                  line = assigner + " cancels task for " + assignee + " to " + tu->taskType().c_str() + " against " +
                         tu->trackId().owner().c_str() + ":" + QString::number(tu->trackId().localTrackNumber());
               }
               else
               {
                  line = assignee + " completes task from " + assigner + " to " + tu->taskType().c_str() + " against " +
                         tu->trackId().owner().c_str() + ":" + QString::number(tu->trackId().localTrackNumber());
               }
               break;
            }
            case (rv::MsgSensorTrackCreated::cMESSAGE_ID):
            {
               rv::MsgSensorTrackCreated* stc          = static_cast<rv::MsgSensorTrackCreated*>(it.first);
               QString                    platformName = db->FindPlatform(stc->ownerIndex())->GetName().c_str();
               if (darkText)
               {
                  pre = "<font color=\"#888800\">";
               }
               else
               {
                  pre = "<font color=\"#FFFFCC\">";
               }
               line = platformName + "'s " + stc->sensorName().c_str() + " has a new sensor track " +
                      stc->trackId().owner().c_str() + ":" + QString::number(stc->trackId().localTrackNumber());
               break;
            }
            case (rv::MsgLocalTrackCreated::cMESSAGE_ID):
            {
               rv::MsgLocalTrackCreated* ltc          = static_cast<rv::MsgLocalTrackCreated*>(it.first);
               QString                   platformName = db->FindPlatform(ltc->ownerIndex())->GetName().c_str();
               if (darkText)
               {
                  pre = "<font color=\"#666600\">";
               }
               else
               {
                  pre = "<font color=\"#EEEE88\">";
               }
               line = platformName + " has a new local track " + ltc->trackId().owner().c_str() + ":" +
                      QString::number(ltc->trackId().localTrackNumber());
               break;
            }
            case (rv::MsgLocalTrackCorrelation::cMESSAGE_ID):
            {
               rv::MsgLocalTrackCorrelation* ltc          = static_cast<rv::MsgLocalTrackCorrelation*>(it.first);
               QString                       platformName = db->FindPlatform(ltc->ownerIndex())->GetName().c_str();
               if (darkText)
               {
                  pre = "<font color=\"#666600\">";
               }
               else
               {
                  pre = "<font color=\"#EEEE88\">";
               }
               line = platformName + " correlates " + ltc->trackId().owner().c_str() + ":" +
                      QString::number(ltc->trackId().localTrackNumber()) + " with " +
                      ltc->addTrackId().owner().c_str() + ":" + QString::number(ltc->addTrackId().localTrackNumber());
               break;
            }
            case (rv::MsgLocalTrackDecorrelation::cMESSAGE_ID):
            {
               rv::MsgLocalTrackDecorrelation* ltd          = static_cast<rv::MsgLocalTrackDecorrelation*>(it.first);
               QString                         platformName = db->FindPlatform(ltd->ownerIndex())->GetName().c_str();
               if (darkText)
               {
                  pre = "<font color=\"#666600\">";
               }
               else
               {
                  pre = "<font color=\"#EEEE88\">";
               }
               line = platformName + " decorrelates " + ltd->trackId().owner().c_str() + ":" +
                      QString::number(ltd->trackId().localTrackNumber()) + " from " +
                      ltd->remTrackId().owner().c_str() + ":" + QString::number(ltd->remTrackId().localTrackNumber());
               break;
            }
            }
            int lw = qApp->fontMetrics().width(time + line);
            w      = (w > lw) ? w : lw;
            message += time + pre + line + post;
            // free the memory
            delete it.first;
         }

         // 3rd figure out how to present this
         QDialog dlg(wkfEnv.GetMainWindow());
         dlg.resize(w + 50, static_cast<int>(eventList.size() * qApp->fontMetrics().lineSpacing() + 50));
         dlg.setWindowTitle("Engagement Events");
         dlg.setLayout(new QHBoxLayout(&dlg));
         QTextBrowser* textEdit = new QTextBrowser(&dlg);
         textEdit->setAcceptRichText(true);
         dlg.layout()->addWidget(textEdit);
         textEdit->setReadOnly(true);
         textEdit->setText(message);
         dlg.exec();
      }
   }
}

bool RvEngagement::Plugin::EngageSort(const std::pair<rv::MsgBase*, unsigned int>& i,
                                      const std::pair<rv::MsgBase*, unsigned int>& j)
{
   std::map<int, int> prec;
   int                mstc  = rv::MsgSensorTrackCreated::cMESSAGE_ID;
   int                mtu   = rv::MsgTaskUpdate::cMESSAGE_ID;
   int                mltc  = rv::MsgLocalTrackCreated::cMESSAGE_ID;
   int                mltco = rv::MsgLocalTrackCorrelation::cMESSAGE_ID;
   int                mltd  = rv::MsgLocalTrackDecorrelation::cMESSAGE_ID;
   int                mwf   = rv::MsgWeaponFired::cMESSAGE_ID;
   int                mwt   = rv::MsgWeaponTerminated::cMESSAGE_ID;
   prec[mstc]               = 0;
   prec[mtu]                = 1;
   prec[mltc]               = 2;
   prec[mltco]              = 3;
   prec[mltd]               = 4;
   prec[mwf]                = 5;
   prec[mwt]                = 6;

   if (i.first->simTime() == j.first->simTime())
   {
      return prec[i.first->GetMessageId()] < prec[j.first->GetMessageId()];
   }
   else
   {
      return (i.first->simTime() < j.first->simTime());
   }
}
