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

#ifndef RVSENSORDETECTIONMODEL_HPP
#define RVSENSORDETECTIONMODEL_HPP

#include "RvResultPlatform.hpp"
#include "statistics/WkfSensorDetectionReport.hpp"

namespace RvDetectionReport
{
class SensorDetectionModel : public wkf::AbstractSensorDetectionModel
{
   Q_OBJECT
public:
   SensorDetectionModel(const rv::ResultPlatform* aPlatform, QObject* aParent);
   ~SensorDetectionModel() override;

   QStringList                GetSensors() override;
   void                       SetSensor(const QString& aSensor) override;
   QList<QPair<int, QString>> GetTargets() override;
   void                       SetTarget(int aPlatorm) override;
   void                       SetCacheRange(float aStart, float aEnd);

private:
   QString  GetStatusString(const rv::EM_Interaction_Status& aStatus, char aDelim) const;
   int      rowCount(const QModelIndex& aIndex) const override;
   QVariant data(const QModelIndex& aIndex, int aRole) const override;
   QVariant headerData(int aSection, Qt::Orientation aOrientation, int aRole) const override;

   const rv::ResultPlatform*                        mPlatform;
   const rv::ResultPlatform::DetectionAttemptArray* mArray;
   QString                                          mSensor;
   float                                            mStart;
   float                                            mEnd;
};
} // namespace RvDetectionReport
#endif
