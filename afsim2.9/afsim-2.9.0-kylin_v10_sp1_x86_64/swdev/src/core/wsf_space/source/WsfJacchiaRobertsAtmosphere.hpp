// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFJACCHIAROBERTSATMOSPHERE_HPP
#define WSFJACCHIAROBERTSATMOSPHERE_HPP

#include "wsf_space_export.h"

#include "UtCalendar.hpp"
#include "UtVec3.hpp"
#include "WsfAtmosphere.hpp"

namespace wsf
{
namespace space
{

//! The Jacchia-Roberts atmsphere models the exospheric temperature profile, and from that
//! derives the density. This includes effects of solar and geomagnetic activity, as well
//! as diurnal variations.
class WSF_SPACE_EXPORT JacchiaRobertsAtmosphere : public Atmosphere
{
public:
   JacchiaRobertsAtmosphere();
   JacchiaRobertsAtmosphere(const JacchiaRobertsAtmosphere& aOther) = default;
   ~JacchiaRobertsAtmosphere() override                             = default;

   JacchiaRobertsAtmosphere* Clone() const override { return new JacchiaRobertsAtmosphere{*this}; }
   bool                      ProcessInput(UtInput& aInput) override;

   //! Get the average 10.7 cm solar flux (in 10^-22 W/m^2/Hz).
   double GetAverageSolarFlux() const { return mF107a; }

   //! Get the 10.7 cm solar flux (in 10^-22 W/m^2/Hz).
   double GetSolarFlux() const { return mF107; }

   //! Get the geomagnetic index, K_p.
   double GetGeomagneticIndex() const { return mKp; }

   //! Set the average 10.7 cm solar flux (in 10^-22 W/m^2/Hz).
   void SetAverageSolarFlux(double aF107a) { mF107a = aF107a; }

   //! Set the 10.7 cm solar flux (in 10^-22 W/m^w/Hz).
   void SetSolarFlux(double aF107) { mF107 = aF107; }

   //! Set the geomagnetic index, K_p.
   void SetGeomagneticIndex(double aKp) { mKp = aKp; }

   const char* GetScriptClassName() const override { return "WsfJacchiaRobertsAtmosphere"; }

   double GetDensity(const UtCalendar& aEpoch, const UtLLAPos& aLLA) const override;

   static constexpr const char* cTYPE{"WSF_JACCHIA_ROBERTS_ATMOSPHERE"};

private:
   double mF107a{150.0};
   double mF107{150.0};
   double mKp{0.0};

   // Various intermediate quantities used during density evaluation.
   mutable double mRoot1{};
   mutable double mRoot2{};
   mutable double mX_Root{};
   mutable double mY_Root{};
   mutable double mTinfinity{};
   mutable double mTx{};
   mutable double mSum{};
   mutable double mX_Temp{};
   mutable bool   mLowAltWarned{false};

   double      GetPolarRadiusKm() const;
   double      GetPolarRadiusKmSquared() const;
   double      JacchiaRoberts(const UtLLAPos& aLLA, const UtCalendar& aEpoch) const;
   double      Exotherm(const UtLLAPos& aLLA, double aSunDec, double aSolarLon) const;
   double      Rho100(double aHeightKm, double aTemperature) const;
   double      Rho125(double aHeightKm, double aTemperature) const;
   double      RhoCorrection(double aHeightKm, double aA1_Time, double aGeoLat) const;
   double      RhoHigh(double aHeightKm, double aTemperature, double aT_500, double aSunDec, double aGeoLat) const;
   static void Roots(double aA[], int aNa, double aCroots[][2], int aIrl);
   static void DeflatePolynomial(double aC[], int aN, double aRoot, double aCnew[]);
};

} // namespace space
} // namespace wsf

#endif // WSFJACCHIAROBERTSATMOSPHERE_HPP
