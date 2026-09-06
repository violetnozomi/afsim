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
#include "RvPlottingWidget.hpp"

#include <QDoubleSpinBox>
#include <QLabel>
#include <QList>
#include <QListIterator>
#include <QMenu>
#include <QPushButton>

#include "RvEnvironment.hpp"
#include "RvPlotUpdater.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "UtQt.hpp"
#include "UtQtGL2DPlot.hpp"
#include "UtQtGL2DPlotWidget.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfObserver.hpp"
#include "WkfUnitsObject.hpp"

rv::PlottingWidget::PlottingWidget(QMainWindow* parent /*= nullptr*/, Qt::WindowFlags f)
   : wkf::DockWidget("plotwidget", parent, f)
   , mPlotUpdaterPtr(nullptr)
{
   mUi.setupUi(this);
   mPlotWidgetPtr = mUi.mPlotWidgetPtr;
   mPlotWidgetPtr->setContextMenuPolicy(Qt::NoContextMenu);
   //   mPlotWidgetPtr->SetEnableDefaultContextMenu(true);
   mUi.mPlatformListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection); // Allow for multiple selections
   connect(mUi.mPlatformListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(PlatformSelectionChanged()));

   mPlotWidgetPtr->GetPlot()->SetDefaultPlotStyle(UtQtGL2DPlot::LINE);
   mPlotWidgetPtr->GetPlot()->SetSeriesColor(QColor(255, 0, 0));

   connect(mUi.mXAxisComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(XAxisChanged(int)));
   connect(mUi.mYAxisComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(YAxisChanged(int)));
   connect(mUi.mFilter, &QLineEdit::textChanged, this, &PlottingWidget::ApplyFilter);
   connect(&rvEnv, &Environment::AdvanceTimeRead, this, &PlottingWidget::SetTime);
   connect(wkfEnv.GetPreferenceObject<wkf::UnitsObject>(),
           &wkf::UnitsObject::UnitsChanged,
           this,
           &PlottingWidget::UnitsChangedCB);
   connect(&rvEnv, &Environment::ClearScenario, this, &PlottingWidget::Clear);
   mCallbacks.Add(wkf::Observer::PlatformConfigured.Connect(&PlottingWidget::AddPlatformObject, this));
   PrepareForDockStateChange(isFloating());
}

void rv::PlottingWidget::SetDataFromPlatforms(const int      aCategoryType,
                                              const QString& aCategoryName,
                                              const QString& aXDatumName,
                                              const QString& aYDatumName)
{
   SetXAxisText(aXDatumName);
   SetYAxisText(aYDatumName);
   //   SetSelectedPlatform(nameStr);
}

void rv::PlottingWidget::PlatformSelectionChanged()
{
   emit PlotChanged();
   mPlotWidgetPtr->UpdateDataTable();
}

QStringList rv::PlottingWidget::GetSelectedText() const
{
   auto        items = mUi.mPlatformListWidget->selectedItems();
   QStringList selectedItems;
   for (auto listItem : items)
   {
      QListWidgetItem* curItem = listItem;
      selectedItems.push_back(curItem->text());
   }
   return selectedItems;
}

void rv::PlottingWidget::SetSeriesName(const std::string& aPlatformName, unsigned int aSeriesNum)
{
   mPlotWidgetPtr->GetPlot()->SetSeriesName(aPlatformName.c_str(), aSeriesNum);
}

void rv::PlottingWidget::AddDataPoint(double aXPt, double aYPt, unsigned int aSeriesNum)
{
   mPlotWidgetPtr->GetPlot()->AddPoint(aXPt, aYPt, aSeriesNum);
}

void rv::PlottingWidget::Clear()
{
   mPlotWidgetPtr->GetPlot()->ClearData();
   mUi.mPlatformListWidget->clear();
}

void rv::PlottingWidget::ClearPlot()
{
   mPlotWidgetPtr->GetPlot()->ClearData();
}

void rv::PlottingWidget::FinishPlotting()
{
   mPlotWidgetPtr->GetPlot()->MakePlotPretty();
}

