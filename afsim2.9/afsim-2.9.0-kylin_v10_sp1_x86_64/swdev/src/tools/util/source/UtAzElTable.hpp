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

#ifndef UTAZELTABLE_HPP
#define UTAZELTABLE_HPP

#include "ut_export.h"

#include "TblLookup.hpp"
#include "UtAzElLookup.hpp"

/**
   An object used to store values that are a function of azimuth and elevation.

   This class is used to maintain tables that are a function of azimuth and
   elevation (antenna patterns, signatures, etc). and provides methods to
   evaluation (look-up) a value given an azimuth and elevation.
   <p>
   The independent variables \b MUST be stored in \b RADIANS and
   be within the following limits:
   <ul>
   <li> -Pi  <=  Azimuth  <=  Pi
   <li> -Pi/2 <= Elevation <= Pi/2
   </ul>
*/

class UT_EXPORT UtAzElTable
{
   using InterpolationType = ut::azel::InterpolationType;

public:
   UtAzElTable()          = default;
   virtual ~UtAzElTable() = default;

   void CreateConstantTable(float aValue);

   void ConvertValuesFromDB(float aReference);

   void ConvertValuesToDB(float aReference);

   //! Get to the table lookup context for looking up values in the table.
   //!
   //! The returned table lookup context can be used to extract values from the table.
   //! The context has been optimized to choose the best method based on the size and
   //! spacing of the independent values.
   //!
   //! @param aContext [output] The context to be used for performing table lookups.
   //!
   //! @note This can be called only after Validate() has been called.

   void          GetContext(UtAzElLookup& aContext) { aContext = mContext; }
   UtAzElLookup& GetContext() { return mContext; }

   void SetInterpolationFlag(bool aInterpolate);
   bool GetInterpolationFlag() const;

   void              SetInterpolationType(InterpolationType aInterpolationType);
   InterpolationType GetInterpolationType() const;

   void MultiplyValues(float aMultiplier);

   bool GetIsConstant() const { return mIsConstant; }

   //! Set evenly spaced az and el values. This is an extension for External Services.
   void SetAzValues(float aMinvalue, float aMaxValue, size_t aNumValues);
   void SetElValues(float aMinvalue, float aMaxValue, size_t aNumValues);

   bool Validate(size_t aAzCount, size_t aElCount);

   // Okay, I know that it is bad to make object public.  But these must be
   // loaded externally and I didn't feel like providing another interface on
   // top of what already existed...

   TblIndVarU<float> mAzValues; //<! azimuth independent values
   TblIndVarU<float> mElValues; //<! elevation independent values

   TblIndVarE<float> mAzValuesE; //<! evenly spaced azimuth independent values
   TblIndVarE<float> mElValuesE; //<! evenly spaced elevation independent values

   TblDepVar2<float> mDataValues; //<! the function table f(az,el)

private:
   UtAzElLookup mContext; //<! Prototype table lookup context

   bool mIsConstant{false}; //<! A flag indicating if this table was created with a constant value
};

#endif
