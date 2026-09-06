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

#ifndef WKFSENSORDETECTIONREPORT_HPP
#define WKFSENSORDETECTIONREPORT_HPP

#include "wkf_common_export.h"

#include <set>

#include <QDialog>

#include "WkfAbstractTableModel.hpp"
#include "ui_WkfSensorDetectionReport.h"

namespace wkf
{
class WKF_COMMON_EXPORT AbstractSensorDetectionModel : public AbstractTableModel
{
   Q_OBJECT
public:
   enum Fields
   {
      eBeam         = 0,
      eRangeErr     = 1,
      eAzErr        = 2,
      eElErr        = 3,
      eRangeRateErr = 4,
      ePd           = 5,
      eReqPd        = 6,
      eReportedSide = 7, // string value
      eReportedType = 8, // string value
      eStatus       = 9, // string value
      eRcvrToTgtAz  = 10,
      eRcvrToTgtEl  = 11,
      eRcvrGain     = 12,
      eXmtrToTgtAz  = 13,
      eXmtrToTgtEl  = 14,
      eXmtrGain     = 15,
      eMasking      = 16,
      eRadarAz      = 17,
      eRadarEl      = 18,
      eRadarSig     = 19,
      eXmitPower    = 20,
      eTgtRcvdPower = 21,
      eRcvdPower    = 22,
      eNoise        = 23,
      eClutter      = 24,
      eInterference = 25,
      eNCI          = 26,
      eSNR          = 27,
      ePropagation  = 28,
      eAbsorption   = 29,
      eDetThresh    = 30,
      eZoneAtten    = 31,
      eOpticalRefl  = 32,
      ePixelCount   = 33,
      eIrAz         = 34,
      eIrEl         = 35,
      eIrSig        = 36,
      eOptAz        = 37,
      eOptEl        = 38,
      eOptSig       = 39,
      eFieldCount   = 40
   };
   AbstractSensorDetectionModel(QObject* aParent);

   virtual QStringList                GetSensors()                      = 0;
   virtual void                       SetSensor(const QString& aSensor) = 0;
   virtual QList<QPair<int, QString>> GetTargets()                      = 0;
   virtual void                       SetTarget(int aPlatorm)           = 0;
};

class WKF_COMMON_EXPORT SensorDetectionReport : public QDialog
{
   Q_OBJECT
public:
   SensorDetectionReport(QWidget* aParent);
   ~SensorDetectionReport() override = default;

   void SetModel(AbstractSensorDetectionModel* aModelPtr);
   void SetTime(float aTime);
   void EnableCustomPlotContextMenu();
signals:
   void CustomPlotContextMenu(const QPoint& aPos, const QPointF& aDataPoint, bool aPlot2D);

private:
   Ui_WkfSensorDetectionReport   mUi;
   AbstractSensorDetectionModel* mModelPtr;
   void                          TargetChanged(const QString& aTarget);
   void                          SensorChanged(const QString& aSensor);

   int           mTimeLineId;
   std::set<int> mColumnDisplaySet;
};
} // namespace wkf
#endif
