// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfBM_PlotWidget.hpp"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QInputDialog>
#include <QLineEdit>

#include "UtMemory.hpp"
#include "UtQtMemory.hpp"
#include "WkfEnvironment.hpp"

wkf::bm::PlotWidget::PlotWidget(QString aTitle, QMainWindow* aParent /* = nullptr */) noexcept
   : wkf::DockWidget(aTitle, aParent, Qt::Dialog)
{
   auto* widget = ut::qt::make_qt_ptr<QWidget>();
   setWidget(widget);
   mUi.setupUi(widget);

   mUi.axisBounds->SetXAxisControls();

   mUi.rightPanelLayout->addStretch(1);
   setWindowTitle(aTitle);
   setWindowFlags(Qt::Tool);

   mUi.plot->SetEnableDefaultContextMenu(true);
   mUi.plot->setContextMenuPolicy(Qt::NoContextMenu);
   mUi.plot->GetPlot()->SetTickIncrementY(5);
   mUi.plot->GetPlot()->SetDefaultPlotStyle(UtQtGL2DPlot::PlotStyle::LINE);
   mUi.plot->GetPlot()->SetLabelXAxis("Time (sec)");
   mUi.plot->GetPlot()->SetLabelYAxis("Platform Count");
   mUi.plot->GetPlot()->SetTitle(aTitle);

   mUi.initialCountList->hide();

   connect(mUi.initialTime, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &PlotWidget::InitialTimeChanged);
   connect(mUi.initialCountButton, &QPushButton::toggled, mUi.initialCountList, &QWidget::setVisible);

   connect(mUi.axisBounds, &ut::qt::AxisBoundsWidget::BoundsChanged, this, &PlotWidget::Redraw);
}

void wkf::bm::PlotWidget::SetNameManglingPattern(QString aPattern)
{
   mNameManglingPattern = std::move(aPattern);

   for (unsigned i = 0; i < SeriesCount(); i++)
   {
      if (SeriesExists(i))
      {
         QString mangledName = GetMangledName(i);

         auto& series = mSeries.at(i);
         mUi.plot->GetPlot()->SetSeriesName(mangledName, i);
         series.mShowOverlayCheckBox->setText(mangledName);
      }
   }
}

QString wkf::bm::PlotWidget::GetSeriesName(unsigned aSeriesIndex) const noexcept
{
   if (SeriesExists(aSeriesIndex))
   {
      return mSeries[aSeriesIndex].mName;
   }
   return QString();
}

QString wkf::bm::PlotWidget::GetGroupName(unsigned aSeriesIndex) const noexcept
{
   if (aSeriesIndex < SeriesCount())
   {
      return mGroups[mSeries[aSeriesIndex].mGroupIndex].mName;
   }
   return QString();
}

QString wkf::bm::PlotWidget::GetMangledName(unsigned aSeriesIndex) const
{
   if (aSeriesIndex < SeriesCount())
   {
      const auto& series = mSeries[aSeriesIndex];
      const auto& group  = mGroups[series.mGroupIndex];

      return MangleName(group.mName, series.mName);
   }
   return QString();
}

QString wkf::bm::PlotWidget::MangleName(const QString& aGroup, const QString& aSeries) const
{
   // Ideally we could use QString::arg() here,
   //  but it results in unintuitive behavior if either %1 or %2 is missing.
   QString retval = mNameManglingPattern;
   retval.replace("%1", aGroup).replace("%2", aSeries);
   return retval;
}

unsigned wkf::bm::PlotWidget::FindSeries(const QString& aGroup, const QString& aSeries) const noexcept
{
   auto groupIndex = FindGroup(aGroup);
   if (groupIndex != cNOT_FOUND)
   {
      for (unsigned i = 0; i < SeriesCount(); i++)
      {
         const auto& series = mSeries[i];
         if (series.mGroupIndex == groupIndex && series.mName == aSeries)
         {
            return i;
         }
      }
   }
   return cNOT_FOUND;
}

