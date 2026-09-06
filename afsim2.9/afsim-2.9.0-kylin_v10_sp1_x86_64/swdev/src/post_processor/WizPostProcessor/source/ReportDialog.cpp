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

#include "ReportDialog.hpp"

#include <fstream>
#include <iostream>

#include <QDialog>
#include <QFile>
#include <QMainWindow>
#include <QMessageBox>

#include "Configuration.hpp"
#include "ProxyInterface.hpp"
#include "GeneralFormat.hpp"
#include "GeneralOutput.hpp"
#include "GeneralTime.hpp"
#include "UtInput.hpp"
#include "UtInputFile.hpp"

namespace
{
std::string SubstituteFileVariables(const std::string& aFileName,
                                    size_t             aRunNumber)
{
   std::string newString = aFileName;

   std::string::size_type firstChar = newString.find("%d");
   if (firstChar != std::string::npos)
   {
      std::ostringstream oss;
      oss << aRunNumber;
      newString.replace(firstChar, 2, oss.str());
   }
   return newString;
}
}

PostProcessor::ReportThread::ReportThread(const Configuration& aConfig)
   : QThread()
   , mConfig(aConfig)
{
}

void PostProcessor::ReportThread::run()
{
   mConfig.Execute();
}

PostProcessor::ReportDialog::ReportDialog(QMainWindow*    aMainWindowPtr,
                                          ReportType      aReportType,
                                          ProxyInterface* aProxyInterface,
                                          Qt::WindowFlags aFlags /*= Qt::WindowFlags()*/)
   : QDialog(aMainWindowPtr, aFlags)
   , mWidgetIndexMap()
   , mWidgetVec()
   , mProxyInterface(aProxyInterface)
   , mReportType(aReportType)
   , mGeneralTime(new GeneralTime())
   , mGeneralDirectories(new GeneralOutput())
{
   mUI.setupUi(this);
   mUI.mStatusMessage->setText("Ready");
   Build();
   Connect();
}

void PostProcessor::ReportDialog::Build()
{
   auto generalItem = new QTreeWidgetItem();
   generalItem->setText(0, "General");
   generalItem->setData(0, Qt::UserRole, "General");
   AddItem(generalItem);
   generalItem->setExpanded(true);

   auto generalOutputItem = new QTreeWidgetItem();
   generalOutputItem->setText(0, "Directory Options");
   generalOutputItem->setData(0, Qt::UserRole, "General.Directories");
   AddItem(generalOutputItem, "", generalItem);
   AddWidget("General.Directories", mGeneralDirectories);
   mWidgetIndexMap["General"] = static_cast<int>(mWidgetVec.size() - 1); // Name mod so when "General" is selected, the Directory Widget appears

   auto generalFormatItem = new QTreeWidgetItem();
   generalFormatItem->setText(0, "Format Options");
   generalFormatItem->setData(0, Qt::UserRole, "General.Format");
   AddItem(generalFormatItem, "", generalItem);
   AddWidget("General.Format", new GeneralFormat());

   auto generalTimeItem = new QTreeWidgetItem();
   generalTimeItem->setText(0, "Time Options");
   generalTimeItem->setData(0, Qt::UserRole, "General.Time");
   AddItem(generalTimeItem, "", generalItem);
   AddWidget("General.Time", mGeneralTime);
}

void PostProcessor::ReportDialog::Connect()
{
   connect(mUI.treeWidget, &QTreeWidget::itemClicked, this, &ReportDialog::ItemSelected);
   connect(mUI.mClose , &QPushButton::clicked, this, [this]() {close(); });
   connect(mUI.mGenerate, &QPushButton::clicked, this, &ReportDialog::GenerateClickedHandler);
   connect(mUI.mRestoreDefaults, &QPushButton::clicked, this, &ReportDialog::RestoreDefaults);
   connect(mProxyInterface, &ProxyInterface::EndTimeChanged, this, &ReportDialog::UpdateEndTime);
}

void PostProcessor::ReportDialog::AddItem(QTreeWidgetItem* aItem,
                                          const QString&   aToolTip /* = "" */,
                                          QTreeWidgetItem* aParentItem /* = nullptr */)
{
   if (aItem != nullptr)
   {
      if (aParentItem == nullptr)
      {
         mUI.treeWidget->addTopLevelItem(aItem);
      }
      else
      {
         aParentItem->addChild(aItem);
      }
      aItem->setToolTip(0, aToolTip);
   }
}

void PostProcessor::ReportDialog::AddWidget(const std::string& aName,
                                            ReportWidget*      aWidgetPtr)
{
   mWidgetVec.push_back(aWidgetPtr);
   mUI.stackedWidget->addWidget(mWidgetVec.back());
   mWidgetIndexMap[aName] = static_cast<int>(mWidgetVec.size() - 1);
}

void PostProcessor::ReportDialog::ItemSelected(QTreeWidgetItem* aTreeItem,
                                               int              aColumn)
{
   if (aTreeItem != nullptr)
   {
      std::string selectedItemName = aTreeItem->data(0, Qt::UserRole).value<QString>().toStdString();
      SetWidget(selectedItemName);
   }
}

