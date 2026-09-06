// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WKFINTERACTIONPREFWIDGET_HPP
#define WKFINTERACTIONPREFWIDGET_HPP

class QComboBox;
class QSpinBox;
class QTreeWidgetItem;

#include "wkf_common_export.h"

#include "WkfInteractionPrefObject.hpp"
#include "WkfPrefWidget.hpp"
#include "ui_WkfInteractionPrefWidget.h"

namespace wkf
{
class WKF_COMMON_EXPORT InteractionPrefWidget : public PrefWidgetT<InteractionPrefObject>
{
public:
   InteractionPrefWidget();
   ~InteractionPrefWidget() override = default;

   QString GetCategoryHint() const override { return "Map"; }

private:
   QColor GetColor(QTreeWidgetItem* aItem) const;
   void   SetColor(QTreeWidgetItem* aItem, const QColor& aColor);

   void ReadPreferenceData(const InteractionPrefData& aPrefData) override;
   void WritePreferenceData(InteractionPrefData& aPrefData) override;

   Ui::WkfInteractionPrefWidget        mUi;
   std::map<QString, QTreeWidgetItem*> mItemMap;
   QSpinBox*                           mTimeoutWidget;
   QCheckBox*                          mStackingAllowedWidget;
   QComboBox*                          mTooltipWidget;
   QSpinBox*                           mLineWidthWidget;
};
} // namespace wkf
#endif // WKFINTERACTIONPREFWIDGET_HPP
