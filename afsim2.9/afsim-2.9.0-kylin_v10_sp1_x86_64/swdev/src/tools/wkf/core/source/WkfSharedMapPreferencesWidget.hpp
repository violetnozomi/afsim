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
#ifndef WKFSHAREDMAPPREFERENCESWIDGET_HPP
#define WKFSHAREDMAPPREFERENCESWIDGET_HPP

#include <QTreeWidget>

#include "WkfPrefWidget.hpp"
#include "WkfSharedMapPreferencesObject.hpp"

namespace wkf
{
class SharedMapPreferencesWidget : public PrefWidgetT<SharedMapPreferencesObject>
{
   Q_OBJECT

public:
   SharedMapPreferencesWidget(QWidget* aParent = nullptr, Qt::WindowFlags aFlags = Qt::WindowFlags());

   QString GetCategoryHint() const override { return "Map"; }

private:
   enum UserRole
   {
      cMAP     = 1,
      cMAPFILE = 2
   };

   //! saves the maps
   void RestoreDefaults() override;

   void    ReadPreferenceData(const SharedMapPreferencesData& aPrefData) override;
   void    UpdateProfiles();
   void    WritePreferenceData(SharedMapPreferencesData& aPrefData) override;
   QString ProfileTypeLookup(SharedMapPreferencesData::MapType aType);
   void    ContextMenuEvent(const QPoint& aPoint);

   void    AddNewMap();
   void    RemoveMap(QTreeWidgetItem* aMapItem);
   void    RenameMap(QTreeWidgetItem* aMapName);
   QString GetUniqueName(const QString& aName);
   bool    NameIsUnique(const QString& aName);

   QTreeWidget* mMapTreePtr;
   QTreeWidget* mProfileTreePtr;
};
} // namespace wkf
#endif
