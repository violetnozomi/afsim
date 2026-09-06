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

#ifndef SOSM_SCALARTABLE3D_HPP
#define SOSM_SCALARTABLE3D_HPP

#include <string>
#include <vector>

class SOSM_Manager;
#include "TblLookup.hpp"

//! A table that represents a scalar function of three variables.
//!
//! @note This class does not currently have text file loading capabilities like the other table
//! classes as it is only used for storing the internally generated tables used by the for the
//! 'high-performance' mode.
class SOSM_ScalarTable3D
{
public:
   SOSM_ScalarTable3D(SOSM_Manager* aManagerPtr);

   // Using default copy constructor.
   // SOSM_ScalarTable3D(const SOSM_SpectralTable3D& aSrc);

   //! Return when the source file from which the object was recreated was last modified.
   double GetModificationTime() const { return mModificationTime; }

   //! Return the independent variable 1 values (const form).
   const TblIndVarU<float>& IV1() const { return mIV1; }

   //! Return the independent variable 1 values (non-const form).
   TblIndVarU<float>& IV1() { return mIV1; }

   //! Return the independent variable 2 values (const form).
   const TblIndVarU<float>& IV2() const { return mIV2; }

   //! Return the independent variable 2 values (non-const form).
   TblIndVarU<float>& IV2() { return mIV2; }

   //! Return the independent variable 3 values (const form).
   const TblIndVarU<float>& IV3() const { return mIV3; }

   //! Return the independent variable 3 values (non-const form).
   TblIndVarU<float>& IV3() { return mIV3; }

   //! Return the dependent variable (const form).
   const std::vector<float>& DV() const { return mDV; }

   //! Return the dependent variable (non-const form).
   std::vector<float>& DV() { return mDV; }

   float Interpolate(const TblLookupL<float>& aIV1_Lookup,
                     const TblLookupL<float>& aIV2_Lookup,
                     const TblLookupL<float>& aIV3_Lookup) const;

   void LoadBinaryFile(const std::string& aFileName);

   void SaveBinaryFile(const std::string& aFileName);

private:
   SOSM_Manager* mManagerPtr;

   //! The name of the file from which the table was loaded.
   std::string mFileName;

   //! The modification date of the above file.
   double mModificationTime;

   //! The first three lines from the file.
   std::string mUserIdent[3];

   //! Dependent variable ([#iv1][#iv2][#iv3]
   std::vector<float> mDV;

   //! Independent variable 1
   TblIndVarU<float> mIV1;

   //! Independent variable 2
   TblIndVarU<float> mIV2;

   //! Independent variable 3
   TblIndVarU<float> mIV3;
};

#endif
