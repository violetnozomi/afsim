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

#include "RvSensorDetectionModel.hpp"

#include "RvEnvironment.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"

RvDetectionReport::SensorDetectionModel::SensorDetectionModel(const rv::ResultPlatform* aPlatform, QObject* aParent)
   : AbstractSensorDetectionModel(aParent)
   , mPlatform(aPlatform)
{
   mStart = mEnd                                               = 0;
   mArray                                                      = nullptr;
   const rv::ResultPlatform::DetectionAttemptMap&          map = mPlatform->GetDetectionAttemptMap();
   rv::ResultPlatform::DetectionAttemptMap::const_iterator it  = map.begin();
   if (it != map.end())
   {
      mSensor                                                                     = it->first.c_str();
      std::map<int, rv::ResultPlatform::DetectionAttemptArray>::const_iterator jt = it->second.begin();
      if (jt != it->second.end())
      {
         mArray = &jt->second;
         mStart = mArray->GetMinTime();
         mEnd   = mArray->GetMaxTime();
      }
   }
}

RvDetectionReport::SensorDetectionModel::~SensorDetectionModel() {}

int RvDetectionReport::SensorDetectionModel::rowCount(const QModelIndex& aIndex) const
{
   if (aIndex.isValid())
   {
      return 0;
   }
   else
   {
      if (nullptr != mArray)
      {
         return (int)(mArray->size());
      }
      return 0;
   }
}

QVariant RvDetectionReport::SensorDetectionModel::data(const QModelIndex& aIndex, int aRole) const
{
   if (aRole == Qt::DisplayRole)
   {
      if (nullptr != mArray)
      {
         const rv::MsgDetectAttempt* da =
            (*mArray)[aIndex.row()]; // dynamic_cast<rv::MsgDetectAttempt*>(*(mArray->begin() + aIndex.row()));

         switch (aIndex.column())
         {
         case (eBeam):
            return Convert(aIndex.column(), da->beam());
         case (eRangeErr):
            return Convert(aIndex.column(), da->rangeError());
         case (eAzErr):
            return Convert(aIndex.column(), da->azimuthError());
         case (eElErr):
            return Convert(aIndex.column(), da->elevationError());
         case (eRangeRateErr):
            return Convert(aIndex.column(), da->rangeRateError());
         case (ePd):
            return Convert(aIndex.column(), da->pd());
         case (eReqPd):
            return Convert(aIndex.column(), da->requiredPd());
         case (eReportedSide):
            return QString(da->reportedSide().c_str());
         case (eReportedType):
            return QString(da->reportedType().c_str());
         case (eStatus):
            return GetStatusString(da->interaction().status(), '|');
         case (eRcvrToTgtAz):
            return Convert(aIndex.column(), da->interaction().rcvrBeam().azimuth());
         case (eRcvrToTgtEl):
            return Convert(aIndex.column(), da->interaction().rcvrBeam().elevation());
         case (eRcvrGain):
            return Convert(aIndex.column(), da->interaction().rcvrBeam().gain());
         case (eXmtrToTgtAz):
            return Convert(aIndex.column(), da->interaction().xmtrBeam().azimuth());
         case (eXmtrToTgtEl):
            return Convert(aIndex.column(), da->interaction().xmtrBeam().elevation());
         case (eXmtrGain):
            return Convert(aIndex.column(), da->interaction().xmtrBeam().gain());
         case (eMasking):
            return Convert(aIndex.column(), da->interaction().maskingFactor());
         case (eRadarAz):
            return Convert(aIndex.column(), da->interaction().radarSignatureAzimuth());
         case (eRadarEl):
            return Convert(aIndex.column(), da->interaction().radarSignatureElevation());
         case (eRadarSig):
            return Convert(aIndex.column(), da->interaction().radarSignature());
         case (eXmitPower):
            return Convert(aIndex.column(), da->interaction().transmittedPower());
         case (eTgtRcvdPower):
            return Convert(aIndex.column(), da->interaction().powerDensityAtTarget());
         case (eRcvdPower):
            return Convert(aIndex.column(), da->interaction().receivedPower());
         case (eNoise):
            return Convert(aIndex.column(), da->interaction().rcvrNoisePower());
         case (eClutter):
            return Convert(aIndex.column(), da->interaction().clutterPower());
         case (eInterference):
            return Convert(aIndex.column(), da->interaction().interferencePower());
         case (eNCI):
            return Convert(aIndex.column(),
                           da->interaction().rcvrNoisePower() + da->interaction().clutterPower() +
                              da->interaction().interferencePower());
         case (eSNR):
            return Convert(aIndex.column(), da->interaction().signalToNoise());
         case (ePropagation):
            return Convert(aIndex.column(), da->interaction().propagationFactor());
         case (eAbsorption):
            return Convert(aIndex.column(), da->interaction().absorbtionFactor());
         case (eDetThresh):
            return Convert(aIndex.column(), da->interaction().detectionThreshold());
         case (eZoneAtten):
            return Convert(aIndex.column(), da->interaction().zoneAttenuationFactor());
         case (eOpticalRefl):
            return Convert(aIndex.column(), da->interaction().opticalReflectivity());
         case (ePixelCount):
            return Convert(aIndex.column(), da->interaction().pixelCount());
         case (eIrAz):
            return Convert(aIndex.column(), da->interaction().infraredSignatureAzimuth());
         case (eIrEl):
            return Convert(aIndex.column(), da->interaction().infraredSignatureElevation());
         case (eIrSig):
            return Convert(aIndex.column(), da->interaction().infraredSignature());
         case (eOptAz):
            return Convert(aIndex.column(), da->interaction().opticalSignatureAzimuth());
         case (eOptEl):
            return Convert(aIndex.column(), da->interaction().opticalSignatureElevation());
         case (eOptSig):
            return Convert(aIndex.column(), da->interaction().opticalSignature());
         default:
            return QVariant();
         }
      }
   }
   return QVariant();
}

