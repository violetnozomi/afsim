// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKF_FILTER_PLOT_HPP
#define WKF_FILTER_PLOT_HPP

#include "wkf_common_export.h"

#include <map>
#include <set>

#include <QDialog>

#include "ui_WkfFilterPlot.h"

class QAbstractItemModel;

namespace WkfFilter
{
//! A dialog designed to display a plot with data from a potentially-filtered event list.
class WKF_COMMON_EXPORT FilterPlot : public QDialog
{
   Q_OBJECT
public:
   explicit FilterPlot(QWidget* aParent = nullptr);

   void SetSourceModel(QAbstractItemModel* aModel);

   void ReloadModel();

   //! Marks a column as a valid series. Only columns marked as such can be selected from the GUI or set with
   //! SetSeriesColumn. Empty column names can never be valid series.
   void MarkColumnAsValidSeries(const QString& aColumnName, bool aValid = true);

   //! Sets the default columns used when loading setting a source model.
   //! @{
   void SetDefaultXAxisColumn(const QString& aColumnName);
   void SetDefaultYAxisColumn(const QString& aColumnName);
   void SetDefaultSeriesColumn(const QString& aColumnName, bool aEnableByDefault);
   //! @}

   //! Sets the X-axis column and invalidates the plot.
   //! @{
   void SetXAxisColumn(int aColumnIndex);
   void SetXAxisColumn(const QString& aColumnName);
   //! @}

   //! Sets the Y-axis column and invalidates the plot.
   //! @{
   void SetYAxisColumn(int aColumnIndex);
   void SetYAxisColumn(const QString& aColumnName);
   //! @}

   //! Sets the series column and invalidates the plot.
   //! A series may only be selected if it has been set selectable.
   //! Returns true if the chosen column is selectable.
   //! @{
   bool SetSeriesColumn(int aColumnIndex);
   bool SetSeriesColumn(const QString& aColumnName);
   //! @}

   //! Sets whether or not the series column is enabled.
   void SetSeriesColumnEnabled(bool aEnable);

   //! Accessors
   //! @{
   int     GetXAxisColumnIndex() const;
   QString GetXAxisColumnName() const;

   int     GetYAxisColumnIndex() const;
   QString GetYAxisColumnName() const;

   int     GetSeriesColumnIndex() const;
   QString GetSeriesColumnName() const;

   bool IsSeriesColumnEnabled() const;
   //! @}

   //! Sets a series to be shown or hidden.
   //! If there is no series with the given name, does nothing and returns false.
   void SetSeriesVisible(const QString& aSeriesName, bool aVisible);

   //! Returns whether a series is visible.
   //! If there is no series with the given name, returns false.
   bool IsSeriesVisible(const QString& aSeriesName) const;

   //! Causes the plot to re-read data from the source model.
   void Invalidate();

protected:
   void showEvent(QShowEvent* aEvent) override
   {
      QDialog::showEvent(aEvent);
      ReloadModel();
   }

private:
   Ui::WkfFilterPlot   mUi;
   QAbstractItemModel* mSourceModel;

   std::set<QString>                                         mValidSeriesColumns;
   std::map<QString, bool>                                   mSeriesVisible;
   std::map<QString, std::vector<std::pair<double, double>>> mSeriesData;

   QString mDefaultXColumn;
   QString mDefaultYColumn;
   QString mDefaultSeriesColumn;
   bool    mDefaultSeriesEnabled;

   int  mXColumn;
   int  mYColumn;
   int  mSeriesColumn;
   bool mSeriesEnabled;

   void DrawPlot();

   // Returns empty string on failure.
   QString ColumnNameFromIndex(int aIndex) const;
   // Returns -1 on failure.
   int ColumnIndexFromName(const QString& aName) const;

   //! If series column is enabled, returns the data in the series column and given row.
   //! Otherwise, returns an empty string.
   QString ControlledSeriesData(int aRow) const;
   double  DoubleData(int aRow, int aColumn) const;

   void ReloadModelP();

   void SetComboBoxOptions();
   void SetComboBoxStatus();
   void SetCheckBoxList();

   //! Returns true if every value in the column represents a double.
   bool IsColumnDoubles(int aColumn) const;

   bool IsColumnValidSeries(int aColumnIndex) const;
   bool IsColumnValidSeries(const QString& aColumnName) const;
};
} // namespace WkfFilter

#endif
