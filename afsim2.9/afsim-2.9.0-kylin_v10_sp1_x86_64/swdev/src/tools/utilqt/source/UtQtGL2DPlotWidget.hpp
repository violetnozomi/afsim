// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2014 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UtQtGL2DPlotWidget_hpp
#define UtQtGL2DPlotWidget_hpp

#include "utilqt_export.h"

#include <QColor>
#include <QMenu>
#include <QPalette>
#include <QPoint>
#include <QScrollArea>
#include <QString>
#include <QTreeWidget>

#include "UtQtGL2DPlot.hpp"
#include "UtQtGLWidgetBase.hpp"

// ************************************************************************************ //
// UtQtGL2DPlotWidget provides basic graphing/charting ability through the use of OpenGL//
// ************************************************************************************ //

class UTILQT_EXPORT UtQtGL2DPlotWidget : public UtQtGLWidgetBase
{
   Q_OBJECT
   // Property macros to support CSS settings of colors
   Q_PROPERTY(QColor gridColor READ GetGridColor WRITE SetGridColor)
   Q_PROPERTY(QColor borderColor READ GetBorderColor WRITE SetBorderColor)
   Q_PROPERTY(QColor textColor READ GetTextColor WRITE SetTextColor)
   Q_PROPERTY(QColor tickColor READ GetTickMarkColor WRITE SetTickMarkColor)
   Q_PROPERTY(QColor plotBgColor READ GetPlotBgColor WRITE SetPlotBgColor)
   Q_PROPERTY(QColor plotLegendColor READ GetPlotLegendColor WRITE SetPlotLegendColor)

public:
   using DataPt     = UtQtGL2DPlot::DataPt;
   using DataPtList = UtQtGL2DPlot::DataPtList;

   UtQtGL2DPlotWidget(QWidget* aParent, UtQtGL2DPlot* aPlot = nullptr);
   ~UtQtGL2DPlotWidget() override;

   UtQtGL2DPlot* GetPlot() { return mPlot; }

   // ************************************************************************************ //
   // These color functions must be present in UtQtGL2DPlotWidget to support setting of
   //  colors via style sheets
   // ************************************************************************************ //
   // Sets the color to use for all text
   virtual void SetTextColor(QColor aColor) { mPlot->SetTextColor(aColor); }

   // Gets the color to use for all text
   virtual QColor GetTextColor() const { return mPlot->GetTextColor(); }

   // Sets the color used to draw the border
   void SetBorderColor(QColor aColor) { mPlot->SetBorderColor(aColor); }

   // Gets the color used to draw the border lines
   virtual QColor GetBorderColor() const { return mPlot->GetBorderColor(); }

   // Sets the color used to draw the grid lines
   void SetGridColor(QColor aColor) { mPlot->SetGridColor(aColor); }

   // Gets the color used to draw the grid lines
   QColor GetGridColor() const { return mPlot->GetGridColor(); }

   // Sets the color used to draw tick marks
   void SetTickMarkColor(QColor aColor) { mPlot->SetTickMarkColor(aColor); }

   // Returns the color used to draw tick marks
   QColor GetTickMarkColor() const { return mPlot->GetTickMarkColor(); }

   // Sets the color used to draw the background of the plot
   void SetPlotBgColor(QColor aColor) { mPlot->SetPlotBackgroundColor(aColor); }

   // Returns the color used to draw the background of the plot
   QColor GetPlotBgColor() { return mPlot->GetPlotBackgroundColor(); }

   // Sets the color used to draw the background of the legend
   void SetPlotLegendColor(QColor aColor) { mPlot->SetLegendBackgroundColor(aColor); }

   // Returns the color used to draw the background of the legend
   QColor GetPlotLegendColor() { return mPlot->GetLegendBackgroundColor(); }

   // ************************************************************************************ //
   // Zoom Functionality
   // ************************************************************************************ //

   // Enable the ability to zoom using the mouse
   virtual void EnableMouseZoom(bool aEnabled) { mEnableMouseZoom = aEnabled; }

   // Enter a zoomed state and set the axis boundaries.
   void ZoomIn(double aMinX, double aMaxX, double aMinY, double aMaxY, double aMinY2, double aMaxY2);

   // If currently zoomed, revert to an non-zoomed state.
   virtual void EndZoom();

   // Returns if the plot is currently zoomed
   virtual bool IsZoomed() const { return mZoomed; }

   // ************************************************************************************ //
   //  Functions relating to editing the data via mouse interactions
   // ************************************************************************************ //
   // Allow editing of data via interactions through the keyboard and mouse
   virtual void SetEnableEditMode(bool aInput);

   // Returns true if editing of data via interactions through the keyboard and mouse is enabled
   virtual bool GetEnableEditMode() const { return mEditModeEnabled; }

   // Sets whether or not the user is able to change the X value when edit mode is enabled
   virtual void SetEditModeLockX(bool aInput) { mEditModeLockX = aInput; }

