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

#include "WsfEM_SurfaceClutter.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "UtEllipsoidalEarth.hpp"
#include "UtInput.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfAntennaPattern.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEnvironment.hpp"
#include "WsfMIT_ClutterStrength.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTypes.hpp"

static constexpr double sWaveHeight[20] = {0.0, 0.152, 0.457, 0.762, 1.22, 1.82, 3.049, 0.0, 0.0, 0.0,
                                           0.0, 0.0,   0.0,   0.0,   0.0,  0.0,  0.0,   0.0, 0.0, 0.0};

// =================================================================================================
WsfEM_SurfaceClutter::WsfEM_SurfaceClutter()
   : WsfEM_Clutter()
   , mMinimumRange(-1.0)
   , mMaximumRange(1.0E+30)
   , mContinuousWaveClutterPower(-1.0)
   , mUseSALRAM_DataTables(false)
{
}

// =================================================================================================
//! Factory method called by WsfEM_ClutterTypes.
// static
WsfEM_Clutter* WsfEM_SurfaceClutter::ObjectFactory(const std::string& aTypeName)
{
   WsfEM_Clutter* instancePtr = nullptr;
   if ((aTypeName == "WSF_SURFACE_CLUTTER") || (aTypeName == "surface_clutter"))
   {
      instancePtr = new WsfEM_SurfaceClutter();
   }
   return instancePtr;
}

// =================================================================================================
// virtual
WsfEM_Clutter* WsfEM_SurfaceClutter::Clone() const
{
   return new WsfEM_SurfaceClutter(*this);
}

// =================================================================================================
// virtual
bool WsfEM_SurfaceClutter::Initialize(WsfEM_Rcvr* aRcvrPtr)
{
   if (aRcvrPtr->GetPlatform()->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_SURFACE)
   {
      const auto seaState = aRcvrPtr->GetSimulation()->GetEnvironment().GetSeaState();
      double     lat;
      double     lon;
      double     alt;
      aRcvrPtr->GetAntenna()->GetLocationLLA(lat, lon, alt);
      if (alt < sWaveHeight[seaState])
      {
         auto out = ut::log::warning() << "surface_clutter: Antenna altitude is too low compared to wave height, and "
                                          "may produce undesired results.";
         out.AddNote() << "Platform: " << aRcvrPtr->GetPlatform()->GetName();
         out.AddNote() << "Antenna Altitude: " << alt << " m";
         out.AddNote() << "Wave Height: " << sWaveHeight[seaState] << " m";
      }
   }
   return true;
}

