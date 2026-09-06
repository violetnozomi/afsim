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

#ifndef WSFEM_GROUNDWAVEPROPAGATION_HPP
#define WSFEM_GROUNDWAVEPROPAGATION_HPP

#include "wsf_export.h"

#include <complex>

#include "WsfEM_Antenna.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Propagation.hpp"
#include "WsfEM_Types.hpp"
class WsfEM_XmtrRcvr;

//! Ground wave propagation model.
class WSF_EXPORT WsfEM_GroundWavePropagation : public WsfEM_Propagation
{
public:
   WsfEM_GroundWavePropagation();
   WsfEM_GroundWavePropagation(const WsfEM_GroundWavePropagation& aSrc);
   ~WsfEM_GroundWavePropagation() override = default;
   WsfEM_GroundWavePropagation& operator=(const WsfEM_GroundWavePropagation& aRhs) = delete;

   static WsfEM_Propagation* ObjectFactory(const std::string& aTypeName);

   WsfEM_Propagation* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;

   double ComputePropagationFactor(WsfEM_Interaction& aInteraction, WsfEnvironment& aEnvironment) override;

protected:
   void SetupExponentialAtmosphere(double& aScale, double& aD1P0);

   void ModifyValuesForVerticalPol(double                aScale,
                                   double                aD1P0,
                                   std::complex<double>& aImpedance,
                                   double&               aD1P0V,
                                   std::complex<double>& aImpedanceV);

   bool FarFieldTransmissionLoss(WsfEM_Interaction&    aInteraction,
                                 double                aScale,
                                 double                aD1P0,
                                 std::complex<double>& aImpedance,
                                 double&               aPropagationFactor);

   void GeometricalOptics(WsfEM_Interaction&    aInteraction,
                          double                aScale,
                          double                aD1P0,
                          std::complex<double>& aImpedance,
                          double&               aPropagationFactor);

   void Eigen(WsfEM_GroundWavePropagation* aPtr,
              int                          aMode,
              std::complex<double>&        aImpedance,
              double                       aScale,
              double                       aD1P0,
              std::complex<double>         aFid[9]);

   double BasicTransmissionLoss(WsfEM_Interaction& aInteraction);

   double ComputeGroundDistance(WsfEM_Interaction& aInteraction);

   static std::complex<double> Zxi(WsfEM_GroundWavePropagation* aPtr,
                                   int                          dummy,
                                   int                          dummy1,
                                   double                       dummy2,
                                   std::complex<double>&        aX,
                                   std::complex<double>*        aZ);

   static std::complex<double> Zpsi(WsfEM_GroundWavePropagation* aPtr,
                                    int                          dummy,
                                    int                          dummy1,
                                    double                       dummy2,
                                    std::complex<double>&        aY,
                                    std::complex<double>*        aZ);

   void StartPropagation(WsfEM_GroundWavePropagation* aPtr,
                         int                          aMode,
                         std::complex<double>&        aImpedance,
                         double                       aScale,
                         double                       aD1P0);

   std::complex<double> PhaseIntegral(int aCode, std::complex<double>& aHeight, int aMode, double aScale);

   void PFunctions(int aN, int aMode, double aScale, const std::complex<double>& aHeight);

   void TurningPoint(double aScale, int aMode);

   static void Integrate(
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
      int                   aMStep);

   static void IntegrationStep(
      WsfEM_GroundWavePropagation* aPtr,
      std::complex<double> (*aF)(WsfEM_GroundWavePropagation*, int, int, double, std::complex<double>&, std::complex<double>*),
      std::complex<double>* aY1,
      std::complex<double>* aY2,
      std::complex<double>  aX1,
      std::complex<double>  aH,
      int                   aMode,
      double                aScale,
      int                   aN);

   void Height(int                   aMode,
               double                aScale,
               std::complex<double>& aReflection,
               std::complex<double>& aHeightGainFunction,
               double                aHeightA,
               double                aHeightB);

   static std::complex<double> Hr(WsfEM_GroundWavePropagation* aPtr,
                                  int                          aCode,
                                  int                          aMode,
                                  double                       aScale,
                                  std::complex<double>&        aHeight,
                                  std::complex<double>*        aReflection);

