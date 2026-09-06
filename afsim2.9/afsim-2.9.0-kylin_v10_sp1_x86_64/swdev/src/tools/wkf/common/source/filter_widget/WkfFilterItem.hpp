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

#ifndef WKF_FILTER_ITEM_HPP
#define WKF_FILTER_ITEM_HPP

#include "wkf_common_export.h"

#include "WkfFilterObject.hpp"
#include "ui_WkfFilterItem.h"

namespace WkfFilter
{
//! A graphical representation of a FilterObject.
//! A FitlerView uses these to interact with the FilterObject they represent.
class WKF_COMMON_EXPORT FilterItem : public QWidget
{
   Q_OBJECT
public:
   explicit FilterItem(bool aIsRoot);

   //! No signals will be emitted.
   //! @{
   void SetType(FilterObject::Type aType);
   void SetEnabled(bool aEnable);
   void SetInverted(bool aInvert);

   void SetColumn(const QString& aName, int aIndex);
   void SetRulesList(const QStringList& aList);
   void SetRules(const QStringList& aList, const QString& aCurrentItem);

   void SetValue(const QString& aValue);

   FilterItem* MakeChild();
   void        RemoveChild(FilterItem* aChild);
   //! @}

signals:
   //! aItem will always be the calling object (this)
   //! @{
   void TypeChanged(FilterItem* aItem, FilterObject::Type aType);
   void EnableChanged(FilterItem* aItem, bool aEnable);
   void InvertChanged(FilterItem* aItem, bool aInvert);
   void ColumnChanged(FilterItem* aItem, const QString& aName);
   void TestChanged(FilterItem* aItem, const QString& aTest);
   void ValueChanged(FilterItem* aItem, const QString& aValue);
   void AddClicked(FilterItem* aItem);
   void RemoveClicked(FilterItem* aItem);

   void ContextMenu(FilterItem* aItem, const QPoint& aPosition);
   //! @}

private:
   void ConnectUi();
   void SetupSizePolicy();

   void GotoConditionView();
   void GotoGroupView();

   Ui::WkfFilterItem mUi;
};
} // namespace WkfFilter

#endif
