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

#include "SOSM_ScalarTable3D.hpp"

#include <algorithm>
#include <fstream>

#include "SOSM_BinaryIO.hpp"
#include "SOSM_Manager.hpp"
#include "SOSM_Utility.hpp"
#include "UtException.hpp"
#include "UtLog.hpp"

// =================================================================================================
SOSM_ScalarTable3D::SOSM_ScalarTable3D(SOSM_Manager* aManagerPtr)
   : mManagerPtr(aManagerPtr)
   , mModificationTime(0.0)
{
}

// =================================================================================================
void SOSM_ScalarTable3D::LoadBinaryFile(const std::string& aFileName)
{
   std::ifstream ifs(aFileName.c_str(), std::ios::in | std::ios::binary);
   if (!ifs)
   {
      throw UtException("Unable to open '" + aFileName + "' for binary input");
   }
   if (mManagerPtr->ShowStatus())
   {
      auto logInfo = ut::log::info() << "Loading binary file.";
      logInfo.AddNote() << aFileName;
   }
   mManagerPtr->FileReferenced(aFileName); // Inform observers
   SOSM_BinaryIO::ReadHeader(ifs, mUserIdent);
   SOSM_BinaryIO::ReadIV(ifs, mIV1);
   SOSM_BinaryIO::ReadIV(ifs, mIV2);
   SOSM_BinaryIO::ReadIV(ifs, mIV3);
   SOSM_BinaryIO::ReadDV(ifs, mDV);
   SOSM_BinaryIO::ReadTrailer(ifs);

   mFileName         = aFileName;
   mModificationTime = SOSM_Utility::FileModificationTime(mFileName);
}

// =================================================================================================
void SOSM_ScalarTable3D::SaveBinaryFile(const std::string& aFileName)
{
   std::ofstream ofs(aFileName.c_str(), std::ios::out | std::ios::binary);
   if (!ofs)
   {
      throw UtException("Unable to open file '" + aFileName + "' for binary output");
   }
   { // RAII block
      auto logInfo = ut::log::info() << "Saving binary file.";
      logInfo.AddNote() << "File: " << aFileName;
   }
   SOSM_BinaryIO::WriteHeader(ofs, mUserIdent);
   SOSM_BinaryIO::WriteIV(ofs, mIV1);
   SOSM_BinaryIO::WriteIV(ofs, mIV2);
   SOSM_BinaryIO::WriteIV(ofs, mIV3);
   SOSM_BinaryIO::WriteDV(ofs, mDV);
   SOSM_BinaryIO::WriteTrailer(ofs);
}

// =================================================================================================
float SOSM_ScalarTable3D::Interpolate(const TblLookupL<float>& aIV1_Lookup,
                                      const TblLookupL<float>& aIV2_Lookup,
                                      const TblLookupL<float>& aIV3_Lookup) const
{
   // 3-D indexing for var[#iv1][#iv2][#iv3]

   size_t i1  = aIV1_Lookup.GetIndex();
   size_t i2  = aIV2_Lookup.GetIndex();
   size_t i3  = aIV3_Lookup.GetIndex();
   size_t n2  = mIV2.GetSize();
   size_t n3  = mIV3.GetSize();
   size_t n23 = n2 * n3;

   size_t i000 = (i1 * n23) + (i2 * n3) + i3; // [i1  ][i2  ][i3  ]
   size_t i001 = i000 + 1;                    // [i1  ][i2  ][i3+1]
   size_t i010 = i000 + n3;                   // [i1  ][i2+1][i3  ]
   size_t i011 = i010 + 1;                    // [i1  ][i2+1][i3+1]
   size_t i100 = i000 + n23;                  // [i1+1][i2  ][i3  ]
   size_t i101 = i001 + n23;                  // [i1+1][i2  ][i3+1]
   size_t i110 = i010 + n23;                  // [i1+1][i2+1][i3  ]
   size_t i111 = i011 + n23;                  // [i1+1][i2+1][i3+1]

   float r1    = aIV1_Lookup.GetRatio();
   float r2    = aIV2_Lookup.GetRatio();
   float r3    = aIV3_Lookup.GetRatio();
   float f000  = mDV[i000];
   float f001  = mDV[i001];
   float f010  = mDV[i010];
   float f011  = mDV[i011];
   float f100  = mDV[i100];
   float f101  = mDV[i101];
   float f110  = mDV[i110];
   float f111  = mDV[i111];
   float a     = f000;
   float b     = f100 - f000;
   float c     = f010 - f000;
   float d     = f001 - f000;
   float e     = f110 - a - b - c;
   float f     = f101 - a - b - d;
   float g     = f011 - a - c - d;
   float h     = f111 - a - b - c - d - e - f - g;
   float value = a + (r1 * (b + e * r2)) + (r2 * (c + (g + h * r1) * r3)) + (r3 * (d + f * r1));
   return value;
}
