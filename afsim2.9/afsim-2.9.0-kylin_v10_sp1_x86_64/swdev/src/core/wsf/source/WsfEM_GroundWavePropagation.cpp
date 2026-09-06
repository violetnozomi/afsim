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
// Ref 1. GRWAVE.FOR
//        ITU-R, Release 2, 18 Feb 1985
//        Copyright GEC PLC 1985

#include "WsfEM_GroundWavePropagation.hpp"

#include <algorithm>
#include <cmath>
#include <complex>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"

// =================================================================================================
WsfEM_GroundWavePropagation::WsfEM_GroundWavePropagation()
   : mRelativePermittivity(70.0)
   , mConductivity(5.0)
   , mTroposphereRefractivity(315.0)
   , mTroposphereHeightScale(7.35)
   , mDel(0.0)
   , mMinDistance(10.0)
   , mDistanceInterval(10.0)
   , mWavenumber(0.0)
   , mWavenumberSquared(0.0)
   , mWavenumberImaginary(0.0, 0.0)
   , mP(0.0)
   , mIntegrationPhase(-1)
   , mIntegrationStep(0.0)
   , mImpedanceCounter(1)
   , mReflectionCounter(1)
   , mReflectionNF(0)
   , mXmtrAlt(1.0)
   , mRcvrAlt(1.0)
{
   for (size_t i = 0; i < 9; ++i)
   {
      mImpedanceFunction[i]            = std::complex<double>(0.0, 0.0);
      mReflectionFunction[i]           = std::complex<double>(0.0, 0.0);
      mReflectionDerivativeFunction[i] = std::complex<double>(0.0, 0.0);
      mP0[i]                           = std::complex<double>(0.0, 0.0);
      mTurningPoint[i]                 = std::complex<double>(0.0, 0.0);
      mHeightChange[i]                 = 0.0;
   }

   for (size_t i = 0; i < 4; ++i)
   {
      mDp[i] = std::complex<double>(0.0, 0.0);
   }
}

// =================================================================================================
WsfEM_GroundWavePropagation::WsfEM_GroundWavePropagation(const WsfEM_GroundWavePropagation& aSrc)
   : mRelativePermittivity(aSrc.mRelativePermittivity)
   , mConductivity(aSrc.mConductivity)
   , mTroposphereRefractivity(aSrc.mTroposphereRefractivity)
   , mTroposphereHeightScale(aSrc.mTroposphereHeightScale)
   , mDel(aSrc.mDel)
   , mMinDistance(aSrc.mMinDistance)
   , mDistanceInterval(aSrc.mDistanceInterval)
   , mWavenumber(aSrc.mWavenumber)
   , mWavenumberSquared(aSrc.mWavenumberSquared)
   , mWavenumberImaginary(aSrc.mWavenumberImaginary)
   , mP(aSrc.mP)
   , mIntegrationPhase(aSrc.mIntegrationPhase)
   , mIntegrationStep(aSrc.mIntegrationStep)
   , mImpedanceCounter(aSrc.mImpedanceCounter)
   , mReflectionCounter(aSrc.mReflectionCounter)
   , mReflectionNF(aSrc.mReflectionNF)
   , mXmtrAlt(aSrc.mXmtrAlt)
   , mRcvrAlt(aSrc.mRcvrAlt)
{
   for (size_t i = 0; i < 9; ++i)
   {
      mImpedanceFunction[i]            = aSrc.mImpedanceFunction[i];
      mReflectionFunction[i]           = aSrc.mReflectionFunction[i];
      mReflectionDerivativeFunction[i] = aSrc.mReflectionDerivativeFunction[i];
      mTurningPoint[i]                 = aSrc.mTurningPoint[i];
      mP0[i]                           = aSrc.mP0[i];
      mHeightChange[i]                 = aSrc.mHeightChange[i];
   }

   for (size_t i = 0; i < 4; ++i)
   {
      mDp[i] = aSrc.mDp[i];
   }
}

// =================================================================================================
//! Factory method called by WsfEM_PropagationTypes.
// static
WsfEM_Propagation* WsfEM_GroundWavePropagation::ObjectFactory(const std::string& aTypeName)
{
   WsfEM_Propagation* instancePtr = nullptr;
   if ((aTypeName == "WSF_GROUND_WAVE_PROPAGATION") || (aTypeName == "ground_wave_propagation") ||
       (aTypeName == "groundwave_propagation"))
   {
      instancePtr = new WsfEM_GroundWavePropagation();
   }
   return instancePtr;
}

// =================================================================================================
// virtual
WsfEM_Propagation* WsfEM_GroundWavePropagation::Clone() const
{
   return new WsfEM_GroundWavePropagation(*this);
}

// =================================================================================================
// virtual
bool WsfEM_GroundWavePropagation::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "relative_permittivity")
   {
      aInput.ReadValue(mRelativePermittivity);
      aInput.ValueGreater(mRelativePermittivity, 0.0);
   }
   else if (command == "conductivity")
   {
      aInput.ReadValue(mConductivity);
      aInput.ValueGreater(mConductivity, 0.0);
   }
   else if (command == "troposphere_refractivity")
   {
      aInput.ReadValue(mTroposphereRefractivity);
      aInput.ValueGreater(mTroposphereRefractivity, 0.0);
   }
   else if (command == "troposphere_height_scale")
   {
      aInput.ReadValueOfType(mTroposphereHeightScale, UtInput::cLENGTH);
      aInput.ValueGreater(mTroposphereHeightScale, 0.0);
      mTroposphereHeightScale *= 0.001; // store value in km's
   }
   else if (command == "minimum_computation_distance")
   {
      aInput.ReadValueOfType(mMinDistance, UtInput::cLENGTH);
      aInput.ValueGreater(mMinDistance, 0.0);
      mMinDistance *= 0.001; // store value in km's
   }
   else if (command == "computation_distance_interval")
   {
      aInput.ReadValueOfType(mDistanceInterval, UtInput::cLENGTH);
      aInput.ValueGreater(mDistanceInterval, 0.0);
      mDistanceInterval *= 0.001;
   }
   else
   {
      myCommand = WsfEM_Propagation::ProcessInput(aInput);
   }
   return myCommand;
}


// =================================================================================================
//! computes the ground-wave propagation factor
double WsfEM_GroundWavePropagation::ComputePropagationFactor(WsfEM_Interaction& aInteraction, WsfEnvironment& aEnvironment)
{
   double frequency = aInteraction.GetTransmitter()->GetFrequency();
   if ((aInteraction.GetReceiver() != nullptr) &&
       (aInteraction.GetReceiver()->GetFunction() != WsfEM_Rcvr::cRF_PASSIVE_SENSOR))
   {
      // use receiver frequency in case xmtr is at another center frequency
      frequency = aInteraction.GetReceiver()->GetFrequency();
   }

   // setup the parameters for the exponential atmosphere
   double scale = 0.0;
   double d1p0  = 0.0;
   SetupExponentialAtmosphere(scale, d1p0);

   // compute wavenumber
   mWavenumber        = 0.02094395 * frequency * 1.0e-6; // GRWAVE uses freq in MHz
   mWavenumberSquared = mWavenumber * mWavenumber;
   std::complex<double> dummyComplex(0.0, mWavenumber);
   mWavenumberImaginary = dummyComplex;

   // compute square of the Earth's refractive index
   std::complex<double> nSquared(mRelativePermittivity, -1.8e4 * mConductivity / (frequency * 1.e-6));

   // compute surface impedance relative to free space
   std::complex<double> surfaceImpedance = std::sqrt(nSquared - 1.0);

   // for vertical polarization, the refractive index and
   // surface impedance need slight modification
   if (aInteraction.GetTransmitter()->GetPolarization() != WsfEM_Types::cPOL_HORIZONTAL)
   {
      surfaceImpedance /= nSquared;
      ModifyValuesForVerticalPol(scale, d1p0, surfaceImpedance, d1p0, surfaceImpedance);
   }

   double propagationFactorOutbound = 1.0;
   // compute one-way propagation factor from radar transmitter
   // to target
   mXmtrAlt = aInteraction.mXmtrLoc.mAlt;
   mRcvrAlt = aInteraction.mTgtLoc.mAlt;

   // compute using far-field effects
   bool farFieldValid = FarFieldTransmissionLoss(aInteraction, scale, d1p0, surfaceImpedance, propagationFactorOutbound);
   if (!farFieldValid)
      GeometricalOptics(aInteraction, scale, d1p0, surfaceImpedance, propagationFactorOutbound);

   // compute one-way propagation factor from target to radar
   // receiver
   double propagationFactorInbound = 1.0;
   mXmtrAlt                        = aInteraction.mTgtLoc.mAlt;
   mRcvrAlt                        = aInteraction.mRcvrLoc.mAlt;

   // compute using far-field effects
   farFieldValid = FarFieldTransmissionLoss(aInteraction, scale, d1p0, surfaceImpedance, propagationFactorInbound);
   if (!farFieldValid)
      GeometricalOptics(aInteraction, scale, d1p0, surfaceImpedance, propagationFactorInbound);

   double propagationFactor = propagationFactorOutbound * propagationFactorInbound;

   return propagationFactor;
}

// =================================================================================================
//! Computes related parameters based on the ITU-R exponential atmosphere
//! virtual protected
void WsfEM_GroundWavePropagation::SetupExponentialAtmosphere(double& aScale, double& aD1P0)
{
   mDel = 2.0e-6 * mTroposphereRefractivity;

   // sc = (Earth radius) / (2. * hScale)
   aScale = 3.185e3 / mTroposphereHeightScale;

   // K0 = effective Earth's radius factor = 1.3755
   double k0 = 1.0 / (1.0 + mDel * (1.0 - aScale));

   // D1P0 = 2 / (effective Earth's radius)
   aD1P0 = 3.14e-7 / k0;
}


// =================================================================================================
//! modifies values for vertical polarization
//! virtual protected
void WsfEM_GroundWavePropagation::ModifyValuesForVerticalPol(double                aScale,
                                                             double                aD1P0,
                                                             std::complex<double>& aSurfaceImpedance,
                                                             double&               aD1P0V,
                                                             std::complex<double>& aSurfaceImpedanceV)
{
   // the normalized surface impedance
   aSurfaceImpedanceV = aSurfaceImpedance + mDel * aScale / (6.37e6 * (1.0 + mDel) * mWavenumberImaginary);

   mDel *= (1.0 + aScale * aScale / (2.028345e13 * mWavenumberSquared));

   // effective Earth's radius factor
   double k0 = 1.0 / (1.0 + mDel * (1.0 - aScale));

   // d1p0 = 2 / (effective Earth's radius factor)
   aD1P0V = 3.14e-7 / k0;
}


