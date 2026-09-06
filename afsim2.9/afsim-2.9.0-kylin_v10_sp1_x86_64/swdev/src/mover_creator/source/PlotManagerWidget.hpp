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

#ifndef PLOT_MANAGER_WIDGET_HPP
#define PLOT_MANAGER_WIDGET_HPP

#include <QJsonObject>
#include <QTreeWidget>
#include <QWidget>
#include <QMenu>
#include <memory>
#include "PlotDataSet.hpp"
#include "KnotAdderDialog.hpp"
#include "KnotEditorDialog.hpp"

namespace Ui
{
   class PlotManagerWidget;
}

namespace Designer
{
   class VehicleAeroCore;
   class VehicleAeroMovable;
   class AeroTableBase;
   // PlotManagerWidget is a wrapper to simplify use
   // and management of all plot interactions. Pointers to PlotDataSet objects are passed in but
   // this manager does NOT take ownership, it just uses the pointer to handle the
   // data modifications directly. Any data clean up is the responsibility of the
   // object that stores the data.
   class PlotManagerWidget : public QWidget
   {
         Q_OBJECT
      public:
         explicit PlotManagerWidget(QWidget*        aParent,
                                    Qt::WindowFlags aFlags = Qt::WindowFlags());
         ~PlotManagerWidget() override;

         void AddPlot(PlotDataSet* aPlotDataSetPtr, double aSecondaryIndependentVal = -1, double aTertiaryIndependentVal = -1, bool aAddToPlot = true);
         void RemovePlot(PlotDataSet* aPlotDataSetPtr);
         void ClearPlots();
         void ClearAllPlots();
         void SetPlotVisible(PlotDataSet* aPlotDataSetPtr, bool aVisible, QTreeWidgetItem* aItem = nullptr);
         void SetSelectedPlot(PlotDataSet* aPlotDataSetPtr);
         void DeselectAllPlots();
         void SetTitle(QString aTitle);
         void SetLabelXAxis(QString aLabel);
         void SetLabelYAxis(QString aLabel);
         void SetComponentName(QString aName);

         void SetXAxisBounds(double aXMin, double aXMax);
         void SetYAxisBounds(double aYMin, double aYMax);

         void AddModifierSeries(bool aVisible);
         void SetModifierSeriesSelected();
         void SetGlobalOffset(double aOffset);
         void SetGlobalMultiplier(double aMultiplier);
         void SetActiveTable(AeroTableBase* aTable);
         void SetPlotColors();
         void SetPlotBounds(double aXmin, double aXmax, double aYmin, double aYmax);

         void SetSeriesColor(unsigned int aSeriesNum, QColor aColor);
         void RefreshPlot();
         void RevertPlot();
         enum ModifierType
         {
            cOFFSET,
            cMULTIPLY
         };
         void SetModifierObject(const QJsonObject aObj) { mAeroModificationObj = aObj; }
         void SetModifierLine(const std::vector<std::pair<double, double>>& aLine);
         void SetModifierLineType(ModifierType aModifierType);
         void SetModifierLineVisible(bool aVisible);
         // Set a point at the given index to be unaffected by the modifier line
         void SetPointUnmodifiable(unsigned int aIndex);

         const QJsonObject& GetModifierObject() { return mAeroModificationObj; }
         std::vector<std::pair<double, double>> GetModifierPoints() { return mModifierPoints; }
         QString GetDependentVarName();
         void SetModifierYAxisBound(double aMin, double aMax);

         void mouseReleaseEvent(QMouseEvent* event);

      signals:
         void ModifierChanged();


      private slots:
         void HandleAddPointCommand(double aXCoord);
         void HandleDataPointSelected(unsigned int aSeriesNum, unsigned int aIndex);
         void HandlePlotManipulation(unsigned int aSeriesNum, unsigned int aIndex);
         void ShowContextMenu(const QPoint& aPoint);
         void ShowEditorDialog();
         void ShowKnotAdderDialog();

      private:
         void SyncPlot(unsigned int aPlotSeriesNum);
         void SyncModifierPlot(unsigned int aPlotSeriesNum);
         void SyncModifierLine(unsigned int aIndex);
         void ApplyModifier();
         void UpdateZeroLines();

         std::map<int, PlotDataSet*>                        mPlotDataSetMap;
         std::map<int, std::unique_ptr<PlotDataSet>>        mModifierPlotDataSetMap;
         std::map<PlotDataSet*, std::pair<double, double>>  mPlotDataSetValueMap;
         std::vector<std::pair<double, double>>             mModifierPoints;
         int                                                mNextMapKey;
         int                                                mModifierPlotIndex = 2;
         KnotEditorDialog*                                  mKnotEditorDialog;
         KnotAdderDialog*                                   mKnotAdderDialog;
         QMenu*                                             mContextMenu;

         double                                             mModifierYAxisMin = -2.5;
         double                                             mModifierYAxisMax = 3.5;

         AeroTableBase*                                     mCurrentTablePtr = nullptr;
         ModifierType                                       mModifierType = cMULTIPLY;
         QString                                            mXAxisLabel = "";
         QString                                            mTitle = "";
         QString                                            mComponentName = "";
         std::vector<QColor>                                mPlotColorMap;

         // A list of points that will not be affected by the modifier line or any global modifiers
         std::vector<unsigned int>                          mUnmodifiablePointList;

         QJsonObject                                        mAeroModificationObj;
         bool                                               mPlotManipulationDirty = false;

         Ui::PlotManagerWidget*                             mUIPtr;
   };
}
#endif // !PLOT_MANAGER_WIDGET_HPP