bool wkf::bm::PlotWidget::GroupExists(const QString& aGroup) const noexcept
{
   return FindGroup(aGroup) != cNOT_FOUND;
}

bool wkf::bm::PlotWidget::SeriesExists(unsigned aSeriesIndex) const noexcept
{
   if (aSeriesIndex < SeriesCount())
   {
      return mSeries[aSeriesIndex].mGroupIndex != cNOT_FOUND;
   }
   return false;
}

bool wkf::bm::PlotWidget::SeriesExists(const QString& aGroup, const QString& aSeries) const noexcept
{
   return FindSeries(aGroup, aSeries) < SeriesCount();
}

void wkf::bm::PlotWidget::AddGroup(QString aName)
{
   if (FindGroup(aName) != cNOT_FOUND)
   {
      throw std::runtime_error(QString("A group with this name already exists: %1").arg(aName).toStdString());
   }

   mGroups.emplace_back();
   auto& group     = mGroups.back();
   group.mName     = std::move(aName);
   group.mGroupBox = ut::qt::make_qt_ptr<QGroupBox>(group.mName);
   group.mGroupBox->setLayout(ut::qt::make_qt_ptr<QVBoxLayout>());

   mUi.rightPanelLayout->insertWidget(mRightPanelItemCount, group.mGroupBox);
   mRightPanelItemCount++;
}

unsigned wkf::bm::PlotWidget::AddSeries(const QString&               aGroup,
                                        QString                      aName,
                                        UtQtGL2DPlot::StipplePattern aStipple /*= none*/)
{
   unsigned groupIndex = FindGroup(aGroup);
   if (groupIndex == cNOT_FOUND)
   {
      throw std::runtime_error(QString("Could not find group: %1").arg(aGroup).toStdString());
   }

   unsigned seriesIndex = FindSeries(aGroup, aName);
   if (seriesIndex < SeriesCount())
   {
      throw std::runtime_error(QString("Series already exists: %1").arg(GetMangledName(seriesIndex)).toStdString());
   }

   seriesIndex = SeriesCount();
   auto& plot  = *mUi.plot->GetPlot();

   mSeries.emplace_back();
   auto& series       = mSeries.back();
   series.mName       = std::move(aName);
   series.mGroupIndex = groupIndex;

   plot.SetSeriesName(GetMangledName(seriesIndex), seriesIndex);
   plot.SetSeriesStipple(aStipple, seriesIndex);

   // Add check boxes
   AddGroupOption(seriesIndex);
   AddOverlayOption(seriesIndex);

   return seriesIndex;
}

unsigned wkf::bm::PlotWidget::AddVisualSeries(QString aName, UtQtGL2DPlot::StipplePattern aStipple /*= none*/)
{
   unsigned seriesIndex = SeriesCount();
   auto&    plot        = *mUi.plot->GetPlot();

   mSeries.emplace_back();
   auto& series = mSeries.back();

   series.mName       = std::move(aName);
   series.mGroupIndex = cVISUAL_SERIES;

   plot.SetSeriesName(series.mName, seriesIndex);
   plot.SetSeriesStipple(aStipple, seriesIndex);

   return seriesIndex;
}

void wkf::bm::PlotWidget::AddPoint(unsigned aSeriesIndex, double aTime, int aCount)
{
   if (SeriesExists(aSeriesIndex))
   {
      mUi.plot->GetPlot()->AddPoint(aTime, aCount, aSeriesIndex);

      if (aTime > GetInitialTime() && mSeries[aSeriesIndex].mOverlayIndex < 0)
      {
         UpdateOverlay(aSeriesIndex);
      }
   }
}

void wkf::bm::PlotWidget::AddPoint(const QString& aGroup, const QString& aSeries, double aTime, int aCount)
{
   auto index = FindSeries(aGroup, aSeries);
   AddPoint(index, aTime, aCount);
}

