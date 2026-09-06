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

#include "PlottingWidget.hpp"

#include "RvEnvironment.hpp"
#include "RvPlotUpdater.hpp"
#include "RvResultData.hpp"
#include "WkfObserver.hpp"
#include "WkfUnitsObject.hpp"

RvTracks::PlottingWidget::PlottingWidget(QMainWindow* aParent /*= nullptr*/, Qt::WindowFlags f /*= Qt::WindowFlags()*/)
   : wkf::DockWidget("trackPlot", aParent, f)
   , mPlotUpdaterPtr(nullptr)
{
   mUi.setupUi(this);
   mUi.mPlotWidgetPtr->setContextMenuPolicy(Qt::NoContextMenu);
   //   mUi.mPlotWidgetPtr->SetEnableDefaultContextMenu(true);

   mUi.mPlotWidgetPtr->GetPlot()->SetDefaultPlotStyle(UtQtGL2DPlot::LINE);
   mUi.mPlotWidgetPtr->GetPlot()->SetSeriesColor(QColor(255, 0, 0));

   connect(mUi.mXAxisComboBox,
           QOverload<int>::of(&QComboBox::currentIndexChanged),
           [this](int aIdx) { emit PlotChanged(); });
   connect(mUi.mYAxisComboBox,
           QOverload<int>::of(&QComboBox::currentIndexChanged),
           [this](int aIdx) { emit PlotChanged(); });
   connect(&rvEnv, &rv::Environment::AdvanceTimeRead, this, &PlottingWidget::SetTime);
   connect(wkfEnv.GetPreferenceObject<wkf::UnitsObject>(),
           &wkf::UnitsObject::UnitsChanged,
           this,
           &PlottingWidget::UnitsChangedCB);
   connect(&rvEnv, &rv::Environment::ClearScenario, this, &PlottingWidget::Clear);
   connect(mUi.mSeriesListWidget, &QListWidget::itemChanged, [this](QListWidgetItem* aItem) { emit PlotChanged(); });
   PrepareForDockStateChange(isFloating());
}

void RvTracks::PlottingWidget::SetTime(const rv::ResultData& aData)
{
   if (GetXAxisText() == "Time")
   {
      mUi.mPlotWidgetPtr->GetPlot()->SetCrosshairsValueX(aData.GetSimTime());
      mUi.mPlotWidgetPtr->repaint();
   }
}

void RvTracks::PlottingWidget::UnitsChangedCB()
{
   if (mPlotUpdaterPtr)
   {
      BuildPlotsFromUpdater();
   }
}

void RvTracks::PlottingWidget::Clear()
{
   mUi.mPlotWidgetPtr->GetPlot()->ClearData();
   mUi.mSeriesListWidget->clear();
}

void RvTracks::PlottingWidget::PrepareForDockStateChange(bool aFloating)
{
   if (aFloating)
   {
      mUi.mScrollArea->show();
   }
   else
   {
      mUi.mScrollArea->hide();
   }
}

QString RvTracks::PlottingWidget::GetXAxisText() const
{
   return mUi.mXAxisComboBox->currentText();
}

QString RvTracks::PlottingWidget::GetYAxisText() const
{
   return mUi.mYAxisComboBox->currentText();
}

void RvTracks::PlottingWidget::SetXAxisText(const QString& aText)
{
   int foundVar = mUi.mXAxisComboBox->findText(aText);
   if (foundVar != -1)
   {
      mUi.mXAxisComboBox->setCurrentIndex(foundVar);
      if (aText == "Time")
      {
         mUi.mPlotWidgetPtr->GetPlot()->SetShowCrosshairs(true);
      }
      else
      {
         mUi.mPlotWidgetPtr->GetPlot()->SetShowCrosshairs(false);
      }
   }
}

void RvTracks::PlottingWidget::SetYAxisText(const QString& aText)
{
   int foundVar = mUi.mYAxisComboBox->findText(aText);
   if (foundVar != -1)
   {
      mUi.mYAxisComboBox->setCurrentIndex(foundVar);
   }
}

