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

#ifndef SOSM_SCALARTABLE2D_HPP
#define SOSM_SCALARTABLE2D_HPP

#include <map>
#include <memory>
#include <string>
#include <vector>

class SOSM_Manager;
#include "SOSM_TableVar.hpp"
#include "TblLookup.hpp"

class SOSM_ScalarTable2D
{
public:
   using TablePtr = std::shared_ptr<SOSM_ScalarTable2D>;

   SOSM_ScalarTable2D(SOSM_Manager* aManagerPtr);

   // Using default copy constructor.
   // SOSM_ScalarTable2D(const SOSM_SpectralTable2D& aSrc);

   static TablePtr LoadSharedPolarTable(SOSM_Manager*        aManagerPtr,
                                        const std::string&   aFileName,
                                        const std::string&   aCachePrefix,
                                        const SOSM_TableVar& aDV_Info);

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

   float Interpolate(const TblLookupL<float>& aIV1_Lookup, const TblLookupL<float>& aIV2_Lookup) const;

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

private:
   void ValidateAndScale_IV(TblIndVarU<float>& aIV, const SOSM_TableVar& aIV_Info);

   SOSM_Manager* mManagerPtr;

   //! The name of the file from which the table was loaded.
   std::string mFileName;

   //! The modification date of the above file.
   double mModificationTime;

   //! The first three lines from the file.
   std::string mUserIdent[3];

   //! Dependent variable ([#iv1][#iv2]
   SOSM_TableVar      mDV_Info;
   std::vector<float> mDV;

   //! Independent variable 1
   SOSM_TableVar     mIV1_Info;
   TblIndVarU<float> mIV1;

   //! Independent variable 2
   SOSM_TableVar     mIV2_Info;
   TblIndVarU<float> mIV2;

   //! @name Persistent collection of polar scalar tables.
   //! A persistent collection of 2D scalar tables is maintained. When using the LoadSharedPolarTable
   //! interface, the first request to load a given file will load the file and return a reference
   //! counted pointer to the table. Subsequent requests to load the same file will return another
   //! reference to the same table.
   //!
   //! The choice was made to maintain this in the base class because both the simple and table
   //! target models can both use area tables. While they probably don't intersect, it allows
   //! sharing of the code. And because the key is the file name, there is no chance for misuse.
   //@{
   typedef std::map<std::string, TablePtr> TableMap;

   //! The persistent collection of area tables that have been read, indexed by filename.
   static TableMap sSharedPolarTables;
   //@}
};

#endif
