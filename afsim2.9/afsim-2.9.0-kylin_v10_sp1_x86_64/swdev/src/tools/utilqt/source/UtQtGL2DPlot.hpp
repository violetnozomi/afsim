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

#ifndef UtQtGL2DPlot_hpp
#define UtQtGL2DPlot_hpp

#include <map>
#include <vector>

#include <QColor>
#include <QObject>
#include <QString>
#include <qgl.h>

class QAbstractItemModel;
class QTextStream;

#include "utilqt_export.h"

class UtQtGLDPlot_PointAdder;
#include "UtVec3.hpp"

// ************************************************************************************ //
// UtQtGL2DPlot provides basic graphing/charting ability through the use of OpenGL      //
//                                                                                      //
// It is written support use within a QT Widget and to be usable with using a QWidget   //
//  such as a world view that is rendered using just openGL.                            //
// ************************************************************************************ //

class UTILQT_EXPORT UtQtGL2DPlot : public QObject
{
   Q_OBJECT
public:
   friend class UtQtGLDPlot_PointAdder;
   friend class UtQtGL2DPlotWidget;

   // Enumeration for Label Positions
   enum LabelPos
   {
      TITLE,
      TOP,
      BOTTOM,
      LEFT,
      RIGHT,
      NUM_LABELS // Must remain last
   };

   // Enumeration for controlling how to draw Tick Marks
   enum TickMarkPos
   {
      NONE,
      EXTERIOR,
      INTERIOR
   };

   // Enumeration for controlling the plot type
   enum PlotStyle
   {
      POINT,
      LINE,
      VERTICAL_BAR,
      HORIZONTAL_BAR,
      MIDPOINT_GRID
   };

   // Enumeration for controlling stipple patterns
   enum StipplePattern
   {
      NO_STIPPLE,
      DOTTED,
      DASHED
   };

   static unsigned int MakeRgba(int r, int g, int b, int a = 255) { return qRgba(r, g, b, a); }

   class DataPt
   {
   public:
      DataPt() = default;
      explicit DataPt(double aX, double aY = 0, double aZ = 0, unsigned int aRgba = 0)
         : x(aX)
         , y(aY)
         , z(aZ)
         , rgba(aRgba)
      {
      }

      bool IsReal() const;
      bool IsColored() const { return rgba != 0; }

      double       x{0};
      double       y{0};
      double       z{0};
      unsigned int rgba{0};
   };

   using DataPtList = std::vector<DataPt>;
   using ColorStops = std::map<double, QColor>;
   using Double2    = std::pair<double, double>;

   // class base of series to support both locally held points and model held points
   class SeriesBase
   {
   public:
      enum SeriesType
      {
         eUnknownSeries,
         eStandardSeries,
         eModelViewSeries
      };

      SeriesBase(int aSeriesNumber = 0, PlotStyle aPlotStyle = LINE, SeriesType aType = eUnknownSeries);
      virtual ~SeriesBase() = default;

      virtual QColor  GetColorAt(double aValue) { return mColor; }
      virtual void    ClearData() {}
      virtual void    ClearColorData() {}
      virtual bool    IsEmpty() const { return true; }
      virtual size_t  Size() const { return 0; }
      virtual void    WriteCSV_X(QTextStream& aOut) const {}
      virtual void    WriteCSV_Y(QTextStream& aOut) const {}
      virtual void    GetBounds(double& aMinX, double& aMaxX, double& aMinY, double& aMaxY) const {}
      virtual void    Get(size_t aIndex, double& aX, double& aY) const {}
      virtual void    GetRGBA(size_t aIndex, unsigned int& aRGBA) const {}
      virtual void    GetZ(size_t aIndex, double& aZ) const {}
      virtual bool    HasZ() const { return false; }
      virtual QString Name() const { return mSeriesName; }
      virtual void    SetName(const QString& aName) { mSeriesName = aName; }

      int            mSeriesNumber{0};
      bool           mVisible{true};
      bool           mSeriesSelected{false};
      bool           mShowPoints{false};
      bool           mFill{false};
      bool           mIsSecondaryAxis{false};
      int            mSelectedDataPoint{-1};
      PlotStyle      mPlotStyle{LINE};
      float          mWidth{1.0f};
      StipplePattern mStipple{NO_STIPPLE};
      QColor         mColor;
      QColor         mFillColor;
      SeriesType     mType{eUnknownSeries};

   protected:
      QString mSeriesName;
   };

   // Class for storing the series data with series points held in local memory
   class Series : public SeriesBase
   {
   public:
      // Don't want default c-tor
      // so we can initialize things correctly
      Series(int aSeriesNumber, PlotStyle aPlotStyle);
      ~Series() override = default;

      void ClearData() override { data.clear(); }
      void ClearColorData() override { mColorPoints.clear(); }

      bool   IsEmpty() const override { return data.empty(); }
      size_t Size() const override { return data.size(); }
      void   WriteCSV_X(QTextStream& aOut) const override;
      void   WriteCSV_Y(QTextStream& aOut) const override;
      void   GetBounds(double& aMinX, double& aMaxX, double& aMinY, double& aMaxY) const override;
      void   Get(size_t aIndex, double& aX, double& aY) const override;
      void   GetRGBA(size_t aIndex, unsigned int& aRGBA) const override;
      void   GetZ(size_t aIndex, double& aZ) const override;
      bool   HasZ() const override { return !mColorPoints.empty(); }
      QColor GetColorAt(double aValue) override;

      DataPtList data;
      // Map< Z-Value, Color@Z-Value>  //Using map because they are sorted with unique keys
      ColorStops mColorPoints;
   };

   using SeriesMap = std::map<int, SeriesBase*>;

   UtQtGL2DPlot();
   ~UtQtGL2DPlot() override;

   // Call this after openGL has been initialized, otherwise text will not appear
   virtual void Initialize();

   // Sets the display width and height of the plot window
   virtual void SetWindowSize(int aWidth, int aHeight);

   // ************************************************************************************ //
   // Data Functions
   // ************************************************************************************ //
   // Clears data and resets the axis values to the defaults.
   virtual void Reset();

   // Clears all Data
   virtual void ClearData();

   // Return true if data points exist within UtQtGL2DPlotWidget
   virtual bool ContainsData() const { return !mData.empty(); }

