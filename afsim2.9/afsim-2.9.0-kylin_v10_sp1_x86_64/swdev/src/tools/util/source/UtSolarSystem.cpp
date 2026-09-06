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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "UtSolarSystem.hpp"

#include <iostream>

#include "UtCalendar.hpp"
#include "UtEarth.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtMoon.hpp"
#include "UtOrbitalElements.hpp"
#include "UtOrbitalPropagator.hpp"
#include "UtOrbitalState.hpp"
#include "UtSphericalEarth.hpp"
#include "UtSun.hpp"
#include "UtVec3.hpp"

namespace
{
const ut::Sun cSUN;

UtOrbitalElements mercuryOE(cSUN,
                            5.790908292757E+10,
                            2.056317500000E-01,
                            3.050744548472E+00,
                            1.222600697561E-01,
                            8.435332132791E-01,
                            5.083310890875E-01);
ut::OrbitalState  mercuryOrbitalState(ut::OrbitalState::CoordinateSystem::cECLIPTIC,
                                     ut::OrbitalState::ReferenceFrame::cECI,
                                     mercuryOE);
UtOrbitalElements mercuryOE_Delta1(cSUN,
                                   0.000000000000E+00,
                                   2.040700000000E-05,
                                   2.608787541452E+03,
                                   3.179117232508E-05,
                                   2.070166965316E-02,
                                   6.463989200381E-03);
// UtOrbitalElements
// mercuryOE(UtOrbitalElements::Equinox::cECI,5.7910050792E+10,2.05635930000000E-01,3.05070510787079E+00,1.22259947932125E-01,8.43530995489195E-01,5.08362580935813E-01);
// UtOrbitalElements
// mercuryOE_Delta1(UtOrbitalElements::Equinox::cECI,5.5352000000E+04,1.90600000000000E-05,2.60878750416041E+03,-1.03803282729437E-04,-2.18760982161662E-03,4.98845992547737E-03);
UtSolarSystem::Planet sMercury(mercuryOrbitalState, mercuryOE_Delta1);

//   UtOrbitalElements
//   venusOE(UtOrbitalElements::Equinox::cECI,1.0821101474E+11,6.77672000000000E-03,8.79238100050585E-01,5.92482741110953E-02,1.33831572240834E+00,9.58580633630427E-01);
//   UtOrbitalElements
//   venusOE_Delta1(UtOrbitalElements::Equinox::cECI,5.8344000000E+05,-4.10700000000000E-05,1.02132850275016E+03,-1.37689024689832E-05,-4.84667775462576E-03,4.89350999991160E-03);

UtOrbitalElements venusOE(cSUN,
                          1.082086008858E+11,
                          6.771920000000E-03,
                          8.799269061081E-01,
                          5.924802889234E-02,
                          1.338317074166E+00,
                          9.579027193001E-01);
ut::OrbitalState  venusOrbitalState(ut::OrbitalState::CoordinateSystem::cECLIPTIC,
                                   ut::OrbitalState::ReferenceFrame::cECI,
                                   venusOE);
UtOrbitalElements venusOE_Delta1(cSUN,
                                 0.000000000000E+00,
                                 -4.776500000000E-05,
                                 1.021328469575E+03,
                                 1.751786970227E-05,
                                 1.572752142755E-02,
                                 8.745987998742E-03);
// UtOrbitalElements venusOE_Delta2(cSUN, 3.101400000000E-04, 0.000000000000E+00, 9.810000000000E-08,
// -8.800000000000E-07, 4.061800000000E-04, -1.076180000000E-03); UtOrbitalElements
// venusOE_Delta3(UtOrbitalElements::Equinox::cMEAN_OF_DATE,1.500000000000E-08, 0.000000000000E+00, 4.600000000000E-10,
// -7.000000000000E-09, 9.300000000000E-08, -5.678000000000E-06); UtSolarSystem::Planet sVenus(venusOE, venusOE_Delta1,
// venusOE_Delta2, venusOE_Delta3);
UtSolarSystem::Planet sVenus(venusOrbitalState, venusOE_Delta1);

//   UtOrbitalElements
//   earthOE(UtOrbitalElements::Equinox::cECI, 1.4960039046E+11,1.67112300000000E-02,-4.31639169763856E-02,-2.67209908480330E-07,0.00000000000000E+00,1.79660147404916E+00);
//   UtOrbitalElements
//   earthOE_Delta1(UtOrbitalElements::Equinox::cECI, 8.4075200000E+05,-4.39200000000000E-05,6.28301935711515E+02,-2.25962193202098E-04,0.00000000000000E+00,5.64218940290681E-03);
UtOrbitalElements     earthOE(cSUN,
                          1.495980229906E+11,
                          1.670863000000E-02,
                          -4.312518340790E-02,
                          0.000000000000E+00,
                          0.000000000000E+00,
                          1.796595645871E+00);
ut::OrbitalState      earthOrbitalState(ut::OrbitalState::CoordinateSystem::cECLIPTIC,
                                   ut::OrbitalState::ReferenceFrame::cECI,
                                   earthOE);
UtOrbitalElements     earthOE_Delta1(cSUN,
                                 0.000000000000E+00,
                                 -4.203700000000E-05,
                                 6.283019551725E+02,
                                 0.000000000000E+00,
                                 0.000000000000E+00,
                                 3.001157527855E-02);
UtOrbitalElements     earthOE_Delta2(cSUN,
                                 0.000000000000E+00,
                                 -1.267000000000E-07,
                                 -2.681872928614E-06,
                                 0.000000000000E+00,
                                 0.000000000000E+00,
                                 7.974060286512E-06);
UtOrbitalElements     earthOE_Delta3(cSUN,
                                 0.000000000000E+00,
                                 1.400000000000E-10,
                                 -2.792526803191E-09,
                                 0.000000000000E+00,
                                 0.000000000000E+00,
                                 3.141592653590E-09);
UtSolarSystem::Planet sEarth(earthOrbitalState, earthOE_Delta1, earthOE_Delta2, earthOE_Delta3);

// UtOrbitalElements
// marsOE(UtOrbitalElements::Equinox::cECI,2.2794706686E+11,9.33941000000000E-02,3.38422789685103E-01,3.22818576816407E-02,8.64977129749737E-01,-1.28287230097317E+00);
// UtOrbitalElements
// marsOE_Delta1(UtOrbitalElements::Equinox::cECI, 2.7631120000E+06,7.88200000000000E-05,3.34053545248297E+02,-1.41918132000339E-04,-5.10636965735313E-03,1.28628027450385E-02);
UtOrbitalElements     marsOE(cSUN,
                         2.279391851928E+11,
                         9.340065000000E-02,
                         3.381185519184E-01,
                         3.228380895974E-02,
                         8.649518938596E-01,
                         5.000405674463E+00);
ut::OrbitalState      marsOrbitalState(ut::OrbitalState::CoordinateSystem::cECLIPTIC,
                                  ut::OrbitalState::ReferenceFrame::cECI,
                                  marsOE);
UtOrbitalElements     marsOE_Delta1(cSUN,
                                0.000000000000E+00,
                                9.048400000000E-05,
                                3.340534951240E+02,
                                -1.049117413374E-05,
                                1.347561559615E-02,
                                1.865667958590E-02);
UtSolarSystem::Planet sMars(marsOrbitalState, marsOE_Delta1);

// UtOrbitalElements
// jupiterOE(UtOrbitalElements::Equinox::cECI,7.7835189520E+11,4.83862400000000E-02,3.43270671018781E-01,2.27660215304717E-02,1.75360052596995E+00,-1.49654005912288E+00);
// UtOrbitalElements jupiterOE_Delta1(UtOrbitalElements::Equinox::cECI,
// -1.7364072000E+07,-1.32530000000000E-04,5.29626026010713E+01,-3.20641418200884E-05,3.57253294639724E-03,1.36757367935117E-04);
UtOrbitalElements     jupiterOE(cSUN,
                            7.782983621634E+11,
                            4.849793000000E-02,
                            3.494203616765E-01,
                            2.274630018259E-02,
                            1.753434683214E+00,
                            -1.503307935279E+00);
ut::OrbitalState      jupiterOrbitalState(ut::OrbitalState::CoordinateSystem::cECLIPTIC,
                                     ut::OrbitalState::ReferenceFrame::cECI,
                                     jupiterOE);
UtOrbitalElements     jupiterOE_Delta1(cSUN,
                                   2.861807266491E+04,
                                   1.632250000000E-04,
                                   5.296533471383E+01,
                                   -9.593202233587E-05,
                                   1.781941721845E-02,
                                   1.032637665511E-02);
UtSolarSystem::Planet sJupiter(jupiterOrbitalState, jupiterOE_Delta1);

// UtOrbitalElements
// saturnOE(UtOrbitalElements::Equinox::cECI,1.4266867206E+12,5.38617900000000E-02,-7.44289273004179E-01,4.33887433093106E-02,1.98378354297539E+00,-3.67628232812339E-01);
// UtOrbitalElements saturnOE_Delta1(UtOrbitalElements::Equinox::cECI,
// -1.8708976000E+08,-5.09910000000000E-04,2.13372700330546E+01,3.37911451149368E-05,-5.03838053087461E-03,4.30713618170866E-03);
UtOrbitalElements     saturnOE(cSUN,
                           1.429394069855E+12,
                           5.554814000000E-02,
                           -7.501388996756E-01,
                           4.343913323374E-02,
                           1.983837273285E+00,
                           -3.596820948268E-01);
ut::OrbitalState      saturnOrbitalState(ut::OrbitalState::CoordinateSystem::cECLIPTIC,
                                    ut::OrbitalState::ReferenceFrame::cECI,
                                    saturnOE);
UtOrbitalElements     saturnOE_Delta1(cSUN,
                                  -3.199898454273E+05,
                                  -3.466410000000E-04,
                                  2.132002248126E+01,
                                  -6.520899151301E-05,
                                  1.530807342973E-02,
                                  1.896602697851E-02);
UtSolarSystem::Planet sSaturn(saturnOrbitalState, saturnOE_Delta1);

// UtOrbitalElements
// uranusOE(UtOrbitalElements::Equinox::cECI,2.8706990301E+12,4.72574400000000E-02,2.48332127460648E+00,1.34850740589641E-02,1.29183904397530E+00,1.69187594782380E+00);
// UtOrbitalElements uranusOE_Delta1(UtOrbitalElements::Equinox::cECI,
// -2.9347929600E+08,-4.39700000000000E-05,7.47130119055725E+00,-4.24008543150248E-05,7.40122402738534E-04,6.38174265377627E-03);
UtOrbitalElements     uranusOE(cSUN,
                           2.875038609038E+12,
                           4.638122000000E-02,
                           2.461781918296E+00,
                           1.349483341654E-02,
                           1.291647615739E+00,
                           1.727864335582E+00);
ut::OrbitalState      uranusOrbitalState(ut::OrbitalState::CoordinateSystem::cECLIPTIC,
                                    ut::OrbitalState::ReferenceFrame::cECI,
                                    uranusOE);
UtOrbitalElements     uranusOE_Delta1(cSUN,
                                  -5.565040790040E+03,
                                  -2.729300000000E-05,
                                  7.476600907440E+00,
                                  1.351582972744E-05,
                                  9.095395933675E-03,
                                  1.684681154883E-02);
UtSolarSystem::Planet sUranus(uranusOrbitalState, uranusOE_Delta1);

// UtOrbitalElements
// neptuneOE(UtOrbitalElements::Equinox::cECI,4.4984604449E+12,8.59048000000000E-03,-1.74680915087554E+00,3.08930864549253E-02,2.30006864135445E+00,-1.51528549236643E+00);
// UtOrbitalElements
// neptuneOE_Delta1(UtOrbitalElements::Equinox::cECI,3.9331336000E+07,5.10500000000000E-05,3.81846393834374E+00,6.17357863015431E-06,-8.87786158636439E-05,-5.53841840876854E-03);
UtOrbitalElements     neptuneOE(cSUN,
                            4.504449761556E+12,
                            9.455750000000E-03,
                            4.472029025131E+00,
                            3.089150745555E-02,
                            2.300065696286E+00,
                            -1.460208443117E+00);
ut::OrbitalState      neptuneOrbitalState(ut::OrbitalState::CoordinateSystem::cECLIPTIC,
                                     ut::OrbitalState::ReferenceFrame::cECI,
                                     neptuneOE);
UtOrbitalElements     neptuneOE_Delta1(cSUN,
                                   -2.487812589741E+04,
                                   6.033000000000E-06,
                                   3.812794159649E+00,
                                   -1.624587374341E-04,
                                   1.923708708332E-02,
                                   5.656468988715E-03);
UtSolarSystem::Planet sNeptune(neptuneOrbitalState, neptuneOE_Delta1);

ut::Moon cMOON;

class Moon : public UtSolarSystem::Body
{
public:
   Moon()
      : UtSolarSystem::Body()
   {
   }

