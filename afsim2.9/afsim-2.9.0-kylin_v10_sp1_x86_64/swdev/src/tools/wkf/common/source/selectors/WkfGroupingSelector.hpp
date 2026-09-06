// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKFGROUPINGSELECTOR
#define WKFGROUPINGSELECTOR

#include "wkf_common_export.h"

#include <QDialog>
#include <QToolButton>

#include "WkfPlatformGrouping.hpp"
#include "ui_WkfGroupingSelectorDialog.h"

class QTreeWidgetItem;

namespace wkf
{
class WKF_COMMON_EXPORT GroupingSelector : public QToolButton
{
   Q_OBJECT
public:
   GroupingSelector(QWidget* aParent);
   ~GroupingSelector() override = default;

   const wkf::PlatformGrouping& GetGrouping() const { return mGrouping; }

signals:
   void GroupingSelected(const wkf::PlatformGrouping);

private:
   wkf::PlatformGrouping mGrouping;
   void                  Activated();
};

class GroupingSelectorDialog : public QDialog
{
public:
   GroupingSelectorDialog(bool aShowIndividuals, bool aShowGroups, QWidget* aParent);
   ~GroupingSelectorDialog() override = default;

   const wkf::PlatformGrouping& GetGrouping() const { return mGrouping; }

private:
   void ItemClicked(QTreeWidgetItem* aItemPtr, int aColumn);
   void FilterChanged(const QString& aFilter);

   wkf::PlatformGrouping        mGrouping;
   Ui_WkfGroupingSelectorDialog mUi;
};
} // namespace wkf
#endif
