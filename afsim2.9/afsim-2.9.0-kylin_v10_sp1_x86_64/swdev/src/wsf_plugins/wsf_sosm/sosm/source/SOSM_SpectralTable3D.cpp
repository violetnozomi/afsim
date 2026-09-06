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

#include "SOSM_SpectralTable3D.hpp"

#include <algorithm>
#include <fstream>

#include "SOSM_BinaryIO.hpp"
#include "SOSM_Manager.hpp"
#include "SOSM_Selector.hpp"
#include "SOSM_Utility.hpp"
#include "UtException.hpp"
#include "UtLog.hpp"

// =================================================================================================
SOSM_SpectralTable3D::SOSM_SpectralTable3D(SOSM_Manager* aManagerPtr)
   : mManagerPtr(aManagerPtr)
   , mModificationTime(0.0)
{
}

// =================================================================================================
void SOSM_SpectralTable3D::Interpolate(std::vector<float>&      aOutput,
                                       const SOSM_Selector&     aSelector,
                                       const TblLookupL<float>& aIV1_Lookup,
                                       const TblLookupL<float>& aIV2_Lookup,
                                       const TblLookupL<float>& aIV3_Lookup) const
{
   // 4-D indexing for var[#iv1][#iv2][#iv3][#sample]

   size_t i1   = aIV1_Lookup.GetIndex();
   size_t i2   = aIV2_Lookup.GetIndex();
   size_t i3   = aIV3_Lookup.GetIndex();
   size_t n2   = mIV2.GetSize();
   size_t n3   = mIV3.GetSize();
   int    n4   = Count();
   size_t n34  = n3 * n4;
   size_t n234 = n2 * n34;

   size_t i000 = (i1 * n234) + (i2 * n34) + (i3 * n4); // [i1  ][i2  ][i3  ][0]
   size_t i001 = i000 + n4;                            // [i1  ][i2  ][i3+1][0]
   size_t i010 = i000 + n34;                           // [i1  ][i2+1][i3  ][0]
   size_t i011 = i010 + n4;                            // [i1  ][i2+1][i3+1][0]
   size_t i100 = i000 + n234;                          // [i1+1][i2  ][i3  ][0]
   size_t i101 = i001 + n234;                          // [i1+1][i2  ][i3+1][0]
   size_t i110 = i010 + n234;                          // [i1+1][i2+1][i3  ][0]
   size_t i111 = i011 + n234;                          // [i1+1][i2+1][i3+1][0]

   float r1    = aIV1_Lookup.GetRatio();
   float r2    = aIV2_Lookup.GetRatio();
   float r3    = aIV3_Lookup.GetRatio();
   int   ii    = aSelector.InputStartIndex();
   int   oi    = aSelector.OutputStartIndex();
   int   count = aSelector.Count();
   for (int index = 0; index < count; ++index)
   {
      float f000  = mDV[i000 + ii];
      float f001  = mDV[i001 + ii];
      float f010  = mDV[i010 + ii];
      float f011  = mDV[i011 + ii];
      float f100  = mDV[i100 + ii];
      float f101  = mDV[i101 + ii];
      float f110  = mDV[i110 + ii];
      float f111  = mDV[i111 + ii];
      float a     = f000;
      float b     = f100 - f000;
      float c     = f010 - f000;
      float d     = f001 - f000;
      float e     = f110 - a - b - c;
      float f     = f101 - a - b - d;
      float g     = f011 - a - c - d;
      float h     = f111 - a - b - c - d - e - f - g;
      float ans   = a + (r1 * (b + e * r2)) + (r2 * (c + (g + h * r1) * r3)) + (r3 * (d + f * r1));
      aOutput[oi] = ans;
      ++ii;
      ++oi;
   }
}
// =================================================================================================
void SOSM_SpectralTable3D::Load(const std::string& aFileName, const std::string& aCachePrefix)
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
      if (GetManager()->WriteCacheFiles())
      {
         std::string cacheFileName = GetManager()->CacheFileName(aFileName, aCachePrefix);
         SaveBinaryFile(cacheFileName);
      }
   }
}