   Moon* Clone() const override { return new Moon(*this); }

   void GetLocationEcliptic(const UtCalendar& aEpoch, UtVec3d& aLocationEcliptic) override
   {
      MoonPropagator::GetLocation(aEpoch, aLocationEcliptic);
   }

   static void GetLocationEclipticS(const UtCalendar& aEpoch, UtVec3d& aLocationEcliptic)
   {
      MoonPropagator::GetLocation(aEpoch, aLocationEcliptic);
   }

   static void GetLocationVelocityEcliptic(const UtCalendar& aEpoch, UtVec3d& aLocationEcliptic, UtVec3d& aVelocityEcliptic)
   {
      MoonPropagator::GetLocationVelocity(aEpoch, aLocationEcliptic, aVelocityEcliptic);
   }

   bool Initialize(const UtCalendar& aEpoch) override { return mPropagator.Initialize(aEpoch); }

   void Update(const UtCalendar& aEpoch) override { mPropagator.Update(aEpoch); }

   const ut::OrbitalState& GetOrbitalState() const override { return mPropagator.GetOrbitalState(); }
   //! Define the lunar propagation.
   class MoonPropagator : public UtOrbitalPropagatorBase
   {
   public:
      MoonPropagator()
         : UtOrbitalPropagatorBase(ut::make_unique<ut::OrbitalState>(ut::EarthWGS84{},
                                                                     ut::OrbitalState::CoordinateSystem::cECLIPTIC,
                                                                     ut::OrbitalState::ReferenceFrame::cECI))
         , mPropagatedOrbitalState(ut::EarthWGS84{},
                                   ut::OrbitalState::CoordinateSystem::cECLIPTIC,
                                   ut::OrbitalState::ReferenceFrame::cECI)
      {
      }
      UtOrbitalPropagatorBase* Clone() const override { return new MoonPropagator(*this); }
      bool                     Initialize(const UtCalendar& aTime) override
      {
         bool ok = UtOrbitalPropagatorBase::Initialize(aTime);
         return ok && InitializeP(aTime);
      }
      static void GetLocationVelocity(const UtCalendar& aTime, UtVec3d& aLocationEcliptic, UtVec3d& aVelocityEcliptic)
      {
         GetLocation(aTime, aLocationEcliptic);

         UtVec3d    locationEclipticNext;
         UtCalendar timeNext = aTime;
         timeNext.AdvanceTimeBy(1.0);
         GetLocation(timeNext, locationEclipticNext);
         UtVec3d deltaLoc;
         deltaLoc.Subtract(locationEclipticNext, aLocationEcliptic);
         aVelocityEcliptic.Set(deltaLoc); // divided by 1 sec
      }
      static void GetLocation(const UtCalendar& aTime, UtVec3d& aLocation)
      {
         double daysSinceJ2000 = aTime.GetJ2000TT_Date();

         // Find perturbations (only compute initial quantities up to second order,
         // although higher precision is available for more precise computations).
         double T  = daysSinceJ2000 / 36525.0; // in centuries
         double T2 = T * T;
         double T3 = T2 * T;
         double T4 = T3 * T;

         // Longitude of the Moon:
         double Lm = UtMath::NormalizeAngle0_360(218.3164477 + 481267.88123421 * T - 0.0015786 * T2 + T3 / 538841.0 -
                                                 T4 / 65194000.0);
         // Mean elongation of the Moon:
         double D = UtMath::NormalizeAngle0_360(297.8501921 + 445267.1114034 * T - 0.0018819 * T2 + T3 / 545868.0 -
                                                T4 / 113065000);
         // Mean anomaly of the Sun:
         double Ms = UtMath::NormalizeAngle0_360(357.5291092 + 35999.0502909 * T - 0.0001536 * T2 - T3 * 0.0001536);
         // Mean anomaly of the Moon:
         double Mm =
            UtMath::NormalizeAngle0_360(134.9633964 + 477198.8675055 * T - 0.0087414 * T2 + T3 / 69699 - T4 / 14712000);
         // Argument of Latitude of the Moon:
         double F = UtMath::NormalizeAngle0_360(93.2720950 + 483202.0175233 * T - 0.0036539 * T2 - T3 / 3526000 +
                                                T4 / 863310000.0);

         Lm *= UtMath::cRAD_PER_DEG;
         D *= UtMath::cRAD_PER_DEG;
         Ms *= UtMath::cRAD_PER_DEG;
         Mm *= UtMath::cRAD_PER_DEG;
         F *= UtMath::cRAD_PER_DEG;

         // Eccentricity term to be used to multiply by the same power as Ms
         double E  = 1.0 - 0.002516 * T - 0.0000074 * T2;
         double E2 = E * E; // E squared

         static const double DM1[60]  = {0, 2, 2, 0, 0, 0, 2, 2, 2, 2, 0, 1, 0, 2, 0, 0, 4, 0, 4, 2,
                                        2, 1, 1, 2, 2, 4, 2, 0, 2, 2, 1, 2, 0, 0, 2, 2, 2, 4, 0, 3,
                                        2, 4, 0, 2, 2, 2, 4, 0, 4, 1, 2, 0, 1, 3, 4, 2, 0, 1, 2, 2};
         static const double MsM1[60] = {0, 0,  0, 0,  1, 0,  0, -1, 0,  -1, 1, 0,  1, 0, 0,  0,  0, 0,  0, 1,
                                         1, 0,  1, -1, 0, 0,  0, 1,  0,  -1, 0, -2, 1, 2, -2, 0,  0, -1, 0, 0,
                                         1, -1, 2, 2,  1, -1, 0, 0,  -1, 0,  1, 0,  1, 0, 0,  -1, 2, 1,  0, 0};
         static const double MmM1[60] = {1, -1, 0,  2,  0,  0, -2, -1, 1,  0,  -1, 0, 1, 0,  1,  1, -1, 3,  -2, -1,
                                         0, -1, 0,  1,  2,  0, -3, -2, -1, -2, 1,  0, 2, 0,  -1, 1, 0,  -1, 2,  -1,
                                         1, -2, -1, -1, -2, 0, 1,  4,  0,  -2, 0,  2, 1, -2, -3, 2, 1,  -1, 3,  -1};
         static const double FM1[60]  = {0, 0, 0, 0, 0, 2,  0, 0, 0, 0, 0,  0,  0, -2, 2, -2, 0, 0, 0, 0,
                                        0, 0, 0, 0, 0, 0,  0, 0, 2, 0, 0,  0,  0, 0,  0, -2, 2, 0, 2, 0,
                                        0, 0, 0, 0, 0, -2, 0, 0, 0, 0, -2, -2, 0, 0,  0, 0,  0, 0, 0, -2};

         static const double DM2[60]  = {0, 0, 0, 2, 2, 2, 2, 0, 2, 0, 2, 2, 2, 2, 2, 2, 2, 0, 4, 0,
                                        0, 0, 1, 0, 0, 0, 1, 0, 4, 4, 0, 4, 2, 2, 2, 2, 0, 2, 2, 2,
                                        2, 4, 2, 2, 0, 2, 1, 1, 0, 2, 1, 2, 0, 4, 4, 1, 4, 1, 4, 2};
         static const double MsM2[60] = {0, 0, 0,  0,  0, 0, 0, 0, 0, 0, -1, 0,  0, 1, -1, -1, -1, 1, 0,  1,
                                         0, 1, 0,  1,  1, 1, 0, 0, 0, 0, 0,  0,  0, 0, -1, 0,  0,  0, 0,  1,
                                         1, 0, -1, -2, 0, 1, 1, 1, 1, 1, 0,  -1, 1, 0, -1, 0,  0,  0, -1, -2};
         static const double MmM2[60] = {0, 1,  1, 0, -1, -1, 0, 2, 1,  2,  0, -2, 1, 0,  -1, 0,  -1, -1, -1, 0,
                                         0, -1, 0, 1, 1,  0,  0, 3, 0,  -1, 1, -2, 0, 2,  1,  -2, 3,  2,  -3, -1,
                                         0, 0,  1, 0, 1,  1,  0, 0, -2, -1, 1, -2, 2, -2, -1, 1,  1,  -1, 0,  0};
         static const double FM2[60]  = {1, 1, -1, -1, 1,  -1, 1,  1, -1, -1, -1, -1, 1,  -1, 1,  1,  -1, -1, -1, 1,
                                        3, 1, 1,  1,  -1, -1, -1, 1, -1, 1,  -3, 1,  -3, -1, -1, 1,  -1, 1,  -1, 1,
                                        1, 1, 1,  -1, 3,  -1, -1, 1, -1, -1, 1,  -1, 1,  -1, -1, -1, -1, -1, -1, 1};

         static const double LonC[60] = {6288774, 1274027, 658314, 213618, -185116, -114332, 58793, 57066, 53322, 45758,
                                         -40923,  -34720,  -30383, 15327,  -12528,  10980,   10675, 10034, 8548,  -7888,
                                         -6766,   -5163,   4987,   4036,   3994,    3861,    3665,  -2689, -2602, 2390,
                                         -2348,   2236,    -2120,  -2069,  2048,    -1773,   -1595, 1215,  -1110, -892,
                                         -810,    759,     -713,   -700,   691,     596,     549,   537,   520,   -487,
                                         -399,    -381,    351,    -340,   330,     327,     -323,  299,   294,   0};
         static const double RadiusC[60] = {-20905355, -3699111, -2955968, -569925, 48888,  -3149, 246158, -152138,
                                            -170733,   -204586,  -129620,  108743,  104755, 10321, 0,      79661,
                                            -34782,    -23210,   -21636,   24208,   30824,  -8379, -16675, -12831,
                                            -10445,    -11650,   14403,    -7003,   0,      10056, 6322,   -9884,
                                            5751,      0,        -4950,    4130,    0,      -3958, 0,      3258,
                                            2616,      -1897,    -2117,    2354,    0,      0,     -1423,  -1117,
                                            -1571,     -1739,    0,        -4421,   0,      0,     0,      0,
                                            1165,      0,        0,        8752};
         static const double LatC[60]    = {5128122, 280602, 277693, 173237, 55413, 46271, 32573, 17198, 9266, 8822,
                                         8216,    4324,   4200,   -3359,  2463,  2211,  2065,  -1870, 1828, -1794,
                                         -1749,   -1565,  -1491,  -1475,  -1410, -1344, -1335, 1107,  1021, 833,
                                         777,     671,    607,    596,    491,   -451,  439,   422,   421,  -366,
                                         -351,    331,    315,    302,    -283,  -229,  223,   223,   -220, -220,
                                         -185,    181,    -177,   176,    166,   -164,  132,   -119,  115,  107};

         double deltaLongitude = 0.0;
         double deltaDistance  = 0.0;
         double latitude       = 0.0;

         for (int i = 0; i < 60; ++i)
         {
            // longitude and distance
            double arg = 0.0;
            if (DM1[i] != 0.0)
            {
               arg += DM1[i] * D;
            }
            if (MsM1[i] != 0.0)
            {
               arg += MsM1[i] * Ms;
            }
            if (MmM1[i] != 0.0)
            {
               arg += MmM1[i] * Mm;
            }
            if (FM1[i] != 0.0)
            {
               arg += FM1[i] * F;
            }
            double sinArg = sin(arg);
            double cosArg = cos(arg);

            double lonTerm    = LonC[i] * sinArg;
            double radiusTerm = RadiusC[i] * cosArg;

            if (MsM1[i] == 1.0)
            {
               lonTerm *= E;
               radiusTerm *= E;
            }
            else if (MsM1[i] == 2.0)
            {
               lonTerm *= E2;
               radiusTerm *= E2;
            }
            deltaLongitude += lonTerm;
            deltaDistance += radiusTerm;

            // latitude
            arg = 0.0;
            if (DM2[i] != 0.0)
            {
               arg += DM2[i] * D;
            }
            if (MsM2[i] != 0.0)
            {
               arg += MsM2[i] * Ms;
            }
            if (MmM2[i] != 0.0)
            {
               arg += MmM2[i] * Mm;
            }
            if (FM2[i] != 0.0)
            {
               arg += FM2[i] * F;
            }
            sinArg = sin(arg);

            double latTerm = LatC[i] * sinArg;

            if (MsM2[i] == 2.0)
            {
               latTerm *= E;
            }
            else if (MsM2[i] == 2.0)
            {
               latTerm *= E2;
            }

            latitude += latTerm;
         }

         // Terms due to the action of Venus (A1), Jupiter (A2), and oblate Earth
         double A1 = UtMath::NormalizeAngle0_TwoPi(UtMath::cRAD_PER_DEG * (119.75 + 131.849 * T));
         double A2 = UtMath::NormalizeAngle0_TwoPi(UtMath::cRAD_PER_DEG * (53.09 + 479264.290 * T));
         double A3 = UtMath::NormalizeAngle0_TwoPi(UtMath::cRAD_PER_DEG * (313.45 + 481266.484 * T));

         deltaLongitude += 3958 * sin(A1) + 1962 * sin(Lm - F) + 318 * sin(A2);
         latitude += -2235 * sin(Lm) + 382 * sin(A3);

         deltaLongitude *= UtMath::cRAD_PER_DEG / 1.0e+6; // Now the result is in radians.
         latitude *= UtMath::cRAD_PER_DEG / 1.0e+6;       // Now the result is in radians.

         UtVec3d locationEcliptic;
         Lm += deltaLongitude;
         double lon          = UtMath::NormalizeAngle0_TwoPi(Lm);
         double sinLon       = sin(lon);
         double cosLon       = cos(lon);
         double sinLat       = sin(latitude);
         double cosLat       = cos(latitude);
         double distance     = 3.8500056e+8 + deltaDistance; // in meters
         locationEcliptic[0] = distance * cosLon * cosLat;
         locationEcliptic[1] = distance * sinLon * cosLat;
         locationEcliptic[2] = distance * sinLat;
         aLocation.Set(locationEcliptic);
      }

