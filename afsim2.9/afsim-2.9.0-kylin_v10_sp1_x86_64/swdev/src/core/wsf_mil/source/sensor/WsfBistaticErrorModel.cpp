// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfBistaticErrorModel.hpp"

#include <algorithm>
#include <cmath>
#include <string>

#include "UtException.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorResult.hpp"

namespace
{
bool IsNaN(double d)
{
   return d != d;
}
bool IsInfinite(double d)
{
   return d == d && d > DBL_MAX;
}
} // namespace

namespace wsf
{

BistaticErrorModel::BistaticErrorModel()
   : StandardSensorErrorModel()
   , mTsErrorStdDev(0.0)
   , mTxErrorStdDev(0.0)
   , mRealBlurring(false)
   , mPositionDeltaMap()
{
}

BistaticErrorModel* BistaticErrorModel::Clone() const
{
   return new BistaticErrorModel(*this);
}

bool BistaticErrorModel::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   double      value;
   std::string command(aInput.GetCommand());

   if (command == "time_reflected_sigma")
   {
      aInput.ReadValueOfType(value, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(value, 0.0);
      mTsErrorStdDev = value;
   }
   else if (command == "time_direct_sigma")
   {
      aInput.ReadValueOfType(value, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(value, 0.0);
      mTxErrorStdDev = value;
   }
   else if (command == "transmitter_position_sigmas")
   {
      LoadPositionSigmaTable(aInput);
   }
   else if (command == "realistic_blurring")
   {
      mRealBlurring = true;
   }
   else
   {
      myCommand = StandardSensorErrorModel::ProcessInput(aInput);
      ;
   }
   return myCommand;
}

bool BistaticErrorModel::Initialize(WsfSensorMode* aSensorModePtr)
{
   bool initialized = StandardSensorErrorModel::Initialize(aSensorModePtr);
   if (initialized)
   {
      mInitialSigmas            = GetSigmas();
      constexpr double cMAX_ANG = UtMath::cPI_OVER_2;
      if (mInitialSigmas.mAzErrorStdDev > cMAX_ANG)
      {
         auto error = ut::log::error() << "Azimuth standard deviation greater than maximum allowed value";
         error.AddNote() << "Provided value: " << mInitialSigmas.mAzErrorStdDev;
         error.AddNote() << "Maximum value: " << cMAX_ANG;
         initialized = false;
      }
      if (mInitialSigmas.mElErrorStdDev > cMAX_ANG)
      {
         auto error = ut::log::error() << "Elevation standard deviation greater than maximum allowed value";
         error.AddNote() << "Provided value: " << mInitialSigmas.mElErrorStdDev;
         error.AddNote() << "Maximum value: " << cMAX_ANG;
         initialized = false;
      }
      if (mInitialSigmas.mRangeErrorStdDev > 0.0)
      {
         auto warning =
            ut::log::warning()
            << "BistaticErrorModel: Range error will be computed by the model; the provided value will be ignored.";
         warning.AddNote() << "Provided value: " << mInitialSigmas.mRangeErrorStdDev;
      }
   }
   return initialized;
}

void BistaticErrorModel::LoadPositionSigmaTable(UtInput& aInput)
{
   std::string tableUnits;
   std::string command;

   aInput.ReadValue(tableUnits);

   // Peek
   aInput.ReadValue(command);
   aInput.PushBack(command);

   int i = 0;
   while (command != "end_transmitter_position_sigmas")
   {
      TransmitterKey key;

      aInput.ReadValue(key.mSide);
      aInput.ReadValue(key.mDomain);

      WsfSpatialDomain domain;
      if (!WsfTypes::StringToEnum(key.mDomain, domain))
      {
         auto out = ut::log::error() << "SensorMode::BistaticTable Domain not recognized.";
         out.AddNote() << "Domain: " << key.mDomain;
         throw UtInput::BadValue(aInput, "SensorMode::BistaticTable Domain not recognized - " + key.mDomain);
      }

      UtVec3d& positionEpsilon = mPositionDeltaMap[key];

      for (int v = 0; v < 3; ++v)
      {
         double d;
         aInput.ReadValue(d);
         positionEpsilon[v] = aInput.ConvertValue(d, tableUnits, UtInput::cLENGTH);
      }
      // aInput.ReadValueOfType(positionEpsilon[0], tableType);
      // aInput.ReadValueOfType(positionEpsilon[1], tableType);
      // aInput.ReadValueOfType(positionEpsilon[2], tableType);

      // positionEpsilon[0] =

      // Redundant, but safe.
      mPositionDeltaMap[key] = positionEpsilon;

      // Peek
      aInput.ReadValue(command);
      aInput.PushBack(command);
      // aInput.ReadCommand(command);
      i++;
   }

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "BISD read inline table.";
      out.AddNote() << "Lines read: " << i;
      out.AddNote() << "Lines stored: " << i; // these are both 'i'???
   }

   aInput.ReadCommand(command);
   if (command == "end_transmitter_position_sigmas")
   {
      // bool valid = true;
      // if (!valid)
      //{
      //    throw UtInput::BadValue(aInput, "SensorMode::BistaticTable Inline table validation error");
      // }
   }
   else
   {
      throw UtInput::UnknownCommand(aInput);
   }

   // Table is good... take ownership of the table object from the unique_ptr
}

//! Factory method for WsfSensorErrorModelTypes.
//! This is called by WsfSensorErrorModelTypes::LoadType to determine if a pattern
//! represented by this class is being requested.
// static
std::unique_ptr<SensorErrorModelBase> BistaticErrorModel::ObjectFactory(const std::string& aTypeName)
{
   if ((aTypeName == "WSF_BISTATIC_ERROR") || // WSF_BISTATIC_ERROR is undocumented and will be removed in the future.
       (aTypeName == "bistatic_error"))
   {
      return ut::make_unique<BistaticErrorModel>();
   }
   return nullptr;
}

SphericalMeasurementErrors BistaticErrorModel::GetSphericalErrorStdDev(ut::Random&      aRandom,
                                                                       WsfSensorResult& aResult,
                                                                       const Sigmas&    aSigmas) const
{
   if ((aResult.GetTransmitter()->GetAntenna() == aResult.GetReceiver()->GetAntenna()) ||
       (aResult.mRcvrToXmtr.mRange < 0.1))
   {
      ut::log::error() << "BistaticErrorModel::GetSphericalErrorStdDev: Input is mono-static.";
      throw UtException("BistaticErrorModel::GetSphericalErrorStdDev: Input is mono-static!");
   }

   // Look up the position sigmas for this type of transmitter platform
   const WsfEM_Xmtr*  xmtr = aResult.GetTransmitter();
   const WsfPlatform* txp  = xmtr->GetPlatform();

   TransmitterKey key;
   key.mDomain = WsfTypes::EnumToString(txp->GetSpatialDomain());
   key.mSide   = txp->GetSide();

   auto       it         = mPositionDeltaMap.find(key);
   static int warningCnt = 0;

   UtVec3d txPosSigma(0.0); // fill with zero

   if (warningCnt < 5 && it == mPositionDeltaMap.end())
   {
      auto out = ut::log::error() << "BistaticErrorModel::GetSphericalErrorStdDev: Transmitter sigmas not found.";
      out.AddNote() << "Side: " << key.mSide;
      out.AddNote() << "Domain: " << key.mDomain;
      warningCnt++;
      txPosSigma[0] = 10.0;
      txPosSigma[1] = 10.0;
      txPosSigma[2] = 10.0;
   }
   else
   {
      // Copy the values
      txPosSigma = it->second;
   }

   // Use the values
   double vXErrorStdDev = txPosSigma[0];
   double vYErrorStdDev = txPosSigma[1];
   double vZErrorStdDev = txPosSigma[2];

   SphericalMeasurementErrors errors;

   // Blur the az and el right away.  Range is "calculated" and then blurred.
   errors.mAzError = aRandom.Gaussian() * aSigmas.mAzErrorStdDev;
   errors.mElError = aRandom.Gaussian() * aSigmas.mElErrorStdDev;

   // WCS locations are available, but the thesis uses PCS.
   // The error calculations will give incorrect results if you use any coordinate system other than PCS.
   // To see this, examine the definition of cosAxt below.
   double locPCS_txx[3];
   double locPCS_tgt[3]; // truth

   WsfEM_Antenna* rcvrAntPtr = aResult.GetReceiver()->GetAntenna();
   double         temp[3];
   rcvrAntPtr->GetRelativeLocationWCS(aResult.GetTarget(), temp);
   rcvrAntPtr->GetArticulatedPart()->ConvertWCSVectorToPCS(locPCS_tgt, temp);

   double temp2[3];
   aResult.GetTransmitter()->GetAntenna()->GetLocationWCS(temp);
   rcvrAntPtr->GetRelativeLocationWCS(temp, temp2);
   rcvrAntPtr->GetArticulatedPart()->ConvertWCSVectorToPCS(locPCS_txx, temp2);

   //// The receiver is the origin
   // double rx = UtVec3d::Magnitude(locPCS_txx);
   double rx = aResult.mRcvrToXmtr.mRange;

   // ------------------------
   // Create the simulated measured values ----
   // ------------------------
   // There seems to be two options - calculate the fuzzy range
   // using the blurred times, or using the blurs to calculate the range
   // error and blur it directly.  If the later, we need to blur
   // rx using the blurred position, but that blurred rx should NOT
   // be used to re-blur the independent measurement of tx.
   // double rt = UtVec3d::Magnitude(locTx2tgt);
   double rt = aResult.mXmtrToTgt.mRange;

   // This could be replaced with equation 9
   // double rr = UtVec3d::Magnitude(locNED_tgt);
   double rr = aResult.mRcvrToTgt.mRange;

   static const double C = UtMath::cLIGHT_SPEED;

   // Not that it matters, we create the signal durations vs absolute rx time.
   double tx = rx / C;
   double ts = (rt + rr) / C;

   // Arguably, we don't (removed) let the blurring cause negative durations.
   double btx = tx + aRandom.Gaussian() * mTxErrorStdDev;
   // if (btx < 0.0) {
   //    btx = 0.0;
   //    //error > etx = tx;
   // }
   tx = btx;

   double bts = ts + aRandom.Gaussian() * mTsErrorStdDev;
   // if (bts < 0.0) {
   //    bts = 0.0;
   //    //error > ets = ts;
   // }
   ts = bts;

   // Compute the PCS errors from NED errors.
   double wcsToPCS[3][3];
   double tempCol[3];
   double nedToWCS[3][3];
   double nedToPCS[3][3];
   // Ugly hack since they don't provide a method to extract the matrix.
   {
      double ned[3] = {1, 0, 0};
      rcvrAntPtr->ConvertNED_VectorToWCS(ned, tempCol);
      nedToWCS[0][0] = tempCol[0];
      nedToWCS[1][0] = tempCol[1];
      nedToWCS[2][0] = tempCol[2];
   }
   {
      double ned[3] = {0, 1, 0};
      rcvrAntPtr->ConvertNED_VectorToWCS(ned, tempCol);
      nedToWCS[0][1] = tempCol[0];
      nedToWCS[1][1] = tempCol[1];
      nedToWCS[2][1] = tempCol[2];
   }
   {
      double ned[3] = {0, 0, 1};
      rcvrAntPtr->ConvertNED_VectorToWCS(ned, tempCol);
      nedToWCS[0][2] = tempCol[0];
      nedToWCS[1][2] = tempCol[1];
      nedToWCS[2][2] = tempCol[2];
   }
   rcvrAntPtr->GetArticulatedPart()->GetWCSToPCSTransform(wcsToPCS);
   UtMat3d::Multiply(nedToPCS, wcsToPCS, nedToWCS);
   // This comes from the formula for linear transforms of normal random variables.
   vXErrorStdDev = sqrt(nedToPCS[0][0] * nedToPCS[0][0] * txPosSigma[0] * txPosSigma[0] +
                        nedToPCS[0][1] * nedToPCS[0][1] * txPosSigma[1] * txPosSigma[1] +
                        nedToPCS[0][2] * nedToPCS[0][2] * txPosSigma[2] * txPosSigma[2]);
   vYErrorStdDev = sqrt(nedToPCS[1][0] * nedToPCS[1][0] * txPosSigma[0] * txPosSigma[0] +
                        nedToPCS[1][1] * nedToPCS[1][1] * txPosSigma[1] * txPosSigma[1] +
                        nedToPCS[1][2] * nedToPCS[1][2] * txPosSigma[2] * txPosSigma[2]);
   vZErrorStdDev = sqrt(nedToPCS[2][0] * nedToPCS[2][0] * txPosSigma[0] * txPosSigma[0] +
                        nedToPCS[2][1] * nedToPCS[2][1] * txPosSigma[1] * txPosSigma[1] +
                        nedToPCS[2][2] * nedToPCS[2][2] * txPosSigma[2] * txPosSigma[2]);
   txPosSigma[0] = vXErrorStdDev;
   txPosSigma[1] = vYErrorStdDev;
   txPosSigma[2] = vZErrorStdDev;

   // Apply PCS error
   locPCS_txx[0] += aRandom.Gaussian() * vXErrorStdDev;
   locPCS_txx[1] += aRandom.Gaussian() * vYErrorStdDev;
   locPCS_txx[2] += aRandom.Gaussian() * vZErrorStdDev;

   BistaticMeasurement measurement;

   measurement.mTs = ts;
   measurement.mTx = tx;

   // In case we blur it into an infeasible angle?  Actually, we should rotate az 180 deg
   // if it blurs past zenith.  Any gimbal lock-ish thing should be there.
   measurement.mEl = aResult.mRcvrToTgt.mEl + errors.mElError;
   // measurement.mEl = UtMath::NormalizeAngleMinusPi_Pi(measurement.mEl);
   if (measurement.mEl > UtMath::cPI_OVER_2)
   {
      measurement.mEl = UtMath::cPI_OVER_2;
   }
   if (measurement.mEl < -UtMath::cPI_OVER_2)
   {
      measurement.mEl = -UtMath::cPI_OVER_2;
   }
   errors.mElError = measurement.mEl - aResult.mRcvrToTgt.mEl;

   measurement.mAz = UtMath::NormalizeAngleMinusPi_Pi(aResult.mRcvrToTgt.mAz) + errors.mAzError;
   measurement.mAz = UtMath::NormalizeAngle0_TwoPi(measurement.mAz);

   // Used for limiting, but range could be calculated there (Eq 9)
   measurement.mRng = rr;

   measurement.mTransmitterPositionPCS.Set(locPCS_txx);
   measurement.mTransmitterPositionSigma = &txPosSigma;

   // Chicken-egg.  Normally the blurred az-el (ect) are used to find the dynamic
   // sigmas.  So if we change the az-el sigmas here, they will be used for the final
   // errors.  If we pre-blur them here, as we do for the TX, those blurrings are only
   // used by this process, and don't need to be communicated.
   // double az = aResult.mRcvrToTgt.mAz;
   // double el = aResult.mRcvrToTgt.mEl;

   //--------------------------------------
   Sigmas sigmas{GetBistaticSphericalErrorStdDev(measurement)};
   //--------------------------------------

   // Now that range sigma is set, get the error.
   if (!mRealBlurring)
   {
      errors.mRangeError = aRandom.Gaussian() * sigmas.mRangeErrorStdDev;
   }
   else
   {
      double rangeBlurred = measurement.mRng;
      errors.mRangeError  = rangeBlurred - aResult.mRcvrToTgt.mRange;
   }

   // 2nd derivative or only valid with Doppler?
   errors.mRangeRateError = aRandom.Gaussian() * sigmas.mRangeRateErrorStdDev;

   // If the temp sigmas were modified, it was done already.  Report the sigmas used.
   // aResult has unfortunate naming.
   aResult.mMeasurement.SetSensorAzimuthError(sigmas.mAzErrorStdDev);
   aResult.mMeasurement.SetSensorElevationError(sigmas.mElErrorStdDev);
   aResult.mMeasurement.SetRangeError(sigmas.mRangeErrorStdDev);
   aResult.mMeasurement.SetRangeRateError(sigmas.mRangeRateErrorStdDev);

   return errors;
}

wsf::StandardSensorErrorModel::Sigmas BistaticErrorModel::GetBistaticSphericalErrorStdDev(BistaticMeasurement& aMeasurement) const
{
   BistaticMeasurement& m = aMeasurement;

   // Make a copy of Sigmas to modify
   Sigmas sigmas = mInitialSigmas;

   static const double C       = UtMath::cLIGHT_SPEED;
   static const double MAX_SEC = 10.0;

   // TODO-AWK Need to add some better debug output for the user!
   if (mTsErrorStdDev > MAX_SEC || mTxErrorStdDev > MAX_SEC || aMeasurement.mTs > MAX_SEC || aMeasurement.mTx > MAX_SEC ||
       std::abs(aMeasurement.mAz) > UtMath::cTWO_PI || std::abs(aMeasurement.mEl) > UtMath::cPI_OVER_2 ||
       aMeasurement.mTransmitterPositionSigma == nullptr || std::abs(aMeasurement.mTransmitterPositionPCS[0]) > 1E30)
   {
      ut::log::error() << "BistaticErrorModel:GetSphericalErrorStdDev: The inputs are not set/invalid.";
      throw UtException("BistaticErrorModel:GetSphericalErrorStdDev: The inputs are not set/invalid.");
   }

   // Az & El are blurred with the standard STD
   double az = m.mAz;
   double el = m.mEl;
   double rr = m.mRng;

   // This is used for the range calculation, it has both timing errors.
   double td = (m.mTs - m.mTx);
   if (td < 0.0)
   {
      // Blurring and a target right between TX and RX might cause a negative time difference.
      auto out = ut::log::error()
                 << "BistaticErrorModel:GetSphericalErrorStdDev: Blurred time difference is negative. Limiting to 0.";
      out.AddNote() << "Time difference: " << td;
      td = 0.0;
   }
   double ctd = C * td;

   const UtVec3d& locPCS_txx       = m.mTransmitterPositionPCS;
   UtVec3d&       txPosErrorStdDev = *m.mTransmitterPositionSigma;

   // Gimbal measurements and errors.  They are not needed yet.  It looks like roll
   // is not used in the paper?  Alrry concurred.
   double gimbleYaw   = 0.0;
   double gimblePitch = 0.0;

   // In the normal process, the az-el are measured.  A sigma is known for them, but
   // then this process may adjust/correct those "sigmas", but the az and el are modified (blurred).
   // Instead, the new sigmas are only used to calculate the covariance.

   // -^----  Above, simulation -----
   // -v----  Below, sigma calculations using simulated, blurred data -----
   // double rx = UtVec3d::Magnitude(locPCS_txx);
   double rx = locPCS_txx.Magnitude();
   if (rx < 0.1)
   {
      auto out = ut::log::error() << "BistaticErrorModel:GetSphericalErrorStdDev: Dist to tx 0. Must be monostatic.";
      out.AddNote() << "Range: " << rr;
      throw UtException("BistaticErrorModel:GetSphericalErrorStdDev: Dist to tx 0.  Must be monostatic!");
   }

   double cosAz = cos(az);
   double sinAz = sin(az);
   double cosEl = cos(el);
   double sinEl = sin(el);
   double tanEl = tan(el);

   double cosAxt   = (locPCS_txx[0] * cosEl * cosAz + locPCS_txx[1] * cosEl * sinAz - locPCS_txx[2] * sinEl) / rx;
   double Axt      = acos(cosAxt);
   double cosAxtD2 = cos(Axt / 2.0);
   double sinAxtD2 = sin(Axt / 2.0);
   double sinAxt   = sin(Axt);

   double cosPit = cos(gimblePitch);
   double cosYaw = cos(gimbleYaw);
   double sinPit = sin(gimblePitch);
   double sinYaw = sin(gimbleYaw);

   double xTx = locPCS_txx[0];
   double yTx = locPCS_txx[1];
   double zTx = locPCS_txx[2];

   double b1;
   double b2;
   double b3;
   double m1;
   double m2;
   double m3;
   double m4;
   double m5;
   double m6;
   double m7;
   double rS;
   double a1;
   double tmp1;

   double g11;
   double g12;
   double g21;
   double g22;
   // Page 24 eq 86
   g11 = cosAz * cosPit * sinYaw + sinAz * cosPit * cosYaw;
   g12 = -cosAz * cosYaw + sinAz * sinYaw;
   g21 = -sinPit - tanEl * cosAz * cosPit * cosYaw + tanEl * sinAz * cosPit * sinYaw;
   g22 = -tanEl * cosAz * sinYaw - tanEl * sinAz * cosYaw;

   tmp1       = ctd + 2 * rx * sinAxtD2 * sinAxtD2; // 51
   double den = tmp1 * tmp1;

   // Q: Why is C part of this term?  Look at units, multiplier of time to get range error.
   m1 = C * (2 * sinAxtD2 * sinAxtD2 * (ctd * rx + rx * rx) + 0.5 * ctd * ctd);
   m1 = m1 / den;
   m2 = -m1;

   b1 = ctd * ctd * cosAxtD2 * cosAxtD2;
   b1 = b1 / den;

   b2 = -(ctd * ctd + 2 * ctd * rx * rx) * sinAxt;
   b2 = b2 / (2 * den);

   rS = xTx * cosEl * cosAz + yTx * cosEl * sinAz - zTx * sinEl;
   a1 = rx * rx - rS * rS;
   a1 = sqrt(a1);

   b3 = rS / (rx * a1);

   m3 = b1 * xTx / rx - b2 * cosEl * cosAz / a1 + b2 * b3 * xTx / rx;
   m4 = b1 * yTx / rx - b2 * cosEl * sinAz / a1 + b2 * b3 * yTx / rx;
   m5 = b1 * zTx / rx + b2 * sinEl / a1 + b2 * b3 * zTx / rx;

   m6 = b2 * (xTx * sinEl * cosAz + yTx * sinEl * sinAz + zTx * cosEl) / a1; // Eq 60
   m7 = b2 * (xTx * cosEl * sinAz - yTx * cosEl * cosAz) / a1;               // Eq 61

   UtMatrixd H(3, 10);
   UtMatrixd e1(10, 1);
   UtMatrixd ets(3, 1);

   // Form it exactly like the paper
   double Ha[3][10] = {
      {m1, m2, m3, m4, m5, m6 * g11 + m7 * g21, m6 * g12 + m7 * g22, -m7, m6, m7},
      {0, 0, 0, 0, 0, g11, g12, 0, 1, 0},
      {0, 0, 0, 0, 0, g21, g22, -1, 0, 1},
   };
   // Put it into a UtMatrix for multiplication
   for (int i = 0; i < 3; ++i)
   {
      for (int j = 0; j < 10; ++j)
      {
         H(i, j) = Ha[i][j];
      }
   }

   // Put the stds in a vector for multiplication
   int ei = 0;

   e1(ei++, 0) = mTsErrorStdDev;
   e1(ei++, 0) = mTxErrorStdDev;

   e1(ei++, 0) = txPosErrorStdDev[0];
   e1(ei++, 0) = txPosErrorStdDev[1];
   e1(ei++, 0) = txPosErrorStdDev[2];

   // Placeholders
   double mRollErrorStdDev = 0.0;
   double mPitErrorStdDev  = 0.0;
   double mYawErrorStdDev  = 0.0;

   e1(ei++, 0) = mRollErrorStdDev;
   e1(ei++, 0) = mPitErrorStdDev;
   e1(ei++, 0) = mYawErrorStdDev;

   // We use the stock angle-sigmas values to multiply
   e1(ei++, 0) = sigmas.mAzErrorStdDev;
   e1(ei++, 0) = sigmas.mElErrorStdDev;

   // Spherical error, where the az and el error with no gimble should remain unmodified.
   // Only range error is determined here.
   ets.Multiply(H, e1);

   // err.range = std::abs(ets[0][0]);
   // err.el    = std::abs(ets[1][0]);
   // err.az    = std::abs(ets[2][0]);
   double rngErrorStdDev = std::abs(ets(0));

   // if (pw != null) pw.close();
   //  If any mathematical noise, set the range error to the range.
   //  This happens when the target flies between the RX and TX.
   if (IsNaN(rngErrorStdDev) || IsInfinite(rngErrorStdDev))
   {
      auto out = ut::log::error()
                 << "BistaticErrorModel:GetSphericalErrorStdDev: Problems were encountered calculated range error.";
      out.AddNote() << "Value: " << rngErrorStdDev;
      rngErrorStdDev = rx / 2.0;
   }

   if (rngErrorStdDev < 0.5)
   {
      auto out = ut::log::error()
                 << "BistaticErrorModel:GetSphericalErrorStdDev: Problems were encountered calculated range error.";
      out.AddNote() << "Value was too low. Overriding using position error.";
      out.AddNote() << "Value: " << rngErrorStdDev;
      // It might have been better to also look at max of cts.  Weakest link sort of thing.
      rngErrorStdDev = std::max(std::max(txPosErrorStdDev[0], txPosErrorStdDev[1]), txPosErrorStdDev[2]);
   }

   // It is possible that the others errors need to be updated, but for now
   // just update the range sigma.
   ////sigmas.mAzErrorStdDev = ???
   ////sigmas.mElErrorStdDev = ???
   sigmas.mRangeErrorStdDev = rngErrorStdDev; //<<<<<!!!!!!!! mRangeErrorStdDev;

   // Calculate the range based on the blurred values
   // Equation 9 page 7
   den    = 2 * (ctd + 2 * rx * sinAxtD2 * sinAxtD2);
   rr     = ctd * ctd + 2 * ctd * rx;
   m.mRng = rr / den; // Equation 9 page 7

   if (IsNaN(m.mRng) || IsInfinite(m.mRng))
   {
      // Again happens when the target is between (in-line?) the RX and TX.
      auto out = ut::log::error()
                 << "BistaticErrorModel:GetSphericalErrorStdDev: Problems were encountered calculating 'range'.";
      out.AddNote() << "Value: " << m.mRng;

      m.mRng = rx / 2.0;
   }

   return sigmas;
}

} // namespace wsf
