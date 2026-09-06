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

#ifndef WKF_FILTER_PREF_OBJECT_HPP
#define WKF_FILTER_PREF_OBJECT_HPP

#include <vector>

#include "WkfFilterObject.hpp"
#include "misc/WkfExportHelper.hpp"

namespace WkfFilter
{
struct FilterExport
{
   FilterObject::Type mType;
   bool               mEnabled;
   bool               mInverted;
   int                mColumn;
   QString            mRule;
   QString            mValue;

   std::vector<FilterExport> mChildren;
};

class WKF_COMMON_EXPORT FilterPrefObject : public wkf::ExportPrefObject<FilterExport>
{
public:
   FilterPrefObject(QObject* aParent, const QString& aName) noexcept;

   FilterExport ReadExport(QSettings& aSettings) const override;
   void         SaveExport(QSettings& aSettings, const FilterExport& aPrefs) const override;
};
} // namespace WkfFilter

Q_DECLARE_METATYPE(WkfFilter::FilterExport);

#endif
