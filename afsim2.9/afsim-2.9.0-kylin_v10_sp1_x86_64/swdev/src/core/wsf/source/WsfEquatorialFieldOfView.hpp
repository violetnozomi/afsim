// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef EQUATORIALFIELDOFVIEW_HPP
#define EQUATORIALFIELDOFVIEW_HPP

#include "wsf_export.h"

#include "UtInput.hpp"
#include "UtMath.hpp"
#include "WsfFieldOfView.hpp"

//! A field of view with azimuth-polar (can also be thought roughly of as latitude-longitude) extents with equator
//! oriented to match Earth's projected equator.
class WSF_EXPORT WsfEquatorialFieldOfView : public WsfFieldOfView
{
public:
   WsfEquatorialFieldOfView();

   bool operator==(const WsfEquatorialFieldOfView& aRhs) const;
   bool operator!=(const WsfEquatorialFieldOfView& aRhs) const;

   bool        ProcessInput(UtInput& aInput) override;
   const char* GetScriptClassName() const override { return "WsfEquatorialFieldOfView"; }

   WsfFieldOfView* Clone() const override { return new WsfEquatorialFieldOfView(*this); }

   bool GetCuedAzElPlatformLat(double& aCuedAz, double& aCuedEl, double& aPlatformLatDeg) const;

   void GetEquatorialFieldOfView(double& aMinEqFOV, double& aMaxEqFOV) const;

   void GetPolarFieldOfView(double& aMinPolarFOV, double& aMaxPolarFOV) const;

   void SetEquatorialFieldOfView(double aMinEqFOV, double aMaxEqFOV);

   void SetPolarFieldOfView(double aMinPolarFOV, double aMaxPolarFOV);

   bool WithinFieldOfView(double aThisToTgtAz, double aThisToTgtEl) override;

   bool WithinFieldOfView(double aThisToTgtAz, double aThisToTgtEl, double aCuedAz, double aCuedEl, double aLatitude) const;

private:
   double mMinEqFOV{-UtMath::cPI};           // radians
   double mMaxEqFOV{UtMath::cPI};            // radians
   double mMinPolarFOV{-UtMath::cPI_OVER_2}; // radians
   double mMaxPolarFOV{UtMath::cPI_OVER_2};  // radians
};
#endif