   // Get the min and max X/Y values of the data within the graph.
   // These values may be smaller or larger than the boundary of the graph if the boundaries were specified explicitly
   // by using the set bounds functions.
   // returns false if no data exist.
   virtual bool GetDataMinMax(double& aMinX, double& aMinY, double& aMaxX, double& aMaxY) const;
   virtual bool GetDataMinMax(double& aMinX, double& aMinY1, double& aMinY2, double& aMaxX, double& aMaxY1, double& aMaxY2) const;

   // Add a point to the Graph, the display will update
   virtual bool AddPoint(double aX, double aY, unsigned int aSeriesNum = 1);
   bool         AddPoint(const DataPt& aPt, unsigned int aSeriesNum = 1);

   // Function on valid for Plot Style = POINT or LINE
   // Add a data point to the Graph with an additional z-value to be represented as Colors
   bool AddPointWithColor(double aX, double aY, double aZ, unsigned int aSeriesNum = 1);

   // Add a vector of points to the Graph, the display will update
   virtual bool AddPoints(const std::vector<std::pair<double, double>>& aPoints,
                          unsigned int                                  aSeriesNum = 1,
                          bool                                          aFlipAxis  = false);
   virtual bool AddPoints(const std::map<double, double>& aPoints, unsigned int aSeriesNum = 1, bool aFlipAxis = false);
   virtual bool AddPoints(const std::vector<DataPt>& aPoints, unsigned int aSeriesNum = 1);
   virtual bool AddPoints(const std::vector<double>& aX, const std::vector<double>& aY, unsigned int aSeriesNum = 1);

   // Function on valid for Plot Style = POINT or LINE
   // Add a vector of points to the Graph with an additional z-values to be represented as Colors
   bool AddPointsWithColor(const std::vector<std::pair<double, double>>& aPoints,
                           const std::vector<double>&                    aZValues,
                           unsigned int                                  aSeriesNum = 1);

   // Insert a new data point in the series identified by aSeriesNum at the index aIndex, with value aX, aY
   virtual bool InsertPoint(unsigned int aIndex, double aX, double aY, unsigned int aSeriesNum = 1);

   // Change the value of the data point in the series identified by aSeriesNum at the index aIndex to the value aX, aY
   virtual bool ChangePoint(unsigned int aIndex, double aX, double aY, unsigned int aSeriesNum = 1);

   // Remove the data point in the series identified by aSeriesNum at the index aIndex
   virtual bool RemovePoint(unsigned int aIndex, unsigned int aSeriesNum = 1);
   // virtual void RemoveSelectedSeries();
   // virtual void RemoveSelectedPoint();

   // Removes all data for a series
   virtual void RemoveSeries(int aSeriesNumber);

   // define a series that you want to pull model data from
   // setting a negative number in the colx or coly will pull data from the vertical headers
   void AddModelSeries(QAbstractItemModel* aModelPtr, int aColX, int aColY, unsigned int aSeriesNum = 1);

   // remove all of the model series from the plot
   void ClearModelSeries();

   size_t GetNumberOfSeries() { return mData.size(); }

   // Allows for setting the name of a series (Currently only used for display in the Legend in widget class)
   virtual void SetSeriesName(QString aName, unsigned int aSeriesNum = 1);

   // Gets the name for a given series.
   virtual QString GetSeriesName(unsigned int aSeriesNum = 1) const;

   // Return true if the series exist, and Gets the series number given the series name
   virtual bool GetSeriesNumberFromSeriesName(QString aName, unsigned int& aSeriesNum) const;

   // Function on valid for Plot Style = POINT or LINE
   // Add an inflection point for data point that have a color value (z-value) specified.
   //  aValue is the z-value coordinate and aColor or rgb specify the color to display at that point.
   //  Z-value coordinates not specified are extrapolated from colors that are provided.
   void AddColorInflectionPoint(double aValue, QColor aColor, unsigned int aSeriesNum = 1);
   void AddColorInflectionPoint(double aValue, int aR, int aG, int aB, unsigned int aSeriesNum = 1);

   bool AddGrid(const std::vector<double>&              aXValues,
                const std::vector<double>&              aYValues,
                const std::vector<std::vector<double>>& aDataValues,
                unsigned int                            aSeriesNum = 1);

   // Function on valid for Plot Style = POINT or LINE
   // Clears all Color data from the graph
   void ClearColorData();

   // Returns all the Data series
   const SeriesMap& GetData() const { return mData; }

   // Return data for the given series
   const SeriesBase* FindSeries(int aSeriesNumber) const { return FindSeriesP(aSeriesNumber); }

   // Retrieve the Y-values for a given series at the specified X-value.
   //  This function will linearly interpolate between values if y = f(x) is not a point in the data set.
   virtual void CalculateValueForSeries(unsigned int aSeriesNum, double aX, QList<double>& aY);

   // ************************************************************************************ //
   // Data Display Functions
   // ************************************************************************************ //
   // Draw the data after rendering the border, grid lines, origin, labels, and tick marks
   virtual void SetDrawDataOnTop(bool aInput) { mDrawDataLast = aInput; }

   // Graphs will be displayed with default colors assigned to each series instead of all black.
   //  This function will override colors selected by SetSeriesColor if SetSeriesColor was called prior to this
   //  function. SetSeriesColor may be called after default colors to change individual series.
   virtual void SetUseMultipleColorsForSeries(bool aInput);

   void SetUsePointColor(bool aUsePointColor) { mUsePointColor = aUsePointColor; }

   // Sets the width used to draw the specified series
   void SetSeriesWidth(float aWidth, int aSeriesNum = 1) { FindOrMakeSeries(aSeriesNum).mWidth = aWidth; }

   // Sets the color used to draw the specified series
   virtual void SetSeriesColor(QColor aColor, unsigned int aSeriesNum = 1);

   // Sets the stipple pattern used to draw the specified series
   void SetSeriesStipple(StipplePattern aStipple, int aSeriesNum = 1)
   {
      FindOrMakeSeries(aSeriesNum).mStipple = aStipple;
   }

   // Gets the width used to draw the specified series
   virtual float GetSeriesWidth(int aSeriesNum = 1) const;

   // Gets the color used to draw the specified series
   virtual QColor GetSeriesColor(int aSeriesNum = 1) const;

   // Gets the stipple pattern used to draw the specified series
   virtual StipplePattern GetSeriesStipple(int aSeriesNum = 1) const;