   protected:
      void Propagate(const UtCalendar& aTime) override
      {
         GetLocationVelocity(aTime, mLocation, mVelocity);
         ut::OrbitalState::Vector orbitalStateVector(mLocation, mVelocity);
         mPropagatedOrbitalState.Set(aTime, orbitalStateVector);
         SetOrbitalState(mPropagatedOrbitalState);
      }
      void UpdateOrbitalState() override
      {
         SetOrbitalState(mPropagatedOrbitalState); // automatically converts from ecliptic to TOD
      }
      bool HyperbolicPropagationAllowed() const override { return false; }

   private:
      ut::OrbitalState mPropagatedOrbitalState;
      UtVec3d          mLocation;
      UtVec3d          mVelocity;
   };

private:
   MoonPropagator mPropagator;
};

Moon sMoon{};

const UtSolarSystem::Planet* sPlanets[UtSolarSystem::Planet::cNUM_PLANETS] =
   {&sMercury, &sVenus, &sEarth, &sMars, &sJupiter, &sSaturn, &sUranus, &sNeptune};

const std::string sPlanetName[UtSolarSystem::Planet::cNUM_PLANETS] =
   {"Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune"};

} // namespace

UtSolarSystem::UtSolarSystem()
{
   for (unsigned planet = 0; planet < Planet::cNUM_PLANETS; ++planet)
   {
      AddBody(GetPlanetName(planet), ut::clone(sPlanets[planet]));
   }
   AddBody("Moon", ut::clone(sMoon));
}

