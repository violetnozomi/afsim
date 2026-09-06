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

#include "WsfEM_BlakeAttenuation.hpp"

#include <iostream>

#include "UtMath.hpp"

// statics used when Atmospheric Losses are computed
double WsfEM_BlakeAttenuation::sElevations[6]  = {0.0, 0.5, 1.0, 2.0, 5.0, 10.0};
double WsfEM_BlakeAttenuation::sFrequencies[7] = {0.1e9, 0.2e9, 0.3e9, 0.6e9, 1.0e9, 3.0e9, 10.0e9};

double WsfEM_BlakeAttenuation::sACoefTable[7][6] = {{.2739, .1881, .1605, .1031, .07371, .04119},
                                                    {.6848, .5533, .4282, .3193, .2158, .1017},
                                                    {1.199, .9917, .7498, .5186, .3029, .1522},
                                                    {2.210, 1.830, 1.314, .9499, .4724, .2512},
                                                    {2.758, 2.177, 1.798, 1.168, .5732, .3007},
                                                    {3.484, 2.592, 1.964, 1.345, .6478, .3408},
                                                    {4.935, 3.450, 2.601, 1.718, .9130, .4420}};

double WsfEM_BlakeAttenuation::sBCoefTable[7][6] = {{.008648, .008644, .01106, .01723, .02313, .04076},
                                                    {.008648, .008644, .01104, .01374, .02213, .04886},
                                                    {.006837, .008795, .01110, .01474, .03116, .05360},
                                                    {.008499, .009737, .01221, .01623, .03677, .07204},
                                                    {.01030, .01223, .01163, .01831, .03927, .08056},
                                                    {.009745, .01225, .01455, .02055, .04500, .08280},
                                                    {.00999, .01340, .01620, .02240, .03750, .08470}};

// =================================================================================================
WsfEM_BlakeAttenuation::WsfEM_BlakeAttenuation() {}

// =================================================================================================
WsfEM_BlakeAttenuation::~WsfEM_BlakeAttenuation() {}

// =================================================================================================
WsfEM_BlakeAttenuation::WsfEM_BlakeAttenuation(const WsfEM_BlakeAttenuation& aSrc)
   : WsfEM_Attenuation(aSrc)
{
}

// =================================================================================================
//! Factory method called by WsfEM_AttenuationTypes.
// static
WsfEM_Attenuation* WsfEM_BlakeAttenuation::ObjectFactory(const std::string& aTypeName)
{
   WsfEM_Attenuation* instancePtr = nullptr;
   if ((aTypeName == "WSF_BLAKE_ATTENUATION") || (aTypeName == "blake"))
   {
      instancePtr = new WsfEM_BlakeAttenuation();
   }
   return instancePtr;
}

// =================================================================================================
// virtual
WsfEM_Attenuation* WsfEM_BlakeAttenuation::Clone() const
{
   return new WsfEM_BlakeAttenuation(*this);
}

// =================================================================================================
// protected virtual
double WsfEM_BlakeAttenuation::ComputeAttenuationFactorP(double aRange, double aElevation, double aAltitude, double aFrequency)
{
   // note: The altitude is assumed to be zero and is not used.
   return sqrt(Calculate2WayAttenuation(aRange, aElevation, aFrequency));
}

// =================================================================================================
// protected
double WsfEM_BlakeAttenuation::Calculate2WayAttenuation(double aRange, double aElevation, double aFrequency)
{
   double frequency;
   double elevation;
   double freqFrac  = 0.0;
   double elFrac    = 0.0;
   int    freqIndex = 0;
   int    elIndex   = 0;

   double lossDB[2][2] = {{0.0, 0.0}, {0.0, 0.0}};

   double loss[2][2] = {{0.0, 0.0}, {0.0, 0.0}};

   // convert range from meters to nm
   double rangeNM = aRange / UtMath::cM_PER_NM;

   // limit range to 300 nm, data is flat beyond this point
   if (rangeNM > 300.0)
   {
      rangeNM = 300.0;
   }

   // convert Elevation into degrees
   elevation = aElevation * UtMath::cDEG_PER_RAD;

   // Frequency is already in hertz
   frequency = aFrequency;

   if (frequency < 0.1E9)
   {
      frequency = 0.10001E9;
   }
   else if (frequency > 10.0E+9)
   {
      frequency = 9.99999E9;
   }

   SearchForValue(frequency, sFrequencies, 7, freqIndex, freqFrac);

   if (elevation <= sElevations[0])
   {
      elevation = sElevations[0];
   }
   else if (elevation >= sElevations[5])
   {
      elevation = sElevations[5];
   }

   SearchForValue(elevation, sElevations, 6, elIndex, elFrac);

   // Set-up the coefficient matrices to be used in the interpolation
   double Acoef[2][2] = {{sACoefTable[freqIndex][elIndex], sACoefTable[freqIndex][elIndex + 1]},
                         {sACoefTable[freqIndex + 1][elIndex], sACoefTable[freqIndex + 1][elIndex + 1]}};

   double Bcoef[2][2] = {{sBCoefTable[freqIndex][elIndex], sBCoefTable[freqIndex][elIndex + 1]},
                         {sBCoefTable[freqIndex + 1][elIndex], sBCoefTable[freqIndex + 1][elIndex + 1]}};

   for (int i = 0; i < 2; ++i)
   {
      for (int j = 0; j < 2; ++j)
      {
         lossDB[i][j] = Acoef[i][j] * (1.0 - exp(-Bcoef[i][j] * rangeNM));
         loss[i][j]   = UtMath::DB_ToLinear(lossDB[i][j]);
      }
   }

   double x1 = loss[0][0] + elFrac * (loss[0][1] - loss[0][0]);
   double x2 = loss[1][0] + elFrac * (loss[1][1] - loss[1][0]);

   double loss2Way = x1 + freqFrac * (x2 - x1);

   return 1.0 / loss2Way;
}

// =================================================================================================
void WsfEM_BlakeAttenuation::SearchForValue(double aVal, double aTable[], int aLength, int& aLowIndex, double& aFrac)
{
   int lo = 0;
   int hi = aLength - 1;

   while (hi > lo + 1)
   {
      int mid = (lo + hi) / 2;

      if (aVal >= aTable[mid])
      {
         lo = mid;
      }
      else
      {
         hi = mid;
      }
   }

   aLowIndex = lo;
   aFrac     = (aVal - aTable[lo]) / (aTable[hi] - aTable[lo]);
}
