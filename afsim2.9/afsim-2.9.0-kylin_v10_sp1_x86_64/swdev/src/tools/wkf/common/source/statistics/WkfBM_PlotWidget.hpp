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

#ifndef WKF_BM_PLOT_WIDGET_HPP
#define WKF_BM_PLOT_WIDGET_HPP

#include "wkf_common_export.h"

#include <memory>
#include <vector>

#include <QCheckBox>
#include <QDialog>

#include "UtOptional.hpp"
#include "WkfBM_PlotPrefObjects.hpp"
#include "WkfDockWidget.hpp"
#include "misc/WkfExportHelper.hpp"
#include "ui_WkfBM_PlotWidget.h"

class QCheckBox;

namespace wkf
{
namespace bm
{
//! PlotWidget is designed to work with positive times and values.
class WKF_COMMON_EXPORT PlotWidget : public wkf::DockWidget
{
   Q_OBJECT
public:
   explicit PlotWidget(QString aTitle, QMainWindow* aParent = nullptr) noexcept;

   PlotWidget(const PlotWidget&) = delete;
   PlotWidget(PlotWidget&&)      = delete;

   //! The name mangling pattern determines how the name of the series and group should be combined.
   //! By default, this value is "%1.%2", which equates to "Group.Series".
   //! The mangled name is what is used for the series name in the plot legend, and in the overlay list.
   void SetNameManglingPattern(QString aPattern);

   //! Returns empty string if the series does not exist.
   //! @{
   QString GetSeriesName(unsigned aSeriesIndex) const noexcept;
   QString GetGroupName(unsigned aSeriesIndex) const noexcept;
   QString GetMangledName(unsigned aSeriesIndex) const;
   //! @}

   QString MangleName(const QString& aGroup, const QString& aSeries) const;

   //! Returned by FindSeries when the desired series or group could not be found.
   constexpr static unsigned cNOT_FOUND = static_cast<unsigned>(-1);

   //! Returns the series index based on the group and series names.
   //! Returns cNOT_FOUND if the series does not exist.
   unsigned FindSeries(const QString& aGroup, const QString& aSeries) const noexcept;

   //! Returns true if the selected group or series exists.
   //! @{
   bool GroupExists(const QString& aGroup) const noexcept;

   bool SeriesExists(unsigned aSeriesIndex) const noexcept;
   bool SeriesExists(const QString& aGroup, const QString& aSeries) const noexcept;
   //! @}

   //! Adds a group box to put series controls into.
   //! Groups must be added BEFORE the series that go in them.
   //! If the group already exists, throws std::runtime_error.
   void AddGroup(QString aName);

   //! Adds a series to the plot and creates check boxes to control visibility.
   //! Returns the series index.
   //! If the group can not be found, throws std::runtime_error.
   unsigned AddSeries(const QString&               aGroup,
                      QString                      aSeries,
                      UtQtGL2DPlot::StipplePattern aStipple = UtQtGL2DPlot::StipplePattern::NO_STIPPLE);

   //! Adds a series to the plot, but does not add check boxes for this series.
   //! Visual series must be modified using the index; name lookup will not work.
   //! Visual series cannot have overlays or be disabled. They can be made not visible.
   //! Returns the series index.
   unsigned AddVisualSeries(QString                      aName,
                            UtQtGL2DPlot::StipplePattern aStipple = UtQtGL2DPlot::StipplePattern::NO_STIPPLE);

   //! Adds a point to a series.
   //! Does nothing if the series does not exist.
   //! @{
   void AddPoint(unsigned aSeriesIndex, double aTime, int aCount);
   void AddPoint(const QString& aSeries, const QString& aGroup, double aTime, int aCount);
   //! @}

   //! Clears the data for a series. The series still exists, but has no data in the plot.
   //! Users must call Redraw() to see changes.
   //! @{
   void ClearSeries(unsigned aSeriesIndex);
   void ClearSeries(const QString& aGroup, const QString& aSeries);
   //! @}

   //! Removes a series from the plot.
   //! Any check boxes or overlay lines associated with it are also removed.
   //! @{
   void RemoveSeries(unsigned aSeriesIndex);
   void RemoveSeries(const QString& aGroup, const QString& aSereis);
   //! @}

   //! Sets the visibility of a series.
   //! Series cannot be set visible if they are not enabled.
   //! Does nothing if the series does not exist.
   //! @{
   void SetSeriesVisible(unsigned aSeriesIndex, bool aVisible);
   void SetSeriesVisible(const QString& aGroup, const QString& aSeries, bool aVisible);
   //! @}

   //! Returns whether a series is visible or not.
   //! Returns false if the series does not exist.
   //! @{
   bool IsSeriesVisible(unsigned aSeriesIndex) const noexcept;
   bool IsSeriesVisible(const QString& aGroup, const QString& aSeries) const noexcept;
   //! @}

   //! When a series is disabled, its checkboxes are unclicked and disabled.
   //! Intended use case: team visibility preferences.
   //! Does nothing if the series does not exist.
   //! @{
   void SetSeriesEnabled(unsigned aSeriesIndex, bool aEnabled);
   void SetSeriesEnabled(const QString& aGroup, const QString& aSeries, bool aEnabled);
   //! @}