UtSolarSystem::Body* UtSolarSystem::GetBody(const std::string& aBodyName)
{
   auto iter = mNameToBodyMap.find(aBodyName);
   if (iter != mNameToBodyMap.end())
   {
      return iter->second;
   }
   return nullptr;
}

void UtSolarSystem::AddBody(const std::string& aName, std::unique_ptr<Body> aBody)
{
   mNameToBodyMap[aName] = std::move(aBody);
}

std::unique_ptr<UtSolarSystem::Planet> UtSolarSystem::GetPlanet(int aPlanet)
{
   return ut::clone(sPlanets[aPlanet]);
}

//! A strongly-typed way to ensure we have the right planet name.
//! This method can be used in a call to GetLocationXXX(GetPlanetName(<name>), ...)
const std::string& UtSolarSystem::GetPlanetName(int aPlanet)
{
   return sPlanetName[aPlanet];
}

bool UtSolarSystem::PlanetPropagator::Initialize(const UtCalendar& aEpoch)
{
   return UtOrbitalPropagatorBase::Initialize(aEpoch) && InitializeP(aEpoch);
}

// virtual
void UtSolarSystem::PlanetPropagator::Propagate(const UtCalendar& aEpoch)
{
   double            centuriesSinceEpoch = aEpoch.GetJ2000_Date() / 36525;
   UtOrbitalElements newElements(GetInitialOrbitalState().GetOrbitalElements());
   double            timeToPower = 1.0;
   for (auto& oe : mDeltaOE_Vec)
   {
      timeToPower *= centuriesSinceEpoch;
      newElements.SetSemiMajorAxis(newElements.GetSemiMajorAxis() + oe.GetSemiMajorAxis() * timeToPower);
      newElements.SetEccentricity(newElements.GetEccentricity() + oe.GetEccentricity() * timeToPower);
      newElements.SetMeanAnomaly(
         UtMath::NormalizeAngle0_TwoPi(newElements.GetMeanAnomaly() + oe.GetMeanAnomaly() * timeToPower));
      newElements.SetInclination(newElements.GetInclination() + oe.GetInclination() * timeToPower);
      newElements.SetRAAN(UtMath::NormalizeAngle0_TwoPi(newElements.GetRAAN() + oe.GetRAAN() * timeToPower));
      newElements.SetArgumentOfPeriapsis(UtMath::NormalizeAngle0_TwoPi(newElements.GetArgumentOfPeriapsis() +
                                                                       oe.GetArgumentOfPeriapsis() * timeToPower));
   }

   mPropagatedOrbitalState.Set(newElements);
   SetOrbitalState(mPropagatedOrbitalState);
}