   // Sets whether or not the user is able to change the Y value when edit mode is enabled
   virtual void SetEditModeLockY(bool aInput) { mEditModeLockY = aInput; }

   // Gets whether or not the user is able to change the X value when edit mode is enabled
   virtual bool GetEditModeLockX(bool aInput) const { return mEditModeLockX; }

   // Gets whether or not the user is able to change the Y value when edit mode is enabled
   virtual bool GetEditModeLockY(bool aInput) const { return mEditModeLockY; }

   // Set the minimum change increment for the X value when using Edit Mode to change the value of data points
   virtual void SetEditModeChangeIncrementX(double aIncrement) { mMinIncrementX = aIncrement; }

   // Set the minimum change increment for the Y value when using Edit Mode to change the value of data points
   virtual void SetEditModeChangeIncrementY(double aIncrement) { mMinIncrementY = aIncrement; }

   // Set the minimum change increment for the Y value for a series using the secondary Y axis
   //  when using Edit Mode to change the value of data points
   virtual void SetEditModeChangeIncrementY2(double aIncrement) { mMinIncrementY2 = aIncrement; }

   // Returns the minimum change increment for the X value when using Edit Mode to change the value of data points
   virtual double GetEditModeChangeIncrementX() const { return mMinIncrementX; }

   // Returns the minimum change increment for the Y value when using Edit Mode to change the value of data points
   virtual double GetEditModeChangeIncrementY() const { return mMinIncrementY; }

   // Set the minimum change increment for the Y value for a series using the secondary Y axis
   //  when using Edit Mode to change the value of data points
   virtual double GetEditModeChangeIncrementY2() const { return mMinIncrementY2; }

   // ************************************************************************************ //
   //  Context Menu Functions
   // ************************************************************************************ //
   // Set the context menu to the default context menu for the plot widget, this will remove a custom context menu if
   // aEnable is true.
   virtual void SetEnableDefaultContextMenu(bool aEnable);

   // Return a list of QActions for the default context menu.
   // This will return the actions regardless of whether the default context menu is enabled or not.
   virtual QList<QAction*> GetDefaultContextMenuActions();

   // ************************************************************************************ //
   //  Boundary Tabs
   // ************************************************************************************ //
   // Enable the display of Boundary Tabs
   virtual void SetEnableBoundaryTabs(bool aEnable) { mBoundaryTabsEnabled = aEnable; }

   // Sets whether Boundary Tabs can be controlled by the mouse or not.
   virtual void SetBoundaryTabsMouseEnabled(bool aEnable) { mBoundaryTabsMouseEnabled = aEnable; }

   // Function to get/set the Boundary Tab value for tab 1 and tab2
   virtual void   SetBoundaryTab1(double aValue);
   virtual void   SetBoundaryTab2(double aValue);
   virtual double GetBoundaryTab1() { return mBoundaryTabValue1; }
   virtual double GetBoundaryTab2() { return mBoundaryTabValue2; }

   // When using the mouse to control the Boundary Tab position, have the Boundary Tab jump to the nearest tick mark.
   //  This prevent moving the Boundary Tabs to x-values that do not have a tick mark at that value.
   virtual void SetBoundaryTabLockedToTickmarks(bool aEnable) { mBoundaryTabLockedToTickmarks = aEnable; }

   // Toggles whether the labels for where the Boundary Tabs intersect a series should be basic or detailed descriptions.
   virtual void SetBoundaryTabDetailedLabels(bool aEnable) { mBoundaryTabDetailedLabels = aEnable; }

   // ************************************************************************************ //
   //  Other Functions
   // ************************************************************************************ //
   // Enables cross-hairs and ties the location of the cross-hairs to the mouse location
   virtual void SetEnableCrosshairMouseControlled(bool aEnabled);

   // Toggle the dispay of the focus Indications (Dark triangle at each corner of the widget when the widget hasFocus() == true)
   virtual void SetShowFocusIndications(bool aEnabled) { mDrawFocusIndicators = aEnabled; }

   // Sets whether the data should be displayed in tabular form instead of on a plot.
   virtual void SetShowDataView(bool aShow);

   // Updates the data displayed in tabular form.  The table data does not automatically
   //  update when new data points are added to the plot.
   virtual void UpdateDataTable();

   // Sets whether the plot should use the Application QPalette to get background colors for the plot
   // This will set things like text color, background, plot background, and legend background using colors from the
   // qApp->palette() This defaults to true, and the plot will initialize with colors from the palette regardless of
   // this settings. This settings indicates whether to change the plots colors when the QPalette changes.  Disable this
   // if you
   //  are specifically settings colors for the plot, and do not want the colors to change when the QPalette changes.
   virtual void SetEnableApplicationPalette(bool aEnabled) { mUseApplicationPalette = aEnabled; }

   QPointF ConvertPositionToData(const QPoint& aPoint) const;

signals:
   // emitted when the position of a boundary tabs are changed
   void BoundaryTabPositionChanged();

protected:
   void PaletteChangedCB(const QPalette& aPalette);