// =================================================================================================
// virtual
bool WsfEM_SurfaceClutter::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "minimum_range")
   {
      aInput.ReadValueOfType(mMinimumRange, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mMinimumRange, 0.0);
      aInput.ValueLess(mMinimumRange, mMaximumRange);
   }
   else if (command == "maximum_range")
   {
      aInput.ReadValueOfType(mMaximumRange, UtInput::cLENGTH);
      aInput.ValueGreater(mMaximumRange, mMinimumRange);
   }
   else if (command == "use_legacy_data")
   {
      aInput.ReadValue(mUseSALRAM_DataTables);
   }
   else
   {
      myCommand = WsfEM_Clutter::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
//! Computes the clutter signal for various terrain types.
//!
//! Land clutter data based on MIT Lincoln Lab's report on radar propagation at
//! low altitude and tabulated radar ground clutter, and from Nathanson page 272.
//! Data voids filled by J.A. Hueseman, E245 MDAC-STL. Radar ground clutter
//! data  as a function of depression angle and frequency was input by Ray
//! Perry in Nov of 1986.
//!
//! The sea clutter backscatter coefficient is computed as a function of
//! radar frequency (FMhz) and radar beam grazing angle for several sea
//!  environments and radar polarizations
//!
//! Basic sea clutter routine taken from NISC P001/ADRAD Model.
//! Original data from Nathanson pgs 231-239.
//! Data voids filled by E.M. Schultz, E245 MDAC-STL
// virtual
double WsfEM_SurfaceClutter::ComputeClutterPower(WsfEM_Interaction& aInteraction,
                                                 WsfEnvironment&    aEnvironment,
                                                 double             aProcessingFactor)
{
   // This model currently only works for monostatic radar interactions.
   double clutterPower = 0.0;
   if ((aInteraction.GetTransmitter() == nullptr) || (aInteraction.GetReceiver() == nullptr) || aInteraction.mBistatic)
   {
      return clutterPower;
   }

   if (aInteraction.GetTransmitter()->GetPulseRepetitionFrequency() == 0.0)
   {
      // C.W. radar cannot use a range gate to limit clutter returns
      // to target range. therefore, clutter from all ranges is seen.
      // if the antenna does not track the target, this clutter will
      // be the same on each call to cwlut. Therefore, only one call
      // is made for the "starting" antennas (e.g. csc(x)**2)

      WsfEM_Rcvr* rcvrPtr = aInteraction.GetReceiver();
      if ((mContinuousWaveClutterPower <= 0.0) && (rcvrPtr->GetAntenna()->GetScanMode() == WsfEM_Antenna::cSCAN_AZ))
      {
         clutterPower                = ContinuousWaveClutter(aInteraction, aEnvironment);
         mContinuousWaveClutterPower = clutterPower;
      }
   }
   // else if (GetPulseDoppler())
   //{
   //    clutterPower = PulseDopplerClutter(aInteraction, aEnvironment);
   // }
   else
   {
      clutterPower = PulseDopplerClutter(aInteraction, aEnvironment);
   }

   return clutterPower * aProcessingFactor;
}

// =================================================================================================
//! Compute clutter for continuous wave radars.
//!
//! @returns The clutter power (watts)
// From Salram::ContinuousWaveClutter.
double WsfEM_SurfaceClutter::ContinuousWaveClutter(WsfEM_Interaction& aInteraction, WsfEnvironment& aEnvironment)
{
   // Get the pointers to the receiver and transmitter
   WsfEM_Xmtr* xmtrPtr = aInteraction.GetTransmitter();
   WsfEM_Rcvr* rcvrPtr = aInteraction.GetReceiver();

   double clutterW = 1.0E-30;

   // Compute clutter horizon range (and other auxiliary variables)
   double viewVecNE[2];
   double ha; // adjusted antenna height
   double re; // adjusted effective earth radius
   double clutterHorizonRange;
   double depressionAngle;
   ComputeGeometry(aInteraction, aEnvironment, viewVecNE, ha, re, clutterHorizonRange, depressionAngle);

   // Calculate clutter cell step size required for 100 steps to clutter horizon
   double clutterStepSize = clutterHorizonRange / 100.0;

   WsfAntennaPattern* patternPtr(nullptr);
   WsfSpatialDomain   domain = WSF_SPATIAL_DOMAIN_SURFACE;

   // Assume clutter cell limited by range cell size (clutterStepSize)
   // use receiver frequency in case xmtr is at another center frequency
   double frequency = xmtrPtr->GetFrequency();
   if (rcvrPtr != nullptr)
   {
      if (rcvrPtr->GetFunction() != WsfEM_Rcvr::cRF_PASSIVE_SENSOR)
      {
         frequency = rcvrPtr->GetFrequency();
      }

      patternPtr = rcvrPtr->GetAntennaPattern(xmtrPtr->GetPolarization(), frequency);
   }

   double azBeamwidth(0.0);
   if (patternPtr != nullptr)
   {
      azBeamwidth =
         patternPtr->GetAzimuthBeamwidth(rcvrPtr->GetFrequency(), 0.0, 0.0); // use receiver frequency in case xmtr is
                                                                             // at another center frequency
   }
   if (azBeamwidth < (0.001 * UtMath::cRAD_PER_DEG))
   {
      return clutterW;
   }
   double azBeamWidth2    = 0.71 * azBeamwidth;
   double clutterCellArea = clutterStepSize * azBeamWidth2;

   // Loop for summation of clutter contributions from all
   // range cells nearer than the clutter horizon

   // First range from which clutter is received equals antenna height + 1m
   // (I don't know why slant range was not used in pdclut!)
   double rn = ha + 1.0;

   // If clutter response would come from range beyond clutter horizon, no clutter returned
   double loopVar;
   if (domain == WSF_SPATIAL_DOMAIN_SURFACE)
   {
      // If by sea don't allow clutter returns beyond horizon
      loopVar = clutterHorizonRange;
   }
   else
   {
      // If by land, allow for diffracted returns beyond clutter horizon
      loopVar = clutterHorizonRange * 2.0;
   }
   loopVar = std::min(loopVar, mMaximumRange);

   // If a minimum range was not specified use a value of 200m. There is an issue with the
   // antenna patterns (compared to SALRAM), which causes BIG returns for very short ranges.
   double minimumRange = (mMinimumRange >= 0.0) ? mMinimumRange : 200.0;

   rn -= clutterStepSize;
   while (rn <= loopVar)
   {
      // Increment clutter ground range
      rn += clutterStepSize;

      if (rn < minimumRange)
      {
         continue;
      }

      // Compute radar grazing angle at clutter spot - clutterGrazingAngle (radians)
      // and radar beam depression angle to same spot - depressionAngle (radians)
      double clutterGrazingAngle = asin((ha * (1 + (ha / (2.0 * re))) / rn) - rn / (2.0 * re));
      if (clutterGrazingAngle < 0.0)
      {
         clutterGrazingAngle = 0.0;
      }

      double lat;
      double lon;
      double alt;
      double vecNED[3] = {viewVecNE[0], viewVecNE[1], sin(depressionAngle)};
      double vecWCS[3];
      aInteraction.GetReceiver()->GetAntenna()->ConvertNED_ToWCS(vecNED, vecWCS);
      UtEllipsoidalEarth::ConvertECEFToLLA(vecWCS, lat, lon, alt);

      // Compute clutter backscatter coefficient - z(dB) & sigo(pwr)
      double z;
      if (domain == WSF_SPATIAL_DOMAIN_SURFACE)
      {
         z = WsfMIT_ClutterStrength::GetSeaClutterStrength(clutterGrazingAngle,
                                                           aEnvironment.GetSeaState(),
                                                           xmtrPtr->GetFrequency(),
                                                           xmtrPtr->GetPolarization());
      }
      else if (mUseSALRAM_DataTables)
      {
         z = WsfMIT_ClutterStrength::GetLandClutterStrengthLegacy(depressionAngle,
                                                                  aEnvironment.GetLandFormation(lat, lon),
                                                                  aEnvironment.GetLandCover(lat, lon),
                                                                  xmtrPtr->GetFrequency(),
                                                                  xmtrPtr->GetPolarization());
      }
      else
      {
         z = WsfMIT_ClutterStrength::GetLandClutterStrength(depressionAngle,
                                                            aEnvironment.GetLandFormation(lat, lon),
                                                            aEnvironment.GetLandCover(lat, lon),
                                                            xmtrPtr->GetFrequency(),
                                                            xmtrPtr->GetPolarization());
      }
      double sig0 = pow(10.0, (z / 10.0));

      // Compute clutter power returned from range rn - pc1(watts)
      // based on Nathanson pg 66, eqns 2-39 & 2-40
      //
      // Assume clutter cell limited by range cell size (clutterStepSize)
      double cellRCS = sig0 * clutterCellArea * rn / cos(clutterGrazingAngle);
      double pc1     = ComputeRF_TwoWayPower(aInteraction, viewVecNE, depressionAngle, rn, cellRCS);

      // auto out = ut::log::info() << "Continuous Wave Clutter:";
      // out.AddNote() << "PC1: " << UtMath::LinearToDB(pc1) << " dB";
      // out.AddNote() << "Sig0: " << sig0;
      // out.AddNote() << "R: " << rn;
      // out.AddNote() << "GA: " << clutterGrazingAngle * UtMath::cDEG_PER_RAD << " deg";
      // out.AddNote() << "DA: " << depressionAngle * UtMath::cDEG_PER_RAD << " deg"
      // out.AddNote() << "CCS: " << cellRCS;

      // Sum clutter from all range cells - pclut(watts)
      clutterW += pc1;
   }
   return clutterW;
}

// =================================================================================================
//! Computes surface clutter return to radar.
//!
//! Clutter may be present from surface patches at the target range
//! or at ambiguous ranges closer than the target or more distant.
//! This Routine computes the additive components from all applicable
//! unambiguous and ambiguous ranges with appropriate antenna pattern
//! and backscatter reflectivity weighting.
//!
//! @returns The clutter power (watts)
// From SalramClutter::PulseDopperClutter
double WsfEM_SurfaceClutter::PulseDopplerClutter(WsfEM_Interaction& aInteraction, WsfEnvironment& aEnvironment)
{
   // Get the pointers to the receiver and transmitter
   WsfEM_Xmtr* xmtrPtr = aInteraction.GetTransmitter();
   WsfEM_Rcvr* rcvrPtr = aInteraction.GetReceiver();

   double clutterW = 1.0E-30;

   // Compute range resolution for clutter cell size - resolution(m)
   double pulseWidth = xmtrPtr->GetPulseWidth();
   if (pulseWidth == 0.0)
   {
      // No pulse width... assume matched filter.
      double bandwidth = rcvrPtr->GetBandwidth();
      if (bandwidth <= 0.0)
      {
         return clutterW;
      }
      pulseWidth = 1.0 / bandwidth;
   }
   pulseWidth /= xmtrPtr->GetPulseCompressionRatio(); // The processed pulse width
   double resolution = (UtMath::cLIGHT_SPEED * pulseWidth) / 2.0;

   // Assume clutter cell limited by pulse length
   WsfAntennaPattern* patternPtr  = rcvrPtr->GetAntennaPattern(xmtrPtr->GetPolarization(), xmtrPtr->GetFrequency());
   double             azBeamwidth = patternPtr->GetAzimuthBeamwidth(xmtrPtr->GetFrequency(), 0.0, 0.0);
   if (azBeamwidth < (0.001 * UtMath::cRAD_PER_DEG))
   {
      return clutterW;
   }
   double azBeamWidth2    = 0.71 * azBeamwidth;
   double clutterCellArea = resolution * azBeamWidth2;

   double slantRange = aInteraction.mXmtrToTgt.mRange;

   // Compute the unambiguous range
   double prf              = xmtrPtr->GetPulseRepetitionFrequency();
   double unambiguousRange = UtMath::cLIGHT_SPEED / (2.0 * prf);

   // Compute radar blind range
   double blindRange = 0.1 * unambiguousRange;

   // Compute clutter horizon range (and other auxiliary variables)
   double viewVecNE[2];
   double ha; // adjusted antenna height
   double re; // adjusted effective earth radius
   double clutterHorizonRange;
   double depressionAngle;
   ComputeGeometry(aInteraction, aEnvironment, viewVecNE, ha, re, clutterHorizonRange, depressionAngle);

   // Determine number of ambiguous ranges at which clutter responses will be generated - inumb1
   double maximumRange = std::max(2.0 * clutterHorizonRange, slantRange);
   maximumRange        = std::min(maximumRange, mMaximumRange);
   double anumb        = maximumRange / unambiguousRange;
   int    inumb        = static_cast<int>(anumb + 0.5);
   int    inumb1       = (2 * inumb) + 1;

   double minimumRange = std::max(mMinimumRange, ha + 1.0);
   // Loop for summation of clutter contributions from ambiguous ranges
   WsfSpatialDomain domain = rcvrPtr->GetPlatform()->GetSpatialDomain();
   for (int ii = 1; ii <= inumb1; ++ii)
   {
      int i = ii - (inumb + 1);

      // Compute slant range from which clutter is received - rn (meters)
      double rn = slantRange - (unambiguousRange * i);
      if (rn < minimumRange)
      {
         continue;
      }
      // Check if high prf return falls inside radar blind range
      if ((prf >= 6.0E+3) && (blindRange > rn))
      {
         continue;
      }
      if ((domain == WSF_SPATIAL_DOMAIN_SURFACE) && (rn > clutterHorizonRange))
      {
         continue;
      }

      // Compute radar grazing angle at clutter spot and radar beam depression angle to same spot.
      // Don't allow radar beam to look through the earth's limb

      double clutterGrazingAngle = asin((ha * (1.0 + (ha / (2.0 * re))) / rn) - (rn / (2.0 * re)));
      if (clutterGrazingAngle < 0.0)
      {
         clutterGrazingAngle = 0.0;
      }

      // Compute clutter backscatter coefficient - z(db) & sig0(pwr)
      double z;
      if (domain == WSF_SPATIAL_DOMAIN_SURFACE)
      {
         z = WsfMIT_ClutterStrength::GetSeaClutterStrength(clutterGrazingAngle,
                                                           aEnvironment.GetSeaState(),
                                                           xmtrPtr->GetFrequency(),
                                                           xmtrPtr->GetPolarization());
      }
      else if (mUseSALRAM_DataTables)
      {
         z = WsfMIT_ClutterStrength::GetLandClutterStrengthLegacy(depressionAngle,
                                                                  aEnvironment.GetLandFormation(),
                                                                  aEnvironment.GetLandCover(),
                                                                  xmtrPtr->GetFrequency(),
                                                                  xmtrPtr->GetPolarization());
      }
      else
      {
         z = WsfMIT_ClutterStrength::GetLandClutterStrength(depressionAngle,
                                                            aEnvironment.GetLandFormation(),
                                                            aEnvironment.GetLandCover(),
                                                            xmtrPtr->GetFrequency(),
                                                            xmtrPtr->GetPolarization());
      }
      double sig0 = pow(10.0, (z / 10.0));

      // Compute clutter power returned from range rn - pc1(watts)
      // based on Nathanson pg 66, eqns 2-39 & 2-40
      // Assume clutter cell limited by pulse length
      double cellRCS = sig0 * clutterCellArea * rn / cos(clutterGrazingAngle);
      double pc1     = ComputeRF_TwoWayPower(aInteraction, viewVecNE, depressionAngle, rn, cellRCS);

      // auto out = ut::log::info() << "Pulse Doppler Clutter:";
      // out.AddNote() << "PC1: " << UtMath::LinearToDB(pc1) << " dB";
      // out.AddNote() << "Sig0: " << sig0;
      // out.AddNote() << "R: " << rn;
      // out.AddNote() << "GA: " << clutterGrazingAngle * UtMath::cDEG_PER_RAD << " deg";
      // out.AddNote() << "DA: " << depressionAngle * UtMath::cDEG_PER_RAD << " deg"
      // out.AddNote() << "CCS: " << cellRCS;

      // Sum clutter from all range cells -aPClut(watts)
      clutterW += pc1;
   }
   return clutterW;
}


// =================================================================================================
//! Compute the clutter horizon range and other auxiliary variables.
// private
void WsfEM_SurfaceClutter::ComputeGeometry(WsfEM_Interaction& aInteraction,
                                           WsfEnvironment&    aEnvironment,
                                           double             aViewVecNE[2],
                                           double&            aAdjustedAntennaHeight,
                                           double&            aAdjustedEarthRadius,
                                           double&            aClutterHorizonRange,
                                           double&            aDepressionAngle)
{
   // Compute the adjusted antenna height and earth radius as in SalramMode::Initialize
   WsfEM_Xmtr* xmtrPtr = aInteraction.GetTransmitter();
   double      lat;
   double      lon;
   double      alt;
   xmtrPtr->GetAntenna()->GetLocationLLA(lat, lon, alt);
   double antennaHeight = std::max(alt - xmtrPtr->GetPlatform()->GetTerrainHeight(), 1.0);

   // We're using SALRAM's version of the scaled earth radius (SalramUtil::cAE) for comparison purposes.

   // double earthRadius   = UtSphericalEarth::cEARTH_RADIUS * xmtrPtr->GetEarthRadiusMultiplier();
   double earthRadius = 8476090.0 * (3.0 / 4.0) * xmtrPtr->GetEarthRadiusMultiplier();

   // Compute the basic height adjustment and adjusted earth radius and antenna
   // height according to the sea state or land formation...

   WsfSpatialDomain domain = xmtrPtr->GetPlatform()->GetSpatialDomain();
   if (domain == WSF_SPATIAL_DOMAIN_SURFACE)
   {
      // Sea-based radar
      int seaState           = aEnvironment.GetSeaState();
      seaState               = std::min(std::max(seaState, 0), 6);
      aAdjustedAntennaHeight = antennaHeight - sWaveHeight[seaState];
      aAdjustedEarthRadius   = earthRadius + sWaveHeight[seaState];
   }
   else
   {
      // Land-based radar
      int landFormation      = aEnvironment.GetLandFormation();
      landFormation          = std::min(std::max(landFormation, 0), 10);
      aAdjustedAntennaHeight = antennaHeight + (sWaveHeight[9 + landFormation] / 2.0);
      aAdjustedEarthRadius   = earthRadius + (sWaveHeight[9 + landFormation] / 2.0);
   }

   // The standard clutter horizon range calculation from SalramClutter.
   aClutterHorizonRange = 4123.0 * sqrt(std::max(aAdjustedAntennaHeight, 0.0));

   // Compute the 'north-east viewing unit vector'.
   //
   // This is a 2-D unit vector from the receiver that points in along the line-of-sight.
   // This is only the horizontal (north-east) components. The vertical component will get
   // computed so as to point to the clutter cell.

   double viewVecBCS[3] = {1.0, 0.0, 0.0};
   double viewVecWCS[3];
   double viewVecNED[3];
   UtMat3d::InverseTransform(viewVecWCS, aInteraction.mRcvrBeam.mWCS_ToBeamTransform, viewVecBCS);
   aInteraction.GetReceiver()->GetAntenna()->ConvertWCS_VectorToNED(viewVecWCS, viewVecNED);
   aDepressionAngle = asin(viewVecNED[2]);

   // Get the normalized 2-D vector (horizontal components only)
   viewVecNED[2] = 0.0;
   UtVec3d::Normalize(viewVecNED);
   aViewVecNE[0] = viewVecNED[0];
   aViewVecNE[1] = viewVecNED[1];
}

// =================================================================================================
//! Computes the power received from a clutter cell.
//! @param aInteraction     The interaction object.
//! @param aViewVecNE       The line of sight viewing vector (see ComputeGeometry for details).
//! @param aDepressionAngle The depression angle to the clutter cell (radians).
//! @param aSlantRange      The slant range to the clutter cell (meters).
//! @param aCrossSection    The effective radar cross section the clutter cell (meters^2).
//! @returns the received clutter power (Watts).
// private
double WsfEM_SurfaceClutter::ComputeRF_TwoWayPower(WsfEM_Interaction& aInteraction,
                                                   const double       aViewVecNE[2],
                                                   double             aDepressionAngle,
                                                   double             aSlantRange,
                                                   double             aCrossSection)
{
   WsfEM_Xmtr* xmtrPtr = aInteraction.GetTransmitter();
   WsfEM_Rcvr* rcvrPtr = aInteraction.GetReceiver();

   // Compute the elevation angle of the clutter cell with respect to the beam center.
   //
   // aViewVecNE is a unit vector that points in the horizontal direction from the receiver
   // along the line-of-sight towards the target (See ComputeGeometry). The vertical component
   // will now be computed to now point at the clutter cell.

   double clutterVecNED[3] = {aViewVecNE[0], aViewVecNE[1], sin(aDepressionAngle)};
   double clutterVecWCS[3];
   rcvrPtr->GetAntenna()->ConvertNED_VectorToWCS(clutterVecNED, clutterVecWCS);

   double beamToClutterAz;
   double beamToClutterEl;
   rcvrPtr->GetAntenna()->ComputeBeamAspect(aInteraction.mRcvrBeam.mWCS_ToBeamTransform,
                                            clutterVecWCS,
                                            beamToClutterAz,
                                            beamToClutterEl);

   // Get radiated power (watts)
   double xmtrGain;
   double xmtdPower = xmtrPtr->ComputeRadiatedPower(aInteraction.mXmtrBeam.mAz,
                                                    beamToClutterEl,
                                                    aInteraction.mXmtrBeam.mEBS_Az,
                                                    aInteraction.mXmtrBeam.mEBS_El,
                                                    rcvrPtr->GetFrequency(),
                                                    xmtrGain);

   // Compute attenuation factor (assume monostatic)
   //
   // SALRAM did not perform atmospheric attenuation on the clutter, so neither will we...
   double attenuation = 1.0;

   // Propagate signal to clutter cell
   double p_density_at_tgt = xmtdPower * attenuation / (UtMath::cFOUR_PI * aSlantRange * aSlantRange);

   // Compute reflected signal from clutter cell
   double p_reflected = p_density_at_tgt * aCrossSection;

   // Propagate reflected signal to receiver
   double p_density_at_rcvr = p_reflected * attenuation / (UtMath::cFOUR_PI * aSlantRange * aSlantRange);

   // Receive the signal
   double rcvrGain;
   double rcvdPower = rcvrPtr->ComputeReceivedPower(aInteraction.mRcvrBeam.mAz,
                                                    beamToClutterEl,
                                                    aInteraction.mRcvrBeam.mEBS_Az,
                                                    aInteraction.mRcvrBeam.mEBS_El,
                                                    p_density_at_rcvr,
                                                    xmtrPtr->GetPolarization(),
                                                    xmtrPtr->GetFrequency(),
                                                    rcvrGain);

   return rcvdPower;
}
