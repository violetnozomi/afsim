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

#include "AeroData.hpp"

#include <cmath>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>

#include "MoverCreatorMainWindow.hpp"
#include "UtIterator.hpp"
#include "UtMath.hpp"
#include "UtTableOperations.hpp"

namespace Designer
{

AeroData::AeroData()
{
}

bool AeroData::LoadFromFile(QString aFileName, QString aSurfaceName)
{
   mCurrentAirfoilFilename = aFileName.toStdString();
   mSurfaceName            = aSurfaceName.toStdString();

   // Load the control points from the currently selected airfoil file

   // Note that airfoil data must be in the "Airfoils" folder
   QString dirPath = MoverCreatorMainWindow::GetDataPath() + "/Airfoils/";
   QDir listDir(dirPath);
   if (listDir.exists())
   {
      QString airFoilFileName = aFileName + ".foil";
      if (listDir.exists(airFoilFileName))
      {
         airFoilFileName = dirPath + airFoilFileName;
         QFile jFile(airFoilFileName);
         if (jFile.open(QIODevice::ReadOnly))
         {
            QByteArray fileData = jFile.readAll();
            mCurrentAirfoilFile = std::make_shared<QJsonDocument>(QJsonDocument::fromJson(fileData));

            ProcessAeroData();
            return true;
         }
      }
   }
   return false;
}

bool AeroData::ProcessAeroData()
{
   if (mCurrentAirfoilFile != nullptr)
   {
      QJsonObject mainObj          = mCurrentAirfoilFile->object();

      QJsonObject liftObject       = mainObj["Cl_Alpha"].toObject();
      QJsonArray  liftAlphaArray   = liftObject["Alpha"].toArray();
      QJsonArray  liftArray        = liftObject["Lift"].toArray();

      QJsonObject dragObject       = mainObj["Cd_Alpha"].toObject();
      QJsonArray  dragAlphaArray   = dragObject["Alpha"].toArray();
      QJsonArray  dragArray        = dragObject["Drag"].toArray();

      QJsonObject momentObject     = mainObj["Cm_Alpha"].toObject();
      QJsonArray  momentAlphaArray = momentObject["Alpha"].toArray();
      QJsonArray  momentArray      = momentObject["Moment"].toArray();

      return ProcessLiftData(liftAlphaArray, liftArray) &&
             ProcessDragData(dragAlphaArray, dragArray) &&
             ProcessMomentData(momentAlphaArray, momentArray);
   }
   return false;
}

void AeroData::CalculateNominalData(double aAspectRatio, double aSweepAngle_deg)
{
   double deltaLeft, deltaRight, stallLeft, stallRight;

   CalculateNominalLift(aAspectRatio, aSweepAngle_deg);

   CalculateAlphaChange(aSweepAngle_deg, deltaLeft, deltaRight, stallLeft, stallRight);

   CalculateNominalDrag(aSweepAngle_deg, deltaLeft, deltaRight, stallLeft, stallRight);

   CalculateNominalMoment(aSweepAngle_deg, deltaLeft, deltaRight, stallLeft, stallRight);

   CalcLiftDataCharacteristics(aSweepAngle_deg);
}

QStringList Designer::AeroData::Audit()
{
   return AuditLift() + AuditDrag() + AuditMoment();
}

QStringList Designer::AeroData::AuditLift()
{
   double alphaMin = 0;
   double alphaMax = 0;
   return AuditLift(alphaMin, alphaMax);
}

QStringList Designer::AeroData::AuditLift(double& aAlphaMin, double& aAlphaMax)
{
   bool foundPeakPos = false;
   bool foundPeakNeg = false;

   QStringList errors;
   if (mRawAirfoil.find(DataType::cLIFT) != mRawAirfoil.end())
   {
      if (!(mRawAirfoil.at(DataType::cLIFT).second.size() < 2))
      {
         // Iterate over positive half
         std::pair<double, double> peakPos;
         auto prevLiftData0 = mRawAirfoil.at(DataType::cLIFT).second.begin();
         auto it_r0 = std::next(mRawAirfoil.at(DataType::cLIFT).second.begin(), 1);
         for (; it_r0 != mRawAirfoil.at(DataType::cLIFT).second.end(); ++it_r0)
         {
            peakPos = *prevLiftData0;

            double a = it_r0->first;
            double l = it_r0->second;

            double localSlope = util::CalcSlope(UtMath::cRAD_PER_DEG * peakPos.first,
                                                UtMath::cRAD_PER_DEG * a,
                                                peakPos.second,
                                                l);

            if (localSlope >= 0)
            {
               prevLiftData0 = it_r0;
            }
            else
            {
               foundPeakPos = true;
               aAlphaMax = a;
               break;
            }
         }
      }
      if (!(mRawAirfoil.at(DataType::cLIFT).first.size() < 2))
      {
         // Iterate over negative half
         std::pair<double, double> peakNeg;
         auto prevLiftData = mRawAirfoil.at(DataType::cLIFT).first.rbegin();
         auto it_l0 = std::next(mRawAirfoil.at(DataType::cLIFT).first.rbegin(), 1);
         for (; it_l0 != mRawAirfoil.at(DataType::cLIFT).first.rend(); ++it_l0)
         {
            peakNeg = *prevLiftData;

            double a = it_l0->first;
            double l = it_l0->second;

            double localSlope = util::CalcSlope(UtMath::cRAD_PER_DEG * peakNeg.first,
                                                UtMath::cRAD_PER_DEG * a,
                                                peakNeg.second,
                                                l);

            if (localSlope >= 0)
            {
               prevLiftData = it_l0;
            }
            else
            {
               foundPeakNeg = true;
               aAlphaMin = a;
               break;
            }
         }
      }
   }

   if (!foundPeakPos)
   {
      QString message = QString::fromStdString("Airfoil file ") +
                        QString::fromStdString(mCurrentAirfoilFilename) +
                        QString::fromStdString(" on surface ") +
                        QString::fromStdString(mSurfaceName) +
                        QString::fromStdString(" is invalid. Lift data must contain at least one data point beyond the post-stall maxima for positive alpha");
      errors.push_back(message);
   }
   if (!foundPeakNeg)
   {
      QString message = QString::fromStdString("Airfoil file ") +
                        QString::fromStdString(mCurrentAirfoilFilename) +
                        QString::fromStdString(" on surface ") +
                        QString::fromStdString(mSurfaceName) +
                        QString::fromStdString(" is invalid. Lift data must contain at least one data point beyond the post-stall maxima for negative alpha");
      errors.push_back(message);
   }
   return errors;
}

QStringList Designer::AeroData::AuditDrag()
{
   QStringList errors;
   double alphaMin = 0.0;
   double alphaMax = 0.0;

   if (AuditLift(alphaMin, alphaMax).empty())
   {
      bool minValid = false;
      bool maxValid = false;

      // verify that mRawAirfoil covers drag data in the range [alphaMin, alphaMax]
      if (mRawAirfoil.find(DataType::cDRAG) != mRawAirfoil.end())
      {
         if (!mRawAirfoil.at(DataType::cDRAG).first.empty())
         {
            double aMin = mRawAirfoil.at(DataType::cDRAG).first.begin()->first;
            if (aMin <= alphaMin)
            {
               minValid = true;
            }
         }
         if (!mRawAirfoil.at(DataType::cDRAG).second.empty())
         {
            double aMax = mRawAirfoil.at(DataType::cDRAG).second.rbegin()->first;
            if (aMax >= alphaMax)
            {
               maxValid = true;
            }
         }
      }
      if (!minValid)
      {
         QString message = QString::fromStdString("Airfoil file ") +
                           QString::fromStdString(mCurrentAirfoilFilename) +
                           QString::fromStdString(" on surface ") +
                           QString::fromStdString(mSurfaceName) +
                           QString::fromStdString(" is invalid. Drag data must contain at least one data point beyond the post-stall maxima for positive alpha");
         errors.push_back(message);
      }
      if (!maxValid)
      {
         QString message = QString::fromStdString("Airfoil file ") +
                           QString::fromStdString(mCurrentAirfoilFilename) +
                           QString::fromStdString(" on surface ") +
                           QString::fromStdString(mSurfaceName) +
                           QString::fromStdString(" is invalid. Drag data must contain at least one data point beyond the post-stall maxima for negative alpha");
         errors.push_back(message);
      }
   }

   return errors;
}

QStringList Designer::AeroData::AuditMoment()
{
   QStringList errors;
   double alphaMin = 0.0;
   double alphaMax = 0.0;

   if (AuditLift(alphaMin, alphaMax).empty())
   {
      bool minValid = false;
      bool maxValid = false;

      // Verify that mRawAirfoil covers moment data in the range [alphaMin, alphaMax]
      if (mRawAirfoil.find(DataType::cMOMENT) != mRawAirfoil.end())
      {
         if (!mRawAirfoil.at(DataType::cMOMENT).first.empty())
         {
            double aMin = mRawAirfoil.at(DataType::cMOMENT).first.begin()->first;
            if (aMin <= alphaMin)
            {
               minValid = true;
            }
         }
         if (!mRawAirfoil.at(DataType::cMOMENT).second.empty())
         {
            double aMax = mRawAirfoil.at(DataType::cMOMENT).second.rbegin()->first;
            if (aMax >= alphaMax)
            {
               maxValid = true;
            }
         }
      }
      if (!minValid)
      {
         QString message = QString::fromStdString("Airfoil file ") +
                           QString::fromStdString(mCurrentAirfoilFilename) +
                           QString::fromStdString(" on surface ") +
                           QString::fromStdString(mSurfaceName) +
                           QString::fromStdString(" is invalid. Moment data must contain at least one data point beyond the post-stall maxima for positive alpha");
         errors.push_back(message);
      }
      if (!maxValid)
      {
         QString message = QString::fromStdString("Airfoil file ") +
                           QString::fromStdString(mCurrentAirfoilFilename) +
                           QString::fromStdString(" on surface ") +
                           QString::fromStdString(mSurfaceName) +
                           QString::fromStdString(" is invalid. Moment data must contain at least one data point beyond the post-stall maxima for negative alpha");
         errors.push_back(message);
      }
   }

   return errors;
}

void AeroData::CalculateNominalLift(double aAspectRatio, double aSweepAngle_deg)
{
   double sweep_rad      = aSweepAngle_deg * UtMath::cRAD_PER_DEG;
   double liftMultiplier = std::pow(std::cos(sweep_rad), 0.5);
   if (aAspectRatio != 0)
   {
      double lift0 = mRawAirfoil[DataType::cLIFT].first.rbegin()->second;
      // Left half
      {
         auto prevLiftData = mRawAirfoil[DataType::cLIFT].first.rbegin();
         mNominalData[DataType::cLIFT][aSweepAngle_deg].first.emplace(prevLiftData->first, prevLiftData->second * liftMultiplier);
         double prevNominalAlpha = UtMath::cRAD_PER_DEG * prevLiftData->first; // radians
         double nominalAlpha     = prevNominalAlpha; // radians

         for (auto it_l = std::next(mRawAirfoil[DataType::cLIFT].first.rbegin(), 1); it_l != mRawAirfoil[DataType::cLIFT].first.rend(); ++it_l)
         {
            double a0 = prevLiftData->first;
            double l0 = prevLiftData->second;

            double a = it_l->first;
            double l = it_l->second;

            double rawSlope = util::CalcSlope(UtMath::cRAD_PER_DEG * a0, UtMath::cRAD_PER_DEG * a, l0, l);

            double den = sqrt(1 + ((rawSlope * cos(sweep_rad)) / (UtMath::cPI * aAspectRatio)) * ((rawSlope * cos(sweep_rad)) / (UtMath::cPI * aAspectRatio)));
            double num = rawSlope * cos(sweep_rad);

            if (num != 0)
            {
               double nominalLiftSlope = num / den;
               nominalAlpha = prevNominalAlpha + (l - l0) / nominalLiftSlope;
            }

            if (std::abs(l - lift0) >  std::abs((mAirfoilCLMaxLeft - lift0) * sin(2 * nominalAlpha)))
            {
               mNominalData[DataType::cLIFT][aSweepAngle_deg].first.emplace(UtMath::cDEG_PER_RAD * nominalAlpha, l * liftMultiplier);
               prevLiftData = it_l;
               prevNominalAlpha = nominalAlpha;
            }
            else
            {
               break;
            }
         }

         int a_l = std::floor(UtMath::cDEG_PER_RAD * prevNominalAlpha);
         for (; a_l >= -180; a_l -= 5)
         {
            mNominalData[DataType::cLIFT][aSweepAngle_deg].first[a_l] = (mAirfoilCLMaxLeft * liftMultiplier + lift0) * sin(2 * a_l * UtMath::cRAD_PER_DEG) + lift0;
         }
         if ((a_l + 5) > -180)
         {
            a_l = -180;
            mNominalData[DataType::cLIFT][aSweepAngle_deg].first[a_l] = (mAirfoilCLMaxLeft * liftMultiplier + lift0) * sin(2 * a_l * UtMath::cRAD_PER_DEG) + lift0;
         }
      }

      // Right half
      {
         auto prevLiftData = mRawAirfoil[DataType::cLIFT].second.begin();
         mNominalData[DataType::cLIFT][aSweepAngle_deg].second.emplace(prevLiftData->first, prevLiftData->second * liftMultiplier);
         double prevNominalAlpha = UtMath::cRAD_PER_DEG * prevLiftData->first;
         double nominalAlpha     = prevNominalAlpha;

         for (auto it_r = std::next(mRawAirfoil[DataType::cLIFT].second.begin(), 1); it_r != mRawAirfoil[DataType::cLIFT].second.end(); ++it_r)
         {
            double a0 = prevLiftData->first;
            double l0 = prevLiftData->second;

            double a = it_r->first;
            double l = it_r->second;

            double rawSlope = util::CalcSlope(UtMath::cRAD_PER_DEG * a0, UtMath::cRAD_PER_DEG * a, l0, l);

            double den = sqrt(1 + ((rawSlope * cos(sweep_rad)) / (UtMath::cPI * aAspectRatio)) * ((rawSlope * cos(sweep_rad)) / (UtMath::cPI * aAspectRatio)));
            double num = rawSlope * cos(sweep_rad);

            if (num != 0)
            {
               double nominalLiftSlope = num / den;
               nominalAlpha = prevNominalAlpha + (l - l0) / nominalLiftSlope;
            }

            if (std::abs(l - lift0) > std::abs((mAirfoilCLMaxRight - lift0) * sin(2 * nominalAlpha)))
            {
               mNominalData[DataType::cLIFT][aSweepAngle_deg].second.emplace(UtMath::cDEG_PER_RAD * nominalAlpha, l * liftMultiplier);
               prevLiftData = it_r;
               prevNominalAlpha = nominalAlpha;
            }
            else
            {
               break;
            }
         }

         int a_r = std::ceil(UtMath::cDEG_PER_RAD * prevNominalAlpha);
         for (; a_r <= 180; a_r += 5)
         {
            mNominalData[DataType::cLIFT][aSweepAngle_deg].second[a_r] = ((mAirfoilCLMaxRight * liftMultiplier - lift0) * sin(2 * a_r * UtMath::cRAD_PER_DEG) + lift0);
         }
         if ((a_r - 5) < 180)
         {
            a_r = 180;
            mNominalData[DataType::cLIFT][aSweepAngle_deg].second[a_r] = ((mAirfoilCLMaxRight * liftMultiplier - lift0) * sin(2 * a_r * UtMath::cRAD_PER_DEG) + lift0);
         }
      }
   }
   else
   {
      mNominalData[DataType::cLIFT][aSweepAngle_deg] = mAirfoilData[DataType::cLIFT];
   }
}

void AeroData::CalculateNominalDrag(double aSweepAngle_deg,
                                    double aDeltaAlphaLeft,
                                    double aDeltaAlphaRight,
                                    double aAlphaStallLeft,
                                    double aAlphaStallRight)
{
   {
      const double alphaPeak = -90;

      auto it_stall = ut::make_reverse_iterator(mAirfoilData[DataType::cDRAG].first.lower_bound(aAlphaStallLeft + aDeltaAlphaLeft));
      if (it_stall != mAirfoilData[DataType::cDRAG].first.rbegin())
      {
         int prestallCount   = 1 + std::abs((mAirfoilData[DataType::cDRAG].first.rbegin()->first, it_stall->first));
         double alphaPerStep = aDeltaAlphaLeft / prestallCount;

         // Pre-stall
         auto it = mAirfoilData[DataType::cDRAG].first.rbegin();
         for (; it != it_stall; ++it)
         {
            double alpha = it->first + it->first * alphaPerStep;
            double drag = it->second;
            mNominalData[DataType::cDRAG][aSweepAngle_deg].first.emplace(alpha, drag);
         }

         // Post-stall to peak
         ++it;
         int poststallCount = std::abs(it.base()->first - alphaPeak);
         for (; (it.base()->first >= alphaPeak); ++it)
         {
            auto iter = it.base();
            double rate = (aDeltaAlphaLeft / poststallCount);
            double alpha = iter->first + (alphaPeak - iter->first) * rate;
            double drag = iter->second;
            mNominalData[DataType::cDRAG][aSweepAngle_deg].first.emplace(alpha, drag);
         }

         // Post-peak
         for (; it != mAirfoilData[DataType::cDRAG].first.rend(); ++it)
         {
            auto iter = it.base();
            mNominalData[DataType::cDRAG][aSweepAngle_deg].first.emplace(iter->first, iter->second);
         }
         mNominalData[DataType::cDRAG][aSweepAngle_deg].first[-180] = mAirfoilData[DataType::cDRAG].first[-180];
      }
   }

   {
      const double alphaPeak = 90;

      auto it_stall = mAirfoilData[DataType::cDRAG].second.lower_bound(aAlphaStallRight - aDeltaAlphaRight);
      auto it_peak  = mAirfoilData[DataType::cDRAG].second.lower_bound(alphaPeak);

      if (it_stall->first != (aAlphaStallRight - aDeltaAlphaRight))
      {
         it_stall = std::prev(it_stall, 1);
      }
      if (it_peak->first != (alphaPeak))
      {
         it_peak = std::prev(it_peak, 1);
      }

      if (it_stall != mAirfoilData[DataType::cDRAG].second.end())
      {
         int prestallCount   = 1 + std::abs(mAirfoilData[DataType::cDRAG].second.begin()->first - it_stall->first);
         double alphaPerStep = aDeltaAlphaRight / prestallCount;

         // Pre-stall
         auto it = mAirfoilData[DataType::cDRAG].second.begin();
         for (; it != std::next(it_stall, 1); ++it)
         {
            double alpha = it->first + it->first * alphaPerStep;
            double drag = it->second;
            mNominalData[DataType::cDRAG][aSweepAngle_deg].second.emplace(alpha, drag);
         }

         // Post-stall to peak
         int poststallCount = std::abs(it->first - alphaPeak);
         for (; (it->first <= alphaPeak); ++it)
         {
            double rate = (aDeltaAlphaRight / poststallCount);
            double alpha = it->first + (alphaPeak - it->first) * rate;
            double drag = it->second;
            mNominalData[DataType::cDRAG][aSweepAngle_deg].second.emplace(alpha, drag);
         }

         // Post-peak
         for (; it != mAirfoilData[DataType::cDRAG].second.end(); ++it)
         {
            mNominalData[DataType::cDRAG][aSweepAngle_deg].second.emplace(it->first, it->second);
         }
         mNominalData[DataType::cDRAG][aSweepAngle_deg].second[180] = mAirfoilData[DataType::cDRAG].second[180];
      }
   }
}

void AeroData::CalculateNominalMoment(double aSweepAngle_deg,
                                      double aDeltaAlphaLeft,
                                      double aDeltaAlphaRight,
                                      double aAlphaStallLeft,
                                      double aAlphaStallRight)
{
   {
      auto it_stall = ut::make_reverse_iterator(mAirfoilData[DataType::cMOMENT].first.upper_bound(aAlphaStallLeft));

      if (it_stall != mAirfoilData[DataType::cMOMENT].first.rbegin())
      {
         int prestallCount   = 1 + std::abs((mAirfoilData[DataType::cMOMENT].first.rbegin()->first, it_stall->first));
         double alphaPerStep = aDeltaAlphaLeft / prestallCount;

         // Pre-stall
         auto it = mAirfoilData[DataType::cMOMENT].first.rbegin();
         double alpha = 0.0;
         for (; it != std::next(it_stall, 1); ++it)
         {
            alpha = it->first + it->first * alphaPerStep;
            double drag = it->second;
            mNominalData[DataType::cMOMENT][aSweepAngle_deg].first.emplace(alpha, drag);
         }

         // Post-stall
         for (; it != mAirfoilData[DataType::cMOMENT].first.rend(); ++it)
         {
            auto iter = it.base();
            if (iter->first < alpha)
            {
               mNominalData[DataType::cMOMENT][aSweepAngle_deg].first.emplace(iter->first, iter->second);
            }
         }
         mNominalData[DataType::cMOMENT][aSweepAngle_deg].first[-180] = mAirfoilData[DataType::cMOMENT].first[-180];
      }
   }

   {
      auto it_stall = mAirfoilData[DataType::cMOMENT].second.lower_bound(aAlphaStallRight);
      if (it_stall != mAirfoilData[DataType::cMOMENT].second.end())
      {
         int prestallCount   = 1 + std::abs(mAirfoilData[DataType::cMOMENT].second.begin()->first - it_stall->first);
         double alphaPerStep = aDeltaAlphaRight / prestallCount;

         // Pre-stall
         auto it = mAirfoilData[DataType::cMOMENT].second.begin();
         double alpha = 0.0;
         for (; it != std::next(it_stall, 1); ++it)
         {
            alpha = it->first + it->first * alphaPerStep;
            double drag = it->second;
            mNominalData[DataType::cMOMENT][aSweepAngle_deg].second.emplace(alpha, drag);
         }

         // Post-stall
         for (; it != mAirfoilData[DataType::cMOMENT].second.end(); ++it)
         {
            if (it->first > alpha)
            {
               mNominalData[DataType::cMOMENT][aSweepAngle_deg].second.emplace(it->first, it->second);
            }
         }
         mNominalData[DataType::cMOMENT][aSweepAngle_deg].second[180] = mAirfoilData[DataType::cMOMENT].second[180];
      }
   }
}

void AeroData::CalculateAlphaChange(double  aSweepAngle_deg,
                                    double& aDeltaAlphaLeft,
                                    double& aDeltaAlphaRight,
                                    double& aAlphaStallLeft,
                                    double& aAlphaStallRight)
{
   {
      // Find the stall point of the original airfoil data
      auto prevLiftData = mAirfoilData[DataType::cLIFT].first.rbegin();
      double prevSlope  = 0.0;

      double a0    = prevLiftData->first;
      double l0    = prevLiftData->second;
      double alpha = prevLiftData->first;

      for (auto it_l = std::next(mAirfoilData[DataType::cLIFT].first.rbegin(), 1); it_l != mAirfoilData[DataType::cLIFT].first.rend(); ++it_l)
      {
         double a = it_l->first;
         double l = it_l->second;

         double slope = util::CalcSlope(UtMath::cRAD_PER_DEG * a0, UtMath::cRAD_PER_DEG * a, l0, l);

         if (prevSlope > 0 && slope < 0)
         {
            alpha = a0;
            break;
         }

         a0 = a;
         l0 = l;

         prevSlope = slope;
      }

      // Find the stall point of the nominal data
      auto prevLiftDataNom = mNominalData[DataType::cLIFT][aSweepAngle_deg].first.rbegin();
      double prevSlopeNom  = 0.0;

      a0 = prevLiftDataNom->first;
      l0 = prevLiftDataNom->second;
      double alphaNominal = prevLiftDataNom->first;
      for (auto it_l = std::next(mNominalData[DataType::cLIFT][aSweepAngle_deg].first.rbegin(), 1); it_l != mNominalData[DataType::cLIFT][aSweepAngle_deg].first.rend(); ++it_l)
      {
         double a = it_l->first;
         double l = it_l->second;

         double slope = util::CalcSlope(UtMath::cRAD_PER_DEG * a0, UtMath::cRAD_PER_DEG * a, l0, l);

         if (prevSlopeNom > 0 && slope < 0)
         {
            alphaNominal = a0;
            break;
         }

         a0 = a;
         l0 = l;

         prevSlopeNom = slope;
      }

      aDeltaAlphaLeft = std::abs(alphaNominal - alpha);
      aAlphaStallLeft = alphaNominal;
   }

   {
      // Find the stall point of the original airfoil data
      auto prevLiftData = mAirfoilData[DataType::cLIFT].second.begin();
      double prevSlope  = 0.0;

      double a0    = prevLiftData->first;
      double l0    = prevLiftData->second;
      double alpha = prevLiftData->first;

      for (auto it_r = std::next(mAirfoilData[DataType::cLIFT].second.begin(), 1); it_r != mAirfoilData[DataType::cLIFT].second.end(); ++it_r)
      {
         double a = it_r->first;
         double l = it_r->second;

         double slope = util::CalcSlope(UtMath::cRAD_PER_DEG * a0, UtMath::cRAD_PER_DEG * a, l0, l);

         if (prevSlope > 0 && slope < 0)
         {
            alpha = a0;
            break;
         }

         a0 = a;
         l0 = l;

         prevSlope = slope;
      }

      // Find the stall point of the nominal data
      auto prevLiftDataNom = mNominalData[DataType::cLIFT][aSweepAngle_deg].second.begin();
      double prevSlopeNom  = 0.0;

      a0 = prevLiftDataNom->first;
      l0 = prevLiftDataNom->second;
      double alphaNominal = prevLiftDataNom->first;
      for (auto it_r = std::next(mNominalData[DataType::cLIFT][aSweepAngle_deg].second.begin(), 1); it_r != mNominalData[DataType::cLIFT][aSweepAngle_deg].second.end(); ++it_r)
      {
         double a = it_r->first;
         double l = it_r->second;

         double slope = util::CalcSlope(UtMath::cRAD_PER_DEG * a0, UtMath::cRAD_PER_DEG * a, l0, l);

         if (prevSlopeNom > 0 && slope < 0)
         {
            alphaNominal = a0;
            break;
         }

         a0 = a;
         l0 = l;

         prevSlopeNom = slope;
      }

      aDeltaAlphaRight = std::abs(alphaNominal - alpha);
      aAlphaStallRight = alphaNominal;
   }
}

bool AeroData::ProcessLiftData(QJsonArray aAlpha, QJsonArray aLift)
{
   // ---------------------------------------------------------------------
   // Put the raw data from the JSON structure in the mRawAirfoil structure
   // ---------------------------------------------------------------------
   if (aAlpha.size() == aLift.size())
   {
      for (int i = 0; i < aAlpha.size(); ++i)
      {
         double alpha = aAlpha.at(i).toDouble();
         double lift  = aLift.at(i).toDouble();

         if (lift > mAirfoilCLMaxRight)
         {
            mAirfoilCLMaxRight = lift;
         }
         else if (lift < mAirfoilCLMaxLeft)
         {
            mAirfoilCLMaxLeft = lift;
         }

         if (alpha > -1 * std::numeric_limits<double>::epsilon() &&
             alpha < std::numeric_limits<double>::epsilon())
         {
            mRawAirfoil[DataType::cLIFT].first.emplace(alpha, lift);
            mRawAirfoil[DataType::cLIFT].second.emplace(alpha, lift);
         }
         else if (alpha > std::numeric_limits<double>::epsilon())
         {
            mRawAirfoil[DataType::cLIFT].second.emplace(alpha, lift);
         }
         else if (alpha < std::numeric_limits<double>::epsilon())
         {
            mRawAirfoil[DataType::cLIFT].first.emplace(alpha, lift);
         }
      }
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("Alpha and Lift arrays must be the same length!");
      return false;
   }

   mAirfoilCLMaxLeft = std::abs(mAirfoilCLMaxLeft);
   mAirfoilCLMaxRight = std::abs(mAirfoilCLMaxRight);

   if (!AuditLift().empty())
   {
      return false;
   }
   // -----------------------------------------------------------------------------------
   // Fill in any gaps between the last point in the mRawAirfoil and the nominal sin wave
   // -----------------------------------------------------------------------------------

   // Left side
   bool useNominal = false;

   // Reverse iterate over lift values - go from 0 to min alpha
   auto l0 = mRawAirfoil[DataType::cLIFT].first.rbegin();

   double a_lprev = l0->first;
   double l_lprev = l0->second;
   double a_l     = a_lprev;
   double l_l     = l_lprev;
   double slope_l = 0;

   mAirfoilData[DataType::cLIFT].first[a_lprev] = l_lprev;
   auto l = std::next(mRawAirfoil.at(DataType::cLIFT).first.rbegin(), 1);
   for (; l != mRawAirfoil.at(DataType::cLIFT).first.rend(); ++l)
   {
      a_l = l->first;
      l_l = l->second;

      // search for negative slope, then start comparing to sin function
      slope_l = util::CalcSlope(a_lprev * UtMath::cRAD_PER_DEG,
                                a_l * UtMath::cRAD_PER_DEG,
                                l_lprev,
                                l_l);

      if (slope_l < 0)
      {
         if (l_l >= std::sin(2 * a_l * UtMath::cRAD_PER_DEG))
         {
            useNominal = true;
            break;
         }
      }
      mAirfoilData[DataType::cLIFT].first[a_l] = l_l;
      a_lprev = l->first;
      l_lprev = l->second;
   }

   // If data does not reach sin curve, linearly extrapolate
   if (!useNominal)
   {
      while (l_l < std::sin(2 * a_l * UtMath::cRAD_PER_DEG))
      {
         a_l -= 1;
         l_l = l_l - slope_l * 1 * UtMath::cRAD_PER_DEG;
         mAirfoilData[DataType::cLIFT].first[a_l] = l_l;
         if (a_l <= -180) { break; }
      }
   }

   // Right side
   useNominal = false;

   // Iterate over lift values - go from 0 to max alpha
   auto r0 = mRawAirfoil[DataType::cLIFT].second.begin();

   double a_rprev = r0->first;
   double l_rprev = r0->second;
   double a_r     = a_rprev;
   double l_r     = l_rprev;
   double slope_r = 0;

   mAirfoilData[DataType::cLIFT].second[a_rprev] = l_rprev;
   auto r = std::next(mRawAirfoil.at(DataType::cLIFT).second.begin(), 1);
   for (; r != mRawAirfoil.at(DataType::cLIFT).second.end(); ++r)
   {
      a_r = r->first;
      l_r = r->second;

      // search for negative slope, then start comparing to sin function
      slope_r = util::CalcSlope(a_rprev * UtMath::cRAD_PER_DEG,
                                a_r * UtMath::cRAD_PER_DEG,
                                l_rprev,
                                l_r);
      if (slope_r < 0)
      {
         if (l_r <= std::sin(2 * a_r * UtMath::cRAD_PER_DEG))
         {
            useNominal = true;
            break;
         }
      }
      mAirfoilData[DataType::cLIFT].second[a_r] = l_r;
      a_rprev = r->first;
      l_rprev = r->second;
   }

   // If data does not reach sin curve, linearly extrapolate
   if (!useNominal)
   {
      while (l_r > std::sin(2 * a_r * UtMath::cRAD_PER_DEG))
      {
         a_r += 1;
         l_r = l_r + slope_r * 1 * UtMath::cRAD_PER_DEG;
         mAirfoilData[DataType::cLIFT].second[a_r] = l_r;
         if (a_r >= 180) { break; } // to prevent infinite loop, just in case
      }
   }

   // ---------------------------------------------
   // Set the deep stall lift values to sin(2*alpha)
   // ---------------------------------------------

   // Left side: loop until -180 deg
   for (double a = a_l; a > -180; a -= 5)
   {
      mAirfoilData[DataType::cLIFT].first[a] = std::sin(2 * a * UtMath::cRAD_PER_DEG);
   }

   // Right side: loop until 180 deg
   for (double a = a_r; a < 180; a += 5)
   {
      mAirfoilData[DataType::cLIFT].second[a] = std::sin(2 * a * UtMath::cRAD_PER_DEG);
   }

   // Add +/- 180 as the last points
   mAirfoilData[DataType::cLIFT].first[-180] = 0;
   mAirfoilData[DataType::cLIFT].second[180] = 0;

   return true;
}

bool AeroData::ProcessDragData(QJsonArray aAlpha, QJsonArray aDrag)
{
   // ---------------------------------------------------------------------
   // Put the raw data from the JSON structure in the mRawAirfoil structure
   // ---------------------------------------------------------------------

   if (aAlpha.size() == aDrag.size())
   {
      for (int i = 0; i < aAlpha.size(); ++i)
      {
         double alpha = aAlpha.at(i).toDouble();
         double drag  = aDrag.at(i).toDouble();

         if (alpha > -1 * std::numeric_limits<double>::epsilon() &&
             alpha < std::numeric_limits<double>::epsilon())
         {
            mRawAirfoil[DataType::cDRAG].first.emplace(alpha, drag);
            mRawAirfoil[DataType::cDRAG].second.emplace(alpha, drag);
            mAirfoilData[DataType::cDRAG].first.emplace(alpha, drag);
            mAirfoilData[DataType::cDRAG].second.emplace(alpha, drag);
         }
         else if (alpha > std::numeric_limits<double>::epsilon())
         {
            mRawAirfoil[DataType::cDRAG].second.emplace(alpha, drag);
            mAirfoilData[DataType::cDRAG].second.emplace(alpha, drag);
         }
         else if (alpha < std::numeric_limits<double>::epsilon())
         {
            mRawAirfoil[DataType::cDRAG].first.emplace(alpha, drag);
            mAirfoilData[DataType::cDRAG].first.emplace(alpha, drag);
         }
      }
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("Alpha and Drag arrays must be the same length!");
      return false;
   }

   if (!AuditDrag().empty())
   {
      return false;
   }

   // -------------------------------------------------------
   // Append the nominal cos wave to left half and right half
   // -------------------------------------------------------

   // Left half
   int a_l = mRawAirfoil[DataType::cDRAG].first.begin()->first - 5;
   for (; a_l > -180; a_l -= 5)
   {
      mAirfoilData[DataType::cDRAG].first[a_l] = 0.9 * (1 - cos(2 * a_l * UtMath::cRAD_PER_DEG));
   }
   mAirfoilData[DataType::cDRAG].first[-180] = 0;

   // Right half
   int a_r = mRawAirfoil[DataType::cDRAG].second.rbegin()->first + 5;
   for (; a_r <= 180; a_r += 5)
   {
      mAirfoilData[DataType::cDRAG].second[a_r] = 0.9 * (1 - cos(2 * a_r * UtMath::cRAD_PER_DEG));
   }
   mAirfoilData[DataType::cDRAG].second[180] = 0;

   return true;
}

bool AeroData::ProcessMomentData(QJsonArray aAlpha, QJsonArray aMoment)
{
   // ---------------------------------------------------------------------
   // Put the raw data from the JSON structure in the mRawAirfoil structure
   // ---------------------------------------------------------------------

   double maxMoment = 0;

   if (aAlpha.size() == aMoment.size())
   {
      for (int i = 0; i < aAlpha.size(); ++i)
      {
         double alpha  = aAlpha.at(i).toDouble();
         double moment = aMoment.at(i).toDouble();
         if (std::abs(moment) > maxMoment)
         {
            maxMoment = std::abs(moment);
         }
         if (alpha > -1 * std::numeric_limits<double>::epsilon() &&
             alpha < std::numeric_limits<double>::epsilon())
         {
            mRawAirfoil[DataType::cMOMENT].first.emplace(alpha, moment);
            mRawAirfoil[DataType::cMOMENT].second.emplace(alpha, moment);
            mAirfoilData[DataType::cMOMENT].first.emplace(alpha, moment);
            mAirfoilData[DataType::cMOMENT].second.emplace(alpha, moment);
         }
         else if (alpha > std::numeric_limits<double>::epsilon())
         {
            mRawAirfoil[DataType::cMOMENT].second.emplace(alpha, moment);
            mAirfoilData[DataType::cMOMENT].second.emplace(alpha, moment);
         }
         else if (alpha < std::numeric_limits<double>::epsilon())
         {
            mRawAirfoil[DataType::cMOMENT].first.emplace(alpha, moment);
            mAirfoilData[DataType::cMOMENT].first.emplace(alpha, moment);
         }
      }
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("Alpha and Moment arrays must be the same length!");
      return false;
   }

   if (!AuditMoment().empty())
   {
      return false;
   }

   // -------------------------------------------------------
   // Append the nominal sin wave to left half and right half
   // -------------------------------------------------------

   // Left half
   int a_l = mRawAirfoil[DataType::cMOMENT].first.begin()->first - 5;
   for (; a_l > -180; a_l -= 5)
   {
      mAirfoilData[DataType::cMOMENT].first[a_l] = -maxMoment * sin(a_l * UtMath::cRAD_PER_DEG);
   }
   mAirfoilData[DataType::cMOMENT].first[-180] = 0;

   // Right half
   int a_r = mRawAirfoil[DataType::cMOMENT].second.rbegin()->first + 5;;
   for (; a_r < 180; a_r += 5)
   {
      mAirfoilData[DataType::cMOMENT].second[a_r] = -maxMoment * sin(a_r * UtMath::cRAD_PER_DEG);
   }
   mAirfoilData[DataType::cMOMENT].second[180] = 0;

   return true;
}

double AeroData::GetLiftCoeff(double aAlpha_deg, double aSweepAngle_deg, double aCordFraction, double aDeflectionAngle_deg) const
{
   double liftCoeff          = GetLiftCoeff(aAlpha_deg, aSweepAngle_deg);

   double peakAlphaNeg_deg   = GetNegLiftPeak(aSweepAngle_deg).first;
   double peakClNeg          = GetNegLiftPeak(aSweepAngle_deg).second;
   double peakAlphaPos_deg   = GetPosLiftPeak(aSweepAngle_deg).first;
   double peakClPos          = GetPosLiftPeak(aSweepAngle_deg).second;

   double valleyAlphaNeg_deg = GetNegLiftValley(aSweepAngle_deg).first;
   double valleyClNeg        = GetNegLiftValley(aSweepAngle_deg).second;
   double valleyAlphaPos_deg = GetPosLiftValley(aSweepAngle_deg).first;
   double valleyClPos        = GetPosLiftValley(aSweepAngle_deg).second;

   // Average slope of the pre-stall region
   double slope = GetPreStallSlope(aSweepAngle_deg);

   // Slope perpendicular to the pre-stall region
   double perpSlope = -1.0 / slope;

   // Formula to determine amount of offset. Perpendicular distance.
   double offset = 0.1 * sqrt(aCordFraction) * slope * std::sin(UtMath::cRAD_PER_DEG * aDeflectionAngle_deg);

   // Horizontal distance
   double dAlpha_rad = std::copysign(std::sqrt((offset * offset) / (1 + perpSlope * perpSlope)),
                                     -1 * std::sin(UtMath::cRAD_PER_DEG * aDeflectionAngle_deg));

   // Vertical distance
   double dCl = dAlpha_rad * perpSlope;

   // Calculate peak point for the shifted curve
   double peakAlphaNegNew_deg = peakAlphaNeg_deg + UtMath::cDEG_PER_RAD * dAlpha_rad;
   double peakClNegNew        = peakClNeg + dCl;
   double peakAlphaPosNew_deg = peakAlphaPos_deg + UtMath::cDEG_PER_RAD * dAlpha_rad;
   double peakClPosNew        = peakClPos + dCl;
   double midpointNew         = (peakAlphaPosNew_deg + peakAlphaNegNew_deg) / 2;

   if (aAlpha_deg >= midpointNew)
   {
      if (dAlpha_rad < 0) // left shift
      {
         if (peakAlphaPosNew_deg < valleyAlphaPos_deg)
         {
            if (std::abs(aAlpha_deg - peakAlphaPosNew_deg) < cEpsilon) // if alpha is at the prestall peak
            {
               return liftCoeff + dCl;
            }
            else if (aAlpha_deg < peakAlphaPosNew_deg) // if alpha is in the positive prestall region (before the peak)
            {
               return GetLiftCoeff(aAlpha_deg - UtMath::cDEG_PER_RAD * dAlpha_rad, aSweepAngle_deg) + dCl;
            }
            else if (aAlpha_deg < valleyAlphaPos_deg) // if alpha is in the positive poststall region (after the peak)
            {
               return UtMath::Lerp(UtMath::cRAD_PER_DEG * aAlpha_deg,
                                   UtMath::cRAD_PER_DEG * peakAlphaPosNew_deg,
                                   UtMath::cRAD_PER_DEG * valleyAlphaPos_deg,
                                   peakClPosNew,
                                   valleyClPos);
            }
         }
      }
      else // Right shift
      {
         double Cl0 = peakClPos - slope * dAlpha_rad;
         double Cl1 = Cl0 + slope * UtMath::cRAD_PER_DEG * (aAlpha_deg - peakAlphaPos_deg);
         return std::min(std::max(Cl1, peakClNegNew), liftCoeff);
      }
   }
   else // (aAlpha_deg < midpointNew)
   {
      if (dAlpha_rad < 0) // Left shift;
      {
         double Cl0 = peakClNeg - slope * dAlpha_rad;
         double Cl1 = Cl0 + slope * UtMath::cRAD_PER_DEG * (aAlpha_deg - peakAlphaNeg_deg);
         return std::max(std::min(Cl1, peakClPosNew), liftCoeff);
      }
      else // Right shift
      {
         if (peakAlphaNegNew_deg > valleyAlphaNeg_deg)
         {
            if (std::abs(aAlpha_deg - peakAlphaNegNew_deg) < cEpsilon) // If alpha is at the pre-stall peak
            {
               return liftCoeff + dCl;
            }

            if (aAlpha_deg > peakAlphaNegNew_deg) // If alpha is in the positive pre-stall region (before the peak)
            {
               return GetLiftCoeff(aAlpha_deg - UtMath::cDEG_PER_RAD * dAlpha_rad, aSweepAngle_deg) + dCl;
            }
            else if (aAlpha_deg > valleyAlphaNeg_deg) // If alpha is in the positive post-stall region (after the peak)
            {
               return UtMath::Lerp(UtMath::cRAD_PER_DEG * aAlpha_deg,
                                   UtMath::cRAD_PER_DEG * peakAlphaNegNew_deg,
                                   UtMath::cRAD_PER_DEG * valleyAlphaNeg_deg,
                                   peakClNegNew,
                                   valleyClNeg);
            }
         }
      }
   }
   return liftCoeff;
}

double AeroData::GetLiftCoeff(double aAlpha_deg, double aSweepAngle_deg) const
{
   return GetAeroData(AeroData::DataType::cLIFT, aAlpha_deg, aSweepAngle_deg);
}

double AeroData::GetDragCoeff(double aAlpha_deg, double aSweepAngle_deg) const
{
   return GetAeroData(AeroData::DataType::cDRAG, aAlpha_deg, aSweepAngle_deg);
}

double AeroData::GetPitchMomentCoeff(double aAlpha_deg, double aSweepAngle_deg) const
{
   return GetAeroData(AeroData::DataType::cMOMENT, aAlpha_deg, aSweepAngle_deg);
}

std::pair<double, double> Designer::AeroData::GetPosLiftPeak(double aSweepAngle_deg) const
{
   auto it = mLiftDataCharacteristics.peakPos.lower_bound(aSweepAngle_deg);
   if (it != mLiftDataCharacteristics.peakPos.end())
   {
      if (it->first != aSweepAngle_deg)
      {
         if (it != mLiftDataCharacteristics.peakPos.begin())
         {
            auto it_prev = std::prev(it, 1);

            double a = UtMath::Lerp(aSweepAngle_deg,
                                    it_prev->first,
                                    it->first,
                                    it_prev->second.first,
                                    it->second.first);

            double l = UtMath::Lerp(aSweepAngle_deg,
                                    it_prev->first,
                                    it->first,
                                    it_prev->second.second,
                                    it->second.second);

            return std::make_pair(a, l);
         }
      }
      return it->second;
   }
   return std::pair<double, double>();
}

std::pair<double, double> Designer::AeroData::GetNegLiftPeak(double aSweepAngle_deg) const
{
   auto it = mLiftDataCharacteristics.peakNeg.lower_bound(aSweepAngle_deg);
   if (it != mLiftDataCharacteristics.peakNeg.end())
   {
      if (it->first != aSweepAngle_deg)
      {
         if (it != mLiftDataCharacteristics.peakNeg.begin())
         {
            auto it_prev = std::prev(it, 1);

            double a = UtMath::Lerp(aSweepAngle_deg,
                                    it_prev->first,
                                    it->first,
                                    it_prev->second.first,
                                    it->second.first);

            double l = UtMath::Lerp(aSweepAngle_deg,
                                    it_prev->first,
                                    it->first,
                                    it_prev->second.second,
                                    it->second.second);

            return std::make_pair(a, l);
         }
      }
      return it->second;
   }
   return std::pair<double, double>();
}

std::pair<double, double> Designer::AeroData::GetPosLiftValley(double aSweepAngle_deg) const
{
   auto it = mLiftDataCharacteristics.valleyPos.lower_bound(aSweepAngle_deg);
   if (it != mLiftDataCharacteristics.valleyPos.end())
   {
      if (it->first != aSweepAngle_deg)
      {
         if (it != mLiftDataCharacteristics.valleyPos.begin())
         {
            auto it_prev = std::prev(it, 1);

            double a = UtMath::Lerp(aSweepAngle_deg,
                                    it_prev->first,
                                    it->first,
                                    it_prev->second.first,
                                    it->second.first);

            double l = UtMath::Lerp(aSweepAngle_deg,
                                    it_prev->first,
                                    it->first,
                                    it_prev->second.second,
                                    it->second.second);

            return std::make_pair(a, l);
         }
      }
      return it->second;
   }
   return std::pair<double, double>();
}

std::pair<double, double> Designer::AeroData::GetNegLiftValley(double aSweepAngle_deg) const
{
   auto it = mLiftDataCharacteristics.valleyNeg.lower_bound(aSweepAngle_deg);
   if (it != mLiftDataCharacteristics.valleyNeg.end())
   {
      if (it->first != aSweepAngle_deg)
      {
         if (it != mLiftDataCharacteristics.valleyNeg.begin())
         {
            auto it_prev = std::prev(it, 1);

            double a = UtMath::Lerp(aSweepAngle_deg,
                                    it_prev->first,
                                    it->first,
                                    it_prev->second.first,
                                    it->second.first);

            double l = UtMath::Lerp(aSweepAngle_deg,
                                    it_prev->first,
                                    it->first,
                                    it_prev->second.second,
                                    it->second.second);

            return std::make_pair(a, l);
         }
      }
      return it->second;
   }
   return std::pair<double, double>();
}

double Designer::AeroData::GetPreStallSlope(double aSweepAngle_deg) const
{
   auto it = mLiftDataCharacteristics.preStallSlope.lower_bound(aSweepAngle_deg);
   if (it != mLiftDataCharacteristics.preStallSlope.end())
   {
      if (it->first != aSweepAngle_deg)
      {
         if (it != mLiftDataCharacteristics.preStallSlope.begin())
         {
            auto it_prev = std::prev(it, 1);

            return UtMath::Lerp(aSweepAngle_deg,
                                it_prev->first,
                                it->first,
                                it_prev->second,
                                it->second);
         }
      }
      return it->second;
   }
   return 0.0;
}

double Designer::AeroData::GetAeroData(DataType aDataType,
                                       double   aAlpha_deg,
                                       double   aSweepAngle_deg) const
{
   if (aAlpha_deg > 180.0)
   {
      return GetAeroData(aDataType, aAlpha_deg - 360.0, aSweepAngle_deg);
   }

   if (aAlpha_deg < -180.0)
   {
      return GetAeroData(aDataType, aAlpha_deg + 360.0, aSweepAngle_deg);
   }

   if (aSweepAngle_deg > 180.0)
   {
      return GetAeroData(aDataType, aAlpha_deg, aSweepAngle_deg - 360.0);
   }

   if (aSweepAngle_deg < -180.0)
   {
      return GetAeroData(aDataType, aAlpha_deg, aSweepAngle_deg + 360.0);
   }

   double sweep1 = 0.0;
   double sweep2 = 0.0;
   double alpha1 = 0.0;
   double alpha2 = 0.0;
   double alpha3 = 0.0;
   double alpha4 = 0.0;
   double val1 = 0.0;
   double val2 = 0.0;
   double val3 = 0.0;
   double val4 = 0.0;

   auto it_sweep = mNominalData.at(aDataType).lower_bound(aSweepAngle_deg);

   if (it_sweep != mNominalData.at(aDataType).end())
   {
      sweep2 = it_sweep->first;
      sweep1 = sweep2;

      if (aAlpha_deg < 0)
      {
         auto it = it_sweep->second.first.lower_bound(aAlpha_deg);
         if (it != it_sweep->second.first.end())
         {
            alpha4 = it->first;
            val4 = it->second;
            alpha3 = alpha4;
            val3 = val4;
            auto it_prev = it;
            if (it_prev != it_sweep->second.first.begin())
            {
               it_prev = std::prev(it, 1);
               alpha3 = it_prev->first;
               val3 = it_prev->second;
            }
         }
         alpha2 = alpha4;
         alpha1 = alpha3;
         val2 = val4;
         val1 = val3;
         if (it_sweep != mNominalData.at(aDataType).begin())
         {
            auto it_sweep_p = std::prev(it_sweep, 1);

            sweep1 = it_sweep_p->first;

            auto it_p = it_sweep_p->second.first.lower_bound(aAlpha_deg);
            if (it_p != it_sweep_p->second.first.end())
            {
               alpha2 = it_p->first;
               val2 = it_p->second;
               alpha1 = alpha2;
               val1 = val2;
               auto it_prev_p = it_p;
               if (it_prev_p != it_sweep_p->second.first.begin())
               {
                  it_prev_p = std::prev(it_p, 1);
                  alpha1 = it_prev_p->first;
                  val1 = it_prev_p->second;
               }
            }
         }
      }
      else
      {
         auto it = it_sweep->second.second.lower_bound(aAlpha_deg);
         if (it != it_sweep->second.second.end())
         {
            alpha4 = it->first;
            val4 = it->second;
            alpha3 = alpha4;
            val3 = val4;
            auto it_prev = it;
            if (it_prev != it_sweep->second.second.begin())
            {
               it_prev = std::prev(it, 1);
               alpha3 = it_prev->first;
               val3 = it_prev->second;
            }
         }
         alpha2 = alpha4;
         alpha1 = alpha3;
         val2 = val4;
         val1 = val3;
         if (it_sweep != mNominalData.at(aDataType).begin())
         {
            auto it_sweep_p = std::prev(it_sweep, 1);

            sweep1 = it_sweep_p->first;

            auto it_p = it_sweep_p->second.second.lower_bound(aAlpha_deg);
            if (it_p != it_sweep_p->second.second.end())
            {
               alpha2 = it_p->first;
               val2 = it_p->second;
               alpha1 = alpha2;
               val1 = val2;
               auto it_prev_p = it_p;
               if (it_prev_p != it_sweep_p->second.second.begin())
               {
                  it_prev_p = std::prev(it_p, 1);
                  alpha1 = it_prev_p->first;
                  val1 = it_prev_p->second;
               }
            }
         }
      }
   }

   auto interpolationValues = std::make_pair(std::make_pair(sweep1, std::make_pair(std::make_pair(alpha1, val1), std::make_pair(alpha2, val2))),
                                             std::make_pair(sweep2, std::make_pair(std::make_pair(alpha3, val3), std::make_pair(alpha4, val4))));

   return InterpolateBilinear(interpolationValues, std::make_pair(aSweepAngle_deg, aAlpha_deg));
}

void Designer::AeroData::CalcLiftDataCharacteristics(double aSweepAngle_deg)
{
   {
      // Iterate over positive half
      std::pair<double, double> prevLift = std::make_pair<double, double>(0, GetLiftCoeff(0, aSweepAngle_deg));
      for (int a = 0; a < 180; ++a)
      {
         mLiftDataCharacteristics.peakPos[aSweepAngle_deg] = prevLift;

         std::pair<double, double> lift = std::make_pair<double, double>(a + 1, GetLiftCoeff(a + 1, aSweepAngle_deg));

         double localSlope = util::CalcSlope(UtMath::cRAD_PER_DEG * prevLift.first,
                                             UtMath::cRAD_PER_DEG * lift.first,
                                             prevLift.second,
                                             lift.second);

         if (localSlope > 0)
         {
            prevLift = lift;
         }
         else
         {
            break;
         }
      }

      for (int a = prevLift.first + 1; a < 180; ++a)
      {
         mLiftDataCharacteristics.valleyPos[aSweepAngle_deg] = prevLift;

         std::pair<double, double> lift = std::make_pair<double, double>(a + 1, GetLiftCoeff(a + 1, aSweepAngle_deg));

         double localSlope = util::CalcSlope(UtMath::cRAD_PER_DEG * prevLift.first, UtMath::cRAD_PER_DEG * lift.first, prevLift.second, lift.second);

         if (localSlope < 0)
         {
            prevLift = lift;
         }
         else
         {
            break;
         }
      }
   }

   {
      // Iterate over positive half
      std::pair<double, double> prevLift = std::make_pair<double, double>(0, GetLiftCoeff(0, aSweepAngle_deg));
      for (int a = 0; a > -180; --a)
      {
         mLiftDataCharacteristics.peakNeg[aSweepAngle_deg] = prevLift;

         std::pair<double, double> lift = std::make_pair<double, double>(a - 1, GetLiftCoeff(a - 1, aSweepAngle_deg));

         double localSlope = util::CalcSlope(UtMath::cRAD_PER_DEG * prevLift.first, UtMath::cRAD_PER_DEG * lift.first, prevLift.second, lift.second);

         if (localSlope > 0)
         {
            prevLift = lift;
         }
         else
         {
            break;
         }
      }

      for (int a = prevLift.first - 1; a > -180; --a)
      {
         mLiftDataCharacteristics.valleyNeg[aSweepAngle_deg] = prevLift;

         std::pair<double, double> lift = std::make_pair<double, double>(a - 1, GetLiftCoeff(a - 1, aSweepAngle_deg));

         double localSlope = util::CalcSlope(UtMath::cRAD_PER_DEG * prevLift.first, UtMath::cRAD_PER_DEG * lift.first, prevLift.second, lift.second);

         if (localSlope < 0)
         {
            prevLift = lift;
         }
         else
         {
            break;
         }
      }
   }

   mLiftDataCharacteristics.preStallSlope[aSweepAngle_deg] =
      util::CalcSlope(UtMath::cRAD_PER_DEG * mLiftDataCharacteristics.peakNeg[aSweepAngle_deg].first,
                      UtMath::cRAD_PER_DEG * mLiftDataCharacteristics.peakPos[aSweepAngle_deg].first,
                      mLiftDataCharacteristics.peakNeg[aSweepAngle_deg].second,
                      mLiftDataCharacteristics.peakPos[aSweepAngle_deg].second);
}

double Designer::AeroData::InterpolateBilinear(std::pair<std::pair<double, std::pair<std::pair<double, double>, std::pair<double, double>>>,
                                                 std::pair<double, std::pair<std::pair<double, double>, std::pair<double, double>>>> aMinMaxValues, std::pair<double, double> aIndependentVars) const
{
   // aMinMaxValues    {X1, { {Y1, Z1}, {Y2, Z2} } }
   //                  {X2, { {Y3, Z3}, {Y4, Z4} } }
   // aIndependentVars {X_val, Y_val}
   //
   // This function does the following:
   // Interpolate between {Y1, Z1} and {Y2, Z2} using Y_val, producing {Y_val, Z5}
   // Interpolate between {Y3, Z3} and {Y4, Z4} using Y_val, producing {Y_val, Z6}
   // Interpolate between {X1, Z5} and {X2, Z6} using X_val, producing {X_val, Z_ans}
   // Return Z_ans

   double x1 = aMinMaxValues.first.first;
   double y1 = aMinMaxValues.first.second.first.first;
   double z1 = aMinMaxValues.first.second.first.second;
   double y2 = aMinMaxValues.first.second.second.first;
   double z2 = aMinMaxValues.first.second.second.second;

   double x2 = aMinMaxValues.second.first;
   double y3 = aMinMaxValues.second.second.first.first;
   double z3 = aMinMaxValues.second.second.first.second;
   double y4 = aMinMaxValues.second.second.second.first;
   double z4 = aMinMaxValues.second.second.second.second;

   double x_val = aIndependentVars.first;
   double y_val = aIndependentVars.second;

   double z5, z6, z_ans = 0;

   // Interpolate between {Y1, Z1} and {Y2, Z2} using Y_val, producing {Y_val, Z5}
   if (y_val == y1)
   {
      z5 = z1;
   }
   else if (y_val == y2)
   {
      z5 = z2;
   }
   else
   {
      z5 = UtMath::Lerp(y_val, y1, y2, z1, z2);
   }

   // Interpolate between {Y3, Z3} and {Y4, Z4} using Y_val, producing {Y_val, Z6}
   if (y_val == y3)
   {
      z6 = z3;
   }
   else if (y_val == y4)
   {
      z6 = z4;
   }
   else
   {
      z6 = UtMath::Lerp(y_val, y3, y4, z3, z4);
   }

   // Interpolate between {X1, Z5} and {X2, Z6} using X_val, producing {X_val, Z_ans}
   if (x_val == x1)
   {
      z_ans = z5;
   }
   else if (x_val == x2)
   {
      z_ans = z6;
   }
   else
   {
      z_ans = UtMath::Lerp(x_val, x1, x2, z5, z6);
   }

   // Return Z_ans
   return z_ans;
}

} // namespace Designer
