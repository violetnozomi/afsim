// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UtQt3DTrajectoryPlot_HPP
#define UtQt3DTrajectoryPlot_HPP

#include "utilqt_export.h"

#include <QList>
#include <QVector3D>
#include <QtOpenGL/qgl.h>

class UTILQT_EXPORT UtQt3DTrajectoryPlot
{
public:
   using TrajectoryData = QList<QVector3D>;

   enum Axis
   {
      X_AXIS,
      Y_AXIS,
      Z_AXIS
   };

   UtQt3DTrajectoryPlot();
   ~UtQt3DTrajectoryPlot();

   void Initialize();
   void Paint(QPaintDevice* aPaintDevice);

   void DefineAxis(QString aTitle, double aMin, double aMax, double aInterval, Axis aAxis);
   void SetLabel(QString aTitle, Axis aAxis);
   void SetScale(double aScale) { mScale = aScale; }
   void SetRotation(double aYaw, double aPitch)
   {
      mYaw_deg   = aYaw;
      mPitch_deg = aPitch;
   }

   // This function will set the plot data for a given series.  All previous data for this series will be removed.
   void SetPlotData(QList<double>& aXAxis,
                    QList<double>& aYAxis,
                    QList<double>& aZAxis,
                    int            aSeriesNum            = 1,
                    const double   aUnitConversionFactor = 1.0);
   void RemoveDataSeries(int aSeriesNum = 1);

   int GetWidth() { return mWidth; }
   int GetHeight() { return mHeight; }
   int GetDepth() { return mDepth; }

   bool GetXMinMax(double& aXMin, double& aXMax);
   bool GetYMinMax(double& aYMin, double& aYMax);
   bool GetZMinMax(double& aZMin, double& aZMax);
   bool GetMinMax(double& aXMin, double& aXMax, double& aYMin, double& aYMax, double& aZMin, double& aZMax);

   void ShowGrid(bool aShowGrid) { mDrawGrid = aShowGrid; }
   void ShowLegend(bool aShowLegend) { mDrawLegend = aShowLegend; }
   void ShowOrigin(bool aShowOrigin) { mDrawOrigin = aShowOrigin; }

   void SetFontSize(int aFontSize) { mFontSize = aFontSize; }
   void SetNumberOfSignificantDigits(int aNumber) { mSignificantDigits = aNumber; }
   void SetGridColor(QColor aColor);
   void SetBorderColor(QColor aColor);
   void SetTextColor(QColor aColor);
   void SetSeriesColor(QColor aColor, int aSeriesNum = 1);

   QColor GetGridColor() const { return mGridColor; }
   QColor GetBorderColor() const { return mBorderColor; }
   QColor GetTextColor() const { return mTextColor; }
   QColor GetSeriesColor(int aSeriesNum = 1);


   void Clear();

private:
   struct AxisData
   {
      double  _increment;
      double  _min;
      double  _max;
      QString _label;
   };

   struct PlaneData
   {
      GLuint _border;
      GLuint _grid;
   };

   void BuildPlot();
   void BuildAxisPlanes();
   void BuildXYPlane();
   void BuildYZPlane();
   void BuildXZPlane();

   void PaintOrigin();
   void PaintTrajectory();
   void PaintText(QPaintDevice*  aPaintDevice,
                  const QString& aText,
                  Axis           aAxis,
                  Qt::Alignment  aAlign = Qt::AlignLeft | Qt::AlignVCenter);

   double GetMinIncrement(double min, double increment);
   double GetMaxIncrement(double max, double increment);

   double mScale;

   int mWidth;
   int mHeight;
   int mDepth;

   QFont  mFont;
   double mFontSize;
   int    mSignificantDigits;

   bool mDirty;

   PlaneData mXYPlane;
   PlaneData mYZPlane;
   PlaneData mXZPlane;

   AxisData mXAxis;
   AxisData mYAxis;
   AxisData mZAxis;

   double mYaw_deg;
   double mPitch_deg;

   double mXMin; // lowest x value
   double mXMax; // highest x value
   double mYMin; // lowest y value
   double mYMax; // highest y value
   double mZMin; // lowest z value
   double mZMax; // highest z value

   std::map<int, TrajectoryData> mTrajectoryRawDataMap;
   GLuint                        mTrajectoryPlot; // Plot data

   bool mDrawGrid;
   bool mDrawLegend;
   bool mDrawOrigin;

   QColor                mGridColor;
   QColor                mBorderColor;
   QColor                mTextColor;
   std::map<int, QColor> mSeriesColorMap;
};

#endif // WAVEGLCHARTVIEW_HPP
