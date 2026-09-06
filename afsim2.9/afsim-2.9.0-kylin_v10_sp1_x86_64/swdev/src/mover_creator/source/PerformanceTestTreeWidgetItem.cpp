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

#include "PerformanceTestTreeWidgetItem.hpp"

#include <QDateTime>
#include <QFile>
#include <QMessageBox>
#include <QMetaObject>
#include <QObject>
#include <QProcess>
#include <QTextStream>
#include <QThread>

#include "MoverCreatorEnvironment.hpp"
#include "MoverCreatorWidget.hpp"
#include "PerformanceTestThread.hpp"
#include "UtLog.hpp"

namespace Designer
{

PerformanceTestTreeWidgetItem::PerformanceTestTreeWidgetItem(const std::vector<QFileInfo>& aTestFiles,
                                                             QTreeWidgetItem*   aParent)
   : QTreeWidgetItem(aParent)
   , mTestFiles(aTestFiles)
   , mTitle("")
   , mY_Label("")
   , mX_Label("")
   , mTurnPerformance(false)
   , mTP_SpeedMultiplier(0)
{
}

QString PerformanceTestTreeWidgetItem::GetTimeStampMessage() const
{
   QString dateFormat = "hh:mm:ss dd MMMM yyyy";
   QString message = "\n\nLast Updated:\n\n";

   if (IsComparisonTest())
   {
      MoverCreatorWidget* mw = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget();

      if (mw->GenerateGuidedMover())
      {
         message.append("Guided Mover:   ");
         auto ts = mTestTimeStamps.find("gm");

         if (ts != mTestTimeStamps.end()) // GM test exists and has been executed
         {
            message.append(ts->second.toString(dateFormat));
         }
         else // GM test has not been executed
         {
            auto it = find_if(mTestFiles.begin(), mTestFiles.end(), 
                      [](const QFileInfo & fileInfo) {return fileInfo.dir().dirName() == "gm"; });

            if (it != mTestFiles.end()) // GM test exists, but has not been executed
            {
               message.append("Data Not Available");
            }
            else // GM test does not exist
            {
               message.append("Test Not Available");
            }
         }
         message.append("\n");
      }
      if (mw->GenerateP6DOFMover())
      {
         message.append("P6DOF Mover:   ");
         auto ts = mTestTimeStamps.find("p6dof");

         if (ts != mTestTimeStamps.end())
         {
            message.append(ts->second.toString(dateFormat));
         }
         else // P6DOF test has not been executed
         {
            auto it = find_if(mTestFiles.begin(), mTestFiles.end(),
                     [](const QFileInfo & fileInfo) {return fileInfo.dir().dirName() == "p6dof"; });

            if (it != mTestFiles.end()) // P6DOF test exists, but has not been executed
            {
               message.append("Data Not Available");
            }
            else // P6DOF test does not exist
            {
               message.append("Test Not Available");
            }
         }
         message.append("\n");
      }
      if (mw->GenerateRB6DOFMover())
      {
         message.append("RB6DOF Mover: ");
         auto ts = mTestTimeStamps.find("rb6dof");

         if (ts != mTestTimeStamps.end())
         {
            message.append(ts->second.toString(dateFormat));
         }
         else // RB6DOF test has not been executed
         {
            auto it = find_if(mTestFiles.begin(), mTestFiles.end(),
               [](const QFileInfo & fileInfo) {return fileInfo.dir().dirName() == "rb6dof"; });

            if (it != mTestFiles.end()) // RB6DOF test exists, but has not been executed
            {
               message.append("Data Not Available");
            }
            else // RB6DOF test does not exist
            {
               message.append("Test Not Available");
            }
         }
         message.append("\n");
      }
      if (mw->GeneratePM6DOFMover())
      {
         message.append("PM6DOF Mover: ");
         auto ts = mTestTimeStamps.find("pm6dof");

         if (ts != mTestTimeStamps.end())
         {
            message.append(ts->second.toString(dateFormat));
         }
         else // PM6DOF test has not been executed
         {
            auto it = find_if(mTestFiles.begin(), mTestFiles.end(),
               [](const QFileInfo& fileInfo) {return fileInfo.dir().dirName() == "pm6dof"; });

            if (it != mTestFiles.end()) // PM6DOF test exists, but has not been executed
            {
               message.append("Data Not Available");
            }
            else // PM6DOF test does not exist
            {
               message.append("Test Not Available");
            }
         }
         message.append("\n");
      }
   }
   else
   {
      if (mTestTimeStamps.size() == 1)
      {
         message.append(mTestTimeStamps.begin()->second.toString(dateFormat));
      }
      else
      {
         message.append("Data Not Available");
      }
   }
   return message;
}

void PerformanceTestTreeWidgetItem::ParseTestOutput(const std::vector<QDir>& aOutputDirs, const QString& aFileName)
{
   mTestDataMap.clear();
   mSeriesNames.clear();
   mTestTimeStamps.clear();

   std::vector<std::pair<SeriesId, SeriesData>> testData;
   int seriesIndex = 1;
   for (const auto& outputDir : aOutputDirs)
   {
      testData.clear();
      QString dirName = outputDir.dirName();
      QFile file(outputDir.absolutePath() + aFileName);
      QFileInfo fileInfo(file);
      if (file.open(QIODevice::ReadOnly))
      {
         QTextStream in(&file);

         if (!in.atEnd())
         {
            // Check for Meta Data
            // Read Top Line for Headers
            QString line = in.readLine();
            QStringList fields = line.split(",");

            //If there was Meta Data present, skip to next line, this line has been parsed.
            if (CheckForMetaData(fields))
            {
               if (in.atEnd())
               {
                  //error
               }
               else //read next line
               {
                  fields = in.readLine().split(",");
               }
            }

            // Check for Headers
            bool headerPresent = false;
            // Identify which columns have data in them
            int col = 0;
            std::map<int, QString> columns;
            for (auto& str : fields)
            {
               if (str != "")
               {
                  bool ok = false;
                  str.toDouble(&ok);

                  if (ok) //First Row is data and not strings, No Headers
                  {
                     columns[col] = "";
                  }
                  else //Header located
                  {
                     headerPresent = true;
                     columns[col] = str;
                  }
               }
               col++;
            }

            mX_Label = columns[0];

            // Create Series Pairs by grouping columns together that contain data
            // independent var column, dependent var column
            std::vector< std::pair<int, int>> series;
            int previousKey = -10; //Some bogus value
            int independentVarCol = -10; //Another bogus value
            for (auto& i : columns)
            {
               // If the previous column was empty, then this column is treated as an Independent variable
               if (i.first - 1 != previousKey)
               {
                  independentVarCol = i.first;
               }
               else //This column is treated as an Dependent variable
               {
                  series.push_back(std::pair<int, int>(independentVarCol, i.first));
               }
               previousKey = i.first;
            }

            // Parse all data present in the file and store in column vectors
            // If there is no header, Read the first line as if it is data
            std::map<int, std::vector<double>> columnData;
            if (!headerPresent)
            {
               //Read the first line as if it is data (in case there was no header)
               bool ok = false;
               col = 0;

               for (auto& i : fields)
               {
                  double value = i.toDouble(&ok);
                  if (ok)
                  {
                     columnData[col].emplace_back(value);
                  }
                  else if (i != "")
                  {
                     //mErrors.push_back(QString("Warning: ") + i + " is not a number.");
                  }
                  col++;
               }
            }

            while (!in.atEnd())
            {
               bool ok = false;
               col = 0;

               for (auto& i : in.readLine().split(","))
               {
                  double value = i.toDouble(&ok);
                  if (ok)
                  {
                     columnData[col].emplace_back(value);
                  }
                  else if (i != "")
                  {
                     //mErrors.push_back(QString("Warning: ") + i + " is not a number.");
                  }
                  col++;
               }
            }
            int index = 0;
            for (auto& i : series)
            {
               QString seriesName = columns[i.second];
               std::vector<std::pair<double, double>> data;
               for (size_t j = 0; j < columnData[i.first].size(); ++j)
               {
                  data.emplace_back(columnData[i.first][j], columnData[i.second][j]);
               }
               testData.emplace_back(std::make_pair(std::make_pair(seriesIndex, index), seriesName), data);
               ++seriesIndex;
               ++index;
               if (std::find(mSeriesNames.begin(), mSeriesNames.end(), seriesName) == mSeriesNames.end())
               {
                  mSeriesNames.push_back(seriesName);
               }
            }
         }
         mTestDataMap.emplace_back(dirName, testData);
         mTestTimeStamps.emplace(dirName, fileInfo.lastModified());
         file.close();
      }
   }
}

bool PerformanceTestTreeWidgetItem::CheckForMetaData(const QStringList& aStrings)
{
   if (!aStrings.empty() && aStrings.at(0) == "Meta")
   {
      for (auto& str : aStrings)
      {
         if (str.startsWith("title="))
         {
            mTitle = str;
            mTitle.remove(0, 6); //6 = length of "title="
         }
         else if (str.startsWith("y_label="))
         {
            mY_Label = str;
            mY_Label.remove(0, 8);//8 = length of "y_label="
         }
         else if (str.startsWith("tp_unit="))
         {
            QString temp = str;
            temp = temp.remove(0, 8);//8 = length of "tp_unit="
            int unit = UtUnitSpeed::ReadUnit(temp.toStdString());
            if (UtUnitSpeed::IsUnitValid(unit))
            {
               mTP_SpeedUnit = (UtUnitSpeed::BaseUnit)unit;
               mTurnPerformance = true;
            }
            else
            {
               //Warning: unit not valid, so disable background lines.
               mTurnPerformance = false;
               mTP_SpeedUnit = UtUnitSpeed::cFEET_PER_SECOND;
            }
         }
         else if (str.startsWith("tp_multiplier="))
         {
            QString temp = str;
            temp = temp.remove(0, 14);//8 = length of "tp_multiplier="
            bool ok = false;
            double multiplier = temp.toDouble(&ok);
            if (ok)
            {
               mTurnPerformance = true;
               mTP_SpeedMultiplier = multiplier;
            }
            else
            {
               mTurnPerformance = false;
            }
         }
      }
      return true;
   }
   return false;
}

} // namespace Designer
