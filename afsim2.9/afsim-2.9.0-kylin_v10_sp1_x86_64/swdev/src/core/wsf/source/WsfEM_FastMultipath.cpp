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

// Documents referenced in this file:
//
// Ref 1. "Radar Range-Performance Analysis"
//        Lamont V. Blake, 1986, Artech House, Inc.

#include "WsfEM_FastMultipath.hpp"

#include <algorithm>
#include <cmath>
#include <complex>
#include <iostream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"

// =================================================================================================
WsfEM_FastMultipath::WsfEM_FastMultipath()
   : mSoilMoistureFraction(0.15)
   , mSurfaceRoughness(3.0)
{
}

// =================================================================================================
WsfEM_FastMultipath::WsfEM_FastMultipath(const WsfEM_FastMultipath& aSrc)
   : mSoilMoistureFraction(aSrc.mSoilMoistureFraction)
   , mSurfaceRoughness(aSrc.mSurfaceRoughness)
{
}

// =================================================================================================
// virtual
WsfEM_FastMultipath::~WsfEM_FastMultipath() {}

// =================================================================================================
//! Factory method called by WsfEM_PropagationTypes.
// static
WsfEM_Propagation* WsfEM_FastMultipath::ObjectFactory(const std::string& aTypeName)
{
   WsfEM_Propagation* instancePtr = nullptr;
   if ((aTypeName == "WSF_FAST_MULTIPATH") || (aTypeName == "fast_multipath"))
   {
      instancePtr = new WsfEM_FastMultipath();
   }
   return instancePtr;
}

// =================================================================================================
// virtual
WsfEM_Propagation* WsfEM_FastMultipath::Clone() const
{
   return new WsfEM_FastMultipath(*this);
}

