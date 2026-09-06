// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "PerformanceWidget.hpp"

#include <algorithm>

#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QJsonArray>
#include <QProgressDialog>
#include <QRadioButton>
#include <QTextStream>

#include "AutopilotSupportFileWork.hpp"
#include "CalculateAeroDialog.hpp"
#include "ControlsWidget.hpp"
#include "MoverCreatorEnvironment.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorWidget.hpp"
#include "PerformanceTestThread.hpp"
#include "PerformanceTestTreeWidgetItem.hpp"
#include "ScriptGeneratorGM.hpp"
#include "ScriptGeneratorP6DOF.hpp"
#include "ScriptGeneratorPointMassSixDOF.hpp"
#include "ScriptGeneratorRigidBodySixDOF.hpp"
#include "UtLog.hpp"
#include "ui_PerformanceWidget.h"

namespace Designer
{

PerformanceWidget::PerformanceWidget(QWidget*        aParent,
                                     Qt::WindowFlags aFlags /*= Qt::WindowFlags()*/)
   : QWidget(aParent, aFlags)
   , mUIPtr(new Ui::PerformanceWidget)
   , mPerformanceTestThread(new PerformanceTestThread)
{
   mUIPtr->setupUi(this);
   connect(mUIPtr->executePushButton, &QPushButton::clicked, this, &PerformanceWidget::ExecuteTestButtonClicked);
   connect(mUIPtr->mPreviousButton, &QPushButton::clicked, this, &PerformanceWidget::PreviousButtonClicked);
   connect(mUIPtr->mNextButton, &QPushButton::clicked, this, &PerformanceWidget::NextButtonClicked);

   connect(mUIPtr->mMoverComboBox, &QComboBox::currentTextChanged, this, &PerformanceWidget::HandleMoverChanged);

   SetPlotColors();
}

PerformanceWidget::~PerformanceWidget()
{
   delete mUIPtr;
   delete mPerformanceTestThread;
}

void PerformanceWidget::Initialize()
{
   mUIPtr->performancePlotWidget->GetPlot()->SetUseMultipleColorsForSeries(true);
   mUIPtr->performancePlotWidget->GetPlot()->SetShowLegend(true);
}

void PerformanceWidget::HandleMoverChanged(const QString& aMoverText)
{
   ClearPlotArea();

   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   QString vehicleName = QString::fromStdString(mw.GetMoverCreatorWidget()->GetVehicle()->GetVehicleName());

   mTestDir = QDir(MoverCreatorMainWindow::GetDataPath() + "/AFSIM_Scripts/" + vehicleName + "/tests/");
   mOutputDirs.clear();

   QDir outputRoot = QDir(MoverCreatorMainWindow::GetDataPath() + "/AFSIM_Scripts/" + vehicleName + "/output/");
   if (aMoverText == "Comparison")
   {
      std::vector<QDir> testDirs;
      if (mw.GetMoverCreatorWidget()->GenerateGuidedMover())
      {
         testDirs.push_back(mTestDir.absolutePath() + "/gm/");
         mOutputDirs.push_back(outputRoot.absolutePath() + "/gm/");
      }
      if (mw.GetMoverCreatorWidget()->GenerateP6DOFMover())
      {
         testDirs.push_back(mTestDir.absolutePath() + "/p6dof/");
         mOutputDirs.push_back(outputRoot.absolutePath() + "/p6dof/");
      }
      if (mw.GetMoverCreatorWidget()->GenerateRB6DOFMover())
      {
         testDirs.push_back(mTestDir.absolutePath() + "/rb6dof/");
         mOutputDirs.push_back(outputRoot.absolutePath() + "/rb6dof/");
      }
      if (mw.GetMoverCreatorWidget()->GeneratePM6DOFMover())
      {
          testDirs.push_back(mTestDir.absolutePath() + "/pm6dof/");
          mOutputDirs.push_back(outputRoot.absolutePath() + "/pm6dof/");
      }
      PopulateComparisonTests(testDirs);
   }
   else
   {
      if (aMoverText == "RB6DOF Mover")
      {
         mTestDir = QDir(mTestDir.absolutePath() + "/rb6dof/");
         mOutputDirs.push_back(outputRoot.absolutePath() + "/rb6dof/");
      }
      if (aMoverText == "P6DOF Mover")
      {
         mTestDir = QDir(mTestDir.absolutePath() + "/p6dof/");
         mOutputDirs.push_back(outputRoot.absolutePath() + "/p6dof/");
      }
      else if (aMoverText == "PM6DOF Mover")
      {
          mTestDir = QDir(mTestDir.absolutePath() + "/pm6dof/");
          mOutputDirs.push_back(outputRoot.absolutePath() + "/pm6dof/");
      }
      else if (aMoverText == "Guided Mover")
      {
          mTestDir = QDir(mTestDir.absolutePath() + "/gm/");
          mOutputDirs.push_back(outputRoot.absolutePath() + "/gm/");
      }
      PopulatePerformanceTests();
   }

}

std::vector<PerformanceWidget::PerformanceTestData> PerformanceWidget::GetPerformanceTestData(const QDir& aTestDir)
{
   std::vector<PerformanceTestData> testData;

   for (auto& i : aTestDir.entryList(QDir::Filter::Files))
   {
      if (i.contains("_test"))
      {
         QString testName = i.section(".", 0, 0);
         QFile file(aTestDir.absolutePath() + "/" + i);
         if (file.open(QIODevice::ReadOnly))
         {
            QTextStream stream(&file);
            QString titleStr = "Untitled Test";
            QString testGroup = "Untitled Group";
            QString testDescription = "";
            QString testDetailedDescription = "";
            bool containsHeader = false;
            while (true)
            {
               QString line = stream.readLine();
               line.remove(0, 2); // Opening "//"

               if (line.startsWith("TEST_HEADER"))
               {
                  containsHeader = true;
                  while (true)
                  {
                     line = stream.readLine();
                     line.remove(0, 2); // Opening "//"

                     if (line.startsWith("TITLE"))
                     {
                        line.remove(0, 6);
                        titleStr = line;
                     }
                     else if (line.startsWith("TEST_GROUP"))
                     {
                        line.remove(0, 11); // Length of "TEST_GROUP:"
                        testGroup = line;
                     }
                     else if (line.startsWith("DESCRIPTION"))
                     {
                        line.remove(0, 12);
                        testDescription = line;
                     }
                     else if (line.startsWith("DETAILED_DESCRIPTION"))
                     {
                        QString descriptionLine = stream.readLine();
                        while (true)
                        {
                           descriptionLine.remove(0, 2); // Opening "//"
                           testDetailedDescription = testDetailedDescription + descriptionLine + "\n";
                           descriptionLine = stream.readLine();
                           if (descriptionLine.startsWith("//END_DETAILED_DESCRIPTION") || stream.atEnd())
                           {
                              break;
                           }
                        }
                     }
                     else if (line.startsWith("END_TEST_HEADER"))
                     {
                        break;
                     }
                     else if (stream.atEnd())
                     {
                        break;
                     }
                  }

                  break;
               }
               else if (stream.atEnd())
               {
                  break;
               }
            }

            if (containsHeader)
            {
               PerformanceTestData testInfo;
               testInfo.mTestDescription = testDescription;
               testInfo.mTestDetailedDescription = testDetailedDescription;
               testInfo.mTestFiles.push_back(aTestDir.absolutePath() + "/" + i);
               testInfo.mTestTitle = titleStr;
               testData.push_back(testInfo);
            }

         }
      }
   }


   std::sort(testData.begin(), testData.end(),
      [](const PerformanceTestData& t1, const PerformanceTestData& t2) {return t1.mTestTitle < t2.mTestTitle; });

   return testData;
}

void PerformanceWidget::PopulateTreeWidget(const std::vector<PerformanceTestData>& aTestData)
{
   mUIPtr->performanceTestTreeWidget->clear();

   for (const auto& testData : aTestData)
   {
      PerformanceTestTreeWidgetItem* newItem = new PerformanceTestTreeWidgetItem(testData.mTestFiles, nullptr);
      mUIPtr->performanceTestTreeWidget->addTopLevelItem(newItem);
      newItem->setToolTip(0, testData.mTestDescription);
      newItem->SetDetailedDescription(testData.mTestDetailedDescription);

      QRadioButton* itemRadioButton = new QRadioButton(testData.mTestTitle);
      mUIPtr->performanceTestTreeWidget->setItemWidget(newItem, 0, itemRadioButton);
      connect(itemRadioButton, &QRadioButton::toggled, this, &PerformanceWidget::HandleRadioButtonChanged);
   }

   mUIPtr->performancePlotWidget->GetPlot()->SetDefaultPlotStyle(UtQtGL2DPlot::LINE);
   ClearPlotArea();
}

void PerformanceWidget::PopulatePerformanceTests()
{
   PopulateTreeWidget(GetPerformanceTestData(mTestDir));
}

void PerformanceWidget::PopulateComparisonTests(const std::vector<QDir>& aTestDirs)
{
   // ---------------------- Find all tests for multiple movers ---------------------- //

   if (aTestDirs.size() <= 1) return;

   // Find tests whose titles show up more than once
   // Keep track of test titles that show up twice (sufficient to be added in Comparison list)
   // Push the second version of that test to the common list (all tests should be the same)

   std::map<QString, size_t> testTitleFrequency;
   std::vector<PerformanceTestData> commonTestData;

   for (const auto& testDir : aTestDirs)
   {
      std::vector<PerformanceTestData> dirTestData = GetPerformanceTestData(testDir);
      for (const auto& test : dirTestData)
      {
         if (testTitleFrequency.find(test.mTestTitle) == testTitleFrequency.end())
         {
            testTitleFrequency[test.mTestTitle] = 1;
         }
         else
         {
            if (testTitleFrequency[test.mTestTitle] == 1) { commonTestData.push_back(test); }

            testTitleFrequency[test.mTestTitle]++;
         }
      }
   }

   // Test data for all movers. 
   // Key is the mover directory name. (e.g. "gm", "p6dof", "rb6dof") 
   // Value is the performance data for the mover.

   std::map<QString, std::vector<PerformanceTestData>> testData;
   for (const auto& testDir : aTestDirs)
   {
      testData.emplace(testDir.dirName(), GetPerformanceTestData(testDir));
   }

   for (auto& data : commonTestData)
   {
      std::vector<QFileInfo> testFiles;
      // For all mover types, search for the test title
      for (const auto& allData : testData)
      {
         // Search for the item in the intersection in the full list of all tests
         auto iter = std::find_if(allData.second.begin(), allData.second.end(), 
            [data](const PerformanceTestData& t) {return t.mTestTitle == data.mTestTitle; });

         if (iter != allData.second.end())
         {
            std::copy(iter->mTestFiles.begin(), iter->mTestFiles.end(), std::back_inserter(testFiles));
         }
      }
      data.mTestFiles = testFiles;
   }

   // -------------------- Populate the tests in the widget --------------------- //
   PopulateTreeWidget(commonTestData);
}

void PerformanceWidget::HandleRadioButtonChanged(bool aChecked)
{
   // Loop through all the tree widget items and un-select them
   for (int i = 0; i < mUIPtr->performanceTestTreeWidget->topLevelItemCount(); ++i)
   {
      auto curItem = dynamic_cast<PerformanceTestTreeWidgetItem*>(mUIPtr->performanceTestTreeWidget->topLevelItem(i));
      QRadioButton* curRadioButton = dynamic_cast<QRadioButton*>(mUIPtr->performanceTestTreeWidget->itemWidget(curItem, 0));
      if (curRadioButton && curRadioButton->isChecked())
      {
         mUIPtr->performanceTestTreeWidget->setItemSelected(curItem, true);
      }
      else
      {
         mUIPtr->performanceTestTreeWidget->setItemSelected(curItem, false);
      }
   }
   if (aChecked)
   {
      DoParse(false);
   }
}

void PerformanceWidget::ShowPerformanceTestData()
{
   UtQtGL2DPlot* plot = ClearPlotArea();
   mUIPtr->mDetailedDescriptionTextEdit->setText("");
   mUIPtr->mSeriesSelectionTreeWidget->clear();
   mUIPtr->mSeriesSelectionTreeWidget->disconnect();

   auto curItem = GetCurrentTestItem();
   if (curItem)
   {
      QString detailedDescription = curItem->GetDetailedDescription();
      detailedDescription.append(curItem->GetTimeStampMessage());

      mUIPtr->mDetailedDescriptionTextEdit->setText(detailedDescription);
      if (curItem->GetTestData().empty())
      {
         plot->AddOverlayText("No Available Test Data", 0.5, 0.5);
      }
      else
      {
         UtQtGL2DPlot::StipplePattern lineStyle = UtQtGL2DPlot::NO_STIPPLE;

         for (const auto& s : curItem->GetTestData())
         {
            for (const auto& i : s.second)
            {
               QString seriesName  = i.first.second;
               int plotSeriesIndex = i.first.first.first;
               int seriesIndex     = i.first.first.second;
               int lineWidth       = 2;
               size_t plotColorIndex = seriesIndex % mPlotColorMap.size();
               QColor plotColor = mPlotColorMap[plotColorIndex];

               // If we are doing a comparison test,
               // we want to add the mover type to the legend key
               // and change the line style appropriately.
               if (curItem->GetTestData().size() > 1)
               {
                  if (s.first == "gm")
                  {
                     lineStyle = UtQtGL2DPlot::DASHED; // Dashed line
                     seriesName += ", Guided Mover";
                  }
                  else if (s.first == "p6dof")
                  {
                     lineStyle = UtQtGL2DPlot::NO_STIPPLE; // Dotted line
                     lineWidth = 1;
                     seriesName += ", P6DOF Mover";
                  }
                  else if (s.first == "pm6dof")
                  {
                     lineStyle = UtQtGL2DPlot::DOTTED; // Dotted line
                     seriesName += ", PM6DOF Mover";
                  }
                  else if (s.first == "rb6dof")
                  {
                     lineStyle = UtQtGL2DPlot::NO_STIPPLE; // Solid line
                     lineWidth = 3;
                     seriesName += ", RB6DOF Mover";
                  }
               }

               plot->AddPoints(i.second, plotSeriesIndex);
               plot->SetSeriesName(seriesName, plotSeriesIndex);
               plot->SetTitle(curItem->GetTitle());
               plot->SetLabelXAxis(curItem->GetX_Label());
               plot->SetLabelYAxis(curItem->GetY_Label());
               plot->SetSeriesWidth(lineWidth, plotSeriesIndex);
               plot->SetShowTickMarkLabels(true);
               mUIPtr->performancePlotWidget->GetPlot()->SetSeriesColor(plotColor, plotSeriesIndex);
               mUIPtr->performancePlotWidget->GetPlot()->SetSeriesStipple(lineStyle, plotSeriesIndex);
            }
         }

         // Add a checkbox for each series in the series selection widget
         for (const auto& n : curItem->GetSeriesNames())
         {
            QTreeWidgetItem* seriesItem = new QTreeWidgetItem(QStringList() << n);
            seriesItem->setCheckState(0, Qt::CheckState::Checked);
            mUIPtr->mSeriesSelectionTreeWidget->addTopLevelItem(seriesItem);
         }

         connect(mUIPtr->mSeriesSelectionTreeWidget, &QTreeWidget::itemChanged, this, &PerformanceWidget::SeriesSelectionChanged);
         connect(mUIPtr->mSelectAllButton, &QPushButton::clicked, this, [this]() {ChangeAllSeriesSelection(Qt::Checked); });
         connect(mUIPtr->mSelectNoneButton, &QPushButton::clicked, this, [this]() {ChangeAllSeriesSelection(Qt::Unchecked); });

         if (curItem->IsTurnPerformancePlot())
         {
            UtQtGL2DPlotTP* TP_Plot = dynamic_cast<UtQtGL2DPlotTP*>(plot);

            if (TP_Plot)
            {
               TP_Plot->SetShowBackgroundCurves(true);
            }
         }
         else
         {
            UtQtGL2DPlotTP* TP_Plot = dynamic_cast<UtQtGL2DPlotTP*>(plot);

            if (TP_Plot)
            {
               TP_Plot->SetShowBackgroundCurves(false);
            }
         }
      }
      plot->MakePlotPretty();
      mUIPtr->performancePlotWidget->Update();
   }
}

void PerformanceWidget::ExecuteTestButtonClicked()
{
   PerformanceTestTreeWidgetItem* curItem = GetCurrentTestItem();
   if (curItem == nullptr)
   {
      return;
   }

   curItem->ClearTestData();
   ClearPlotArea();

   if (!mPerformanceTestDialogPtr)
   {
      mPerformanceTestDialogPtr = new CalculateAeroDialog(this);
   }

   mPerformanceTestDialogPtr->SetLabelText("Calculating Vehicle Performance");
   mPerformanceTestDialogPtr->SetWindowTitle("Calculating Performance...");
   mPerformanceTestDialogPtr->setFixedSize(QSize(400, 100));
   mPerformanceTestDialogPtr->setModal(true);
   mPerformanceTestDialogPtr->show();

   mPerformanceTestThread->SetTestFiles(curItem->GetTestFiles());
   connect(mPerformanceTestThread, &PerformanceTestThread::finished, this, [this]() {DoParse(true); });
   mPerformanceTestThread->start();
}

void PerformanceWidget::showEvent(QShowEvent* aEvent)
{
   // TODO for 2.8.0:
   // If we are only generating a guided mover, all successive tabs are
   // disabled - we don't want to allow the user to move on to any other page
   // Once we have a relevant flight test to perform, we can remove this code
   MoverCreatorWidget* mcw = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget();
   mUIPtr->mNextButton->setEnabled(mcw->GenerateP6DOFMover() || mcw->GenerateRB6DOFMover() || mcw->GeneratePM6DOFMover());

   BuildMoverList();

   UtQtGL2DPlot* plot = mUIPtr->performancePlotWidget->GetPlot();
   QColor BGColor(0, 102, 204);
   plot->SetPlotBackgroundColor(BGColor);
   QColor GridColor(0, 128, 255);
   plot->SetGridColor(GridColor);
   plot->SetGridStipple(UtQtGL2DPlot::NO_STIPPLE);
   plot->SetShowTickMarkLabels(false);
   plot->SetTitle("");
   plot->SetLabelXAxis("");
   plot->SetLabelYAxis("");
   PopulatePerformanceTests();
   QWidget::showEvent(aEvent);
}

void PerformanceWidget::PreviousButtonClicked()
{
   MoverCreatorWidget* mcWidget = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget();
   mcWidget->MoveToPreviousTab();
}

void PerformanceWidget::NextButtonClicked()
{
   MoverCreatorWidget* mcWidget = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget();
   mcWidget->MoveToNextTab();
}

void PerformanceWidget::DoParse(bool aCheckOutputExists)
{
   mPerformanceTestThread->disconnect();
   if (mPerformanceTestDialogPtr)
   {
      mPerformanceTestDialogPtr->EndDialog();
   }

   PerformanceTestTreeWidgetItem* curItem = GetCurrentTestItem();
   if (curItem != nullptr)
   {
      auto testfiles = curItem->GetTestFiles();

      std::vector<QDir> applicableOutputDirs;
      std::copy_if(mOutputDirs.begin(), mOutputDirs.end(), std::back_inserter(applicableOutputDirs), [testfiles](const QDir& outputDir) 
      {
         return (std::find_if(testfiles.begin(), testfiles.end(),
            [outputDir](const QFileInfo& fileInfo) {return fileInfo.dir().dirName() == outputDir.dirName(); }) != testfiles.end());
      });


      QRadioButton* curRadioButton = dynamic_cast<QRadioButton*>(mUIPtr->performanceTestTreeWidget->itemWidget(curItem, 0));
      if (curRadioButton)
      {
         QString testName = curRadioButton->text().replace(" ", "_");
         MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
         QString vehicleName = QString::fromStdString(mw.GetMoverCreatorWidget()->GetVehicle()->GetVehicleName());
         QString outputFileName = "/" + vehicleName.toLower() + "_" + testName.toLower() + ".csv";

         bool outputFilesValid = true;
         if (aCheckOutputExists)
         {
            if (!mPerformanceTestThread->ExecutedCleanly())
            {
               QMessageBox::warning(nullptr, "Performance Test Error", "Performance test did not execute successfully. Please check the auto-generated mover files for errors.");
               return;
            }

            for (const auto& outputDir : applicableOutputDirs)
            {
               QFile file(outputDir.absolutePath() + outputFileName);
               if (!file.open(QIODevice::ReadOnly))
               {
                  outputFilesValid = false;
               }
               file.close();
            }
         }
         if (outputFilesValid)
         {
            curItem->ParseTestOutput(applicableOutputDirs, outputFileName);
         }
         else
         {
            QMessageBox::information(nullptr, "Error", "Unable to display performance data. The output file could not be found.");
         }
      }
   }
   ShowPerformanceTestData();
}

void PerformanceWidget::BeginScriptGeneration()
{
   MoverCreatorWidget* mw = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget();
   Vehicle* vehiclePtr = mw->GetVehicle();
   if (vehiclePtr != nullptr)
   {
      Vehicle::VehicleControlConfig config = vehiclePtr->GetVehicleControlConfiguration();
      bool hasControls = (config != Vehicle::cNO_CONTROL);

      if (mw->GenerateGuidedMover())
      {
         vehiclePtr->GetAero()->EnableDisableTables(false, true, false);
         GenerateGMScripts();
      }
      if (mw->GeneratePM6DOFMover())
      {
         vehiclePtr->GetAero()->EnableDisableTables(false, false, true);

         if (hasControls)
         {
            mAutopilotSupportProgressDialog = QSharedPointer<QProgressDialog>(new QProgressDialog(QString("Generate PM6DOF Test Scenario"), QString(), 0, 1000, this));
            mAutopilotSupportProgressDialog->setLabelText(QString("Generate PM6DOF Test Scenario"));
            mAutopilotSupportProgressDialog->setWindowModality(Qt::WindowModal);
            mAutopilotSupportProgressDialog->setWindowTitle(QString("Generating PM6DOF Autopilot Support File"));
            mAutopilotSupportProgressDialog->setMinimumDuration(0);
            mAutopilotSupportProgressDialog->setFixedSize(QSize(400, 100));
            mAutopilotSupportProgressDialog->setValue(0);
            mAutopilotSupportProgressDialog->setAutoReset(false);
            mAutopilotSupportProgressDialog->show();

            ShowAutopilotSupportProgress(0, QString("Creating Scenario for PM6DOF Autopilot Support Tables"));
         }

         GeneratePM6DOFScripts();
      }
      if (mw->GenerateP6DOFMover() || mw->GenerateRB6DOFMover())
      {
         vehiclePtr->GetAero()->EnableDisableTables(true, false, false);

         // Note: The Generate*Scripts functions take a while to run.
         // We need to invoke them on different threads so that the show event on the
         // progress dialog can be processed before the script generation completes.
         // Otherwise, the progress dialog will appear blank/white for the duration of
         // the Generate*Scripts function calls.
         
         if (mAutopilotSupportProgressDialog != nullptr)
         {
             mAutopilotSupportProgressDialog->close();
             mAutopilotSupportProgressDialog.reset();
         }
         
         if (mw->GenerateRB6DOFMover())
         {
            if (hasControls)
            {
               mAutopilotSupportProgressDialog = QSharedPointer<QProgressDialog>(new QProgressDialog(QString("Generate RB6DOF Test Scenario"), QString(), 0, 1000, this));
               mAutopilotSupportProgressDialog->setLabelText(QString("Generate RB6DOF Test Scenario"));
               mAutopilotSupportProgressDialog->setWindowModality(Qt::WindowModal);
               mAutopilotSupportProgressDialog->setWindowTitle(QString("Generating RB6DOF Autopilot Support File"));
               mAutopilotSupportProgressDialog->setMinimumDuration(0);
               mAutopilotSupportProgressDialog->setFixedSize(QSize(400, 100));
               mAutopilotSupportProgressDialog->setValue(0);
               mAutopilotSupportProgressDialog->setAutoReset(false);
               mAutopilotSupportProgressDialog->show();

               ShowAutopilotSupportProgress(0, QString("Creating Scenario for RB6DOF Autopilot Support Tables"));
            }

            GenerateRB6DOFScripts();
         }

         if (mAutopilotSupportProgressDialog != nullptr)
         {
            mAutopilotSupportProgressDialog->close();
            mAutopilotSupportProgressDialog.reset();
         }

         if (mw->GenerateP6DOFMover())
         {
            if (hasControls)
            {
               mAutopilotSupportProgressDialog = QSharedPointer<QProgressDialog>(new QProgressDialog(QString("Generate P6DOF Test Scenario"), QString(), 0, 1000, this));
               mAutopilotSupportProgressDialog->setWindowModality(Qt::WindowModal);
               mAutopilotSupportProgressDialog->setWindowTitle(QString("Generating P6DOF Autopilot Support File"));
               mAutopilotSupportProgressDialog->setMinimumDuration(0);
               mAutopilotSupportProgressDialog->setFixedSize(QSize(400, 100));
               mAutopilotSupportProgressDialog->setValue(0);
               mAutopilotSupportProgressDialog->setAutoReset(false);
               mAutopilotSupportProgressDialog->show();

               ShowAutopilotSupportProgress(0, QString("Creating Scenario for P6DOF Autopilot Support Tables"));
            }

            // P6DOF and SixDOF tables may be different, so re-eval
            GenerateP6DOFScripts();
         }
      }
   }
}

void PerformanceWidget::AutopilotSupportThreadFinished(bool aControllable)
{
   if (mAutopilotSupportProgressDialog != nullptr)
   {
      ShowAutopilotSupportProgress(1000, QString("Autopilot Support Data is Complete."));

      mAutopilotSupportProgressDialog->close();
      mAutopilotSupportProgressDialog.reset();
   }

   if (!aControllable)
   {
      MoverCreatorWidget* mcWidget = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget();
      mcWidget->DisableTabsAfterCurrent();
      mUIPtr->performanceTestTreeWidget->clear();

      QMessageBox::critical(nullptr, "Vehicle Uncontrollable", 
                                        "The vehicle is uncontrollable. Either modify the geometry, aerodynamics, or use a template with the cNO_CONTROL configuration." );

   }
}

void PerformanceWidget::ProcessApSupportStdOut(QString aSingleLine)
{
   if (mAutopilotSupportProgressDialog == nullptr) { return; }

   // The following line can be uncommented-out to provide console output that can be helpful when debugging
   ut::log::developer() << aSingleLine.toStdString();

   if (aSingleLine.contains("<<") && aSingleLine.contains(">>"))
   {
      if (aSingleLine.contains("PROGRESS"))
      {
         // Extract between the single quotes
         int indexFirst = aSingleLine.indexOf("'");
         aSingleLine.remove(0, indexFirst + 1);
         int indexLast = aSingleLine.lastIndexOf("'");
         int length = (int)aSingleLine.size();
         int numToChop = (length - indexLast);
         aSingleLine.chop(numToChop);

         // Get the integer
         int value = aSingleLine.toInt();
         ShowAutopilotSupportProgress(value);
      }
      else if (aSingleLine.contains("TEST_NAME"))
      {
         // Extract between the single quotes
         int indexFirst = aSingleLine.indexOf("'");
         aSingleLine.remove(0, indexFirst + 1);
         int indexLast = aSingleLine.lastIndexOf("'");
         int length = (int)aSingleLine.size();
         int numToChop = (length - indexLast);
         aSingleLine.chop(numToChop);

         QString labelText = "Processing " + aSingleLine;
         ShowAutopilotSupportProgress(0, labelText);
      }
      else if (aSingleLine.contains("LOADING"))
      {
         ShowAutopilotSupportProgress(0, QString("Autopilot Support AFSIM Scenario is Loading..."));
      }
      else if (aSingleLine.contains("BEGIN"))
      {
         ShowAutopilotSupportProgress(0, QString("Begin Processing Autopilot Support Data ..."));
      }
      else if (aSingleLine.contains("OUTPUT_FILES"))
      {
         // Extract between the single quotes
         int indexFirst = aSingleLine.indexOf("'");
         aSingleLine.remove(0, indexFirst + 1);
         int indexLast = aSingleLine.lastIndexOf("'");
         int length = (int)aSingleLine.size();
         int numToChop = (length - indexLast);
         aSingleLine.chop(numToChop);

         // Get the integer
         int value = aSingleLine.toInt();

         ShowAutopilotSupportProgress(value, QString("Output to files ..."));
      }
      else if (aSingleLine.contains("ERROR"))
      {
         mAutopilotSupportFileWork->SetControllable(false);
      }
   }
}

void PerformanceWidget::ShowAutopilotSupportProgress(int aValue, const QString& aMessage)
{
   if (mAutopilotSupportProgressDialog == nullptr)
   {
      return;
   }

   if (!aMessage.isEmpty())
   {
      mAutopilotSupportProgressDialog->setLabelText(aMessage);
   }
   mAutopilotSupportProgressDialog->setValue(aValue);
}

void PerformanceWidget::Reset()
{
   auto plotPtr = mUIPtr->performancePlotWidget->GetPlot();
   plotPtr->Reset();
   mClearingTests = true;
   mUIPtr->performanceTestTreeWidget->clear();
   mClearingTests = false;
   mUIPtr->mDetailedDescriptionTextEdit->clear();
}

void PerformanceWidget::GenerateP6DOFScripts()
{
   ScriptGeneratorP6DOF scriptGeneratorP6DOF;
   
   if (scriptGeneratorP6DOF.GenerateDataFiles())
   {
      PopulatePerformanceTests();
      GenerateAutopilotSupportTables(scriptGeneratorP6DOF.GetTestDir());
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("The script generator wasn't able to generate the data files!");
   }
}

void PerformanceWidget::GenerateRB6DOFScripts()
{
   ScriptGeneratorRigidBodySixDOF scriptGenerator;
   if (scriptGenerator.GenerateDataFiles())
   {
      PopulatePerformanceTests();
      GenerateAutopilotSupportTables(scriptGenerator.GetTestDir());
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("The script generator wasn't able to generate the data files!");
   }
}

void PerformanceWidget::GenerateAutopilotSupportTables(const QDir& aTestDir)
{
   Vehicle* vehiclePtr = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetVehicle();
   if (vehiclePtr != nullptr)
   {
      Vehicle::VehicleControlConfig config = vehiclePtr->GetVehicleControlConfiguration();

      if (config != Vehicle::cNO_CONTROL)
      {
         mAutopilotSupportFileWork.reset(new AutopilotSupportFileWork{ vehiclePtr, aTestDir });
         connect(mAutopilotSupportFileWork.get(), &AutopilotSupportFileWork::WorkFinished, this, &PerformanceWidget::AutopilotSupportThreadFinished);

         connect(mAutopilotSupportFileWork.get(), &AutopilotSupportFileWork::WorkProgress, this, &PerformanceWidget::ProcessApSupportStdOut);

         mAutopilotSupportFileWork->BeginWork();
      }
   }
}

UtQtGL2DPlot* PerformanceWidget::ClearPlotArea()
{
   UtQtGL2DPlot* plot = mUIPtr->performancePlotWidget->GetPlot();
   plot->ClearData();
   plot->RemoveAllOverlayText();
   plot->SetShowTickMarkLabels(false);
   plot->SetTitle("");
   plot->SetLabelXAxis("");
   plot->SetLabelYAxis("");
   UtQtGL2DPlotTP* TP_Plot = dynamic_cast<UtQtGL2DPlotTP*>(plot);

   if (TP_Plot)
   {
      TP_Plot->SetShowBackgroundCurves(false);
   }
   mUIPtr->performancePlotWidget->Update();
   mUIPtr->mDetailedDescriptionTextEdit->clear();
   mUIPtr->mSeriesSelectionTreeWidget->clear();
   return plot;
}

void PerformanceWidget::GenerateGMScripts()
{
   ScriptGeneratorGM scriptGenerator;
   if (!scriptGenerator.GenerateDataFiles())
   {
      PopulatePerformanceTests();
      MoverCreatorMainWindow().CreateErrorBox("The script generator wasn't able to generate the data files!");
   }
}

void PerformanceWidget::GeneratePM6DOFScripts()
{
    ScriptGeneratorPointMassSixDOF scriptGenerator;
    if (scriptGenerator.GenerateDataFiles())
    {
        PopulatePerformanceTests();
        GenerateAutopilotSupportTables(scriptGenerator.GetTestDir());
    }
    else
    {
        MoverCreatorMainWindow().CreateErrorBox("The script generator wasn't able to generate the data files!");
    }
}

void PerformanceWidget::BuildMoverList()
{
   mUIPtr->mMoverComboBox->clear();
   MoverCreatorWidget* mcw = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget();

   if (mcw->GenerateRB6DOFMover())
   {
      mUIPtr->mMoverComboBox->addItem("RB6DOF Mover");
   }
   if (mcw->GeneratePM6DOFMover())
   {
       mUIPtr->mMoverComboBox->addItem("PM6DOF Mover");
   }
   if (mcw->GenerateP6DOFMover())
   {
      mUIPtr->mMoverComboBox->addItem("P6DOF Mover");
   }
   if (mcw->GenerateGuidedMover())
   {
      mUIPtr->mMoverComboBox->addItem("Guided Mover");
   }
   if (mcw->GetMoverCount() > 1)
   {
      mUIPtr->mMoverComboBox->addItem("Comparison");
   }
}

PerformanceTestTreeWidgetItem* PerformanceWidget::GetCurrentTestItem()
{
   if (!mClearingTests)
   {
      for (int i = 0; i < mUIPtr->performanceTestTreeWidget->topLevelItemCount(); ++i)
      {
         auto curItem = dynamic_cast<PerformanceTestTreeWidgetItem*>(mUIPtr->performanceTestTreeWidget->topLevelItem(i));
         QRadioButton* curRadioButton = dynamic_cast<QRadioButton*>(mUIPtr->performanceTestTreeWidget->itemWidget(curItem, 0));
         if (curRadioButton && curRadioButton->isChecked())
         {
            return curItem;
         }
      }
   }
   return nullptr;
}

void PerformanceWidget::SetPlotColors()
{
   // Load up the colors for the plots
   mPlotColorMap.emplace_back(255, 50, 50);
   mPlotColorMap.emplace_back(50, 255, 50);
   mPlotColorMap.emplace_back(50, 255, 255);
   mPlotColorMap.emplace_back(255, 50, 255);
   mPlotColorMap.emplace_back(255, 255, 50);
   mPlotColorMap.emplace_back(175, 175, 175);
   mPlotColorMap.emplace_back(175, 20, 20);
   mPlotColorMap.emplace_back(20, 175, 20);
   mPlotColorMap.emplace_back(20, 20, 175);
   mPlotColorMap.emplace_back(20, 175, 175);
   mPlotColorMap.emplace_back(175, 20, 175);
   mPlotColorMap.emplace_back(175, 175, 20);
   mPlotColorMap.emplace_back(75, 75, 75);
}

void PerformanceWidget::SeriesSelectionChanged(QTreeWidgetItem* aItem, int aColumn)
{
   auto plot = mUIPtr->performancePlotWidget->GetPlot();
   auto allTestData = GetCurrentTestItem()->GetTestData();
   for (const auto& testData : allTestData)
   {
      QString seriesName = aItem->text(aColumn);
      auto it = std::find_if(testData.second.begin(), testData.second.end(), 
                     [seriesName](std::pair<SeriesId, SeriesData> series) 
                     {return series.first.second == seriesName; });
      if (it != testData.second.end())
      {
         int plotSeriesIndex = it->first.first.first;
         int seriesIndex = it->first.first.second;
         size_t plotColorIndex = seriesIndex % mPlotColorMap.size();
         QColor plotColor = mPlotColorMap[plotColorIndex];
         switch (aItem->checkState(aColumn))
         {
            case Qt::Unchecked:
               mUIPtr->performancePlotWidget->GetPlot()->RemoveSeries(it->first.first.first);
               break;
            case Qt::Checked:
            {
               // If we are doing a comparison test,
               // we want to add the mover type to the legend key
               // and change the line style appropriately.
               UtQtGL2DPlot::StipplePattern lineStyle = UtQtGL2DPlot::NO_STIPPLE;
               int lineWidth = 2;
               if (allTestData.size() > 1)
               {
                  if (testData.first == "gm")
                  {
                     lineStyle = UtQtGL2DPlot::DASHED; // Dashed line
                     seriesName += ", Guided Mover";
                  }
                  else if (testData.first == "p6dof")
                  {
                     lineStyle = UtQtGL2DPlot::NO_STIPPLE; // Dotted line
                     lineWidth = 1;
                     seriesName += ", P6DOF Mover";
                  }
                  else if (testData.first == "pm6dof")
                  {
                     lineStyle = UtQtGL2DPlot::DOTTED; // Dotted line
                     seriesName += ", PM6DOF Mover";
                  }
                  else if (testData.first == "rb6dof")
                  {
                     lineStyle = UtQtGL2DPlot::NO_STIPPLE; // Solid line
                     lineWidth = 3;
                     seriesName += ", RB6DOF Mover";
                  }
               }

               plot->AddPoints(it->second, it->first.first.first);
               plot->SetSeriesName(seriesName, it->first.first.first);
               plot->SetSeriesWidth(lineWidth, plotSeriesIndex);
               plot->SetShowTickMarkLabels(true);
               plot->SetSeriesColor(plotColor, plotSeriesIndex);
               plot->SetSeriesStipple(lineStyle, plotSeriesIndex);
               break;
            }
            default:
               break;
         }
      }
   }
}

void PerformanceWidget::ChangeAllSeriesSelection(Qt::CheckState aCheckState)
{
   int numSeries = mUIPtr->mSeriesSelectionTreeWidget->topLevelItemCount();

   for (int i = 0; i < numSeries; ++i)
   {
      mUIPtr->mSeriesSelectionTreeWidget->topLevelItem(i)->setCheckState(0, aCheckState);
   }

}

} // namespace Designer