// =================================================================================================
//! computes the far-field propagation factor (<= 1.0) using the Residue series
//! for elevated terminals
//! virtual protected
bool WsfEM_GroundWavePropagation::FarFieldTransmissionLoss(WsfEM_Interaction&    aInteraction,
                                                           double                aScale,
                                                           double                aD1P0,
                                                           std::complex<double>& aImpedance,
                                                           double&               aPropagationFactor)
{
   using std::complex;

   // compute propagation constant P0 and excitation factors
   // fid(m) m=0,8
   complex<double> fid[9];
   complex<double> p0Sub[9];
   complex<double> sm1[9];
   complex<double> exc[9];
   for (unsigned int i = 0; i < 9; ++i)
   {
      Eigen(this, i, aImpedance, aScale, aD1P0, fid);
      p0Sub[i] = mP0[i];
      sm1[i]   = mWavenumberImaginary * (mDel - mP0[i]) / (std::sqrt(1.0 + mDel - mP0[i]) + 1.0);
      exc[i]   = std::log(-2.0 * fid[i] * (1.0 + sm1[i] / mWavenumberImaginary));
   }

   for (unsigned int i = 0; i < 9; ++i)
   {
      mP0[i] = p0Sub[i];
   }

   // the reference dipole has a field of 150 milliVolts/m at 1 km
   // in free space
   double          a = 0.5 * log(8.877e10 * mWavenumber * mWavenumberSquared);
   complex<double> aitken[9];
   complex<double> complexZero(0.0, 0.0);
   aitken[0] = complexZero;
   aitken[1] = complexZero;

   // compute the height gain functions
   complex<double> heightGainRcvr[9];
   complex<double> heightGainXmtr[9];
   for (unsigned int i = 0; i < 9; ++i)
   {
      // the boundary condition at the surface (H=0) for the reciprocal
      // of the reflection coefficient, height-gain function and
      // the wave impedance are:
      complex<double> reflectionCoefficient = 1.0 - 2.0 * aImpedance / (aImpedance + std::sqrt(mP0[i]));

      double hSub = 0.0;
      if (mRcvrAlt <= mXmtrAlt)
      {
         Height(i, aScale, reflectionCoefficient, heightGainRcvr[i], hSub, mRcvrAlt);
         hSub = mRcvrAlt;
         if (hSub != mXmtrAlt)
         {
            Height(i, aScale, reflectionCoefficient, heightGainXmtr[i], hSub, mXmtrAlt);
         }
         else
         {
            heightGainXmtr[i] = heightGainRcvr[i];
         }
      }
      else
      {
         Height(i, aScale, reflectionCoefficient, heightGainXmtr[i], hSub, mXmtrAlt);
         hSub = mXmtrAlt;
         if (hSub != mRcvrAlt)
         {
            Height(i, aScale, reflectionCoefficient, heightGainRcvr[i], hSub, mRcvrAlt);
         }
         else
         {
            heightGainRcvr[i] = heightGainXmtr[i];
         }
      }
   }

   // compute the reside series beginning at the target range and
   // moving inwards to dMin or until the convergence fails
   double zeroLossFieldStrength = UtMath::DB_ToLinear(BasicTransmissionLoss(aInteraction));

   int na = 100;

   double range = ComputeGroundDistance(aInteraction) * 0.001;

   complex<double> term[9];
   complex<double> series[9];
   bool            computeField = false;
   bool            useAitken    = false;
   unsigned int    index        = 0;
   for (unsigned int i = 0; i < 9; ++i)
   {
      index                     = i;
      complex<double> termLarge = -1.e3 * range * sm1[i] + heightGainRcvr[i] + heightGainXmtr[i] - exc[i] + a;

      // if the term of the residue series is too large, move on
      if (std::real(termLarge) >= 50.0e1)
      {
         if (na == 100)
         {
            return false;
         }

         ++na;
         break;
      }
      else if (std::real(termLarge) <= -50.0e1)
      {
         // if the term of the residue series is too small, move on
         break;
      }
      else
      {
         // compute the terms and partial sums of the residue series
         term[i] = std::exp(termLarge);
         if (i == 0)
         {
            series[i] = term[i];
         }
         else
         {
            series[i] = series[i - 1] + term[i];
         }

         // test for convergence
         double eps = std::norm(term[i] / series[i]);
         if (eps < 1.e-5)
         {
            computeField = true;
            break;
         }

         // accelerate the convergence using Aitken extrapolation
         if (i > 0)
         {
            aitken[i] = series[i] - term[i] * term[i] / (term[i] - term[i - 1]);

            // test for convergence
            eps = std::norm(1.0 - aitken[i - 1] / aitken[i]);
            if (eps < 1.e-5)
            {
               computeField = true;
               useAitken    = true;
               break;
            }
         }
      }
   }

   // compute field strength in dB above 1 microvolt/m
   if (computeField)
   {
      complex<double> bc;
      if (useAitken)
      {
         bc = aitken[index];
      }
      else
      {
         bc = series[index];
      }

      double fieldStrength = std::norm(bc) / sin(range * 1.57e-4);
      aPropagationFactor   = fieldStrength / zeroLossFieldStrength;
      return true;
   }

   return false;
}

// =================================================================================================
//! computes propagation factor using geometrical optics
//! virtual protected
void WsfEM_GroundWavePropagation::GeometricalOptics(WsfEM_Interaction&    aInteraction,
                                                    double                aScale,
                                                    double                aD1P0,
                                                    std::complex<double>& aImpedance,
                                                    double&               aPropagationFactor)
{
   using std::complex;

   // set loss to default value
   aPropagationFactor = 1.0;

   double range = ComputeGroundDistance(aInteraction) * 0.001;

   double          hn     = 120.0 * pow(mWavenumber, -2.0 / 3.0);
   complex<double> factor = 2.0e-3 * (1.0 + 0.5 * mDel) / mWavenumberImaginary;

   double               zeroLossFieldStrength = UtMath::DB_ToLinear(BasicTransmissionLoss(aInteraction));
   std::complex<double> heightLowC(std::min(mRcvrAlt, mXmtrAlt), 0.0);
   std::complex<double> heightHighC(std::max(mRcvrAlt, mXmtrAlt), 0.0);

   if ((mRcvrAlt > hn) || (mXmtrAlt > hn))
   {
      double heightMax   = std::real(heightHighC);
      double distanceMin = 1.2 * sqrt(8.0 * 4.0 / 3.0 * 6.37e6 * 1.0 - 3 * heightMax) * 1.0e-3;
      mMinDistance       = std::max(distanceMin, mMinDistance);

      // compute dc, distance at which the direct ray from height hhc turns at
      // height hlc (distance in kilometers)
      int             m = 0;
      complex<double> complexZero(0.0, 0.0);
      mP0[0] = complexZero;
      PFunctions(0, m, aScale, heightLowC);
      mP0[0]           = -mP;
      mTurningPoint[0] = heightLowC;
      double dc        = std::real(factor * PhaseIntegral(2, heightHighC, m, aScale));

      // compute dh and dl, distances at which the direct rays from height
      // hhc and hlc turn at the surface.  their sum, dz, is the
      // horizon distance
      mP0[0]           = complexZero;
      mTurningPoint[0] = complexZero;
      double dh        = std::real(factor * PhaseIntegral(2, heightHighC, m, aScale));
      double dl        = std::real(factor * PhaseIntegral(2, heightLowC, m, aScale));
      double dz        = dh + dl;

      // compute the first range d
      int    n = 0;
      double d = mMinDistance;
      if (d == ComputeGroundDistance(aInteraction) * 0.001)
      {
         return;
      }

      int             ifin = 0;
      complex<double> g[9];
      complex<double> f[9];
      complex<double> fdd[9];
      while (ifin == 0)
      {
         // if d is greater than the horizon distance dz or 100 evaluations
         // have been made, finish
         if (((d >= dz) && (range >= dz)) || (n > 99))
         {
            break;
         }

         // if d is greater than the maximum range or 0.75 * dz, compute
         // at this range and then finish
         if ((d < range) && (range < dz))
         {
            // nothing
         }
         else if ((d >= range) && (range < dz))
         {
            d    = range;
            ifin = 1;
         }
         else if ((d < range) || (d < 0.75 * dz))
         {
            // nothing
         }
         else
         {
            d = std::max(range, 0.75 * dz);
         }

         // compute the direct wave
         int m1 = 0;
         if (d > dc)
         {
            m1 = 1;
         }
         DirectRay(aScale, aD1P0, m1, heightLowC, heightHighC, d, dc, dh, dl, f, fdd, g);
         complex<double> edexp(0.0, (2.0 - (m1 + 1) * 1.570796));
         edexp += 0.5 * log(5.625e12 * mWavenumber * g[m1] * g[m1] / (d * fdd[m1])) + f[m1];

         // compute the reflected wave
         m1 = 2;
         DirectRay(aScale, aD1P0, m1, heightLowC, heightHighC, d, dc, dh, dl, f, fdd, g);
         complex<double> factor1 = sqrt(mP0[2]);
         complex<double> r       = log(g[2] * (aImpedance - factor1) / (aImpedance + factor));
         complex<double> erexp(0.0, 4.712389);
         erexp += 0.5 * log(5.625e12 * mWavenumber / (d * fdd[2])) + f[2] + r;

         // compute the field strength e, free space field efs, direct wave
         // ed, reflected wave er, envelopes of the interference pattern emax
         // and emin all in dB relative to 1 microVolt / meter
         // compute the phase difference between the reflected and direct waves
         // in units of 2Pi
         // the reference dipole has a field strength of 150 microVolts/meters
         // at a distance of 1 km in free space
         double ed          = 8.686 * std::real(edexp);
         double e           = ed + UtMath::LinearToDB(std::norm(1.0 + exp(erexp - edexp)));
         aPropagationFactor = UtMath::DB_ToLinear(e) / zeroLossFieldStrength;

         // compute the next range d
         ++n;
         d += mDistanceInterval;
      }
   }
   else
   {
      aPropagationFactor = FlatEarthPropagation(aInteraction, aScale, aImpedance, heightLowC, heightHighC);
   }
}