void wkf::bm::PlotWidget::ClearSeries(unsigned aSeriesIndex)
{
   mUi.plot->GetPlot()->RemoveSeries(aSeriesIndex);
   UpdateOverlay(aSeriesIndex);
}

void wkf::bm::PlotWidget::ClearSeries(const QString& aGroup, const QString& aSeries)
{
   auto index = FindSeries(aGroup, aSeries);
   ClearSeries(index);
}

void wkf::bm::PlotWidget::RemoveSeries(unsigned aSeriesIndex)
{
   ClearSeries(aSeriesIndex);

   if (SeriesExists(aSeriesIndex))
   {
      auto& series       = mSeries[aSeriesIndex];
      series.mGroupIndex = cNOT_FOUND;
      series.mName.clear();
      if (series.mShowOverlayCheckBox)
      {
         delete series.mShowOverlayCheckBox;
      }
      if (series.mShowSeriesCheckBox)
      {
         delete series.mShowSeriesCheckBox;
      }
      UpdateOverlay(aSeriesIndex);
   }
}

void wkf::bm::PlotWidget::RemoveSeries(const QString& aGroup, const QString& aSeries)
{
   auto index = FindSeries(aGroup, aSeries);
   RemoveSeries(index);
}

void wkf::bm::PlotWidget::SetSeriesVisible(unsigned aSeriesIndex, bool aVisible)
{
   if (IsSeriesEnabled(aSeriesIndex))
   {
      auto* checkBox = mSeries[aSeriesIndex].mShowSeriesCheckBox;
      if (checkBox)
      {
         checkBox->setChecked(aVisible);
      }
      mUi.plot->GetPlot()->SetSeriesVisible(aVisible, aSeriesIndex);
   }
}

void wkf::bm::PlotWidget::SetSeriesVisible(const QString& aGroup, const QString& aSeries, bool aVisible)
{
   auto index = FindSeries(aGroup, aSeries);
   SetSeriesVisible(index, aVisible);
}

bool wkf::bm::PlotWidget::IsSeriesVisible(unsigned aSeriesIndex) const noexcept
{
   if (SeriesExists(aSeriesIndex))
   {
      return mUi.plot->GetPlot()->GetSeriesVisible(aSeriesIndex);
   }
   return false;
}

bool wkf::bm::PlotWidget::IsSeriesVisible(const QString& aGroup, const QString& aSeries) const noexcept
{
   auto index = FindSeries(aGroup, aSeries);
   return IsSeriesVisible(index);
}

void wkf::bm::PlotWidget::SetSeriesEnabled(unsigned aSeriesIndex, bool aEnabled)
{
   if (SeriesExists(aSeriesIndex))
   {
      auto& series = mSeries[aSeriesIndex];
      if (series.mGroupIndex != cVISUAL_SERIES)
      {
         if (!aEnabled)
         {
            series.mShowSeriesCheckBox->setChecked(false);
            series.mShowOverlayCheckBox->setChecked(false);
         }

         series.mShowSeriesCheckBox->setEnabled(aEnabled);
         series.mShowOverlayCheckBox->setEnabled(aEnabled);
      }
   }
}

void wkf::bm::PlotWidget::SetSeriesEnabled(const QString& aGroup, const QString& aSeries, bool aEnabled)
{
   auto index = FindSeries(aGroup, aSeries);
   SetSeriesEnabled(index, aEnabled);
}

bool wkf::bm::PlotWidget::IsSeriesEnabled(unsigned aSeriesIndex) const noexcept
{
   if (SeriesExists(aSeriesIndex))
   {
      if (mSeries[aSeriesIndex].mGroupIndex == cVISUAL_SERIES)
      {
         return true;
      }
      return mSeries[aSeriesIndex].mShowSeriesCheckBox->isEnabled();
   }
   return false;
}

