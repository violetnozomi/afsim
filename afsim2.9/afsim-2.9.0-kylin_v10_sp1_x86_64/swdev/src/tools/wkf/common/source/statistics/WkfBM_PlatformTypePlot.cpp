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

#include "WkfBM_PlatformTypePlot.hpp"

#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QString>

#include "UtQtMemory.hpp"
#include "WkfBM_PlatformDatabase.hpp"
#include "WkfEnvironment.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"

const QString cSERIES = "Series";
const QString cANY    = "(any)";
const QString cNONE   = "(none)";

wkf::bm::PlatformTypePlot::PlatformTypePlot(PlatformPlotDatabase& aDatabase,
                                            TypePlotPrefObject&   aPrefObject,
                                            QString               aTitle,
                                            QMainWindow*          aParent)
   : PlotWidget(std::move(aTitle), aParent)
   , mPrefObject(aPrefObject)
   , mExportHelper(mPrefObject, "Settings")
   , mDatabase(aDatabase)
{
   Initialize();

   auto* prefs = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>();
   if (prefs)
   {
      connect(prefs, &wkf::TeamVisibilityPrefObject::TeamColorChanged, this, &PlatformTypePlot::TeamVisibilityPrefsChanged);
      connect(prefs, &wkf::TeamVisibilityPrefObject::TeamVisibilityChanged, this, &PlatformTypePlot::TeamVisibilityPrefsChanged);
   }
   else
   {
      throw std::runtime_error("wkf::TeamVisibilityPrefObject not found.");
   }

   connect(&mExportHelper, &wkf::ExportHelper::WriteState, this, &PlatformTypePlot::Export);
   connect(&mExportHelper, &wkf::ExportHelper::ReadState, this, &PlatformTypePlot::Import);
}

void wkf::bm::PlatformTypePlot::Update(double aMaxTime)
{
   if (mDatabase.EntryCount() > mDatabaseEntryCount)
   {
      mDatabaseEntryCount = mDatabase.EntryCount();
      UpdateComboBoxOptions();
   }

   for (auto& series : mSeriesInfo)
   {
      if (IsSeriesVisible(series.mIndex))
      {
         while (series.mTime < aMaxTime)
         {
            AddPoint(series.mIndex, series.mTime, GetPlatformCount(series));
            series.mTime += 0.05;
         }
      }
   }
}

void wkf::bm::PlatformTypePlot::Reset()
{
   PlotWidget::Reset();
   mSeriesInfo.clear();

   Initialize();
}

void wkf::bm::PlatformTypePlot::BuildMenu(QMenu& aMenu)
{
   mExportHelper.BuildContextMenu(aMenu);
}

void wkf::bm::PlatformTypePlot::Export(QString aName)
{
   PlatformTypeExport prefs;

   prefs.mBounds = ExportBounds();

   for (size_t i = 1; i < mSeriesInfo.size(); i++)
   {
      const auto& series = mSeriesInfo[i];
      TypeSeries  ts;

      ts.mSide     = series.mSide;
      ts.mType     = series.mType;
      ts.mCategory = series.mCategory;
      ts.mVisible  = IsSeriesVisible(series.mIndex);
      ts.mOverlay  = IsOverlayVisible(series.mIndex);

      prefs.mSeries.append(ts);
   }

   const auto& previewSeries = mSeriesInfo[0];
   prefs.mPreview.mSide      = previewSeries.mSide;
   prefs.mPreview.mType      = previewSeries.mType;
   prefs.mPreview.mCategory  = previewSeries.mCategory;
   prefs.mPreview.mVisible   = IsSeriesVisible(previewSeries.mIndex);
   prefs.mPreview.mOverlay   = IsOverlayVisible(previewSeries.mIndex);

   mPrefObject.Export(aName, std::move(prefs));
}

void wkf::bm::PlatformTypePlot::Import(QString aName)
{
   Reset();

   const auto& prefs = mPrefObject.GetExport(aName);

   ImportBounds(prefs.mBounds);

   for (const auto& series : prefs.mSeries)
   {
      mSeriesInfo.emplace_back();
      auto& newSeries = mSeriesInfo.back();

      newSeries.mTime     = 0;
      newSeries.mSide     = series.mSide;
      newSeries.mType     = series.mType;
      newSeries.mCategory = series.mCategory;

      newSeries.mIndex = AddSeries(cSERIES, newSeries.mSide + " " + newSeries.mType + ", " + newSeries.mCategory);

      SetSeriesVisible(newSeries.mIndex, series.mVisible);
      SetOverlayVisible(newSeries.mIndex, series.mOverlay);
   }

   TeamVisibilityPrefsChanged();

   mSideComboBox->setCurrentText(prefs.mPreview.mSide);
   mTypeComboBox->setCurrentText(prefs.mPreview.mType);
   mCategoryComboBox->setCurrentText(prefs.mPreview.mCategory);

   SetSeriesVisible(mSeriesInfo[0].mIndex, prefs.mPreview.mVisible);
   SetOverlayVisible(mSeriesInfo[0].mIndex, prefs.mPreview.mOverlay);
}

