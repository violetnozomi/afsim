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

#include "UtNoise.hpp"

#include <math.h>

#include "UtMath.hpp"
#include "UtRandom.hpp"

UtNoise::UtNoise(ut::Random& aRandom, const unsigned int aBaseFrequency, const unsigned int aOctaves)
   : mRandom(aRandom)
   , mRandomValues(nullptr)
   , mNumOctaves(aOctaves)
   , mBaseOctaveFrequency(aBaseFrequency)
   , mOctaveLimit(aOctaves)
{
   Regenerate();
}

UtNoise::~UtNoise()
{
   if (mRandomValues != nullptr)
   {
      delete[] mRandomValues;
   }
}

void UtNoise::Regenerate()
{
   mHighestFrequency = 1 + ((mBaseOctaveFrequency + 1) << (mNumOctaves - 1));
   // first clean up any existing values
   if (mRandomValues != nullptr)
   {
      delete[] mRandomValues;
   }
   // allocate memory for new values

   mRandomValues = new float[mHighestFrequency * mHighestFrequency * mHighestFrequency];
   for (unsigned int i = 0; i < mHighestFrequency - 1; ++i)
   {
      for (unsigned int j = 0; j < mHighestFrequency - 1; ++j)
      {
         unsigned int k;
         for (k = 0; k < mHighestFrequency - 1; ++k)
         {
            float rv                        = mRandom.Uniform<float>();
            mRandomValues[Address(i, j, k)] = rv;
            // ensure that the function is loopable
            if (i == 0)
            {
               mRandomValues[Address(mHighestFrequency - 1, j, k)] = rv;
               if (j == 0)
               {
                  mRandomValues[Address(i, mHighestFrequency - 1, k)]                     = rv;
                  mRandomValues[Address(mHighestFrequency - 1, mHighestFrequency - 1, k)] = rv;
                  if (k == 0)
                  {
                     mRandomValues[Address(i, j, mHighestFrequency - 1)]                                         = rv;
                     mRandomValues[Address(mHighestFrequency - 1, j, mHighestFrequency - 1)]                     = rv;
                     mRandomValues[Address(i, mHighestFrequency - 1, mHighestFrequency - 1)]                     = rv;
                     mRandomValues[Address(mHighestFrequency - 1, mHighestFrequency - 1, mHighestFrequency - 1)] = rv;
                  }
               }
               else if (k == 0)
               {
                  mRandomValues[Address(i, j, mHighestFrequency - 1)]                     = rv;
                  mRandomValues[Address(mHighestFrequency - 1, j, mHighestFrequency - 1)] = rv;
               }
            }
            else if (j == 0)
            {
               mRandomValues[Address(i, mHighestFrequency - 1, k)] = rv;
               if (k == 0)
               {
                  mRandomValues[Address(i, j, mHighestFrequency - 1)]                     = rv;
                  mRandomValues[Address(i, mHighestFrequency - 1, mHighestFrequency - 1)] = rv;
               }
            }
            else if (k == 0)
            {
               mRandomValues[Address(i, j, mHighestFrequency - 1)] = rv;
            }
         }
      }
   }
}

//! Query a single value from the noise function.
//! Every address component should be between zero and one
float UtNoise::QueryValue(const float aX, const float aY, const float aZ)
{
   float x = aX;
   while (x >= 1.0)
   {
      x -= 1.0;
   }

   float y = aY;
   while (y >= 1.0)
   {
      y -= 1.0;
   }

   float z = aZ;
   while (z >= 1.0)
   {
      z -= 1.0;
   }

   float retval = 0.0F;
   float factor = 1.0F;
   for (unsigned int octave = 0; octave < mOctaveLimit; ++octave)
   {
      factor *= 0.5F;
      unsigned int frequency = 1 + ((mBaseOctaveFrequency + 1) << octave);
      retval += factor * QuerySingleOctave(x, y, z, frequency, 1 << (mNumOctaves - octave - 1));
   }
   return retval / (1 - factor);
}

float UtNoise::QuerySingleOctave(const float        aX,
                                 const float        aY,
                                 const float        aZ,
                                 const unsigned int aFrequency,
                                 const unsigned int aHighestFrequencyFactor)
{
   unsigned int xLeft      = (int)(aX * (aFrequency - 1)); // left int address for the octave
   unsigned int yDown      = (int)(aY * (aFrequency - 1)); // down int address for the octave
   unsigned int zNear      = (int)(aZ * (aFrequency - 1)); // near int address for the octave
   unsigned int xRight     = xLeft + 1;                    // right int address for the octave
   unsigned int yUp        = yDown + 1;                    // up int address for the octave
   unsigned int zFar       = zNear + 1;                    // far int address for the octave
   float        horzFactor = ((aX * (aFrequency - 1)) - xLeft);
   float        vertFactor = ((aY * (aFrequency - 1)) - yDown);
   float        nfFactor   = ((aZ * (aFrequency - 1)) - zNear);
   // calculate the horzFactor and vertFactor fp values
   xLeft *= aHighestFrequencyFactor;
   yDown *= aHighestFrequencyFactor;
   zNear *= aHighestFrequencyFactor;
   xRight *= aHighestFrequencyFactor;
   yUp *= aHighestFrequencyFactor;
   zFar *= aHighestFrequencyFactor; // convert to highest octave addresses

   float LLN = mRandomValues[Address(xLeft, yDown, zNear)];
   float LLF = mRandomValues[Address(xLeft, yDown, zFar)];
   float LRN = mRandomValues[Address(xRight, yDown, zNear)];
   float LRF = mRandomValues[Address(xRight, yDown, zFar)];
   float ULN = mRandomValues[Address(xLeft, yUp, zNear)];
   float ULF = mRandomValues[Address(xLeft, yUp, zFar)];
   float URN = mRandomValues[Address(xRight, yUp, zNear)];
   float URF = mRandomValues[Address(xRight, yUp, zFar)];
   float LL  = Interpolate(LLN, LLF, nfFactor);
   float LR  = Interpolate(LRN, LRF, nfFactor);
   float UL  = Interpolate(ULN, ULF, nfFactor);
   float UR  = Interpolate(URN, URF, nfFactor);

   float LO = Interpolate(LL, LR, horzFactor);
   float HI = Interpolate(UL, UR, horzFactor);

   return Interpolate(LO, HI, vertFactor);
   // return the interpolate between LL and UR using the offset
}

float UtNoise::Interpolate(const float aA, const float aB, const float aF) const
{
   //   float f = 6 * pow(aF, 5) - 15 * pow(aF, 4) + 10 * pow(aF, 3);

   float ft = static_cast<float>(aF * UtMath::cPI);
   float f  = (1.0F - cos(ft)) * 0.5F;
   return aA * (1 - f) + aB * f;
}

unsigned long UtNoise::Address(const unsigned int i, const unsigned int j, const unsigned int k) const
{
   return k + j * mHighestFrequency + i * mHighestFrequency * mHighestFrequency;
}