// =================================================================================================
//! compute propagation constant P0 and excitation factor Fid for the
//! m-th mode
//! virtual protected
void WsfEM_GroundWavePropagation::Eigen(WsfEM_GroundWavePropagation* aPtr,
                                        int                          aMode,
                                        std::complex<double>&        aImpedance,
                                        double                       aScale,
                                        double                       aD1P0,
                                        std::complex<double>         aFid[9])
{
   using std::complex;

   complex<double> complexZero(0.0, 0.0);

   // compute an approximate value for P0[mode]
   StartPropagation(aPtr, aMode, aImpedance, aScale, aD1P0);

   // compute the turning point
   complex<double> r0[2];
   complex<double> dp0;
   complex<double> ddp0;
   complex<double> r03;
   for (unsigned int iter = 0; iter < 9; ++iter)
   {
      TurningPoint(aScale, aMode);

      // compute the height at which the numerical integration of
      // the differential equations is to begin
      //
      // the following block is required because the concept of effective
      // Earth's radius breaks down at low frequencies
      complex<double> ak23;
      complex<double> dummyC(0.5, -sqrt(3.0) * 0.5);
      if (aD1P0 > 0.0)
      {
         ak23 = pow((aD1P0 * mWavenumberSquared), -1.0 / 3.0) * dummyC;
      }
      else
      {
         ak23 = pow((3.14e-7 * mWavenumberSquared), -1.0 / 3.0) * dummyC;
      }

      complex<double> ha = mTurningPoint[aMode] + 5.0 * ak23;

      // compute the wave impedance Fia(0) and its derivative WRT P0,
      // Fia(1) at the height ha using WKB approximations
      PFunctions(1, aMode, aScale, ha);
      complex<double> factor  = -mWavenumberImaginary * std::sqrt(mP);
      complex<double> factor1 = -0.25 * mDp[0] / mP;
      complex<double> fia[2];
      fia[0] = factor + factor1;
      fia[1] = (0.5 * factor - factor1) / mP;

      // compute the height hb at which the integration method changes
      complex<double> hb = mTurningPoint[aMode] - 0.5 * ak23;

      // integrate the differential equations from ha to hb
      complex<double> fib[2];
      int             mstep = 0;
      Integrate(this, Hfi, fia, fib, ha, hb, 1.0e-11, 0.1, 2, aMode, aScale, mstep);

      // compute the reflection coefficient rb[0] and its derivative
      // rb[1] at hb
      PFunctions(1, aMode, aScale, hb);
      factor  = mWavenumberImaginary * std::sqrt(mP);
      factor1 = factor + fib[0];
      complex<double> rb[2];
      rb[0] = (factor - fib[0]) / factor1;
      rb[1] = factor * (fib[0] / mP - 2.0 * fib[1]) / (factor1 * factor1);

      // integrate from hb to 0
      Integrate(this, Hr, rb, r0, hb, complexZero, 1.0e-11, 0.25 / (aMode + 1), 2, aMode, aScale, mstep);

      // compute a new value of P0[mode] using Newton's method
      factor             = std::sqrt(mP0[aMode]);
      factor1            = factor + aImpedance;
      complex<double> f  = r0[0] - (factor - aImpedance) / factor1;
      complex<double> df = r0[1] - aImpedance / (factor * factor1 * factor1);
      r03                = -0.375 * (1.0 - r0[0] * r0[0]) / (mP0[aMode] * mP0[aMode]) +
            r0[1] / r0[0] * (r0[1] - 0.25 * (1.0 - r0[0]) * (1.0 - r0[0]) / mP0[aMode]);

      factor1 *= factor;
      complex<double> ddf = r03 + aImpedance * (aImpedance + 3.0 * factor) / (2.0 * factor1 * factor1 * factor1);
      dp0                 = -f / df;
      ddp0                = -0.5 * dp0 * dp0 * ddf / df;
      mP0[aMode] += dp0 + ddp0;
      double err = std::norm(ddp0 / mP0[aMode]);
      if (err < 1.0e-9)
      {
         break;
      }
   }

   PFunctions(1, aMode, aScale, complexZero);
   complex<double> factor = std::sqrt(mP0[aMode]);
   r0[0]                  = (factor - aImpedance) / (factor + aImpedance);
   r0[1] += (dp0 + ddp0) * r03;

   aFid[aMode] =
      mWavenumberImaginary / (1.0 + r0[0]) * (0.5 * (1.0 - r0[0]) / factor - 2.0 * factor * r0[1] / (1.0 + r0[0]));
}

// =================================================================================================
//! compute basic transmission loss constant. assume perfectly-conducting ground
//! near the aerials
//! virtual protected
double WsfEM_GroundWavePropagation::BasicTransmissionLoss(WsfEM_Interaction& aInteraction)
{
   double rr = 1.0;
   double x  = 2.0 * mWavenumber * mXmtrAlt;

   if (aInteraction.GetTransmitter()->GetPolarization() == WsfEM_Types::cPOL_HORIZONTAL)
   {
      for (unsigned int i = 0; i < 2; ++i)
      {
         if (x < 0.1)
         {
            double xSquared = x * x;
            rr *= (xSquared * (0.2 - xSquared * (1.071429e-3 + xSquared * 2.645503e-4)));
         }
         else
         {
            rr *= (1.0 + 1.5 * ((1.0 - x * x) * sin(x) - x * cos(x)) / x / x / x);
         }

         x = 2.0 * mWavenumber * mRcvrAlt;
      }
   }
   else
   {
      for (unsigned int i = 0; i < 2; ++i)
      {
         if (x < 0.1)
         {
            double xSquared = x * x;
            rr *= (2.0 - xSquared * (0.1 + xSquared * 3.571429e-3));
         }
         else
         {
            rr *= (1.0 + 3.0 * (sin(x) - x * cos(x)) / x / x / x);
         }

         x = 2.0 * mWavenumber * mRcvrAlt;
      }
   }

   double y    = mWavenumberSquared * rr;
   double btlc = 0.0;
   if (y < 1.e-70)
   {
      btlc = 1000.0;
   }
   else
   {
      btlc = 169.542 + UtMath::LinearToDB(y);
   }
   return btlc;
}

// =================================================================================================
//! compute ground distance from transmitter to target
//! virtual protected
double WsfEM_GroundWavePropagation::ComputeGroundDistance(WsfEM_Interaction& aInteraction)
{
   double xmtrLat = aInteraction.mXmtrLoc.mLat;
   double xmtrLon = aInteraction.mXmtrLoc.mLon;

   double tgtLat = aInteraction.mTgtLoc.mLat;
   double tgtLon = aInteraction.mTgtLoc.mLon;

   double groundRange = 0.0;
   double dummyAngle  = 0.0;

   UtSphericalEarth::GreatCircleHeadingAndDistance(xmtrLat, xmtrLon, tgtLat, tgtLon, dummyAngle, groundRange);

   return groundRange;
}

// =================================================================================================
//! dummy value required to maintain commonality with other functions
//! being integrated in Runge-Kutta
//! static
std::complex<double> WsfEM_GroundWavePropagation::Zxi(WsfEM_GroundWavePropagation* aPtr,
                                                      int                          dummy,
                                                      int                          dummy1,
                                                      double                       dummy2,
                                                      std::complex<double>&        aX,
                                                      std::complex<double>*        aZ)
{
   return 1.0 / (aZ[0] - aX * aX);
}

// =================================================================================================
//! dummy value required to maintain commonality with other functions
//! being integrated in Runge-Kutta
//! static
std::complex<double> WsfEM_GroundWavePropagation::Zpsi(WsfEM_GroundWavePropagation* aPtr,
                                                       int                          dummy,
                                                       int                          dummy1,
                                                       double                       dummy2,
                                                       std::complex<double>&        aY,
                                                       std::complex<double>*        aZ)
{
   return 1.0 / (1.0 - aZ[0] * aY * aY);
}

// =================================================================================================
//! computes an initial value for the propagation constant P0(m)
//! derived from a uniform approximation based on airy integral
//! functions (Langers method or the extended WKB approximation)
//! virtual protected
void WsfEM_GroundWavePropagation::StartPropagation(WsfEM_GroundWavePropagation* aPtr,
                                                   int                          aMode,
                                                   std::complex<double>&        aImpedance,
                                                   double                       aScale,
                                                   double                       aD1P0)
{
   using std::complex;

   // define static data
   // as are the first 10 roots of ai(z) = 0
   static const double cAS[10] =
      {-2.338107, -4.087949, -5.520560, -6.786708, -7.944134, -9.022651, -10.04017, -11.00852, -11.93602, -12.82878};
   // asd are the first 10 roots of dai(z) / dz = 0
   static const double cASD[10] =
      {-1.018793, -3.248198, -4.820099, -6.163307, -7.372177, -8.488487, -9.535449, -10.52766, -11.47506, -12.38479};

   // compute an initial value for xi
   double factor1;
   if (aD1P0 > 0.0)
   {
      factor1 = pow(mWavenumberSquared * aD1P0, 1.0 / 3.0);
   }
   else
   {
      factor1 = pow(mWavenumberSquared * 3.14e-7, 1.0 / 3.0);
   }

   complex<double> tempComplex(0.8660254, 0.5);
   complex<double> factor = mWavenumber / factor1 * tempComplex;

   complex<double> xi = factor * aImpedance;

   // determine whether xi is small or large
   double atau = 4.724 * ((aMode + 1) - 0.5);
   atau *= atau;
   double a     = std::norm(xi);
   bool   large = false;
   if (a * a * a > atau)
   {
      large = true;
   }

   // initialize z0t, xit, psit
   complex<double> z0t(0.0, 0.0);
   if (large)
   {
      z0t = cAS[aMode];
   }
   else
   {
      z0t = cASD[aMode];
   }

   complex<double> xit(0.0, 0.0);
   complex<double> psit(0.0, 0.0);

   // iteration cycle for z0
   complex<double> z0(0.0, 0.0);
   complex<double> p0t;
   for (unsigned int iteration = 0; iteration < 9; ++iteration)
   {
      if (large)
      {
         // this is integrated from z=z0t at y=psit to z=z0 at y=psi
         // for the first integration z0t=AS(m) at psit=0 where
         // AS(m) is the m th root of AI(z)
         int step = 0;
         Integrate(aPtr, Zpsi, &z0t, &z0, psit, 1.0 / xi, 1.e-10, 0.5, 1, aMode, aScale, step);
      }
      else
      {
         // this is integrated from z=z0t at x-xit to z=z0 at x=xi
         // for the first integration z0t=ASD(m) at xit=0 where
         // ASD(m) is the m th root of DAI(z) / dz
         int step = 0;
         Integrate(aPtr, Zxi, &z0t, &z0, xit, xi, 1.e-10, 0.5, 1, aMode, aScale, step);
      }

      // z0 is related to the phase integral which depends
      // upon P0, the propagation constant. P0 is found by Newton's method
      complex<double> dummyComplex(0.0, 2.0 / 3.0);
      complex<double> phint = dummyComplex * z0 * sqrt(-z0);

      // initialize P0(m).
      if (iteration == 0)
      {
         mP0[aMode] = z0 / (factor * factor);
         p0t        = mP0[aMode];
      }

      for (unsigned int iter = 0; iter < 9; ++iter)
      {
         TurningPoint(aScale, aMode);

         // compute P0(m) by Newton's method
         // compute the phase integral and its derivative
         // by 16-point gaussian integration
         complex<double> cZero(0.0, 0.0);
         complex<double> term = (phint - PhaseIntegral(1, cZero, aMode, aScale)) / PhaseIntegral(2, cZero, aMode, aScale);

         mP0[aMode] += term;

         if (std::norm(term / mP0[aMode]) < 1.e-8)
         {
            break;
         }
      }

      if (std::norm(1.0 - p0t / mP0[aMode]) < 1.e-8)
      {
         break;
      }

      // compute xi
      xit  = xi;
      psit = 1.0 / xit;
      z0t  = z0;
      p0t  = mP0[aMode];
      xi   = sqrt(z0 / mP0[aMode]) * aImpedance;
   }

   // compute the height HeightChange(m) at which the method of computing the
   // height-gain function changes
   double z02     = std::norm(z0);
   double factor3 = 73.0 - 0.75 * z02;
   if (factor3 > 0.0)
   {
      mHeightChange[aMode] = (0.5 * sqrt(z02) + sqrt(factor3)) / factor1;
      if (mHeightChange[aMode] > (2.0 * std::real(mTurningPoint[aMode])))
      {
         return;
      }
   }
   mHeightChange[aMode] = 2.0 * std::real(mTurningPoint[aMode]);
}

