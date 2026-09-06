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

#ifndef WKFPLATFORMDATATREEITEM_HPP
#define WKFPLATFORMDATATREEITEM_HPP

#include "wkf_export.h"

#include <QTreeWidgetItem>

namespace wkf
{
class WKF_EXPORT PlatformDataTreeItem : public QTreeWidgetItem
{
public:
   PlatformDataTreeItem(QTreeWidgetItem* aParentPtr = nullptr)
      : QTreeWidgetItem(aParentPtr)
      , mHideCause(0)
   {
   }
   PlatformDataTreeItem(QTreeWidgetItem* aParentPtr, const QStringList& aText)
      : QTreeWidgetItem(aParentPtr, aText)
      , mHideCause(0)
   {
   }

   enum HideCause
   {
      PROGRAMMATIC = 1,
      PREFERENCE   = 2
   };
   void SetHidden(bool aState, HideCause aCause = PROGRAMMATIC);
   void SetUnhideable(bool aValue = true);
   bool PreferenceHideable() const;
   void SetDefaultHidden(bool aValue = true);
   bool DefaultHidden() const;
   void SetPlottable(bool aValue = true);
   bool Plottable() const;

private:
   unsigned int mHideCause;
   bool         mPlottable = false;
};
} // namespace wkf

#endif