// =================================================================================================
void SOSM_SpectralTable3D::LoadTextFile(const std::string& aFileName)
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
      out.AddNote() << "File: " << mFileName;
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
   int iv3Count;
   if (!(ifs >> iv1Count >> iv2Count >> iv3Count))
   {
      throw UtException("File read error\n   In file: " + mFileName);
   }
   ifs.ignore(9999, '\n');

   if ((iv1Count < 2) || (iv2Count < 2) || (iv3Count < 2))
   {
      throw UtException("Invalid dimensions\n   In file: " + mFileName);
   }

   mIV1.Resize(iv1Count);
   mIV2.Resize(iv2Count);
   mIV3.Resize(iv3Count);

   int dvSize = iv1Count * iv2Count * iv3Count * Count();
   mDV.resize(dvSize);
   float iv1Value;
   float iv2Value;
   float iv3Value;
   float dvValue;
   int   dvIndex = 0;
   for (int iv1Index = 0; iv1Index < iv1Count; ++iv1Index)
   {
      if (!(ifs >> iv1Value))
      {
         throw UtException("File read error\n   In file: " + mFileName);
      }
      ifs.ignore(9999, '\n');

      mIV1.Set(iv1Value, iv1Index);
      for (int iv2Index = 0; iv2Index < iv2Count; ++iv2Index)
      {
         for (int iv3Index = 0; iv3Index < iv3Count; ++iv3Index)
         {
            if (!(ifs >> iv2Value >> iv3Value))
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
               if (iv3Index == 0)
               {
                  throw UtException("'copy last values' not valid in current context\n   In file: " + mFileName);
               }
            }

            // Make sure repeated specifications of the 2nd and 3rd independent variables
            // are consistent with earlier specifications.

            if (iv1Index != 0)
            {
               if (mIV2.Get(iv2Index) != iv2Value)
               {
                  throw UtException(mIV2_Info.Name() + " values are not consistent\n   In file: " + mFileName);
               }
               if (mIV3.Get(iv3Index) != iv3Value)
               {
                  throw UtException(mIV3_Info.Name() + " values are not consistent\n   In file: " + mFileName);
               }
            }
            else if (iv2Index != 0)
            {
               if (mIV3.Get(iv3Index) != iv3Value)
               {
                  throw UtException(mIV3_Info.Name() + " values are not consistent\n   In file: " + mFileName);
               }
            }
            else if (iv3Index != 0)
            {
               if (mIV2.Get(iv2Index) != iv2Value)
               {
                  throw UtException(mIV2_Info.Name() + " values are not consistent\n   In file: " + mFileName);
               }
            }

            mIV2.Set(iv2Value, iv2Index);
            mIV3.Set(iv3Value, iv3Index);

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
   }
   if (dvIndex != dvSize)
   {
      throw UtException("Table size error\n   In file: " + mFileName);
   }

   ValidateAndScale_IV(mIV1, mIV1_Info);
   ValidateAndScale_IV(mIV2, mIV2_Info);
   ValidateAndScale_IV(mIV3, mIV3_Info);

   mModificationTime = SOSM_Utility::FileModificationTime(mFileName);
}

// =================================================================================================
void SOSM_SpectralTable3D::LoadBinaryFile(const std::string& aFileName)
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
   SOSM_BinaryIO::ReadIV(ifs, mIV3);
   SOSM_BinaryIO::ReadDV(ifs, mDV);
   SOSM_BinaryIO::ReadTrailer(ifs);

   mModificationTime = SOSM_Utility::FileModificationTime(mFileName);
}

// =================================================================================================

void SOSM_SpectralTable3D::SaveBinaryFile(const std::string& aFileName)
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
   SOSM_BinaryIO::WriteIV(ofs, mIV3);
   SOSM_BinaryIO::WriteDV(ofs, mDV);
   SOSM_BinaryIO::WriteTrailer(ofs);
}

// =================================================================================================
void SOSM_SpectralTable3D::ValidateAndScale_IV(TblIndVarU<float>& aIV, const SOSM_TableVar& aIV_Info)
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
