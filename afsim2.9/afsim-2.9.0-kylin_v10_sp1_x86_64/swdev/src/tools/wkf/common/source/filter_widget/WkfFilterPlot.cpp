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

#include "WkfFilterPlot.hpp"

#include <QMessageBox>

namespace WkfFilter
{
constexpr int cSERIES_LIMIT = 100;

static bool IsDouble(const QVariant& aVariant)
{
   bool success;
   aVariant.toDouble(&success);
   return success;
}

FilterPlot::FilterPlot(QWidget* aParent)
   : QDialog(aParent)
   , mSourceModel(nullptr)
   , mDefaultSeriesEnabled(false)
   , mXColumn(-1)
   , mYColumn(-1)
   , mSeriesColumn(-1)
   , mSeriesEnabled(false)
{
   mUi.setupUi(this);

   connect(mUi.xAxisComboBox, &QComboBox::currentTextChanged, this, QOverload<const QString&>::of(&FilterPlot::SetXAxisColumn));
   connect(mUi.yAxisComboBox, &QComboBox::currentTextChanged, this, QOverload<const QString&>::of(&FilterPlot::SetYAxisColumn));
   connect(mUi.seriesComboBox,
           &QComboBox::currentTextChanged,
           this,
           QOverload<const QString&>::of(&FilterPlot::SetSeriesColumn));

   connect(mUi.seriesCheckBox, &QCheckBox::clicked, this, &FilterPlot::SetSeriesColumnEnabled);
}

void FilterPlot::SetSourceModel(QAbstractItemModel* aModel)
{
   mSourceModel = aModel;

   mXColumn       = ColumnIndexFromName(mDefaultXColumn);
   mYColumn       = ColumnIndexFromName(mDefaultYColumn);
   mSeriesColumn  = ColumnIndexFromName(mDefaultSeriesColumn);
   mSeriesEnabled = mDefaultSeriesEnabled;

   // ReloadModelP();
}

void FilterPlot::ReloadModel()
{
   if (isVisible())
   {
      ReloadModelP();
   }
}

void FilterPlot::MarkColumnAsValidSeries(const QString& aColumnName, bool aValid)
{
   if (aColumnName.isEmpty())
   {
      // Do nothing
   }
   else if (aValid)
   {
      mValidSeriesColumns.insert(aColumnName);
   }
   else
   {
      mValidSeriesColumns.erase(aColumnName);
   }
}

void FilterPlot::SetDefaultXAxisColumn(const QString& aColumnName)
{
   mDefaultXColumn = aColumnName;
}
void FilterPlot::SetDefaultYAxisColumn(const QString& aColumnName)
{
   mDefaultYColumn = aColumnName;
}
void FilterPlot::SetDefaultSeriesColumn(const QString& aColumnName, bool aEnableByDefault)
{
   mDefaultSeriesColumn  = aColumnName;
   mDefaultSeriesEnabled = aEnableByDefault;
}

// Member accessors
void FilterPlot::SetXAxisColumn(int aColumnIndex)
{
   if (IsColumnDoubles(aColumnIndex))
   {
      mXColumn = aColumnIndex;
      Invalidate();
   }
}
void FilterPlot::SetXAxisColumn(const QString& aColumnName)
{
   int index = ColumnIndexFromName(aColumnName);
   if (index >= 0)
   {
      SetXAxisColumn(index);
   }
}
void FilterPlot::SetYAxisColumn(int aColumnIndex)
{
   if (IsColumnDoubles(aColumnIndex))
   {
      mYColumn = aColumnIndex;
      Invalidate();
   }
}
void FilterPlot::SetYAxisColumn(const QString& aColumnName)
{
   int index = ColumnIndexFromName(aColumnName);
   if (index >= 0)
   {
      SetYAxisColumn(index);
   }
}
bool FilterPlot::SetSeriesColumn(int aColumnIndex)
{
   return SetSeriesColumn(ColumnNameFromIndex(aColumnIndex));
}
bool FilterPlot::SetSeriesColumn(const QString& aColumnName)
{
   if (IsColumnValidSeries(aColumnName))
   {
      mSeriesColumn = ColumnIndexFromName(aColumnName);
      SetSeriesColumnEnabled(true);
      return true;
   }
   return false;
}
void FilterPlot::SetSeriesColumnEnabled(bool aEnable)
{
   mSeriesEnabled = aEnable;
   mUi.seriesCheckBox->setChecked(aEnable);
   Invalidate();
}

// Member mutators
int FilterPlot::GetXAxisColumnIndex() const
{
   return mXColumn;
}
QString FilterPlot::GetXAxisColumnName() const
{
   return ColumnNameFromIndex(mXColumn);
}
int FilterPlot::GetYAxisColumnIndex() const
{
   return mYColumn;
}
QString FilterPlot::GetYAxisColumnName() const
{
   return ColumnNameFromIndex(mYColumn);
}
int FilterPlot::GetSeriesColumnIndex() const
{
   return mSeriesColumn;
}
QString FilterPlot::GetSeriesColumnName() const
{
   return ColumnNameFromIndex(mSeriesColumn);
}
bool FilterPlot::IsSeriesColumnEnabled() const
{
   return mSeriesEnabled;
}

void FilterPlot::SetSeriesVisible(const QString& aSeriesName, bool aVisible)
{
   mSeriesVisible[aSeriesName] = aVisible;
   DrawPlot();
}
bool FilterPlot::IsSeriesVisible(const QString& aSeriesName) const
{
   if (!IsSeriesColumnEnabled())
   {
      return aSeriesName.isEmpty();
   }

   auto it = mSeriesVisible.find(aSeriesName);
   if (it != mSeriesVisible.end())
   {
      return it->second;
   }
   return true;
}

void FilterPlot::Invalidate()
{
   mSeriesData.clear();

   if (mSourceModel != nullptr)
   {
      const int cROW_COUNT = mSourceModel->rowCount();

      // Collect & Organize Data
      for (int i = 0; i < cROW_COUNT; i++)
      {
         mSeriesData[ControlledSeriesData(i)].emplace_back(DoubleData(i, mXColumn), DoubleData(i, mYColumn));
      }
   }

   // Setup UI elements.
   SetCheckBoxList();

   // Draw
   DrawPlot();
}
void FilterPlot::DrawPlot()
{
   UtQtGL2DPlot& plot = *mUi.plotWidget->GetPlot();

   plot.ClearData();

   plot.SetDefaultPlotStyle(UtQtGL2DPlot::PlotStyle::LINE);
   plot.SetLabelXAxis(mUi.xAxisComboBox->currentText());
   plot.SetLabelYAxis(mUi.yAxisComboBox->currentText());

   int index = 1;
   for (const auto& series : mSeriesData)
   {
      if (IsSeriesVisible(series.first))
      {
         plot.SetSeriesName(series.first, index);
         plot.AddPoints(series.second, index);
         plot.SetSeriesWidth(2.0f, index);
      }
      index++;
   }

   plot.MakePlotPretty();
   mUi.plotWidget->update();
}

QString FilterPlot::ColumnNameFromIndex(int aIndex) const
{
   if (mSourceModel != nullptr && aIndex >= 0)
   {
      return mSourceModel->headerData(aIndex, Qt::Horizontal).toString();
   }
   return QString();
}

int FilterPlot::ColumnIndexFromName(const QString& aName) const
{
   if (mSourceModel != nullptr)
   {
      const int cCOLUMN_COUNT = mSourceModel->columnCount();
      for (int i = 0; i < cCOLUMN_COUNT; i++)
      {
         if (mSourceModel->headerData(i, Qt::Horizontal).toString() == aName)
         {
            return i;
         }
      }
   }
   return -1;
}

QString FilterPlot::ControlledSeriesData(int aRow) const
{
   if (IsSeriesColumnEnabled() && mSourceModel != nullptr)
   {
      return mSourceModel->data(mSourceModel->index(aRow, mSeriesColumn)).toString();
   }
   return QString();
}

double FilterPlot::DoubleData(int aRow, int aColumn) const
{
   if (mSourceModel != nullptr)
   {
      return mSourceModel->data(mSourceModel->index(aRow, aColumn)).toDouble();
   }
   return 0;
}

void FilterPlot::ReloadModelP()
{
   SetComboBoxOptions();

   if (!IsColumnDoubles(mXColumn))
   {
      mXColumn = ColumnIndexFromName(mUi.xAxisComboBox->currentText());
   }

   if (!IsColumnDoubles(mYColumn))
   {
      mYColumn = ColumnIndexFromName(mUi.yAxisComboBox->currentText());
   }

   if (!IsColumnValidSeries(mSeriesColumn))
   {
      if (!mValidSeriesColumns.empty())
      {
         mSeriesColumn = ColumnIndexFromName(*mValidSeriesColumns.begin());
      }
      else
      {
         mSeriesColumn = -1;
      }
   }

   SetComboBoxStatus();

   Invalidate();

   if (static_cast<int>(mSeriesData.size()) > cSERIES_LIMIT)
   {
      QMessageBox::warning(
         this,
         "Series limit exceeded",
         QString("Only up to %1 series may be present in the plot. Use filters to refine your search.").arg(cSERIES_LIMIT));
   }
}

void FilterPlot::SetComboBoxOptions()
{
   mUi.xAxisComboBox->blockSignals(true);
   mUi.yAxisComboBox->blockSignals(true);
   mUi.seriesComboBox->blockSignals(true);

   mUi.xAxisComboBox->clear();
   mUi.yAxisComboBox->clear();
   mUi.seriesComboBox->clear();

   const int cCOLUMN_COUNT = mSourceModel->columnCount();
   for (int i = 0; i < cCOLUMN_COUNT; i++)
   {
      const QString cCOLUMN_TITLE = ColumnNameFromIndex(i);

      if (IsColumnDoubles(i))
      {
         mUi.xAxisComboBox->addItem(cCOLUMN_TITLE);
         mUi.yAxisComboBox->addItem(cCOLUMN_TITLE);
      }
      if (IsColumnValidSeries(cCOLUMN_TITLE))
      {
         mUi.seriesComboBox->addItem(cCOLUMN_TITLE);
      }
   }

   mUi.xAxisComboBox->blockSignals(false);
   mUi.yAxisComboBox->blockSignals(false);
   mUi.seriesComboBox->blockSignals(false);
}

void FilterPlot::SetComboBoxStatus()
{
   mUi.xAxisComboBox->blockSignals(true);
   mUi.yAxisComboBox->blockSignals(true);
   mUi.seriesComboBox->blockSignals(true);
   mUi.seriesCheckBox->blockSignals(true);

   mUi.xAxisComboBox->setCurrentText(ColumnNameFromIndex(mXColumn));
   mUi.yAxisComboBox->setCurrentText(ColumnNameFromIndex(mYColumn));
   mUi.seriesComboBox->setCurrentText(ColumnNameFromIndex(mSeriesColumn));

   mUi.seriesCheckBox->setChecked(mSeriesEnabled);

   mUi.xAxisComboBox->blockSignals(false);
   mUi.yAxisComboBox->blockSignals(false);
   mUi.seriesComboBox->blockSignals(false);
   mUi.seriesCheckBox->blockSignals(false);
}

void FilterPlot::SetCheckBoxList()
{
   // Clear the list
   mUi.checkBoxList->clear();

   // Crate new list
   if (mSeriesEnabled)
   {
      for (const auto& series : mSeriesData)
      {
         QString name     = series.first;
         auto    listItem = new QListWidgetItem(mUi.checkBoxList);
         auto    checkBox = new QCheckBox;

         checkBox->setText(name.isEmpty() ? "(none)" : name);
         checkBox->setChecked(IsSeriesVisible(name));
         connect(checkBox, &QCheckBox::clicked, [this, name](bool aChecked) { SetSeriesVisible(name, aChecked); });
         mUi.checkBoxList->setItemWidget(listItem, checkBox);
      }
   }
}

bool FilterPlot::IsColumnDoubles(int aColumn) const
{
   if (mSourceModel != nullptr && aColumn >= 0)
   {
      const int cROW_COUNT = mSourceModel->rowCount();

      for (int i = 0; i < cROW_COUNT; i++)
      {
         const QVariant& cDATA = mSourceModel->data(mSourceModel->index(i, aColumn));

         if (!IsDouble(cDATA))
         {
            return false;
         }
      }

      return true;
   }
   return false;
}

bool FilterPlot::IsColumnValidSeries(int aColumnIndex) const
{
   return IsColumnValidSeries(ColumnNameFromIndex(aColumnIndex));
}

bool FilterPlot::IsColumnValidSeries(const QString& aColumnName) const
{
   return mValidSeriesColumns.count(aColumnName) > 0;
}
} // namespace WkfFilter
