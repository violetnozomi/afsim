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

#ifndef WKF_BM_PLOT_PREF_OBJECTS_HPP
#define WKF_BM_PLOT_PREF_OBJECTS_HPP

#include <QList>
#include <QStringList>

#include "WkfPrefObject.hpp"
#include "misc/WkfExportHelper.hpp"

namespace wkf
{
namespace bm
{
struct PlotAxisBounds
{
   enum class Mode
   {
      Value,
      Range,
      Automatic
   };

   Mode   mTimeMode  = Mode::Automatic;
   double mTimeMin   = 0;
   double mTimeMax   = 0;
   double mTimeRange = 0;

   double mInitialTime = 0;
};

struct PlatformSideExport
{
   PlotAxisBounds mBounds;

   QStringList mPlatforms;
   QStringList mWeapons;
   QStringList mPlatformOverlays;
   QStringList mWeaponOverlays;
};

struct TypeSeries
{
   QString mSide;
   QString mType;
   QString mCategory;
   bool    mVisible = true;
   bool    mOverlay = false;
};

struct PlatformTypeExport
{
   PlotAxisBounds mBounds;

   QList<TypeSeries> mSeries;
   TypeSeries        mPreview;
};

class SidePlotPrefObject final : public wkf::ExportPrefObject<PlatformSideExport>
{
   Q_OBJECT

public:
   static constexpr const char* cNAME = "PlatformSidePlotExports";

   SidePlotPrefObject(QObject* aParent = nullptr) noexcept;
   ~SidePlotPrefObject() noexcept override = default;

   bool IsSharedPreference() const override { return true; }

private:
   PlatformSideExport ReadExport(QSettings& aSettings) const override;
   void               SaveExport(QSettings& aSettings, const PlatformSideExport& aExport) const override;
};

class TypePlotPrefObject final : public wkf::ExportPrefObject<PlatformTypeExport>
{
   Q_OBJECT

public:
   static constexpr const char* cNAME = "PlatformTypePlotExports";

   TypePlotPrefObject(QObject* aParent = nullptr) noexcept;
   ~TypePlotPrefObject() noexcept override = default;

   bool IsSharedPreference() const override { return true; }

private:
   PlatformTypeExport ReadExport(QSettings& aSettings) const override;
   void               SaveExport(QSettings& aSettings, const PlatformTypeExport& aExport) const override;

   TypeSeries ReadSeries(QSettings& aSettings) const;
   void       SaveSeries(QSettings& aSettings, const TypeSeries& aSeries) const;
};
} // namespace bm
} // namespace wkf

Q_DECLARE_METATYPE(wkf::bm::PlatformSideExport);
Q_DECLARE_METATYPE(wkf::bm::SidePlotPrefObject::ExportList);

Q_DECLARE_METATYPE(wkf::bm::PlatformTypeExport);
Q_DECLARE_METATYPE(wkf::bm::TypePlotPrefObject::ExportList);

#endif
