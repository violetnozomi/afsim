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

#ifndef P6DOFSIZEFACTOR_H
#define P6DOFSIZEFACTOR_H

#include "p6dof_export.h"

// The SizeFactor is used to indicate the physical size of the object relative
// to its 'nominal' size. For most objects, this is always one (1), but some
// objects, such as parachutes and balloons, can change size and use values
// other than 1 during their 'lifetime'. Size factor is a relative area to the
// initial area to support aerodynamic reference areas. Physical radius is also
// supported via a secondary parameter.
class P6DOF_EXPORT P6DofSizeFactor
{
public:
   P6DofSizeFactor();

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
   bool mSizeFactorEnabled;
   bool mSizeFactorVolumeEnabled;
   bool mSizeFactorAreaEnabled;
   bool mSizeFactorRadiusEnabled;

   // This is used as the "reference" size (the default is 1m)
   double mRefSizeFactorRadius_m;
   double mRefSizeFactorArea_m2;
   double mRefSizeFactorVolume_m3;

   // Max and min size factors (relative to nominal radius)
   double mSizeFactorMin;
   double mSizeFactorMax;

   // Only one of the three 'rates' below may be specified
   // If more than use is specified, the last one specified will be used
   double mSizeFactorVolumeRate_m3_perSec; // Increase size by volume (good for balloons)
   double mSizeFactorAreaRate_m2_perSec;   // Increase size by area (good for parachutes)
   double mSizeFactorRadiusRate_m_perSec;  // Increase size by radius (good for parachutes)

   double mSizeFactorVolume;
   double mSizeFactorArea;
   double mSizeFactorRadius;
   double mCurrentSizeFactorRadius_m;
   double mCurrentSizeFactorArea_m2;
   double mCurrentSizeFactorVolume_m3;
};

#endif