   // Sets whether to display the series against the secondary Y Axis instead of the primary Y axis
   virtual void SetSeriesUseSecondaryAxis(bool aUseSecondaryAxis, unsigned int aSeriesNum = 1);

   // Returns whether to display the series against the secondary Y Axis instead of the primary Y axis
   virtual bool GetSeriesUseSecondaryAxis(unsigned int aSeriesNum = 1) const;

   // Set the plot style for new data
   //  Note: This does not change the current display of existing series, for that use
   //        SetSeriesPlotStyle() instead.
   void SetDefaultPlotStyle(PlotStyle aStyle) { mDefaultPlotStyle = aStyle; }

   // Returns whether data should be shown as individual points or connected with a line
   PlotStyle GetDefaultPlotStyle() const { return mDefaultPlotStyle; }

   // Set the plotting style for a particular series.
   // Returns false is aSeriesNum is not found
   void SetSeriesPlotStyle(PlotStyle aStyle, unsigned int aSeriesNum = 1);

   // Set the plotting style for a particular series.
   // Returns false if a series with the name of aName is not found
   bool SetSeriesPlotStyle(QString aName, PlotStyle aStyle);

   // Return the plotting style for a particular series.
   PlotStyle GetSeriesPlotStyle(int aSeriesNum = 1) const;

   // Set a series as selected. Only one series can be selected at a time.
   // Currently this highlights the series
   void SetSeriesSelected(bool aSelected, unsigned int aSeriesNum = 1);

   // Returns which series is selected or -1 if none
   int GetSelectedSeries();

   // Removes the selected series from the plot
   void RemoveAllSeriesSelections();

   // Sets a data point as selected for a series.  Only one data point can be selected per series.
   // Currently this highlights the datapoint
   void SetDataPointSelected(bool aSelected, unsigned int aIndex, unsigned int aSeriesNum = 1);

   // Returns the selected data points index for the series
   int GetSelectedDataPoint(unsigned int aSeriesNum = 1);

   // Enable a data point label on the selected data points
   void SetShowLabelOnSelectedDataPoint(bool aShow) { mShowLabelOnSelectedDataPoint = aShow; }

   // Returns whether a point label is to be displayed on selected points.
   bool GetShowLabelOnSelectedDataPoint() { return mShowLabelOnSelectedDataPoint; }

   // Un-selects all data point selections for all series
   void RemoveAllSeriesDataPointSelections();

   // Sets whether or not to display the data point in addition to the line between the points
   void SetSeriesShowDataPoints(bool aInput, unsigned int aSeriesNum = 1);

   // Gets whether or not to display the data point in addition to the line between the points
   bool GetSeriesShowDataPoints(unsigned int aSeriesNum = 1) const;

   // Function on valid for Plot Style = LINE, VERTICAL_BAR or HORIZONTAL_BAR
   // Sets whether or not to use FillColor to fill in the Bars of the Bar graph
   void SetSeriesFill(bool aFill, unsigned int aSeriesNum = 1) { FindOrMakeSeries(aSeriesNum).mFill = aFill; }

   // Function on valid for Plot Style = LINE, VERTICAL_BAR or HORIZONTAL_BAR
   // Gets bool used to fill in the Bars of the Bar graph
   bool GetSeriesFill(unsigned int aSeriesNum = 1) const;

   // Function on valid for Plot Style = LINE, VERTICAL_BAR or HORIZONTAL_BAR
   // Set the color used to fill in the bars
   void SetSeriesFillColor(QColor aColor, unsigned int aSeriesNum = 1)
   {
      FindOrMakeSeries(aSeriesNum).mFillColor = aColor;
   }

   // Function on valid for Plot Style = LINE, VERTICAL_BAR or HORIZONTAL_BAR
   // Get the color used to fill in the bars of the specified series
   QColor GetSeriesFillColor(unsigned int aSeriesNum = 1) const;

   // Set whether all series should be visible or not
   void SetAllSeriesVisible(bool aVisible);

   // Sets whether a series should be visible or not
   void SetSeriesVisible(bool aVisible, unsigned int aSeriesNum = 1);

   // Returns whether a series is visible
   bool GetSeriesVisible(unsigned int aSeriesNum = 1) const;


   // ************************************************************************************ //
   // Data Export Functions
   // ************************************************************************************ //
   // Export the Data in the plot to a CSV file.  This will export all series data in the plot.
   // Returns true if the file was successfully created.
   bool WriteDataToFile(QString aFilename);

   // ************************************************************************************ //
   // Bar Graph Display Functions (PlotStyle == VERTICAL_BAR || HORIZONTAL_BAR)
   // ************************************************************************************ //
   // Function on valid for Plot Style = VERTICAL_BAR or HORIZONTAL_BAR
   // Sets the width of a Bar as a Percent of the total space on the grid.
   // Range for aPercentage is [0,100]
   void SetBarWidth(double aPercentage);

   // ************************************************************************************ //
   // Axis Display Functions
   // ************************************************************************************ //
   // Initializes the start boundaries for data to be supplied for the Graph.
   // The graph will automatically expand if data is outside this boundary.
   // This function can be used to reduce the overhead of constantly redrawing
   // the graph when expanding the plot size when new data is added.
   // It is not necessary to call, but recommended if working with large data sets.
   // Sets the X and Y1 axis bounds
   virtual void SetAxesBounds(double aMinX, double aMinY, double aMaxX, double aMaxY);
   // Sets the X, Y1, and Y2 axis bounds
   virtual void SetAxesBounds(double aMinX, double aMinY1, double aMinY2, double aMaxX, double aMaxY1, double aMaxY2);
   // Sets the X and Y2 axis bounds
   virtual void SetAxesBounds2(double aMinX, double aMinY, double aMaxX, double aMaxY);

   // Initializes the starting boundaries for the X axis
   virtual void SetXAxisBounds(double aMinX, double aMaxX);

   // Initializes the starting boundaries for the Y axis
   virtual void SetYAxisBounds(double aMinY, double aMaxY);

   // Initializes the starting boundaries for the secondary Y axis
   virtual void SetYAxisBounds2(double aMinY, double aMaxY);

   // Gets the min and max X axis values (regardless of whether graph is zoomed or not)
   virtual void GetXAxisBounds(double& aMinX, double& aMaxX) const;

   // Gets the min and max Y axis values (regardless of whether graph is zoomed or not)
   virtual void GetYAxisBounds(double& aMinY, double& aMaxY) const;