// =================================================================================================
// virtual
double WsfEM_FastMultipath::ComputePropagationFactor(WsfEM_Interaction& aInteraction, WsfEnvironment& aEnvironment)
{
   WsfEM_Xmtr* xmtrPtr = aInteraction.GetTransmitter();
   if (xmtrPtr == nullptr)
   {
      return 1.0;
   }
   WsfEM_Rcvr* rcvrPtr = aInteraction.GetReceiver();

   double frequency = xmtrPtr->GetFrequency();
   if ((rcvrPtr != nullptr) && (rcvrPtr->GetFunction() != WsfEM_Rcvr::cRF_PASSIVE_SENSOR))
   {
      // use receiver frequency in case xmtr is at another center frequency
      frequency = rcvrPtr->GetFrequency();
   }

   // Get the relative dielectric constant
   std::complex<double> epsilon;
   GetSoilDielectricConstant(frequency, mSoilMoistureFraction, epsilon);

   double localEarthRadius = UtVec3d::Magnitude(aInteraction.mRcvrLoc.mLocWCS) - aInteraction.mRcvrLoc.mAlt;
   double earthRadius      = xmtrPtr->GetEarthRadiusMultiplier() * localEarthRadius;
   double wavelength       = UtMath::cLIGHT_SPEED / frequency;

   double antToRefSlantRange;
   double refToTgtSlantRange;
   double antElevationAngle;
   double grazingAngle;
   double pathLengthDifference;
   if (!ComputeReflectionGeometry(earthRadius,
                                  xmtrPtr->GetAntenna()->GetHeight(),
                                  aInteraction.mXmtrToTgt.mRange,
                                  aInteraction.mXmtrToTgt.mEl,
                                  antToRefSlantRange,
                                  refToTgtSlantRange,
                                  antElevationAngle,
                                  grazingAngle,
                                  pathLengthDifference))
   {
      return 1.0;
   }

   // Compute the magnitude and phase shift of the reflection

   double rho_0; // The magnitude of the reflection coefficient
   double phi;   // The phase angle of the reflection coefficient
   ComputeReflectionCoefficient(grazingAngle, epsilon, xmtrPtr->GetPolarization(), rho_0, phi);

   // -----------------------------------------------------------------------
   // Determine the specularity coefficient which reduces the purely
   // specular reflection due to the surface roughness.  This
   // calculation is based on equation 6.74 on page 266 of reference 1.
   //
   // Note that diffuse reflection as an enhancement factor is not taken
   // into account as described on page 268 and 269 of reference 1.
   // -----------------------------------------------------------------------

   double temp1 = (UtMath::cTWO_PI * mSurfaceRoughness * sin(grazingAngle)) / wavelength;
   double temp2 = -2.0 * temp1 * temp1;
   double rho_s = 0.0;
   if (temp2 > -700.0)
   {
      rho_s = exp(temp2);
   }

   // -----------------------------------------------------------------------
   // Determine the two intermediate values that will be used to
   // compute F.
   //
   // Determine F, the one-way pattern propagation factor based on
   // equation 6.9 on page 242 of reference 1.
   //
   //  RATIO = SQRT(GAINR/GAIND)
   //  F = DSQRT(1.0D0 + (RHO*RATIO)**2 + RHO * RATIO * TWOCOS)
   //
   // Where GAINR is the antenna gain in the direction of the reflected
   // path and GAIND is the antenna gain along the direct path.
   //
   // F is needs to be computed outside this routine where the antenna
   // gain values can be looked up.
   //
   // Note: In reference 1 this value is multiplied by the gain in the
   // direction of the target, however, in this case the gain is
   // included separately in the S/N equation in subroutine SGTONO.
   // -----------------------------------------------------------------------

   double twoCos = 2.0 * cos((UtMath::cTWO_PI * pathLengthDifference / wavelength) + phi);
   double rho    = rho_s * rho_0;

   // TODO-FOR NOW we assume the geometry for transmit and receive is the same!
   double xmtElevationAngle = antElevationAngle; // TODO
   double rcvElevationAngle = antElevationAngle; // TODO

   // Determine the direct and reflected transmit antenna gain.

   double xmtFactor = 1.0;
   double xmtGainD  = aInteraction.mXmtrBeam.mGain;
   double xmtGainR  = ComputeReflectionGain(xmtrPtr,
                                           aInteraction.mXmtrBeam,
                                           aInteraction.mXmtrToTgt,
                                           xmtElevationAngle,
                                           frequency,
                                           xmtrPtr->GetPolarization());
   double xmtRatio  = sqrt(xmtGainR / xmtGainD);
   if (fabs(rho) > 1.0E-100)
   {
      double temp = rho * xmtRatio;
      xmtFactor   = 1.0 + (temp * temp) + (temp * twoCos);
   }

   // Determine the direct and reflected receive antenna gain.

   double rcvFactor = 1.0;
   if (rcvrPtr != nullptr)
   {
      double rcvGainD = aInteraction.mRcvrBeam.mGain;
      double rcvGainR = ComputeReflectionGain(rcvrPtr,
                                              aInteraction.mRcvrBeam,
                                              aInteraction.mRcvrToTgt,
                                              rcvElevationAngle,
                                              frequency,
                                              xmtrPtr->GetPolarization());
      double rcvRatio = sqrt(rcvGainR / rcvGainD);
      if (fabs(rho) > 1.0E-100)
      {
         double temp = rho * rcvRatio;
         rcvFactor   = 1.0 + (temp * temp) + (temp * twoCos);
      }
   }

   // Combine the transmit and receive propagation factors.
   return xmtFactor * rcvFactor;
}

