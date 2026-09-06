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

#ifndef WKF_BM_PLATFORM_STATUS_PREF_OBJECT_HPP
#define WKF_BM_PLATFORM_STATUS_PREF_OBJECT_HPP

#include <map>

#include <QByteArray>
#include <QList>
#include <QStringList>

#include "UtOptional.hpp"
#include "WkfBM_ColorRule.hpp"
#include "WkfBM_RuleSet.hpp"
#include "WkfPrefObject.hpp"
#include "misc/WkfExportHelper.hpp"

namespace wkf
{
namespace bm
{
struct ColorData
{
   QColor  mColor;
   QString mComparison;
   QString mValue;
   bool    mBool = false;
};

struct ColumnData
{
   QString          mName;
   ColorSetting     mDefaultColor;
   ColorRule::Mode  mMode   = ColorRule::Mode::Number;
   bool             mHidden = false;
   QList<ColorData> mColors;
};

struct Configuration
{
   QByteArray mSplitter;

   QList<QStringList> mWhiteList;
   QList<QStringList> mBlackList;

   QList<ColumnData> mColumns;
   // QByteArray        mHeader;
};

class PlatformStatusPrefObject final : public wkf::ExportPrefObject<Configuration>
{
   Q_OBJECT

public:
   static constexpr const char* cNAME = "PlatformStatusConfigs";

   PlatformStatusPrefObject(QObject* aParent = nullptr) noexcept;
   ~PlatformStatusPrefObject() noexcept override = default;

   bool IsSharedPreference() const override { return true; }

signals:
   void ReadComplete();

private:
   void Apply() override { emit ReadComplete(); }

   Configuration ReadExport(QSettings& aSettings) const override;
   void          SaveExport(QSettings& aSettings, const Configuration& aConfiguration) const override;

   ColumnData ReadColumn(QSettings& aSettings) const;
   void       SaveColumn(QSettings& aSettings, const ColumnData& aColumn) const;

   ColorData ReadColor(QSettings& aSettings) const;
   void      SaveColor(QSettings& aSettings, const ColorData& aColor) const;
};
} // namespace bm
} // namespace wkf

Q_DECLARE_METATYPE(wkf::bm::Configuration);
Q_DECLARE_METATYPE(wkf::bm::PlatformStatusPrefObject::ExportList);

#endif
