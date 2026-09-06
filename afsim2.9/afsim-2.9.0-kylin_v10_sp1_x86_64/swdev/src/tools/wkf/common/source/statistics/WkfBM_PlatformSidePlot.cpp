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

#include "WkfBM_PlatformSidePlot.hpp"

#include "WkfEnvironment.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"

const QString cPLATFORMS = "Platforms";
const QString cWEAPONS   = "Weapons";

wkf::bm::PlatformSidePlot::PlatformSidePlot(PlatformPlotDatabase& aDatabase,
                                            SidePlotPrefObject&   aPrefObject,
                                            QString               aTitle,
                                            QMainWindow*          aParent /* = nullptr */)
   : PlotWidget(aTitle, aParent)
   , mPrefObject(aPrefObject)
   , mExportHelper(mPrefObject, "Settings")
   , mDatabase(aDatabase)
{
   Initialize();

   auto* prefs = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>();
   if (prefs)
   {
      connect(prefs, &wkf::TeamVisibilityPrefObject::TeamColorChanged, this, &PlatformSidePlot::TeamVisibilityPrefsChanged);
      connect(prefs, &wkf::TeamVisibilityPrefObject::TeamVisibilityChanged, this, &PlatformSidePlot::TeamVisibilityPrefsChanged);
   }
   else
   {
      throw std::runtime_error("wkf::TeamVisibilityPrefObject not found.");
   }

   connect(&mExportHelper, &wkf::ExportHelper::WriteState, this, &PlatformSidePlot::Export);
   connect(&mExportHelper, &wkf::ExportHelper::ReadState, this, &PlatformSidePlot::Import);
}

void wkf::bm::PlatformSidePlot::AddSide(const QString& aSide)
{
   if (!mSides.contains(aSide))
   {
      AddSideP(aSide);
   }
   else
   {
      throw std::runtime_error(QString("Side already exists: %1").arg(aSide).toStdString());
   }
}

void wkf::bm::PlatformSidePlot::AddData(const QString& aSide, double aTime, int aPlatformCount, int aWeaponCount)
{
   if (!mSides.contains(aSide))
   {
      AddSideP(aSide);
   }

   AddPoint(cPLATFORMS, aSide, aTime, aPlatformCount);
   AddPoint(cWEAPONS, aSide, aTime, aWeaponCount);
}

void wkf::bm::PlatformSidePlot::Update(double aMaxTime)
{
   while (mLastSimTime < aMaxTime)
   {
      for (const auto& side : mSides)
      {
         const int platformCount = mDatabase.PlatformCount(mLastSimTime, side);
         const int weaponCount   = mDatabase.WeaponCount(mLastSimTime, side);

         AddData(side, mLastSimTime, platformCount, weaponCount);
      }

      mLastSimTime += 0.05;
   }

   if (mDatabase.EntryCount() > mDatabaseEntryCount)
   {
      auto newSides       = mDatabase.GetSides();
      mDatabaseEntryCount = mDatabase.EntryCount();
      for (const auto& side : newSides)
      {
         if (!mSides.contains(side))
         {
            AddSideP(side);
            for (double time = 0; time < aMaxTime; time += 0.05)
            {
               AddData(side, time, mDatabase.PlatformCount(time, side), mDatabase.WeaponCount(time, side));
            }
         }
      }
   }
}

void wkf::bm::PlatformSidePlot::Reset()
{
   PlotWidget::Reset();
   mSides.clear();
   Initialize();
}

void wkf::bm::PlatformSidePlot::BuildMenu(QMenu& aMenu)
{
   mExportHelper.BuildContextMenu(aMenu);
}

void wkf::bm::PlatformSidePlot::Export(QString aName)
{
   PlatformSideExport prefs;

   prefs.mBounds = ExportBounds();

   for (const auto& side : mSides)
   {
      if (IsSeriesVisible(cPLATFORMS, side))
      {
         prefs.mPlatforms += side;
      }
      if (IsSeriesVisible(cWEAPONS, side))
      {
         prefs.mWeapons += side;
      }
      if (IsOverlayVisible(cPLATFORMS, side))
      {
         prefs.mPlatformOverlays += side;
      }
      if (IsOverlayVisible(cWEAPONS, side))
      {
         prefs.mWeaponOverlays += side;
      }
   }

   mPrefObject.Export(aName, std::move(prefs));
}

void wkf::bm::PlatformSidePlot::Import(QString aName)
{
   const auto& prefs = mPrefObject.GetExport(aName);

   ImportBounds(prefs.mBounds);

   for (const auto& side : mSides)
   {
      SetSeriesVisible(cPLATFORMS, side, prefs.mPlatforms.contains(side));
      SetSeriesVisible(cWEAPONS, side, prefs.mWeapons.contains(side));
      SetOverlayVisible(cPLATFORMS, side, prefs.mPlatformOverlays.contains(side));
      SetOverlayVisible(cWEAPONS, side, prefs.mWeaponOverlays.contains(side));
   }
}

void wkf::bm::PlatformSidePlot::AddSideP(const QString& aSide)
{
   auto* prefs = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>();
   if (prefs)
   {
      auto color   = prefs->GetTeamColor(aSide);
      bool visible = prefs->IsTeamVisible(aSide);
      auto pIndex  = AddSeries(cPLATFORMS, aSide);
      auto wIndex  = AddSeries(cWEAPONS, aSide, UtQtGL2DPlot::StipplePattern::DASHED);

      SetSeriesColor(pIndex, color);
      SetSeriesColor(wIndex, color);

      SetSeriesEnabled(pIndex, visible);
      SetSeriesEnabled(wIndex, visible);

      mSides.append(aSide);
   }
   else
   {
      throw std::runtime_error("wkf::TeamVisibilityPrefObject not found.");
   }
}

void wkf::bm::PlatformSidePlot::Initialize()
{
   SetNameManglingPattern("%2 %1");

   AddGroup(cPLATFORMS);
   AddGroup(cWEAPONS);
}

void wkf::bm::PlatformSidePlot::TeamVisibilityPrefsChanged()
{
   auto* prefs = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>();

   for (const auto& side : mSides)
   {
      auto color   = prefs->GetTeamColor(side);
      bool visible = prefs->IsTeamVisible(side);
      SetSeriesColor(cPLATFORMS, side, color);
      SetSeriesColor(cWEAPONS, side, color);
      SetSeriesEnabled(cPLATFORMS, side, visible);
      SetSeriesEnabled(cWEAPONS, side, visible);
   }
}
