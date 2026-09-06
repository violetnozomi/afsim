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
#ifndef WKFTypeVisibilityPREFWIDGET_HPP
#define WKFTypeVisibilityPREFWIDGET_HPP

#include "wkf_common_export.h"

#include "WkfPrefWidget.hpp"
#include "WkfTypeVisibilityPrefObject.hpp"
#include "ui_WkfTypeVisibilityPrefWidget.h"

namespace wkf
{
class WKF_COMMON_EXPORT TypeVisibilityPrefWidget : public PrefWidgetT<TypeVisibilityPrefObject>
{
public:
   TypeVisibilityPrefWidget(QWidget* aParent = nullptr);

   QString GetCategoryHint() const override { return "Visibility"; }

private:
   void showEvent(QShowEvent* aEvent) override;

   void ReadPreferenceData(const TypeVisibilityPrefData& aTypeVisibilityPrefData) override;
   void WritePreferenceData(TypeVisibilityPrefData& aTypeVisibilityPrefData) override;

   void AddFilter();
   void RemoveFilters();
   bool eventFilter(QObject* aObject, QEvent* aEvent) override;

   Ui::WkfTypeVisibilityPrefWidget mUI;
};
} // namespace wkf
#endif