   // Gets the min and max values of the secondary Y axis (regardless of whether graph is zoomed or not)
   virtual void GetYAxisBounds2(double& aMinY, double& aMaxY) const;

   // Forces the secondary axis to be visible.
   // If false, the secondary axis will still be visible if any visible series use it.
   virtual void SetShowSecondaryAxis(bool aShow);

   // Returns whether the secondary Y Axis is bring forced to be displayed on the right side of the plot.
   virtual bool GetShowSecondaryAxis() const;

   // Returns whether the secondary Y Axis is being displayed on the right side of the plot
   virtual bool IsSecondaryAxisVisible() const;

   // ************************************************************************************ //
   // Cross-hairs Overlay Functions
   // ************************************************************************************ //
   // Enable Crosshairs to be displayed
   virtual void SetShowCrosshairs(bool aEnable);

   // Sets the position of the Crosshairs to a value on the X axis, if this value is not between
   //  the min and max X values then no crosshair will be displayed.
   virtual void SetCrosshairsValueX(double aXValue);

   // Sets the position of the Crosshairs as a percentage of the widget (not the plot region).
   //  This is useful for setting the position of the crosshairs via mouse actions.
   //  Valid range for aPercentage is [0,100]
   virtual void SetCrosshairsPosition(double aPercentage);

   // Sets whether or not the crosshair labels should be display for all series intersected by the crosshair.
   virtual void SetCrosshairsShowAllSeries(bool aShow) { mCrosshairShowAllSeries = aShow; }

   // Sets the specific series which the crosshair label should be displayed if it intersects the crosshair.
   virtual void SetCrosshairsShowSeries(int aSeriesNum);

   // ************************************************************************************ //
   // Overlay Functions
   // ************************************************************************************ //
   // draws an Overlay Line that will display over the graph using the form y= ax +b where a is aSlope and b is
   // aIntercept Return an int as an Identifier for this line. Allows for calling RemoveOverlayLine(id)
   virtual int AddOverlayLine(double         aSlope,
                              double         aIntercept,
                              QColor         aColor   = QColor(),
                              int            aWidth   = 1,
                              StipplePattern aStipple = NO_STIPPLE);

   // Add a horizontal line that will display over the graph.
   //  Return an int as an Identifier for this line. Allows for calling RemoveOverlayLine(id)
   virtual int AddHorizontalOverlayLine(double         aYIntercept,
                                        QColor         aColor   = QColor(),
                                        int            aWidth   = 1,
                                        StipplePattern aStipple = NO_STIPPLE);

   // Add a vertical line that will display over the graph
   //  Return an int as an Identifier for this line. Allows for calling RemoveOverlayLine(id)
   virtual int AddVerticalOverlayLine(double         aXIntercept,
                                      QColor         aColor   = QColor(),
                                      int            aWidth   = 1,
                                      StipplePattern aStipple = NO_STIPPLE);

   // Remove an OverlayLine with the specified Id;
   virtual void RemoveOverlayLine(int aId);

   // Remove all the overlay lines.
   virtual void RemoveAllOverlayLines();

   // Add overlay text to appear on the plot.
   //  aText is the string to be displayed
   //  aX and aY values control the position of the text, and are values between [0,1] to represent the % of the plot to
   //  display the text at.
   //   for example to display text in the middle of the screen use X = .5 and Y - .5
   //  aColor is the color to display the text in
   //  aDegrees is the rotation angle of the Text in degrees
   //  aSize is the point size of the text to be displayed
   //  aHAlign and aVAlign controls the alignment of the text relative to the point (aX, aY)
   // Return an identifier for the newly created text object
   virtual int AddOverlayText(QString       aText,
                              double        aX,
                              double        aY,
                              QColor        aColor   = QColor(),
                              double        aDegrees = 0,
                              int           aSize    = 8,
                              Qt::Alignment aHAlign  = Qt::AlignLeft | Qt::AlignBottom);

   // The following functions can be used to modify Overlay Text using the integer identifier returned by AddOverlayText()
   virtual bool ChangeOverlayTextString(int aId, QString aNewText);
   virtual bool ChangeOverlayTextPosition(int aId, double aX, double aY);
   virtual bool ChangeOverlayTextColor(int aId, QColor aColor);
   virtual bool ChangeOverlayTextRotation(int aId, double aDegrees);
   virtual bool ChangeOverlayTextSize(int aId, int aSize);

   // Remove an OverlayLine with the specified Id;
   virtual void RemoveOverlayText(int aId);

   // Remove all the overlay lines.
   virtual void RemoveAllOverlayText();

   // ************************************************************************************ //
   // Text Display Functions
   // ************************************************************************************ //
   // Set the Label text for the selected Label on the graph
   // If color is not specified, the default text color (as specified with SetTextColor()) will be used
   virtual void SetLabelText(LabelPos aLabel, QString aText);
   virtual void SetLabelText(LabelPos aLabel, QString aText, QColor aColor);

   // Returns the Label text for the selected Label on the graph
   virtual QString GetLabelText(LabelPos aLabel);

   // Sets the Label for the title of the graph
   // If color is not specified, the default text color (as specified with SetTextColor()) will be used
   // This is the same as SetLabelText(TITLE)
   virtual void SetTitle(QString aTitle);
   virtual void SetTitle(QString aTitle, QColor aColor);

   // Sets the Label for the X Axis of the graph
   // If color is not specified, the default text color (as specified with SetTextColor()) will be used
   // This is the same as SetLabelText(BOTTOM)
   virtual void SetLabelXAxis(QString aLabel);
   virtual void SetLabelXAxis(QString aLabel, QColor aColor);

   // Sets the Label for the Y Axis of the graph
   // If color is not specified, the default text color (as specified with SetTextColor()) will be used
   virtual void SetLabelYAxis(QString aLabel);
   virtual void SetLabelYAxis(QString aLabel, QColor aColor);

   // Sets the color to use for all text
   virtual void SetTextColor(QColor aColor) { mTextColor = aColor; }

   // Gets the color to use for all text
   virtual QColor GetTextColor() const { return mTextColor; }

   // Returns the color for the given label, if no color was specified for the label, the default text color is returned.
   virtual const QColor& GetLabelColor(LabelPos aLabel);

   // Sets the scale of the Font for the label text
   virtual void SetLabelFontSize(int aSize);

   // Gets the scale of the Font for the label text
   virtual int GetLabelFontSize() { return mLabelFontSize; }

