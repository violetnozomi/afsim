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

// ----------------------------------------------------------------------------
//
// This class represents a directory that contains ARC Digitized Raster
// Graphics (ADRG) data.  The directory is expected to be in the standard
// NIMA structure.
//
// Refer to MIL-A-89007, 22 Feb 1990, for the ADRG specification.
//
// See 'AdrgFile.cpp' for a description of the ADRG file format.
// ----------------------------------------------------------------------------

#include "AdrgDir.hpp"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include "AdrgFile.hpp"
#include "AdrgTileMatrix.hpp"
#include "NimaUtil.hpp"
#include "RIFData.hpp"
#include "UtLog.hpp"

//#define DEBUG_ON

// TRANSMITTAL_HEADER_FIELD (tag VDR)
struct VDRData
{
public:
   char mUnsed1;
   char mTitle[200];
   char mUnsed2;
   char mNumVol;
   char mVolSeqNum;
   char mNumDistRect[3];
   char mStockNum[16];
   char mEdition[3];
   char mPubDate[12]; // "017,YYYYMMDD"
   char mFieldDelim;  // 0x1E
};

// DATA_SET_DESCRIPTION_FIELD (tag FDR)
struct FDRData
{
public:
   char mName[8];
   char mDataStruct; // '3' = Raster RGB
   char mSeries[4];  // "ADRG"
   char mSWLon[11];  // "+/-DDDMMSS.SS"
   char mSWLat[10];  // "+/-DDMMSS.SS"
   char mNELon[11];  // "+/-DDDMMSS.SS"
   char mNELat[10];  // "+/-DDMMSS.SS"
   char mFieldDelim; // 0x1E
};

// DATA_SET_DESCRIPTION_FIELD (take two!) (tag DRF)
struct DRFData
{
public:
   char mNumHorAcc[2];
   char mNumVerAcc[2];
   char mNumImgFiles[2];
   char mNumSrcGraphics[2];
   char mFieldDelim; // 0x1E
};

// ----------------------------------------------------------------------------

// 'Open' a ADRG directory.  The specified name should be a directory
// that represents either an ADRG CD-ROM or an EXACT COPY of a CD-ROM.

AdrgDir::AdrgDir(const char* aDirName, int& aError)
   : mDirName()
   , mLowerCase(true)
{
   if ((aDirName == nullptr) || (aDirName[0] == '\0'))
   {
      aError = 3; // Null or empty directory
   }
   else
   {
      mDirName = aDirName;

      // Add the trailing path delimiter if it is not present

      char lastChar = mDirName[mDirName.size() - 1];
      if ((lastChar != '/') && (lastChar != '\\'))
      {
         mDirName += "/";
      }

      std::string fileName = mDirName + "transh01.thf";
#ifdef DEBUG_ON
      {
         auto out = ut::log::debug() << "Opening ADRG file.";
         out.AddNote() << "File: " << fileName;
      }
#endif
      FILE* thFilePtr = ::fopen(fileName.c_str(), "rb");
      if (thFilePtr == nullptr)
      {
         fileName   = mDirName + "TRANSH01.THF";
         mLowerCase = false;
         thFilePtr  = ::fopen(fileName.c_str(), "rb");
      }
      if (thFilePtr != nullptr)
      {
         AdrgFile thFile(thFilePtr);
         try
         {
            ReadTransmittalHeaderFile(thFile);
         }
         catch (std::exception&)
         {
            aError = 2;
         }
         ::fclose(thFilePtr);
      }
      else
      {
         aError = 1;
      }
   }
}

AdrgDir::~AdrgDir()
{
   // Release all the tile matrices associated with this directory

   for (unsigned int i = 0; i < mTileMatrix.size(); ++i)
   {
      delete mTileMatrix[i];
   }
}

