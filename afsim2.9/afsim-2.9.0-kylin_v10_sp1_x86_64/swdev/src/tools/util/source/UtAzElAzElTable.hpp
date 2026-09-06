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

#ifndef UTAZELAZELTABLE_HPP
#define UTAZELAZELTABLE_HPP

#include <string>
#include <utility>
#include <vector>

#include "TblLookup.hpp"
class UtAzElTable;
#include "ut_export.h"
class UtInput;

//! A class that utilizes a table to represent a 4D function f(az1,el1,az2,el2).
//! Such tables are typically used to represent bistatic radar signatures and bidirectional reflectance distribution
//! functions (BRDF).
class UT_EXPORT UtAzElAzElTable
{
public:
   UtAzElAzElTable() = default;
   ~UtAzElAzElTable();

   float Lookup(float aTgtToXmtrAz, float aTgtToXmtrEl, float aTgtToRcvrAz, float aTgtToRcvrEl) const;

   float Lookup(double aTgtToXmtrAz, double aTgtToXmtrEl, double aTgtToRcvrAz, double aTgtToRcvrEl) const
   {
      return Lookup(static_cast<float>(aTgtToXmtrAz),
                    static_cast<float>(aTgtToXmtrEl),
                    static_cast<float>(aTgtToRcvrAz),
                    static_cast<float>(aTgtToRcvrEl));
   }

   static bool ProcessInput(UtInput& aInput, UtAzElAzElTable*& aTablePtr, std::string& aTableUnits);

private:
   class IndVar
   {
   public:
      IndVar() = default;

      void SetValues(const std::vector<float>& aValues);

      void Lookup(float aValue, size_t& aIndex, float& aRatio) const;

   private:
      enum LookupMethod
      {
         cLM_NONE, //!< Single point - no lookup
         cLM_E,    //!< Equally spaced arguments
         cLM_U,    //!< Unequally spaced arguments - use linear search
         cLM_UB,   //!< Unequally spaced arguments - use binary search
         cLM_LE,   //!< Equally spaced arguments
         cLM_LU,   //!< Unequally spaced arguments - use linear search
         cLM_LUB   //!< Unequally spaced arguments - use binary search
      };

      //! One or the other of the following will be defined depending on the spacing of the values.
      TblIndVarE<float> mValuesE;
      TblIndVarU<float> mValuesU;
      LookupMethod      mLookupMethod{cLM_NONE};
   };

   //! A slice of either constant azimuth or elevation.
   class Slice
   {
   public:
      float Lookup(float aVarValue, float aTgtToRcvrAz, float aTgtToRcvrEl) const;

      std::vector<UtAzElTable*> mTablePtrs; //!< The 'points' within the slice
      IndVar                    mVarValues; //!< The coordinates of the 'points' within the slice.
   };

   class InputPoint
   {
   public:
      InputPoint(float aAz, float aEl, UtAzElTable* aTablePtr);
      float        mAz;
      float        mEl;
      UtAzElTable* mTablePtr;
   };

   class InputTable
   {
   public:
      ~InputTable();
      std::vector<InputPoint> mPoints;
   };

   static void LoadFromFile(UtInput& aInput, UtAzElAzElTable*& aTablePtr, std::string& aTableUnits);

   static void LoadFromInput(UtInput& aInput, UtAzElAzElTable*& aTablePtr, std::string& aTableUnits);

   static void MakeTable(UtInput& aInput, InputTable& aInputTable, UtAzElAzElTable*& aTablePtr);

   std::vector<Slice> mSlices;
   IndVar             mConValues; //!< The azimuth or elevation of the slices
   //! If the following is true then the slices are of constant azimuth.
   //! Otherwise they are of constant elevation.
   bool mConstantAzSlices{false};
};

#endif