// =================================================================================================
//! computes the phase integral (code=1), its first (code=2) or second (code=3)
//! derivative WRT mP0[m] from TurningPoint[m] to H
//! virtual protected
std::complex<double> WsfEM_GroundWavePropagation::PhaseIntegral(int aCode, std::complex<double>& aHeight, int aMode, double aScale)
{
   using std::complex;

   // define static data
   // y1, w1, y2 & w2 are the Gaussian abcissas and the weights
   static const double cY1[16] = {0.005143979,
                                  0.02690510,
                                  0.06525346,
                                  0.1188529,
                                  0.1858245,
                                  0.2638194,
                                  0.3501023,
                                  0.4416466,
                                  0.5352415,
                                  0.6276043,
                                  0.7154952,
                                  0.7958316,
                                  0.8657955,
                                  0.9229332,
                                  0.9652404,
                                  0.9912332};
   static const double cW1[16] = {0.01314449,
                                  0.02981894,
                                  0.04470995,
                                  0.05692274,
                                  0.06578789,
                                  0.07089086,
                                  0.07210076,
                                  0.06957807,
                                  0.06375982,
                                  0.05532300,
                                  0.04512950,
                                  0.03415651,
                                  0.02341822,
                                  0.01388466,
                                  0.006404388,
                                  0.00163687};
   static const double cY2[16] = {0.00546479,
                                  0.02856995,
                                  0.06923379,
                                  0.1259506,
                                  0.1966084,
                                  0.2785747,
                                  0.3687955,
                                  0.4639093,
                                  0.5603723,
                                  0.6545902,
                                  0.7430525,
                                  0.8224631,
                                  0.8898632,
                                  0.9427416,
                                  0.9791279,
                                  0.9976664};
   static const double cW2[16] = {0.01403722,
                                  0.03254879,
                                  0.05078413,
                                  0.06854773,
                                  0.08567180,
                                  0.1019961,
                                  0.1173682,
                                  0.1316444,
                                  0.1446916,
                                  0.1563878,
                                  0.1666238,
                                  0.1753042,
                                  0.1823478,
                                  0.1876888,
                                  0.1912774,
                                  0.1930802};

   complex<double> f = aHeight - mTurningPoint[aMode];
   complex<double> phaseIntegral(0.0, 0.0);
   double          f1 = std::norm(f);
   double          f2 = std::norm(mTurningPoint[aMode]) * 1.0e-6;

   if (f1 <= f2)
   {
      return phaseIntegral;
   }

   for (unsigned int i = 0; i < 16; ++i)
   {
      if (aCode == 1)
      {
         complex<double> y = aHeight - f * cY1[i];
         PFunctions(0, aMode, aScale, y);
         complex<double> cOne(1.0, 1.0);
         complex<double> temp(0.0, -0.5);
         phaseIntegral += cW1[i] * cOne * sqrt(temp * mP / (1.0 - cY1[i]));
      }
      else if (aCode == 2)
      {
         PFunctions(0, aMode, aScale, aHeight - f * cY2[i]);
         complex<double> cOne(1.0, -1.0);
         complex<double> temp(0.0, -2.0);
         phaseIntegral += cW2[i] * cOne * sqrt((1.0 - cY2[i]) / (temp * mP));
      }
      else
      {
         PFunctions(0, aMode, aScale, aHeight - f * cY2[i]);
         complex<double> cOne(1.0, -1.0);
         complex<double> temp(0.0, -2.0);
         phaseIntegral += cW2[i] * mDp[1] / (mDp[0] * mDp[0]) * cOne * sqrt((1.0 - cY2[i]) / (temp * mP));
      }
   }

   if (aCode == 1)
   {
      phaseIntegral *= mWavenumberImaginary * f;
   }
   else if (aCode == 2)
   {
      phaseIntegral *= 0.5 * mWavenumberImaginary * f;
   }
   else
   {
      complex<double> cOne(1.0, -1.0);
      complex<double> temp(0.0, -2.0);
      phaseIntegral = 0.5 * mWavenumberImaginary * (cOne / (mDp[0] * sqrt(temp * mP)) + f * phaseIntegral);
   }

   return phaseIntegral;
}

// =================================================================================================
//! computes the function P and its first N derivatives Dp(m)
//! (m=1,n) at complex height H.  if n=0 only P is computed
//! virtual protected
void WsfEM_GroundWavePropagation::PFunctions(int aN, int aMode, double aScale, const std::complex<double>& aHeight)
{
   std::complex<double> cZero(0.0, 0.0);
   std::complex<double> z[2] = {cZero, cZero};
   std::complex<double> y[2] = {cZero, cZero};

   for (unsigned int j = 0; j < 2; ++j)
   {
      std::complex<double> x(0.0, 0.0);
      if (j == 0)
      {
         x = 1.57e-7 * aHeight;
      }
      else
      {
         x = -2.0 * aScale * y[0];
      }

      double x2 = std::norm(x);
      if (x2 > 0.25)
      {
         if (std::real(x) > -50.0)
         {
            z[j] = std::exp(x);
         }
         y[j] = z[j] - 1.0;
      }
      else
      {
         std::complex<double> temp(1.0, 0.0);
         y[j] = temp;
         if (x2 > 0.0001)
            y[j] = 1.0 + x * 0.2 * (1.0 + x / 6.0 * (1.0 + x / 7.0 * (1.0 + x / 8.0)));
         y[j] = x * (1.0 + x * 0.5 * (1.0 + x / 3.0 * (1.0 + x * y[j] * 0.25)));
         z[j] = 1.0 + y[j];
      }
   }

   std::complex<double> delu = mDel * z[1];
   mP                        = y[0] * (2.0 + y[0]) * (1.0 + delu) + mDel * y[1] + mP0[aMode];

   if (aN == 0)
   {
      return;
   }

   std::complex<double> t  = aScale * z[0];
   std::complex<double> z2 = z[0] * z[0];

   // first derivative
   mDp[0] = 3.14e-7 * z2 * (1.0 + delu * (1.0 - t));
   if (aN == 1)
   {
      return;
   }

   // second derivative
   mDp[1] = 9.8596e-14 * z2 * (1.0 + delu * (1.0 - t * (2.5 - t)));
   if (aN == 2)
   {
      return;
   }

   // third derivative
   mDp[2] = 3.09594e-20 * z2 * (1.0 + delu * (1.0 - t * (4.75 - t * (4.5 - t))));
   if (aN == 3)
   {
      return;
   }

   // fourth derivative
   mDp[3] = 9.71171e-27 * z2 * (1.0 + delu * (1.0 - t * (8.125 - t * (13.75 - t * (7.0 - t)))));
}

// =================================================================================================
//! computes the turning point H1[m] of the differential equation
//! this is the solution of P(h) = 0
//! virtual protected
void WsfEM_GroundWavePropagation::TurningPoint(double aScale, int aMode)
{
   if (std::real(mP0[aMode]) >= 1.e-3)
   {
      double c = mDel * aScale;
      if (std::real(mP0[aMode]) >= 1.e-2)
      {
         double yr  = (mDel - 0.5 * std::real(mP0[aMode])) / (1.0 + c);
         double yrt = yr;
         double b   = 0.0;
         for (unsigned int i = 0; i < 9; ++i)
         {
            double a = 2.0 * yr - mDel + std::real(mP0[aMode]);
            c        = 2.0 * aScale * yr;
            double d = 0.0;
            if (c > -20.0)
            {
               d = exp(c);
            }
            b  = (UtMath::cPI + aScale * std::imag(mP0[aMode])) / (aScale + d / mDel);
            yr = -0.25 / aScale * log((a * a + b * b) / (mDel * mDel));
            if (fabs(1.0 - yrt / yr) < 1.e-3)
            {
               break;
            }
            yrt = yr;
         }

         double               yi = 0.5 * (b - std::imag(mP0[aMode]));
         std::complex<double> temp(yr, -yi);
         mTurningPoint[aMode] = 6.37e6 * temp;
      }
      else
      {
         double               b = log(c);
         double               a = 3.185e6 / aScale;
         std::complex<double> cTemp1(1.0, -1.0);
         std::complex<double> cTemp2(0.0, 1.0);
         mTurningPoint[aMode] = a * (b + cTemp1 * sqrt(cTemp2 * (-1.0 - b + c - aScale * mP0[aMode])));
      }
   }
   else
   {
      mTurningPoint[aMode] = -3.185e6 * mP0[aMode];
   }

   for (unsigned int i = 0; i < 9; ++i)
   {
      PFunctions(1, aMode, aScale, mTurningPoint[aMode]);
      std::complex<double> term = mP / mDp[0];
      mTurningPoint[aMode] -= term;
      double eps = std::norm(term / mTurningPoint[aMode]);
      if (eps < 1.e-11)
      {
         break;
      }
   }
}

// =================================================================================================
//! integrates the n first order differential equations
//! dy/dx = f(j,x,y) where j=1,n and y is an n element
//! array.  The 4th order Runge-Kutta method is used
//! The initial values are y=yi and x=xi and the final values
//! are y=yf and x=xy
//! static
void WsfEM_GroundWavePropagation::Integrate(
   WsfEM_GroundWavePropagation* aPtr,
   std::complex<double> (*aF)(WsfEM_GroundWavePropagation*, int, int, double, std::complex<double>&, std::complex<double>*),
   std::complex<double>* aYi,
   std::complex<double>* aYf,
   std::complex<double>  aXi,
   std::complex<double>  aXf,
   double                aError2,
   double                aStepIn,
   int                   aN,
   int                   aMode,
   double                aScale,
   int                   aMStep)
{
   aPtr->mIntegrationPhase = -1;
   aMStep                  = 0;
   double step             = std::min(0.5, aStepIn);

   // select smallest value from list
   bool finished = false;
   if (step >= 0.5)
   {
      finished = true;
   }
   std::complex<double> dx  = aXf - aXi;
   aPtr->mIntegrationStep   = step * dx;
   double               tau = 0.0;
   std::complex<double> x1  = aXi;

   std::complex<double>* y1 = new std::complex<double>[aN];
   for (int j = 0; j < aN; ++j)
   {
      // this puts the initial values Yi(n) into Y1(n)
      y1[j] = aYi[j];
   }

   //! loop until integration completed
   bool                 doubleLength = true;
   std::complex<double> y3s[4];
   while (true)
   {
      // perform a double length step from x1 to x1+2*h and two single
      // length steps from x1 to x1+h and x1+h to x1+2*h
      if (doubleLength)
         IntegrationStep(aPtr, aF, y1, y3s, x1, 2.0 * aPtr->mIntegrationStep, aMode, aScale, aN);

      std::complex<double> y2[4];

      IntegrationStep(aPtr, aF, y1, y2, x1, aPtr->mIntegrationStep, aMode, aScale, aN);

      IntegrationStep(aPtr, aF, y2, aYf, x1 + aPtr->mIntegrationStep, aPtr->mIntegrationStep, aMode, aScale, aN);

      // estimate the truncation error tError and use this to improve
      // Yf.  Compute the average of the modulus squared of the relative
      // truncation error err2.  Compare err2 with aError2
      ++aMStep;
      if (aMStep < 1000)
      {
         double err2 = 0.0;
         for (int j = 0; j < aN; ++j)
         {
            // see page 292 of Hildebrand's 'Numerical Analysis'
            std::complex<double> tError = (aYf[j] - y3s[j]) / 15.0;

            aYf[j] += tError;

            // equation 6.14.15 of Hildebrand
            std::complex<double> ec = y1[j] + y2[j] + aYf[j];

            double e = fabs(std::real(ec)) + fabs(std::imag(ec));
            if (e >= 1.e-10)
            {
               err2 += std::norm(tError / (y1[j] + y2[j] + aYf[j]));
            }
         }

         err2 *= 9.0 / aN;
         if ((err2 > 5.0 * aError2) && (step > 0.0002))
         {
            // truncation error too big.  halve step size
            doubleLength            = false;
            finished                = false;
            aPtr->mIntegrationPhase = 1;
            step                    = std::max(0.0002, 0.5 * step);
            aPtr->mIntegrationStep  = step * dx;
            for (int k = 0; k < aN; ++k)
            {
               y3s[k] = y2[k];
            }
         }
         else
         {
            if (finished)
            {
               return;
            }

            // prepare values and step size for next step
            aPtr->mIntegrationPhase = 0;
            for (int k = 0; k < aN; ++k)
            {
               y1[k] = aYf[k];
            }
            x1 += 2.0 * aPtr->mIntegrationStep;
            tau += 2.0 * step;
            if (err2 == 0.0)
            {
               err2 = 1.0e-14;
            }
            step = std::max(0.0002, step * pow((aError2 / err2), 0.1));
            if ((tau + 2.0 * step) >= 1.0)
            {
               finished = true;
            }
            if (finished)
            {
               step = 0.5 * (1.0 - tau);
            }
            aPtr->mIntegrationStep = step * dx;
            doubleLength           = true;
         }
      }
      else
      {
         ut::log::info() << "1000 steps used in Integrate.";
      }
   }

   delete[] y1;
}

