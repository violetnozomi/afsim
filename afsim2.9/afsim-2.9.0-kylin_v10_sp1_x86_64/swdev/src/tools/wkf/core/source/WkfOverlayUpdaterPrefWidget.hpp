// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WKFOVERLAYUPDATERPREFWIDGET_HPP
#define WKFOVERLAYUPDATERPREFWIDGET_HPP

#include <set>

#include <QListWidget>
#include <QToolButton>

#include "WkfOverlayUpdaterPrefObject.hpp"
#include "WkfPrefWidget.hpp"

namespace wkf
{
class OverlayUpdaterPrefWidget : public PrefWidgetT<OverlayUpdaterPrefObject>
{
   Q_OBJECT

public:
   OverlayUpdaterPrefWidget(QWidget* aParent = nullptr);

   QString GetCategoryHint() const override { return "Map"; }

private:
   void ReadPreferenceData(const OverlayUpdaterPrefData& aPrefData) override;
   void WritePreferenceData(OverlayUpdaterPrefData& aPrefData) override;
   void SelectionChanged();
   void DeleteItem();
   void SetColor();
   void SetStyle();
   void ListContextMenu(const QPoint& aPosition);

   QListWidget*      mListWidget;
   QToolButton*      mEditButton;
   QPushButton*      mDeleteButton;
   QMenu*            mMenu;
   std::set<QString> mFontSet;
   std::set<QString> mColorSet;
};
} // namespace wkf
#endif