   //! Returns whether a series is visible or not.
   //! Returns false if the series does not exist.
   //! @{
   bool IsSeriesEnabled(unsigned aSeriesIndex) const noexcept;
   bool IsSeriesEnabled(const QString& aGroup, const QString& aSeries) const noexcept;
   //! @}

   //! Sets the display color a series and its overlay line.
   //! Does nothing if the series does not exist.
   //! @{
   void SetSeriesColor(unsigned aSeriesIndex, QColor aColor);
   void SetSeriesColor(const QString& aGroup, const QString& aSeries, QColor aColor);
   //! @}

   //! Returns the color of a series.
   //! Returns aDefault if the series does not exist.
   //! @{
   QColor GetSeriesColor(unsigned aSeriesIndex, QColor aDefault = QColor()) const noexcept;
   QColor GetSeriesColor(const QString& aGroup, const QString& aSeries, QColor aDefault = QColor()) const noexcept;
   //! @}

   //! Sets the visibility of the overlay for a series.
   //! Overlays cannot be set visible if they are not enabled.
   //! Does nothing if the series does not exist.
   //! @{
   void SetOverlayVisible(unsigned aSeriesIndex, bool aVisible);
   void SetOverlayVisible(const QString& aGroup, const QString& aSeries, bool aVisible);
   //! @}

   //! Returns whether the overlay for a series is visible or not.
   //! Returns false if the series does not exist or is a visual series.
   //! @{
   bool IsOverlayVisible(unsigned aSeriesIndex) const noexcept;
   bool IsOverlayVisible(const QString& aGroup, const QString& aSeries) const noexcept;
   //! @}

   //! Redraws the widget.
   void Redraw();
   void MakePlotPretty();

   //! Returns the number of series.
   unsigned SeriesCount() const noexcept;

   //! Returns the initial time that overlay lines are based on.
   double GetInitialTime() const noexcept;
   //! Sets the initial time that overlay lines are based on.
   void SetInitialTime(double aTime);

   //! Adds or removes a time indicator vertical line.
   //! This is independent from the initial time.
   //! Only the last set indicator time is used.
   //! @{
   void SetTimeIndicator(double aTime);
   void RemoveTimeIndicator();
   //! @}

   //! Sets the minimum and maximum times.
   //! Enables the group box if it isn't already.
   //! If aMaxTime < aMinTime, sets maxTime = minTime.
   //! If either value is less than 0, sets them to 0.
   void SetMinMaxTime(double aMinTime, double aMaxTime);

   //! Returns the set min/max time.
   //! Does not take into account if the time selection is enabled.
   //! @{
   double GetMinTime() const noexcept;
   double GetMaxTime() const noexcept;
   //! @}

   //! Clears all data from the PlotWidget.
   //! Removes all groups and series.
   //! ALL OVERLOADS NEED TO CALL THIS.
   virtual void Reset();

   //! Tells the plot to update the display to reflect the current time.
   virtual void Update(double aTime) {}

   //! Inserts a widget into the right panel.
   void InsertRightPanelWidget(int aIndex, QWidget* aWidget);

protected:
   void closeEvent(QCloseEvent* aEvent) override;

   virtual void BuildMenu(QMenu& aMenu) = 0;

   PlotAxisBounds ExportBounds() const;
   void           ImportBounds(const PlotAxisBounds& aBounds);

private:
   void PrepareForDockStateChange(bool aFloating) override;
   bool BuildContextMenu(QMenu& aMenu, const QPoint& aPos) override;

   //! The group index for visual series.
   static constexpr unsigned cVISUAL_SERIES = cNOT_FOUND - 1;

   //! Returns cNOT_FOUND if the group could not be found.
   unsigned FindGroup(const QString& aName) const noexcept;

   void AddGroupOption(unsigned aSeriesIndex);
   void AddOverlayOption(unsigned aSeriesIndex);

   void InitialTimeChanged(double aTime);
   void UpdateAllOverlays();
   void UpdateOverlay(unsigned aSeriesIndex);

   struct Group
   {
      QString    mName;
      QGroupBox* mGroupBox = nullptr;
   };

   struct Series
   {
      QString mName;
      //! For visual series, mGroupIndex = cNOT_FOUND - 1.
      //! For series that have been removed, mGroupIndex = cNOT_FOUND
      unsigned mGroupIndex;

      // Check boxes will be cleaned up by Qt's parenting system.
      QCheckBox* mShowSeriesCheckBox  = nullptr;
      QCheckBox* mShowOverlayCheckBox = nullptr;
      int        mOverlayIndex        = -1;
   };

   Ui::WkfBMPlotWidget mUi;
   QString             mNameManglingPattern = "%1.%2";
   int                 mRightPanelItemCount = 0;
   std::vector<Group>  mGroups;
   std::vector<Series> mSeries;
   int                 mIndicatorTimeOverlayIndex = -1;
};
} // namespace bm
} // namespace wkf

#endif