bool wkf::bm::PlotWidget::IsSeriesEnabled(const QString& aGroup, const QString& aSeries) const noexcept
{
   auto index = FindSeries(aGroup, aSeries);
   return IsSeriesEnabled(index);
}

void wkf::bm::PlotWidget::SetSeriesColor(unsigned aSeriesIndex, QColor aColor)
{
   if (aSeriesIndex < SeriesCount())
   {
      mUi.plot->GetPlot()->SetSeriesColor(aColor, aSeriesIndex);
      UpdateOverlay(aSeriesIndex);
   }
}

void wkf::bm::PlotWidget::SetSeriesColor(const QString& aGroup, const QString& aSeries, QColor aColor)
{
   auto index = FindSeries(aGroup, aSeries);
   SetSeriesColor(index, aColor);
}

QColor wkf::bm::PlotWidget::GetSeriesColor(unsigned aSeriesIndex, QColor aDefault /*= QColor()*/) const noexcept
{
   if (SeriesExists(aSeriesIndex))
   {
      return mUi.plot->GetPlot()->GetSeriesColor(aSeriesIndex);
   }
   return aDefault;
}

QColor wkf::bm::PlotWidget::GetSeriesColor(const QString& aGroup,
                                           const QString& aSeries,
                                           QColor         aDefault /*= QColor()*/) const noexcept
{
   auto index = FindSeries(aGroup, aSeries);
   return GetSeriesColor(index, aDefault);
}

void wkf::bm::PlotWidget::SetOverlayVisible(unsigned aSeriesIndex, bool aVisible)
{
   if (IsSeriesEnabled(aSeriesIndex))
   {
      auto* checkBox = mSeries[aSeriesIndex].mShowOverlayCheckBox;
      if (checkBox)
      {
         checkBox->setChecked(aVisible);
         UpdateOverlay(aSeriesIndex);
         mUi.plot->Update();
      }
   }
}

void wkf::bm::PlotWidget::SetOverlayVisible(const QString& aGroup, const QString& aSeries, bool aVisible)
{
   auto index = FindSeries(aGroup, aSeries);
   SetOverlayVisible(index, aVisible);
}

bool wkf::bm::PlotWidget::IsOverlayVisible(unsigned aSeriesIndex) const noexcept
{
   if (SeriesExists(aSeriesIndex))
   {
      auto* checkBox = mSeries[aSeriesIndex].mShowOverlayCheckBox;
      if (checkBox)
      {
         return checkBox->isChecked();
      }
   }
   return false;
}

bool wkf::bm::PlotWidget::IsOverlayVisible(const QString& aGroup, const QString& aSeries) const noexcept
{
   auto index = FindSeries(aGroup, aSeries);
   return IsOverlayVisible(index);
}

void wkf::bm::PlotWidget::Redraw()
{
   if (!mUi.plot->IsZoomed())
   {
      MakePlotPretty();
   }
   mUi.plot->Update();
}

void wkf::bm::PlotWidget::MakePlotPretty()
{
   auto&  plot = *mUi.plot->GetPlot();
   double dummy, maxX, maxY, maxY2;
   plot.GetDataMinMax(dummy, dummy, dummy, maxX, maxY, maxY2);

   mUi.axisBounds->SetXAxisBounds(plot);
   plot.MakePlotPrettyY(0, maxY * 1.05, 0, maxY2 * 1.05);
}

unsigned wkf::bm::PlotWidget::SeriesCount() const noexcept
{
   return static_cast<unsigned>(mSeries.size());
}

double wkf::bm::PlotWidget::GetInitialTime() const noexcept
{
   return mUi.initialTime->value();
}

void wkf::bm::PlotWidget::SetInitialTime(double aTime)
{
   mUi.initialTime->setValue(aTime);
   UpdateAllOverlays();
}

