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

#ifndef PERFORMANCE_WIDGET_HPP
#define PERFORMANCE_WIDGET_HPP

#include "AutopilotSupportFileWork.hpp"

#include <memory>

#include <QWidget>

class QFile;
class UtQtGL2DPlot;
namespace Ui
{
   class PerformanceWidget;
}

namespace Designer
{
   class CalculateAeroDialog;
   class PerformanceTestTreeWidgetItem;
   class PerformanceTestThread;
   class PerformanceWidget : public QWidget
   {
      struct PerformanceTestData
      {
         QString                mTestTitle;
         QString                mTestDescription;
         QString                mTestDetailedDescription;
         std::vector<QFileInfo> mTestFiles;
      };

         Q_OBJECT
      public:
         explicit PerformanceWidget(QWidget*        aParent,
                                    Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~PerformanceWidget();

         void Initialize();
         void ShowPerformanceTestData();
         void HandleRadioButtonChanged(bool aChecked);
         void ExecuteTestButtonClicked();
         void PreviousButtonClicked();
         void NextButtonClicked();
         void BeginScriptGeneration();
         void DoParse(bool aCheckOutputExists = true);
         void AutopilotSupportThreadFinished(bool aControllable);
         void showEvent(QShowEvent* aEvent);

         void ProcessApSupportStdOut(QString aSingleLine);
         void ShowAutopilotSupportProgress(int aValue, const QString& aMessage = QString{});

         void Reset();

         Q_INVOKABLE void GenerateP6DOFScripts();
         Q_INVOKABLE void GenerateRB6DOFScripts();
         Q_INVOKABLE void GeneratePM6DOFScripts();

      private:

         UtQtGL2DPlot* ClearPlotArea(); // Clear the plot area and return the cleared plot
         void GenerateAutopilotSupportTables(const QDir& aTestDir);
         void GenerateGMScripts();
         void BuildMoverList();
         void HandleMoverChanged(const QString& aMoverText);
         std::vector<PerformanceTestData> GetPerformanceTestData(const QDir& aTestDir);
         void PopulateTreeWidget(const std::vector<PerformanceTestData>& aTestData);
         void PopulatePerformanceTests();
         void PopulateComparisonTests(const std::vector<QDir>& aTestDirs);

         // Return the currently checked radio button on the tree widget
         PerformanceTestTreeWidgetItem* GetCurrentTestItem();

         void SetPlotColors();

         void SeriesSelectionChanged(QTreeWidgetItem* aItem, int aColumn);
         void ChangeAllSeriesSelection(Qt::CheckState aCheckState);

         Ui::PerformanceWidget*                    mUIPtr                          = nullptr;
         CalculateAeroDialog*                      mPerformanceTestDialogPtr       = nullptr;
         std::unique_ptr<AutopilotSupportFileWork> mAutopilotSupportFileWork       = nullptr;
         QSharedPointer<QProgressDialog>           mAutopilotSupportProgressDialog = nullptr;

         std::vector<QColor>                       mPlotColorMap;
         QDir                                      mTestDir; 
         std::vector<QDir>                         mOutputDirs;
         PerformanceTestThread*                    mPerformanceTestThread          = nullptr;

         bool mClearingTests = false;
   };
}

Q_DECLARE_METATYPE(QDir);

#endif // !PERFORMANCE_WIDGET_HPP
