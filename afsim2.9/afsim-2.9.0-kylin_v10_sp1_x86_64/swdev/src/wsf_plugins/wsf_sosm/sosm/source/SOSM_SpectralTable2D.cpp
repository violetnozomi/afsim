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

#include "SOSM_SpectralTable2D.hpp"

#include <algorithm>
#include <fstream>

#include "SOSM_BinaryIO.hpp"
#include "SOSM_Manager.hpp"
#include "SOSM_Selector.hpp"
#include "SOSM_Utility.hpp"
#include "UtException.hpp"
#include "UtLog.hpp"

// =================================================================================================
SOSM_SpectralTable2D::SOSM_SpectralTable2D(SOSM_Manager* aManagerPtr)
   : mManagerPtr(aManagerPtr)
   , mModificationTime(0.0)
{
}

// =================================================================================================
void SOSM_SpectralTable2D::Interpolate(std::vector<float>&      aOutput,
                                       const SOSM_Selector&     aSelector,
                                       const TblLookupL<float>& aIV1_Lookup,
                                       const TblLookupL<float>& aIV2_Lookup) const
{
   // 3-D indexing for var[#az][#el][#sample]

   size_t i1  = aIV1_Lookup.GetIndex();
   size_t i2  = aIV2_Lookup.GetIndex();
   size_t n2  = mIV2.GetSize();
   size_t n3  = Count();
   size_t n23 = n2 * n3;

   size_t i00 = (i1 * n23) + (i2 * n3); // [i1  ][i2  ][0];
   size_t i01 = i00 + n3;               // [i1  ][i2+1][0];
   size_t i10 = i00 + n23;              // [i1+1][i2  ][0];
   size_t i11 = i01 + n23;              // [i1+1][i2+1][0];

   float r1    = aIV1_Lookup.GetRatio();
   float r2    = aIV2_Lookup.GetRatio();
   int   ii    = aSelector.InputStartIndex();
   int   oi    = aSelector.OutputStartIndex();
   int   count = aSelector.Count();
   for (int index = 0; index < count; ++index)
   {
      float f00   = mDV[i00 + ii];
      float f01   = mDV[i01 + ii];
      float f10   = mDV[i10 + ii];
      float f11   = mDV[i11 + ii];
      float value = f00 + ((f10 - f00) * r1) + ((f01 - f00) + (f11 - f10 - (f01 - f00)) * r1) * r2;
      aOutput[oi] = value;
      ++ii;
      ++oi;
   }
}

// =================================================================================================
void SOSM_SpectralTable2D::Load(const std::string& aFileName, const std::string& aCachePrefix)
{
   bool fileLoaded = false;

   // First attempt to load the file from the binary cache.
   if (!GetManager()->IgnoreCacheFiles())
   {
      std::string cacheFileName = GetManager()->CacheFileName(aFileName, aCachePrefix);
      if (SOSM_Utility::FileIsCurrent(cacheFileName, aFileName))
      {
         try
         {
            LoadBinaryFile(cacheFileName);
            fileLoaded = true;
         }
         catch (std::exception& e)
         {
            auto out = ut::log::error() << "Failed to load binary file.";
            out.AddNote() << "File: " << aFileName;
            out.AddNote() << "Exception: " << e.what();
         }
      }
   }

   // If not loaded from the binary cache, load it from the text file.
   if (!fileLoaded)
   {
      LoadTextFile(aFileName);
      if (GetManager()->WriteCacheFiles())
      {
         std::string cacheFileName = GetManager()->CacheFileName(aFileName, aCachePrefix);
         SaveBinaryFile(cacheFileName);
      }
   }
}

