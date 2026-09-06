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
#ifndef WKFEVENTMARKERPREFWIDGET_HPP
#define WKFEVENTMARKERPREFWIDGET_HPP

#include <QSlider>
#include <QSpinBox>

#include "WkfEventMarkerPrefObject.hpp"
#include "WkfPrefWidget.hpp"
#include "selectors/WkfImageSelectorWidget.hpp"

class QTreeWidget;
class QTreeWidgetItem;

namespace wkf
{
class EventMarkerPrefWidget : public wkf::PrefWidgetT<EventMarkerPrefObject>
{
public:
   EventMarkerPrefWidget();
   ~EventMarkerPrefWidget() override = default;

   QString GetCategoryHint() const override { return "Map"; }

   void RegisterEvent(const QString& aName, const QColor& aColor);

private:
   void       PopulateOptions(QTreeWidgetItem* aItem);
   QColor     GetColor(QTreeWidgetItem* aItem) const;
   void       SetColor(QTreeWidgetItem* aItem, const QColor& aColor);
   MarkerType GetRadioButtons(const QString& aParentName);
   void       SetRadioButton(const QString& aParentName, const MarkerType& aMarkerType);
   QString    GetIconPath(const QString& aParentName, bool& aOk);
   void       SetIconPath(const QString& aParentName, const QString& aIconPath);

   void ReadPreferenceData(const EventMarkerPrefData& aPrefData) override;
   void WritePreferenceData(EventMarkerPrefData& aPrefData) override;

   QSpinBox*                           mTimeout;
   QSlider*                            mMarkerScale;
   QTreeWidget*                        mTreeWidget;
   std::map<QString, QTreeWidgetItem*> mItemMap;
};
} // namespace wkf
#endif // WKFEVENTMARKERPREFWIDGET_HPP