void wkf::bm::PlotWidget::SetTimeIndicator(double aTime)
{
   auto& plot = *mUi.plot->GetPlot();
   if (mIndicatorTimeOverlayIndex >= 0)
   {
      plot.RemoveOverlayLine(mIndicatorTimeOverlayIndex);
   }
   mIndicatorTimeOverlayIndex = plot.AddVerticalOverlayLine(aTime, Qt::GlobalColor::lightGray);
}

void wkf::bm::PlotWidget::RemoveTimeIndicator()
{
   if (mIndicatorTimeOverlayIndex >= 0)
   {
      auto& plot = *mUi.plot->GetPlot();
      plot.RemoveOverlayLine(mIndicatorTimeOverlayIndex);
      mIndicatorTimeOverlayIndex = -1;
   }
}

double wkf::bm::PlotWidget::GetMinTime() const noexcept
{
   return mUi.axisBounds->GetMin();
}

double wkf::bm::PlotWidget::GetMaxTime() const noexcept
{
   return mUi.axisBounds->GetMax();
}

void wkf::bm::PlotWidget::Reset()
{
   RemoveTimeIndicator();

   // Allocate new layout for mUi.rightPanel
   QVBoxLayout* newLayout = ut::qt::make_qt_ptr<QVBoxLayout>();

   // Move over initial count controls
   mUi.rightPanelLayout->removeWidget(mUi.initialCountButton);
   newLayout->addWidget(mUi.initialCountButton);

   mUi.rightPanelLayout->removeWidget(mUi.initialCountList);
   newLayout->addWidget(mUi.initialCountList);

   // Move axis bounds
   mUi.rightPanelLayout->removeWidget(mUi.axisBounds);
   newLayout->addWidget(mUi.axisBounds);

   newLayout->addStretch(1);

   //! Remove current initial count checkboxes
   //! Start at 2 to skip label and line edit
   QLayoutItem* item = mUi.initialCountLayout->takeAt(2);
   while (item)
   {
      delete item->widget();
      delete item;

      item = mUi.initialCountLayout->takeAt(2);
   }

   // Delete everything else
   item = mUi.rightPanelLayout->takeAt(0);
   while (item)
   {
      delete item->widget();
      delete item;

      item = mUi.rightPanelLayout->takeAt(0);
   }

   delete mUi.rightPanelLayout;
   mUi.rightPanel->setLayout(newLayout);
   mUi.rightPanelLayout = newLayout;

   // Reset state
   mUi.plot->GetPlot()->ClearData();
   mUi.plot->Update();

   mGroups.clear();
   mSeries.clear();
   mRightPanelItemCount = 0;
}

void wkf::bm::PlotWidget::InsertRightPanelWidget(int aIndex, QWidget* aWidget)
{
   if (aWidget)
   {
      mUi.rightPanelLayout->insertWidget(aIndex, aWidget);
      if (aIndex <= mRightPanelItemCount)
      {
         mRightPanelItemCount++;
      }
   }
}

void wkf::bm::PlotWidget::closeEvent(QCloseEvent* aEvent)
{
   deleteLater();
   wkf::DockWidget::closeEvent(aEvent);
}

wkf::bm::PlotAxisBounds wkf::bm::PlotWidget::ExportBounds() const
{
   PlotAxisBounds retval;

   const auto& ab = *mUi.axisBounds;

   retval.mInitialTime = GetInitialTime();

   if (ab.IsValueMode())
   {
      retval.mTimeMode = PlotAxisBounds::Mode::Value;
   }
   else if (ab.IsRangeMode())
   {
      retval.mTimeMode = PlotAxisBounds::Mode::Range;
   }
   else
   {
      retval.mTimeMode = PlotAxisBounds::Mode::Automatic;
   }

   retval.mTimeMin   = ab.GetMin();
   retval.mTimeMax   = ab.GetMax();
   retval.mTimeRange = ab.GetRange();

   return retval;
}

