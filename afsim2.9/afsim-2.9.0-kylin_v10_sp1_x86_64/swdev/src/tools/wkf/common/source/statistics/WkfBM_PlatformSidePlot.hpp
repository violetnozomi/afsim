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

#ifndef WKF_BM_PLATFORM_SIDE_PLOT_HPP
#define WKF_BM_PLATFORM_SIDE_PLOT_HPP

#include "WkfBM_PlatformDatabase.hpp"
#include "WkfBM_PlotWidget.hpp"

class QSettings;

namespace wkf
{
namespace bm
{
//! A plot widget specialized for showing the platform and weapon counts on each side.
//! Automatically connects to team visibility preferences to ensure the data is shown correctly.
class WKF_COMMON_EXPORT PlatformSidePlot final : public PlotWidget
{
   Q_OBJECT

public:
   PlatformSidePlot(PlatformPlotDatabase& aDatabase, SidePlotPrefObject& aPrefObject, QString aTitle, QMainWindow* aParent);

   //! Attempts to create series for aSide.
   void AddSide(const QString& aSide);

   //! Inserts points into the plot for aSide.
   //! If series have not been created yet for aSide, makes them.
   void AddData(const QString& aSide, double aTime, int aPlatformCount, int aWeaponCount);

   void Update(double aMaxTime) override;

   void Reset() override;

protected:
   void BuildMenu(QMenu& aMenu) override;

private:
   void Export(QString aName);
   void Import(QString aName);

   void AddSideP(const QString& aSide);

   void Initialize();
   void TeamVisibilityPrefsChanged();

   //! Pref object comes before export helper.
   SidePlotPrefObject& mPrefObject;
   wkf::ExportHelper   mExportHelper;

   PlatformPlotDatabase& mDatabase;
   int                   mDatabaseEntryCount = 0;
   double                mLastSimTime        = 0;

   QStringList mSides;
};
} // namespace bm
} // namespace wkf

#endif
