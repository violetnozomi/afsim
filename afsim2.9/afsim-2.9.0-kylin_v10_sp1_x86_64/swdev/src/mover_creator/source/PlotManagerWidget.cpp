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

#include "PlotManagerWidget.hpp"

#include <QJsonArray>

#include "AeroTable4d.hpp"
#include "AeroTableBase.hpp"
#include "UtCast.hpp"
#include "UtMemory.hpp"
#include "VehicleAeroCore.hpp"

#include "ui_PlotManagerWidget.h"

namespace Designer
{

PlotManagerWidget::PlotManagerWidget(QWidget*        aParent,
                                     Qt::WindowFlags aFlags/* = Qt::WindowFlags()*/)
   : QWidget(aParent, aFlags)
   , mNextMapKey(2)
   , mKnotEditorDialog(new KnotEditorDialog(this))
   , mKnotAdderDialog(new KnotAdderDialog(this))
   , mContextMenu(new QMenu(this))
   , mUIPtr(new Ui::PlotManagerWidget)
{
   mUIPtr->setupUi(this);

   UtQtGL2DPlot* plot = mUIPtr->plotWidget->GetPlot();
   mUIPtr->plotWidget->SetEnableCrosshairMouseControlled(true);
   plot->SetAutoScaleEnabled(false);
   plot->SetDefaultPlotStyle(UtQtGL2DPlot::LINE);
   plot->SetTickJustifiedX(false);
   plot->SetTickIncrementX(5);
   plot->SetShowCrosshairs(false);
   plot->SetUseMultipleColorsForSeries(false);

   mUIPtr->plotWidget->SetEnableEditMode(true);
   mUIPtr->plotWidget->SetEditModeChangeIncrementY(0.01);
   mUIPtr->plotWidget->SetEditModeChangeIncrementX(0.01);
   mUIPtr->plotWidget->setContextMenuPolicy(Qt::CustomContextMenu);

   connect(plot, &UtQtGL2DPlot::DataPointEdited, this, &PlotManagerWidget::HandlePlotManipulation);
   connect(plot, &UtQtGL2DPlot::DataPointSelected, this, &PlotManagerWidget::HandleDataPointSelected);
   connect(mKnotAdderDialog, &KnotAdderDialog::CreateKnot, this, &PlotManagerWidget::HandleAddPointCommand);
   connect(mUIPtr->plotWidget, &UtQtGL2DPlotWidget::customContextMenuRequested, this, &PlotManagerWidget::ShowContextMenu);
   SetPlotColors();
   UpdateZeroLines();
}

PlotManagerWidget::~PlotManagerWidget()
{
   delete mUIPtr;
}

void PlotManagerWidget::AddPlot(PlotDataSet* aPlotDataSetPtr, double aSecondaryIndependentVal, double aTertiaryIndependentVal, bool aAddtoPlot)
{
   mPlotDataSetMap[mNextMapKey] = aPlotDataSetPtr;
   //mModifierPlotDataSetMap[mNextMapKey] = new PlotDataSet(*aPlotDataSetPtr);
   mModifierPlotDataSetMap.emplace(mNextMapKey, ut::make_unique<PlotDataSet>(*aPlotDataSetPtr));
   mPlotDataSetValueMap[mModifierPlotDataSetMap[mNextMapKey].get()] = std::pair<double, double>(aSecondaryIndependentVal, aTertiaryIndependentVal);

   if (aAddtoPlot)
   {
      UtQtGL2DPlot* plot = mUIPtr->plotWidget->GetPlot();

      plot->AddPoints(aPlotDataSetPtr->GetPoints(), mNextMapKey);
      plot->SetSeriesPlotStyle(plot->GetSeriesName(mNextMapKey), UtQtGL2DPlot::POINT);
      plot->SetSeriesWidth(5, mNextMapKey);
      plot->AddPoints(aPlotDataSetPtr->GetPoints(), mNextMapKey + 1);
      plot->SetSeriesPlotStyle(plot->GetSeriesName(mNextMapKey + 1), UtQtGL2DPlot::LINE);
      plot->SetSeriesWidth(2, mNextMapKey + 1);
      plot->AddPoints(aPlotDataSetPtr->GetPoints(), mNextMapKey + 2);
      plot->SetSeriesPlotStyle(plot->GetSeriesName(mNextMapKey + 2), UtQtGL2DPlot::LINE);
      plot->SetSeriesWidth(2, mNextMapKey + 2);
      size_t plotColorIndex = ((mUIPtr->plotWidget->GetPlot()->GetNumberOfSeries() / 3) - 1) % mPlotColorMap.size();
      QColor plotColor = mPlotColorMap[plotColorIndex];
      mUIPtr->plotWidget->GetPlot()->SetSeriesColor(plotColor, mNextMapKey);
      mUIPtr->plotWidget->GetPlot()->SetSeriesColor(plotColor, mNextMapKey + 1);
      mUIPtr->plotWidget->GetPlot()->SetSeriesColor(plotColor, mNextMapKey + 2);
      mUIPtr->plotWidget->Update();
      plot->MakePlotPretty();
      UpdateZeroLines();

      plot->SetYAxisBounds2(mModifierYAxisMin, mModifierYAxisMax);
   }
   mNextMapKey += 3;
}

void PlotManagerWidget::RemovePlot(PlotDataSet* aPlotDataSetPtr)
{
   for (auto& i : mPlotDataSetMap)
   {
      if (i.second == aPlotDataSetPtr)
      {
         mPlotDataSetMap.erase(i.first);
         mUIPtr->plotWidget->GetPlot()->RemoveSeries(i.first);
         mUIPtr->plotWidget->GetPlot()->RemoveSeries(i.first + 1);
         mUIPtr->plotWidget->GetPlot()->RemoveSeries(i.first + 2);
      }
   }
}

void PlotManagerWidget::ClearAllPlots()
{
   for (auto& i : mPlotDataSetMap)
   {
      mUIPtr->plotWidget->GetPlot()->RemoveSeries(i.first);
      mUIPtr->plotWidget->GetPlot()->RemoveSeries(i.first + 1);
      mUIPtr->plotWidget->GetPlot()->RemoveSeries(i.first + 2);
   }
}

void PlotManagerWidget::ClearPlots()
{
   mPlotDataSetMap.clear();
   mModifierPlotDataSetMap.clear();
   mPlotDataSetValueMap.clear();
   mModifierPoints.clear();
   mUIPtr->plotWidget->GetPlot()->Reset();
   mNextMapKey = 2;
   mModifierPlotIndex = 2;
}

void PlotManagerWidget::SetPlotVisible(PlotDataSet* aPlotDataSetPtr, bool aVisible, QTreeWidgetItem* aItem)
{
   for (auto& i : mPlotDataSetMap)
   {
      if (i.second == aPlotDataSetPtr)
      {
         //first check to see if this plot already exists, if not, add it
         UtQtGL2DPlot* plot = mUIPtr->plotWidget->GetPlot();
         int seriesNum = i.first;
         const UtQtGL2DPlot::SeriesBase* foundSeries = plot->FindSeries(seriesNum);
         if (foundSeries == nullptr || foundSeries->Size() == 0)
         {
            // Points
            plot->AddPoints(aPlotDataSetPtr->GetPoints(), seriesNum);
            if (aItem != nullptr)
            {
               if (aItem->parent() != nullptr)
               {
                  QString seriesTitle(aItem->parent()->text(0) + " " + aItem->text(0) + " (Points)");
                  plot->SetSeriesName(seriesTitle, seriesNum);
               }
               else
               {
                  plot->SetSeriesName(GetDependentVarName() + " (Points)", seriesNum);
               }
            }
            else
            {
               plot->SetSeriesName(GetDependentVarName() + " (Points)", seriesNum);
            }
            plot->SetSeriesPlotStyle(plot->GetSeriesName(seriesNum), UtQtGL2DPlot::POINT);
            plot->SetSeriesVisible(aVisible, seriesNum);
            plot->SetSeriesWidth(5, seriesNum);

            // Line
            plot->AddPoints(aPlotDataSetPtr->GetPoints(), seriesNum + 1);
            if (aItem != nullptr)
            {
               if (aItem->parent() != nullptr)
               {
                  QString seriesTitle(aItem->parent()->text(0) + " " + aItem->text(0) + " (Line)");
                  plot->SetSeriesName(seriesTitle, seriesNum + 1);
               }
               else
               {
                  plot->SetSeriesName(GetDependentVarName() + " (Line)", seriesNum + 1);
               }
            }
            else
            {
               plot->SetSeriesName(GetDependentVarName() + " (Line)", seriesNum + 1);
            }
            plot->SetSeriesPlotStyle(plot->GetSeriesName(seriesNum + 1), UtQtGL2DPlot::LINE);
            plot->SetSeriesVisible(aVisible, seriesNum + 1);
            plot->SetSeriesWidth(2, seriesNum + 1);

            // Modified
            plot->AddPoints(aPlotDataSetPtr->GetPoints(), seriesNum + 2);
            if (aItem != nullptr)
            {
               if (aItem->parent() != nullptr)
               {
                  QString seriesTitle(aItem->parent()->text(0) + " " + aItem->text(0) + " (Modified)");
                  plot->SetSeriesName(seriesTitle, seriesNum + 2);
               }
               else
               {
                  plot->SetSeriesName(GetDependentVarName() + " (Modified)", seriesNum + 2);
               }
            }
            else
            {
               plot->SetSeriesName(GetDependentVarName() + " (Modified)", seriesNum + 2);
            }
            plot->SetSeriesPlotStyle(plot->GetSeriesName(seriesNum + 2), UtQtGL2DPlot::LINE);
            plot->SetSeriesVisible(aVisible, seriesNum + 2);
            plot->SetSeriesWidth(2, seriesNum + 2);

            size_t plotColorIndex = ((mUIPtr->plotWidget->GetPlot()->GetNumberOfSeries() / 3) - 1) % mPlotColorMap.size();
            QColor plotColor = mPlotColorMap[plotColorIndex];
            mUIPtr->plotWidget->GetPlot()->SetSeriesColor(plotColor, seriesNum);
            mUIPtr->plotWidget->GetPlot()->SetSeriesColor(plotColor, seriesNum + 1);
            mUIPtr->plotWidget->GetPlot()->SetSeriesColor(plotColor, seriesNum + 2);

            SyncModifierPlot(seriesNum + 2);
            mUIPtr->plotWidget->Update();
         }
         else
         {
            plot->SetSeriesName(GetDependentVarName() + " (Points)", i.first);
            plot->SetSeriesVisible(aVisible, i.first);

            plot->SetSeriesName(GetDependentVarName() + " (Line)", i.first + 1);
            plot->SetSeriesVisible(aVisible, i.first + 1);

            plot->SetSeriesName(GetDependentVarName() + " (Modified)", i.first + 2);
            plot->SetSeriesVisible(aVisible, i.first + 2);
         }
      }
   }

   mUIPtr->plotWidget->GetPlot()->RemoveSeries(0);
   mUIPtr->plotWidget->GetPlot()->RemoveSeries(1);
   mUIPtr->plotWidget->Update();
   mUIPtr->plotWidget->GetPlot()->MakePlotPretty();
   UpdateZeroLines();
}

void PlotManagerWidget::SetSelectedPlot(PlotDataSet* aPlotDataSetPtr)
{
   //Disable individual series selection and just keep the modifier series selected for now.
   SetModifierSeriesSelected();
}

void PlotManagerWidget::DeselectAllPlots()
{
   mUIPtr->plotWidget->GetPlot()->RemoveAllSeriesSelections();
}

void PlotManagerWidget::SetTitle(QString aTitle)
{
   mUIPtr->plotWidget->GetPlot()->SetTitle(aTitle);
   mTitle = aTitle;
}

void PlotManagerWidget::SetLabelXAxis(QString aLabel)
{
   mUIPtr->plotWidget->GetPlot()->SetLabelXAxis(aLabel);
   mXAxisLabel = aLabel;
}

void PlotManagerWidget::SetLabelYAxis(QString aLabel)
{
   mUIPtr->plotWidget->GetPlot()->SetLabelYAxis(aLabel);
}

void PlotManagerWidget::SetComponentName(QString aName)
{
   mComponentName = aName;
}


void PlotManagerWidget::SetXAxisBounds(double aXMin, double aXMax)
{
   mUIPtr->plotWidget->GetPlot()->SetXAxisBounds(aXMin, aXMax);
}

void PlotManagerWidget::SetYAxisBounds(double aYMin, double aYMax)
{
   mUIPtr->plotWidget->GetPlot()->SetYAxisBounds(aYMin, aYMax);
}

void PlotManagerWidget::HandleAddPointCommand(double aXcoord)
{
   UtQtGL2DPlot* plot = mUIPtr->plotWidget->GetPlot();
   int plotIndex = plot->GetSelectedSeries();

   if (plotIndex == -1 || mModifierPoints.size() == 0)
   {
      //no plot is selected
      return;
   }
   if (plotIndex == mModifierPlotIndex)
   {
      plot->RemoveSeries(plotIndex);
      plot->RemoveSeries(plotIndex + 1);
      std::pair<double, double> prevPoint = *mModifierPoints.begin();
      for (auto iter = mModifierPoints.begin(); iter != mModifierPoints.end(); ++iter)
      {
         std::pair<double, double> curPt = *iter;
         if (curPt.first > aXcoord)
         {
            // Calculate the y position of the point
            // slope = (y2-y1)/(x2-x1)
            double slope = (curPt.second - prevPoint.second) / (curPt.first - prevPoint.first);
            // b = y - (mx)
            double yIntercept = curPt.second - (slope * curPt.first);
            // y = mx+b
            double yCoord = (slope * aXcoord) + yIntercept;
            std::pair<double, double> newPair(aXcoord, yCoord);
            mModifierPoints.insert(iter, newPair);
            break;
         }
         prevPoint = curPt;
      }
      plot->AddPoints(mModifierPoints, mModifierPlotIndex);
      plot->SetSeriesPlotStyle(plot->GetSeriesName(mModifierPlotIndex), UtQtGL2DPlot::POINT);
      plot->SetSeriesWidth(5, mModifierPlotIndex);
      plot->SetSeriesUseSecondaryAxis(true, mModifierPlotIndex);
      plot->AddPoints(mModifierPoints, mModifierPlotIndex + 1);
      plot->SetSeriesWidth(1, mModifierPlotIndex + 1);
      plot->SetSeriesUseSecondaryAxis(true, mModifierPlotIndex + 1);
      plot->SetSeriesSelected(true, plotIndex);
   }
}

void PlotManagerWidget::HandleDataPointSelected(unsigned int aSeriesNum, unsigned int aIndex)
{
   if (ut::cast_to_int(aSeriesNum) == mModifierPlotIndex || ut::cast_to_int(aSeriesNum) == mModifierPlotIndex + 1)
   {
      return;
   }

   mKnotEditorDialog->SetXCoordinate(mPlotDataSetMap.at(aSeriesNum)->GetPoint(aIndex).first);
   mKnotEditorDialog->SetYCoordinate(mPlotDataSetMap.at(aSeriesNum)->GetPoint(aIndex).second);
}

void PlotManagerWidget::HandlePlotManipulation(unsigned int aSeriesNum, unsigned int aIndex)
{
   mPlotManipulationDirty = true;
   UtQtGL2DPlot* plot = mUIPtr->plotWidget->GetPlot();
   std::pair<double, double> temp;

   if (ut::cast_to_int(aSeriesNum) == mModifierPlotIndex)
   {
      SyncModifierLine(aIndex);
      return;
   }
   else if (ut::cast_to_int(aSeriesNum) == mModifierPlotIndex + 1)
   {
      return;  // Do nothing, it's being handled in syncmodifierLine()
   }

   const UtQtGL2DPlot::Series* series = dynamic_cast<const UtQtGL2DPlot::Series*>(plot->FindSeries(aSeriesNum));

   temp.first = series->data.at(aIndex).x;
   temp.second = series->data.at(aIndex).y;

   mPlotDataSetMap.at(aSeriesNum)->SetPoint(temp, aIndex);
   SyncPlot(aSeriesNum + 1);
   SyncModifierPlot(aSeriesNum + 2);

   mKnotEditorDialog->SetXCoordinate(temp.first);
   mKnotEditorDialog->SetYCoordinate(temp.second);
}

void PlotManagerWidget::ShowContextMenu(const QPoint& aPoint)
{
   mContextMenu->clear();


   QAction* addKnotAction = new QAction("Add Modifier Point", mContextMenu);
   connect(addKnotAction, SIGNAL(triggered()), this, SLOT(ShowKnotAdderDialog()));
   mContextMenu->addAction(addKnotAction);

   mContextMenu->addActions(mUIPtr->plotWidget->GetDefaultContextMenuActions());
   mContextMenu->move(mUIPtr->plotWidget->mapToGlobal(aPoint));
   mContextMenu->show();
}

void PlotManagerWidget::ShowEditorDialog()
{
   mKnotEditorDialog->show();
}

void PlotManagerWidget::ShowKnotAdderDialog()
{
   mKnotAdderDialog->show();
}

void PlotManagerWidget::SyncPlot(unsigned int aPlotSeriesNum)
{
   UtQtGL2DPlot* plot = mUIPtr->plotWidget->GetPlot();
   float width = plot->GetSeriesWidth(aPlotSeriesNum);
   QColor color = plot->GetSeriesColor(aPlotSeriesNum);
   plot->RemoveSeries(aPlotSeriesNum);
   plot->AddPoints(mPlotDataSetMap.at(aPlotSeriesNum - 1)->GetPoints(), aPlotSeriesNum);
   plot->SetSeriesWidth(width, aPlotSeriesNum);
   plot->SetSeriesColor(color, aPlotSeriesNum);
   mUIPtr->plotWidget->Update();
}

void PlotManagerWidget::SyncModifierPlot(unsigned int aPlotSeriesNum)
{
   UtQtGL2DPlot* plot = mUIPtr->plotWidget->GetPlot();
   if (plot->GetNumberOfSeries() == 0)
   {
      return;
   }

   const UtQtGL2DPlot::SeriesBase* foundSeries = plot->FindSeries(aPlotSeriesNum);
   if (foundSeries != nullptr && foundSeries->Size() > 0)
   {
      // Check to see if the non modified series is even visible, if it is not, then the modified series should be hidden
      if (!plot->GetSeriesVisible(aPlotSeriesNum - 2))
      {
         plot->RemoveSeries(aPlotSeriesNum);
      }
      else
      {
         float width = plot->GetSeriesWidth(aPlotSeriesNum);
         QColor color(234, 215, 1); // Use a bright yellow color to make the modified lines stand out
         QString previousName = plot->GetSeriesName(aPlotSeriesNum);

         plot->RemoveSeries(aPlotSeriesNum);
         plot->AddPoints(mModifierPlotDataSetMap.at(aPlotSeriesNum - 2)->GetPoints(), aPlotSeriesNum);
         plot->SetSeriesName(previousName, aPlotSeriesNum);
         plot->SetSeriesWidth(width, aPlotSeriesNum);
         plot->SetSeriesColor(color, aPlotSeriesNum);
      }
   }
   else
   {
      plot->RemoveSeries(aPlotSeriesNum);
   }
   mUIPtr->plotWidget->Update();
}

void PlotManagerWidget::SetSeriesColor(unsigned int aSeriesNum, QColor aColor)
{
   UtQtGL2DPlot* plot = mUIPtr->plotWidget->GetPlot();
   plot->SetSeriesColor(aColor, aSeriesNum);
   mUIPtr->plotWidget->Update();
}

void PlotManagerWidget::SetPlotBounds(double aXmin, double aXmax, double aYmin, double aYmax)
{
   UtQtGL2DPlot* plot = mUIPtr->plotWidget->GetPlot();

   plot->SetXAxisBounds(aXmin, aXmax);
   plot->SetYAxisBounds(aYmin, aYmax);
}

void PlotManagerWidget::AddModifierSeries(bool aVisible)
{
   UtQtGL2DPlot* plot = mUIPtr->plotWidget->GetPlot();

   // Return if the plot is uninitialized.
   if(!plot->ContainsData())
   {
      return;
   }

   plot->SetYAxisBounds2(mModifierYAxisMin, mModifierYAxisMax);
   mModifierPoints.clear();
   double minX, minY, maxX, maxY;
   plot->GetDataMinMax(minX, minY, maxX, maxY);
   double yAvg = 1;

   double xStep = (std::abs(minX) + std::abs(maxX)) / 9;
   for (double x = minX; x <= maxX; x += xStep)
   {
      std::pair<double, double> curPt;
      double xVal = x;
      double yVal = yAvg;
      curPt.first = xVal;
      curPt.second = yVal;
      mModifierPoints.emplace_back(curPt);
   }

   mModifierPlotIndex = mNextMapKey;
   plot->AddPoints(mModifierPoints, mModifierPlotIndex);
   plot->SetSeriesName("Offset", mModifierPlotIndex);
   plot->SetSeriesPlotStyle(plot->GetSeriesName(mModifierPlotIndex), UtQtGL2DPlot::POINT);
   plot->SetSeriesWidth(5, mModifierPlotIndex);
   plot->SetSeriesUseSecondaryAxis(true, mModifierPlotIndex);
   plot->AddPoints(mModifierPoints, mModifierPlotIndex + 1);
   plot->SetSeriesName("Multiplier", mModifierPlotIndex + 1);
   plot->SetSeriesWidth(1, mModifierPlotIndex + 1);
   plot->SetSeriesUseSecondaryAxis(true, mModifierPlotIndex + 1);
   plot->SetShowSecondaryAxis(true);
   QColor color(0, 0, 0);
   plot->SetSeriesColor(color, mModifierPlotIndex);
   plot->SetSeriesColor(color, mModifierPlotIndex + 1);
   plot->SetYAxisBounds2(mModifierYAxisMin, mModifierYAxisMax);
   plot->SetTickIncrementY2(0.5);
   mUIPtr->plotWidget->Update();
   mNextMapKey += 2;

   SetModifierSeriesSelected();
   SetModifierLineVisible(aVisible);

}

void PlotManagerWidget::SetModifierSeriesSelected()
{
   UtQtGL2DPlot* plot = mUIPtr->plotWidget->GetPlot();

   //reset the previously selected series...
   if (plot->GetSelectedSeries() != -1)
   {
      plot->SetSeriesWidth(1, plot->GetSelectedSeries() + 1);
   }

   //set the modifier series to be selected
   plot->SetSeriesSelected(true, mModifierPlotIndex);
   plot->SetSeriesWidth(1, mModifierPlotIndex + 1);
}

void PlotManagerWidget::SyncModifierLine(unsigned int aIndex)
{
   UtQtGL2DPlot* plot  = mUIPtr->plotWidget->GetPlot();
   const UtQtGL2DPlot::Series* series = dynamic_cast<const UtQtGL2DPlot::Series*>(plot->FindSeries(mModifierPlotIndex));
   double x, y;
   series->Get(aIndex, x, y);
   plot->ChangePoint(aIndex, x, y, mModifierPlotIndex + 1);
   mModifierPoints.at(aIndex).first = x;
   mModifierPoints.at(aIndex).second = y;
   mUIPtr->plotWidget->Update();

   ApplyModifier();
}

void PlotManagerWidget::ApplyModifier()
{
   if (mCurrentTablePtr == nullptr)
   {
      return;
   }

   auto is3dTablePtr = dynamic_cast<AeroTable3d*>(mCurrentTablePtr);
   if (is3dTablePtr != nullptr)
   {
      int xAxisVariable = 0;  //1 if using primary, 2 if using secondary

      if (QString::fromStdString(is3dTablePtr->GetPrimaryIndependentVarName()) == mXAxisLabel)
      {
         if (mModifierType == cOFFSET)
         {
            is3dTablePtr->SetListBasedPrimaryOffsetModifier(mModifierPoints);
         }
         else
         {
            is3dTablePtr->SetListBasedPrimaryGainModifier(mModifierPoints);
         }
         xAxisVariable = 1;
      }

      else if (QString::fromStdString(is3dTablePtr->GetSecondaryIndependentVarName()) == mXAxisLabel)
      {
         if (mModifierType == cOFFSET)
         {
            is3dTablePtr->SetListBasedSecondaryOffsetModifier(mModifierPoints);
         }
         else
         {
            is3dTablePtr->SetListBasedSecondaryGainModifier(mModifierPoints);
         }
         xAxisVariable = 2;
      }

      is3dTablePtr->UpdateModifiedData();
      UtQtGL2DPlot* plot = mUIPtr->plotWidget->GetPlot();
      for (auto& curPlot : mModifierPlotDataSetMap)
      {
         std::vector<std::pair<double, double>> modifiedData;
         auto foundPlot = mPlotDataSetValueMap.find(curPlot.second.get());
         if (foundPlot != mPlotDataSetValueMap.end())
         {
            std::pair<double, double> keyPair = mPlotDataSetValueMap[curPlot.second.get()];
            if (xAxisVariable == 1)
            {
               is3dTablePtr->GetPrimaryModifiedData(keyPair.first, modifiedData);
            }

            else if (xAxisVariable == 2)
            {
               is3dTablePtr->GetSecondaryModifiedData(keyPair.first, modifiedData);
            }

            if (plot->FindSeries(curPlot.first) == nullptr || !plot->GetSeriesVisible(curPlot.first))
            {
               continue;
            }
            else
            {
               PlotDataSet* curDataSet = curPlot.second.get();
               int curPointIdx = 0;
               for (auto curPair : modifiedData)
               {
                  curDataSet->SetPoint(curPair, curPointIdx);
                  ++curPointIdx;
               }
               SyncModifierPlot(curPlot.first + 2); //The +2 here is for the plot series number, not the point index!
            }
         }
      }
   }
   else //it's not a 3d table, check 4d then 2d...
   {
      auto is4dTablePtr = dynamic_cast<AeroTable4d*>(mCurrentTablePtr);
      if (is4dTablePtr != nullptr)
      {
         int xAxisVariable = 0;  //1 if using primary, 2 if using secondary, 3 if using tertiary

         if (QString::fromStdString(is4dTablePtr->GetPrimaryIndependentVarName()) == mXAxisLabel)
         {
            if (mModifierType == cOFFSET)
            {
               is4dTablePtr->SetListBasedPrimaryOffsetModifier(mModifierPoints);
            }
            else
            {
               is4dTablePtr->SetListBasedPrimaryGainModifier(mModifierPoints);
            }
            xAxisVariable = 1;
         }

         else if (QString::fromStdString(is4dTablePtr->GetSecondaryIndependentVarName()) == mXAxisLabel)
         {
            if (mModifierType == cOFFSET)
            {
               is4dTablePtr->SetListBasedSecondaryOffsetModifier(mModifierPoints);
            }
            else
            {
               is4dTablePtr->SetListBasedSecondaryGainModifier(mModifierPoints);
            }
            xAxisVariable = 2;
         }
         else if (QString::fromStdString(is4dTablePtr->GetTertiaryIndependentVarName()) == mXAxisLabel)
         {
            if (mModifierType == cOFFSET)
            {
               is4dTablePtr->SetListBasedTertiaryOffsetModifier(mModifierPoints);
            }
            else
            {
               is4dTablePtr->SetListBasedTertiaryGainModifier(mModifierPoints);
            }
            xAxisVariable = 3;
         }
         is4dTablePtr->UpdateModifiedData();


         UtQtGL2DPlot* plot = mUIPtr->plotWidget->GetPlot();
         for (auto& curPlot : mModifierPlotDataSetMap)
         {
            std::vector<std::pair<double, double>> modifiedData;
            auto foundPlot = mPlotDataSetValueMap.find(curPlot.second.get());
            if (foundPlot != mPlotDataSetValueMap.end())
            {
               std::pair<double, double> keyPair = mPlotDataSetValueMap[curPlot.second.get()];
               if (xAxisVariable == 1)
               {
                  is4dTablePtr->GetPrimaryModifiedData(keyPair.first, keyPair.second, modifiedData);
               }

               else if (xAxisVariable == 2)
               {
                  is4dTablePtr->GetSecondaryModifiedData(keyPair.first, keyPair.second, modifiedData);
               }
               else if (xAxisVariable == 3)
               {
                  is4dTablePtr->GetTertiaryModifiedData(keyPair.first, keyPair.second, modifiedData);
               }

               if (plot->FindSeries(curPlot.first) == nullptr || !plot->GetSeriesVisible(curPlot.first))
               {
                  continue;
               }
               else
               {
                  PlotDataSet* curDataSet = curPlot.second.get();
                  int curPlotIdx = 0;
                  for (auto curPair : modifiedData)
                  {
                     curDataSet->SetPoint(curPair, curPlotIdx);
                     ++curPlotIdx;
                  }
                  SyncModifierPlot(curPlot.first + 2); //The +2 here is for the plot series number, not the point index!
               }
            }
         }
      }
      else //it's not 3d or 4d table, try 2d
      {
         auto is2dTablePtr = dynamic_cast<AeroTable2d*>(mCurrentTablePtr);
         if (is2dTablePtr != nullptr)
         {

            if (mModifierType == cOFFSET)
            {
               is2dTablePtr->SetListBasedOffsetModifier(mModifierPoints);
            }
            else
            {
               is2dTablePtr->SetListBasedGainModifier(mModifierPoints);
            }

            is2dTablePtr->UpdateModifiedData();
            UtQtGL2DPlot* plot = mUIPtr->plotWidget->GetPlot();
            for (auto& curPlot : mModifierPlotDataSetMap)
            {
               std::vector<std::pair<double, double>> modifiedData;
               auto foundPlot = mPlotDataSetValueMap.find(curPlot.second.get());
               if (foundPlot != mPlotDataSetValueMap.end())
               {
                  is2dTablePtr->GetModifiedData(modifiedData);

                  if (plot->FindSeries(curPlot.first) == nullptr || !plot->GetSeriesVisible(curPlot.first))
                  {
                     continue;
                  }
                  else
                  {
                     PlotDataSet* curDataSet = curPlot.second.get();
                     int curPlotIdx = 0;
                     for (auto curPair : modifiedData)
                     {
                        // See if this point is labeled as unmodifiable and if so, reset it to the base data
                        const auto unmodifedIdx = std::find(mUnmodifiablePointList.begin(), mUnmodifiablePointList.end(), curPlotIdx);
                        if (unmodifedIdx != mUnmodifiablePointList.end())
                        {
                           // The point at this index is labeled as being unmodifiable, set it back to base data
                           std::vector<std::pair<double, double>> baseData;
                           is2dTablePtr->GetBaseData(baseData);
                           curPair = baseData.at(curPlotIdx);
                        }
                        curDataSet->SetPoint(curPair, curPlotIdx);
                        ++curPlotIdx;
                     }
                     SyncModifierPlot(curPlot.first + 2); //The +2 here is for the plot series number, not the point index!
                  }
               }
            }
         }
      }
   }

   //add this modifier to the modification JSON object
   QString componentStr = mComponentName;
   QString tableStr = mTitle;
   QString independendVarStr = mXAxisLabel;
   QJsonArray modifierXValues;
   QJsonArray modifierYValues;
   for (auto curPt : mModifierPoints)
   {
      modifierXValues.push_back(curPt.first);
      modifierYValues.push_back(curPt.second);
   }

   QJsonObject componentObj = mAeroModificationObj[componentStr].toObject();
   QJsonObject tableObj = componentObj[tableStr].toObject();
   QJsonObject modifierObj = tableObj[independendVarStr].toObject();
   QJsonObject arrayObj;
   arrayObj["x_values"] = modifierXValues;
   arrayObj["y_values"] = modifierYValues;
   if (mModifierType == cMULTIPLY)
   {
      modifierObj["gain"] = arrayObj;
   }
   if (mModifierType == cOFFSET)
   {
      modifierObj["offset"] = arrayObj;
   }

   tableObj[independendVarStr] = modifierObj;
   componentObj[tableStr] = tableObj;

   mAeroModificationObj[componentStr] = componentObj;

   UpdateZeroLines();

   emit ModifierChanged();
}

void PlotManagerWidget::SetPlotColors()
{
   //set up the background colors
   UtQtGL2DPlot* plot = mUIPtr->plotWidget->GetPlot();
   QColor BGColor(0, 102, 204);
   plot->SetPlotBackgroundColor(BGColor);
   QColor GridColor(0, 128, 255);
   plot->SetGridColor(GridColor);
   plot->SetGridStipple(UtQtGL2DPlot::NO_STIPPLE);

   //load up the colors for the plots
   mPlotColorMap.emplace_back(QColor(0, 255, 0));
   mPlotColorMap.emplace_back(QColor(251, 153, 12));
   mPlotColorMap.emplace_back(QColor(6, 252, 255));
   mPlotColorMap.emplace_back(QColor(179, 145, 255));
   mPlotColorMap.emplace_back(QColor(255, 0, 0));
   mPlotColorMap.emplace_back(QColor(255, 255, 255));
   mPlotColorMap.emplace_back(QColor(243, 197, 39));
   mPlotColorMap.emplace_back(QColor(255, 0, 255));
   mPlotColorMap.emplace_back(QColor(157, 132, 64));
}

void PlotManagerWidget::SetActiveTable(AeroTableBase* aTable)
{
   mCurrentTablePtr = aTable;
}

void PlotManagerWidget::SetGlobalOffset(double aOffset)
{
   auto is3dTablePtr = dynamic_cast<AeroTable3d*>(mCurrentTablePtr);
   if (is3dTablePtr != nullptr)
   {
      int xAxisVariable = 0;  // 1 if using primary, 2 if using secondary, 3 if using tertiary

      if (QString::fromStdString(is3dTablePtr->GetPrimaryIndependentVarName()) == mXAxisLabel)
      {
         xAxisVariable = 1;
         is3dTablePtr->SetSinglePrimaryOffsetModifier(aOffset);
         is3dTablePtr->UpdateModifiedData();
      }
      else if (QString::fromStdString(is3dTablePtr->GetSecondaryIndependentVarName()) == mXAxisLabel)
      {
         xAxisVariable = 2;
         is3dTablePtr->SetSingleSecondaryOffsetModifier(aOffset);
         is3dTablePtr->UpdateModifiedData();
      }

      for (auto& curPlot : mModifierPlotDataSetMap)
      {
         std::vector<std::pair<double, double>> modifiedData;
         auto foundPlot = mPlotDataSetValueMap.find(curPlot.second.get());
         if (foundPlot != mPlotDataSetValueMap.end())
         {
            std::pair<double, double> keyPair = mPlotDataSetValueMap[curPlot.second.get()];
            if (xAxisVariable == 1)
            {
               is3dTablePtr->GetPrimaryModifiedData(keyPair.first, modifiedData);
               PlotDataSet* curData = curPlot.second.get();
               int curIdx = 0;
               for (auto curPair : modifiedData)
               {
                  curData->SetPoint(curPair, curIdx);
                  ++curIdx;
               }
               SyncModifierPlot(curPlot.first + 2); // The +2 here is for the plot series number, not the point index!
            }
            else if (xAxisVariable == 2)
            {
               is3dTablePtr->GetSecondaryModifiedData(keyPair.first, modifiedData);
               PlotDataSet* curData = curPlot.second.get();
               int curIdx = 0;
               for (auto curPair : modifiedData)
               {
                  curData->SetPoint(curPair, curIdx);
                  ++curIdx;
               }
               SyncModifierPlot(curPlot.first + 2); // The +2 here is for the plot series number, not the plot index!
            }
         }
      }
   }
   else
   {
      auto is4dTablePtr = dynamic_cast<AeroTable4d*>(mCurrentTablePtr);
      if (is4dTablePtr != nullptr)
      {
         int xAxisVariable = 0;  // 1 if using primary, 2 if using secondary, 3 if using tertiary

         if (QString::fromStdString(is4dTablePtr->GetPrimaryIndependentVarName()) == mXAxisLabel)
         {
            xAxisVariable = 1;
            is4dTablePtr->SetSinglePrimaryOffsetModifier(aOffset);
            is4dTablePtr->UpdateModifiedData();
         }
         else if (QString::fromStdString(is4dTablePtr->GetSecondaryIndependentVarName()) == mXAxisLabel)
         {
            xAxisVariable = 2;
            is4dTablePtr->SetSingleSecondaryOffsetModifier(aOffset);
            is4dTablePtr->UpdateModifiedData();
         }
         else if (QString::fromStdString(is4dTablePtr->GetTertiaryIndependentVarName()) == mXAxisLabel)
         {
            is4dTablePtr->SetSingleTertiaryOffsetModifier(aOffset);
            is4dTablePtr->UpdateModifiedData();
            xAxisVariable = 3;
         }

         for (auto& curPlot : mModifierPlotDataSetMap)
         {
            std::vector<std::pair<double, double>> modifiedData;
            auto foundPlot = mPlotDataSetValueMap.find(curPlot.second.get());
            if (foundPlot != mPlotDataSetValueMap.end())
            {
               std::pair<double, double> keyPair = mPlotDataSetValueMap[curPlot.second.get()];
               if (xAxisVariable == 1)
               {
                  is4dTablePtr->GetPrimaryModifiedData(keyPair.first, keyPair.second, modifiedData);
                  PlotDataSet* curData = curPlot.second.get();
                  int curIdx = 0;
                  for (auto curPair : modifiedData)
                  {
                     curData->SetPoint(curPair, curIdx);
                     ++curIdx;
                  }
                  SyncModifierPlot(curPlot.first + 2); // The +2 here is for the plot series number, not the point index!
               }
               else if (xAxisVariable == 2)
               {
                  is4dTablePtr->GetSecondaryModifiedData(keyPair.first, keyPair.second, modifiedData);
                  PlotDataSet* curData = curPlot.second.get();
                  int curIdx = 0;
                  for (auto curPair : modifiedData)
                  {
                     curData->SetPoint(curPair, curIdx);
                     ++curIdx;
                  }
                  SyncModifierPlot(curPlot.first + 2); // The +2 here is for the plot series number, not the point index!
               }
               else if (xAxisVariable == 3)
               {
                  is4dTablePtr->GetTertiaryModifiedData(keyPair.first, keyPair.second, modifiedData);
                  PlotDataSet* curData = curPlot.second.get();
                  int curIdx = 0;
                  for (auto curPair : modifiedData)
                  {
                     curData->SetPoint(curPair, curIdx);
                     ++curIdx;
                  }
                  SyncModifierPlot(curPlot.first + 2); // The +2 here is for the plot series number, not the point index!
               }
            }
         }
      }
      else
      {
         // Check if this is a 2d table
         auto is2dTablePtr = dynamic_cast<AeroTable2d*>(mCurrentTablePtr);
         if (is2dTablePtr != nullptr)
         {
            is2dTablePtr->SetSingleOffsetModifier(aOffset);
            is2dTablePtr->UpdateModifiedData();

            for (auto& curPlot : mModifierPlotDataSetMap)
            {
               std::vector<std::pair<double, double>> modifiedData;
               auto foundPlot = mPlotDataSetValueMap.find(curPlot.second.get());
               if (foundPlot != mPlotDataSetValueMap.end())
               {
                  is2dTablePtr->GetModifiedData(modifiedData);
                  PlotDataSet* curData = curPlot.second.get();
                  int curIdx = 0;
                  for (auto curPair : modifiedData)
                  {
                     curData->SetPoint(curPair, curIdx);
                     ++curIdx;
                  }
                  SyncModifierPlot(curPlot.first + 2); // The +2 here is for the plot series number, not the point index!
               }
            }
         }
      }
   }

   // Add this modifier to the modification JSON object
   QString componentStr = mComponentName;
   QString tableStr = mTitle;
   QString independendVarStr = mXAxisLabel;
   double modifierValue = aOffset;

   QJsonObject componentObj = mAeroModificationObj[componentStr].toObject();
   QJsonObject tableObj = componentObj[tableStr].toObject();
   QJsonObject modifierObj = tableObj[independendVarStr].toObject();

   modifierObj["offset"] = modifierValue;
   tableObj[independendVarStr] = modifierObj;
   componentObj[tableStr] = tableObj;

   mAeroModificationObj[componentStr] = componentObj;

   RefreshPlot();
}

void PlotManagerWidget::SetGlobalMultiplier(double aMultiplier)
{
   auto is3dTablePtr = dynamic_cast<AeroTable3d*>(mCurrentTablePtr);
   if (is3dTablePtr != nullptr)
   {
      int xAxisVariable = 0;  // 1 if using primary, 2 if using secondary, 3 if using tertiary

      if (QString::fromStdString(is3dTablePtr->GetPrimaryIndependentVarName()) == mXAxisLabel)
      {
         xAxisVariable = 1;
         is3dTablePtr->SetSinglePrimaryGainModifier(aMultiplier);
         is3dTablePtr->UpdateModifiedData();
      }
      else if (QString::fromStdString(is3dTablePtr->GetSecondaryIndependentVarName()) == mXAxisLabel)
      {
         xAxisVariable = 2;
         is3dTablePtr->SetSingleSecondaryGainModifier(aMultiplier);
         is3dTablePtr->UpdateModifiedData();
      }

      for (auto& curPlot : mModifierPlotDataSetMap)
      {
         std::vector<std::pair<double, double>> modifiedData;
         auto foundPlot = mPlotDataSetValueMap.find(curPlot.second.get());
         if (foundPlot != mPlotDataSetValueMap.end())
         {
            std::pair<double, double> keyPair = mPlotDataSetValueMap[curPlot.second.get()];
            if (xAxisVariable == 1)
            {
               is3dTablePtr->GetPrimaryModifiedData(keyPair.first, modifiedData);
               PlotDataSet* curData = curPlot.second.get();
               int curIdx = 0;
               for (auto curPair : modifiedData)
               {
                  curData->SetPoint(curPair, curIdx);
                  ++curIdx;
               }
               SyncModifierPlot(curPlot.first + 2); // The +2 here is for the plot series number, not the point index!
            }
            else if (xAxisVariable == 2)
            {
               is3dTablePtr->GetSecondaryModifiedData(keyPair.first, modifiedData);
               PlotDataSet* curData = curPlot.second.get();
               int curIdx = 0;
               for (auto curPair : modifiedData)
               {
                  curData->SetPoint(curPair, curIdx);
                  ++curIdx;
               }
               SyncModifierPlot(curPlot.first + 2); // The +2 here is for the plot series number, not the point index!
            }
         }
      }
   }
   else
   {
      auto is4dTablePtr = dynamic_cast<AeroTable4d*>(mCurrentTablePtr);
      if (is4dTablePtr != nullptr)
      {
         int xAxisVariable = 0;  // 1 if using primary, 2 if using secondary, 3 if using tertiary

         if (QString::fromStdString(is4dTablePtr->GetPrimaryIndependentVarName()) == mXAxisLabel)
         {
            xAxisVariable = 1;
            is4dTablePtr->SetSinglePrimaryGainModifier(aMultiplier);
            is4dTablePtr->UpdateModifiedData();
         }
         else if (QString::fromStdString(is4dTablePtr->GetSecondaryIndependentVarName()) == mXAxisLabel)
         {
            xAxisVariable = 2;
            is4dTablePtr->SetSingleSecondaryGainModifier(aMultiplier);
            is4dTablePtr->UpdateModifiedData();
         }
         else if (QString::fromStdString(is4dTablePtr->GetTertiaryIndependentVarName()) == mXAxisLabel)
         {
            xAxisVariable = 3;
            is4dTablePtr->SetSingleTertiaryGainModifier(aMultiplier);
            is4dTablePtr->UpdateModifiedData();
         }

         for (auto& curPlot : mModifierPlotDataSetMap)
         {
            std::vector<std::pair<double, double>> modifiedData;
            auto foundPlot = mPlotDataSetValueMap.find(curPlot.second.get());
            if (foundPlot != mPlotDataSetValueMap.end())
            {
               std::pair<double, double> keyPair = mPlotDataSetValueMap[curPlot.second.get()];
               if (xAxisVariable == 1)
               {
                  is4dTablePtr->GetPrimaryModifiedData(keyPair.first, keyPair.second, modifiedData);
                  PlotDataSet* curData = curPlot.second.get();
                  int curIdx = 0;
                  for (auto curPair : modifiedData)
                  {
                     curData->SetPoint(curPair, curIdx);
                     ++curIdx;
                  }
                  SyncModifierPlot(curPlot.first + 2); // The +2 here is for the plot series number, not the point index!
               }
               else if (xAxisVariable == 2)
               {
                  is4dTablePtr->GetSecondaryModifiedData(keyPair.first, keyPair.second, modifiedData);
                  PlotDataSet* curData = curPlot.second.get();
                  int curIdx = 0;
                  for (auto curPair : modifiedData)
                  {
                     curData->SetPoint(curPair, curIdx);
                     ++curIdx;
                  }
                  SyncModifierPlot(curPlot.first + 2); // The +2 here is for the plot series number, not the point index!
               }
               else if (xAxisVariable == 3)
               {
                  is4dTablePtr->GetTertiaryModifiedData(keyPair.first, keyPair.second, modifiedData);
                  PlotDataSet* curData = curPlot.second.get();
                  int curIdx = 0;
                  for (auto curPair : modifiedData)
                  {
                     curData->SetPoint(curPair, curIdx);
                     ++curIdx;
                  }
                  SyncModifierPlot(curPlot.first + 2); // The +2 here is for the plot series number, not the point index!
               }
            }
         }
      }
      else
      {
         // Check if this is a 2d table
         auto is2dTablePtr = dynamic_cast<AeroTable2d*>(mCurrentTablePtr);
         if (is2dTablePtr != nullptr)
         {
            is2dTablePtr->SetSingleGainModifier(aMultiplier);
            is2dTablePtr->UpdateModifiedData();

            for (auto& curPlot : mModifierPlotDataSetMap)
            {
               std::vector<std::pair<double, double>> modifiedData;
               auto foundPlot = mPlotDataSetValueMap.find(curPlot.second.get());
               if (foundPlot != mPlotDataSetValueMap.end())
               {
                  is2dTablePtr->GetModifiedData(modifiedData);
                  PlotDataSet* curData = curPlot.second.get();
                  int curIdx = 0;
                  for (auto curPair : modifiedData)
                  {
                     curData->SetPoint(curPair, curIdx);
                     ++curIdx;
                  }
                  SyncModifierPlot(curPlot.first + 2); // The +2 here is for the plot series number, not the point index!
               }
            }
         }
      }
   }

   // Add this modifier to the modification JSON object
   QString componentStr = mComponentName;
   QString tableStr = mTitle;
   QString independendVarStr = mXAxisLabel;
   double modifierValue = aMultiplier;

   QJsonObject componentObj = mAeroModificationObj[componentStr].toObject();
   QJsonObject tableObj = componentObj[tableStr].toObject();
   QJsonObject modifierObj = tableObj[independendVarStr].toObject();

   modifierObj["gain"] = modifierValue;
   tableObj[independendVarStr] = modifierObj;
   componentObj[tableStr] = tableObj;

   mAeroModificationObj[componentStr] = componentObj;

   RefreshPlot();
}

void PlotManagerWidget::SetModifierLineVisible(bool aVisible)
{
   UtQtGL2DPlot* plot = mUIPtr->plotWidget->GetPlot();
   plot->SetSeriesVisible(aVisible, mModifierPlotIndex);
   plot->SetSeriesVisible(aVisible, mModifierPlotIndex + 1);
   if (aVisible)
   {
      SetModifierSeriesSelected();
   }
   plot->SetShowSecondaryAxis(aVisible);
   mUIPtr->plotWidget->Update();
}

void PlotManagerWidget::SetModifierLine(const std::vector<std::pair<double, double>>& aLine)
{
   mModifierPoints = aLine;
   UtQtGL2DPlot* plot = mUIPtr->plotWidget->GetPlot();
   for (unsigned int i = 0; i < mModifierPoints.size(); ++i)
   {
      std::pair<double, double> curPt = mModifierPoints[i];
      plot->ChangePoint(i, curPt.first, curPt.second, mModifierPlotIndex);
      plot->ChangePoint(i, curPt.first, curPt.second, mModifierPlotIndex + 1);
   }
   ApplyModifier();
   RefreshPlot();
}

void PlotManagerWidget::SetModifierLineType(ModifierType aModifierType)
{
   mModifierType = aModifierType;
   double yVal = 0.0; // Reset all of the modifiers to '0' along the y axis if modifier type is OFFSET
   if (mModifierType == cMULTIPLY)
   {
      yVal = 1.0;  // Reset all modifiers to '1' along the y axis if modifier type is MULTIPLY
   }

   // Set the modifier line to the appropriate y-axis
   UtQtGL2DPlot* plot = mUIPtr->plotWidget->GetPlot();
   const UtQtGL2DPlot::Series* series = dynamic_cast<const UtQtGL2DPlot::Series*>(plot->FindSeries(mModifierPlotIndex));
   if (series == nullptr)
   {
      return;
   }

   for (unsigned int i = 0; i < series->Size(); ++i)
   {
      double x, y;
      series->Get(i, x, y);
      plot->ChangePoint(i, x, yVal, mModifierPlotIndex);
      plot->ChangePoint(i, x, yVal, mModifierPlotIndex + 1);
   }

   for (auto& curPt : mModifierPoints)
   {
      curPt.second = yVal;
   }

   mUIPtr->plotWidget->Update();
}

void PlotManagerWidget::RefreshPlot()
{
   UtQtGL2DPlot* plot = mUIPtr->plotWidget->GetPlot();
   plot->MakePlotPretty();
   plot->SetYAxisBounds2(mModifierYAxisMin, mModifierYAxisMax);
   plot->SetTickIncrementY2(0.5);
   UpdateZeroLines();
}

void PlotManagerWidget::RevertPlot()
{
   mCurrentTablePtr->RevertPlot();
   QJsonObject emptyObj;
   mAeroModificationObj = emptyObj;
   ClearPlots();
}

void PlotManagerWidget::UpdateZeroLines()
{
   UtQtGL2DPlot* plot = mUIPtr->plotWidget->GetPlot();

   if (plot->ContainsData())
   {
      plot->RemoveSeries(0);
      plot->RemoveSeries(1);
      double minX, maxX, minY, maxY;
      plot->GetXAxisBounds(minX, maxX);
      plot->GetYAxisBounds(minY, maxY);

      std::vector<std::pair<double, double>> xVec;
      std::vector<std::pair<double, double>> yVec;

      xVec.emplace_back(std::pair<double, double>(minX, 0.0));
      xVec.emplace_back(std::pair<double, double>(maxX, 0.0));

      yVec.emplace_back(std::pair<double, double>(0.0, minY));
      yVec.emplace_back(std::pair<double, double>(0.0, maxY));
      //add bold lines going through 0,0
      QColor GridColor(0, 200, 255);
      plot->AddPoints(xVec, 0);
      plot->SetSeriesName(plot->GetLabelText(plot->LEFT) + " = 0", 0);
      plot->SetSeriesPlotStyle(plot->GetSeriesName(0), UtQtGL2DPlot::LINE);
      plot->SetSeriesWidth(4, 0);
      mUIPtr->plotWidget->GetPlot()->SetSeriesColor(GridColor, 0);

      plot->AddPoints(yVec, 1);
      plot->SetSeriesName(plot->GetLabelText(plot->BOTTOM) + " = 0", 1);
      plot->SetSeriesPlotStyle(plot->GetSeriesName(1), UtQtGL2DPlot::LINE);
      plot->SetSeriesWidth(4, 1);
      mUIPtr->plotWidget->GetPlot()->SetSeriesColor(GridColor, 1);
   }
}

void PlotManagerWidget::SetPointUnmodifiable(unsigned int aIndex)
{
   mUnmodifiablePointList.emplace_back(aIndex);
}

QString PlotManagerWidget::GetDependentVarName()
{
   QString dependentVar;

   auto is3dTablePtr = dynamic_cast<AeroTable3d*>(mCurrentTablePtr);
   if (is3dTablePtr != nullptr)
   {
      dependentVar = QString::fromStdString(is3dTablePtr->GetDependentVarName());
   }
   else
   {
      auto is4dTablePtr = dynamic_cast<AeroTable4d*>(mCurrentTablePtr);
      if (is4dTablePtr != nullptr)
      {
         dependentVar = QString::fromStdString(is4dTablePtr->GetDependentVarName());
      }
      else
      {
         auto is2dTablePtr = dynamic_cast<AeroTable2d*>(mCurrentTablePtr);
         if (is2dTablePtr != nullptr)
         {
            dependentVar = QString::fromStdString(is2dTablePtr->GetDependentVarName());
         }
      }
   }

   return dependentVar;
}

void PlotManagerWidget::SetModifierYAxisBound(double aMin, double aMax)
{
   mModifierYAxisMin = aMin;
   mModifierYAxisMax = aMax;
   UtQtGL2DPlot* plot = mUIPtr->plotWidget->GetPlot();
   plot->SetTickIncrementY2(aMax / 10);
   plot->SetYAxisBounds2(aMin, aMax);
   mUIPtr->plotWidget->Update();
}

void PlotManagerWidget::mouseReleaseEvent(QMouseEvent* event)
{
   if (mPlotManipulationDirty)
   {
      mPlotManipulationDirty = false;
      RefreshPlot();
      SetModifierYAxisBound(mModifierYAxisMin, mModifierYAxisMax);
      emit ModifierChanged();
   }
}

} // namespace Designer