   static std::complex<double> HTau(WsfEM_GroundWavePropagation* aPtr,
                                    int                          aCode,
                                    int                          aMode,
                                    double                       aScale,
                                    std::complex<double>&        aHeight,
                                    std::complex<double>*        aHeightChangeFunction);

   static std::complex<double> HrTau(WsfEM_GroundWavePropagation* aPtr,
                                     int                          aCode,
                                     int                          aMode,
                                     double                       aScale,
                                     std::complex<double>&        aHeight,
                                     std::complex<double>*        aFunction);

   std::complex<double> WaveReflection(int                   aFunctionCode,
                                       int                   aCode,
                                       int                   aMode,
                                       double                aScale,
                                       std::complex<double>& aHeight,
                                       std::complex<double>* aFunction);

   static std::complex<double> Hfi(WsfEM_GroundWavePropagation* aPtr,
                                   int                          aCode,
                                   int                          aMode,
                                   double                       aScale,
                                   std::complex<double>&        aHeight,
                                   std::complex<double>*        aFunction);

   static std::complex<double> HfiTau(WsfEM_GroundWavePropagation* aPtr,
                                      int                          aCode,
                                      int                          aMode,
                                      double                       aScale,
                                      std::complex<double>&        aHeight,
                                      std::complex<double>*        aFunction);

   std::complex<double> WaveImpedanceFunction(int                   aFunctionCode,
                                              int                   aCode,
                                              int                   aMode,
                                              double                aScale,
                                              std::complex<double>& aHeight,
                                              std::complex<double>* aFunction);

   double FlatEarthPropagation(WsfEM_Interaction&    aInteraction,
                               double                aScale,
                               std::complex<double>& aImpedance,
                               std::complex<double>  aHeightLowC,
                               std::complex<double>  aHeightHighC);

   void YCalculation(std::complex<double> aHeightLow,
                     std::complex<double> aHeightHigh,
                     double               aScale,
                     std::complex<double> aYd[7],
                     std::complex<double> aYr[3][7]);

   void JCalculation(int aMaxN, std::complex<double>& aTau, std::complex<double>* aJ);

   void JRCalculation(std::complex<double>& aTauP, std::complex<double>& aTauR, std::complex<double> aJrp[4][7]);

   std::complex<double> WFunction(std::complex<double> aZ);

   std::complex<double>
   PhaseIntegralAB(int aCode, int aMode, double aScale, std::complex<double>& aHeightA, std::complex<double>& aHeightB);

   std::complex<double> PhaseIntegralCombinations(int                  aCode,
                                                  int                  aMode,
                                                  double               aScale,
                                                  std::complex<double> aHeightLow,
                                                  std::complex<double> aHeightHigh);

   double DirectRayFunction(double aX, double aB, double aDistanceH, double aDistance);

   void DirectRay(double                aScale,
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
                  std::complex<double>  aG[9]);

   //! relative permittivity of Earth's surface
   double mRelativePermittivity;
   //! conductivity of Earth's surface
   double mConductivity;
   //! troposphere refractivity
   double mTroposphereRefractivity;
   //! troposphere scale height (km's)
   double mTroposphereHeightScale;
   //! del - unknown
   double mDel;
   //! minimum distance for computing transmission loss (km's)
   double mMinDistance;
   //! distance interval for computing transmission loss (km's)
   double mDistanceInterval;
   //! frequency wavenumber
   double               mWavenumber;
   double               mWavenumberSquared;
   std::complex<double> mWavenumberImaginary;
   //!
   std::complex<double> mP;
   //! integration phase in Runge-Kutta integration
   int mIntegrationPhase;
   //! integration step in Runge-Kutta integration
   std::complex<double> mIntegrationStep;
   //! counter inside impedance function
   unsigned int mImpedanceCounter;
   //!
   std::complex<double> mImpedanceFunction[9];
   //!
   std::complex<double> mReflectionFunction[9];
   //!
   std::complex<double> mReflectionDerivativeFunction[9];
   //! counter inside reflection function
   unsigned int mReflectionCounter;
   //!
   unsigned int mReflectionNF;
   //! function
   std::complex<double> mP0[9];
   //! turning point
   std::complex<double> mTurningPoint[9];
   //! height change
   double mHeightChange[9];
   //! derivative of function P
   std::complex<double> mDp[4];
   //! altitude of "transmitter"
   double mXmtrAlt;
   //! altitude of "receiver"
   double mRcvrAlt;
};

#endif