void rv::PlottingWidget::AddPlatformObject(wkf::Platform* aPlatformPtr)
{
   mUi.mPlatformListWidget->addItem(QString(aPlatformPtr->GetName().c_str()));
}

void rv::PlottingWidget::AddPlatform(std::string aPlatformName)
{
   if (!aPlatformName.empty())
   {
      mUi.mPlatformListWidget->addItem(QString(aPlatformName.c_str()));
   }
}

void rv::PlottingWidget::XAxisChanged(int aIndex)
{
   if (mUi.mXAxisComboBox->currentText() == "Time")
   {
      mPlotWidgetPtr->GetPlot()->SetShowCrosshairs(true);
   }
   else
   {
      mPlotWidgetPtr->GetPlot()->SetShowCrosshairs(false);
   }
   emit PlotChanged();
}

void rv::PlottingWidget::YAxisChanged(int aIndex)
{
   emit PlotChanged();
}

QString rv::PlottingWidget::GetXAxisText() const
{
   return mUi.mXAxisComboBox->currentText();
}
QString rv::PlottingWidget::GetYAxisText() const
{
   return mUi.mYAxisComboBox->currentText();
}

void rv::PlottingWidget::SetXAxisText(QString aText)
{
   int foundVar = mUi.mXAxisComboBox->findText(aText);
   if (foundVar != -1)
   {
      mUi.mXAxisComboBox->setCurrentIndex(foundVar);
      if (aText == "Time")
      {
         mPlotWidgetPtr->GetPlot()->SetShowCrosshairs(true);
      }
      else
      {
         mPlotWidgetPtr->GetPlot()->SetShowCrosshairs(false);
      }
   }
}

void rv::PlottingWidget::SetYAxisText(QString aText)
{
   int foundVar = mUi.mYAxisComboBox->findText(aText);
   if (foundVar != -1)
   {
      mUi.mYAxisComboBox->setCurrentIndex(foundVar);
   }
}
void rv::PlottingWidget::SetSelectedPlatform(QString aPlatformName)
{
   QList<QListWidgetItem*> itemList = mUi.mPlatformListWidget->findItems(aPlatformName, Qt::MatchExactly);
   if (!itemList.empty())
   {
      mUi.mPlatformListWidget->setItemSelected(itemList[0], true);
   }
}

void rv::PlottingWidget::PopulateAxisComboBoxesX(QStringList aList)
{
   mUi.mXAxisComboBox->addItems(aList);
}

void rv::PlottingWidget::PopulateAxisComboBoxesY(QStringList aList)
{
   mUi.mYAxisComboBox->addItems(aList);
}

void rv::PlottingWidget::SetTime(const ResultData& aData)
{
   if (GetXAxisText() == "Time")
   {
      if (mPlotWidgetPtr)
      {
         mPlotWidgetPtr->GetPlot()->SetCrosshairsValueX(aData.GetSimTime());
         mPlotWidgetPtr->repaint();
      }
   }
}

void rv::PlottingWidget::SetUpdater(std::unique_ptr<rv::PlotUpdater> aPlotUpdater)
{
   QStringList plats = aPlotUpdater->GetPlatforms();
   QStringList xAxes = aPlotUpdater->GetAxesX();
   QStringList yAxes = aPlotUpdater->GetAxesY();
   for (auto&& it : plats)
   {
      AddPlatform(it.toStdString());
   }
   PopulateAxisComboBoxesX(xAxes);
   PopulateAxisComboBoxesY(yAxes);
   QStringList sel = aPlotUpdater->GetInitialSelection();
   for (auto&& it : sel)
   {
      SetSelectedPlatform(it);
   }
   QString initx = aPlotUpdater->GetInitialXAxis();
   QString inity = aPlotUpdater->GetInitialYAxis();
   SetXAxisText(initx);
   SetYAxisText(inity);
   // build up the selection from the updater

   if (aPlotUpdater->SupportsMultiplatform())
   {
      mUi.mPlatformListWidget->show();
      mUi.mFilter->show();
   }
   else
   {
      mUi.mPlatformListWidget->hide();
      mUi.mFilter->hide();
   }

   connect(this, &PlottingWidget::PlotChanged, this, &PlottingWidget::BuildPlotsFromUpdater);

   mPlotUpdaterPtr = std::move(aPlotUpdater);
   BuildPlotsFromUpdater();
}