// =================================================================================================
//! Compute the antenna gain in the direction of the reflection point.
//! @param aXmtrRcvrPtr     Pointer to the transmitter/receiver.
//! @param aBeamData        The beam pointing data.
//! @param aRelTgtLoc       The position of the target relative to the antenna.
//! @param aDepressionAngle The elevation angle of the reflection point relative to the horizontal plane.
//!                         This should be negative to represent that it is below the horizontal plane.
//! @param aFrequency       The frequency of the signal (Hz)
//! @param aPolarization    The polarization of the signal
// virtual
double WsfEM_FastMultipath::ComputeReflectionGain(WsfEM_XmtrRcvr*                        aXmtrRcvrPtr,
                                                  const WsfEM_Interaction::BeamData&     aBeamData,
                                                  const WsfEM_Interaction::RelativeData& aRelTgtLoc,
                                                  double                                 aDepressionAngle,
                                                  double                                 aFrequency,
                                                  WsfEM_Types::Polarization              aPolarization)
{
   // Convert the WCS vector that points from the antenna to the target into the NED frame of the antenna.
   // We only need this to get the horizontal components (azimuth). The depression angle will be used to
   // get the full 3-D pointing vector for the reflected path.

   WsfEM_Antenna* antennaPtr = aXmtrRcvrPtr->GetAntenna();
   double         refVecNED[3]; // Eventual NED vector to the reflection point
   antennaPtr->ConvertWCS_VectorToNED(aRelTgtLoc.mUnitVecWCS, refVecNED);

   double newD  = -sin(aDepressionAngle);
   double oldNE = std::max(1.0, sqrt(refVecNED[0] * refVecNED[0] + refVecNED[1] * refVecNED[1]));
   double newNE = sqrt(1.0 - newD * newD);

   refVecNED[0] = newNE * refVecNED[0] / oldNE;
   refVecNED[1] = newNE * refVecNED[1] / oldNE;
   refVecNED[2] = newD;

   // Convert the NED vector back to the WCS frame and then get the aspect relative to the beam boresight.
   double refVecWCS[3]; // WCS vector to the reflection point.
   double az;
   double el;
   antennaPtr->ConvertNED_VectorToWCS(refVecNED, refVecWCS);
   antennaPtr->ComputeBeamAspect(aBeamData.mWCS_ToBeamTransform, refVecWCS, az, el);

   double gain = aXmtrRcvrPtr->GetAntennaGain(aPolarization, aFrequency, az, el, aBeamData.mEBS_Az, aBeamData.mEBS_El);
   return gain;
}

