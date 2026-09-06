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

#include "SOSM_ScalarTable2D.hpp"

#include <algorithm>
#include <fstream>
#include <memory>

#include "SOSM_BinaryIO.hpp"
#include "SOSM_Manager.hpp"
#include "SOSM_Utility.hpp"
#include "UtException.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

// The persistent collection of shared polar tables.
SOSM_ScalarTable2D::TableMap SOSM_ScalarTable2D::sSharedPolarTables;

// =================================================================================================
SOSM_ScalarTable2D::SOSM_ScalarTable2D(SOSM_Manager* aManagerPtr)
   : mManagerPtr(aManagerPtr)
   , mModificationTime(0.0)
{
}

// =================================================================================================
SOSM_ScalarTable2D::TablePtr SOSM_ScalarTable2D::LoadSharedPolarTable(SOSM_Manager*        aManagerPtr,
                                                                      const std::string&   aFileName,
                                                                      const std::string&   aCachePrefix,
                                                                      const SOSM_TableVar& aDV_Info)
{
   TablePtr           tablePtr;
   TableMap::iterator ati = sSharedPolarTables.find(aFileName);
   if (ati != sSharedPolarTables.end())
   {
      tablePtr = ati->second;
   }
   else
   {
      std::unique_ptr<SOSM_ScalarTable2D> tempTablePtr(new SOSM_ScalarTable2D(aManagerPtr));
      tempTablePtr->SetDV_Info(aDV_Info);
      tempTablePtr->SetIV1_Info(SOSM_TableVar("azimuth", -180.0F, 180.0F, static_cast<float>(UtMath::cRAD_PER_DEG)));
      tempTablePtr->SetIV2_Info(SOSM_TableVar("elevation", -90.0F, 90.0F, static_cast<float>(UtMath::cRAD_PER_DEG)));
      tempTablePtr->Load(aFileName, aCachePrefix);
      tablePtr                      = TablePtr(tempTablePtr.release());
      sSharedPolarTables[aFileName] = tablePtr;
   }
   return tablePtr;
}

// =================================================================================================
void SOSM_ScalarTable2D::Load(const std::string& aFileName, const std::string& aCachePrefix)
{
   bool fileLoaded = false;

   // First attempt to load the file from the binary cache.
   if (!mManagerPtr->IgnoreCacheFiles())
   {
      std::string cacheFileName = mManagerPtr->CacheFileName(aFileName, aCachePrefix);
      if (SOSM_Utility::FileIsCurrent(cacheFileName, aFileName))
      {
         try
         {
            LoadBinaryFile(cacheFileName);
            fileLoaded = true;
         }
         catch (std::exception& e)
         {
            auto logError = ut::log::error() << "Failed to load binary file.";
            logError.AddNote() << "File: " << aFileName;
            logError.AddNote() << "Exception: " << e.what();
         }
      }
   }

   // If not loaded from the binary cache, load it from the text file.
   if (!fileLoaded)
   {
      LoadTextFile(aFileName);
      if (mManagerPtr->WriteCacheFiles())
      {
         std::string cacheFileName = mManagerPtr->CacheFileName(aFileName, aCachePrefix);
         SaveBinaryFile(cacheFileName);
      }
   }
}

// =================================================================================================
void SOSM_ScalarTable2D::LoadTextFile(const std::string& aFileName)
{
   std::ifstream ifs(aFileName.c_str());
   if (!ifs)
   {
      throw UtException("Unable to open '" + aFileName + "'");
   }
   mFileName = aFileName;
   if (mManagerPtr->ShowStatus())
   {
      auto logInfo = ut::log::info() << "Loading text file.";
      logInfo.AddNote() << "File: " << mFileName;
   }
   mManagerPtr->FileReferenced(mFileName); // Inform observers

   // Read the three user identification lines

   getline(ifs, mUserIdent[0]);
   getline(ifs, mUserIdent[1]);
   getline(ifs, mUserIdent[2]);
   if (!ifs)
   {
      throw UtException("File read error\n   In file: " + mFileName);
   }

   int iv1Count;
   int iv2Count;
   if (!(ifs >> iv1Count >> iv2Count))
   {
      throw UtException("File read error\n   In file: " + mFileName);
   }
   ifs.ignore(9999, '\n');

   if ((iv1Count < 2) || (iv2Count < 2))
   {
      throw UtException("File read error\n   In file: " + mFileName);
   }

   mIV1.Resize(iv1Count);
   mIV2.Resize(iv2Count);
   int dvSize = iv1Count * iv2Count;
   mDV.resize(dvSize);

   float iv1Value;
   float iv2Value;
   for (int iv2Index = 0; iv2Index < iv2Count; ++iv2Index)
   {
      if (!(ifs >> iv2Value))
      {
         throw UtException("File read error\n   In file: " + mFileName);
      }
      mIV2.Set(iv2Value, iv2Index);
   }
   ifs.ignore(9999, '\n');

   float dvValue;
   int   dvIndex = 0;
   for (int iv1Index = 0; iv1Index < iv1Count; ++iv1Index)
   {
      if (!(ifs >> iv1Value))
      {
         throw UtException("File read error\n   In file: " + mFileName);
      }
      mIV1.Set(iv1Value, iv1Index);

      for (int iv2Index = 0; iv2Index < iv2Count; ++iv2Index)
      {
         if (!(ifs >> dvValue))
         {
            throw UtException("File read error\n   In file: " + mFileName);
         }
         if ((dvValue < mDV_Info.Min()) || (dvValue > mDV_Info.Max()))
         {
            throw UtException("Invalid " + mDV_Info.Name() + " value\n   In file: " + mFileName);
         }
         mDV[dvIndex] = dvValue * mDV_Info.Scale();
         ++dvIndex;
      }
      ifs.ignore(9999, '\n');
   }
   if (dvIndex != dvSize)
   {
      throw UtException("Table size error");
   }

   ValidateAndScale_IV(mIV1, mIV1_Info);
   ValidateAndScale_IV(mIV2, mIV2_Info);

   mModificationTime = SOSM_Utility::FileModificationTime(mFileName);
}