// virtual
void UtSolarSystem::PlanetPropagator::UpdateOrbitalState()
{
   SetOrbitalState(mPropagatedOrbitalState);
}

//! Return the right ascension and declination for a given registered body
//! (currently limited to the planets and Pluto).
bool UtSolarSystem::GetLocationRA_Dec(const std::string& aBodyName, const UtCalendar& aEpoch, double& aRA, double& aDec)
{
   UtVec3d locationECI;
   bool    ok = GetLocationECI(aBodyName, aEpoch, locationECI);
   if (ok)
   {
      UtSphericalEarth::ConvertECIToRA_Dec(locationECI.GetData(), aRA, aDec);
   }
   return ok;
}

//! Return the right ascension and declination for a given registered body
//! (currently limited to the planets and Pluto).
void UtSolarSystem::Body::GetLocationRA_Dec(const UtCalendar& aEpoch, double& aRA, double& aDec)
{
   UtVec3d locationECI;
   GetLocationECI(aEpoch, locationECI);
   UtSphericalEarth::ConvertECIToRA_Dec(locationECI.GetData(), aRA, aDec);
}

//! Return the location of a named body (currently restricted to the planets)
//! in Earth-Centered Inertial coordinates (ECI).
bool UtSolarSystem::GetLocationECI(const std::string& aBodyName, const UtCalendar& aEpoch, UtVec3d& aLocationECI)
{
   // Get the planet's Ecliptic location:
   bool ok = true;

   if (aBodyName != "Earth")
   {
      UtVec3d locationEcliptic;
      ok = GetLocationEcliptic(aBodyName, aEpoch, locationEcliptic);

      // Get the earth's ecliptic location:
      UtVec3d earthLocationEcliptic;

      // TODO optimize for specific bodies:
      GetLocationEcliptic("Earth", aEpoch, earthLocationEcliptic);

      // Subtract
      UtVec3d locationVectorEcliptic;
      locationVectorEcliptic.Subtract(locationEcliptic, earthLocationEcliptic);
      ConvertEclipticToEquatorial(aEpoch, locationVectorEcliptic, aLocationECI);
   }
   else
   {
      aLocationECI.Set(0.0);
   }

   return ok;
}