   // ************************************************************************************ //
   // Legend Functions
   // ************************************************************************************ //
   // Sets whether to display a Legend of all the series
   virtual void SetShowLegend(bool aInput);

   // Get whether the Legend is to be displayed
   virtual bool GetShowLegend() const { return mShowLegend; }

   // Sets the position of the bottom left corner of the Legend as a percentage of the view
   //  0 to 1 is valid range for aX and aY
   virtual void SetLegendPosition(double aX, double aY);

   // Sets the scale of the Font for the legend text
   virtual void SetLegendFontSize(int aScale) { mLegendFontSize = aScale; }

   // Sets the scale of the Font for the legend text
   virtual double GetLegendFontSize() { return mLegendFontSize; }

   // Sets the background color for the legend.
   virtual void SetLegendBackgroundColor(QColor aColor) { mLegendBackgroundColor = aColor; }

   // Returns the background color for the legend.
   virtual QColor GetLegendBackgroundColor() const { return mLegendBackgroundColor; }

   // Return the bounding box for the legend.
   //  The bounding box for the Legend is return in Qt's Coordinate system where (0,0) is the
   //  topleft corner, NOT openGL where (0,0) is the bottomleft corner.
   virtual QRect GetLegendRect() const { return mLegendRect; }

   // ************************************************************************************ //
   // Plot Background Display Functions
   // ************************************************************************************ //
   // Sets the color of the background of the plot region.
   virtual void SetPlotBackgroundColor(QColor aColor);

   // Gets the color of the background of the plot region.
   //  Needed to support setting of Plot Background color via stylesheets.
   virtual QColor GetPlotBackgroundColor() { return mPlotBackgroundColor; }

   // Allows for setting color of certain point in the background of the plot region.
   //  This allows for gradients to be applied to the background of the plot.
   //  aX, and aY are valid in the range [-1,1] with (-1,-1) representing the bottom left corner of the plot
   //  This function uses GL_QUADS and points should be provided in groups of 4.
   virtual void AddPlotBackgroundPoint(double aX, double xY, QColor aColor);

   // Clears out the selection for the color due to SetPlotBackgroundColor, and
   // Clears out any points and colors added due to AddPlotBackgroundPoint(), causing the background
   //  of the plot region to be the same color as the background of the widget.
   virtual void ClearPlotBackground();

   // Set an image (in TGA format) to be the background of the plot.
   virtual void SetPlotBackgroundTGA(QString aFilename);

   // Remove the Current Background Image that was set using SetPlotBackgroundTGA()
   virtual void RemoveCurrentPlotBackgroundImage();

   // ************************************************************************************ //
   // Tick Mark Display Functions
   // ************************************************************************************ //
   // Sets how tick marks are to be displayed
   virtual void SetTickMarkDisplay(TickMarkPos aInput);

   // Sets whether or not to show the labels for the tick marks
   virtual void SetShowTickMarkLabels(bool aShow) { mTickMarkShowLabel = aShow; }

   // Sets the scale of the Font for the tick mark labels text
   virtual void SetTickMarkFontSize(double aScale) { mTickMarkFontSize = aScale; }

   // Sets the scale of the Font for the tick mark labels text
   virtual double GetTickMarkFontSize() { return mTickMarkFontSize; }

   // Sets the increment between tick marks on the X axis, this is mutually exclusive from SetNumberOfTicks()
   virtual void SetTickIncrementX(double aInput);

   // Sets the justification used for tick marks on the X axis
   virtual void SetTickJustifiedX(bool aJustified);

   // Get the value of the X axis tick increment.
   // Note: this is only valid after a SetTickIncrementX() call or a MakePlotPretty() call.
   virtual double GetTickIncrementX() const;

   // Sets the increment between tick marks on the Y axis, this is mutually exclusive from SetNumberOfTicks()
   virtual void SetTickIncrementY(double aInput);

   // Sets the increment between tick marks on the secondary Y axis, this is mutually exclusive from SetNumberOfTicks2()
   virtual void SetTickIncrementY2(double aInput);

   // Sets the justification used for tick marks on the Y axis
   virtual void SetTickJustifiedY(bool aJustified);

   // Sets the justification used for tick marks on the secondary Y axis
   virtual void SetTickJustifiedY2(bool aJustified);

   // Get the value of the Y axis tick increment.
   // Note: this is only valid after a SetTickIncrementY() call or a MakePlotPretty() call.
   virtual double GetTickIncrementY() const;

   // Get the value of the secondary Y axis tick increment.
   // Note: this is only valid after a SetTickIncrementY2() call or a MakePlotPretty() call.
   virtual double GetTickIncrementY2() const;

   // Sets number of tick marks for both X and Y axes
   virtual void SetTickCount(unsigned int aNumTicks);

   // Sets number of tick marks for the X axis
   virtual void SetTickCountX(unsigned int aNumTicks);

   // Get the number of ticks on the X axis.
   // Note: This call is invalidated by SetTickIncrementX() or MakePlotPretty() calls.
   virtual double GetTickCountX() const;

   // Sets number of tick marks for the Y axis
   virtual void SetTickCountY(unsigned int aNumTicks);

   // Sets number of tick marks for the secondary Y axis
   virtual void SetTickCountY2(unsigned int aNumTicks);

   // Get the number of ticks on the Y axis.
   // Note: This call is invalidated by SetTickIncrementY() or MakePlotPretty() calls.
   virtual double GetTickCountY() const;

   // Get the number of ticks on the secondary Y axis.
   // Note: This call is invalidated by SetTickIncrementY2() or MakePlotPretty() calls.
   virtual double GetTickCountY2() const;

   // Sets the number of significant digits to use on the tick mark labels
   virtual void SetNumberOfSignificantDigitsOnLabels(unsigned int aNumberDigits);

   // Sets the width used to draw tick marks
   void SetTickMarkWidth(int aWidth) { mTickWidth = aWidth; }

   // Sets the color used to draw tick marks
   void SetTickMarkColor(QColor aColor) { mTickColor = aColor; }

   // Returns the width used to draw tick marks
   int GetTickMarkWidth() const { return mTickWidth; }

   // Returns the color used to draw tick marks
   QColor GetTickMarkColor() const { return mTickColor; }

   // ************************************************************************************ //
   // Grid Lines Display Functions
   // ************************************************************************************ //
   // Sets whether to display a grid for the plot
   virtual void SetShowGrid(bool aInput);