   void PlotBoundariesChangedCB();
   void HandleSeriesSelected(unsigned int aSeriesNum);

   // Context Menu Actions
   void LegendAction();
   void DataViewAction();
   void ExportDataAction();
   void ChangeSeriesColorsAction();

   bool GetDataPoint(unsigned int aSeriesNum, unsigned int index, std::pair<double, double>& aPoint);

   // Deletes the context menu, thus removing any context menu so none will appear
   void DeleteContextMenu();

   void ApplyPalette(const QPalette& aPalette);

   // Functions to draw the display of the zoom box when using mouse zoom
   virtual void DrawZoomBox();
   virtual void DrawBoundaryTabs();
   virtual void DrawBoundaryTabsText(QPainter* aPainter);
   virtual void DrawFocusIndicators();
   // Function to Store value prior to Zooming
   virtual void StorePreZoomValues();

   void PopulateContextMenuActions();
   void PopulateDataTable();

   void ZoomIn();

   // Functions to support editing data values via the plot
   enum pointPlacement
   {
      ePOINT_FIRST,
      ePOINT_MIDDLE,
      ePOINT_LAST
   };
   virtual void AddDataPoint(pointPlacement aPlacement);
   virtual void RemoveSelectedDataPoint();
   virtual void ChangeSelectedSeries();
   void         keyPressEvent(QKeyEvent* event) override;
   void         contextMenuEvent(QContextMenuEvent* event) override;

   // Event handlers to support mouse actions.
   void mousePressEvent(QMouseEvent* event) override;
   void mouseReleaseEvent(QMouseEvent* event) override;
   void mouseMoveEvent(QMouseEvent* event) override;
   void wheelEvent(QWheelEvent* event) override;

   virtual bool BoundaryTabSelected(QMouseEvent* event);
   virtual bool DataPointSelected(QMouseEvent* event);
   virtual void ZoomEvent(QMouseEvent* event);

   // Pure Virtual function required by base class
   void Draw() override;
   void QtDraw() override;
   void initializeGL() override;
   void resizeGL(int aWidth, int aHeight) override;

   // Creates colorbuttons for each visible series in a plot and sets the button to the series color.
   // Also adds the ability to change the color of said series.
   void SetColor(int seriesNum, QTreeWidgetItem* aItem, const QColor& aColor) const;

   bool mPreventUpdates;
   // Enable zooming with mouse
   bool mEnableMouseZoom;
   // Values to support Zooming:
   QPoint mMousePressLocation;
   QPoint mMouseCurrentLocation;
   bool   mButtonDownZooming;
   bool   mButtonDownLegend; // To support moving legend
   int    mLegendSelectionXOffset;
   int    mLegendSelectionYOffset;

   bool   mZoomed;
   bool   mPreZoomAutoUpdatePlot;
   bool   mPreZoomUseTickIncrementX;
   bool   mPreZoomUseTickIncrementY;
   bool   mPreZoomUseTickIncrementY2;
   int    mPreZoomTickCountX;
   int    mPreZoomTickCountY;
   int    mPreZoomTickCountY2;
   double mPreZoomTickIncrementX;
   double mPreZoomTickIncrementY;
   double mPreZoomTickIncrementY2;
   double mPreZoomMaxXBoundary;
   double mPreZoomMinXBoundary;
   double mPreZoomMaxYBoundary;
   double mPreZoomMinYBoundary;
   double mPreZoomMaxYBoundary2;
   double mPreZoomMinYBoundary2;

   bool      mBoundaryTabsEnabled;
   bool      mBoundaryTabsMouseEnabled;
   double    mBoundaryTabValue1;
   double    mBoundaryTabValue2;
   bool      mBoundaryTab1Selected;
   bool      mBoundaryTab2Selected;
   bool      mBoundaryTabLockedToTickmarks;
   bool      mBoundaryTabDetailedLabels;
   QPolygonF mBoundaryTab1Points;
   QPolygonF mBoundaryTab2Points;

   bool   mMouseCrosshairsEnabled;
   bool   mEditModeEnabled;
   bool   mEditModeLockX;
   bool   mEditModeLockY;
   double mMinIncrementX;
   double mMinIncrementY;
   double mMinIncrementY2;
   int    mCurrentSelectedSeries;
   bool   mDataPointSelected;
   int    mDataPointIndex;
   bool   mDrawFocusIndicators;

   bool mUseApplicationPalette;

   QMenu* mContextMenu;

   bool         mDataView;
   QWidget*     mDataWidget;
   QScrollArea* mDataScrollArea;
   QTreeWidget* mSeriesColorsList;

   UtQtGL2DPlot* mPlot;
};

#include "UtQtGL2DPlotTP.hpp"
class UtQtGL2DPlotTPWidget : public UtQtGL2DPlotWidget
{
public:
   UtQtGL2DPlotTPWidget(QWidget* aParent)
      : UtQtGL2DPlotWidget(aParent, new UtQtGL2DPlotTP)
   {
   }
};


#endif
