// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfEventUtils.hpp"

#include <iomanip>

#include "UtAngle.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtLatPos.hpp"
#include "UtLonPos.hpp"
#include "UtTime.hpp"
#include "WsfComm.hpp"
#include "WsfCommRouter.hpp"
#include "WsfCovariance.hpp"
#include "WsfDateTime.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEM_XmtrRcvr.hpp"
#include "WsfEventResult.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationExtension.hpp"
#include "WsfStatusMessage.hpp"
#include "WsfTaskAssignMessage.hpp"
#include "WsfTrackDropMessage.hpp"
#include "WsfTrackMessage.hpp"
#include "WsfUtil.hpp"

namespace wsf
{
namespace event
{

namespace utils
{

// =================================================================================================
void PrintLLA(std::ostream& aStream, double aLat, double aLon, double aAlt, int aLatLonFormat)
{
   aStream << UtLatPos(aLat, aLatLonFormat) << ' ' << UtLonPos(aLon, aLatLonFormat) << ' ';
   std::streamsize oldPrecision = aStream.precision(8);
   aStream << aAlt << " m";
   aStream.precision(oldPrecision);
}

// =================================================================================================
std::string ContinueChar(bool aPrintSingleLinePerEvent)
{
   if (aPrintSingleLinePerEvent)
   {
      return " ";
   }
   else
   {
      return " \\\n";
   }
}

// =================================================================================================
void PrintTime(std::ostream& aStream, double aSimTime, int aTimeFormat)
{
   aStream << UtTime(aSimTime, aTimeFormat) << ' ';
}

// =================================================================================================
void PrintTime(std::ostream& aStream, WsfSimulation& aSimulation, int aTimeFormat)
{
   PrintTime(aStream, aSimulation.GetSimTime(), aTimeFormat);
}

// =================================================================================================
void PrintDateTime(std::ostream& aStream, const WsfSimulation& aSimulation)
{
   PrintDateTime(aStream, aSimulation.GetSimTime(), aSimulation);
}

// =================================================================================================
void PrintDateTime(std::ostream& aStream, double aSimTime, const WsfSimulation& aSimulation)
{
#undef GetCurrentTime // Avoid conflict with Windows macro
   UtCalendar cal(aSimulation.GetDateTime().GetCurrentTime(aSimTime));
   aStream << "Year: " << cal.GetYear() << " Month: " << cal.GetMonth() << " Day: " << cal.GetDay()
           << " Hour: " << cal.GetHour() << " Minute: " << cal.GetMinute()
           << " Second: " << (trunc(cal.GetSecond() * 100) / 100);
}

// =================================================================================================
double PrintableAngle(double aAngle)
{
   double angle = aAngle * UtMath::cDEG_PER_RAD;
   if (fabs(aAngle) < 1.0E-12)
   {
      angle = 0.0;
   }
   return angle;
}

// =================================================================================================
void PrintCommData(std::ostream& aStream, wsf::comm::Comm* aCommPtr, bool aShowAddress /* = true*/)
{
   aStream << aCommPtr->GetPlatform()->GetName() << ' ' << aCommPtr->GetName();
   if (aShowAddress)
   {
      aStream << ' ' << aCommPtr->GetAddress();
   }
}

// =================================================================================================
void PrintRouterData(std::ostream& aStream, wsf::comm::router::Router* aRouterPtr)
{
   aStream << aRouterPtr->GetPlatform()->GetName() << ' ' << aRouterPtr->GetName();
}

// =================================================================================================
//! Transform an input string that may contain unprintable characters into one that is 'acceptable'.
// protected
std::string MakePrintableString(const std::string& aInput)
{
   std::string output;

   // Reserve space to prevent constant reallocation
   size_t inputSize = aInput.size();
   output.reserve(3 * inputSize);

   // Skip trailing whitespace
   while (inputSize > 0)
   {
      char ch = aInput[inputSize - 1];
      if ((ch != ' ') && isprint(ch))
      {
         break;
      }
      --inputSize;
   }

   // Skip leading whitespace
   size_t inputPos = 0;
   while (inputPos < inputSize)
   {
      char ch = aInput[inputPos];
      if ((ch != ' ') && isprint(ch))
      {
         break;
      }
      ++inputPos;
   }

   // The transformation does the following:
   //
   // 1) Make sure that each embedded new-line ('\n') is preceded by a '\'. Post-processors rely
   //    on the fact that multi-line events have a '\' at the end of every line except the last.
   // 2) As a side effect of 1), do no emit a '\' as that would indicate a continuation.
   // 3) All other non-printable characters are copied as spaces.
   while (inputPos < inputSize)
   {
      char ch = aInput[inputPos];
      if (isprint(ch))
      {
         if (ch != '\\')
         {
            output.push_back(ch); // Copy all printable characters except '\'
         }
      }
      else if (ch == '\n')
      {
         output.append("\\\n");
      }
      else
      {
         output.push_back(' '); // All other non-printable characters are copied as spaces
      }
      ++inputPos;
   }
   return output;
}

// =================================================================================================
void PrintEM_Interaction(std::ostream& aStream, WsfEM_Interaction& aResult, const Settings& aSettings)
{
   aResult.ComputeUndefinedGeometry();
   bool printOneLine = aSettings.PrintSingleLinePerEvent();
   if (aResult.GetTarget() == nullptr)
   {
      // One way operation involving a transmitter and a receiver.
      if ((aResult.GetTransmitter() != nullptr) && (aResult.GetReceiver() != nullptr))
      {
         PrintEM_AbsXmtrRcvrData(aStream, aResult, "  Xmtr:", aResult.GetTransmitter(), aResult.mXmtrLoc, aSettings);
         PrintEM_AbsXmtrRcvrData(aStream, aResult, "  Rcvr:", aResult.GetReceiver(), aResult.mRcvrLoc, aSettings);
         PrintEM_RelXmtrRcvrData(aStream, aResult, "  Xmtr->Rcvr:", aResult.GetTransmitter(), aResult.mXmtrToRcvr, printOneLine);
         PrintEM_RelXmtrRcvrData(aStream, aResult, "  Rcvr->Xmtr:", aResult.GetReceiver(), aResult.mRcvrToXmtr, printOneLine);
         PrintEM_BeamData(aStream, "  XmtrBeam:", aResult.GetTransmitter(), aResult.mXmtrBeam, printOneLine);
         PrintEM_BeamData(aStream, "  RcvrBeam:", aResult.GetReceiver(), aResult.mRcvrBeam, printOneLine);
      }
   }
   else if (aResult.GetTransmitter() == nullptr)
   {
      // One way operation involving a receiver and a target
      if (aResult.GetReceiver() != nullptr)
      {
         PrintEM_AbsXmtrRcvrData(aStream, aResult, "  Rcvr:", aResult.GetReceiver(), aResult.mRcvrLoc, aSettings);
         PrintEM_AbsTargetData(aStream, aResult, "  Tgt:", aResult.GetTarget(), aResult.mTgtLoc, aSettings);
         PrintEM_RelXmtrRcvrData(aStream, aResult, "  Rcvr->Tgt:", aResult.GetReceiver(), aResult.mRcvrToTgt, printOneLine);
         PrintEM_RelTargetData(aStream, aResult, "  Tgt->Rcvr:", aResult.GetTarget(), aResult.mTgtToRcvr, printOneLine);
         PrintEM_BeamData(aStream, "  RcvrBeam:", aResult.GetReceiver(), aResult.mRcvrBeam, printOneLine);
      }
   }
   else if (aResult.GetReceiver() != nullptr)
   {
      // Two way operation involving a transmitter, target and a receiver.
      if (aResult.mBistatic)
      {
         PrintEM_AbsXmtrRcvrData(aStream, aResult, "  Xmtr:", aResult.GetTransmitter(), aResult.mXmtrLoc, aSettings);
         PrintEM_AbsXmtrRcvrData(aStream, aResult, "  Rcvr:", aResult.GetReceiver(), aResult.mRcvrLoc, aSettings);
         PrintEM_AbsTargetData(aStream, aResult, "  Tgt:", aResult.GetTarget(), aResult.mTgtLoc, aSettings);
         PrintEM_RelXmtrRcvrData(aStream, aResult, "  Xmtr->Tgt:", aResult.GetTransmitter(), aResult.mXmtrToTgt, printOneLine);
         PrintEM_RelTargetData(aStream, aResult, "  Tgt->Xmtr:", aResult.GetTarget(), aResult.mTgtToXmtr, printOneLine);
         PrintEM_RelXmtrRcvrData(aStream, aResult, "  Rcvr->Tgt:", aResult.GetReceiver(), aResult.mRcvrToTgt, printOneLine);
         PrintEM_RelTargetData(aStream, aResult, "  Tgt->Rcvr:", aResult.GetTarget(), aResult.mTgtToRcvr, printOneLine);
      }
      else
      {
         PrintEM_AbsXmtrRcvrData(aStream, aResult, "  Xmtr/Rcvr:", aResult.GetReceiver(), aResult.mRcvrLoc, aSettings);
         PrintEM_AbsTargetData(aStream, aResult, "  Tgt:", aResult.GetTarget(), aResult.mTgtLoc, aSettings);
         PrintEM_RelXmtrRcvrData(aStream, aResult, "  Xmtr/Rcvr->Tgt:", aResult.GetReceiver(), aResult.mRcvrToTgt, printOneLine);
         PrintEM_RelTargetData(aStream, aResult, "  Tgt->Xmtr/Rcvr:", aResult.GetTarget(), aResult.mTgtToRcvr, printOneLine);
      }
      PrintEM_BeamData(aStream, "  XmtrBeam:", aResult.GetTransmitter(), aResult.mXmtrBeam, printOneLine);
      PrintEM_BeamData(aStream, "  RcvrBeam:", aResult.GetReceiver(), aResult.mRcvrBeam, printOneLine);
   }

   if (aResult.mRadarSig > 0.0)
   {
      aStream << "  Radar_Sig: " << UtMath::LinearToDB(aResult.mRadarSig) << " dBsm (" << aResult.mRadarSig << " m^2)";
      aStream << " Az: " << PrintableAngle(aResult.mRadarSigAz) << " deg";
      aStream << " El: " << PrintableAngle(aResult.mRadarSigEl) << " deg" << ContinueChar(printOneLine);
   }

   if (aResult.mOpticalSig > 0.0)
   {
      aStream << "  Optical_Sig: " << UtMath::LinearToDB(aResult.mOpticalSig) << " dBsm (" << aResult.mOpticalSig
              << " m^2)";
      aStream << " Az: " << PrintableAngle(aResult.mOpticalSigAz) << " deg";
      aStream << " El: " << PrintableAngle(aResult.mOpticalSigEl) << " deg";
      if (aResult.mOpticalReflectivity > 0.0)
      {
         aStream << "  Reflectivity: " << aResult.mOpticalReflectivity;
      }
      aStream << ContinueChar(printOneLine);
   }

   if (aResult.mInfraredSig > 0.0)
   {
      aStream << "  Infrared_Sig: " << aResult.mInfraredSig << " w/sr";
      aStream << " Az: " << PrintableAngle(aResult.mInfraredSigAz) << " deg";
      aStream << " El: " << PrintableAngle(aResult.mInfraredSigEl) << " deg" << ContinueChar(printOneLine);
      if (aResult.mSignalToNoise > 0.0)
      {
         aStream << "  Background_radiant_intensity: " << aResult.mBackgroundRadiantIntensity << " w/sr";
         aStream << "  Contrast_radiant_intensity: " << aResult.mContrastRadiantIntensity << " w/sr";
         aStream << "  Transmittance: " << aResult.mAbsorptionFactor << ContinueChar(printOneLine);
      }
   }

   bool printMaskingFactor = ((aResult.mMaskingFactor >= 0.0) && (aResult.mMaskingFactor < 1.0));
   if ((aResult.mAbsorptionFactor > 0.0) || (aResult.mPropagationFactor > 0.0) || printMaskingFactor)
   {
      if (aResult.mAbsorptionFactor > 0.0)
      {
         aStream << "  Absorption_factor: " << UtMath::LinearToDB(aResult.mAbsorptionFactor) << " dB ("
                 << aResult.mAbsorptionFactor << ")";
      }
      if (aResult.mPropagationFactor > 0.0)
      {
         aStream << "  Propagation_factor_F^4: " << UtMath::LinearToDB(aResult.mPropagationFactor) << " dB ("
                 << aResult.mPropagationFactor << ") F: " << pow(aResult.mPropagationFactor, 0.25);
      }
      if (printMaskingFactor)
      {
         aStream << "  Masking_Factor: " << aResult.mMaskingFactor;
      }
      aStream << ContinueChar(printOneLine);
   }

   if ((aResult.mXmtdPower > 0.0) || (aResult.mRcvdPower > 0.0) || (aResult.mPixelCount > 0.0) ||
       (aResult.mSignalToNoise > 0.0))
   {
      aStream << " ";
      if (aResult.mXmtdPower > 0.0)
      {
         aStream << " Xmtd_Power: " << UtMath::LinearToDB(aResult.mXmtdPower) << " dBw";
      }
      if (aResult.mRcvdPower > 0.0)
      {
         aStream << " Rcvd_Power: " << UtMath::LinearToDB(aResult.mRcvdPower) << " dBw";
      }
      if (aResult.mRcvrNoisePower > 0.0)
      {
         aStream << " Rcvr_Noise: " << UtMath::LinearToDB(aResult.mRcvrNoisePower) << " dBw";
      }
      if (aResult.mClutterPower > 0.0)
      {
         aStream << " Clutter_Power: " << UtMath::LinearToDB(aResult.mClutterPower) << " dBw";
      }
      if (aResult.mInterferencePower)
      {
         aStream << ' ' << aSettings.GetInterferencePowerLabel() << ": "
                 << UtMath::LinearToDB(aResult.mInterferencePower) << " dBw";
      }
      const WsfEM_Interaction::ComponentList& components(aResult.GetComponents());
      for (WsfEM_Interaction::ComponentList::Iterator i = components.Begin(); i != components.End(); ++i)
      {
         i->PrintEventOutput_Powers(aStream);
      }
      if (aResult.mPixelCount > 0.0)
      {
         aStream << " Pixel_Count: " << aResult.mPixelCount;
      }
      aStream << ContinueChar(printOneLine);

      if (aResult.mSignalToNoise > 0.0)
      {
         aStream << "  S/I: " << UtMath::LinearToDB(aResult.mSignalToNoise) << " dB";
         if (aResult.mDetectionThreshold > 0.0)
         {
            aStream << " Threshold: " << UtMath::LinearToDB(aResult.mDetectionThreshold) << " dB";
         }
         if (aResult.mRcvrNoisePower > 0.0)
         {
            double noisePower   = aResult.mRcvrNoisePower;
            double clutterPower = std::max(aResult.mClutterPower, 0.0);
            double intfPower    = std::max(aResult.mInterferencePower, 0.0);
            // Signal power used to be aResult.mRcvdPower, but that didn't always contain all the parts that were
            // considered 'signal' (depends on the sensor implementation). The following recovers the signal.
            double signalPower = aResult.mSignalToNoise * (noisePower + clutterPower + intfPower);
            aStream << " S/N: " << UtMath::LinearToDB(signalPower / noisePower) << " dB";
            aStream << " S/(N+C): " << UtMath::LinearToDB(signalPower / (noisePower + clutterPower)) << " dB";
            aStream << ' ' << aSettings.GetSNR_InterferenceLabel() << ": ";
            aStream << UtMath::LinearToDB(signalPower / (noisePower + clutterPower + intfPower)) << " dB";
         }
         aStream << ContinueChar(printOneLine);
      }
   }

   // Process component output.
   const WsfEM_Interaction::ComponentList& components(aResult.GetComponents());
   for (WsfEM_Interaction::ComponentList::Iterator i = components.Begin(); i != components.End(); ++i)
   {
      if (i->PrintEventOutput(aStream))
      {
         aStream << ContinueChar(printOneLine);
      }
   }

   if ((aResult.mFailedStatus & WsfEM_Interaction::cXMTR_RANGE_LIMITS) != 0)
   {
      aStream << "  Xmtr_Range_Limits_Exceeded" << ContinueChar(printOneLine);
   }
   if ((aResult.mFailedStatus & WsfEM_Interaction::cRCVR_RANGE_LIMITS) != 0)
   {
      aStream << "  Rcvr_Range_Limits_Exceeded" << ContinueChar(printOneLine);
   }
   if ((aResult.mFailedStatus & WsfEM_Interaction::cXMTR_ALTITUDE_LIMITS) != 0)
   {
      aStream << "  Xmtr_Altitude_Limits_Exceeded" << ContinueChar(printOneLine);
   }
   if ((aResult.mFailedStatus & WsfEM_Interaction::cRCVR_ALTITUDE_LIMITS) != 0)
   {
      aStream << "  Rcvr_Altitude_Limits_Exceeded" << ContinueChar(printOneLine);
   }
   if ((aResult.mFailedStatus & WsfEM_Interaction::cXMTR_ANGLE_LIMITS) != 0)
   {
      aStream << "  Xmtr_Angle_Limits_Exceeded" << ContinueChar(printOneLine);
   }
   if ((aResult.mFailedStatus & WsfEM_Interaction::cRCVR_ANGLE_LIMITS) != 0)
   {
      aStream << "  Rcvr_Angle_Limits_Exceeded" << ContinueChar(printOneLine);
   }
   if ((aResult.mFailedStatus & WsfEM_Interaction::cXMTR_HORIZON_MASKING) != 0)
   {
      aStream << "  Xmtr_Masked_By_Horizon" << ContinueChar(printOneLine);
   }
   if ((aResult.mFailedStatus & WsfEM_Interaction::cRCVR_HORIZON_MASKING) != 0)
   {
      aStream << "  Rcvr_Masked_By_Horizon" << ContinueChar(printOneLine);
   }
   if ((aResult.mFailedStatus & WsfEM_Interaction::cXMTR_TERRAIN_MASKING) != 0)
   {
      aStream << "  Xmtr_Masked_By_Terrain" << ContinueChar(printOneLine);
   }
   if ((aResult.mFailedStatus & WsfEM_Interaction::cRCVR_TERRAIN_MASKING) != 0)
   {
      aStream << "  Rcvr_Masked_By_Terrain" << ContinueChar(printOneLine);
   }
   if ((aResult.mFailedStatus & WsfEM_Interaction::cSIGNAL_LEVEL) != 0)
   {
      aStream << "  Insufficient_Signal" << ContinueChar(printOneLine);
   }
}

// =================================================================================================
void PrintEM_AbsTargetData(std::ostream&                          aStream,
                           const WsfEM_Interaction&               aResult,
                           const char*                            aPrefix,
                           WsfPlatform*                           aTgtPtr,
                           const WsfEM_Interaction::LocationData& aLocData,
                           const Settings&                        aSettings)
{
   aStream << aPrefix << " Type: " << aTgtPtr->GetType();
   PrintEM_LocationData(aStream, "", aTgtPtr, aLocData, aSettings);
   aStream << ContinueChar(aSettings.PrintSingleLinePerEvent());
}

// =================================================================================================
void PrintEM_RelTargetData(std::ostream&                          aStream,
                           const WsfEM_Interaction&               aResult,
                           const char*                            aPrefix,
                           WsfPlatform*                           aTgtPtr,
                           const WsfEM_Interaction::RelativeData& aRelData,
                           bool                                   aPrintSingleLinePerEvent)
{
   if (aRelData.mRange > 0.0)
   {
      PrintEM_Range(aStream, aPrefix, aRelData.mRange, aPrintSingleLinePerEvent, false);
      double otherLocNED[3];
      aTgtPtr->ConvertWCSVectorToNED(otherLocNED, aRelData.mTrueUnitVecWCS);
      double magnitudeNE = sqrt(otherLocNED[0] * otherLocNED[0] + otherLocNED[1] * otherLocNED[1]);
      double otherAzNED  = atan2(otherLocNED[1], otherLocNED[0]);
      double otherElNED  = atan2(-otherLocNED[2], magnitudeNE);
      aStream << " Brg: " << PrintableAngle(UtMath::NormalizeAngle0_TwoPi(otherAzNED)) << " deg";
      aStream << " El: " << PrintableAngle(otherElNED) << " deg";

      if (aResult.mEarthRadiusScale != 1.0)
      {
         aTgtPtr->ConvertWCSVectorToNED(otherLocNED, aRelData.mUnitVecWCS);
         magnitudeNE = sqrt(otherLocNED[0] * otherLocNED[0] + otherLocNED[1] * otherLocNED[1]);
         otherAzNED  = atan2(otherLocNED[1], otherLocNED[0]);
         otherElNED  = atan2(-otherLocNED[2], magnitudeNE);
         aStream << " Apparent:";
         aStream << " Brg: " << PrintableAngle(UtMath::NormalizeAngle0_TwoPi(otherAzNED)) << " deg";
         aStream << " El: " << PrintableAngle(otherElNED) << " deg";
      }
      aStream << ContinueChar(aPrintSingleLinePerEvent);
   }
}

// =================================================================================================
void PrintEM_AbsXmtrRcvrData(std::ostream&                          aStream,
                             const WsfEM_Interaction&               aResult,
                             const char*                            aPrefix,
                             WsfEM_XmtrRcvr*                        aXmtrRcvrPtr,
                             const WsfEM_Interaction::LocationData& aLocData,
                             const Settings&                        aSettings)
{
   aStream << aPrefix << " Type: " << aXmtrRcvrPtr->GetArticulatedPart()->GetType();
   PrintEM_LocationData(aStream, "", aXmtrRcvrPtr->GetPlatform(), aLocData, aSettings);
   aStream << ContinueChar(aSettings.PrintSingleLinePerEvent());
}

// =================================================================================================
void PrintEM_RelXmtrRcvrData(std::ostream&                          aStream,
                             const WsfEM_Interaction&               aResult,
                             const char*                            aPrefix,
                             WsfEM_XmtrRcvr*                        aXmtrRcvrPtr,
                             const WsfEM_Interaction::RelativeData& aRelData,
                             bool                                   aPrintSingleLinePerEvent)
{
   if (aRelData.mRange > 0.0)
   {
      PrintEM_Range(aStream, aPrefix, aRelData.mRange, aPrintSingleLinePerEvent, false);
      double otherLocNED[3];
      aXmtrRcvrPtr->GetAntenna()->ConvertWCS_VectorToNED(aRelData.mTrueUnitVecWCS, otherLocNED);
      double magnitudeNE = sqrt(otherLocNED[0] * otherLocNED[0] + otherLocNED[1] * otherLocNED[1]);
      double otherAzNED  = atan2(otherLocNED[1], otherLocNED[0]);
      double otherElNED  = atan2(-otherLocNED[2], magnitudeNE);
      aStream << " Brg: " << PrintableAngle(UtMath::NormalizeAngle0_TwoPi(otherAzNED)) << " deg";
      aStream << " El: " << PrintableAngle(otherElNED) << " deg";

      if (aResult.mEarthRadiusScale != 1.0)
      {
         aXmtrRcvrPtr->GetAntenna()->ConvertWCS_VectorToNED(aRelData.mUnitVecWCS, otherLocNED);
         magnitudeNE = sqrt(otherLocNED[0] * otherLocNED[0] + otherLocNED[1] * otherLocNED[1]);
         otherAzNED  = atan2(otherLocNED[1], otherLocNED[0]);
         otherElNED  = atan2(-otherLocNED[2], magnitudeNE);
         aStream << " Apparent:";
         aStream << " Brg: " << PrintableAngle(UtMath::NormalizeAngle0_TwoPi(otherAzNED)) << " deg";
         aStream << " El: " << PrintableAngle(otherElNED) << " deg";
      }
      aStream << ContinueChar(aPrintSingleLinePerEvent);
   }
}

// =================================================================================================
void PrintEM_BeamData(std::ostream&                      aStream,
                      const char*                        aPrefix,
                      WsfEM_XmtrRcvr*                    aXmtrRcvrPtr,
                      const WsfEM_Interaction::BeamData& aBeamData,
                      bool                               aPrintSingleLinePerEvent)
{
   if (aBeamData.mGain >= 0.0)
   {
      double beamPointNED[3];
      aXmtrRcvrPtr->GetAntenna()->ConvertWCS_VectorToNED(aBeamData.mWCS_ToBeamTransform[0], beamPointNED);
      double magnitudeNE = sqrt(beamPointNED[0] * beamPointNED[0] + beamPointNED[1] * beamPointNED[1]);
      double beamPointAz = atan2(beamPointNED[1], beamPointNED[0]);
      double beamPointEl = atan2(-beamPointNED[2], magnitudeNE);
      aStream << aPrefix << " Brg: " << PrintableAngle(UtMath::NormalizeAngle0_TwoPi(beamPointAz)) << " deg";
      aStream << " El: " << PrintableAngle(beamPointEl) << " deg";
      aStream << " TgtRel";
      aStream << " Az: " << PrintableAngle(aBeamData.mAz) << " deg";
      aStream << " El: " << PrintableAngle(aBeamData.mEl) << " deg";
      if (aBeamData.mGain > 0.0)
      {
         if (aXmtrRcvrPtr->GetAntenna()->GetEBS_Mode() != WsfEM_Antenna::cEBS_NONE)
         {
            double cosTheta = cos(aBeamData.mEBS_Az) * cos(aBeamData.mEBS_El);
            cosTheta        = std::max(std::min(cosTheta, 1.0), -1.0); // For safety
            aStream << " EBS Az: " << PrintableAngle(aBeamData.mEBS_Az) << " deg";
            aStream << " El: " << PrintableAngle(aBeamData.mEBS_El) << " deg";
            aStream << " Omega: " << PrintableAngle(acos(cosTheta)) << " deg";
         }
         aStream << " Gain: " << UtMath::LinearToDB(aBeamData.mGain) << " dB";
      }
      aStream << ContinueChar(aPrintSingleLinePerEvent);
   }
}

// =================================================================================================
void PrintEM_LocationData(std::ostream&                          aStream,
                          const char*                            aPrefix,
                          WsfPlatform*                           aPlatformPtr,
                          const WsfEM_Interaction::LocationData& aLocationData,
                          const Settings&                        aSettings)
{
   double lat;
   double lon;
   double alt;
   double locationECI[3];

   aStream << aPrefix;
   if (aLocationData.mIsValid)
   {
      UtEllipsoidalEarth::ConvertECEFToLLA(aLocationData.mLocWCS, lat, lon, alt);
      aPlatformPtr->ConvertWCSToECI(aLocationData.mLocWCS,
                                    locationECI); // using the last update time of the platform.
   }
   else
   {
      aPlatformPtr->GetLocationLLA(lat, lon, alt);
      aPlatformPtr->GetLocationECI(locationECI);
   }

   if (aSettings.PrintLLA_Locations())
   {
      PrintLocationDataLLA(aStream, lat, lon, alt, aSettings.GetLatLonFormat());
   }
   else // Use ECI
   {
      PrintLocationDataECI(aStream, locationECI);
   }

   double yaw;
   double pitch;
   double roll;
   aPlatformPtr->GetOrientationNED(yaw, pitch, roll);
   aStream << " Heading: " << PrintableAngle(yaw) << " deg";
   aStream << " Pitch: " << PrintableAngle(pitch) << " deg";
   aStream << " Roll: " << PrintableAngle(roll) << " deg";

   aStream << " Speed: " << aPlatformPtr->GetSpeed() << " m/s";
}

// =================================================================================================
void PrintEM_Range(std::ostream& aStream,
                   const char*   aPrefix,
                   double        aRange,
                   bool          aPrintSingleLinePerEvent,
                   bool          aWriteNewLine /* = true */)
{
   aStream << aPrefix << " Range: " << aRange * 0.001 << " km (" << aRange / UtMath::cM_PER_NM << " nm)";
   if (aWriteNewLine)
   {
      aStream << ContinueChar(aPrintSingleLinePerEvent);
   }
}

// =================================================================================================
//! Like the public PrintLLA, but also includes the " LLA: " prefix.
//! (Didn't want to potentially mess up somebody else's code).
void PrintLocationDataLLA(std::ostream& aStream, double aLat, double aLon, double aAlt, int aLatLonFormat)
{
   aStream << " LLA: " << UtLatPos(aLat, aLatLonFormat) << ' ' << UtLonPos(aLon, aLatLonFormat) << ' ';
   std::streamsize oldPrecision = aStream.precision(8);
   aStream << aAlt << " m";
   aStream.precision(oldPrecision);
}

// =================================================================================================
// Like the other PrintLocationData, but location is gotten from the platform.
void PrintLocationData(std::ostream& aStream, UtEntity* aPlatformPtr, const Settings& aSettings)
{
   if (aSettings.PrintLLA_Locations())
   {
      double lat;
      double lon;
      double alt;
      aPlatformPtr->GetLocationLLA(lat, lon, alt);
      PrintLocationDataLLA(aStream, lat, lon, alt, aSettings.GetLatLonFormat());
   }
   else // Use ECI
   {
      double locationECI[3];
      aPlatformPtr->GetLocationECI(locationECI);
      PrintLocationDataECI(aStream, locationECI);
   }
}

// =================================================================================================
//! Like PrintLocationDataLLA, but outputs ECI values instead.
void PrintLocationDataECI(std::ostream& aStream, const double aLocationECI[3])
{
   std::streamsize    oldPrecision = aStream.precision(8);
   std::ios::fmtflags oldFlags     = aStream.setf(std::ios::scientific, std::ios::floatfield);
   aStream << " ECI Location: ";
   aStream << " m * [ " << aLocationECI[0] << ' ' << aLocationECI[1] << ' ' << aLocationECI[2] << " ]";
   aStream.precision(oldPrecision);
   aStream.setf(oldFlags);
}

// =================================================================================================
//! Print the 'non-location' parts of the kinematic data (orientation, velocity and acceleration).
//!
//! It is assumed this is called immediately after PrintLocationData. The orientation data will appear
//! on the same line as the location data and the velocity and acceleration will appear on the next line.
void PrintNonLocationData(std::ostream& aStream, WsfPlatform* aPlatformPtr, const std::string& aIndent, const Settings& aSettings)
{
   std::ios::fmtflags oldFlags     = aStream.flags();
   std::streamsize    oldPrecision = aStream.precision(3);
   aStream.setf(std::ios::fixed, std::ios::floatfield);

   double heading;
   double pitch;
   double roll;
   aPlatformPtr->GetOrientationNED(heading, pitch, roll);
   aStream << " Heading: " << PrintableAngle(UtMath::NormalizeAngle0_TwoPi(heading)) << " deg";
   aStream << " Pitch: " << PrintableAngle(pitch) << " deg";
   aStream << " Roll: " << PrintableAngle(roll) << " deg";

   double vec[3];
   double vecMag;

   if (aSettings.PrintLLA_Locations()) // use NED velocity
   {
      aPlatformPtr->GetVelocityNED(vec);
   }
   else // use ECI velocity
   {
      aPlatformPtr->GetVelocityECI(vec);
   }
   vecMag = UtVec3d::Normalize(vec);
   aStream << ContinueChar(aSettings.PrintSingleLinePerEvent()) << aIndent << " Speed: " << vecMag << " m/s * [ "
           << vec[0] << ' ' << vec[1] << ' ' << vec[2] << " ]";

   if (aSettings.PrintLLA_Locations()) // use NED velocity
   {
      aPlatformPtr->GetAccelerationNED(vec);
   }
   else
   {
      aPlatformPtr->GetAccelerationECI(vec);
   }
   vecMag = UtVec3d::Normalize(vec);
   aStream << " Acceleration: " << vecMag << " m/s2 * [ " << vec[0] << ' ' << vec[1] << ' ' << vec[2] << " ]";

   aStream.flags(oldFlags);
   aStream.precision(oldPrecision);
}

// =================================================================================================
void PrintMessageData(std::ostream&     aStream,
                      double            aSimTime,
                      const WsfMessage& aMessage,
                      WsfSimulation&    aSimulation,
                      const Settings&   aSettings)
{
   WsfStringId messageType = aMessage.GetType();
   if (const auto* trackMsg = dynamic_cast<const WsfTrackMessage*>(&aMessage))
   {
      if (trackMsg->GetTrack())
      {
         aStream << ContinueChar(aSettings.PrintSingleLinePerEvent());
         aStream << "  TrackId: " << trackMsg->GetTrack()->GetTrackId();
         if (aSettings.PrintTrackInMessage())
         {
            PrintTrackData(aStream, aSimTime, trackMsg->GetTrack(), aSimulation, aSettings);
         }
         else
         {
            PrintTrackDataBrief(aStream, aSimTime, trackMsg->GetTrack(), aSettings.GetTimeFormat());
         }
      }
   }
   else if (const auto* trackDropMsg = dynamic_cast<const WsfTrackDropMessage*>(&aMessage))
   {
      aStream << " TrackId: " << trackDropMsg->GetTrackId();
   }
   else if (const auto* statusMsg = dynamic_cast<const WsfStatusMessage*>(&aMessage))
   {
      aStream << ContinueChar(aSettings.PrintSingleLinePerEvent()) << "  Status: " << statusMsg->GetStatus();
      if (!statusMsg->GetRequestId().IsNull())
      {
         aStream << " RequestId: " << statusMsg->GetRequestId();
      }
      if (statusMsg->GetSystemNameId() != 0)
      {
         aStream << " System: " << statusMsg->GetSystemName();
      }
      if (statusMsg->GetPlatform() != nullptr)
      {
         aStream << " Platform: " << statusMsg->GetPlatform()->GetName();
      }
   }
   else if (const auto* taskAssignMsg = dynamic_cast<const WsfTaskAssignMessage*>(&aMessage))
   {
      aStream << ContinueChar(aSettings.PrintSingleLinePerEvent());
      aStream << "  TrackId: " << taskAssignMsg->GetTrack().GetTrackId();
      if (aSettings.PrintTrackInMessage())
      {
         PrintTrackData(aStream, aSimTime, &(taskAssignMsg->GetTrack()), aSimulation, aSettings);
      }
   }
   else
   {
      auto i = aSettings.GetMessagePrinters().find(messageType);
      if (i != aSettings.GetMessagePrinters().end())
      {
         (i->second)(aSimTime, aMessage);
      }
   }
}

// =================================================================================================
//! Perform common print processing for the "PLATFORM_" messages.
void PrintPlatformEvent(std::ostream& aStream, WsfPlatform* aPlatformPtr, bool aShowPs, const Settings& aSettings)
{
   aStream << aPlatformPtr->GetName() << " Type: " << aPlatformPtr->GetType() << " Side: " << aPlatformPtr->GetSide();
   if (aShowPs)
   {
      aStream << " Ps: " << 1.0 - aPlatformPtr->GetDamageFactor();
   }
   aStream << ContinueChar(aSettings.PrintSingleLinePerEvent());
   if (aPlatformPtr->IsInitialized())
   {
      PrintLocationData(aStream, aPlatformPtr, aSettings);
      PrintNonLocationData(aStream, aPlatformPtr, "", aSettings);
   }
}

// =================================================================================================
void PrintSensorResult(std::ostream& aStream, WsfSensor* aSensorPtr, WsfSensorResult& aResult, const Settings& aSettings)
{
   bool printOneLine = aSettings.PrintSingleLinePerEvent();
   aStream << " Sensor: " << aSensorPtr->GetName();
   if (aSensorPtr->GetModeCount() > 0)
   {
      aStream << " Mode: " << aSensorPtr->GetModeName(aResult.mModeIndex);
   }
   else
   {
      aStream << " Mode: <none>";
   }
   aStream << " Beam: " << aResult.mBeamIndex + 1 << ContinueChar(printOneLine);

   PrintEM_Interaction(aStream, aResult, aSettings);

   if ((aResult.mFailedStatus & WsfSensorResult::cCONCEALMENT) != 0)
   {
      aStream << "  Target_Concealed" << ContinueChar(printOneLine);
   }

   if ((aResult.mFailedStatus & WsfSensorResult::cDOPPLER_LIMITS) != 0)
   {
      aStream << "  Doppler_Limits_Exceeded" << ContinueChar(printOneLine);
   }

   if ((aResult.mFailedStatus & WsfSensorResult::cVELOCITY_LIMITS) != 0)
   {
      aStream << "  Velocity_Limits_Exceeded" << ContinueChar(printOneLine);
   }

   if ((aResult.mFailedStatus & WsfSensorResult::cTARGET_DELETED) != 0)
   {
      aStream << "  Target_Deleted" << ContinueChar(printOneLine);
   }

   if ((aResult.mFailedStatus & WsfSensorResult::cOTH_LIMITS) != 0)
   {
      aStream << "  OTH_Limits_Exceeded" << ContinueChar(printOneLine);
   }

   if ((aResult.mFailedStatus & WsfSensorResult::cEXCLUSION_SOLAR) != 0)
   {
      aStream << "  Solar_Exclusion" << ContinueChar(printOneLine);
   }

   if ((aResult.mFailedStatus & WsfSensorResult::cEXCLUSION_LUNAR) != 0)
   {
      aStream << "  Lunar_Exclusion" << ContinueChar(printOneLine);
   }

   if ((aResult.mFailedStatus & WsfSensorResult::cMOON_BLOCKED) != 0)
   {
      aStream << "  Blocked_By_Moon" << ContinueChar(printOneLine);
   }

   if ((aResult.mFailedStatus & WsfSensorResult::cDETECTION_STOP) != 0)
   {
      aStream << "  Detection_Stopped" << ContinueChar(printOneLine);
   }

   if ((aResult.mFailedStatus & WsfSensorResult::cSCRIPT_CONSTRAINT) != 0)
   {
      aStream << "  Blocked_By_Script" << ContinueChar(printOneLine);
   }

   // A successful detection (mFailedStatus == 0) is valid only if something was
   // actually checked (mCheckedStatus != 0).
   // bool detected = ((aResult.mCheckedStatus != 0) &&
   //                 (aResult.mFailedStatus  == 0) &&
   //                 (aResult.mPd >= aResult.mRequiredPd));
   aStream << "  Pd: " << aResult.mPd << " RequiredPd: " << aResult.mRequiredPd << " Detected: " << aResult.Detected();
}

// =================================================================================================
//! Print common information associated with turning a system off or on.
void PrintSystemOffOnEvent(std::ostream&      aStream,
                           double             aSimTime,
                           const std::string& aEvent,
                           const std::string& aSystemType,
                           WsfPlatformPart*   aSystemPtr,
                           const Settings&    aSettings)
{
   PrintTime(aStream, aSimTime, aSettings.GetTimeFormat());
   aStream << aEvent << aSystemPtr->GetPlatform()->GetName() << aSystemType << aSystemPtr->GetName()
           << " Type: " << aSystemPtr->GetType() << ContinueChar(aSettings.PrintSingleLinePerEvent());
   PrintLocationData(aStream, aSystemPtr->GetPlatform(), aSettings);
   PrintNonLocationData(aStream, aSystemPtr->GetPlatform(), "", aSettings);
   aStream << '\n';
}

// =================================================================================================
void PrintTrackCovarianceData(std::ostream&        aStream,
                              double               aSimTime,
                              const std::string&   aCovarianceType,
                              const WsfCovariance* aCovariancePtr,
                              const WsfTrack*      aTrackPtr,
                              bool                 aPrintSingleLinePerEvent)
{
   aStream << ContinueChar(aPrintSingleLinePerEvent) << "  " << aCovarianceType << ": ";
   if (aCovariancePtr != nullptr)
   {
      WsfCovariance covar(*aCovariancePtr);
      double        originatorLocWCS[3];
      aTrackPtr->GetOriginatorLocationWCS(originatorLocWCS);
      double lat, lon, alt;
      UtEllipsoidalEarth::ConvertECEFToLLA(originatorLocWCS, lat, lon, alt);
      double locWCS[3];
      double WCS_ToNED_Transform[3][3];
      UtEllipsoidalEarth::ComputeNEDTransform(lat, lon, alt, WCS_ToNED_Transform, locWCS);
      WsfCovariance::ConvertFromFrameB_ToFrameA(covar, WCS_ToNED_Transform);

      // Now that we have an ned transform, copy the NE part to a 2x2 vector:
      std::vector<double> neVec;
      neVec.push_back(covar(0, 0));
      neVec.push_back(covar(0, 1));
      neVec.push_back(covar(1, 0));
      neVec.push_back(covar(1, 1));
      double semiMajorAxis, semiMinorAxis, bearing;
      WsfUtil::CovarianceToEllipse_2x2(neVec, semiMajorAxis, semiMinorAxis, bearing);
      aStream << "Major Axis: " << semiMajorAxis * 2.0 << " m Minor Axis: " << semiMinorAxis * 2.0
              << " m Bearing: " << bearing * UtMath::cDEG_PER_RAD << " deg" << ContinueChar(aPrintSingleLinePerEvent);
      covar.Stream(aStream, true); // Stream the NED covar, not the WCS covar, as this should be more intuitive.
   }
   else
   {
      aStream << " (None)";
   }
}

void PrintAuxData(std::ostream& aStream, const UtAttributeContainer* aAuxDataPtr)
{
   UtAttributeBase::ConstIterator iter(*aAuxDataPtr);
   while (iter->HasNext())
   {
      const UtAttributeBase& attr = *iter;
      if (attr.IsContainerType())
      {
         // Print recursively.
         PrintAuxData(aStream, static_cast<const UtAttributeContainer*>(&(*iter)));
      }
      const std::string& name = attr.GetName();
      UtVariant          var;
      aStream << " (" << name << ":";
      if (attr.GetVariant(var))
      {
         aStream << var.ToString();
      }
      aStream << ')';
      iter->Next();
   }
}

// =================================================================================================
void PrintTrackData(std::ostream&        aStream,
                    double               aSimTime,
                    const WsfTrack*      aTrackPtr,
                    const WsfSimulation& aSimulation,
                    const Settings&      aSettings)
{
   bool   printOneLine = aSettings.PrintSingleLinePerEvent();
   double locationWCS[3];
   double trackLocNED[3] = {0.0, 0.0, 0.0};
   double truthLocNED[3] = {0.0, 0.0, 0.0};

   aStream << ContinueChar(printOneLine);
   aStream << "  Start_Time: " << UtTime(aTrackPtr->GetStartTime(), aSettings.GetTimeFormat());
   aStream << " Update_Time: " << UtTime(aTrackPtr->GetUpdateTime(), aSettings.GetTimeFormat());
   aStream << " Update_Count: " << aTrackPtr->GetUpdateCount() << " Quality: " << aTrackPtr->GetTrackQuality();
   aStream << " Domain: " << WsfTypes::EnumToString(aTrackPtr->GetSpatialDomain());

   aStream << " Type: ";
   switch (aTrackPtr->GetTrackType())
   {
   case WsfTrack::cFILTERED_SENSOR:
      aStream << 'F';
      break;
   case WsfTrack::cUNFILTERED_SENSOR:
      aStream << 'M';
      break;
   case WsfTrack::cPREDEFINED:
      aStream << 'I';
      break;
   case WsfTrack::cPROCESSED:
      aStream << 'P';
      break;
   case WsfTrack::cSTATIC_IMAGE:
      aStream << 'S';
      break;
   case WsfTrack::cPSEUDO_SENSOR:
      aStream << 'V';
      break;
   default:
      aStream << 'U';
      break;
   }

   if (aTrackPtr->IsCandidate())
   {
      aStream << 'C';
   }

   if (aTrackPtr->IsFalseTarget())
   {
      aStream << 'F';
   }

   aTrackPtr->GetOriginatorLocationWCS(locationWCS);
   UtEntity originator; // TODO MJM This is very inefficient; rework to not use UtEntity explictly.
   originator.SetLocationWCS(locationWCS);
   if (!aSettings.PrintLLA_Locations())
   {
      // TODO modify WsfDateTime to keep a current calendar (?)
      // UtCalendar& cal = WsfDateTime::GetCurrentTime(aTrackPtr->GetUpdateTime());
      originator.SetECI_ReferenceEpoch(aSimulation.GetDateTime().GetStartDateAndTime());
      originator.SetTime(aTrackPtr->GetUpdateTime());
   }

   WsfPlatform* platformPtr = aSimulation.GetPlatformByIndex(aTrackPtr->GetTargetIndex());
   if (platformPtr != nullptr)
   {
      // Make sure the target's platform location is up-to-date.
      // Making the call here ensures that there is no extra processing if
      // we do not use the event output.
      platformPtr->Update(aSimTime);
      originator.GetRelativeLocationNED(platformPtr, truthLocNED);
      aStream << ContinueChar(printOneLine) << "  Target_Truth: Name: " << platformPtr->GetName()
              << " Type: " << platformPtr->GetType() << " Side: " << platformPtr->GetSide();
   }

   aStream << ContinueChar(printOneLine) << "  Originator:";
   PrintLocationData(aStream, &originator, aSettings);

   bool showLoc2D = false;
   bool showLoc3D = false;
   if (aTrackPtr->LocationValid())
   {
      showLoc3D = true;

      double trackLocWCS[3];
      aTrackPtr->GetLocationWCS(trackLocWCS);
      originator.ConvertWCSToNED(trackLocWCS, trackLocNED);
   }
   else if (aTrackPtr->RangeValid() && aTrackPtr->BearingValid())
   {
      if (aTrackPtr->ElevationValid())
      {
         showLoc3D      = true;
         double ne      = aTrackPtr->GetRange() * cos(aTrackPtr->GetElevation());
         double d       = -aTrackPtr->GetRange() * sin(aTrackPtr->GetElevation());
         trackLocNED[0] = ne * cos(aTrackPtr->GetBearing());
         trackLocNED[1] = ne * sin(aTrackPtr->GetBearing());
         trackLocNED[2] = d;
      }
      else
      {
         showLoc2D      = true;
         double ne      = aTrackPtr->GetRange();
         trackLocNED[0] = ne * cos(aTrackPtr->GetBearing());
         trackLocNED[1] = ne * sin(aTrackPtr->GetBearing());
         trackLocNED[2] = 0.0;
      }
   }

   if (showLoc2D || showLoc3D)
   {
      double lat;
      double lon;
      double alt;
      double locationECI[3];

      if (showLoc3D)
      {
         aStream << ContinueChar(printOneLine) << "  Track:";
         if (aSettings.PrintLLA_Locations())
         {
            originator.ConvertNEDToLLA(trackLocNED, lat, lon, alt);
            PrintLocationDataLLA(aStream, lat, lon, alt, aSettings.GetLatLonFormat());
         }
         else
         {
            originator.ConvertNEDToECI(trackLocNED, locationECI);
            PrintLocationDataECI(aStream, locationECI);
         }
      }
      else if (aSettings.PrintLLA_Locations())
      {
         aStream << ContinueChar(printOneLine) << "  Track:";
         originator.ConvertNEDToLLA(trackLocNED, lat, lon, alt);
         alt = 0.0;
         PrintLocationDataLLA(aStream, lat, lon, alt, aSettings.GetLatLonFormat());
      }

      aStream << " Flags: ";
      if (aTrackPtr->LocationValid())
      {
         aStream << 'L';
      }
      if (aTrackPtr->Is3D())
      {
         aStream << '3';
      }
      if (aTrackPtr->RangeValid())
      {
         aStream << 'R';
      }
      if (aTrackPtr->BearingValid())
      {
         aStream << 'B';
      }
      if (aTrackPtr->ElevationValid())
      {
         aStream << 'E';
      }

      if (platformPtr != nullptr)
      {
         aStream << ContinueChar(printOneLine) << "  Truth:";

         if (aSettings.PrintLLA_Locations())
         {
            platformPtr->GetLocationLLA(lat, lon, alt);
            PrintLocationDataLLA(aStream, lat, lon, alt, aSettings.GetLatLonFormat());
         }
         else
         {
            platformPtr->GetLocationECI(locationECI);
            PrintLocationDataECI(aStream, locationECI);
         }
         double deltaLocNED[3];
         UtVec3d::Subtract(deltaLocNED, trackLocNED, truthLocNED);
         if (showLoc2D)
         {
            deltaLocNED[2] = 0.0;
         }
         aStream << "  Difference: " << UtVec3d::Magnitude(deltaLocNED) << " m ";
      }
   }

   if (aTrackPtr->VelocityValid())
   {
      double trackVelWCS[3];

      aTrackPtr->GetVelocityWCS(trackVelWCS);
      aStream << ContinueChar(printOneLine) << "  Track: Vel: " << UtVec3d::Magnitude(trackVelWCS) << " m/s";
      double trackLocWCS[3];
      if (aTrackPtr->GetExtrapolatedLocationWCS(aSimTime, trackLocWCS))
      {
         UtEntity entity;
         entity.SetLocationWCS(trackLocWCS);
         entity.SetVelocityWCS(trackVelWCS);
         double trackVelNED[3];
         entity.GetVelocityNED(trackVelNED);
         double heading = atan2(trackVelNED[1], trackVelNED[0]);
         aStream << " Hdg: " << PrintableAngle(UtMath::NormalizeAngle0_TwoPi(heading)) << " deg";
      }
      if (platformPtr != nullptr)
      {
         double truthVelNED[3];
         double heading;
         double pitch;
         double roll;
         platformPtr->GetVelocityNED(truthVelNED);
         platformPtr->GetOrientationNED(heading, pitch, roll);
         aStream << "  Truth: Vel: " << UtVec3d::Magnitude(truthVelNED) << " m/s";
         aStream << " Hdg: " << PrintableAngle(UtMath::NormalizeAngle0_TwoPi(heading)) << " deg";
      }
   }

   // Display range, bearing, elevation data.

   std::string prefix = ContinueChar(printOneLine) + " ";
   if (aTrackPtr->LocationValid() || aTrackPtr->RangeValid() || aTrackPtr->BearingValid() || aTrackPtr->ElevationValid())
   {
      // First the 'track' data.
      prefix = ContinueChar(printOneLine) + "  Track:";
      if (aTrackPtr->RangeValid())
      {
         aStream << prefix << " Range: " << aTrackPtr->GetRange() << " m";
         prefix = "";
      }
      else if (aTrackPtr->LocationValid())
      {
         aStream << prefix << " Range: " << UtVec3d::Magnitude(trackLocNED) << " m";
         prefix = "";
      }

      if (aTrackPtr->BearingValid())
      {
         double bearing = UtMath::NormalizeAngle0_TwoPi(aTrackPtr->GetBearing());
         aStream << prefix << " Bearing: " << PrintableAngle(bearing) << " deg";
         prefix = "";
      }
      else if (aTrackPtr->LocationValid())
      {
         double bearing = atan2(trackLocNED[1], trackLocNED[0]);
         bearing        = UtMath::NormalizeAngle0_TwoPi(bearing);
         aStream << prefix << " Bearing: " << PrintableAngle(bearing) << " deg";
         prefix = "";
      }

      if (aTrackPtr->ElevationValid())
      {
         aStream << prefix << " Elevation: " << PrintableAngle(aTrackPtr->GetElevation()) << " deg";
         prefix = "";
      }
      else if (aTrackPtr->LocationValid())
      {
         double range     = UtVec3d::Magnitude(trackLocNED);
         double elevation = 0.0;
         if (range > 0.0)
         {
            elevation = asin(-trackLocNED[2] / range);
         }
         aStream << prefix << " Elevation: " << PrintableAngle(elevation) << " deg";
         prefix = "";
      }

      // Then the 'truth' data.

      if (platformPtr != nullptr)
      {
         prefix = ContinueChar(printOneLine) + "  Truth:";
         if (aTrackPtr->RangeValid() || aTrackPtr->LocationValid())
         {
            aStream << prefix << " Range: " << UtVec3d::Magnitude(truthLocNED) << " m";
            prefix = "";
         }
         if (aTrackPtr->BearingValid() || aTrackPtr->LocationValid())
         {
            double bearing = atan2(truthLocNED[1], truthLocNED[0]);
            bearing        = UtMath::NormalizeAngle0_TwoPi(bearing);
            aStream << prefix << " Bearing: " << PrintableAngle(bearing) << " deg";
            prefix = "";
         }
         if (aTrackPtr->ElevationValid() || aTrackPtr->LocationValid())
         {
            double range     = UtVec3d::Magnitude(truthLocNED);
            double elevation = 0.0;
            if (range > 0.0)
            {
               elevation = asin(-truthLocNED[2] / range);
            }
            aStream << prefix << " Elevation: " << PrintableAngle(elevation) << " deg";
            prefix = "";
         }
      }

      // Print measurement errors for unfiltered sensor reports
      if ((aTrackPtr->GetTrackType() == WsfTrack::cUNFILTERED_SENSOR) &&
          ((aTrackPtr->GetRangeError() != 0.0) || (aTrackPtr->GetBearingError() != 0.0) ||
           (aTrackPtr->GetElevationError() != 0.0)))
      {
         aStream << ContinueChar(printOneLine) << "  Measurement_Error_Sigma:";
         if (aTrackPtr->GetRangeError() != 0.0)
         {
            aStream << " Range: " << aTrackPtr->GetRangeError() << " m";
         }
         if (aTrackPtr->GetBearingError() != 0.0)
         {
            aStream << " Bearing: " << PrintableAngle(aTrackPtr->GetBearingError()) << " deg";
         }
         if (aTrackPtr->GetElevationError() != 0.0)
         {
            aStream << " Elevation: " << PrintableAngle(aTrackPtr->GetElevationError()) << " deg";
         }
      }
   }

   prefix = ContinueChar(printOneLine) + " ";
   if (aTrackPtr->TypeIdsValid() && (aTrackPtr->GetTypeIds().size() > 1))
   {
      aStream << prefix << " Type_IDs: [ ";
      for (const UtStringIdInformation& type : aTrackPtr->GetTypeIds())
      {
         aStream << type.Get() << " (" << type.GetQuality() << ") ";
      }
      aStream << "]";
      prefix = "";
   }
   else if (aTrackPtr->TypeIdValid())
   {
      aStream << prefix << " Type_ID: " << aTrackPtr->GetTypeId();
      prefix = "";
   }
   if (aTrackPtr->SideIdValid())
   {
      aStream << prefix << " Side_ID: " << aTrackPtr->GetSideId();
      prefix = "";
   }
   if (aTrackPtr->SignalToNoiseValid())
   {
      aStream << prefix << " Signal-To-Noise: " << UtMath::SafeLinearToDB(aTrackPtr->GetSignalToNoise()) << " dB";
      prefix = "";
   }
   if (aTrackPtr->GetPixelCount() > 0.0)
   {
      aStream << prefix << " Pixel_Count: " << aTrackPtr->GetPixelCount();
      prefix = "";
   }

   if (aTrackPtr->FrequencyValid())
   {
      unsigned int count = aTrackPtr->GetSignalCount();
      if (count != 0)
      {
         aStream << ContinueChar(printOneLine) << "  Frequency: [";
         WsfTrack::Signal signal;
         for (unsigned int index = 0; index < count; ++index)
         {
            aTrackPtr->GetSignalEntry(index, signal);
            aStream << " (";
            double frequency = signal.mLowerFrequency;
            if (frequency >= 1.0E+9)
            {
               aStream << frequency * 1.0E-9 << " GHz";
            }
            else if (frequency >= 1.0E+6)
            {
               aStream << frequency * 1.0E-6 << " MHz";
            }
            else if (frequency >= 1.0E+3)
            {
               aStream << frequency * 1.0E-3 << " kHz";
            }
            else
            {
               aStream << frequency << " Hz";
            }
            aStream << ' ';
            frequency = signal.mUpperFrequency;
            if (frequency >= 1.0E+9)
            {
               aStream << frequency * 1.0E-9 << " GHz";
            }
            else if (frequency >= 1.0E+6)
            {
               aStream << frequency * 1.0E-6 << " MHz";
            }
            else if (frequency >= 1.0E+3)
            {
               aStream << frequency * 1.0E-3 << " kHz";
            }
            else
            {
               aStream << frequency << " Hz";
            }
            aStream << ')';
         }
         aStream << " ]";
      }
   }

   if (aTrackPtr->HasAuxData())
   {
      aStream << ContinueChar(printOneLine) << "  Aux_Data:";
      const WsfAttributeContainer* auxDataPtr = &aTrackPtr->GetAuxDataConst();
      PrintAuxData(aStream, auxDataPtr);
   }
   if (aSettings.PrintTrackCovariance())
   {
      if (aTrackPtr->MeasurementCovarianceValid())
      {
         PrintTrackCovarianceData(aStream,
                                  aSimTime,
                                  "Measurement_Covariance",
                                  aTrackPtr->GetMeasurementCovariance(),
                                  aTrackPtr,
                                  printOneLine);
      }
      if (aTrackPtr->StateCovarianceValid())
      {
         PrintTrackCovarianceData(aStream, aSimTime, "State_Covariance", aTrackPtr->GetStateCovariance(), aTrackPtr, printOneLine);
      }
   }
   if (aSettings.PrintTrackResidualCovariance())
   {
      if (aTrackPtr->ResidualCovarianceValid())
      {
         PrintTrackCovarianceData(aStream,
                                  aSimTime,
                                  "Residual_Covariance",
                                  aTrackPtr->GetResidualCovariance(),
                                  aTrackPtr,
                                  printOneLine);
      }
   }
}

// =================================================================================================
void PrintTrackDataBrief(std::ostream& aStream, double aSimTime, const WsfTrack* aTrackPtr, int aTimeFormat)
{
   aStream << " Update_Time: " << UtTime(aTrackPtr->GetUpdateTime(), aTimeFormat);
   aStream << " Update_Count: " << aTrackPtr->GetUpdateCount() << " Quality: " << aTrackPtr->GetTrackQuality();

   aStream << " Type: ";
   switch (aTrackPtr->GetTrackType())
   {
   case WsfTrack::cFILTERED_SENSOR:
      aStream << 'F';
      break;
   case WsfTrack::cUNFILTERED_SENSOR:
      aStream << 'M';
      break;
   case WsfTrack::cPREDEFINED:
      aStream << 'I';
      break;
   case WsfTrack::cPROCESSED:
      aStream << 'P';
      break;
   default:
      aStream << 'U';
      break;
   }
   if (aTrackPtr->IsCandidate())
   {
      aStream << 'C';
   }
   if (aTrackPtr->IsFalseTarget())
   {
      aStream << 'F';
   }
}

// =================================================================================================
//! Add the target name to the current line.
//! If the target name is empty then just print the track ID in '(..)'.
void PrintTrackTargetName(std::ostream& aStream, const WsfTrack* aTrackPtr, const WsfSimulation& aSimulation)
{
   if (aTrackPtr == nullptr)
   {
      aStream << " UNKNOWN";
   }
   else if (aTrackPtr->GetTargetName() != 0)
   {
      aStream << ' ' << aTrackPtr->GetTargetName();
   }
   else
   {
      WsfStringId nameId = aSimulation.GetPlatformNameId(aTrackPtr->GetTargetIndex());
      if (!nameId.IsNull())
      {
         aStream << ' ' << nameId;
      }
      else
      {
         aStream << " (" << aTrackPtr->GetTrackId() << ')';
      }
   }
}

// =================================================================================================
void PrintEvent(std::ostream&      aStream,
                double             aSimTime,
                const std::string& aSubjectName,
                const std::string& aEventName,
                const std::string& aObjectName,
                const Settings&    aSettings,
                const std::string& aAdditionalInformation)
{
   PrintTime(aStream, aSimTime, aSettings.GetTimeFormat());
   aStream << aEventName << ' ' << aSubjectName << ' ' << aObjectName
           << ContinueChar(aSettings.PrintSingleLinePerEvent()) << ' ' << aAdditionalInformation << '\n';
}

} // namespace utils

namespace utilsCSV
{

// =================================================================================================
void PrintTime(std::ostream& aStream, double aSimTime)
{
   aStream << aSimTime;
}

// =================================================================================================
void PrintDateTime(std::ostream& aStream, const WsfSimulation& aSimulation)
{
   PrintDateTime(aStream, aSimulation.GetSimTime(), aSimulation);
}

// =================================================================================================
void PrintDateTime(std::ostream& aStream, double aSimTime, const WsfSimulation& aSimulation)
{
   UtCalendar cal(aSimulation.GetDateTime().GetStartDateAndTime());
   cal.AdvanceTimeBy(aSimTime);
   aStream << cal.GetYear();
   aStream << ',' << cal.GetMonth();
   aStream << ',' << cal.GetDay();
   aStream << ',' << cal.GetHour();
   aStream << ',' << cal.GetMinute();
   aStream << ',' << (trunc(cal.GetSecond() * 100) / 100);
}

// =================================================================================================
void PrintTime(std::ostream& aStream, WsfSimulation& aSimulation)
{
   PrintTime(aStream, aSimulation.GetSimTime());
}

// =================================================================================================
void PrintLLA(std::ostream& aStream, double aLat, double aLon, double aAlt)
{
   aStream << aLat << ',' << aLon;
   std::streamsize oldPrecision = aStream.precision(8);
   aStream << ',' << aAlt << ',';
   aStream.precision(oldPrecision);
}

// =================================================================================================
//! Transform an input string that may contain unprintable characters into one that is 'acceptable'.
std::string MakePrintableString(const std::string& aInput)
{
   std::string output;

   // Pre-reserve space to prevent constant reallocation
   size_t inputSize = aInput.size();
   output.reserve(3 * inputSize);

   // Skip trailing whitespace
   while (inputSize > 0)
   {
      char ch = aInput[inputSize - 1];
      if ((ch != ' ') && isprint(ch))
      {
         break;
      }
      --inputSize;
   }

   // Skip leading whitespace
   size_t inputPos = 0;
   while (inputPos < inputSize)
   {
      char ch = aInput[inputPos];
      if ((ch != ' ') && isprint(ch))
      {
         break;
      }
      ++inputPos;
   }

   // The transformation does the following:
   //
   // 1) Make sure that each embedded new-line ('\n') is preceded by a '\'. Post-processors rely
   //    on the fact that multi-line events have a '\' at the end of every line except the last.
   // 2) As a side effect of 1), do no emit a '\' as that would indicate a continuation.
   // 3) All other non-printable characters are copied as spaces.
   while (inputPos < inputSize)
   {
      char ch = aInput[inputPos];
      if (isprint(ch))
      {
         if (ch != '\\')
         {
            output.push_back(ch); // Copy all printable characters except '\'
         }
      }
      else if (ch == '\n')
      {
         output.append("\\\n");
      }
      else
      {
         output.push_back(' '); // All other non-printable characters are copied as spaces
      }
      ++inputPos;
   }
   return output;
}

// =================================================================================================
void PrintEM_Interaction(std::ostream& aStream, WsfEM_Interaction& aResult)
{
   aResult.ComputeUndefinedGeometry();
   if (aResult.GetTarget() == nullptr)
   {
      // One way operation involving a transmitter and a receiver.
      if ((aResult.GetTransmitter() != nullptr) && (aResult.GetReceiver() != nullptr))
      {
         PrintEM_AbsXmtrRcvrData(aStream, aResult, aResult.GetTransmitter(), aResult.mXmtrLoc); // 11
         PrintEM_AbsXmtrRcvrData(aStream, aResult, aResult.GetReceiver(), aResult.mRcvrLoc);
         aStream << ",,,,,,,,,,,"; // PrintEM_AbsTargetData (tgt)
         PrintEM_RelXmtrRcvrData(aStream, aResult, aResult.GetTransmitter(), aResult.mXmtrToRcvr);
         PrintEM_RelXmtrRcvrData(aStream, aResult, aResult.GetReceiver(), aResult.mRcvrToXmtr);
         aStream << ",,,,,"; // PrintEM_RelXmtrRcvrData (xmtr->>Tgt)
         aStream << ",,,,,"; // PrintEM_RelTargetData (tgt->>Xmtr)
         aStream << ",,,,,"; // PrintEM_RelXmtrRcvrData (rcvr->>Tgt)
         aStream << ",,,,,"; // PrintEM_RelTargetData (tgt->>Rcvr)
         PrintEM_BeamData(aStream, aResult.GetTransmitter(), aResult.mXmtrBeam);
         PrintEM_BeamData(aStream, aResult.GetReceiver(), aResult.mRcvrBeam);
      }
      else
      {
         aStream << ",,,,,,,,,,,"; // PrintEM_AbsXmtrRcvrData (xmtr)
         aStream << ",,,,,,,,,,,"; // PrintEM_AbsXmtrRcvrData (rcvr)
         aStream << ",,,,,,,,,,,"; // PrintEM_AbsTargetData (tgt)
         aStream << ",,,,,";       // PrintEM_RelXmtrRcvrData (xmtr->>Rcvr)
         aStream << ",,,,,";       // PrintEM_RelXmtrRcvrData (rcvr->>Xmtr)
         aStream << ",,,,,";       // PrintEM_RelXmtrRcvrData (xmtr->>Tgt)
         aStream << ",,,,,";       // PrintEM_RelTargetData (tgt->>Xmtr)
         aStream << ",,,,,";       // PrintEM_RelXmtrRcvrData (rcvr->>Tgt)
         aStream << ",,,,,";       // PrintEM_RelTargetData (tgt->>Rcvr)
         aStream << ",,,,,,,,";    // PrintEM_BeamData (xmtr)
         aStream << ",,,,,,,,";    // PrintEM_BeamData (rcvr)
      }
   }
   else if (aResult.GetTransmitter() == nullptr)
   {
      // One way operation involving a receiver and a target
      if (aResult.GetReceiver() != nullptr)
      {
         aStream << ",,,,,,,,,,,"; // PrintEM_AbsXmtrRcvrData (xmtr)
         PrintEM_AbsXmtrRcvrData(aStream, aResult, aResult.GetReceiver(), aResult.mRcvrLoc);
         PrintEM_AbsTargetData(aStream, aResult, aResult.GetTarget(), aResult.mTgtLoc);
         aStream << ",,,,,"; // PrintEM_RelXmtrRcvrData (xmtr->>Rcvr)
         aStream << ",,,,,"; // PrintEM_RelXmtrRcvrData (rcvr->>Xmtr)
         aStream << ",,,,,"; // PrintEM_RelXmtrRcvrData (xmtr->>Tgt)
         aStream << ",,,,,"; // PrintEM_RelTargetData (tgt->>Xmtr)
         PrintEM_RelXmtrRcvrData(aStream, aResult, aResult.GetReceiver(), aResult.mRcvrToTgt);
         PrintEM_RelTargetData(aStream, aResult, aResult.GetTarget(), aResult.mTgtToRcvr);
         aStream << ",,,,,,,,"; // PrintEM_BeamData (xmtr)
         PrintEM_BeamData(aStream, aResult.GetReceiver(), aResult.mRcvrBeam);
      }
      else
      {
         aStream << ",,,,,,,,,,,"; // PrintEM_AbsXmtrRcvrData (xmtr)
         aStream << ",,,,,,,,,,,"; // PrintEM_AbsXmtrRcvrData (rcvr)
         aStream << ",,,,,,,,,,,"; // PrintEM_AbsTargetData (tgt)
         aStream << ",,,,,";       // PrintEM_RelXmtrRcvrData (xmtr->>Rcvr)
         aStream << ",,,,,";       // PrintEM_RelXmtrRcvrData (rcvr->>Xmtr)
         aStream << ",,,,,";       // PrintEM_RelXmtrRcvrData (xmtr->>Tgt)
         aStream << ",,,,,";       // PrintEM_RelTargetData (tgt->>Xmtr)
         aStream << ",,,,,";       // PrintEM_RelXmtrRcvrData (rcvr->>Tgt)
         aStream << ",,,,,";       // PrintEM_RelTargetData (tgt->>Rcvr)
         aStream << ",,,,,,,,";    // PrintEM_BeamData (xmtr)
         aStream << ",,,,,,,,";    // PrintEM_BeamData (rcvr)
      }
   }
   else if (aResult.GetReceiver() != nullptr)
   {
      // Two way operation involving a transmitter, target and a receiver.
      if (aResult.mBistatic)
      {
         PrintEM_AbsXmtrRcvrData(aStream, aResult, aResult.GetTransmitter(), aResult.mXmtrLoc);
         PrintEM_AbsXmtrRcvrData(aStream, aResult, aResult.GetReceiver(), aResult.mRcvrLoc);
         PrintEM_AbsTargetData(aStream, aResult, aResult.GetTarget(), aResult.mTgtLoc);
         aStream << ",,,,,"; // PrintEM_RelXmtrRcvrData (xmtr->>Rcvr)
         aStream << ",,,,,"; // PrintEM_RelXmtrRcvrData (rcvr->>Xmtr)
         PrintEM_RelXmtrRcvrData(aStream, aResult, aResult.GetTransmitter(), aResult.mXmtrToTgt);
         PrintEM_RelTargetData(aStream, aResult, aResult.GetTarget(), aResult.mTgtToXmtr);
         PrintEM_RelXmtrRcvrData(aStream, aResult, aResult.GetReceiver(), aResult.mRcvrToTgt);
         PrintEM_RelTargetData(aStream, aResult, aResult.GetTarget(), aResult.mTgtToRcvr);
      }
      else
      {
         aStream << ",,,,,,,,,,,"; // PrintEM_AbsXmtrRcvrData (xmtr)
         PrintEM_AbsXmtrRcvrData(aStream, aResult, aResult.GetReceiver(), aResult.mRcvrLoc);
         PrintEM_AbsTargetData(aStream, aResult, aResult.GetTarget(), aResult.mTgtLoc);
         aStream << ",,,,,"; // PrintEM_RelXmtrRcvrData (xmtr->>Rcvr)
         aStream << ",,,,,"; // PrintEM_RelXmtrRcvrData (rcvr->>Xmtr)
         aStream << ",,,,,"; // PrintEM_RelXmtrRcvrData (xmtr->>Tgt)
         aStream << ",,,,,"; // PrintEM_RelTargetData (tgt->>Xmtr)
         PrintEM_RelXmtrRcvrData(aStream, aResult, aResult.GetReceiver(), aResult.mRcvrToTgt);
         PrintEM_RelTargetData(aStream, aResult, aResult.GetTarget(), aResult.mTgtToRcvr);
      }
      PrintEM_BeamData(aStream, aResult.GetTransmitter(), aResult.mXmtrBeam);
      PrintEM_BeamData(aStream, aResult.GetReceiver(), aResult.mRcvrBeam);
   }
   else
   {
      aStream << ",,,,,,,,,,,"; // PrintEM_AbsXmtrRcvrData (xmtr)
      aStream << ",,,,,,,,,,,"; // PrintEM_AbsXmtrRcvrData (rcvr)
      aStream << ",,,,,,,,,,,"; // PrintEM_AbsTargetData (tgt)
      aStream << ",,,,,";       // PrintEM_RelXmtrRcvrData (xmtr->>Rcvr)
      aStream << ",,,,,";       // PrintEM_RelXmtrRcvrData (rcvr->>Xmtr)
      aStream << ",,,,,";       // PrintEM_RelXmtrRcvrData (xmtr->>Tgt)
      aStream << ",,,,,";       // PrintEM_RelTargetData (tgt->>Xmtr)
      aStream << ",,,,,";       // PrintEM_RelXmtrRcvrData (rcvr->>Tgt)
      aStream << ",,,,,";       // PrintEM_RelTargetData (tgt->>Rcvr)
      aStream << ",,,,,,,,";    // PrintEM_BeamData (xmtr)
      aStream << ",,,,,,,,";    // PrintEM_BeamData (rcvr)
   }

   if (aResult.mRadarSig > 0.0)
   {
      aStream << ',' << UtMath::LinearToDB(aResult.mRadarSig); // Radar_Sig dBsm
      aStream << ',' << aResult.mRadarSig;                     // m^2
      aStream << ',' << aResult.mRadarSigAz;                   // Az deg
      aStream << ',' << aResult.mRadarSigEl;                   // El deg
   }
   else
   {
      aStream << ",,,,";
   }

   if (aResult.mOpticalSig > 0.0)
   {
      aStream << ',' << UtMath::LinearToDB(aResult.mOpticalSig); // Optical_Sig dBsm
      aStream << ',' << aResult.mOpticalSig;                     // m^2
      aStream << ',' << aResult.mOpticalSigAz;                   // Az deg
      aStream << ',' << aResult.mOpticalSigEl;                   // El deg
      aStream << ',';
      if (aResult.mOpticalReflectivity > 0.0)
      {
         aStream << aResult.mOpticalReflectivity; // Reflectivity
      }
   }
   else
   {
      aStream << ",,,,,";
   }

   if (aResult.mInfraredSig > 0.0)
   {
      aStream << ',' << aResult.mInfraredSig;   // Infrared_Sig w/sr
      aStream << ',' << aResult.mInfraredSigAz; // Az deg
      aStream << ',' << aResult.mInfraredSigEl; // El deg
      if (aResult.mSignalToNoise > 0.0)
      {
         aStream << ',' << aResult.mBackgroundRadiantIntensity; // Background_radiant_intensity w/sr
         aStream << ',' << aResult.mContrastRadiantIntensity;   // Contrast_radiant_intensity w/sr
         aStream << ',' << aResult.mAbsorptionFactor;           // Transmittance
      }
      else
      {
         aStream << ",,,";
      }
   }
   else
   {
      aStream << ",,,,,,";
   }

   bool printMaskingFactor = ((aResult.mMaskingFactor >= 0.0) && (aResult.mMaskingFactor < 1.0));
   if ((aResult.mAbsorptionFactor > 0.0) || (aResult.mPropagationFactor > 0.0) || printMaskingFactor)
   {
      if (aResult.mAbsorptionFactor > 0.0)
      {
         aStream << ',' << UtMath::LinearToDB(aResult.mAbsorptionFactor); // Absorption_factor dB
         aStream << ',' << aResult.mAbsorptionFactor;                     // ratio
      }
      else
      {
         aStream << ",,";
      }

      if (aResult.mPropagationFactor > 0.0)
      {
         aStream << ',' << UtMath::LinearToDB(aResult.mPropagationFactor); // Propagation_factor_F^4 dB
         aStream << ',' << aResult.mPropagationFactor;
         aStream << ',' << pow(aResult.mPropagationFactor, 0.25); // ratio ratio^.25
      }
      else
      {
         aStream << ",,,";
      }

      aStream << ',';
      if (printMaskingFactor)
      {
         aStream << aResult.mMaskingFactor;
      }
   }
   else
   {
      aStream << ",,,,,,";
   }

   if ((aResult.mXmtdPower > 0.0) || (aResult.mRcvdPower > 0.0) || (aResult.mPixelCount > 0.0) ||
       (aResult.mSignalToNoise > 0.0))
   {
      aStream << ',';
      if (aResult.mXmtdPower > 0.0)
      {
         aStream << UtMath::LinearToDB(aResult.mXmtdPower); // Xmtd_Power(dBw)
      }

      aStream << ',';
      if (aResult.mRcvdPower > 0.0)
      {
         aStream << UtMath::LinearToDB(aResult.mRcvdPower); // Rcvd_Power(dbw)
      }

      aStream << ',';
      if (aResult.mRcvrNoisePower > 0.0)
      {
         aStream << UtMath::LinearToDB(aResult.mRcvrNoisePower); // Rcvr_Noise(dBw)
      }

      aStream << ',';
      if (aResult.mClutterPower > 0.0)
      {
         aStream << UtMath::LinearToDB(aResult.mClutterPower); // Clutter_Power(dBw)
      }

      aStream << ',';
      if (aResult.mInterferencePower > 0.0)
      {
         aStream << UtMath::LinearToDB(aResult.mInterferencePower);
      }

      bool                                    printedPowers = false;
      const WsfEM_Interaction::ComponentList& components(aResult.GetComponents());
      for (WsfEM_Interaction::ComponentList::Iterator i = components.Begin(); i != components.End(); ++i)
      {
         if (i->PrintCSV_EventOutput_Powers(aStream))
         {
            printedPowers = true;
         }
      }
      if (!printedPowers)
      {
         aStream << ",,,";
      }

      aStream << ',';
      if (aResult.mPixelCount > 0.0)
      {
         aStream << aResult.mPixelCount; // Pixel_Count
      }

      if (aResult.mSignalToNoise > 0.0)
      {
         aStream << ',' << UtMath::LinearToDB(aResult.mSignalToNoise); // S/I dB

         aStream << ',';
         if (aResult.mDetectionThreshold > 0.0)
         {
            aStream << UtMath::LinearToDB(aResult.mDetectionThreshold); // Threshold
         }

         if (aResult.mRcvrNoisePower > 0.0)
         {
            double noisePower   = aResult.mRcvrNoisePower;
            double clutterPower = std::max(aResult.mClutterPower, 0.0);
            double intfPower    = std::max(aResult.mInterferencePower,
                                        0.0); // TODO may need to have a EM_Interaction component calculate this
            // Signal power used to be aResult.mRcvdPower, but that didn't always contain all the parts that were
            // considered 'signal' (depends on the sensor implementation). The following recovers the signal.
            double signalPower = aResult.mSignalToNoise * (noisePower + clutterPower + intfPower);
            aStream << ',' << UtMath::LinearToDB(signalPower / noisePower);                              // S/N dB
            aStream << ',' << UtMath::LinearToDB(signalPower / (noisePower + clutterPower));             // S/(N+C) dB
            aStream << ',' << UtMath::LinearToDB(signalPower / (noisePower + clutterPower + intfPower)); // S/(N+C+I) dB
         }
         else
         {
            aStream << ",,,";
         }
      }
      else
      {
         aStream << ",,,,,";
      }
   }
   else
   {
      aStream << ",,,,,,,,,,,,,,";
   }

   // Process component output.
   bool                                    printedOutput = false;
   const WsfEM_Interaction::ComponentList& components(aResult.GetComponents());
   for (WsfEM_Interaction::ComponentList::Iterator i = components.Begin(); i != components.End(); ++i)
   {
      if (i->PrintCSV_EventOutput(aStream))
      {
         printedOutput = true;
      }
   }
   if (!printedOutput)
   {
      aStream << ',';
   }

   std::string failures;
   if ((aResult.mFailedStatus & WsfEM_Interaction::cXMTR_RANGE_LIMITS) != 0)
   {
      failures += "Xmtr_Range_Limits_Exceeded ";
   }
   if ((aResult.mFailedStatus & WsfEM_Interaction::cRCVR_RANGE_LIMITS) != 0)
   {
      failures += "Rcvr_Range_Limits_Exceeded ";
   }
   if ((aResult.mFailedStatus & WsfEM_Interaction::cXMTR_ALTITUDE_LIMITS) != 0)
   {
      failures += "Xmtr_Altitude_Limits_Exceeded ";
   }
   if ((aResult.mFailedStatus & WsfEM_Interaction::cRCVR_ALTITUDE_LIMITS) != 0)
   {
      failures += "Rcvr_Altitude_Limits_Exceeded ";
   }
   if ((aResult.mFailedStatus & WsfEM_Interaction::cXMTR_ANGLE_LIMITS) != 0)
   {
      failures += "Xmtr_Angle_Limits_Exceeded ";
   }
   if ((aResult.mFailedStatus & WsfEM_Interaction::cRCVR_ANGLE_LIMITS) != 0)
   {
      failures += "Rcvr_Angle_Limits_Exceeded ";
   }
   if ((aResult.mFailedStatus & WsfEM_Interaction::cXMTR_HORIZON_MASKING) != 0)
   {
      failures += "Xmtr_Masked_By_Horizon ";
   }
   if ((aResult.mFailedStatus & WsfEM_Interaction::cRCVR_HORIZON_MASKING) != 0)
   {
      failures += "Rcvr_Masked_By_Horizon ";
   }
   if ((aResult.mFailedStatus & WsfEM_Interaction::cXMTR_TERRAIN_MASKING) != 0)
   {
      failures += "Xmtr_Masked_By_Terrain ";
   }
   if ((aResult.mFailedStatus & WsfEM_Interaction::cRCVR_TERRAIN_MASKING) != 0)
   {
      failures += "Rcvr_Masked_By_Terrain ";
   }
   if ((aResult.mFailedStatus & WsfEM_Interaction::cSIGNAL_LEVEL) != 0)
   {
      failures += "Insufficient_Signal ";
   }
   aStream << ',' << failures;
}

// =================================================================================================
void PrintEM_AbsTargetData(std::ostream&                          aStream,
                           const WsfEM_Interaction&               aResult,
                           WsfPlatform*                           aTgtPtr,
                           const WsfEM_Interaction::LocationData& aLocData)
{
   aStream << ',' << aTgtPtr->GetType();
   PrintEM_LocationData(aStream, aTgtPtr, aLocData);
}

// =================================================================================================
void PrintEM_RelTargetData(std::ostream&                          aStream,
                           const WsfEM_Interaction&               aResult,
                           WsfPlatform*                           aTgtPtr,
                           const WsfEM_Interaction::RelativeData& aRelData)
{
   if (aRelData.mRange > 0.0)
   {
      PrintEM_Range(aStream, aRelData.mRange, false);
      double otherLocNED[3];
      aTgtPtr->ConvertWCSVectorToNED(otherLocNED, aRelData.mTrueUnitVecWCS);
      double magnitudeNE = sqrt(otherLocNED[0] * otherLocNED[0] + otherLocNED[1] * otherLocNED[1]);
      double otherAzNED  = atan2(otherLocNED[1], otherLocNED[0]);
      double otherElNED  = atan2(-otherLocNED[2], magnitudeNE);
      aStream << ',' << UtMath::NormalizeAngle0_TwoPi(otherAzNED);
      aStream << ',' << otherElNED;

      if (aResult.mEarthRadiusScale != 1.0)
      {
         aTgtPtr->ConvertWCSVectorToNED(otherLocNED, aRelData.mUnitVecWCS);
         magnitudeNE = sqrt(otherLocNED[0] * otherLocNED[0] + otherLocNED[1] * otherLocNED[1]);
         otherAzNED  = atan2(otherLocNED[1], otherLocNED[0]);
         otherElNED  = atan2(-otherLocNED[2], magnitudeNE);
         aStream << ',' << UtMath::NormalizeAngle0_TwoPi(otherAzNED);
         aStream << ',' << otherElNED;
      }
      else
      {
         aStream << ",,";
      }
   }
   else
   {
      aStream << ",,,,,";
   }
}

// =================================================================================================
void PrintEM_AbsXmtrRcvrData(std::ostream&                          aStream,
                             const WsfEM_Interaction&               aResult,
                             WsfEM_XmtrRcvr*                        aXmtrRcvrPtr,
                             const WsfEM_Interaction::LocationData& aLocData)
{
   aStream << ',' << aXmtrRcvrPtr->GetArticulatedPart()->GetType();
   PrintEM_LocationData(aStream, aXmtrRcvrPtr->GetPlatform(), aLocData);
}

// =================================================================================================
void PrintEM_RelXmtrRcvrData(std::ostream&                          aStream,
                             const WsfEM_Interaction&               aResult,
                             WsfEM_XmtrRcvr*                        aXmtrRcvrPtr,
                             const WsfEM_Interaction::RelativeData& aRelData)
{
   if (aRelData.mRange > 0.0)
   {
      PrintEM_Range(aStream, aRelData.mRange, false);
      double otherLocNED[3];
      aXmtrRcvrPtr->GetAntenna()->ConvertWCS_VectorToNED(aRelData.mTrueUnitVecWCS, otherLocNED);
      double magnitudeNE = sqrt(otherLocNED[0] * otherLocNED[0] + otherLocNED[1] * otherLocNED[1]);
      double otherAzNED  = atan2(otherLocNED[1], otherLocNED[0]);
      double otherElNED  = atan2(-otherLocNED[2], magnitudeNE);
      aStream << ',' << UtMath::NormalizeAngle0_TwoPi(otherAzNED);
      aStream << ',' << otherElNED;

      if (aResult.mEarthRadiusScale != 1.0)
      {
         aXmtrRcvrPtr->GetAntenna()->ConvertWCS_VectorToNED(aRelData.mUnitVecWCS, otherLocNED);
         magnitudeNE = sqrt(otherLocNED[0] * otherLocNED[0] + otherLocNED[1] * otherLocNED[1]);
         otherAzNED  = atan2(otherLocNED[1], otherLocNED[0]);
         otherElNED  = atan2(-otherLocNED[2], magnitudeNE);
         aStream << ',' << UtMath::NormalizeAngle0_TwoPi(otherAzNED);
         aStream << ',' << otherElNED;
      }
      else
      {
         aStream << ",,";
      }
   }
   else
   {
      aStream << ",,,,,";
   }
}

// =================================================================================================
void PrintEM_BeamData(std::ostream& aStream, WsfEM_XmtrRcvr* aXmtrRcvrPtr, const WsfEM_Interaction::BeamData& aBeamData)
{
   if (aBeamData.mGain >= 0.0)
   {
      double beamPointNED[3];
      aXmtrRcvrPtr->GetAntenna()->ConvertWCS_VectorToNED(aBeamData.mWCS_ToBeamTransform[0], beamPointNED);
      double magnitudeNE = sqrt(beamPointNED[0] * beamPointNED[0] + beamPointNED[1] * beamPointNED[1]);
      double beamPointAz = atan2(beamPointNED[1], beamPointNED[0]);
      double beamPointEl = atan2(-beamPointNED[2], magnitudeNE);
      aStream << ',' << UtMath::NormalizeAngle0_TwoPi(beamPointAz);
      aStream << ',' << beamPointEl;
      aStream << ',' << aBeamData.mAz;
      aStream << ',' << aBeamData.mEl;
      if (aBeamData.mGain > 0.0)
      {
         if (aXmtrRcvrPtr->GetAntenna()->GetEBS_Mode() != WsfEM_Antenna::cEBS_NONE)
         {
            double cosTheta = cos(aBeamData.mEBS_Az) * cos(aBeamData.mEBS_El);
            cosTheta        = std::max(std::min(cosTheta, 1.0), -1.0); // For safety
            aStream << ',' << aBeamData.mEBS_Az;
            aStream << ',' << aBeamData.mEBS_El;
            aStream << ',' << acos(cosTheta);
         }
         else
         {
            aStream << ",,,";
         }
         aStream << ',' << UtMath::LinearToDB(aBeamData.mGain);
      }
      else
      {
         aStream << ",,,,";
      }
   }
   else
   {
      aStream << ",,,,,,,,";
   }
}

// =================================================================================================
void PrintEM_LocationData(std::ostream& aStream, WsfPlatform* aPlatformPtr, const WsfEM_Interaction::LocationData& aLocationData)
{
   double lat;
   double lon;
   double alt;
   double locationECI[3];

   if (aLocationData.mIsValid)
   {
      UtEllipsoidalEarth::ConvertECEFToLLA(aLocationData.mLocWCS, lat, lon, alt);
      aPlatformPtr->ConvertWCSToECI(aLocationData.mLocWCS, locationECI); // using the last update time of the platform.
   }
   else
   {
      aPlatformPtr->GetLocationLLA(lat, lon, alt);
      aPlatformPtr->GetLocationECI(locationECI);
   }

   PrintLocationDataLLA(aStream, lat, lon, alt);
   PrintLocationDataECI(aStream, locationECI);

   double yaw;
   double pitch;
   double roll;
   aPlatformPtr->GetOrientationNED(yaw, pitch, roll);
   aStream << ',' << yaw;
   aStream << ',' << pitch;
   aStream << ',' << roll;
   aStream << ',' << aPlatformPtr->GetSpeed();
}

// =================================================================================================
void PrintEM_Range(std::ostream& aStream, double aRange, bool aWriteNewLine /* = true */)
{
   aStream << ',' << aRange;
}

// =================================================================================================
void PrintLocationDataLLA(std::ostream& aStream, double aLat, double aLon, double aAlt)
{
   aStream << ',' << aLat;
   aStream << ',' << aLon;
   aStream << ',';
   std::streamsize oldPrecision = aStream.precision(8);
   aStream << aAlt;
   aStream.precision(oldPrecision);
}

// =================================================================================================
// Like the other PrintLocationData, but location is gotten from the platform.
void PrintLocationData(std::ostream& aStream, UtEntity* aPlatformPtr)
{
   // LLA
   double lat;
   double lon;
   double alt;
   aPlatformPtr->GetLocationLLA(lat, lon, alt);
   PrintLocationDataLLA(aStream, lat, lon, alt);

   // ECI
   double locationECI[3];
   aPlatformPtr->GetLocationECI(locationECI);
   PrintLocationDataECI(aStream, locationECI);
}

// =================================================================================================
//! Like PrintLocationDataLLA, but outputs ECI values instead.
void PrintLocationDataECI(std::ostream& aStream, const double aLocationECI[3])
{
   std::streamsize    oldPrecision = aStream.precision(8);
   std::ios::fmtflags oldFlags     = aStream.setf(std::ios::scientific, std::ios::floatfield);
   aStream << ',' << aLocationECI[0];
   aStream << ',' << aLocationECI[1];
   aStream << ',' << aLocationECI[2];
   aStream.precision(oldPrecision);
   aStream.setf(oldFlags);
}

// =================================================================================================
//! Print the 'non-location' parts of the kinematic data (orientation, velocity and acceleration).
//!
//! It is assumed this is called immediately after PrintLocationData. The orientation data will appear
//! on the same line as the location data and the velocity and acceleration will appear on the next line.
// protected
void PrintNonLocationData(std::ostream& aStream, WsfPlatform* aPlatformPtr, const std::string& aIndent)
{
   std::ios::fmtflags oldFlags     = aStream.flags();
   std::streamsize    oldPrecision = aStream.precision(3);
   aStream.setf(std::ios::fixed, std::ios::floatfield);

   double heading;
   double pitch;
   double roll;
   aPlatformPtr->GetOrientationNED(heading, pitch, roll);
   aStream << ',' << UtMath::NormalizeAngle0_TwoPi(heading);
   aStream << ',' << pitch;
   aStream << ',' << roll;

   double nedVec[3];
   double eciVec[3];
   double nedVecMag;
   double eciVecMag;

   aPlatformPtr->GetVelocityNED(nedVec);
   aPlatformPtr->GetVelocityECI(eciVec);

   nedVecMag = UtVec3d::Normalize(nedVec);
   aStream << ',' << nedVecMag;
   aStream << ',' << nedVec[0];
   aStream << ',' << nedVec[1];
   aStream << ',' << nedVec[2];
   eciVecMag = UtVec3d::Normalize(eciVec);
   aStream << ',' << eciVecMag;
   aStream << ',' << eciVec[0];
   aStream << ',' << eciVec[1];
   aStream << ',' << eciVec[2];
   aPlatformPtr->GetAccelerationNED(nedVec);
   aPlatformPtr->GetAccelerationECI(eciVec);
   nedVecMag = UtVec3d::Normalize(nedVec);
   aStream << ',' << nedVecMag;
   aStream << ',' << nedVec[0];
   aStream << ',' << nedVec[1];
   aStream << ',' << nedVec[2];
   eciVecMag = UtVec3d::Normalize(eciVec);
   aStream << ',' << eciVecMag;
   aStream << ',' << eciVec[0];
   aStream << ',' << eciVec[1];
   aStream << ',' << eciVec[2];

   aStream.flags(oldFlags);
   aStream.precision(oldPrecision);
}

// =================================================================================================
void PrintMessageData(std::ostream&             aStream,
                      double                    aSimTime,
                      const WsfMessage&         aMessage,
                      WsfSimulation&            aSimulation,
                      Settings::MessagePrintMap aMessagePrinters,
                      bool                      aPrintTrackDataBrief)
{
   WsfStringId messageType = aMessage.GetType();
   if (const auto* trackMsg = dynamic_cast<const WsfTrackMessage*>(&aMessage))
   {
      aStream << ",,,,,";
      if (trackMsg->GetTrack())
      {
         aStream << trackMsg->GetTrack()->GetTrackId();
         PrintTrackData(aStream, aSimTime, trackMsg->GetTrack(), aSimulation);
      }
      else
      {
         aStream << std::setfill(',') << std::setw(60) << "";
      }
   }
   else if (const auto* trackDropMsg = dynamic_cast<const WsfTrackDropMessage*>(&aMessage))
   {
      aStream << ",,,,," << trackDropMsg->GetTrackId() << std::setfill(',') << std::setw(60) << "";
   }
   else if (const auto* statusMsg = dynamic_cast<const WsfStatusMessage*>(&aMessage))
   {
      aStream << ',' << statusMsg->GetStatus();

      aStream << ',';
      if (!statusMsg->GetRequestId().IsNull())
      {
         aStream << statusMsg->GetRequestId();
      }

      aStream << ',';
      if (statusMsg->GetSystemNameId() != 0)
      {
         aStream << statusMsg->GetSystemName();
      }

      aStream << ',';
      if (statusMsg->GetPlatform() != nullptr)
      {
         aStream << statusMsg->GetPlatform()->GetName();
      }
      aStream << std::setfill(',') << std::setw(61) << "";
   }
   else if (const auto* taskAssignMsg = dynamic_cast<const WsfTaskAssignMessage*>(&aMessage))
   {
      aStream << ",,,,," << taskAssignMsg->GetTrack().GetTrackId();
      aStream << std::setfill(',') << std::setw(60) << "";
      // if (PrintTrackInMessage())
      //{
      // TODO  PrintTrackData(aSimTime, &(taskAssignMsg->GetTrack()));
      //}
   }
   else
   {
      Settings::MessagePrintMap::const_iterator i = aMessagePrinters.find(messageType);
      if (i != aMessagePrinters.end())
      {
         (i->second)(aSimTime, aMessage);
      }
      else
      {
         aStream << std::setfill(',') << std::setw(65) << "";
      }
   }
}

// =================================================================================================
//! Perform common print processing for the "PLATFORM_" messages.
void PrintPlatformEvent(std::ostream& aStream, WsfPlatform* aPlatformPtr, bool aShowPs)
{
   aStream << aPlatformPtr->GetName();
   aStream << ',' << aPlatformPtr->GetSide();
   aStream << ',' << aPlatformPtr->GetType();
   aStream << ',';
   if (aShowPs)
   {
      aStream << 1.0 - aPlatformPtr->GetDamageFactor();
   }
   if (aPlatformPtr->IsInitialized())
   {
      PrintLocationData(aStream, aPlatformPtr);
      PrintNonLocationData(aStream, aPlatformPtr, "");
   }
}

// =================================================================================================
void PrintSensorResult(std::ostream& aStream, WsfSensor* aSensorPtr, WsfSensorResult& aResult)
{
   aStream << ',' << aSensorPtr->GetName() << ',';
   if (aSensorPtr->GetModeCount() > 0)
   {
      aStream << aSensorPtr->GetModeName(aResult.mModeIndex);
   }
   else
   {
      aStream << "none";
   }
   aStream << ',' << aResult.mBeamIndex + 1;

   PrintEM_Interaction(aStream, aResult);

   std::string failures;
   if ((aResult.mFailedStatus & WsfSensorResult::cCONCEALMENT) != 0)
   {
      failures += "Target_Concealed ";
   }
   if ((aResult.mFailedStatus & WsfSensorResult::cDOPPLER_LIMITS) != 0)
   {
      failures += "Doppler_Limits_Exceeded ";
   }
   if ((aResult.mFailedStatus & WsfSensorResult::cVELOCITY_LIMITS) != 0)
   {
      failures += "Velocity_Limits_Exceeded ";
   }
   if ((aResult.mFailedStatus & WsfSensorResult::cTARGET_DELETED) != 0)
   {
      failures += "Target_Deleted ";
   }
   if ((aResult.mFailedStatus & WsfSensorResult::cDETECTION_STOP) != 0)
   {
      failures += "Detection_Stopped ";
   }
   if ((aResult.mFailedStatus & WsfSensorResult::cSCRIPT_CONSTRAINT) != 0)
   {
      failures += "Blocked_By_Script ";
   }
   aStream << ',' << failures;

   // A successful detection (mFailedStatus == 0) is valid only if something was
   // actually checked (mCheckedStatus != 0).
   // bool detected = ((aResult.mCheckedStatus != 0) &&
   //                 (aResult.mFailedStatus  == 0) &&
   //                 (aResult.mPd >= aResult.mRequiredPd));
   aStream << ',' << aResult.mPd;
   aStream << ',' << aResult.mRequiredPd;
   aStream << ',' << aResult.Detected();
}

// =================================================================================================
//! Print common information associated with turning a system off or on.
void PrintSystemOffOnEvent(std::ostream&      aStream,
                           double             aSimTime,
                           const std::string& aEvent,
                           const std::string& aSystemType,
                           WsfPlatformPart*   aSystemPtr)
{
   PrintTime(aStream, aSimTime);
   aStream << ',' << aEvent;
   aStream << ',' << aSystemPtr->GetPlatform()->GetName();
   aStream << ',' << aSystemPtr->GetPlatform()->GetSide();
   aStream << ',' << aSystemType;
   aStream << ',' << aSystemPtr->GetName();
   aStream << ',' << aSystemPtr->GetType();
   PrintLocationData(aStream, aSystemPtr->GetPlatform());
   PrintNonLocationData(aStream, aSystemPtr->GetPlatform(), "");
   aStream << '\n';
}

// =================================================================================================
void PrintTrackCovarianceData(std::ostream&       aStream,
                              double              aSimTime,
                              const std::string&  aCovarianceType,
                              const UtCovariance* aCovariancePtr,
                              const WsfTrack*     aTrackPtr)
{
   aStream << ',' << aCovarianceType;
   if (aCovariancePtr != nullptr)
   {
      WsfCovariance covar(*aCovariancePtr);
      double        originatorLocWCS[3];
      aTrackPtr->GetOriginatorLocationWCS(originatorLocWCS);
      double lat, lon, alt;
      UtEllipsoidalEarth::ConvertECEFToLLA(originatorLocWCS, lat, lon, alt);
      double locWCS[3];
      double WCS_ToNED_Transform[3][3];
      UtEllipsoidalEarth::ComputeNEDTransform(lat, lon, alt, WCS_ToNED_Transform, locWCS);
      WsfCovariance::ConvertFromFrameB_ToFrameA(covar, WCS_ToNED_Transform);

      // Now that we have an NED transform, copy the NE part to a 2x2 vector:
      std::vector<double> neVec;
      neVec.push_back(covar(0, 0));
      neVec.push_back(covar(0, 1));
      neVec.push_back(covar(1, 0));
      neVec.push_back(covar(1, 1));
      double semiMajorAxis, semiMinorAxis, bearing;
      WsfUtil::CovarianceToEllipse_2x2(neVec, semiMajorAxis, semiMinorAxis, bearing);
      aStream << ',' << semiMajorAxis * 2.0;
      aStream << ',' << semiMinorAxis * 2.0;
      aStream << ',' << bearing * UtMath::cDEG_PER_RAD;
      // TODO - need a CSV for below call...
      covar.Stream(aStream, true); // Stream the NED covariance, not the WCS covariance, as this should be more intuitive.
   }
   else
   {
      aStream << ",,,"; // TODO
   }
}

// =================================================================================================
void PrintAuxData(std::ostream& aStream, const UtAttributeContainer* aAuxDataPtr)
{
   aStream << ',';
   UtAttributeBase::ConstIterator iter(*aAuxDataPtr);
   while (iter->HasNext())
   {
      const UtAttributeBase& attr = *iter;
      if (attr.IsContainerType())
      {
         // Print recursively.
         PrintAuxData(aStream, static_cast<const UtAttributeContainer*>(&(*iter)));
      }
      const std::string& name = attr.GetName();
      UtVariant          var;
      aStream << "(" << name << ":";
      if (attr.GetVariant(var))
      {
         aStream << var.ToString();
      }
      aStream << ")";
      iter->Next();
   }
}

// =================================================================================================
void PrintTrackData(std::ostream&        aStream,
                    double               aSimTime,
                    const WsfTrack*      aTrackPtr,
                    const WsfSimulation& aSimulation,
                    bool                 aPrintTrackDataBrief /*= false*/)
{
   double locationWCS[3];
   double trackLocNED[3] = {0.0, 0.0, 0.0};
   double truthLocNED[3] = {0.0, 0.0, 0.0};

   aStream << ',' << aTrackPtr->GetStartTime();
   aStream << ',' << aTrackPtr->GetUpdateTime();
   aStream << ',' << aTrackPtr->GetUpdateCount();
   aStream << ',' << aTrackPtr->GetTrackQuality();
   aStream << ',' << WsfTypes::EnumToString(aTrackPtr->GetSpatialDomain());

   aStream << ',';
   switch (aTrackPtr->GetTrackType())
   {
   case WsfTrack::cFILTERED_SENSOR:
      aStream << 'F';
      break;
   case WsfTrack::cUNFILTERED_SENSOR:
      aStream << 'M';
      break;
   case WsfTrack::cPREDEFINED:
      aStream << 'I';
      break;
   case WsfTrack::cPROCESSED:
      aStream << 'P';
      break;
   case WsfTrack::cSTATIC_IMAGE:
      aStream << 'S';
      break;
   case WsfTrack::cPSEUDO_SENSOR:
      aStream << 'V';
      break;
   default:
      aStream << 'U';
      break;
   }

   aStream << ',';
   if (aTrackPtr->IsCandidate())
   {
      aStream << 'C';
   }

   aStream << ',';
   if (aTrackPtr->IsFalseTarget())
   {
      aStream << 'F';
   }

   // Don't print if requesting only brief summary
   if (aPrintTrackDataBrief)
   {
      return;
   }

   aTrackPtr->GetOriginatorLocationWCS(locationWCS);
   UtEntity originator;
   originator.SetLocationWCS(locationWCS);
   originator.SetECI_ReferenceEpoch(aSimulation.GetDateTime().GetStartDateAndTime());
   originator.SetTime(aTrackPtr->GetUpdateTime());

   WsfPlatform* platformPtr = aSimulation.GetPlatformByIndex(aTrackPtr->GetTargetIndex());
   if (platformPtr != nullptr)
   {
      // Make sure the target's platform location is up-to-date.
      // Making the call here ensures that there is no extra processing if
      // we do not use the event output.
      platformPtr->Update(aSimTime);
      originator.GetRelativeLocationNED(platformPtr, truthLocNED);
      aStream << ',' << platformPtr->GetName();
      aStream << ',' << platformPtr->GetType();
      aStream << ',' << platformPtr->GetSide();
   }
   else
   {
      aStream << ",,,";
   }

   PrintLocationData(aStream, &originator);

   bool showLoc2D = false;
   bool showLoc3D = false;
   if (aTrackPtr->LocationValid())
   {
      showLoc3D = true;

      double trackLocWCS[3];
      aTrackPtr->GetLocationWCS(trackLocWCS);
      originator.ConvertWCSToNED(trackLocWCS, trackLocNED);
   }
   else if (aTrackPtr->RangeValid() && aTrackPtr->BearingValid())
   {
      if (aTrackPtr->ElevationValid())
      {
         showLoc3D      = true;
         double ne      = aTrackPtr->GetRange() * cos(aTrackPtr->GetElevation());
         double d       = -aTrackPtr->GetRange() * sin(aTrackPtr->GetElevation());
         trackLocNED[0] = ne * cos(aTrackPtr->GetBearing());
         trackLocNED[1] = ne * sin(aTrackPtr->GetBearing());
         trackLocNED[2] = d;
      }
      else
      {
         showLoc2D      = true;
         double ne      = aTrackPtr->GetRange();
         trackLocNED[0] = ne * cos(aTrackPtr->GetBearing());
         trackLocNED[1] = ne * sin(aTrackPtr->GetBearing());
         trackLocNED[2] = 0.0;
      }
   }

   if (showLoc2D || showLoc3D)
   {
      double lat;
      double lon;
      double alt;
      double locationECI[3];

      if (showLoc3D)
      {
         originator.ConvertNEDToLLA(trackLocNED, lat, lon, alt);
         PrintLocationDataLLA(aStream, lat, lon, alt);

         originator.ConvertNEDToECI(trackLocNED, locationECI);
         PrintLocationDataECI(aStream, locationECI);
      }
      else
      {
         originator.ConvertNEDToLLA(trackLocNED, lat, lon, alt);
         alt = 0.0;
         PrintLocationDataLLA(aStream, lat, lon, alt);
         aStream << ",,,"; // eci blanks
      }

      aStream << ',';
      if (aTrackPtr->LocationValid())
      {
         aStream << 'L';
      }

      aStream << ',';
      if (aTrackPtr->Is3D())
      {
         aStream << '3';
      }

      aStream << ',';
      if (aTrackPtr->RangeValid())
      {
         aStream << 'R';
      }

      aStream << ',';
      if (aTrackPtr->BearingValid())
      {
         aStream << 'B';
      }

      aStream << ',';
      if (aTrackPtr->ElevationValid())
      {
         aStream << 'E';
      }

      if (platformPtr != nullptr)
      {
         platformPtr->GetLocationLLA(lat, lon, alt);
         PrintLocationDataLLA(aStream, lat, lon, alt);

         platformPtr->GetLocationECI(locationECI);
         PrintLocationDataECI(aStream, locationECI);

         double deltaLocNED[3];
         UtVec3d::Subtract(deltaLocNED, trackLocNED, truthLocNED);
         if (showLoc2D)
         {
            deltaLocNED[2] = 0.0;
         }
         aStream << ',' << UtVec3d::Magnitude(deltaLocNED);
      }
      else
      {
         aStream << ",,,,,,,";
      }
   }
   else
   {
      aStream << ",,,,,,,,,,,,,,,,,,";
   }

   aStream << ',';
   if (aTrackPtr->VelocityValid())
   {
      double trackVelWCS[3];
      aTrackPtr->GetVelocityWCS(trackVelWCS);
      aStream << UtVec3d::Magnitude(trackVelWCS);
      double trackLocWCS[3];
      aStream << ',';
      if (aTrackPtr->GetExtrapolatedLocationWCS(aSimTime, trackLocWCS))
      {
         UtEntity entity;
         entity.SetLocationWCS(trackLocWCS);
         entity.SetVelocityWCS(trackVelWCS);
         double trackVelNED[3];
         entity.GetVelocityNED(trackVelNED);
         double heading = atan2(trackVelNED[1], trackVelNED[0]);
         aStream << UtMath::NormalizeAngle0_TwoPi(heading);
      }

      aStream << ',';
      if (platformPtr != nullptr)
      {
         double truthVelNED[3];
         double heading;
         double pitch;
         double roll;
         platformPtr->GetVelocityNED(truthVelNED);
         platformPtr->GetOrientationNED(heading, pitch, roll);
         aStream << UtVec3d::Magnitude(truthVelNED) << ',' << UtMath::NormalizeAngle0_TwoPi(heading);
      }
      else
      {
         aStream << ',';
      }
   }
   else
   {
      aStream << ",,,";
   }

   // Display range, bearing, elevation data.

   if (aTrackPtr->LocationValid() || aTrackPtr->RangeValid() || aTrackPtr->BearingValid() || aTrackPtr->ElevationValid())
   {
      // First the 'track' data.
      aStream << ',';
      if (aTrackPtr->RangeValid())
      {
         aStream << aTrackPtr->GetRange();
      }
      else if (aTrackPtr->LocationValid())
      {
         aStream << UtVec3d::Magnitude(trackLocNED);
      }

      aStream << ',';
      if (aTrackPtr->BearingValid())
      {
         double bearing = UtMath::NormalizeAngle0_TwoPi(aTrackPtr->GetBearing());
         aStream << bearing;
      }
      else if (aTrackPtr->LocationValid())
      {
         double bearing = atan2(trackLocNED[1], trackLocNED[0]);
         bearing        = UtMath::NormalizeAngle0_TwoPi(bearing);
         aStream << bearing;
      }

      aStream << ',';
      if (aTrackPtr->ElevationValid())
      {
         aStream << aTrackPtr->GetElevation();
      }
      else if (aTrackPtr->LocationValid())
      {
         double range     = UtVec3d::Magnitude(trackLocNED);
         double elevation = 0.0;
         if (range > 0.0)
         {
            elevation = asin(-trackLocNED[2] / range);
         }
         aStream << elevation;
      }

      // Then the 'truth' data.

      if (platformPtr != nullptr)
      {
         aStream << ',';
         if (aTrackPtr->RangeValid() || aTrackPtr->LocationValid())
         {
            aStream << UtVec3d::Magnitude(truthLocNED);
         }

         aStream << ',';
         if (aTrackPtr->BearingValid() || aTrackPtr->LocationValid())
         {
            double bearing = atan2(truthLocNED[1], truthLocNED[0]);
            bearing        = UtMath::NormalizeAngle0_TwoPi(bearing);
            aStream << bearing;
         }

         aStream << ',';
         if (aTrackPtr->ElevationValid() || aTrackPtr->LocationValid())
         {
            double range     = UtVec3d::Magnitude(truthLocNED);
            double elevation = 0.0;
            if (range > 0.0)
            {
               elevation = asin(-truthLocNED[2] / range);
            }
            aStream << elevation;
         }
      }
      else
      {
         aStream << ",,,";
      }

      // Print measurement errors for unfiltered sensor reports
      if ((aTrackPtr->GetTrackType() == WsfTrack::cUNFILTERED_SENSOR) &&
          ((aTrackPtr->GetRangeError() != 0.0) || (aTrackPtr->GetBearingError() != 0.0) ||
           (aTrackPtr->GetElevationError() != 0.0)))
      {
         aStream << ',';
         if (aTrackPtr->GetRangeError() != 0.0)
         {
            aStream << aTrackPtr->GetRangeError();
         }

         aStream << ',';
         if (aTrackPtr->GetBearingError() != 0.0)
         {
            aStream << aTrackPtr->GetBearingError();
         }

         aStream << ',';
         if (aTrackPtr->GetElevationError() != 0.0)
         {
            aStream << aTrackPtr->GetElevationError();
         }
      }
      else
      {
         aStream << ",,,";
      }
   }
   else
   {
      aStream << ",,,,,,,,,";
   }

   aStream << ',';
   if (aTrackPtr->TypeIdValid())
   {
      aStream << aTrackPtr->GetTypeId();
   }

   aStream << ',';
   if (aTrackPtr->SideIdValid())
   {
      aStream << aTrackPtr->GetSideId();
   }

   aStream << ',';
   if (aTrackPtr->SignalToNoiseValid())
   {
      aStream << UtMath::SafeLinearToDB(aTrackPtr->GetSignalToNoise());
   }

   aStream << ',';
   if (aTrackPtr->GetPixelCount() > 0.0)
   {
      aStream << aTrackPtr->GetPixelCount();
   }

   aStream << ',';
   if (aTrackPtr->FrequencyValid())
   {
      unsigned int count = aTrackPtr->GetSignalCount();
      aStream << count << ',';
      if (count != 0)
      {
         WsfTrack::Signal signal;
         for (unsigned int index = 0; index < count; ++index)
         {
            aTrackPtr->GetSignalEntry(index, signal);
            aStream << signal.mLowerFrequency;
            aStream << ',' << signal.mUpperFrequency;
         }
      }
      else
      {
         aStream << ',';
      }
   }
   else
   {
      aStream << "0,,";
   }

   if (aTrackPtr->HasAuxData())
   {
      const WsfAttributeContainer* auxDataPtr = &aTrackPtr->GetAuxDataConst();
      PrintAuxData(aStream, auxDataPtr);
   }
   else
   {
      aStream << ',';
   }
}

// =================================================================================================
//! Add the target name to the current line.
//! If the target name is empty then just print the track ID in '(..)'.
void PrintTrackTargetName(std::ostream& aStream, const WsfTrack* aTrackPtr, WsfSimulation& aSimulation)
{
   aStream << ',';
   if (aTrackPtr == nullptr)
   {
      aStream << "UNKNOWN";
   }
   else if (aTrackPtr->GetTargetName() != 0)
   {
      aStream << aTrackPtr->GetTargetName();
   }
   else
   {
      WsfStringId nameId = aSimulation.GetPlatformNameId(aTrackPtr->GetTargetIndex());
      if (!nameId.IsNull())
      {
         aStream << nameId;
      }
      else
      {
         aStream << aTrackPtr->GetTrackId();
      }
   }
}

// =================================================================================================
void PrintEvent(std::ostream&      aStream,
                double             aSimTime,
                const std::string& aSubjectName,
                const std::string& aEventName,
                const std::string& aObjectName,
                const std::string& aAdditionalInformation)
{
   PrintTime(aStream, aSimTime);
   aStream << ',' << aEventName;
   aStream << ',' << aSubjectName;
   aStream << ',' << aObjectName;
   aStream << ',' << aAdditionalInformation;
   aStream << '\n';
}

} // namespace utilsCSV

} // namespace event
} // namespace wsf