// Read a Distribution Rectangle (DR)
// private
void AdrgDir::ReadDistributionRectangle(AdrgFile& aGenFile, const char* aDrDirName)
{
   char tempBuf[32];

   // Determine the length of the DATA_DESCRIPTION_RECORD (DDR)
   aGenFile.Get(tempBuf, 5);
   int ddrLen = NimaUtil::StrToInt(tempBuf, 5);

   // DATA_SET_DESCRIPTION_RECORD (DSDR) is next.  We want to position
   // ourselves to the start of the 'field area'.  To do this we skip over
   // the first 12 bytes of the DSDR (record length and ident items) and
   // read the 'Base Data Address' item.

   aGenFile.Seek(ddrLen + 12, SEEK_SET);
   aGenFile.Get(tempBuf, 5);
   int baseDataOffset = NimaUtil::StrToInt(tempBuf, 5);
   aGenFile.Seek(ddrLen + baseDataOffset, SEEK_SET);

   // Skip over the RECORD_ID_FIELD
   RIFData rif;
   aGenFile.Get(&rif, sizeof(rif));

   // Read the DATA_SET_DESCRIPTION_FIELD to get the number of image files.
   DRFData drf;
   aGenFile.Get(&drf, sizeof(drf));
   int numImgFiles = NimaUtil::StrToInt(drf.mNumImgFiles, sizeof(drf.mNumImgFiles));

   // At this point we should have completely read the DSDR and we should
   // be positioned at the OVERVIEW_RECORD.  We're just going to skip this
   // for now...
   aGenFile.Get(tempBuf, 5);
   int recLen = NimaUtil::StrToInt(tempBuf, 5);
   aGenFile.Seek(recLen - 5, SEEK_CUR);

   // We should now be positioned at the first GENERAL_INFORMATION_RECORD.
   // There must be one of these for each image file.

   int imgFileNum;
   for (imgFileNum = 0; imgFileNum < numImgFiles; ++imgFileNum)
   {
      // Skip to baseDataOffset in leader
      aGenFile.Seek(12, SEEK_CUR);
      aGenFile.Get(tempBuf, 5);
      int baseOffset = NimaUtil::StrToInt(tempBuf, 5);
      aGenFile.Seek(baseOffset - 12 - 5, SEEK_CUR);

      int             status;
      AdrgTileMatrix* tmPtr = new AdrgTileMatrix(aGenFile, aDrDirName, status);
      if (status == 0)
      {
         mTileMatrix.push_back(tmPtr);
      }
      else
      {
         delete tmPtr;
         throw AdrgFile::IOError();
      }
   }
}

// Read the transmittal header file.

// private
void AdrgDir::ReadTransmittalHeaderFile(AdrgFile& aThFile)
{
   char tempBuf[32];

   // Determine the length of the DATA_DESCRIPTION_RECORD (DDR)
   aThFile.Get(tempBuf, 5);
   int ddrLen = NimaUtil::StrToInt(tempBuf, 5);

   // The TRANSMITTAL_DESCRIPTION_RECORD (TDR) is next.  We want to position
   // ourselves to the start of the 'field area'.  To do this we skip over
   // the first 12 bytes of the TDR (record length and ident items) and
   // read the 'Base Data Address' item.

   aThFile.Seek(ddrLen + 12, SEEK_SET);
   aThFile.Get(tempBuf, 5);
   int baseDataOffset = NimaUtil::StrToInt(tempBuf, 5);
   aThFile.Seek(ddrLen + baseDataOffset, SEEK_SET);

   // Skip over the RECORD_ID_FIELD
   RIFData rif;
   aThFile.Get(&rif, sizeof(rif));

   // Read the TRANSMITTAL_HEADER_FIELD to get the number distribution
   // rectangles...

   VDRData vdr;
   aThFile.Get(&vdr, sizeof(vdr));
   int numDistRect = NimaUtil::StrToInt(vdr.mNumDistRect, sizeof(vdr.mNumDistRect));

   // Read 'DATASET_DESCRIPTION_FIELD's.  Each one of these defines a
   // 'Definition Rectangle' (DR).
   FDRData fdr;
   for (int i = 0; i < numDistRect; ++i)
   {
      aThFile.Get(&fdr, sizeof(fdr));
      char drName[9];
      memcpy(drName, fdr.mName, 8);
      drName[8] = '\0';

#ifdef DEBUG_ON
      {
         double swLat = aThFile.StrToDeg(fdr.mSWLat, sizeof(fdr.mSWLat));
         double swLon = aThFile.StrToDeg(fdr.mSWLon, sizeof(fdr.mSWLon));
         double neLat = aThFile.StrToDeg(fdr.mNELat, sizeof(fdr.mNELat));
         double neLon = aThFile.StrToDeg(fdr.mNELon, sizeof(fdr.mNELon));
         auto   out   = ut::log::debug() << "DR #" << i << ":";
         out.AddNote() << "Name: " << drName;
         out.AddNote() << "SW Lat: " << swLat;
         out.AddNote() << "SW Lon: " << swLon;
         out.AddNote() << "NE Lat: " << neLat;
         out.AddNote() << "Ne Lon: " << neLon;
      }
#endif

      if (mLowerCase)
      {
         NimaUtil::LowerCase(drName);
      }
      std::string drDirName   = mDirName + drName + "/";
      std::string genFileName = drDirName + drName;
      if (mLowerCase)
      {
         genFileName += ".gen";
      }
      else
      {
         genFileName += ".GEN";
      }
      FILE* genFilePtr = ::fopen(genFileName.c_str(), "rb");
      if (genFilePtr != nullptr)
      {
         AdrgFile genFile(genFilePtr);
         try
         {
            ReadDistributionRectangle(genFile, drDirName.c_str());
         }
         catch (std::exception&)
         {
            // TODO
         }
         ::fclose(genFilePtr);
      }
   }
}