   // Sets the width used to draw the grid lines
   void SetGridWidth(int aWidth) { mGridWidth = aWidth; }

   // Sets the color used to draw the grid lines
   void SetGridColor(QColor aColor) { mGridColor = aColor; }

   // Sets the stipple pattern used to draw the grid lines
   void SetGridStipple(StipplePattern aStipple) { mGridStipple = aStipple; }

   // Gets the width used to draw the grid lines
   int GetGridWidth() const { return mGridWidth; }

   // Gets the color used to draw the grid lines
   QColor GetGridColor() const { return mGridColor; }

   // Gets the stipple pattern used to draw the grid lines
   StipplePattern GetGridStipple() const { return mGridStipple; }

   // ************************************************************************************ //
   // Border Display Functions
   // ************************************************************************************ //
   // Sets whether a border should be drawn on all four sides of the plotting region or just drawn on the two axes.
   virtual void SetShowFullBorder(bool aInput);

   // Sets the width used to draw the border
   void SetBorderWidth(int aWidth) { mBorderWidth = aWidth; }

   // Sets the color used to draw the border
   void SetBorderColor(QColor aColor) { mBorderColor = aColor; }

   // Sets the stipple pattern used to draw the border
   void SetBorderStipple(StipplePattern aStipple) { mBorderStipple = aStipple; }

   // Gets the width used to draw the border lines
   virtual int GetBorderWidth() const { return mBorderWidth; }

   // Gets the color used to draw the border lines
   virtual QColor GetBorderColor() const { return mBorderColor; }

   // Gets the stipple pattern used to draw the border lines
   virtual StipplePattern GetBorderStipple() const { return mBorderStipple; }

   // ************************************************************************************ //
   // General Display Functions
   // ************************************************************************************ //
   // Call this function to Draw the chart.
   void Draw();

   // Call to draw text
   void QtDraw(QPainter* aPainter);

   // Toggle whether the plot will auto-resize when a new point is added outside the X/Y boundaries
   virtual void SetAutoScaleEnabled(bool aInput) { mAbleToBeAutoResized = aInput; }
   // Returns whether the plot will auto-resize when a new point is added outside the X/Y boundaries
   bool GetAutoScaledEnabled() { return mAbleToBeAutoResized; }

   // The following MakePlotPretty functions will change Axis boundaries and tick increments to appear "nice".
   //  When the plot auto-scales this function is not automatically called to reduce overhead of adding points.

   // Changes both the X / Y axis boundaries to limits to include all data with in the plot
   // These function will do nothing if no data is present.
   virtual void MakePlotPretty();
   // Changes only the X axis boundaries to limits to include all data with in the plot
   // These function will do nothing if no data is present.
   virtual void MakePlotPrettyX();
   // Changes both the Y1 / Y2 axis boundaries to limits to include all data with in the plot
   // These function will do nothing if no data is present.
   virtual void MakePlotPrettyY();
   // Changes the X axis tick mark size and number to "best" match the specified min/max X boundary.
   virtual void MakePlotPrettyX(double aMinX, double aMaxX);
   // Changes the Y axis tick mark size and number to "best" match the specified min/max Y1/Y2 boundaries.
   virtual void MakePlotPrettyY(double aMinY, double aMaxY, double aMinY2, double aMaxY2);

   // Set whether to display of a small cross-hair on the origin (0,0)
   virtual void SetShowOrigin(bool aInput);

   // Reduces the amount of padding on the top and right sides (if given true).
   // Only the top and right sides are considered, because the left and bottom
   // sides need extra padding anyway to account for the text of tick labels.
   void ReduceTopPadding(bool aReduced);

signals:
   // emitted when the X,Y, or Y2 boundaries are changed
   void BoundsChanged();

   // emitted when a series is removed
   void SeriesRemoved(unsigned int aSeriesNum);

   // emitted when a series' name changes
   void SeriesNameChanged(unsigned int aSeriesNum);

   // emitted when a series' color changes
   void SeriesColorChanged(unsigned int aSeriesNum);

   // emitted when a series is selected
   void SeriesSelected(unsigned int aSeriesNum);

   // emitted when a data point is selected
   void DataPointSelected(unsigned int aSeriesNum, unsigned int aIndex);

   // emitted when a data point is edited
   void DataPointEdited(unsigned int aSeriesNum, unsigned int aIndex);

   // emitted when a data point is deleted
   void DataPointDeleted(unsigned int aSeriesNum, unsigned int aIndex);

   // emitted when a data point is added
   void DataPointAdded(unsigned int aSeriesNum, unsigned int aIndex);

   // emitted when an item model series is updated
   void ItemModelSeriesUpdated(unsigned int aSeriesNum);

protected:
   // Called by MakePlotPretty*() to get the new bounds and step size.
   // aMin and aMax are inout parameters.
   // aStepSize is an out parameter.
   // Returns true if the provided min and max are "valid".
   static bool GetPrettyBounds(double& aMin, double& aMax, double& aStepSize);

   static bool IsDoubleValid(double aNum);

   SeriesBase* FindSeriesP(int aSeriesNumber) const
   {
      auto i = mData.find(aSeriesNumber);
      if (i != mData.end())
      {
         return const_cast<SeriesBase*>(i->second);
      }
      return nullptr;
   }

   SeriesBase* FindSeriesP(const QString& aName) const
   {
      for (const auto& i : mData)
      {
         if (i.second->Name() == aName)
         {
            return const_cast<SeriesBase*>(i.second);
         }
      }
      return nullptr;
   }

   virtual void ComputePlotBoundaries();

   SeriesBase& FindOrMakeSeries(int aSeriesNumber);

   // Returns the boundaries of the Plot.
   void GetPlotBoundaries(int& aPlotLeft, int& aPlotRight, int& aPlotBottom, int& aPlotTop);

   // Updates whether to display the secondary Y Axis on the right side of the plot
   // Secondary axis is shown if there is at least one visible series set to use the secondary axis or force show is true.
   virtual void UpdateSecondaryAxisVisibility();

   // Translates a point in the data space into a point in screen space
   void TranslatePointFromRealToScene(unsigned int aSeriesNum, double aInputX, double aInputY, double& aOutputX, double& aOuputY);
   void TranslatePointFromRealToScene(const SeriesBase& aSeries,
                                      double            aInputX,
                                      double            aInputY,
                                      double&           aOutputX,
                                      double&           aOuputY);