// =================================================================================================
//! performs a single step of the 4th order Runge-Kutta method on
//! the n first order differential equations dy/dx = f(j,x,y)
//! where j=1,n and y is an n element array
//! static
void WsfEM_GroundWavePropagation::IntegrationStep(
   WsfEM_GroundWavePropagation* aPtr,
   std::complex<double> (*aF)(WsfEM_GroundWavePropagation*, int, int, double, std::complex<double>&, std::complex<double>*),
   std::complex<double>* aY1,
   std::complex<double>* aY2,
   std::complex<double>  aX1,
   std::complex<double>  aH,
   int                   aMode,
   double                aScale,
   int                   aN)
{
   static const double a[4] = {0.0, 0.5, 0.5, 1.0};
   static const double b[4] = {0.1666667, 0.333333, 0.333333, 0.16666667};

   const int n = std::min(aN, 4); //< GCC optimizer falsely flags out-of-bounds array access when using aN directly.
   const std::complex<double> complexZero(0.0, 0.0);
   std::complex<double>       ak[4] = {complexZero, complexZero, complexZero, complexZero};

   // initialize Y2 array
   for (int i = 0; i < n; ++i)
   {
      aY2[i] = aY1[i];
   }

   for (unsigned int i = 0; i < 4; ++i)
   {
      std::complex<double> x = aX1 + (a[i] * aH);

      std::complex<double> y[4];
      for (int j = 0; j < n; ++j)
      {
         y[j] = aY1[j] + a[i] * ak[j];
      }

      for (int j = 0; j < n; ++j)
      {
         ak[j] = aH * (*aF)(aPtr, j, aMode, aScale, x, y);
         aY2[j] += b[i] * ak[j];
      }
   }
}

// =================================================================================================
//! Height-gain function or logarithm of the wave function
//! virtual protected
void WsfEM_GroundWavePropagation::Height(int                   aMode,
                                         double                aScale,
                                         std::complex<double>& aReflection,
                                         std::complex<double>& aHeightGainFunction,
                                         double                aHeightA,
                                         double                aHeightB)
{
   std::complex<double> hac(aHeightA, 0.0);
   std::complex<double> hbc(aHeightB, 0.0);
   double               hc = std::real(mTurningPoint[aMode]);
   std::complex<double> hcc(hc, 0.0);
   std::complex<double> hchc(mHeightChange[aMode], 0.0);

   std::complex<double> fii[2] = {aReflection, aHeightGainFunction};
   std::complex<double> fif[2];
   fif[0]                    = aReflection;
   std::complex<double> htgi = aHeightGainFunction;

   int mstep = 0;
   if (aHeightA < hc)
   {
      if (aHeightB > hc)
      {
         Integrate(this, HrTau, fii, fif, hac, hcc, 1.e-10, 0.01, 2, aMode, aScale, mstep);
         PFunctions(0, aMode, aScale, hcc);
         fii[0] = mWavenumberImaginary * sqrt(mP) * (1.0 - fif[0]) / (fif[0] + 1.0);
         fii[1] = fif[1];
         if (aHeightB > mHeightChange[aMode])
         {
            Integrate(this, HfiTau, fii, fif, hcc, hchc, 1.e-10, 0.001, 2, aMode, aScale, mstep);
            aHeightGainFunction = fif[1];
            aReflection         = fif[0];
            Integrate(this, HTau, &htgi, &aHeightGainFunction, hchc, hbc, 1.e-10, 0.125, 1, aMode, aScale, mstep);
            return;
         }
         else
         {
            Integrate(this, HfiTau, fii, fif, hcc, hbc, 1.e-10, 0.001, 2, aMode, aScale, mstep);
            aReflection         = fif[0];
            aHeightGainFunction = fif[1];
            return;
         }
      }
      else
      {
         Integrate(this, HrTau, fii, fif, hac, hbc, 1.e-10, 0.01, 2, aMode, aScale, mstep);
         aReflection         = fif[0];
         aHeightGainFunction = fif[1];
         return;
      }
   }
   else
   {
      if (aHeightA < mHeightChange[aMode])
      {
         if (aHeightB < mHeightChange[aMode])
         {
            Integrate(this, HfiTau, fii, fif, hac, hbc, 1.e-10, 0.001, 2, aMode, aScale, mstep);
            aReflection         = fif[0];
            aHeightGainFunction = fif[1];
            return;
         }
         else
         {
            hcc = hac;
            Integrate(this, HfiTau, fii, fif, hcc, hchc, 1.e-10, 0.001, 2, aMode, aScale, mstep);
            htgi = fif[1];
            Integrate(this, HTau, &htgi, &aHeightGainFunction, hchc, hbc, 1.e-10, 0.125, 1, aMode, aScale, mstep);
         }
      }
      else
      {
         htgi = aHeightGainFunction;
         Integrate(this, HTau, &htgi, &aHeightGainFunction, hac, hbc, 1.e-10, 0.125, 2, aMode, aScale, mstep);
      }
   }
   aReflection = fif[0];
}

// =================================================================================================
//! used as a stub to call WaveReflection with appropriate values
//! static
std::complex<double> WsfEM_GroundWavePropagation::Hr(WsfEM_GroundWavePropagation* aPtr,
                                                     int                          aCode,
                                                     int                          aMode,
                                                     double                       aScale,
                                                     std::complex<double>&        aHeight,
                                                     std::complex<double>*        aReflection)
{
   return aPtr->WaveReflection(1, aCode, aMode, aScale, aHeight, aReflection);
}

// =================================================================================================
//! used as a stub to call WaveReflection with appropriate values
//! static
std::complex<double> WsfEM_GroundWavePropagation::HTau(WsfEM_GroundWavePropagation* aPtr,
                                                       int                          aCode,
                                                       int                          aMode,
                                                       double                       aScale,
                                                       std::complex<double>&        aHeight,
                                                       std::complex<double>*        aHeightChangeFunction)
{
   return aPtr->WaveReflection(2, aCode, aMode, aScale, aHeight, aHeightChangeFunction);
}

// =================================================================================================
//! used as a stub to call WaveReflection with appropriate values
//! static
std::complex<double> WsfEM_GroundWavePropagation::HrTau(WsfEM_GroundWavePropagation* aPtr,
                                                        int                          aCode,
                                                        int                          aMode,
                                                        double                       aScale,
                                                        std::complex<double>&        aHeight,
                                                        std::complex<double>*        aFunction)
{
   return aPtr->WaveReflection(3, aCode, aMode, aScale, aHeight, aFunction);
}

// =================================================================================================
//! virtual protected
std::complex<double> WsfEM_GroundWavePropagation::WaveReflection(int                   aFunctionCode,
                                                                 int                   aCode,
                                                                 int                   aMode,
                                                                 double                aScale,
                                                                 std::complex<double>& aHeight,
                                                                 std::complex<double>* aFunction)
{
   // define static data
   static const unsigned int i[12] = {1, 3, 3, 5, 1, 2, 2, 3, 3, 4, 4, 5};

   // all values of P and D1P for two steps of the Runge-Kutta method
   // are computed in the first call to Hr or HTau
   if ((mReflectionCounter == 1) && (aCode == 0))
   {
      unsigned int loopInterval = 0;
      if (mIntegrationPhase < 0)
      {
         PFunctions(1, aMode, aScale, aHeight);
         mReflectionFunction[0]           = sqrt(mP);
         mReflectionDerivativeFunction[0] = mDp[0] / mP;
         loopInterval                     = 1;
      }
      else if (mIntegrationPhase == 0)
      {
         mReflectionFunction[0]           = mReflectionFunction[4];
         mReflectionDerivativeFunction[0] = mReflectionDerivativeFunction[4];
         loopInterval                     = 1;
      }
      else
      {
         mReflectionFunction[4]           = mReflectionFunction[2];
         mReflectionFunction[2]           = mReflectionFunction[1];
         mReflectionDerivativeFunction[4] = mReflectionDerivativeFunction[2];
         mReflectionDerivativeFunction[2] = mReflectionDerivativeFunction[1];
         mReflectionCounter               = 5;
         loopInterval                     = 2;
      }

      for (unsigned int n = 0; n < 4; n += loopInterval)
      {
         PFunctions(1, aMode, aScale, (aHeight + (0.5 * (n + 1) * mIntegrationStep)));
         mReflectionFunction[n + 1]           = sqrt(mP);
         mReflectionDerivativeFunction[n + 1] = mDp[0] / mP;
      }
   }

   std::complex<double> value(0.0, 0.0);
   if (aCode == 0)
   {
      mReflectionNF = i[mReflectionCounter - 1];
      if (aFunctionCode != 2)
      {
         value = -2.0 * mWavenumberImaginary * mReflectionFunction[mReflectionNF - 1] * aFunction[0] +
                 0.25 * mReflectionDerivativeFunction[mReflectionNF - 1] * (1.0 - aFunction[0] * aFunction[0]);
      }
      else
      {
         value = -mWavenumberImaginary * mReflectionFunction[mReflectionNF - 1] -
                 0.25 * mReflectionDerivativeFunction[mReflectionNF - 1];
      }
      ++mReflectionCounter;
      if (mReflectionCounter >= 13)
      {
         mReflectionCounter = 1;
      }
   }
   else
   {
      if (aFunctionCode == 1)
         value = -mWavenumberImaginary * (aFunction[0] / mReflectionFunction[mReflectionNF - 1] +
                                          2.0 * mReflectionFunction[mReflectionNF - 1] * aFunction[1]) -
                 mReflectionDerivativeFunction[mReflectionNF - 1] *
                    (0.25 * (1.0 - aFunction[0] * aFunction[0]) /
                        (mReflectionFunction[mReflectionNF - 1] * mReflectionFunction[mReflectionNF - 1]) +
                     0.5 * aFunction[0] * aFunction[1]);

      if (aFunctionCode == 3)
         value =
            mWavenumberImaginary * mReflectionFunction[mReflectionNF - 1] * (1.0 - aFunction[0]) / (1.0 + aFunction[0]);
   }

   return value;
}