void wkf::bm::PlatformTypePlot::Initialize()
{
   AddGroup(cSERIES);
   SetNameManglingPattern("%2");

   mSideComboBox     = ut::qt::make_qt_ptr<QComboBox>();
   mTypeComboBox     = ut::qt::make_qt_ptr<QComboBox>();
   mCategoryComboBox = ut::qt::make_qt_ptr<QComboBox>();
   mAddButton        = ut::qt::make_qt_ptr<QPushButton>("Add Series");

   mCategoryComboBox->addItem(cANY);
   mCategoryComboBox->addItem(cNONE);

   mSeriesInfo.emplace_back();
   mSeriesInfo.back().mIndex = AddSeries(cSERIES, "Preview");
   SetSeriesColor(mSeriesInfo.back().mIndex, QColor{255, 255, 255});

   connect(mSideComboBox, &QComboBox::currentTextChanged, this, &PlatformTypePlot::ComboBoxChanged);
   connect(mTypeComboBox, &QComboBox::currentTextChanged, this, &PlatformTypePlot::ComboBoxChanged);
   connect(mCategoryComboBox, &QComboBox::currentTextChanged, this, &PlatformTypePlot::ComboBoxChanged);

   connect(mAddButton, &QPushButton::clicked, this, &PlatformTypePlot::AddSeriesClicked);

   InsertRightPanelWidget(0, ut::qt::make_qt_ptr<QLabel>("Side:"));
   InsertRightPanelWidget(1, mSideComboBox);
   InsertRightPanelWidget(2, ut::qt::make_qt_ptr<QLabel>("Type:"));
   InsertRightPanelWidget(3, mTypeComboBox);
   InsertRightPanelWidget(4, ut::qt::make_qt_ptr<QLabel>("Category:"));
   InsertRightPanelWidget(5, mCategoryComboBox);

   InsertRightPanelWidget(6, mAddButton);

   UpdateComboBoxOptions();
}

void wkf::bm::PlatformTypePlot::TeamVisibilityPrefsChanged()
{
   auto* prefs = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>();

   // Adjust existing series
   for (const auto& series : mSeriesInfo)
   {
      SetSeriesColor(series.mIndex, prefs->GetTeamColor(series.mSide));
      SetSeriesEnabled(series.mIndex, prefs->IsTeamVisible(series.mSide));
   }

   UpdateComboBoxOptions();
}

void wkf::bm::PlatformTypePlot::UpdateComboBoxOptions()
{
   auto* prefs = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>();

   // Update sides
   QString currentSide = mSideComboBox->currentText();
   mSeriesInfo[0].mSide.clear();
   mSideComboBox->clear();

   auto sides = mDatabase.GetSides();
   for (const auto& side : sides)
   {
      if (prefs->IsTeamVisible(side))
      {
         mSideComboBox->addItem(side);
      }
   }

   mSideComboBox->setCurrentText(currentSide);

   // Update types
   QString currentType = mTypeComboBox->currentText();
   mSeriesInfo[0].mType.clear();

   mTypeComboBox->clear();
   mTypeComboBox->addItems(mDatabase.GetTypes());
   mTypeComboBox->setCurrentText(currentType);

   // Update categories
   QString currentCategory = mCategoryComboBox->currentText();
   mSeriesInfo[0].mCategory.clear();

   mCategoryComboBox->clear();
   mCategoryComboBox->addItem(cANY);
   mCategoryComboBox->addItem(cNONE);
   mCategoryComboBox->addItems(mDatabase.GetCategories());
   mCategoryComboBox->setCurrentText(currentCategory);
}

void wkf::bm::PlatformTypePlot::AddSeriesClicked()
{
   const QString newSeriesName = mSeriesInfo[0].mSide + " " + mSeriesInfo[0].mType + ", " + mSeriesInfo[0].mCategory;
   if (IsSeriesVisible(mSeriesInfo[0].mIndex) && !mSeriesInfo[0].mSide.isEmpty() && !SeriesExists(cSERIES, newSeriesName))
   {
      mSeriesInfo.emplace_back(mSeriesInfo[0]);
      mSeriesInfo.back().mTime  = 0;
      mSeriesInfo.back().mIndex = AddSeries(cSERIES, newSeriesName);

      auto* prefs = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>();
      if (prefs)
      {
         SetSeriesColor(mSeriesInfo.back().mIndex, prefs->GetTeamColor(mSeriesInfo[0].mSide));
      }
   }

   SetSeriesVisible(mSeriesInfo[0].mIndex, false);
   SetSeriesVisible(cSERIES, newSeriesName, true);
}

void wkf::bm::PlatformTypePlot::ComboBoxChanged(const QString& aOption)
{
   auto& series     = mSeriesInfo[0];
   series.mSide     = mSideComboBox->currentText();
   series.mType     = mTypeComboBox->currentText();
   series.mCategory = mCategoryComboBox->currentText();

   series.mTime = 0;
   ClearSeries(series.mIndex);
   SetSeriesVisible(series.mIndex, true);
}

int wkf::bm::PlatformTypePlot::GetPlatformCount(const SeriesInfo& aSeries) const noexcept
{
   if (aSeries.mCategory == cANY)
   {
      return mDatabase.TotalCount(aSeries.mTime, aSeries.mSide, aSeries.mType);
   }
   else if (aSeries.mCategory == cNONE)
   {
      return mDatabase.CategoryCount(aSeries.mTime, aSeries.mSide, aSeries.mType, QString());
   }
   else
   {
      return mDatabase.CategoryCount(aSeries.mTime, aSeries.mSide, aSeries.mType, aSeries.mCategory);
   }
}
