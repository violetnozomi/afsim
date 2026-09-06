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

#ifndef WSFSIXDOFSIZEFACTOR_HPP
#define WSFSIXDOFSIZEFACTOR_HPP

#include "wsf_six_dof_export.h"

namespace wsf
{
namespace six_dof
{
// The SizeFactor is used to indicate the physical size of the object relative
// to its 'nominal' size. For most objects, this is always one (1), but some
// objects, such as parachutes and balloons, can change size and use values
// other than 1 during their 'lifetime'. Size factor is a relative area to the
// initial area to support aerodynamic reference areas. Physical radius is also
// supported via a secondary parameter.
class WSF_SIX_DOF_EXPORT SizeFactor
{
public:
   SizeFactor() = default;

   void CalculateSizeFactor(double dT_sec);

   void InitializeValues();

   bool IsEnabled() const { return mSizeFactorEnabled; }

   double GetSizeFactorRadius() const;

   void SetSizeFactorEnabled(bool aEnabled);
   void SetRefSizeFactorRadius_m(double aValue_m);
   void SetSizeFactorMin(double aValue);
   void SetSizeFactorMax(double aValue);

   void SetSizeFactorVolumeRate_m3_perSec(double aVolumeRate_m3_perSec);
   void SetSizeFactorAreaRate_m2_perSec(double aAreaRate_m2_perSec);
   void SetSizeFactorRadiusRate_m_perSec(double aRadiusRate_m_perSec);

protected:
   bool mSizeFactorEnabled       = false;
   bool mSizeFactorVolumeEnabled = false;
   bool mSizeFactorAreaEnabled   = false;
   bool mSizeFactorRadiusEnabled = false;

   // This is used as the "reference" size (the default is 1m)
   double mRefSizeFactorRadius_m  = 1.0;
   double mRefSizeFactorArea_m2   = 0.0;
   double mRefSizeFactorVolume_m3 = 0.0;

   // Max and min size factors (relative to nominal radius)
   double mSizeFactorMin = 1.0;
   double mSizeFactorMax = 1.0;

   // Only one of the three 'rates' below may be specified
   // If more than use is specified, the last one specified will be used
   double mSizeFactorVolumeRate_m3_perSec = 0.0; // Increase size by volume (good for balloons)
   double mSizeFactorAreaRate_m2_perSec   = 0.0; // Increase size by area (good for parachutes)
   double mSizeFactorRadiusRate_m_perSec  = 0.0; // Increase size by radius (good for parachutes)

   double mSizeFactorVolume           = 1.0;
   double mSizeFactorArea             = 1.0;
   double mSizeFactorRadius           = 1.0;
   double mCurrentSizeFactorRadius_m  = 0.0;
   double mCurrentSizeFactorArea_m2   = 0.0;
   double mCurrentSizeFactorVolume_m3 = 0.0;
};
} // namespace six_dof
} // namespace wsf

#endif