// =================================================================================================
// virtual
bool WsfEM_FastMultipath::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "soil_moisture_fraction")
   {
      double soilMoistureFraction;
      aInput.ReadValue(soilMoistureFraction);
      aInput.ValueInClosedRange(soilMoistureFraction, 0.0, 1.0);
      SetSoilMoistureFraction(soilMoistureFraction);
   }
   else if (command == "soil_moisture")
   {
      double soilMoisture;
      aInput.ReadValue(soilMoisture);
      aInput.ValueInClosedRange(soilMoisture, 0.0, 100.0);
      SetSoilMoistureFraction(0.01 * soilMoisture);
   }
   else if ((command == "surface_roughness") || (command == "stddev_surface_height"))
   {
      double surfaceRoughness;
      aInput.ReadValueOfType(surfaceRoughness, UtInput::cLENGTH);
      aInput.ValueGreater(surfaceRoughness, 0.0);
      SetSurfaceRoughness(surfaceRoughness);
   }
   else
   {
      myCommand = WsfEM_Propagation::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
void WsfEM_FastMultipath::SetSoilMoistureFraction(double aSoilMoistureFraction)
{
   mSoilMoistureFraction = aSoilMoistureFraction;
}

// =================================================================================================
void WsfEM_FastMultipath::SetSurfaceRoughness(double aSurfaceRoughness)
{
   mSurfaceRoughness = aSurfaceRoughness;
}

// =================================================================================================
//! Compute the magnitude and phase shift of a reflected signal.
//!
//! @param aGrazingAngle [input] The grazing angle (radians)
//! @param aEpsilon      [input] The relative dielectric constant of the soil.
//! @param aPolarization [input] The polarization of the signal.
//! @param aMagnitude    [output] The magnitude of the complex reflection coefficient.
//! @param aPhaseShift   [output] The phase shift (radians) of the complex reflection coefficient.
//!
// static
void WsfEM_FastMultipath::ComputeReflectionCoefficient(double                      aGrazingAngle,
                                                       const std::complex<double>& aEpsilon,
                                                       WsfEM_Types::Polarization   aPolarization,
                                                       double&                     aMagnitude,
                                                       double&                     aPhaseShift)
{
   double               sinalp = sin(aGrazingAngle);
   double               cosalp = cos(aGrazingAngle);
   std::complex<double> csrec2(std::sqrt(aEpsilon - (cosalp * cosalp)));

   // Determine the complex reflection coefficient.

   std::complex<double> gamma;
   if (aPolarization == WsfEM_Types::cPOL_VERTICAL)
   {
      // The antenna is vertically polarized.

      gamma = std::complex<double>(aEpsilon * sinalp - csrec2) / std::complex<double>(aEpsilon * sinalp + csrec2);
   }
   else
   {
      // If the antenna is not vertically polarized then it is assumed to be horizontally polarized.

      gamma = std::complex<double>(sinalp - csrec2) / std::complex<double>(sinalp + csrec2);
   }

   // Compute the magnitude and phase shift of the complex reflection coefficient

   aMagnitude  = std::abs(gamma);  // rho_0
   aPhaseShift = -std::arg(gamma); // phi
}

// =================================================================================================
//! Compute the reflection geometry for a smooth, spherical earth.
//!
//! This method determines the reflection point using the algorithm defined in chapter 6 of:
//!
//!   "Radar Range-Performance Analysis", Lamont V. Blake, 1986, Artech House, Inc.
//!
//! @param aEarthRadius           [input] The effective earth radius
//!                                       (a_e in the algorithm)
//! @param aAntHeight             [input] Antenna above MSL (meters).
//!                                       (h_1 in the algorithm).
//! @param aTgtSlantRange         [input] The slant range from the antenna to the target
//!                                       (R_d in the algorithm).
//! @param aTgtElevation          [input] The elevation angle of the target relative to the horizontal plane at the antenna.
//!                                       (theta_d in the algorithm).
//! @param aAntToRefSlantRange   [output] The slant range from the antenna to the reflection point.
//!                                       (R_1 in the algorithm).
//! @param aRefToTgtSlantRange   [output] The slant range from the reflection point to the target.
//!                                       (R_2 in the algorithm).
//! @param aAntElevationAngle    [output] The angle between the horizontal plane at the antenna and the ray from the antenna
//!                                       to the reflection point. This is always negative to indicate that it is BELOW.
//!                                       (theta_r in the algorithm).
//! @param aGrazingAngle         [output] The angle between the reflection path and the horizontal at the reflection point.
//!                                       (psi in the algorithm).
//! @param aPathLengthDifference [output] The difference in the path length for the direct and reflected path.
//!
//! @return true if successfully computed or false if either the antenna or target is below the ground.
// static
bool WsfEM_FastMultipath::ComputeReflectionGeometry(double  aEarthRadius,
                                                    double  aAntHeight,
                                                    double  aTgtSlantRange,
                                                    double  aTgtElevation,
                                                    double& aAntToRefSlantRange,
                                                    double& aRefToTgtSlantRange,
                                                    double& aAntElevationAngle,
                                                    double& aGrazingAngle,
                                                    double& aPathLengthDifference)
{
   static const double cCONST1 = 2.0 / sqrt(3.0);

   double a_e = aEarthRadius;

   // -----------------------------------------------------------------------
   // If the antenna height is zero then it is assumed that there is no
   // multipath, therefore the multipath factor is set to one.
   // -----------------------------------------------------------------------

   double h_1 = aAntHeight;
   if (h_1 <= 0.0)
   {
      return false;
   }

   // -----------------------------------------------------------------------
   // The total distance from the center of the earth to the antenna,
   // for a 4/3 earth is:
   // -----------------------------------------------------------------------
   double ah_1 = a_e + h_1;

   // -----------------------------------------------------------------------
   // The distance from the center of the earth to the target is found
   // using the law of cosines, noting that cos(PI/2 + ELEVTGT) =
   // -sin(ELVTGT).
   // -----------------------------------------------------------------------
   double R_d     = aTgtSlantRange;
   double theta_d = aTgtElevation;
   double ah_2    = sqrt((R_d * R_d) + (ah_1 * ah_1) + (2.0 * R_d * ah_1 * sin(theta_d)));

   // -----------------------------------------------------------------------
   // Check to see if the target is masked by the earth surface.
   // -----------------------------------------------------------------------
   if (ah_2 < a_e)
   {
      return false;
   }

   // -----------------------------------------------------------------------
   // The target altitude is found by:
   // -----------------------------------------------------------------------
   double h_2 = ah_2 - a_e;

   // -----------------------------------------------------------------------
   // The ground range from the antenna to the target is found using
   // equation 6.57a, page 256 of reference 1.
   // -----------------------------------------------------------------------
   // Using Equation 6.57a, since 6.57 cause too large a number in the
   // denominator.  This is an approximation ???
   // -----------------------------------------------------------------------

   // -----------------------------------------------------------------------
   // Avoid SQRT of a neg. for small ranges
   // -----------------------------------------------------------------------
   double G     = R_d; // assume a small range
   double del_h = h_2 - h_1;
   if (R_d > fabs(del_h))
   {
      G = sqrt(((R_d * R_d) - (del_h * del_h)) / (1.0 + (h_1 + h_2) / a_e));
   }

   // -----------------------------------------------------------------------
   // Determine the ground range from the target to the specular
   // reflection point, but first calculate some intermediate
   // quantities, from equations 6.49 and 6.50, page 254 of reference 1.
   // -----------------------------------------------------------------------

   double G_over_2 = 0.5 * G;
   double p        = cCONST1 * sqrt(a_e * (h_1 + h_2) + (G_over_2 * G_over_2));
   double zeta     = asin((2.0 * a_e * G * (h_2 - h_1)) / (p * p * p));

   // -----------------------------------------------------------------------
   // Using equation 6.48 of reference 1, the ground range from the
   // antenna to the specular reflection point is then calculated.
   // -----------------------------------------------------------------------
   double G_1 = (G / 2.0) - (p * sin(zeta / 3.0));

   // -----------------------------------------------------------------------
   // The ground range from the specular reflection point is the difference
   // between G_1 calculated above and the total ground range G.
   // -----------------------------------------------------------------------
   double G_2 = G - G_1;

   // -----------------------------------------------------------------------
   // Next, find the angle between the line intersecting the earth
   // center and the antenna and the line intersecting the earth center
   // and the specular reflection point using equation 6.51, page 254 of
   // reference 1.
   // -----------------------------------------------------------------------
   double phi_1 = G_1 / a_e;

   // -----------------------------------------------------------------------
   // The angle between the line intersecting the earth center and the
   // target and the line intersecting the earth center and the specular
   // reflection point is now found based on equation 6.52, page 254 of
   // reference 1.
   // -----------------------------------------------------------------------
   double phi_2 = G_2 / a_e;

   // -----------------------------------------------------------------------
   // Calculate slant range from the antenna to the specular reflection
   // point and the range from the specular reflection point to the
   // target using equation 6.54, page 254 of reference 1.
   // -----------------------------------------------------------------------
   double tmp_1 = sin(0.5 * phi_1);
   double R_1   = sqrt((h_1 * h_1) + (4.0 * a_e * ah_1 * (tmp_1 * tmp_1)));

   double tmp_2 = sin(0.5 * phi_2);
   double R_2   = sqrt((h_2 * h_2) + (4.0 * a_e * ah_2 * (tmp_2 * tmp_2)));

   // -----------------------------------------------------------------------
   // Determine the angle of the reflected path to the horizontal at the
   // antenna using equation 6.60, page 256 of reference 1.  This value
   // and the elevation angle of the target are needed to determine the
   // antenna gain values applied to the direct and reflected paths.
   // -----------------------------------------------------------------------
   double theta_r = asin(((2.0 * a_e * h_1) + (h_1 * h_1) + (R_1 * R_1)) / (2.0 * ah_1 * R_1));

   // -----------------------------------------------------------------------
   // Find the grazing angle relative the earth surface at the specular
   // reflection point using equation 6.62, page 257 of reference 1.
   // -----------------------------------------------------------------------
   // Large numbers cause problems with eq 6.62, therefore use equation 6.62b
   // -----------------------------------------------------------------------
   double psi = theta_r - phi_1;

   // -----------------------------------------------------------------------
   // Calculate the path length difference between the direct and
   // reflected paths using equation 6.64, page 257 of reference 1.
   // -----------------------------------------------------------------------
   double sin_psi = sin(psi);
   double delta   = (4.0 * R_1 * R_2 * sin_psi * sin_psi) / (R_1 + R_2 + R_d);

   // Copy the computed values into the output arguments

   aAntToRefSlantRange   = R_1;
   aRefToTgtSlantRange   = R_2;
   aAntElevationAngle    = -theta_r;
   aGrazingAngle         = psi;
   aPathLengthDifference = delta;
   return true;
}

// =================================================================================================
//! Determines the relative dielectric constant of soil as a function of
//! the radar frequency and the moisture content by volume of the soil.
//!
//! The tabular data below and the notation used in the subroutine are contained on pages 14 and 15
//! of 'Radar Propagation at Low Altitudes,' by M. L. Meeks, Artech House, Inc.,
//! Dedham, Massachusetts, 1982.
//!
//! @param aFrequency          [input] Frequency in Hertz.
//! @param aMoistureFraction   [input] The moisture content by volume of the soil [0..1].
//! @param aDielectricConstant [output] The complex relative dielectric constant.
// static
void WsfEM_FastMultipath::GetSoilDielectricConstant(double                aFrequency,
                                                    double                aMoistureFraction,
                                                    std::complex<double>& aDielectricConstant)
{
   static const double frqghz[5] = {0.3, 3.0, 8.0, 14.0, 24.0};

   static const double cmoist[4] = {0.003, 0.100, 0.200, 0.300};

   static const double epsReal[5][4] = {
      {2.9, 6.0, 10.5, 16.7}, // 0.3 GHz
      {2.9, 6.0, 10.5, 16.7}, // 3.0 GHz
      {2.8, 5.8, 10.3, 15.3}, // 8.0 GHz
      {2.8, 5.6, 9.4, 12.6},  // 14.0 GHz
      {2.6, 4.9, 7.7, 9.6}    // 24.0 GHz
   };

   static const double epsImag[5][4] = {
      {0.071, 0.450, 0.750, 1.200}, // 0.3 GHz
      {0.027, 0.400, 1.100, 2.000}, // 3.0 GHz
      {0.032, 0.870, 2.250, 4.100}, // 8.0 GHz
      {0.035, 1.140, 3.700, 6.300}, // 14.0 GHz
      {0.030, 1.150, 4.800, 8.500}  // 24.0 GHz
   };

   // Determine the index of the left-hand endpoint of the interval containing the
   // frequency. Also determine the fractional distance of the frequency from the
   // left-hand endpoint of the interval.

   double ghzfrq = aFrequency * 1.0E-9;

   int    ifreql = 0;
   double frqfrc = 0.0;
   if (ghzfrq <= frqghz[0])
   {
      ifreql = 0;
      frqfrc = 0.0;
   }
   else if (ghzfrq >= frqghz[4])
   {
      ifreql = 3;
      frqfrc = 1.0;
   }
   else
   {
      for (ifreql = 0; ifreql < 4; ++ifreql)
      {
         if (ghzfrq < frqghz[ifreql + 1])
         {
            frqfrc = (ghzfrq - frqghz[ifreql]) / (frqghz[ifreql + 1] - frqghz[ifreql]);
            break;
         }
      }
   }

   // Determine the index of the left-hand endpoint of the interval containing the water
   // content. Also determine the fractional distance of the water content from the
   // left-hand endpoint of the interval.

   double h2ocon = aMoistureFraction;
   int    iwater = 0;
   double h2ofrc = 0.0;
   if (h2ocon <= cmoist[0])
   {
      iwater = 0;
      h2ofrc = 0.0;
   }
   else if (h2ocon >= cmoist[3])
   {
      iwater = 2;
      h2ofrc = 1.0;
   }
   else
   {
      for (iwater = 0; iwater < 3; ++iwater)
      {
         if (h2ocon < cmoist[iwater + 1])
         {
            h2ofrc = (h2ocon - cmoist[iwater]) / (cmoist[iwater + 1] - cmoist[iwater]);
            break;
         }
      }
   }

   // Perform two dimensional interpolation to determine the real part of the relative dielectric constant.

   double eps1l = ((1.0 - frqfrc) * epsReal[ifreql][iwater]) + (frqfrc * epsReal[ifreql + 1][iwater]);
   double eps1r = ((1.0 - frqfrc) * epsReal[ifreql][iwater + 1]) + (frqfrc * epsReal[ifreql + 1][iwater + 1]);
   double eps1  = ((1.0 - h2ofrc) * eps1l) + (h2ofrc * eps1r);

   // Perform two dimensional interpolation to determine the imaginary part of the relative dielectric constant.

   double eps2l = ((1.0 - frqfrc) * epsImag[ifreql][iwater]) + (frqfrc * epsImag[ifreql + 1][iwater]);
   double eps2r = ((1.0 - frqfrc) * epsImag[ifreql][iwater + 1]) + (frqfrc * epsImag[ifreql + 1][iwater + 1]);
   double eps2  = ((1.0 - h2ofrc) * eps2l) + (h2ofrc * eps2r);


   // Return the complex value.

   aDielectricConstant = std::complex<double>(eps1, eps2);
}
