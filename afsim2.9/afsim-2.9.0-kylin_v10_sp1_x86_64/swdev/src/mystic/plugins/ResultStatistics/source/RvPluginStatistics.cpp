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
#include "RvPluginStatistics.hpp"

#include <QAction>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QMenu>
#include <QMessageBox>
#include <QTableView>
#include <QTextBrowser>

#include "RvEnvironment.hpp"
#include "RvEventTableModel.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvStatisticsRules.hpp"
#include "UtPackSchema.hpp"
#include "UtQtPieChart.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfScenario.hpp"
#include "WkfVtkEnvironment.hpp"
#include "statistics/WkfEngagementStatistics.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(RvStatistics::Plugin, "Statistics", "Displays statistics about the data in a result file.", "mystic")

RvStatistics::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
   , mEventListDialog(new QDialog())
   , mEventListWidget(new WkfFilter::FilterWidget(nullptr, "ResultStatisticsEventList"))
   , mMetaDataDialog(nullptr)
   , mMetaDataStrings()
{
   wkf::MainWindow* mainWindowPtr = wkfEnv.GetMainWindow();
   if (nullptr != mainWindowPtr)
   {
      QMenu* toolsMenu = mainWindowPtr->GetMenuByName("Tools");
      if (nullptr != toolsMenu)
      {
         QAction* show = new QAction("Show Result Statistics", toolsMenu);
         toolsMenu->insertAction(*toolsMenu->actions().begin(), show);
         connect(show, &QAction::triggered, this, &RvStatistics::Plugin::ShowStats);
         QAction* raw = new QAction("Show Event List", toolsMenu);
         toolsMenu->insertAction(*toolsMenu->actions().begin(), raw);
         connect(raw, &QAction::triggered, this, &RvStatistics::Plugin::ShowEventList);
         QAction* meta = new QAction("Show AFSIM Information", toolsMenu);
         toolsMenu->insertAction(*toolsMenu->actions().begin(), meta);
         connect(meta, &QAction::triggered, this, &RvStatistics::Plugin::ShowMetaData);
      }
   }

   auto* layout = new QVBoxLayout();
   layout->addWidget(mEventListWidget);
   mEventListDialog->setLayout(layout);
   mEventListDialog->resize(700, 800);

   mEventListWidget->RegisterRule<StatisticsRuleSide>();
   mEventListWidget->RegisterRule<StatisticsRuleType>();
   mEventListWidget->RegisterRule<StatisticsRuleCategory>();

   WkfFilter::FilterPlot& plot = mEventListWidget->GetPlot();

   plot.MarkColumnAsValidSeries("type");
   plot.MarkColumnAsValidSeries("platform");
   plot.MarkColumnAsValidSeries("interactor");
   plot.MarkColumnAsValidSeries("track id");
   plot.MarkColumnAsValidSeries("component");

   plot.SetDefaultXAxisColumn("time");
   plot.SetDefaultSeriesColumn("platform", true);
}

