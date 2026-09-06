// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef AERODYNAMICS_WIDGET_HPP
#define AERODYNAMICS_WIDGET_HPP

#include <memory>
#include <QWidget>

#include "MoverCreatorTabs.hpp"

namespace Ui
{
   class AerodynamicsWidget;
}

class QTreeWidgetItem;
class QTreeWidget;

namespace Designer
{
   class VehicleAeroCore;
   class VehicleAeroMovable;
   class AeroTable2d;
   class AeroTable3d;
   class AeroTable4d;
   class AeroTableBase;
   class PlotDataSet;
   class AerodynamicsWidget : public QWidget
   {
         Q_OBJECT
      public:
         explicit AerodynamicsWidget(QWidget*        aParent,
                                     Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~AerodynamicsWidget();

         void Initialize();
         void showEvent(QShowEvent* aEvent);

         // This should be called when transitioning from the current tab to the "moving to" tab
         void TabTransition(Designer::eMoverCreatorTab aMovingToTab);

         void Reset() { }

      public slots:
         void SetPlotVisibility(QTreeWidgetItem* aItem, int aColumn);
         void SetPlotSelected(QTreeWidgetItem* aItem, int aColumn);
         void PopulateTableSelectionList();
         void PopulateGMProperties();
         void NextButtonClicked();
         void PreviousButtonClicked();
         void HandleGlobalOffsetClicked();
         void HandleGlobalMultiplierClicked();
         void ToggleModifierLine();
         void ToggleModifierType();
         void HandleSelectNoneClicked();
         void HandleSelectAllClicked();
         void HandleRefreshTrimClicked();
         void HandleRevertClicked();
         void SaveAero();
         void HandleRefreshPlotClicked();
         void HandleMoverChanged(const QString& aMoverText);
         void CLMaxChanged();

      private:
         Ui::AerodynamicsWidget* mUIPtr;

         void BuildMoverList();
         void BuildTableList();
         void BuildPlotList();
         void BuildComponentList();
         void ClearTables();
         void SortTree(QTreeWidget* aTreeWidget, QString aSuffix);
         void SortTree(QTreeWidgetItem* aTreeWidget, QString aSuffix);
         void ClearGlobalModifierLineEdits();

         void LoadSelectedCoreAeroTable();
         void LoadSelectedMovableAeroTable();
         void Load4dTable(AeroTable4d* aTablePtr);
         void Load3dTable(AeroTable3d* aTablePtr);
         void Load2dTable(AeroTable2d* aTablePtr);

         std::map<QTreeWidgetItem*, std::unique_ptr<PlotDataSet>> mPlotMap;
         bool mAeroPopulated = false;

         AeroTableBase* mCurrentTablePtr = nullptr;

         std::shared_ptr<VehicleAeroCore> mVehicleAeroCore = nullptr;

   };
}
#endif // !AERODYNAMICS_WIDGET_HPP