void wkf::bm::PlotWidget::ImportBounds(const PlotAxisBounds& aBounds)
{
   SetInitialTime(aBounds.mInitialTime);

   auto& ab = *mUi.axisBounds;

   ab.SetMinMax(aBounds.mTimeMin, aBounds.mTimeMax);
   ab.SetRange(aBounds.mTimeRange);

   switch (aBounds.mTimeMode)
   {
   case PlotAxisBounds::Mode::Value:
      ab.SetValueMode();
      break;
   case PlotAxisBounds::Mode::Range:
      ab.SetRangeMode();
      break;
   default:
      ab.SetAutoMode();
   }
}

void wkf::bm::PlotWidget::PrepareForDockStateChange(bool aFloating)
{
   mUi.rightPanel->setVisible(aFloating);
}

bool wkf::bm::PlotWidget::BuildContextMenu(QMenu& aMenu, const QPoint& aPos)
{
   if (mUi.plot->contentsRect().contains(aPos))
   {
      aMenu.addActions(mUi.plot->GetDefaultContextMenuActions());
      aMenu.addSeparator();
   }

   BuildMenu(aMenu);
   return true;
}

unsigned wkf::bm::PlotWidget::FindGroup(const QString& aName) const noexcept
{
   for (unsigned i = 0; i < static_cast<unsigned>(mGroups.size()); i++)
   {
      if (mGroups[i].mName == aName)
      {
         return i;
      }
   }
   return cNOT_FOUND;
}

void wkf::bm::PlotWidget::AddGroupOption(unsigned aSeriesIndex)
{
   auto& series                              = mSeries[aSeriesIndex];
   auto* checkBox                            = ut::qt::make_qt_ptr<QCheckBox>(series.mName);
   mSeries[aSeriesIndex].mShowSeriesCheckBox = checkBox;
   checkBox->setChecked(true);

   connect(checkBox,
           &QCheckBox::toggled,
           this,
           [this, aSeriesIndex](bool aOn)
           {
              SetSeriesVisible(aSeriesIndex, aOn);
              Redraw();
           });

   mGroups[series.mGroupIndex].mGroupBox->layout()->addWidget(checkBox);
}

void wkf::bm::PlotWidget::AddOverlayOption(unsigned aSeriesIndex)
{
   auto* checkBox                             = ut::qt::make_qt_ptr<QCheckBox>(GetMangledName(aSeriesIndex));
   mSeries[aSeriesIndex].mShowOverlayCheckBox = checkBox;

   connect(checkBox,
           &QCheckBox::toggled,
           [this, aSeriesIndex](bool aOn)
           {
              SetOverlayVisible(aSeriesIndex, aOn);
              Redraw();
           });

   mUi.initialCountList->layout()->addWidget(checkBox);
}

void wkf::bm::PlotWidget::InitialTimeChanged(double aTime)
{
   UpdateAllOverlays();
}

void wkf::bm::PlotWidget::UpdateAllOverlays()
{
   for (unsigned i = 0; i < SeriesCount(); i++)
   {
      UpdateOverlay(i);
   }

   mUi.plot->Update();
}

void wkf::bm::PlotWidget::UpdateOverlay(unsigned aSeriesIndex)
{
   int&  overlayIndex = mSeries[aSeriesIndex].mOverlayIndex;
   auto& plot         = *mUi.plot->GetPlot();

   if (overlayIndex >= 0)
   {
      plot.RemoveOverlayLine(overlayIndex);
      overlayIndex = -1;
   }
   if (IsOverlayVisible(aSeriesIndex))
   {
      QList<qreal> values;
      plot.CalculateValueForSeries(aSeriesIndex, GetInitialTime(), values);

      if (values.size() == 1)
      {
         overlayIndex = plot.AddHorizontalOverlayLine(values.first(),
                                                      plot.GetSeriesColor(aSeriesIndex),
                                                      1,
                                                      UtQtGL2DPlot::StipplePattern::DOTTED);
      }
   }
}
