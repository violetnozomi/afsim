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

#ifndef SOSM_SPECTRALTABLE2D_HPP
#define SOSM_SPECTRALTABLE2D_HPP

#include <string>
#include <vector>

class SOSM_Manager;
class SOSM_Selector;
#include "SOSM_SpectralObject.hpp"
#include "SOSM_TableVar.hpp"
#include "TblLookup.hpp"

//! A spectral table with two independent variables.
class SOSM_SpectralTable2D : public SOSM_SpectralObject
{
public:
   SOSM_SpectralTable2D(SOSM_Manager* aManagerPtr);

   // Using default copy constructor.
   // SOSM_SpectralTable2D(const SOSM_SpectralTable2D& aSrc);

   //! Return when the source file from which the object was recreated was last modified.
   double GetModificationTime() const { return mModificationTime; }

   //! Return the dependent variable (const form).
   const std::vector<float>& DV() const { return mDV; }

   //! Return the dependent variable (non-const form).
   std::vector<float>& DV() { return mDV; }

   //! Return the independent variable 1 values (const form).
   const TblIndVarU<float>& IV1() const { return mIV1; }

   //! Return the independent variable 1 values (non-const form).
   TblIndVarU<float>& IV1() { return mIV1; }

   //! Return the independent variable 2 values (const form).
   const TblIndVarU<float>& IV2() const { return mIV2; }

   //! Return the independent variable 2 values (non-const form).
   TblIndVarU<float>& IV2() { return mIV2; }

   //! Set the name, limits and scale for the dependent variable.
   void SetDV_Info(const SOSM_TableVar& aDV_Info) { mDV_Info = aDV_Info; }

   //! Set the name, limits and scale for independent variable 1.
   void SetIV1_Info(const SOSM_TableVar& aIV1_Info) { mIV1_Info = aIV1_Info; }

   //! Set the name, limits and scale for independent variable 2.
   void SetIV2_Info(const SOSM_TableVar& aIV2_Info) { mIV2_Info = aIV2_Info; }

   void Interpolate(std::vector<float>&      aOutput,
                    const SOSM_Selector&     aSelector,
                    const TblLookupL<float>& aIV1_Lookup,
                    const TblLookupL<float>& aIV2_Lookup) const;

   //! A helper function to perform the lookup function for a polar (az/el) table.
   //! @param aAzLookup  [output] The azimuth   lookup object.
   //! @param aElLookup  [output] The elevation lookup object.
   //! @param aAzimuth   [input]  The azimuth (radians).
   //! @param aElevation [input]  The elevation (radians).
   void PolarLookup(TblLookupLU<float>& aAzLookup, TblLookupLU<float>& aElLookup, float aAzimuth, float aElevation) const
   {
      // Check if table has implied azimuth symmetry (i.e. - first value is non-negative).
      float tempAzimuth = aAzimuth;
      if ((aAzimuth < 0.0F) && (mIV1.Get(0) >= 0.0F))
      {
         tempAzimuth = -aAzimuth;
      }
      aAzLookup.Lookup(mIV1, tempAzimuth);
      aElLookup.Lookup(mIV2, aElevation);
   }

   void Load(const std::string& aFileName, const std::string& aCachePrefix);

   void LoadTextFile(const std::string& aFileName);

   void LoadBinaryFile(const std::string& aFileName);

   void SaveBinaryFile(const std::string& aFileName);

   SOSM_Manager* GetManager() const { return mManagerPtr; }

protected:
   void ValidateAndScale_IV(TblIndVarU<float>& aIV, const SOSM_TableVar& aIV_Info);

   SOSM_Manager* mManagerPtr;

   //! The name of the file from which the table was loaded.
   std::string mFileName;

   //! The modification date of the above file.
   double mModificationTime;

   //! The first three lines from the file.
   std::string mUserIdent[3];

   //! Dependent variable ([iv1][iv2][sample]
   SOSM_TableVar      mDV_Info;
   std::vector<float> mDV;

   //! Independent variable 1
   SOSM_TableVar     mIV1_Info;
   TblIndVarU<float> mIV1;

   //! Independent variable 2
   SOSM_TableVar     mIV2_Info;
   TblIndVarU<float> mIV2;
};

#endif