   // Private function for translating from real X/Y values to display X/Y values
   virtual void TranslatePointFromRealToScene(double  aInputX,
                                              double  aInputY,
                                              double& aOutputX,
                                              double& aOuputY,
                                              bool    aUseSecondaryAxis = false);

   // Private function for computing grid intersection of a line between two points and the grid boundaries.
   //  Note: x1 and y1 are assume to be within the grid boundaries.
   virtual QList<std::pair<double, double>> ComputeGridIntercept(double x1, double y1, double x2, double y2);
   // Private functions to draw data points.
   virtual void DrawDataDisplay();
   virtual void DrawDataDisplayBar(int aSeriesNum);
   virtual void DrawDataDisplayPoint(int aSeriesNum, int aWidth);
   virtual void DrawDataDisplayLine(int aSeriesNum, bool aHighlight);
   virtual void DrawDataDisplayMidpointGrid(int aSeriesNum);
   virtual void DrawDataDisplayText(QPainter* aPainter);
   virtual void DrawDataDisplayPointText(QPainter* aPainter, int aSeriesNum);
   virtual void DrawDataDisplayLineText(QPainter* aPainter, int aSeriesNum, bool aHighlight);
   // Private function to draw the display of the border
   virtual void DrawBorder();

   virtual void DrawCrosshairs();
   virtual void DrawCrosshairsText(QPainter* aPainter);
   // Private function to draw the tick marks
   virtual void DrawTickMarks();
   // Private function to update the labels.
   virtual void DrawLabels(QPainter* aPainter);
   // Private function to draw the display of the legend
   virtual void DrawLegend(QPainter* aPainter);
   // Private function to draw the display of the origin
   virtual void DrawOrigin();
   // Private function to draw the background color of the plot
   virtual void DrawPlotBackground();
   // Private function to draw the display of the grid
   virtual void DrawGrid();
   // Private function to draw the display of the underlay (or background) lines
   // Note: no implementation is provide for this function.
   virtual void DrawUnderlayLines(){};
   // Private function to draw the display of the overlay lines
   virtual void DrawOverlayLines();
   // Private function to draw the display of the overlay text
   virtual void DrawOverlayText(QPainter* aPainter);
   // private function to set the Axes Boundaries
   virtual void SetAxesBoundsP(double aMinX, double aMinY, double aMaxX, double aMaxY);

   virtual void RenderText(QPainter* aPainter, int aX, int aY, const QString& aText, int aJust, float aRot);

   virtual double GetYMinBoundary(bool aSecondary) const { return aSecondary ? mMinYBoundary2 : mMinYBoundary; }
   virtual double GetYMaxBoundary(bool aSecondary) const { return aSecondary ? mMaxYBoundary2 : mMaxYBoundary; }

   virtual void ComputeTickPositions();

   // OpenGL support functions
   void EnableStippling(StipplePattern aStipple);
   void DisableStippling();

   // Helper functions to draw Bar Graphs
   virtual double GetBarWidth(int aSeriesNum);
   virtual bool   UseDefaultBarWidth() { return mBarWidth == -1; }

   virtual void SeriesRedrawRequest();

   int mLabelFontSize{10};

   std::vector<QString>       mLabelStrings{NUM_LABELS};
   std::map<LabelPos, QColor> mLabelColors;
   QColor                     mTextColor{QColor()};
   double                     mLeftLabelPosition;
   double                     mRightLabelPosition;

   int mDisplayWidth;  // area width
   int mDisplayHeight; // area height

   // Defines bounding box for plotting region as a % of the total widget
   double mPlotLeft_percent;
   double mPlotRight_percent;
   double mPlotBottom_percent;
   double mPlotTop_percent;

   // Defines boundary box for the plotting region in pixel positions
   int mPlotLeft_pixels;
   int mPlotTop_pixels;
   int mPlotRight_pixels;
   int mPlotBottom_pixels;

   // Variables to draw the Plot background and support drawing a gradient for the Plot Background
   bool   mDrawPlotBackground;
   bool   mPlotBackgroundGradient;
   QColor mPlotBackgroundColor;
   int    mBackgroundImage;

   class gradientPoint
   {
   public:
      gradientPoint(double aX, double aY, const QColor& aColor)
         : x(aX)
         , y(aY)
         , color(aColor)
      {
      }

      double x;
      double y;
      QColor color;
   };
   std::vector<gradientPoint> mPlotGradientPoints;

   // variables for controlling the display of the legend
   bool mShowLegend{false};

   // Legend position as a percentage of the viewing region (user inputs legend position this way)
   double mLegendXPosition{0.75};
   double mLegendYPosition{0.25};

   // Legend display box shape, used to determine mouse clicks on the legend for moving it.
   QRect  mLegendRect;
   QColor mLegendBackgroundColor{QColor(255, 255, 255, 128)};
   int    mLegendFontSize{10};

   // Number of significant digits to display on Axis Labels
   unsigned int mSignificantDigits{6};

   // Enum to control how tick marks should be drawn
   TickMarkPos mTickMarkPosition{INTERIOR};

   // Boolean to control display tick marks via increment or total # of ticks
   bool mUseTickMarkIncrementX{false};
   bool mUseTickMarkIncrementY{false};
   bool mUseTickMarkIncrementY2{false};

   // Increment to use between tick marks
   double mTickMarkIncrementX{0};
   double mTickMarkIncrementY{0};
   double mTickMarkIncrementY2{0};

   // Number of tick marks to display on each axis
   unsigned int mTickMarkNumberX{5};
   unsigned int mTickMarkNumberY{5};
   unsigned int mTickMarkNumberY2{5};

   // Booleans to control hows tick marks are displayed when increment is selected.
   bool mLeftJustifiedXTicks{true};
   bool mBottomJustifiedYTicks{true};
   bool mBottomJustifiedYTicks2{true};
   int  mTickMarkFontSize{8};
   bool mTickMarkShowLabel{true};

   std::vector<Double2> mXAxisTicks;
   std::vector<Double2> mYAxisTicks;
   std::vector<Double2> mYAxisTicks2;

   // Defines bounding box for the real data (mData)
   double mMaxXBoundary{std::numeric_limits<double>::lowest()};
   double mMinXBoundary{std::numeric_limits<double>::max()};
   double mMaxYBoundary{std::numeric_limits<double>::lowest()};
   double mMinYBoundary{std::numeric_limits<double>::max()};
   double mMaxYBoundary2{std::numeric_limits<double>::lowest()};
   double mMinYBoundary2{std::numeric_limits<double>::max()};

