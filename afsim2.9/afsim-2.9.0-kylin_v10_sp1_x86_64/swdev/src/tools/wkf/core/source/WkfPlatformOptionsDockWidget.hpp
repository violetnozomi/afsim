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
#ifndef WKFPLATFORMOPTIONSDOCKWIDGET_HPP
#define WKFPLATFORMOPTIONSDOCKWIDGET_HPP

#include <set>

#include <QTreeWidgetItem>

#include "WkfPlatformContextDockWidget.hpp"
#include "WkfPlatformGrouping.hpp"

namespace wkf
{
class PlatformOptionsDockWidget : public PlatformContextDockWidget
{
public:
   PlatformOptionsDockWidget(QWidget* aParent = nullptr, Qt::WindowFlags aFlags = Qt::WindowFlags());

protected:
   bool SetCurrentPlatform(const Platform& aPlatform) override;
   void ClearCurrent(bool aExplicit) override;
   void ItemChanged(const ChangeInfo& aInfo) override;
   void GroupIndexChanged(int aIndex) override;

   void UpdateCheckStates();
   void UpdateCheckStates(QTreeWidgetItem* aItem);

   using PlatformSet = std::set<Platform*>;
   void GetPlatformSet(PlatformSet& aSet, PlatformGrouping aGrouping);
   void PlatformConfigured(Platform* aPlatform);
   void PlatformDeleted(Platform* aPlatform);

   void UpdateDisplay();
   void NewGrouping(int aType, const QString& aName, int aPlatformIndex);
   void GroupingChanged(int aType, const QString& aName, int aPlatformIndex);

private:
   void PlatformConfigured(Platform* aPlatform, QTreeWidgetItem* aItemPtr);
   void PlatformDeleted(Platform* aPlatform, QTreeWidgetItem* aItemPtr);

   //! rebuilds the internal grouping set maintained by this widget
   //! @param aPlatformName is the name of the platform for which the groups are shown for
   //! @param aPlatformIndex is the index of the platform for which the groups are shown for
   //! @param aSelectPlatform overrides the select the previous entry behavior and instead will select the specified platform
   void RebuildGroupingSet(const std::string& aPlatformName, int aPlatformIndex, bool aSelectPlatform);
   //! rebuilds the group selection combo box
   //! @param aGroupText the name of the group that will be selected after rebuilding the groupings
   void RebuildGroupComboBox(const QString& aGroupText);

   int mPlatformIndex;

   std::set<PlatformGrouping> mGroupingSet;
   PlatformGrouping           mGrouping;
   vespa::VaCallbackHolder    mCallbacks;
};
} // namespace wkf
#endif
