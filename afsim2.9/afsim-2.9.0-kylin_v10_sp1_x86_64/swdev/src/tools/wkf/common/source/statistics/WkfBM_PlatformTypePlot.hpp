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

#ifndef WKF_BM_PLATFORM_TYPE_PLOT_HPP
#define WKF_BM_PLATFORM_TYPE_PLOT_HPP

#include "WkfBM_PlotWidget.hpp"

class QComboBox;
class QPushButton;
class QSettings;

namespace wkf
{
namespace bm
{
class PlatformPlotDatabase;

//! A plot widget specialized for showing the platform and weapon counts on each side.
//! Automatically connects to team visibility preferences to ensure the data is shown correctly.
class WKF_COMMON_EXPORT PlatformTypePlot : public PlotWidget
{
   Q_OBJECT

public:
   PlatformTypePlot(PlatformPlotDatabase& aDatabase, TypePlotPrefObject& aPrefObject, QString aTitle, QMainWindow* aParent);

   void Update(double aMaxTime) override;

   void Reset() override;

protected:
   void BuildMenu(QMenu& aMenu) override;

private:
   void Export(QString aName);
   void Import(QString aName);

   void Initialize();
   void TeamVisibilityPrefsChanged();

   void UpdateComboBoxOptions();

   void AddSeriesClicked();
   void ComboBoxChanged(const QString& aOption);

   struct SeriesInfo
   {
      QString  mSide;
      QString  mType;
      QString  mCategory; // Can be the name of a category, "(any)", or "(none)"
      unsigned mIndex;
      double   mTime = 0;
   };

   int GetPlatformCount(const SeriesInfo& aSeries) const noexcept;

   TypePlotPrefObject& mPrefObject;
   wkf::ExportHelper   mExportHelper;

   PlatformPlotDatabase& mDatabase;
   int                   mDatabaseEntryCount = 0;

   //! The first element is the preview.
   std::vector<SeriesInfo> mSeriesInfo;
   QComboBox*              mSideComboBox     = nullptr;
   QComboBox*              mTypeComboBox     = nullptr;
   QComboBox*              mCategoryComboBox = nullptr;
   QPushButton*            mAddButton        = nullptr;
};
} // namespace bm
} // namespace wkf

#endif
