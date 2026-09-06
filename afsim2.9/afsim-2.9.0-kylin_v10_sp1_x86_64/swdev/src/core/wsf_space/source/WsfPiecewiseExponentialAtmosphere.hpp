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

#ifndef WSFPIECEWISEEXPONENTIALATMOSPHERE_HPP
#define WSFPIECEWISEEXPONENTIALATMOSPHERE_HPP

#include "wsf_space_export.h"

#include <array>

#include "WsfAtmosphere.hpp"

namespace wsf
{
namespace space
{

//! A simple exponential atmosphere, defined on piecewise segments of altitude.
//!
//! This model uses a simple exponential profile in one of a number of segments in
//! altitude. This models does not depend on latitude or longitude, and only provides
//! the density. Details about the model can be found in Fundamentals of Astrodynamics and
//! Applications, Fourth Edition, by David A. Vallado, on pages 565-568.
class WSF_SPACE_EXPORT PiecewiseExponentialAtmosphere : public Atmosphere
{
public:
   PiecewiseExponentialAtmosphere();
   PiecewiseExponentialAtmosphere(const PiecewiseExponentialAtmosphere& aOther) = default;
   ~PiecewiseExponentialAtmosphere() override                                   = default;

   PiecewiseExponentialAtmosphere* Clone() const override { return new PiecewiseExponentialAtmosphere{*this}; }

   const char* GetScriptClassName() const override { return "WsfPiecewiseExponentialAtmosphere"; }

   double GetDensity(const UtCalendar& aEpoch, const UtLLAPos& aLLA) const override;

   static constexpr const char* cTYPE{"WSF_PIECEWISE_EXPONENTIAL_ATMOSPHERE"};

private:
   struct Row
   {
      Row(double aBaseHeight, double aScaleHeight, double aBaseDensity)
         : mBaseHeight{aBaseHeight}
         , mScaleHeight{aScaleHeight}
         , mBaseDensity{aBaseDensity}
      {
      }

      double mBaseHeight;  // km
      double mScaleHeight; // km
      double mBaseDensity; // kg / m^3
   };

   static const std::array<Row, 28> mTable; //! Data table giving the coefficients in each segment.
};

} // namespace space
} // namespace wsf

#endif // WSFPIECEWISEEXPONENTIALATMOSPHERE_HPP
