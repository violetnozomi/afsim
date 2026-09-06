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

#include "WkfSensorDetectionReport.hpp"

#include <QDialogButtonBox>
#include <QListWidget>
#include <QStandardItemModel>

#include "UtUnits.hpp"
#include "WkfEnvironment.hpp"

wkf::SensorDetectionReport::SensorDetectionReport(QWidget* aParent)
   : QDialog(aParent)
   , mTimeLineId(-1)
{
   mUi.setupUi(this);
   connect(mUi.mTarget, &QComboBox::currentTextChanged, this, &SensorDetectionReport::TargetChanged);
   connect(mUi.mSensor, &QComboBox::currentTextChanged, this, &SensorDetectionReport::SensorChanged);
   mUi.mTablePlot->ShowColumn(AbstractSensorDetectionModel::eStatus);
   mUi.mTablePlot->ShowColumn(AbstractSensorDetectionModel::ePd);
   mUi.mTablePlot->ShowColumn(AbstractSensorDetectionModel::eReqPd);
   mUi.mTablePlot->ShowColumn(AbstractSensorDetectionModel::eRcvdPower);
   mUi.mTablePlot->ShowColumn(AbstractSensorDetectionModel::eNoise);
   mUi.mTablePlot->ShowColumn(AbstractSensorDetectionModel::eClutter);
   mUi.mTablePlot->ShowColumn(AbstractSensorDetectionModel::eInterference);
   mUi.mTablePlot->ShowColumn(AbstractSensorDetectionModel::eNCI);
   mUi.mTablePlot->ShowColumn(AbstractSensorDetectionModel::eSNR);
}

void wkf::SensorDetectionReport::SetModel(AbstractSensorDetectionModel* aModelPtr)
{
   mModelPtr = aModelPtr;
   mUi.mTablePlot->SetModel(aModelPtr);

   QStringList sensors = aModelPtr->GetSensors();
   mUi.mSensor->addItems(sensors);

   connect(mModelPtr, &AbstractSensorDetectionModel::TableUnitsChanged, mUi.mTablePlot, &UtQtTablePlotWidget::MakePretty);

   mUi.mTablePlot->AddPlot(AbstractSensorDetectionModel::eRcvdPower);
   mUi.mTablePlot->AddPlot(AbstractSensorDetectionModel::eNCI);
   mUi.mTablePlot->MakePretty();
}

void wkf::SensorDetectionReport::TargetChanged(const QString& aTarget)
{
   int comboIdx   = mUi.mTarget->findText(aTarget);
   int platformId = mUi.mTarget->itemData(comboIdx, Qt::UserRole).toInt();
   mModelPtr->SetTarget(platformId);
   mUi.mTablePlot->update();
   mUi.mTablePlot->MakePretty();
}

void wkf::SensorDetectionReport::SensorChanged(const QString& aSensor)
{
   mModelPtr->SetSensor(aSensor);
   mUi.mTarget->clear();
   QList<QPair<int, QString>> targets = mModelPtr->GetTargets();
   for (auto&& it : targets)
   {
      mUi.mTarget->addItem(it.second, it.first);
   }
}

void wkf::SensorDetectionReport::SetTime(float aTime)
{
   mUi.mTablePlot->SetTime(aTime);
}

void wkf::SensorDetectionReport::EnableCustomPlotContextMenu()
{
   mUi.mTablePlot->EnableCustomPlotContextMenu();
   connect(mUi.mTablePlot,
           &UtQtTablePlotWidget::CustomPlotContext,
           this,
           [this](const QPoint& aPos, const QPointF& aData, bool aPlot2D)
           { emit CustomPlotContextMenu(aPos, aData, aPlot2D); });
}

wkf::AbstractSensorDetectionModel::AbstractSensorDetectionModel(QObject* aParent)
   : AbstractTableModel(aParent)
{
   DefineColumn("beam");
   DefineColumn("range error", UtUnits::cLENGTH);
   DefineColumn("az. error", UtUnits::cANGLE);
   DefineColumn("el. error", UtUnits::cANGLE);
   DefineColumn("range rate error", UtUnits::cSPEED);
   DefineColumn("pd");
   DefineColumn("req. pd");
   DefineColumn("rep. side", -1, 2);
   DefineColumn("rep. type", -1, 2);
   DefineColumn("status", -1, 2);
   DefineColumn("r_to_t az", UtUnits::cANGLE);
   DefineColumn("r_to_t el", UtUnits::cANGLE);
   DefineColumn("r gain", UtUnits::cRATIO);
   DefineColumn("x_to_t az", UtUnits::cANGLE);
   DefineColumn("x_to_t el", UtUnits::cANGLE);
   DefineColumn("x gain", UtUnits::cRATIO);
   DefineColumn("masking");
   DefineColumn("radar t_to_x az", UtUnits::cANGLE);
   DefineColumn("radar t_to_x el", UtUnits::cANGLE);
   DefineColumn("radar sig.", UtUnits::cRATIO);
   DefineColumn("xmit power", UtUnits::cRATIO);
   DefineColumn("power density at target", UtUnits::cRATIO);
   DefineColumn("rcvd power", UtUnits::cRATIO);
   DefineColumn("noise", UtUnits::cRATIO);
   DefineColumn("clutter", UtUnits::cRATIO);
   DefineColumn("interference", UtUnits::cRATIO);
   DefineColumn("N+C+J", UtUnits::cRATIO);
   DefineColumn("signal to noise", UtUnits::cRATIO);
   DefineColumn("propagation factor");
   DefineColumn("absorption factor");
   DefineColumn("detection thresh.");
   DefineColumn("zone atten.");
   DefineColumn("pixel count");
   DefineColumn("optical refl.");
   DefineColumn("ir t_to_x az", UtUnits::cANGLE);
   DefineColumn("ir t_to_x el", UtUnits::cANGLE);
   DefineColumn("ir sig.", UtUnits::cRATIO);
   DefineColumn("opt. t_to_x az", UtUnits::cANGLE);
   DefineColumn("opt. t_to_x el", UtUnits::cANGLE);
   DefineColumn("opt. sig.", UtUnits::cRATIO);
}
