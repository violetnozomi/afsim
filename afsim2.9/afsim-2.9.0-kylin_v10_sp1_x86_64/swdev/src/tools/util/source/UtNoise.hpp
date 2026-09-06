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

#ifndef UTNOISE_HPP
#define UTNOISE_HPP

#include "ut_export.h"

#include <list>

#include "UtRandom.hpp"

//! An implementation of Perlin Noise based on GPU Gems, Chapter 5, Robert Perlin, 2003
//!
//! Provides a repeatable pseudo-random signal over N-dimensions that is isotropic

class UT_EXPORT UtNoise
{
public:
   //! Create a noise object.
   UtNoise(ut::Random& aRandom, const unsigned int aBaseFrequency, const unsigned int aOctaves);

   //! Destroy a noise object.
   virtual ~UtNoise();

   unsigned int GetOctaves() const { return mNumOctaves; }
   unsigned int GetFrequency() const { return mBaseOctaveFrequency; }

   void SetOctaves(const unsigned int aOctaves)
   {
      mNumOctaves  = aOctaves;
      mOctaveLimit = aOctaves;
   }
   void SetFrequency(const unsigned int aFrequency) { mBaseOctaveFrequency = aFrequency; }

   void Regenerate();

   //! Query a single value from the noise function.
   //! Every aX and aY should be between zero and one
   //! Return will be between 0 and 1
   virtual float QueryValue(const float aX, const float aY, const float aZ);

   //! Limit queries to only chech the first n octaves.
   //! This is just for demonstration purposes
   virtual void LimitOctaves(const unsigned int aOctaves)
   {
      mOctaveLimit = (aOctaves < mNumOctaves) ? aOctaves : mNumOctaves;
   }

protected:
   //! Assignment operator declared by not defined to prevent use.
   UtNoise& operator=(const UtNoise& aRhs) = delete;

private:
   float QuerySingleOctave(const float        aX,
                           const float        aY,
                           const float        aZ,
                           const unsigned int aFrequency,
                           const unsigned int aHighestFrequencyFactor);

   virtual float Interpolate(const float aA, const float aB, const float aF) const;

   unsigned long Address(const unsigned int i, const unsigned int j, const unsigned int k) const;

   ut::Random mRandom;
   // This contains an array of random values
   float* mRandomValues;
   // The number of octaves.  Each additional octave will add higher frequency noise [1, n]
   unsigned int mNumOctaves;
   // The base octaves frequency. [1, n]
   unsigned int mBaseOctaveFrequency;
   // This is the number of random values made for each dimension
   unsigned int mHighestFrequency;

   unsigned int mOctaveLimit;
};

#endif