   SeriesMap mData;

   bool mAbleToBeAutoResized{true};
   bool mForceShowSecondaryAxis{false};
   bool mEnableSecondaryAxis{false};
   bool mDrawDataLast{true};

   StipplePattern mBorderStipple{NO_STIPPLE};
   StipplePattern mGridStipple{DOTTED};

   int mBorderWidth{2};
   int mGridWidth{1};
   int mTickWidth{1};

   QColor mBorderColor{QColor()};
   QColor mGridColor{QColor(128, 128, 128, 128)};
   QColor mTickColor{QColor()};

   bool mShowLabelOnSelectedDataPoint{true};
   bool mShowFullBorder{true};
   bool mShowOrigin{false};
   bool mShowGrid{true};

   bool           mShowCrosshairs{false};
   bool           mUseCrosshairValue{true};
   double         mCrosshairsValue{0.0};
   double         mCrosshairsPercent{0.0};
   int            mCrosshairWidth{1};
   StipplePattern mCrosshairStipple{NO_STIPPLE};
   QColor         mCrosshairColor{QColor(0, 0, 0)};
   int            mCrosshairSeriesNum{1};
   bool           mCrosshairShowAllSeries{true};

   // 'true' to auto-assign distinct colors
   bool mUseDefaultPens{true};

   class overlay
   {
   public:
      overlay() { mId = mCount++; }
      const int GetId() const { return mId; }

   private:
      static int mCount;
      int        mId;
   };

   class lineClass : public overlay
   {
   public:
      lineClass(double aSlope, double aIntercept, bool aVertical = false)
         : overlay()
         , mSlope(aSlope)
         , mIntercept(aIntercept)
         , mVertical(aVertical)
      {
      }

      double         mSlope;
      double         mIntercept;
      bool           mVertical;
      int            mWidth;
      QColor         mColor;
      StipplePattern mStipple;
   };

   class textClass : public overlay
   {
   public:
      textClass(QString aText, double aXPos, double aYPos)
         : overlay()
         , mText{aText}
         , mX{aXPos}
         , mY{aYPos}
      {
      }

      QString       mText;
      QColor        mColor;
      double        mX;
      double        mY;
      double        mAngle;
      int           mSize;
      Qt::Alignment mAlign;
   };

   std::list<lineClass> mOverlayLines;
   std::list<textClass> mOverlayText;

   // Data to support storing Plot Styles
   PlotStyle mDefaultPlotStyle{POINT};

   // std::map<int, QColor>     mSeriesFillColorMap;

   // Data to support drawing Bar Graphs
   double mBarWidth{-1.0};

   // Functions to support drawing LINE and POINT charts with varying colors.
   // Private function that will generate a QPen for a point on the grid, the QPen will take into account an gradient
   // applied to the data QPen GetPenP(int aSeriesNum, double aCurrentX, double aCurrentY,
   //                              double aPreviousX, double aPreviousY);
   // Private function to get the color for a specified Z-value
   QColor GetColorAt(int aSeriesNum, double aValue);
   // Private function to get the next color (first color greater than aValue)
   QColor GetNextColor(int aSeriesNum, double& aValue);

   // Data to support drawing LINE and POINT charts with varying colors.
   bool mUseZColor{false};

   // Use the color value supplied with the point
   bool mUsePointColor{false};

   bool mReduceTopPadding{false};

   void   GenerateMidpointGridData(int aSeriesNum);
   double GetBoundedX(double aX);
   double GetBoundedY(double aY);

   std::map<int, std::vector<double>>              mGridXValues;
   std::map<int, std::vector<double>>              mGridYValues;
   std::map<int, std::vector<std::vector<double>>> mGridDataValues;
   std::map<int, std::vector<double>>              mGridMidXValues;
   std::map<int, std::vector<double>>              mGridMidYValues;

   // Use these variables to remove "magic numbers", currently no method to change these values
   constexpr static int    cHighlightWidthModifier{4};  // Additional width to highlight lines & points
   constexpr static int    cPointWidthModifier{6};      // Additional width to draw points on lines
   constexpr static double cSeriesFontLabelFactor{1.0}; // Font scale factor to change font size of x/y value labels
   constexpr static int    cSeriesFontLabelOffset{5};   // Offset used to display value labels
   constexpr static double cTitleLabelPosition{0.96};
   constexpr static double cTopLabelPosition{0.93};
   constexpr static double cLeftLabelDefaultPosition{0.5};
   constexpr static double cRightLabelDefaultPosition{0.92};
   constexpr static double cRightLabelDefaultPositionReduced{0.97};
   constexpr static double cBottomLabelPosition{0.05};
   constexpr static double cTickMarkLength{0.01};
   constexpr static double cPlotMinDefaultBoundary{0.1};
   constexpr static double cPlotMaxDefaultBoundary{0.9};
   constexpr static double cPlotMaxDefaultBoundaryReduced{0.95};
   constexpr static double cTextLabelOffset{0.01};
   constexpr static double cRightLeftLabelTransition{
      0.8}; // Value in % which label switch from right side to left side for points and crosshairs
};

class UtQtAbstractItemModelSeries : public QObject, public UtQtGL2DPlot::SeriesBase
{
   Q_OBJECT
public:
   UtQtAbstractItemModelSeries(QAbstractItemModel*     aItemModel,
                               int                     aColumnX,
                               int                     aColumnY,
                               int                     aSeriesNumber,
                               UtQtGL2DPlot::PlotStyle aPlotStyle);
   ~UtQtAbstractItemModelSeries() override = default;

   bool   IsEmpty() const override;
   size_t Size() const override;
   void   WriteCSV_X(QTextStream& aOut) const override;
   void   WriteCSV_Y(QTextStream& aOut) const override;
   void   GetBounds(double& aMinX, double& aMaxX, double& aMinY, double& aMaxY) const override;
   void   Get(size_t aIndex, double& aX, double& aY) const override;

   void    rowsInserted(const QModelIndex& aParent, int aStart, int aEnd);
   void    rowsRemoved(const QModelIndex& aParent, int aStart, int aEnd);
   void    modelReset();
   QString Name() const override;

signals:
   void Redraw();

private:
   QAbstractItemModel* mModelPtr;
   int                 mColumnX;
   int                 mColumnY;
};

#endif