// =================================================================================================
//! used as a stub to call WaveImpedanceFunction with appropriate values
//! static
std::complex<double> WsfEM_GroundWavePropagation::Hfi(WsfEM_GroundWavePropagation* aPtr,
                                                      int                          aCode,
                                                      int                          aMode,
                                                      double                       aScale,
                                                      std::complex<double>&        aHeight,
                                                      std::complex<double>*        aFunction)
{
   return aPtr->WaveImpedanceFunction(1, aCode, aMode, aScale, aHeight, aFunction);
}

// =================================================================================================
//! used as a stub to call WaveImpedanceFunction with appropriate values
//! static
std::complex<double> WsfEM_GroundWavePropagation::HfiTau(WsfEM_GroundWavePropagation* aPtr,
                                                         int                          aCode,
                                                         int                          aMode,
                                                         double                       aScale,
                                                         std::complex<double>&        aHeight,
                                                         std::complex<double>*        aFunction)
{
   return aPtr->WaveImpedanceFunction(2, aCode, aMode, aScale, aHeight, aFunction);
}

// =================================================================================================
//! function[0] is the wave impedance and function[2] is the height-gain
//! function at height.  these satisfy the first-order differential
//! equations dfi[j]/dh = hfi(j,height,function) where j=1 or j=2 which
//! are integrated in Integration
//! virtual protected
std::complex<double> WsfEM_GroundWavePropagation::WaveImpedanceFunction(int                   aFunctionCode,
                                                                        int                   aCode,
                                                                        int                   aMode,
                                                                        double                aScale,
                                                                        std::complex<double>& aHeight,
                                                                        std::complex<double>* aFunction)
{
   // define static data
   static const unsigned int i[12] = {1, 3, 3, 5, 1, 2, 2, 3, 3, 4, 4, 5};

   // all values of P for two steps of the Runge-Kutta method are
   // computed in the first call
   if ((mImpedanceCounter == 1) && (aCode == 0))
   {
      unsigned int loopStep = 1;
      if (mIntegrationPhase > 0)
      {
         mImpedanceFunction[4] = mImpedanceFunction[2];
         mImpedanceFunction[2] = mImpedanceFunction[1];
         mImpedanceCounter     = 5;
         loopStep              = 2;
      }
      else
      {
         if (mIntegrationPhase < 0)
         {
            PFunctions(0, aMode, aScale, aHeight);
            mImpedanceFunction[0] = mP;
         }
         else
         {
            mImpedanceFunction[0] = mImpedanceFunction[4];
         }
         loopStep = 1;
      }

      for (unsigned int count = 0; count < 4; count += loopStep)
      {
         PFunctions(0, aMode, aScale, aHeight + 0.5 * (count + 1) * mIntegrationStep);
         mImpedanceFunction[count + 1] = mP;
      }
   }

   std::complex<double> value(0.0, 0.0);
   if (aCode == 0)
   {
      // compute hfi(1,h,fi), this is the same as hfitau(1,h,fi)
      mReflectionNF = i[mImpedanceCounter - 1];
      value         = -mWavenumberSquared * mImpedanceFunction[mReflectionNF - 1] - aFunction[0] * aFunction[0];
      ++mImpedanceCounter;
      if (mImpedanceCounter >= 13)
      {
         mImpedanceCounter = 1;
      }
   }
   else
   {
      // compute hfi(2,f,fi), this is the same as hfitau(2,h,fi)
      // if aFunctionCode == 2
      if (aFunctionCode == 1)
      {
         value = -mWavenumberSquared - 2.0 * aFunction[0] * aFunction[1];
      }
      else
      {
         return aFunction[0];
      }
   }

   return value;
}

// =================================================================================================
//! computes the field strength-distance variation for terminals
//! close to the surface at short ranges. A series generalization of the
//! Sommerfeld flat-earth theory is used
//! virtual protected
double WsfEM_GroundWavePropagation::FlatEarthPropagation(WsfEM_Interaction&    aInteraction,
                                                         double                aScale,
                                                         std::complex<double>& aImpedance,
                                                         std::complex<double>  aHeightLowC,
                                                         std::complex<double>  aHeightHighC)
{
   using std::complex;

   // compute arrays yd and yr using YCalculation
   complex<double> yd[7];
   complex<double> yr[3][7];
   YCalculation(aHeightLowC, aHeightHighC, aScale, yd, yr);

   // first value of range
   double distance          = mMinDistance;
   int    n                 = 0;
   bool   convergence       = false;
   double range             = ComputeGroundDistance(aInteraction) * 0.001;
   double tlc               = UtMath::DB_ToLinear(BasicTransmissionLoss(aInteraction));
   double propagationFactor = 1.0;
   while ((!convergence) && (distance <= range))
   {
      // compute arrays jd and jrf using JCalculation & JRCalculation
      double          a = 1000.0 * mMinDistance / (1.0 + 0.5 * mDel);
      complex<double> u(0.5, -0.5);
      u *= sqrt(a * mWavenumber);
      complex<double> taud = -u * (aHeightHighC - aHeightLowC) / a;
      complex<double> taur = -u * (aHeightHighC + aHeightLowC) / a;
      complex<double> taup = taur - u * aImpedance;
      u *= 0.0004;
      complex<double> jrp[4][7];
      JRCalculation(taup, taur, jrp);
      complex<double> jd[17];
      JCalculation(7, taud, jd);

      // compute the surface wave from a series in powers of u
      complex<double> er(1.0, 0.0);
      if (std::real(aHeightHighC) > 0.0)
      {
         er = exp(taur * taur);
      }
      complex<double> fs     = er * yr[0][0] * jrp[1][6];
      complex<double> factor = er;

      complex<double> terms(0.0, 0.0);
      for (unsigned int i = 0; i < 6; ++i)
      {
         factor *= u;
         complex<double> s(0.0, 0.0);
         unsigned int    lMax = 1 + (i + 1) / 3;
         double          sn   = -1.0;

         for (unsigned int l = 0; l < lMax; ++l)
         {
            sn      = -sn;
            int ml  = (i + 1) - 3 * (l + 1) + 4;
            int ml1 = (i + 1) - (l + 1) + 1;
            s += sn * yr[l][ml - 1] * jrp[l + 1][ml1 - 1];
         }
         terms = factor * s;
         fs += terms;
      }

      // compute the space wave from series in powers of u
      complex<double> fd(0.0, 0.0);
      complex<double> fr(0.0, 0.0);
      complex<double> termd(0.0, 0.0);
      complex<double> termr(0.0, 0.0);

      if (std::real(aHeightLowC) != 0.0)
      {
         complex<double> factorD = 0.5 * exp(taud * taud);
         complex<double> factorR = -0.5 * er;

         for (unsigned int i = 0; i < 7; ++i)
         {
            termd = factorD * yd[i] * jd[i];
            termr = factorR * yr[0][i] * jrp[0][i];
            fd += termd;
            fr += termr;
            factorD *= u;
            factorR *= u;
         }
      }

      // compute the field strength in dB relative to 1 microVolt / meter
      // the reference dipole has a field of 150.0 microVolt / meter at
      // a distance of 1 km
      complex<double> field         = fs + fd + fr;
      complex<double> term          = terms + termd + termr;
      double          fieldStrength = 8.877e10 * std::norm(field) / (distance * distance);
      propagationFactor             = fieldStrength / tlc;

      double error = std::norm(term / field);
      if (error < 0.0025)
      {
         if (n == 99)
         {
            break;
         }

         // compute next range
         ++n;
         distance += mDistanceInterval;
         if (distance >= range)
         {
            distance    = range;
            convergence = true;
         }
      }
      else
      {
         distance *= pow((0.0025 / error), (1.0 / 6.0));
         convergence = true;
      }
   }

   return propagationFactor;
}