//! Return the location of a named body (currently restricted to the planets)
//! in Earth-Centered Inertial coordinates (ECI).
void UtSolarSystem::Body::GetLocationECI(const UtCalendar& aEpoch, UtVec3d& aLocationECI)
{
   // Get the planet's Ecliptic location:
   UtVec3d locationEcliptic = {0.0};
   GetLocationEcliptic(aEpoch, locationEcliptic);

   // Get the earth's ecliptic location:
   UtVec3d earthLocationEcliptic;

   Planet earth(sEarth);
   earth.GetLocationEcliptic(aEpoch, earthLocationEcliptic);

   // Subtract
   UtVec3d locationVectorEcliptic;
   locationVectorEcliptic.Subtract(locationEcliptic, earthLocationEcliptic);
   ConvertEclipticToEquatorial(aEpoch, locationVectorEcliptic, aLocationECI);
}

//! Return the sun's location in Earth-Centered Inertial (ECI) coordinates.
// static
void UtSolarSystem::GetSunLocationECI(const UtCalendar& aEpoch, UtVec3d& aLocationECI)
{
   UtVec3d locEcliptic;
   auto    earth = GetPlanet(UtSolarSystem::Planet::cEARTH);
   earth->GetLocationEcliptic(aEpoch, locEcliptic);
   locEcliptic.Multiply(-1.0);
   ConvertEclipticToEquatorial(aEpoch, locEcliptic, aLocationECI);
}

