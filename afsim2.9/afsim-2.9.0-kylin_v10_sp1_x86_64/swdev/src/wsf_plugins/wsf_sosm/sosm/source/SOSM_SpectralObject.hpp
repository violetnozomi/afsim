// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef SOSM_SPECTRALOBJECT_HPP
#define SOSM_SPECTRALOBJECT_HPP

#include "sosm_export.h"

#include <string>

//! A class that represents 'spectral' object.
//!
//! This class is the base class for a 'spectral object'. A 'spectral object' is a sensor, target
//! or atmospheric model that whose spectral range is represented one of two ways:
//!
//! - A continuous spectrum model.
//! - A discrete spectrum model.
//!
//! In the case of the discrete spectrum model, the range is represented by a set of sample points
//! where each sample is separated by a constant number of wavenumbers. The spectral extent is
//! characterized by three values:
//!
//! - The 'origin', or the wavenumber of the first sample point.
//! - The 'increment' between each sample point.
//! - The 'count' or number of sample points.
//!
//! Within SOSM, the sensor model in always discrete, but the target or atmospheric model may be either.
class SOSM_EXPORT SOSM_SpectralObject
{
public:
   //! Return value from 'UnionOf'.
   enum Status
   {
      cOK = 0,
      cINCOMPATIBLE_INCREMENTS,
      cINCOMPATIBLE_ORIGINS
   };

   static std::string StatusString(Status aStatus);

   SOSM_SpectralObject()
      : mOrigin(0.0F)
      , mIncrement(0.0F)
      , mCount(0)
      , mLowerWavelength(0.0F)
      , mUpperWavelength(0.0F)
   {
   }

   SOSM_SpectralObject(float aOrigin, float aIncrement, int aCount)
      : mOrigin(aOrigin)
      , mIncrement(aIncrement)
      , mCount(aCount)
      , mLowerWavelength(0.0F)
      , mUpperWavelength(0.0F)
   {
      UpdateWavelengthLimits();
   }

   virtual ~SOSM_SpectralObject() {}

   // Using default copy constructor
   // Using default assignment operator

   //! Return the origin (wavenumber, cm-1) of the first sample point.
   float Origin() const { return mOrigin; }

   //! Return the increment between each sample point (wavenumber, cm-1)
   //! The return value is zero if the object is not represented with a discrete spectrum.
   float Increment() const { return mIncrement; }

   //! Return the number of sample points in the spectrum.
   //! The return value is zero if the object is not represented with a discrete spectrum.
   int Count() const { return mCount; }

   //! Define the spectral limits of the object.
   //! @param aOrigin    The wavenumber (cm-1) of the first point in the spectrum.
   //! @param aIncrement The increment between each sample point in spectrum (wavenumbers, cm-1)
   //! @param aCount     The number of sample points in the spectrum.
   void DefineSpectralLimits(float aOrigin, float aIncrement, int aCount)
   {
      mOrigin    = aOrigin;
      mIncrement = aIncrement;
      mCount     = aCount;
      UpdateWavelengthLimits();
   }

   //! Define the spectral limits of the object.
   //! This is just like an assignment operator, but it is more explicit when this or the
   //! source are derived classes (eliminates casting).
   //!
   //! @param aSource The spectral object whose definition is to be assigned to this object.
   void DefineSpectralLimits(const SOSM_SpectralObject& aSource)
   {
      mOrigin          = aSource.mOrigin;
      mIncrement       = aSource.mIncrement;
      mCount           = aSource.mCount;
      mLowerWavelength = aSource.mLowerWavelength;
      mUpperWavelength = aSource.mUpperWavelength;
   }

   //! Do this object and another object have the same spectral limits?
   bool Equals(const SOSM_SpectralObject& aRhs) const
   {
      return ((mOrigin == aRhs.mOrigin) && (mIncrement == aRhs.mIncrement) && (mCount == aRhs.mCount));
   }

   Status Union(const SOSM_SpectralObject& aObject);

   Status Union(const SOSM_SpectralObject& aObject1, const SOSM_SpectralObject& aObject2);

   //! @name The declared wavelength limits of a sensor object.
   //!
   //! This is technically only for sensor objects and is used to record the user-defined
   //! wavelength limits to which the sensor is sensitive. These limits may be different that
   //! the wavelengths that correspond to the discrete limits defined by Origin(), Count() and
   //! Increment().
   //!
   //! This is a bit of a hack and is really only used for sensor objects, and even then
   //! it is applied only when the 'constant intensity' simple target model is being used.
   //@{

   // Return the actual lower wavelength (um).
   float LowerWavelength() const { return mLowerWavelength; }
   // Return the actual upper wavelength (um).
   float UpperWavelength() const { return mUpperWavelength; }

   // Set the actual lower and upper wavelengths (um).
   void SetWavelengthLimits(float aLowerWavelength, float aUpperWavelength)
   {
      mLowerWavelength = aLowerWavelength;
      mUpperWavelength = aUpperWavelength;
   }

   //@}
private:
   void UpdateWavelengthLimits();

   float mOrigin;
   float mIncrement;
   int   mCount;
   float mLowerWavelength;
   float mUpperWavelength;
};

#endif