// =================================================================================================
void SOSM_ScalarTable2D::LoadBinaryFile(const std::string& aFileName)
{
   std::ifstream ifs(aFileName.c_str(), std::ios::in | std::ios::binary);
   if (!ifs)
   {
      throw UtException("Unable to open '" + aFileName + "' for binary input");
   }
   mFileName = aFileName;
   if (mManagerPtr->ShowStatus())
   {
      auto logInfo = ut::log::info() << "Loading binary file.";
      logInfo.AddNote() << "File: " << mFileName;
   }
   mManagerPtr->FileReferenced(mFileName); // Inform observers
   SOSM_BinaryIO::ReadHeader(ifs, mUserIdent);
   SOSM_BinaryIO::ReadIV(ifs, mIV1);
   SOSM_BinaryIO::ReadIV(ifs, mIV2);
   SOSM_BinaryIO::ReadDV(ifs, mDV);
   SOSM_BinaryIO::ReadTrailer(ifs);

   mModificationTime = SOSM_Utility::FileModificationTime(mFileName);
}

// =================================================================================================
void SOSM_ScalarTable2D::SaveBinaryFile(const std::string& aFileName)
{
   std::ofstream ofs(aFileName.c_str(), std::ios::out | std::ios::binary);
   if (!ofs)
   {
      throw UtException("Unable to open file '" + aFileName + "' for binary output");
   }
   if (mManagerPtr->ShowStatus())
   {
      auto logInfo = ut::log::info() << "Saving binary file.";
      logInfo.AddNote() << "File: " << aFileName;
   }
   SOSM_BinaryIO::WriteHeader(ofs, mUserIdent);
   SOSM_BinaryIO::WriteIV(ofs, mIV1);
   SOSM_BinaryIO::WriteIV(ofs, mIV2);
   SOSM_BinaryIO::WriteDV(ofs, mDV);
   SOSM_BinaryIO::WriteTrailer(ofs);
}

// =================================================================================================
float SOSM_ScalarTable2D::Interpolate(const TblLookupL<float>& aIV1_Lookup, const TblLookupL<float>& aIV2_Lookup) const
{
   // 2-D indexing for var[#iv1][#iv2]

   size_t i1 = aIV1_Lookup.GetIndex();
   size_t i2 = aIV2_Lookup.GetIndex();
   size_t n2 = mIV2.GetSize();

   size_t i00 = (i1 * n2) + i2; // [i1  ][i2  ];
   size_t i01 = i00 + 1;        // [i1  ][i2+1];
   size_t i10 = i00 + n2;       // [i1+1][i2  ];
   size_t i11 = i01 + n2;       // [i1+1][i2+1];

   float r1    = aIV1_Lookup.GetRatio();
   float r2    = aIV2_Lookup.GetRatio();
   float f00   = mDV[i00];
   float f01   = mDV[i01];
   float f10   = mDV[i10];
   float f11   = mDV[i11];
   float value = f00 + ((f10 - f00) * r1) + ((f01 - f00) + (f11 - f10 - (f01 - f00)) * r1) * r2;
   return value;
}

// =================================================================================================
void SOSM_ScalarTable2D::ValidateAndScale_IV(TblIndVarU<float>& aIV, const SOSM_TableVar& aIV_Info)
{
   // Check values for validity
   for (size_t i = 0; i < aIV.GetSize(); ++i)
   {
      float value = aIV.Get(i);
      if ((value < aIV_Info.Min()) || (value > aIV_Info.Max()))
      {
         throw UtException("Invalid " + aIV_Info.Name() + " value\n   In file: " + mFileName);
      }
      if ((i > 0) && (value <= aIV.Get(i - 1)))
      {
         throw UtException("Non-ascending " + aIV_Info.Name() + " value\n   In file: " + mFileName);
      }
   }

   // Scale the data
   for (size_t i = 0; i < aIV.GetSize(); ++i)
   {
      float value = aIV.Get(i) * aIV_Info.Scale();
      aIV.Set(value, i);
   }
}
