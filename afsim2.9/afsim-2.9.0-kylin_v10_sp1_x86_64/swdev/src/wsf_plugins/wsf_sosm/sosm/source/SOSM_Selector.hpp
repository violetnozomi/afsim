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

#ifndef SOSM_SELECTOR_HPP
#define SOSM_SELECTOR_HPP

#include <string>
#include <vector>

class SOSM_SpectralObject;

//! This class represents the intersection of two SOSM_SpectralObject's.
//!
//! This class is used to compute the 'intersection' between to two SOSM_SpectralObjects.
class SOSM_Selector
{
public:
   SOSM_Selector();
   // Using default copy constructor
   // SOSM_Selector(const SOSM_Selector& aSrc);

   SOSM_Selector(const SOSM_SpectralObject& aInputOutput);

   //! Return value from 'Intersect'.
   enum Status
   {
      cINTERSECTS = 0,
      cOUTPUT_IS_CONTINUOUS,
      cINCOMPATIBLE_INCREMENTS,
      cINCOMPATIBLE_ORIGINS,
      cNO_INTERSECTION
   };

   static std::string StatusString(Status aStatus);

   Status Intersect(const SOSM_SpectralObject& aInput, const SOSM_SpectralObject& aOutput);

   void InitializeOutput(std::vector<float>& aOutput, float aValue) const;

   //! Return the origin (wavenumber of the first sample point) in the intersection
   float Origin() const { return mOrigin; }

   //! Returns the wavenumber increment between sample points.
   float Increment() const { return mIncrement; }

   //! Returns the number of points in the intersection
   int Count() const { return mCount; }

   //! Returns the index of the sample point in the input that corresponds to the Origin().
   int InputStartIndex() const { return mInputStartIndex; }

   //! Returns the index of the sample in point the output that corresponds to the Origin().
   int OutputStartIndex() const { return mOutputStartIndex; }

private:
   //! The origin of the intersection (wavenumber of the first sample point)
   float mOrigin;
   //! The increment between the sample points in the intersection.
   //! This should be the same as the increment of the target.
   float mIncrement;
   //! The number of sample points in the intersection
   int mCount;

   //! The index of the sample point in the input that corresponds to mOrigin.
   int mInputStartIndex;
   //! The index of the sample point in the output that corresponds to mOrigin.
   int mOutputStartIndex;
   //! The full count from the 'Output' object in the Intersect call.
   int mOutputCount;
};

#endif