bool rv::PlottingWidget::BuildContextMenu(QMenu& aMenu, const QPoint& pos)
{
   if (mPlotWidgetPtr->geometry().contains(pos))
   {
      aMenu.addActions(mPlotWidgetPtr->GetDefaultContextMenuActions());

      QAction* setSampleRateAction = new QAction(&aMenu);
      setSampleRateAction->setText("Set Sample Rate");
      connect(setSampleRateAction, &QAction::triggered, this, &rv::PlottingWidget::SetSampleRateAction);

      aMenu.addAction(setSampleRateAction);
   }
   return true;
}

void rv::PlottingWidget::BuildPlotsFromUpdater()
{
   ClearPlot();
   if (mPlotUpdaterPtr)
   {
      QStringList sel = GetSelectedText();

      int seriesId = 0;
      for (auto&& it : sel)
      {
         mPlotUpdaterPtr->SetLastUpdate(0.0);
         std::vector<std::pair<double, double>> series = mPlotUpdaterPtr->GetSeries(it, GetXAxisText(), GetYAxisText());
         for (auto&& jt : series)
         {
            AddDataPoint(jt.first, jt.second, seriesId);
         }
         SetSeriesName(it.toStdString(), seriesId);
         mPlotWidgetPtr->GetPlot()->SetLabelXAxis(GetXAxisLabel());
         mPlotWidgetPtr->GetPlot()->SetLabelYAxis(GetYAxisLabel());
         ++seriesId;
      }
   }
   FinishPlotting();
}

void rv::PlottingWidget::UnitsChangedCB()
{
   if (mPlotUpdaterPtr)
   {
      BuildPlotsFromUpdater();
   }
}

void rv::PlottingWidget::ApplyFilter(const QString& aFilter)
{
   for (int i = 0; i < mUi.mPlatformListWidget->count(); ++i)
   {
      QListWidgetItem* item     = mUi.mPlatformListWidget->item(i);
      QString          itemText = item->text();
      bool             hidden   = true;
      if (item->text().contains(aFilter))
      {
         hidden = false;
      }
      item->setHidden(hidden);
   }
}

void rv::PlottingWidget::PrepareForDockStateChange(bool aFloating)
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

void rv::PlottingWidget::SetSampleRateAction()
{
   // build dialog
   QDialog* sampleDialog = new QDialog(this);

   QLabel* sampleRateLabel = new QLabel(sampleDialog);
   sampleRateLabel->setText("Sample Rate(sec): ");
   QDoubleSpinBox* sampleRateSpinBox = new QDoubleSpinBox(sampleDialog);
   sampleRateSpinBox->setValue(mPlotUpdaterPtr->GetSampleRate());
   QPushButton* sampleRateOKButton = new QPushButton("Ok", sampleDialog);

   QGridLayout* gridLayout = new QGridLayout;
   gridLayout->addWidget(sampleRateLabel, 0, 0);
   gridLayout->addWidget(sampleRateSpinBox, 0, 1);
   gridLayout->addWidget(sampleRateOKButton, 1, 1);

   sampleDialog->setLayout(gridLayout);
   sampleDialog->show();

   connect(sampleRateOKButton,
           &QPushButton::clicked,
           this,
           [=]()
           {
              mPlotUpdaterPtr->SetSampleRate(sampleRateSpinBox->value());

              emit PlotChanged();
              mPlotWidgetPtr->UpdateDataTable();

              sampleDialog->close();
           });
}

QString rv::PlottingWidget::GetXAxisLabel() const
{
   QString desc = GetXAxisText();
   QString unit = mPlotUpdaterPtr->GetUnitsX(desc);
   if (unit.isEmpty())
   {
      return desc;
   }
   return desc + " [" + unit + "]";
}

QString rv::PlottingWidget::GetYAxisLabel() const
{
   QString desc = GetYAxisText();
   QString unit = mPlotUpdaterPtr->GetUnitsY(desc);
   if (unit.isEmpty())
   {
      return desc;
   }
   return desc + " [" + unit + "]";
}