// =================================================================================================
//! computes arrays yd and yr
//! virtual protected
void WsfEM_GroundWavePropagation::YCalculation(std::complex<double> aHeightLowC,
                                               std::complex<double> aHeightHighC,
                                               double               aScale,
                                               std::complex<double> aYd[7],
                                               std::complex<double> aYr[3][7])
{
   using std::complex;

   // define static data
   static const double cY3[16] = {-0.09501251,
                                  0.09501251,
                                  -0.28160355,
                                  0.28160355,
                                  -0.45801678,
                                  0.45801678,
                                  -0.61787624,
                                  0.61787624,
                                  -0.75540440,
                                  0.75540440,
                                  -0.86563120,
                                  0.86563120,
                                  -0.94457502,
                                  0.94457502,
                                  -0.98940093,
                                  0.98940093};

   static const double cW3[16] = {0.18945061,
                                  0.18945061,
                                  0.18260342,
                                  0.18260342,
                                  0.16915652,
                                  0.16915652,
                                  0.14959599,
                                  0.14959599,
                                  0.12462897,
                                  0.12462897,
                                  0.09515851,
                                  0.09515851,
                                  0.06225352,
                                  0.06225352,
                                  0.02715246,
                                  0.02715246};

   // compute the integrals ai using 16-point gaussian quadrature
   complex<double> complexZero(0.0, 0.0);
   mP0[0] = complexZero;
   int m  = 0;

   bool            lh = (std::real(aHeightHighC) == 0.0);
   bool            ll = (std::real(aHeightLowC) == 0.0);
   complex<double> gamma[7][2];
   unsigned int    lMax = 0;
   if (!lh)
   {
      complex<double> ap[2];
      complex<double> am[2];
      ap[0] = 0.5 * (aHeightLowC + aHeightHighC);
      am[0] = ap[0] - aHeightHighC;
      ap[1] = 0.5 * aHeightLowC;
      am[1] = -ap[1];

      lMax = 2;
      if (ll)
      {
         lMax = 1;
      }
      complex<double> ai[3][2];
      for (unsigned int l = 0; l < lMax; ++l)
      {
         for (unsigned int n = 0; n < 3; ++n)
         {
            ai[n][l] = complexZero;
         }

         for (unsigned int i = 0; i < 16; ++i)
         {
            PFunctions(0, m, aScale, am[l] * cY3[i] + ap[l]);
            complex<double> fi = 0.0004 * cW3[i];
            for (unsigned int n = 0; n < 3; ++n)
            {
               fi *= 3.125e6 * mP;
               ai[n][l] += fi;
            }
         }

         for (unsigned int n = 0; n < 3; ++n)
         {
            ai[n][l] = am[l] * mWavenumberImaginary * ai[n][l];
            if (l == 1)
            {
               ai[n][l] = ai[n][l - 1] + 2.0 * ai[n][l];
            }
         }
      }

      // compute array alp
      complex<double> alp[7][2];
      for (unsigned l = 0; l < lMax; ++l)
      {
         complex<double> dummyC(1.0, 0.0);
         alp[0][l] = dummyC;

         for (unsigned int n = 1; n < 7; ++n)
         {
            alp[n][l] = alp[n - 1][l] * ai[0][l] / static_cast<double>(n);
         }
         alp[3][l] -= 0.5 * ai[1][l];
         alp[4][l] -= 0.5 * ai[1][l] * ai[0][l];
         alp[5][l] += -0.5 * ai[1][l] * alp[2][l] + 0.5 * ai[2][l];
         alp[6][l] += -0.5 * ai[1][l] * (0.5 * alp[3][l] + 0.125 * ai[1][l]) + 0.5 * ai[0][l] * ai[2][l];
      }

      // compute array beta
      complex<double> beta[4];
      PFunctions(0, m, aScale, aHeightLowC);
      complex<double> rl = 6.25e6 * mP;
      PFunctions(0, m, aScale, aHeightHighC);
      complex<double> rh = 6.25e6 * mP;

      complex<double> dummyC(1.0, 0.0);
      beta[0] = dummyC;
      beta[1] = -0.25 * (rl + rh);
      beta[2] = (5.0 * (rl * rl + rh * rh) + 2.0 * rl * rh) / 32.0;
      beta[3] = -(rl * rl * (3.0 * rl + rh) + rh * rh * (3.0 * rh + rl)) / 25.6;

      // compute array gamma
      for (unsigned int l = 0; l < lMax; ++l)
      {
         for (unsigned int n = 0; n < 7; ++n)
         {
            unsigned int iMax = static_cast<int>(n * 0.5 + 1);
            gamma[n][l]       = complexZero;

            for (unsigned int i = 0; i < iMax; ++i)
            {
               int is = (n + 1) - 2 * (i + 1) + 2;
               gamma[n][l] += alp[is - 1][l] * beta[i];
            }
         }
      }
   }

   // compute the array A
   complex<double> a[4];
   PFunctions(4, m, aScale, complexZero);
   complex<double> factor(3.125e6, 0.0);
   for (unsigned int n = 0; n < 4; ++n)
   {
      factor *= 1250.0 / mWavenumberImaginary;
      a[n] = mDp[n] * factor;
   }
   a[3] -= 2.5 * a[0] * a[0];

   // compute the arrays yd and yr
   for (unsigned int n = 0; n < 7; ++n)
   {
      aYd[n] = complexZero;
      for (unsigned int i = 0; i < 3; ++i)
      {
         aYr[i][n] = complexZero;
      }
   }

   if (lh)
   {
      complex<double> dummyC(1.0, 0.0);
      aYd[0]    = dummyC;
      aYr[0][0] = dummyC;
      for (unsigned int n = 0; n < 4; ++n)
      {
         aYr[1][n] = a[n];
      }
      aYr[2][0] = a[0] * a[0];
   }
   else
   {
      for (unsigned int n = 0; n < 7; ++n)
      {
         aYd[n]    = gamma[n][0];
         aYr[0][n] = gamma[n][lMax - 1];
      }

      for (unsigned int n = 0; n < 4; ++n)
      {
         aYr[1][n] = complexZero;
         for (unsigned int m1 = 0; m1 < n; ++m1)
         {
            int n1 = n - m1;
            aYr[1][n] += a[m1] * aYr[0][n1];
         }
      }
      aYr[2][0] = aYr[1][0] * a[0];
   }
}

// =================================================================================================
//! computes a set of functions J(m) (m=1,max) with arguement tau
//! virtual protected
void WsfEM_GroundWavePropagation::JCalculation(int aMaxN, std::complex<double>& aTau, std::complex<double>* aJ)
{
   std::complex<double>  a[34];
   std::complex<double>* b = &a[17];

   // compute j[0] & j[1]
   std::complex<double> dummyC(1.0, 0.0);
   aJ[0] = dummyC;
   std::complex<double> dummyC2(0.0, 1.772454);
   aJ[1] = dummyC2 * WFunction(aTau);

   if (aMaxN == 2)
   {
      return;
   }

   // compute a(n) and b(n) (n=1, nmax) using the forward recurrence relation
   std::complex<double> complexZero(0.0, 0.0);
   a[0] = aJ[0];
   a[1] = complexZero;
   b[0] = complexZero;
   b[1] = aJ[1];
   for (int n = 2; n < aMaxN; ++n)
   {
      double c = 1.0 / (1.0 - 0.5 * (n + 1));
      a[n]     = c * (aTau * a[n - 1] + a[n - 2]);
      b[n]     = c * (aTau * b[n - 1] + b[n - 2]);
   }

   // test for bounding errors
   if (std::norm(1.0 + b[aMaxN - 1] / a[aMaxN - 1]) < 1.e-5)
   {
      // for large tau, use the backward recurrence relation
      a[33]                       = complexZero;
      a[32]                       = dummyC;
      std::complex<double> factor = 0.5 / (aTau * aTau);
      for (unsigned int n = 0; n < 32; ++n)
      {
         unsigned int n1 = 32 - n;
         a[n1]           = -a[n1 + 1] - static_cast<double>(n1 + 1) * factor * a[n1 + 2 - 1];
      }
      factor                    = 1.0 / a[0];
      std::complex<double> rTau = 1.0 / aTau;
      for (int n = 1; n < aMaxN; ++n)
      {
         factor *= rTau;
         a[n] *= factor;
      }
      a[0] = dummyC;
      for (int n = 2; n < aMaxN; ++n)
      {
         aJ[n] = a[n];
      }
   }
   else
   {
      for (int n = 2; n < aMaxN; ++n)
      {
         aJ[n] = a[n] + b[n];
      }
   }
}

// =================================================================================================
//! computes the array jrp. each element is a combination of the error
//! functions w(taup) and w(taur)
//! virtual protected
void WsfEM_GroundWavePropagation::JRCalculation(std::complex<double>& aTauP,
                                                std::complex<double>& aTauR,
                                                std::complex<double>  aJrp[4][7])
{
   // difference procedures are used according as the modulus of
   // (taup - taur) is less than or greater than 0.4
   std::complex<double> complexZero(0.0, 0.0);
   for (unsigned int m = 0; m < 7; ++m)
   {
      for (unsigned int n = 0; n < 4; ++n)
      {
         aJrp[n][m] = complexZero;
      }
   }

   std::complex<double> dt = aTauP - aTauR;
   bool                 lt = (std::norm(dt) < 0.16);
   if (lt)
   {
      // small delta. compute jrp[1][m] (m=1,7) using JCalculation
      // compute jrp[2][6] from a series in powers of (taup - taur)
      std::complex<double> j[17];
      JCalculation(17, aTauR, j);
      for (unsigned int m = 0; m < 7; ++m)
      {
         aJrp[0][m] = j[m];
      }
      aJrp[1][5] = j[6];

      std::complex<double> factor(1.0, 0.0);
      for (unsigned int n = 7; n < 17; ++n)
      {
         factor *= dt;
         std::complex<double> term = factor * j[n];
         aJrp[1][5] += term;
         if (std::norm(term / aJrp[1][5]) < 1.0e-10)
         {
            break;
         }
      }

      // compute jrp(2,m) (m=1,5) from the backward recurrence relation
      for (unsigned int m = 0; m < 5; ++m)
      {
         int m1      = 5 - m - 1;
         aJrp[1][m1] = aJrp[0][m1 + 1] + dt * aJrp[1][m1 + 1];
      }
   }
   else
   {
      // large (tauP - tauR). compute jrp(1,m) (m=1,7) and jrp(2,1) using
      // JCalculation
      std::complex<double> j[17];
      JCalculation(2, aTauP, j);
      aJrp[1][0] = j[1];

      JCalculation(7, aTauR, j);
      for (unsigned int m = 0; m < 7; ++m)
      {
         aJrp[0][m] = j[m];
      }

      // use the recurrence relations forward to compute jrp(2,m) (m=2,6)
      std::complex<double> rdt = 1.0 / dt;
      for (unsigned int m = 1; m < 6; ++m)
      {
         aJrp[1][m] = rdt * (aJrp[1][m - 1] - aJrp[0][m]);
      }
   }

   // compute jrp(3,m) (m=2,5) and jrp(4,4) from the forward recurrence relation
   for (unsigned int m = 1; m < 5; ++m)
   {
      aJrp[2][m] = -2.0 * aJrp[1][m - 1] - 2.0 * aTauR * aJrp[1][m] - static_cast<double>(m) * aJrp[1][m + 1];
   }
   aJrp[3][3] = -aJrp[2][2] - aTauR * aJrp[2][3] - 1.5 * aJrp[2][4];

   // compute jrp(2,0) and place in jrp(2,7)
   aJrp[1][6] = aJrp[0][0] + dt * aJrp[1][0];
}

// =================================================================================================
//! w(z) = exp(-z^2) * erfc(-i * z);
//! virtual protected
std::complex<double> WsfEM_GroundWavePropagation::WFunction(std::complex<double> aZ)
{
   using std::complex;

   double               zr       = std::real(aZ);
   double               zi       = std::imag(aZ);
   std::complex<double> zSquared = aZ * aZ;
   double               cz       = zr * zr + zi * zi;

   bool            type = (fabs(zr) > fabs(zi));
   complex<double> returnValue(0.0, 0.0);
   if ((0.238 * fabs(zr) + 0.269 * fabs(zi) > 1.0) || (0.3225 * fabs(zr) - 0.37 * fabs(zi) > 1.0) ||
       (0.09 * fabs(zr) + 0.4 * fabs(zi) > 1.0))
   {
      complex<double> ra = 1.0 / aZ;
      if (zi < 0.0)
      {
         ra = -ra;
      }
      complex<double> raSquared = ra * ra;
      complex<double> dummyC(0.0, 0.5641896);
      complex<double> term = dummyC * ra;
      complex<double> sum  = term;
      complex<double> test = sum;
      for (unsigned int m = 0; m < 40; ++m)
      {
         term *= (m + 1 - 0.5) * raSquared;
         sum += term;
         complex<double> factor = 1.0 / (zSquared - static_cast<double>(m + 1) - 0.5);
         if ((2.0 * (m + 1) + 1) * std::norm(factor) <= 0.4)
         {
            returnValue = sum + ((m + 1) + 0.5) * term * factor * (1.0 + zSquared * factor * factor);
         }
         else
         {
            returnValue = sum + term * (-1.0 + zSquared * dummyC / sqrt(2.0 * (m + 1) + 1.0) +
                                        (zSquared - static_cast<double>(m + 1) - 0.5) / static_cast<double>(m + 1 + 0.5));
         }

         if (std::norm(1.0 - test / returnValue) < 1.0e-10)
         {
            break;
         }

         test = returnValue;
      }

      if ((type) && (fabs(zr * zi) < 0.25))
      {
         returnValue += exp(-zSquared);
      }
      if (zi < 0.0)
      {
         returnValue = 2.0 * exp(-zSquared) - returnValue;
      }
   }
   else if (cz == 0.0)
   {
      complex<double> dummyC(1.0, 0.0);
      returnValue = dummyC;
   }
   else
   {
      complex<double> term(0.0, 1.128379);
      term *= aZ;
      complex<double> sum  = term;
      returnValue          = sum;
      complex<double> test = sum;

      for (unsigned int m = 0; m < 40; ++m)
      {
         if (type)
         {
            term = (m + 1 - 0.5) / ((m + 1) * (m + 1 + 0.5)) * zSquared * term;
            sum += term;
            returnValue = sum;
            if ((m + 1) >= cz + 2)
            {
               complex<double> factor = zSquared / ((m + 1) + 1.0 - zSquared);
               returnValue += (factor * (1.0 - factor * factor / (m + 1 + 1.0)) - 1.0 / ((m + 1) + 1.0)) * term;
            }
         }
         else
         {
            term = -term * zSquared / (m + 1 + 0.5);
            sum += term;
            returnValue = sum;
            if ((m + 1) >= cz + 2)
            {
               complex<double> factor = 1.0 / (m + 1 + 1.5 + zSquared);
               returnValue -= zSquared * term * factor * (1.0 + zSquared * factor * factor);
            }
         }

         if (std::norm(1.0 - test / returnValue) < 1.0e-10)
         {
            break;
         }

         test = returnValue;
      }

      if (type)
      {
         returnValue = exp(-zSquared) * (1.0 + returnValue);
      }
      else
      {
         returnValue += exp(-zSquared);
      }
   }

   return returnValue;
}