void RvStatistics::Plugin::ShowStats()
{
   rv::ResultData* results = rvEnv.GetData();
   if (nullptr != results)
   {
      const UtPackSchema* schema = results->GetDb()->GetSchema();
      if (schema)
      {
         QDialog dlg(wkfEnv.GetMainWindow());
         dlg.setWindowTitle("Result Statistics");
         QHBoxLayout* lo = new QHBoxLayout(&dlg);
         dlg.setLayout(lo);
         QTextBrowser* textb = new QTextBrowser(&dlg);
         textb->setWordWrapMode(QTextOption::NoWrap);
         textb->setReadOnly(true);
         lo->addWidget(textb);
         textb->resize(textb->document()->size().width(), textb->document()->size().height());
         UtQtPieChart* gv = new UtQtPieChart(&dlg);
         lo->addWidget(gv);
         gv->setMinimumWidth(200);
         gv->setMinimumHeight(200);

         size_t memMax              = 0;
         using MessageBytesAndCount = std::pair<size_t, size_t>;
         using MessageStatsAndType  = std::pair<MessageBytesAndCount, std::string>;
         std::vector<MessageStatsAndType> counts;
         rv::ResultDb::MessageStats       stats = results->GetDb()->GetMessageStats();
         std::vector<std::string>         types = schema->GetTypeNames();
         for (const auto& i : types)
         {
            const UtPackType* type = schema->FindType(i);
            if (type && type->IsStruct())
            {
               UtPackStruct* structPtr = (UtPackStruct*)type;
               if (structPtr->IsMessage())
               {
                  int id = structPtr->GetMessageId();
                  if (id >= 0)
                  {
                     memMax = (stats.mMessageBytesByType[id] > memMax) ? stats.mMessageBytesByType[id] : memMax;
                     counts.emplace_back(MessageBytesAndCount(stats.mMessageBytesByType[id], stats.mMessageCountByType[id]),
                                         structPtr->GetTypeName());
                  }
               }
            }
         }
         QString text;
         text += "Event Size\tCount\tName:\n";
         std::sort(counts.begin(), counts.end());
         for (std::vector<MessageStatsAndType>::reverse_iterator i = counts.rbegin(); i != counts.rend(); ++i)
         {
            if (memMax > 100000)
            {
               text += QString::number(i->first.first / 1000000.0, 'f', 2);
               text += "MB\t";
            }
            else if (memMax > 100)
            {
               text += QString::number(i->first.first / 1000.0, 'f', 2);
               text += "KB\t";
            }
            else
            {
               text += QString::number(i->first.first, 'f', 2);
               text += "B\t";
            }
            text += QString::number(i->first.second);
            text += "\t";
            text += QString::fromStdString(i->second);
            text += "\n";
            if (i->first.first > 0)
            {
               gv->AddData(QString::fromStdString(i->second), i->first.first);
            }
         }

         textb->setText(text);
         textb->setMinimumWidth(350);
         dlg.exec();
      }
   }
}

void RvStatistics::Plugin::ShowEventList()
{
   rv::ResultData* results = rvEnv.GetData();
   if (nullptr != results)
   {
      rv::ResultDb* db = results->GetDb();
      if (nullptr != db)
      {
         EventTableModel data(nullptr);

         rv::MessageBaseArray& apml = db->GetAllPagedMessageList();
         rv::MessageBaseArray& aoml = db->GetAllOneTimeMessageList();

         data.SetData(apml, aoml, db);

         connect(wkfEnv.GetMainWindow(), &wkf::MainWindow::MainWindowClose, mEventListDialog.data(), &QDialog::close);
         connect(this, &Plugin::ScenarioCleared, mEventListDialog.data(), &QDialog::reject);


         mEventListWidget->SetSourceModel(&data);
         rvEnv.SendPauseCommand();
         mEventListDialog->exec();

         mEventListWidget->SetSourceModel(nullptr);
      }
   }
}

void RvStatistics::Plugin::MetaDataRead(const rv::MsgExecData& aExecMsg)
{
   mMetaDataStrings.clear();
   mMetaDataStrings.push_back(QStringList{"executable", QString::fromStdString(aExecMsg.version())});
   mMetaDataStrings.push_back(QStringList{"path", QString::fromStdString(aExecMsg.execPath())});
   mMetaDataStrings.push_back(QStringList{"cmdLine", QString::fromStdString(aExecMsg.cmdLine())});
   mMetaDataStrings.push_back(QStringList{"time", QString::fromStdString(aExecMsg.timeStamp())});
   QStringList feats = QString::fromStdString(aExecMsg.features()).split("|", QString::SkipEmptyParts);
   feats.prepend("features");
   mMetaDataStrings.push_back(feats);
   QStringList exts = QString::fromStdString(aExecMsg.extensions()).split("|", QString::SkipEmptyParts);
   exts.prepend("extensions");
   mMetaDataStrings.push_back(exts);
   if (mMetaDataDialog)
   {
      mMetaDataDialog->SetData(mMetaDataStrings);
   }
}

void RvStatistics::Plugin::ShowMetaData()
{
   if (!mMetaDataDialog)
   {
      mMetaDataDialog = new MetaDataDialog(wkfEnv.GetMainWindow());
      mMetaDataDialog->SetData(mMetaDataStrings);
   }
   mMetaDataDialog->show();
}

void RvStatistics::Plugin::ClearScenario(bool aFullReset)
{
   mMetaDataStrings.clear();
   delete mMetaDataDialog;
   mMetaDataDialog = nullptr;
   emit ScenarioCleared();
}
