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

#include "WkfBM_PlotPrefObjects.hpp"

static wkf::bm::PlotAxisBounds ReadBounds(QSettings& aSettings)
{
   wkf::bm::PlotAxisBounds retval;

   ut::qt::GroupScope gs{aSettings, "bounds"};

   retval.mTimeMode  = static_cast<wkf::bm::PlotAxisBounds::Mode>(aSettings.value("mode").toInt());
   retval.mTimeMin   = aSettings.value("time_min", 0).toDouble();
   retval.mTimeMax   = aSettings.value("time_max", 0).toDouble();
   retval.mTimeRange = aSettings.value("time_range", 0).toDouble();

   retval.mInitialTime = aSettings.value("initial_time", 0).toDouble();

   return retval;
}

static void SaveBounds(QSettings& aSettings, const wkf::bm::PlotAxisBounds& aBounds)
{
   ut::qt::GroupScope gs{aSettings, "bounds"};

   aSettings.setValue("mode", static_cast<int>(aBounds.mTimeMode));
   aSettings.setValue("time_min", aBounds.mTimeMin);
   aSettings.setValue("time_max", aBounds.mTimeMax);
   aSettings.setValue("time_range", aBounds.mTimeRange);

   aSettings.setValue("initial_time", aBounds.mInitialTime);
}

wkf::bm::SidePlotPrefObject::SidePlotPrefObject(QObject* aParent /* = nullptr */) noexcept
   : wkf::ExportPrefObject<PlatformSideExport>(aParent, cNAME)
{
}

wkf::bm::PlatformSideExport wkf::bm::SidePlotPrefObject::ReadExport(QSettings& aSettings) const
{
   PlatformSideExport retval;

   retval.mBounds = ReadBounds(aSettings);

   retval.mPlatforms = aSettings.value("platforms").toStringList();
   retval.mWeapons   = aSettings.value("weapons").toStringList();

   retval.mPlatformOverlays = aSettings.value("platform_overlays").toStringList();
   retval.mWeaponOverlays   = aSettings.value("weapon_overlays").toStringList();

   return retval;
}

void wkf::bm::SidePlotPrefObject::SaveExport(QSettings& aSettings, const PlatformSideExport& aExport) const
{
   SaveBounds(aSettings, aExport.mBounds);

   aSettings.setValue("platforms", aExport.mPlatforms);
   aSettings.setValue("weapons", aExport.mWeapons);

   aSettings.setValue("platform_overlays", aExport.mPlatformOverlays);
   aSettings.setValue("weapon_overlays", aExport.mWeaponOverlays);
}

wkf::bm::TypePlotPrefObject::TypePlotPrefObject(QObject* aParent /* = nullptr */) noexcept
   : wkf::ExportPrefObject<PlatformTypeExport>(aParent, cNAME)
{
}

wkf::bm::PlatformTypeExport wkf::bm::TypePlotPrefObject::ReadExport(QSettings& aSettings) const
{
   PlatformTypeExport retval;

   retval.mBounds = ReadBounds(aSettings);

   { // RAII block
      ut::qt::GroupScope gs{aSettings, "preview"};
      retval.mPreview = ReadSeries(aSettings);
   }

   ut::qt::ReadArrayScope ras{aSettings, "series"};
   for (int i = 0; i < ras.Size(); i++)
   {
      aSettings.setArrayIndex(i);
      retval.mSeries.append(ReadSeries(aSettings));
   }

   return retval;
}

void wkf::bm::TypePlotPrefObject::SaveExport(QSettings& aSettings, const PlatformTypeExport& aExport) const
{
   SaveBounds(aSettings, aExport.mBounds);

   { // RAII block
      ut::qt::GroupScope gs{aSettings, "preview"};
      SaveSeries(aSettings, aExport.mPreview);
   }

   ut::qt::WriteArrayScope was{aSettings, "series"};
   for (int i = 0; i < aExport.mSeries.size(); i++)
   {
      aSettings.setArrayIndex(i);
      SaveSeries(aSettings, aExport.mSeries[i]);
   }
}

wkf::bm::TypeSeries wkf::bm::TypePlotPrefObject::ReadSeries(QSettings& aSettings) const
{
   TypeSeries retval;

   retval.mSide     = aSettings.value("side").toString();
   retval.mType     = aSettings.value("type").toString();
   retval.mCategory = aSettings.value("category").toString();
   retval.mVisible  = aSettings.value("visible", true).toBool();
   retval.mOverlay  = aSettings.value("overlay", false).toBool();

   return retval;
}

void wkf::bm::TypePlotPrefObject::SaveSeries(QSettings& aSettings, const TypeSeries& aSeries) const
{
   aSettings.setValue("side", aSeries.mSide);
   aSettings.setValue("type", aSeries.mType);
   aSettings.setValue("category", aSeries.mCategory);
   aSettings.setValue("visible", aSeries.mVisible);
   aSettings.setValue("overlay", aSeries.mOverlay);
}