//! A calculation for the moon's location.
//! @note This implementation does not use the propagator in order to provide a fast return
//! from a static method.
void UtSolarSystem::GetMoonLocationECI(const UtCalendar& aEpoch, UtVec3d& aLocationECI)
{
   UtVec3d locEcliptic;
   Moon::GetLocationEclipticS(aEpoch, locEcliptic);
   ConvertEclipticToEquatorial(aEpoch, locEcliptic, aLocationECI);
}

//! A calculation for the moon's location and velocity.
void UtSolarSystem::GetMoonLocationVelocityECI(const UtCalendar& aEpoch, UtVec3d& aLocationECI, UtVec3d& aVelocityECI)
{
   UtVec3d locEcliptic;
   UtVec3d velEcliptic;
   Moon::GetLocationVelocityEcliptic(aEpoch, locEcliptic, velEcliptic);
   ConvertEclipticToEquatorial(aEpoch, locEcliptic, aLocationECI);
   ConvertEclipticToEquatorial(aEpoch, velEcliptic, aVelocityECI);
}

//! Get location in Ecliptic coordinates (i.e., heliocentric, in the plane of the ecliptic,
//! so that the x-axis points toward the vernal equinox, and the z-axis is normal to the plane).
//! @note (10/12 mjm) The referenced ecliptic is wrt to J2000.  When we convert to "ECI"
//! we must understand that it is J2000 referenced, and we may want to convert to a Mean-of-day frame.
//! This work is TODO.
bool UtSolarSystem::GetLocationEcliptic(const std::string& aBodyName, const UtCalendar& aEpoch, UtVec3d& aLocationEcliptic)
{
   auto iter = mNameToBodyMap.find(aBodyName);
   bool ok   = (iter != mNameToBodyMap.end());
   if (ok)
   {
      Body& body = *(iter->second);
      body.GetLocationEcliptic(aEpoch, aLocationEcliptic);
   }
   return ok;
}

// Most generic implementation (also slower).
void UtSolarSystem::Body::GetLocationEcliptic(const UtCalendar& aEpoch, UtVec3d& aLocationEcliptic)
{
   Update(aEpoch);
   aLocationEcliptic = GetOrbitalState().GetOrbitalStateVector().GetLocation();
}

//! Get location in Ecliptic coordinates (i.e., heliocentric, in the plane of the ecliptic,
//! so that the x-axis points toward the vernal equinox, and the z-axis is normal to the plane).

// static
//! Convert from ecliptic coordinates to an equatorial frame by rotating
//! by the obliquity of the ecliptic.  The obliquity is calculated for the given
//! epoch by a fit in UtSun::GetObliquity.
void UtSolarSystem::ConvertEclipticToEquatorial(const UtCalendar& aEpoch,
                                                const UtVec3d&    aLocationEcliptic,
                                                UtVec3d&          aLocationEquatorial)
{
   // Need to rotate by the obliquity to go from the ecliptic frame
   // to an earth-offset inertial.
   double obliquity = UtSun::GetMeanObliquity(aEpoch);
   double sino      = sin(obliquity);
   double coso      = cos(obliquity);

   // x-axis unchanged
   aLocationEquatorial[0] = aLocationEcliptic[0];
   aLocationEquatorial[1] = coso * aLocationEcliptic[1] - sino * aLocationEcliptic[2];
   aLocationEquatorial[2] = sino * aLocationEcliptic[1] + coso * aLocationEcliptic[2];
}

// static
//! Convert from ecliptic coordinates to an equatorial frame by rotating
//! by the obliquity of the ecliptic.  The obliquity is calculated for the given
//! epoch by a fit in UtSun::GetObliquity.
void UtSolarSystem::ConvertEquatorialToEcliptic(const UtCalendar& aEpoch,
                                                const UtVec3d&    aLocationEquatorial,
                                                UtVec3d&          aLocationEcliptic)
{
   // Need to rotate by the obliquity to go from the ecliptic frame
   // to an earth-offset inertial.
   double obliquity = UtSun::GetMeanObliquity(aEpoch);
   double sino      = sin(obliquity);
   double coso      = cos(obliquity);

   // x-axis unchanged
   aLocationEcliptic[0] = aLocationEquatorial[0];
   aLocationEcliptic[1] = coso * aLocationEquatorial[1] + sino * aLocationEquatorial[2];
   aLocationEcliptic[2] = -sino * aLocationEquatorial[1] + coso * aLocationEquatorial[2];
}

UtSolarSystem::Planet::Planet(const ut::OrbitalState&  aOsculatingState,
                              const UtOrbitalElements& aDeltaOE_1, // plus first second and third order deltas
                              const UtOrbitalElements& aDeltaOE_2, // plus first second and third order deltas
                              const UtOrbitalElements& aDeltaOE_3) // plus first second and third order deltas

   : Body()
   , mPropagator(aOsculatingState, aDeltaOE_1, aDeltaOE_2, aDeltaOE_3)
{
}

UtSolarSystem::Planet::Planet(const ut::OrbitalState& aOsculatingState, const UtOrbitalElements& aDeltaOE_1)
   : Body()
   , mPropagator(aOsculatingState, aDeltaOE_1)
{
}

bool UtSolarSystem::Planet::Initialize(const UtCalendar& aEpoch)
{
   return mPropagator.Initialize(aEpoch);
}

void UtSolarSystem::Planet::Update(const UtCalendar& aEpoch)
{
   mPropagator.Update(aEpoch);
}

void UtSolarSystem::Planet::GetLocationEcliptic(const UtCalendar& aEpoch, UtVec3d& aLocationEcliptic)
{
   if (!mInitialized)
   {
      Initialize(aEpoch);
      mInitialized = true;
   }
   Update(aEpoch);
   aLocationEcliptic = static_cast<PlanetPropagator&>(mPropagator).GetOrbitalStateVectorEcliptic().GetLocation();
}