void RvTracks::PlottingWidget::BuildPlotsFromUpdater()
{
   mUi.mPlotWidgetPtr->GetPlot()->ClearData();
   if (mPlotUpdaterPtr)
   {
      std::vector<std::pair<double, double>> series =
         mPlotUpdaterPtr->GetSeries(mPlatformName, GetXAxisText(), GetYAxisText());
      int seriesId = 0;
      for (auto&& jt : series)
      {
         mUi.mPlotWidgetPtr->GetPlot()->AddPoint(jt.first, jt.second, seriesId);
      }
      mUi.mPlotWidgetPtr->GetPlot()->SetSeriesName(mPlotUpdaterPtr->GetTrackId().toString(), seriesId);
      mUi.mPlotWidgetPtr->GetPlot()->SetSeriesWidth(2, seriesId);
      mUi.mPlotWidgetPtr->GetPlot()->SetLabelXAxis(GetXAxisText());
      mUi.mPlotWidgetPtr->GetPlot()->SetLabelYAxis(GetYAxisText());

      ++seriesId;

      for (int i = 0; i < mUi.mSeriesListWidget->count(); ++i)
      {
         QListWidgetItem& item = *mUi.mSeriesListWidget->item(i);
         if (item.checkState() == Qt::Checked)
         {
            if (item.type() == 0)
            {
               mUi.mPlotWidgetPtr->GetPlot()->SetSeriesName("Truth", seriesId);
               std::vector<std::pair<double, double>> truthSeries =
                  mPlotUpdaterPtr->GetTruthSeries(GetXAxisText(), GetYAxisText());
               for (auto&& jt : truthSeries)
               {
                  mUi.mPlotWidgetPtr->GetPlot()->AddPoint(jt.first, jt.second, seriesId);
               }
               seriesId++;
               // this is a truth series
            }
            else
            {
               QString ownerName = item.data(Qt::UserRole).toString();
               int     index     = item.data(Qt::UserRole + 1).toInt();
               mUi.mPlotWidgetPtr->GetPlot()->SetSeriesName(item.text(), seriesId);
               std::vector<std::pair<double, double>> contribSeries =
                  mPlotUpdaterPtr->GetSensorTrackSeries(ownerName, index, GetXAxisText(), GetYAxisText());
               for (auto&& jt : contribSeries)
               {
                  mUi.mPlotWidgetPtr->GetPlot()->AddPoint(jt.first, jt.second, seriesId);
               }
               seriesId++;
               // this is a contributor series
            }
         }
      }
      // for everything checked in our contributor list, add another series
   }
   mUi.mPlotWidgetPtr->GetPlot()->MakePlotPretty();
}

void RvTracks::PlottingWidget::PopulateAxisComboBoxesX(QStringList aList)
{
   mUi.mXAxisComboBox->addItems(aList);
}

void RvTracks::PlottingWidget::PopulateAxisComboBoxesY(QStringList aList)
{
   mUi.mYAxisComboBox->addItems(aList);
}

void RvTracks::PlottingWidget::SetUpdater(std::unique_ptr<RvTracks::PlotUpdater> aPlotUpdater)
{
   mPlotUpdaterPtr   = std::move(aPlotUpdater);
   QStringList xAxes = mPlotUpdaterPtr->GetAxesX();
   QStringList yAxes = mPlotUpdaterPtr->GetAxesY();
   PopulateAxisComboBoxesX(xAxes);
   PopulateAxisComboBoxesY(yAxes);
   mPlatformName = *mPlotUpdaterPtr->GetInitialSelection().begin();

   // clear and populate contributors' series
   // populate truth series
   if (!mPlotUpdaterPtr->GetTargetName().isEmpty())
   {
      QListWidgetItem* truthItem =
         new QListWidgetItem(QString("Truth (") + mPlotUpdaterPtr->GetTargetName() + ")", mUi.mSeriesListWidget, 0);
      truthItem->setFlags(truthItem->flags() | Qt::ItemIsUserCheckable);
      truthItem->setCheckState(Qt::Unchecked);
      truthItem->setData(Qt::UserRole, mPlotUpdaterPtr->GetTargetIndex());
   }

   for (auto&& it : mPlotUpdaterPtr->GetContributors())
   {
      QListWidgetItem* contribItem = new QListWidgetItem(it.toString(), mUi.mSeriesListWidget, 1);
      contribItem->setFlags(contribItem->flags() | Qt::ItemIsUserCheckable);
      contribItem->setCheckState(Qt::Unchecked);
      contribItem->setData(Qt::UserRole, QString::fromStdString(it.GetPlatformName()));
      contribItem->setData(Qt::UserRole + 1, it.GetLocalTrackId());
   }

   QString initx = mPlotUpdaterPtr->GetInitialXAxis();
   QString inity = mPlotUpdaterPtr->GetInitialYAxis();
   SetXAxisText(initx);
   SetYAxisText(inity);
   // build up the selection from the updater

   connect(this, &PlottingWidget::PlotChanged, this, &PlottingWidget::BuildPlotsFromUpdater);
   BuildPlotsFromUpdater();
}

bool RvTracks::PlottingWidget::BuildContextMenu(QMenu& aMenu, const QPoint& pos)
{
   if (mUi.mPlotWidgetPtr->geometry().contains(pos))
   {
      aMenu.addActions(mUi.mPlotWidgetPtr->GetDefaultContextMenuActions());
      //      mPlotWidgetPtr->AddToContextMenu(aMenu);
      // add to aMenu from the plot
   }
   return true;
}