void PostProcessor::ReportDialog::SetWidget(const std::string& aName)
{
   auto iter = mWidgetIndexMap.find(aName);
   if (iter != mWidgetIndexMap.end())
   {
      mUI.stackedWidget->setCurrentIndex(iter->second);
   }
}

PostProcessor::ReportWidget* PostProcessor::ReportDialog::GetWidget(const std::string& aName)
{
   return mWidgetVec.at(static_cast<size_t>(mWidgetIndexMap.at(aName)));
}

void PostProcessor::ReportDialog::GetPrimaryWidgets(std::vector<ReportWidget*>& aWidgets)
{
   // copy map over to vector of pairs so we can compare the value rather than the key
   std::vector<std::pair<std::string, int> > widgetIndexVec;
   for (const auto& p : mWidgetIndexMap)
   {
      widgetIndexVec.emplace_back(p);
   }

   // remove duplicates (comparing by index)
   widgetIndexVec.erase(std::unique(widgetIndexVec.begin(), widgetIndexVec.end(), [](std::pair<std::string, int>& a, std::pair<std::string, int>& b) {
                           return a.second == b.second;
                        }), widgetIndexVec.end());

   // sort on index
   std::sort(widgetIndexVec.begin(), widgetIndexVec.end(),
   [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) { return a.second < b.second; });


   aWidgets.clear();
   for (const auto& w : widgetIndexVec)
   {
      if (!mWidgetVec.at(static_cast<size_t>(w.second))->IsOptionWidget())
      {
         aWidgets.push_back(mWidgetVec.at(static_cast<size_t>(w.second)));
      }
   }
}

void PostProcessor::ReportDialog::GenerateClickedHandler()
{
   QString type = "";
   if (mReportType == ReportType::cCOMMUNICATION)
   {
      type = "Communication";
   }
   else if (mReportType == ReportType::cDETECTION)
   {
      type = "Detection";
   }
   else if (mReportType == ReportType::cDSV)
   {
      type = "DSV";
   }
   else if (mReportType == ReportType::cECLIPSE)
   {
      type = "Eclipse";
   }
   else if (mReportType == ReportType::cENGAGEMENT)
   {
      type = "Engagement";
   }
   else if (mReportType == ReportType::cTRAJECTORY)
   {
      type = "Trajectory";
   }

   mUI.mStatusMessage->setText("Generating Config File...");
   mUI.mStatusMessage->setText(type);
   repaint();

   std::string configFilePath = mGeneralDirectories->GetConfigName().toStdString();
   std::string dataFilePath = SubstituteFileVariables(mGeneralDirectories->GetDataFileName().toStdString(), static_cast<size_t>(mGeneralDirectories->GetMonteCarloStart()));

   std::ifstream dataStream(dataFilePath);
   if (dataStream.good())
   {
      bool allWidgetsValid = true;
      for (auto w : mWidgetVec)
      {
         if (!w->CheckValidity())
         {
            allWidgetsValid = false;
            mUI.mStatusMessage->setText("Error");
            repaint();
            break;
         }
      }

      if (allWidgetsValid)
      {
         std::ofstream currentStream(configFilePath, std::ofstream::out);
         currentStream << "# post_processor configuration input file" << '\n';
         currentStream << "# post_processor -c <path-to-this-file>" << '\n';
         currentStream << "report " << type.toStdString() << '\n';
         std::vector<ReportWidget*> primaryWidgets;
         GetPrimaryWidgets(primaryWidgets);
         if (currentStream.good())
         {
            for (auto w : primaryWidgets)
            {
               w->WriteData(currentStream);
            }

            currentStream.close();

            mUI.mStatusMessage->setText("Generating Report...");
            repaint();

            // Run the post_processor
            Configuration config(configFilePath);
            try
            {
               if (config.Execute())
               {
                  mUI.mStatusMessage->setText("Report generated");
               }
               else
               {
                  mUI.mStatusMessage->setText("Report not generated");
               }
            }
            // Catch any problems that occur
            catch (const UtException& aReason)
            {
               QMessageBox::critical(nullptr, "Post Processor:  Fatal", QString("post_processor terminated unexpectedly\n   Reason:  %1").arg(aReason.what()));
            }

            //Configuration config(configFilePath);
            //ReportThread thread(config);
            //thread.start();

            mUI.mStatusMessage->setText("Ready");
            repaint();
         }
         else
         {
            QMessageBox::critical(this, "Error", "Report Output Path Invalid. Check directory location");
            mUI.mStatusMessage->setText("Error");
            repaint();
         }
      }
   }
   else
   {
      QMessageBox::critical(this, "Error", "Data file invalid. Check file location");
      mUI.mStatusMessage->setText("Error");
      repaint();
   }
}

void PostProcessor::ReportDialog::RestoreDefaults()
{
   int i = mUI.stackedWidget->currentIndex();
   mWidgetVec[static_cast<size_t>(i)]->RestoreDefaults();
}

void PostProcessor::ReportDialog::UpdateEndTime(const WsfProxy::Time& aEndTime)
{
   mGeneralTime->UpdateEndTime(aEndTime);
}