QString RvDetectionReport::SensorDetectionModel::GetStatusString(const rv::EM_Interaction_Status& aStatus, char aDelim) const
{
   QString message;
   if (!aStatus.rcvrRangeLimits() && aStatus.rcvrRangeLimitsValid())
   {
      message += "Range Limited";
   }
   if (!aStatus.rcvrAltitudeLimits() && aStatus.rcvrAltitudeLimitsValid())
   {
      if (!message.isEmpty())
      {
         message += aDelim;
      }
      message += "Alt Limited";
   }
   if (!aStatus.rcvrAngleLimits() && aStatus.rcvrAngleLimitsValid())
   {
      if (!message.isEmpty())
      {
         message += aDelim;
      }
      message += "Angle Limited";
   }
   if (!aStatus.rcvrHorizonMasking() && aStatus.rcvrHorizonMaskingValid())
   {
      if (!message.isEmpty())
      {
         message += aDelim;
      }
      message += "Horizon Masked";
   }
   if (!aStatus.rcvrTerrainMasking() && aStatus.rcvrTerrainMaskingValid())
   {
      if (!message.isEmpty())
      {
         message += aDelim;
      }
      message += "Terrain Masked";
   }
   if (!aStatus.signalLevel() && aStatus.signalLevelValid())
   {
      if (!message.isEmpty())
      {
         message += aDelim;
      }
      message += "Low Signal";
   }
   if (message.isEmpty())
   {
      message = "OK";
   }
   return message;
}

QVariant RvDetectionReport::SensorDetectionModel::headerData(int aSection, Qt::Orientation aOrientation, int aRole) const
{
   if (aRole == Qt::DisplayRole)
   {
      if (aOrientation == Qt::Vertical)
      {
         if (nullptr != mArray)
         {
            const rv::MsgDetectAttempt* da =
               (*mArray)[aSection]; // dynamic_cast<rv::MsgDetectAttempt*>(*(mArray->begin() + aSection));
            return da->simTime();
         }
      }
   }
   return wkf::AbstractTableModel::headerData(aSection, aOrientation, aRole);
}

QStringList RvDetectionReport::SensorDetectionModel::GetSensors()
{
   QStringList foo;
   for (auto&& it : mPlatform->GetDetectionAttemptMap())
   {
      foo.push_back(it.first.c_str());
   }
   return foo;
}

void RvDetectionReport::SensorDetectionModel::SetSensor(const QString& aSensor)
{
   mArray                                                      = nullptr;
   mSensor                                                     = aSensor;
   const rv::ResultPlatform::DetectionAttemptMap&          map = mPlatform->GetDetectionAttemptMap();
   rv::ResultPlatform::DetectionAttemptMap::const_iterator it  = map.find(aSensor.toStdString());
   if (it != map.end())
   {
      std::map<int, rv::ResultPlatform::DetectionAttemptArray>::const_iterator jt = it->second.begin();
      if (jt != it->second.end())
      {
         beginResetModel();
         mArray = &jt->second;
         endResetModel();
      }
   }
}

QList<QPair<int, QString>> RvDetectionReport::SensorDetectionModel::GetTargets()
{
   QList<QPair<int, QString>>                              foo;
   const rv::ResultPlatform::DetectionAttemptMap&          map = mPlatform->GetDetectionAttemptMap();
   rv::ResultPlatform::DetectionAttemptMap::const_iterator it  = map.find(mSensor.toStdString());
   if (it != map.end())
   {
      for (auto&& jt : it->second)
      {
         int id = jt.first;
         // get a name
         QString         name    = "";
         rv::ResultData* results = rvEnv.GetData();
         if (0 != results)
         {
            rv::ResultDb* db = results->GetDb();
            if (0 != db)
            {
               rv::ResultPlatform* ep_platform = db->FindPlatform(id);
               if (0 != ep_platform)
               {
                  name = ep_platform->GetName().c_str();
               }
            }
         }
         foo.push_back(QPair<int, QString>(id, name));
      }
   }
   return foo;
}

void RvDetectionReport::SensorDetectionModel::SetTarget(int aPlatform)
{
   mArray                                                      = nullptr;
   const rv::ResultPlatform::DetectionAttemptMap&          map = mPlatform->GetDetectionAttemptMap();
   rv::ResultPlatform::DetectionAttemptMap::const_iterator it  = map.find(mSensor.toStdString());
   if (it != map.end())
   {
      std::map<int, rv::ResultPlatform::DetectionAttemptArray>::const_iterator jt = it->second.find(aPlatform);
      if (jt != it->second.end())
      {
         beginResetModel();
         mArray = &jt->second;
         endResetModel();
      }
   }
}

void RvDetectionReport::SensorDetectionModel::SetCacheRange(float aStart, float aEnd)
{
   float start = mArray->GetMinTime();
   float end   = mArray->GetMaxTime();
   if ((start != mStart) || (end != mEnd))
   {
      mStart = start;
      mEnd   = end;
      beginResetModel();
      endResetModel();
   }
}