UtSolarSystem::PlanetPropagator::PlanetPropagator(const ut::OrbitalState&  aOsculatingState,
                                                  const UtOrbitalElements& aDeltaOE_1,
                                                  const UtOrbitalElements& aDeltaOE_2,
                                                  const UtOrbitalElements& aDeltaOE_3)
   : UtOrbitalPropagatorBase(ut::clone(aOsculatingState))
   , mPropagatedOrbitalState(aOsculatingState)
{
   mDeltaOE_Vec.push_back(aDeltaOE_1);
   mDeltaOE_Vec.push_back(aDeltaOE_2);
   mDeltaOE_Vec.push_back(aDeltaOE_3);
}

UtSolarSystem::PlanetPropagator::PlanetPropagator(const ut::OrbitalState&  aOsculatingState,
                                                  const UtOrbitalElements& aDeltaOE_1)
   : UtOrbitalPropagatorBase(ut::clone(aOsculatingState))
   , mPropagatedOrbitalState(aOsculatingState)
{
   mDeltaOE_Vec.push_back(aDeltaOE_1);
}

//**********************************************************
// Following is a unit test (TODO augment with daily test).
//#define UNIT_TEST_UTSOLARSYSTEM
#ifdef UNIT_TEST_UTSOLARSYSTEM

#include <sstream>

void GetHMS(double aRA, double aDec, string& aSRA, string& aSDec)
{
   double        rd = aRA / 15.0;
   double        rm = (rd - (int)rd) * 60.0;
   double        rs = (rm - (int)rm) * 60.0;
   int           dd = ((int)aDec);
   double        dm = fabs((aDec - dd) * 60.0);
   double        ds = fabs((dm - (int)dm) * 60.0);
   ostringstream oss;
   oss << (int)rd << ":" << (int)rm << ":" << rs << ends;
   aSRA = oss.str();
   oss.str("");
   oss << dd << ":" << (int)dm << ":" << ds << ends;
   aSDec = oss.str();
}

void PrintECIData(const UtVec3d& aLocECI, const string& aName)
{
   double ra, dec;
   UtSphericalEarth::ConvertECIToRA_Dec(aLocECI.GetData(), ra, dec);
   ra *= UtMath::cDEG_PER_RAD;
   dec *= UtMath::cDEG_PER_RAD;

   string sRA, sDec;
   GetHMS(ra, dec, sRA, sDec);

   auto out = ut::log::info() << "ECI Data:";
   out.AddNote() << "Name: " << aName;
   out.AddNote() << "Right Ascension(degrees): " << sRA;
   out.AddNote() << "Declination: " << sDec;
   out.AddNote() << "Radius(Astronomical Units): " << UtVec3d::Magnitude(aLocECI) / 1.496E+11;
}
#include "ut_export.h"

int UT_EXPORT main(int argc, char* argv[])
{
   // This test loops through the planets and determines their ra and dec for
   // Jan 1 2004.  This correlates with the results from the following webpage (table 3):
   // http://www.astro.uu.nl/~strous/AA/en/reken/hemelpositie.html
   // Table 3: Planet Positions on 1-1-2004
   /**
      Name  right ascension  declination distance
            degrees  time    degrees     AU
      Sun      280.710  18h42m50s  -23.074  0.98331
      Mercury  268.693  17h54m46s  -20.296  0.70403
      Venus    316.189  21h04m45s  -18.614  1.3061
      Mars     8.335    0h33m20s    +3.660  1.1115
      Jupiter  170.120  11h20m29s   +5.567  4.9716
      Saturn   100.256  6h41m01s   +22.420  8.0443
      Uranus   333.148  22h12m36s  -11.868  20.654
      Neptune  313.525  20h54m06s  -17.459  30.973
      Pluto    260.277  17h21m07s  -14.497  31.700
   */
   // Accuracy is within one degree (these results should be better than those on the  webpage.
   int year = 2004, month = 1, day = 1;

   if (argc == 4)
   {
      year  = atoi(argv[1]);
      month = atoi(argv[2]);
      day   = atoi(argv[3]);
   }

   UtSolarSystem solarSystem;
   UtCalendar    test;
   test.SetDate(year, month, day);
   test.SetTime(0, 0, 0);
   UtVec3d locECI;
   solarSystem.GetSunLocationECI(test, locECI);
   PrintECIData(locECI, "Sun");
   for (int planetNum = UtSolarSystem::Planet::cMERCURY; planetNum < UtSolarSystem::Planet::cNUM_PLANETS; ++planetNum)
   {
      if (planetNum != UtSolarSystem::Planet::cEARTH)
      {
         solarSystem.GetLocationECI(solarSystem.GetPlanetName(planetNum), test, locECI);
         PrintECIData(locECI, UtSolarSystem::GetPlanetName(planetNum));
      }
   }

   UtVec3d locationEcliptic;

   UtSolarSystem::Body* moon = solarSystem.GetBody("Moon");
   UtCalendar           test2;
   test2.SetDate(1992, 4, 12); // April 12 1992
   test2.SetTime(0, 0, 0.0);
   moon->Update(test2);
   moon->GetLocationEcliptic(test2, locationEcliptic);
   double distance = locationEcliptic.Magnitude();
   double lonEcl   = atan2(locationEcliptic[1], locationEcliptic[0]);
   double latEcl   = asin(locationEcliptic[2] / distance);
   lonEcl *= UtMath::cDEG_PER_RAD;
   latEcl *= UtMath::cDEG_PER_RAD;

   auto out = ut::log::info() << "Moon Position:";
   out.AddNote() << "Lon: " << UtMath::NormalizeAngle0_360(lonEcl);
   out.AddNote() << "Lat: " << latEcl;
   out.AddNote() << "R: " << distance / 1000.0;

   // Test case from Meeus' Astronomical Algorithms 2nd ed. p. 342-343:
   // Compare vs. lat, lon, r: 133.162655 -3.229126 368409.7
}
#endif