// =================================================================================================
void SOSM_SpectralTable2D::LoadTextFile(const std::string& aFileName)
{
   std::ifstream ifs(aFileName.c_str());
   if (!ifs)
   {
      throw UtException("Unable to open '" + aFileName + "'");
   }
   mFileName = aFileName;
   if (GetManager()->ShowStatus())
   {
      auto out = ut::log::info() << "Loading text file.";
      out.AddNote() << "File: " << aFileName;
   }
   GetManager()->FileReferenced(mFileName); // Inform observers

   // Read the three user identification lines

   getline(ifs, mUserIdent[0]);
   getline(ifs, mUserIdent[1]);
   getline(ifs, mUserIdent[2]);
   if (!ifs)
   {
      throw UtException("File read error\n   In file: " + mFileName);
   }

   // Read the spectral limits

   float origin;
   float increment;
   int   count;
   if (!(ifs >> origin >> increment >> count))
   {
      throw UtException("File read error\n   In file: " + mFileName);
   }
   ifs.ignore(9999, '\n');
   if ((origin < 0.0F) || (increment <= 0.0F) || (count <= 0))
   {
      throw UtException("Invalid spectral origin/increment/count values\n   In file: " + mFileName);
   }
   DefineSpectralLimits(origin, increment, count);

   int iv1Count;
   int iv2Count;
   if (!(ifs >> iv1Count >> iv2Count))
   {
      throw UtException("File read error\n   In file: " + mFileName);
   }
   ifs.ignore(9999, '\n');

   if ((iv1Count < 2) || (iv2Count < 2))
   {
      throw UtException("Invalid dimensions\n   In file: " + mFileName);
   }

   mIV1.Resize(iv1Count);
   mIV2.Resize(iv2Count);
   int dvSize = iv1Count * iv2Count * Count();
   mDV.resize(dvSize);

   float iv1Value;
   float iv2Value;
   float dvValue;
   int   dvIndex = 0;
   for (int iv1Index = 0; iv1Index < iv1Count; ++iv1Index)
   {
      for (int iv2Index = 0; iv2Index < iv2Count; ++iv2Index)
      {
         if (!(ifs >> iv1Value >> iv2Value))
         {
            throw UtException("File read error\n   In file: " + mFileName);
         }

         // Determine if the optional 'copy last values' flag exists on the end of the line.
         bool        copyLastValues = false;
         std::string extraData;
         std::getline(ifs, extraData);
         if (extraData.find_first_of("cC") != std::string::npos)
         {
            copyLastValues = true;
            if (iv2Index == 0)
            {
               throw UtException("'copy last values' not valid in current context\n   In file: " + mFileName);
            }
         }

         // Ensure the consistency of repeated independent variable specifications.
         if (iv1Index != 0)
         {
            if ((iv2Value != mIV2.Get(iv2Index)))
            {
               throw UtException(mIV2_Info.Name() + " values are not consistent\n   In file: " + mFileName);
            }
         }
         if (iv2Index != 0)
         {
            if ((iv1Value != mIV1.Get(iv1Index)))
            {
               throw UtException(mIV1_Info.Name() + " values are not consistent\n   In file: " + mFileName);
            }
         }

         mIV1.Set(iv1Value, iv1Index);
         mIV2.Set(iv2Value, iv2Index);

         // Read the spectral data samples or copy from previous entry.
         if (copyLastValues)
         {
            for (int index = 0; index < Count(); ++index)
            {
               mDV[dvIndex] = mDV[dvIndex - Count()];
               ++dvIndex;
            }
         }
         else
         {
            for (int index = 0; index < Count(); ++index)
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
      }
   }
   if (dvIndex != dvSize)
   {
      throw UtException("Table size error\n   In file: " + mFileName);
   }

   ValidateAndScale_IV(mIV1, mIV1_Info);
   ValidateAndScale_IV(mIV2, mIV2_Info);

   mModificationTime = SOSM_Utility::FileModificationTime(mFileName);
}

// =================================================================================================
void SOSM_SpectralTable2D::LoadBinaryFile(const std::string& aFileName)
{
   std::ifstream ifs(aFileName.c_str(), std::ios::in | std::ios::binary);
   if (!ifs)
   {
      throw UtException("Unable to open '" + aFileName + "' for binary input");
   }
   mFileName = aFileName;
   if (GetManager()->ShowStatus())
   {
      auto out = ut::log::info() << "Loading binary file.";
      out.AddNote() << "File: " << mFileName;
   }
   GetManager()->FileReferenced(mFileName); // Inform observers
   SOSM_BinaryIO::ReadHeader(ifs, mUserIdent);
   SOSM_BinaryIO::ReadSpectralObject(ifs, *this);
   SOSM_BinaryIO::ReadIV(ifs, mIV1);
   SOSM_BinaryIO::ReadIV(ifs, mIV2);
   SOSM_BinaryIO::ReadDV(ifs, mDV);
   SOSM_BinaryIO::ReadTrailer(ifs);

   mModificationTime = SOSM_Utility::FileModificationTime(mFileName);
}

// =================================================================================================
void SOSM_SpectralTable2D::SaveBinaryFile(const std::string& aFileName)
{
   std::ofstream ofs(aFileName.c_str(), std::ios::out | std::ios::binary);
   if (!ofs)
   {
      throw UtException("Unable to open file '" + aFileName + "' for binary output");
   }
   if (GetManager()->ShowStatus())
   {
      auto out = ut::log::info() << "Saving binary file.";
      out.AddNote() << "File: " << aFileName;
   }
   SOSM_BinaryIO::WriteHeader(ofs, mUserIdent);
   SOSM_BinaryIO::WriteSpectralObject(ofs, *this);
   SOSM_BinaryIO::WriteIV(ofs, mIV1);
   SOSM_BinaryIO::WriteIV(ofs, mIV2);
   SOSM_BinaryIO::WriteDV(ofs, mDV);
   SOSM_BinaryIO::WriteTrailer(ofs);
}

// =================================================================================================
void SOSM_SpectralTable2D::ValidateAndScale_IV(TblIndVarU<float>& aIV, const SOSM_TableVar& aIV_Info)
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
