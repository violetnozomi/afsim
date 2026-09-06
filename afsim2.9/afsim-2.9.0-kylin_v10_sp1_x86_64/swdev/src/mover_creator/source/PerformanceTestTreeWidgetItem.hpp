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

#ifndef PERFORMANCE_TEST_TREE_WIDGET_HPP
#define PERFORMANCE_TEST_TREE_WIDGET_HPP

#include <unordered_map>

#include <QDir>
#include <QFile>
#include <QTreeWidget>

#include "UtUnitTypes.hpp"

using SeriesData = std::vector<std::pair<double, double>>;
using SeriesId = std::pair<std::pair<int, int>, QString>; // [ [seriesIndex, colorIndex], seriesName] ]
using MoverSeriesData = std::vector<std::pair<SeriesId, SeriesData>>;

namespace Designer
{
   class PerformanceTestTreeWidgetItem : public QTreeWidgetItem
   {
      public:
         explicit PerformanceTestTreeWidgetItem(const std::vector<QFileInfo>& aTestFiles,
                                                QTreeWidgetItem*   aParent = nullptr);

         ~PerformanceTestTreeWidgetItem() = default;

         std::vector<QFileInfo> GetTestFiles() const { return mTestFiles; }

         std::vector<std::pair<QString, MoverSeriesData>>& GetTestData()    { return mTestDataMap; }
         void ClearTestData() { mTestDataMap.clear(); }
         bool TestIsValidForMover() const;

         QString GetTitle() const { return mTitle; }
         QString GetX_Label() const { return mX_Label; }
         QString GetY_Label() const { return mY_Label; }

         const std::map<QString, QDateTime> GetTimeStamps() const { return mTestTimeStamps; }
         std::vector<QString> GetSeriesNames() { return mSeriesNames; }
         bool IsTurnPerformancePlot() const { return mTurnPerformance; }

         bool IsComparisonTest() const { return mTestFiles.size() > 1; }

         void ParseTestOutput(const std::vector<QDir>& aOutputDirs, const QString& aFileName);

         void SetDetailedDescription(const QString& aDescription) { mDetailedDescription = aDescription; }
         QString GetDetailedDescription() const { return mDetailedDescription; }
         QString GetTimeStampMessage() const; // Get the time stamp message that will be appended onto the description

      signals:
         void TestComplete();

      private:
         bool CheckForMetaData(const QStringList& strings);

         std::vector<QFileInfo>                                mTestFiles;
         QString                                               mTitle;
         QString                                               mY_Label;
         QString                                               mX_Label;

         bool                                                  mTurnPerformance;
         UtUnitSpeed::BaseUnit                                 mTP_SpeedUnit;
         double                                                mTP_SpeedMultiplier;

         std::vector<std::pair<QString, MoverSeriesData>>      mTestDataMap;
         std::vector<QString>                                  mSeriesNames;
         QString                                               mDetailedDescription;
         std::map<QString, QDateTime>                          mTestTimeStamps;
   };
}
#endif // !PERFORMANCE_TEST_TREE_WIDGET_HPP
