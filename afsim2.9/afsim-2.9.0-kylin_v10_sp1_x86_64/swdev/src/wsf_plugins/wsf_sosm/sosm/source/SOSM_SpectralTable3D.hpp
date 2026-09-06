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

#ifndef SOSM_SPECTRALTABLE3D_HPP
#define SOSM_SPECTRALTABLE3D_HPP

#include <string>
#include <vector>

class SOSM_Manager;
class SOSM_Selector;
#include "SOSM_SpectralObject.hpp"
#include "SOSM_TableVar.hpp"
#include "TblLookup.hpp"

//! A spectral table with two independent variables.
class SOSM_SpectralTable3D : public SOSM_SpectralObject
{
public:
   SOSM_SpectralTable3D(SOSM_Manager* aManagerPtr);

   // Using default copy constructor.
   // SOSM_SpectralTable3D(const SOSM_SpectralTable3D& aSrc);

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

   //! Set the name, limits and scale for the dependent variable.
   void SetDV_Info(const SOSM_TableVar& aDV_Info) { mDV_Info = aDV_Info; }

   //! Set the name, limits and scale for independent variable 1.
   void SetIV1_Info(const SOSM_TableVar& aIV1_Info) { mIV1_Info = aIV1_Info; }

   //! Set the name, limits and scale for independent variable 2.
   void SetIV2_Info(const SOSM_TableVar& aIV2_Info) { mIV2_Info = aIV2_Info; }

   //! Set the name, limits and scale for independent variable 3.
   void SetIV3_Info(const SOSM_TableVar& aIV3_Info) { mIV3_Info = aIV3_Info; }

   void Interpolate(std::vector<float>&      aOutput,
                    const SOSM_Selector&     aSelector,
                    const TblLookupL<float>& aIV1_Lookup,
                    const TblLookupL<float>& aIV2_Lookup,
                    const TblLookupL<float>& aIV3_Lookup) const;

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

   //! Dependent variable ([iv1][iv2][iv3][sample])
   SOSM_TableVar      mDV_Info;
   std::vector<float> mDV;

   //! Independent variable 1
   SOSM_TableVar     mIV1_Info;
   TblIndVarU<float> mIV1;

   //! Independent variable 2
   SOSM_TableVar     mIV2_Info;
   TblIndVarU<float> mIV2;

   //! Independent variable 3
   SOSM_TableVar     mIV3_Info;
   TblIndVarU<float> mIV3;
};

#endif