// =================================================================================================
//! computes the phase integral (j=1), its first (j=2) or second
//! (j=3) derivative WRT P0(m) from ha to hb.
//! virtual protected
std::complex<double> WsfEM_GroundWavePropagation::PhaseIntegralAB(int                   aCode,
                                                                  int                   aMode,
                                                                  double                aScale,
                                                                  std::complex<double>& aHeightA,
                                                                  std::complex<double>& aHeightB)
{
   // define static data
   static const double cY3[16] = {-0.09501251,
                                  0.09501251,
                                  -0.28160355,
                                  0.28160355,
                                  -0.45801678,
                                  0.45801678,
                                  -0.61787624,
                                  0.61787624,
                                  -0.75540440,
                                  0.75540440,
                                  -0.86563120,
                                  0.86563120,
                                  -0.94457502,
                                  0.94457502,
                                  -0.98940093,
                                  0.98940093};

   static const double cW3[16] = {0.18945061,
                                  0.18945061,
                                  0.18260342,
                                  0.18260342,
                                  0.16915652,
                                  0.16915652,
                                  0.14959599,
                                  0.14959599,
                                  0.12462897,
                                  0.12462897,
                                  0.09515851,
                                  0.09515851,
                                  0.06225352,
                                  0.06225352,
                                  0.02715246,
                                  0.02715246};
   static const double a[3]    = {1.0, 0.5, -0.25};

   std::complex<double> fp = 0.5 * (aHeightB + aHeightA);
   std::complex<double> fm = fp - aHeightA;
   std::complex<double> returnValue(0.0, 0.0);

   for (unsigned int i = 0; i < 16; ++i)
   {
      PFunctions(0, aMode, aScale, fm * cY3[i] + fp);

      std::complex<double> dumC(0.0, -0.5);
      std::complex<double> factor = cW3[i] * sqrt(dumC * mP);
      if (aCode >= 2)
      {
         factor /= mP;
      }
      if (aCode == 3)
      {
         factor /= mP;
      }

      returnValue += factor;
   }

   std::complex<double> complexOne(1.0, 1.0);
   returnValue *= a[aCode - 1] * complexOne * mWavenumberImaginary * fm;

   return returnValue;
}

// =================================================================================================
//! computes combinations of phase integrals used in DirectRay
//! virtual protected
std::complex<double> WsfEM_GroundWavePropagation::PhaseIntegralCombinations(int                  aCode,
                                                                            int                  aMode,
                                                                            double               aScale,
                                                                            std::complex<double> aHeightLow,
                                                                            std::complex<double> aHeightHigh)
{
   std::complex<double> returnValue(0.0, 0.0);
   if (aMode == 0)
   {
      if (std::real(aHeightLow - mTurningPoint[aMode]) < std::max(10.0, std::real(aHeightHigh - aHeightLow)))
      {
         if (std::real(aHeightLow - mTurningPoint[aMode]) < 0.0)
         {
            returnValue =
               -PhaseIntegral(aCode, aHeightLow, aMode, aScale) - PhaseIntegral(aCode, aHeightHigh, aMode, aScale);
         }
         else
         {
            returnValue =
               PhaseIntegral(aCode, aHeightLow, aMode, aScale) - PhaseIntegral(aCode, aHeightHigh, aMode, aScale);
         }
      }
      else
      {
         returnValue = PhaseIntegralAB(aCode, aMode, aScale, aHeightHigh, aHeightLow);
      }
   }
   else if (aMode == 1)
   {
      returnValue = -PhaseIntegral(aCode, aHeightLow, aMode, aScale) - PhaseIntegral(aCode, aHeightHigh, aMode, aScale);
   }
   else
   {
      std::complex<double> complexZero(0.0, 0.0);
      bool                 a = (std::real(-mTurningPoint[aMode]) < std::max(10.0, std::real(aHeightLow)));
      bool                 b = (std::real(-mTurningPoint[aMode]) < std::max(10.0, std::real(aHeightHigh)));
      if (a)
         returnValue = 2.0 * PhaseIntegral(aCode, complexZero, aMode, aScale) -
                       PhaseIntegral(aMode, aHeightLow, aMode, aScale) - PhaseIntegral(aMode, aHeightHigh, aMode, aScale);

      if ((!a) && (b))
         returnValue = PhaseIntegral(aCode, complexZero, aMode, aScale) -
                       PhaseIntegral(aMode, aHeightHigh, aMode, aScale) +
                       PhaseIntegralAB(aCode, aMode, aScale, aHeightLow, complexZero);

      if (!b)
         returnValue = 2.0 * PhaseIntegralAB(aCode, aMode, aScale, aHeightLow, complexZero) +
                       PhaseIntegralAB(aCode, aMode, aScale, aHeightHigh, aHeightLow);
   }

   return returnValue;
}

// =================================================================================================
//! implicit function inside DirectRay
//! virtual protected
double WsfEM_GroundWavePropagation::DirectRayFunction(double aX, double aB, double aDH, double aD)
{
   double value = aB * aX - aDH * aDH * aD - aX * (aD - aX) * (aD - 2.0 * aX);
   return value;
}

// =================================================================================================
//! computes components of the direct ray before (m=0) or after (m=1)
//! it has turned or the reflected ray (m=2)
//! these components are used in stationary phase integration
void WsfEM_GroundWavePropagation::DirectRay(double                aScale,
                                            double                aD1P0,
                                            int                   aMode,
                                            std::complex<double>& aHeightLow,
                                            std::complex<double>& aHeightHigh,
                                            double                aDistance,
                                            double                aDistanceC,
                                            double                aDistanceH,
                                            double                aDistanceL,
                                            std::complex<double>  aF[9],
                                            std::complex<double>  aFdd[9],
                                            std::complex<double>  aG[9])
{
   double a = 250.0 * aD1P0 / (1.0 + 0.5 * mDel);

   // compute a starting value for the saddle point P0(m) when distance
   // is closer to the horizon that distanceC
   if (aMode == 2)
   {
      double b     = aDistanceH * aDistanceH + aDistanceL * aDistanceL;
      double dHigh = 0.0;
      if (aDistanceH != aDistanceL)
      {
         double d0 = 0.5 * aDistance;
         double d1 = aDistance * aDistanceH * aDistanceH / b;
         double f0 = DirectRayFunction(d0, b, aDistanceH, aDistance);
         double f1 = DirectRayFunction(d1, b, aDistanceH, aDistance);

         for (unsigned int i = 0; i < 20; ++i)
         {
            dHigh     = d1 - (d1 - d0) * f1 / (f1 - f0);
            double d3 = 1.e-4 * dHigh;
            double d4 = fabs(dHigh - d1);
            double d5 = fabs(dHigh - d0);
            if ((d4 < d3) || (d5 < d3))
            {
               break;
            }

            double f2 = DirectRayFunction(dHigh, b, aDistanceH, aDistance);
            if (d5 > 10.0 * d4)
            {
               d3        = 0.5 * (dHigh + d0);
               double f3 = DirectRayFunction(d3, b, aDistanceH, aDistance);
               if (UtMath::Sign(f0) == UtMath::Sign(f3))
               {
                  d0 = d3;
                  f0 = f3;
               }
            }
            else if (d4 > 10.0 * d5)
            {
               d3        = 0.5 * (dHigh + d1);
               double f3 = DirectRayFunction(d3, b, aDistanceH, aDistance);
               if (UtMath::Sign(f1) == UtMath::Sign(f3))
               {
                  d1 = d3;
                  f1 = f3;
               }
            }

            if (UtMath::Sign(f2) == UtMath::Sign(f1))
            {
               f1 = f2;
               d1 = dHigh;
            }
            else
            {
               f0 = f2;
               d0 = dHigh;
            }
         }
      }
      else
      {
         dHigh = 0.5 * aDistance;
      }

      mP0[2] = (aDistanceH * aDistanceH / dHigh - dHigh) * a;
      mP0[2] *= mP0[2];
   }
   else
   {
      // compute a starting value for the saddle point P0(m) when
      // distance is closer to distanceC than the horizon
      double pl = (aDistanceC * aDistanceC / aDistance - aDistance) * a;
      pl *= pl;
      std::complex<double> complexZero(0.0, 0.0);
      mP0[aMode] = complexZero;
      PFunctions(0, aMode, aScale, aHeightLow);
      mP0[aMode] = pl - mP;
   }

   // compute the saddle point P0[m] by Newton's method
   std::complex<double> factor = 500.0 * aDistance * mWavenumberImaginary / (1.0 + mDel);
   TurningPoint(aScale, aMode);
   for (unsigned int iter = 0; iter < 9; ++iter)
   {
      std::complex<double> dp0 = (factor + PhaseIntegralCombinations(2, aMode, aScale, aHeightLow, aHeightHigh)) /
                                 PhaseIntegralCombinations(3, aMode, aScale, aHeightLow, aHeightHigh);
      mP0[aMode] -= dp0;
      TurningPoint(aScale, aMode);
      double eps = std::norm(dp0 / mP0[aMode]);
      if ((eps < 1.0e-8) || (std::norm(dp0) < 1.0e-18))
      {
         break;
      }
   }

   // compute the quantities f(m), fdd(m) and g(m) required for
   // stationary phase integration
   aF[aMode]   = factor * mP0[aMode] + PhaseIntegralCombinations(1, aMode, aScale, aHeightLow, aHeightHigh);
   aFdd[aMode] = PhaseIntegralCombinations(3, aMode, aScale, aHeightLow, aHeightHigh);
   PFunctions(0, aMode, aScale, aHeightLow);
   factor = mP;
   PFunctions(0, aMode, aScale, aHeightHigh);
   aG[aMode] = 1.0 / sqrt(sqrt(factor * mP));
}
