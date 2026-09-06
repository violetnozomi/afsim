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

// GenLoggerFileI  - A derived class of GenI that does buffered input from a
// file.  Physical reads are performed only when required.
//
// Since C++ iostream logic is only set up for a max file size of 2 GB, the
// non standard C version of file position pointers are used.  This is ugly,
// but we have no choice, because we want to read large logger files.
//

#include "GenLoggerFileI.hpp"

#include <cstdlib>
#include <iostream>

#include "GenBuf.hpp"
#include "GenBufIManaged.hpp"
#include "UtLog.hpp"

GenLoggerFileI::GenLoggerFileI(const char* aFile)
   : GenI("GenLoggerFileI")
   , mStatus(0)
   , mDataSet(0)
   , mMultiplePass(1)
   , mDacRunTime(0.0F)
   , mDacTimeTag(0.0F)
   , mBuf(nullptr)
   , mDacBuf(nullptr)
   , mDataFileInfoPtr(nullptr)
   , mLogVersion(0)
{
   // Reserve DAC's vector size to minimize the copying when the container
   // needs to increase its size.
   mDacObjVec.reserve(500);

   // Name of the log data file
   mFileName = aFile;

   // Read log file
   Init(aFile);
}

// Overload constructor
GenLoggerFileI::GenLoggerFileI(const char* aFile, int aDataSet)
   : GenI("GenLoggerFileI")
   , mStatus(0)
   , mDataSet(0)
   , mMultiplePass(1)
   , mDacRunTime(0.0F)
   , mDacTimeTag(0.0F)
   , mBuf(nullptr)
   , mDacBuf(nullptr)
   , mDataFileInfoPtr(nullptr)
   , mLogVersion(0)
{
   // Reserve DAC's vector size to minimize the copying when the container
   // needs to increase its size.
   mDacObjVec.reserve(500);

   // Name of the log data file
   mFileName = aFile;

   // Read log file
   Init(aFile);

   // If opening logfile was a success, read the logfile data of the given
   // dataset and save them in mBuf.
   if (mStatus)
   {
      SetDataSet(aDataSet);
   }
}

GenLoggerFileI::~GenLoggerFileI()
{
   // File will get closed automatically when mFin's destructor is called
   delete mBuf;
   delete mDacBuf;
   delete mDataFileInfoPtr;
}

int GenLoggerFileI::SetDataSet(int aDataSet)
{
   // A sucess until proven otherwise
   mStatus  = 1;
   mDataSet = aDataSet;

   // -------------------------------------------------------------------------
   // This function may be called several times by the user, i.e.
   // SetDataSet(4), and then SetDataSet(5002).  Before we can process
   // the call for set Id 5002, clear the buffer containing the log details
   // (mbuf) and vector containing all DAC for data set 4.
   // -------------------------------------------------------------------------
   mDacObjVec.clear();
   SetGenBufXferI(mDacBuf);

   // Point to the begining of the buffer
   SetGetOffset(0, GenBuf::FromBeg);

   // Size of logger details
   unsigned int bufSize = 0;

   // A local dac for use in for loop
   LoggerDAC dac;
   // -------------------------------------------------------------------------
   // Load dataset's DAC requested by the user
   // -------------------------------------------------------------------------
   for (GENIO_UINT64 j = 0; j < mDataFileInfoPtr->mNumsets; ++j)
   {
      dac.Get(*this);

      if (mFin.eof())
      {
         // Premature end of file found
         auto out = ut::log::error() << "GenLoggerFileI::Init() hit a premature end of file.";
         out.AddNote() << "File: " << mFileName;
         out.AddNote() << "Data Set: " << mDataSet;

         mFin.close();
         mStatus = -2;
         break;
      }

      // Find data set that we want and store it in a vector
      if (dac.mSet_ID == aDataSet)
      {
         mDacObjVec.push_back(dac);
         bufSize += dac.mData_size;
      }
   } // End for loop

   // Initialize DAC Container interator
   mDacObjVecIter = mDacObjVec.begin();

   // Delete the current buffer.  This buffer may be the buffer that contains
   // the LoggerDataFileInfo or the one that contains the data of another
   // data set
   delete mBuf;

   // Create a new buffer to hold the data that the user want.
   // Take the default to not let the buffer grow (8meg)
   if (bufSize > 8388608)
   {
      mBuf = new GenBufIManaged(GenBuf::BigEndian, 8388608);
   }
   else
   {
      mBuf = new GenBufIManaged(GenBuf::BigEndian, bufSize);
   }

   return mStatus;

} // End SetDataSet()

// Return value: >= 0, number of bytes in the input buffer.
//                < 0, error (such as EOF)
int GenLoggerFileI::Receive(int /* aWaitTimeInMicroSec */)
{
   SetGenBufXferI(mBuf);
   ClearInputErrorStatus();

   unsigned long validBytes = mBuf->GetGetOffset(GenBuf::FromDataEnd);
   if (validBytes)
   {
      unsigned long getOffset = mBuf->GetGetOffset(GenBuf::FromBeg);

      auto out = ut::log::info() << "GenLoggerFileI::Receive called.";
      out.AddNote() << "Data Set: " << mDataSet;
      out.AddNote() << "Remaining Valid Bytes: " << validBytes;
      out.AddNote() << "Initial Valid Bytes: " << validBytes + getOffset;
   }

   // Clear the input buffer
   mBuf->Reset();

   unsigned int jcount;
   if (mMultiplePass)
   {
      jcount = 0xFFFFFFFF;
   }
   else
   {
      jcount = 1;
   }

   int numBytes = 0;
   for (; mDacObjVecIter != mDacObjVec.end() && jcount--; ++mDacObjVecIter)
   {
      // Be sure there is room in the buffer
      if (static_cast<unsigned int>((*mDacObjVecIter).mData_size) > mBuf->GetNumUnusedBytes())
      {
         break;
      }

      mFin.seekg(static_cast<long>((*mDacObjVecIter).mOffset), std::ios::beg);
      mFin.read(reinterpret_cast<char*>(mBuf->GetBuffer() + mBuf->GetNumValidBytes()), (*mDacObjVecIter).mData_size);

      // Data has been received.  Update the buffer pointer to reflect
      // the amount of data available.
      numBytes += (*mDacObjVecIter).mData_size;
      mBuf->SetNumValidBytes(numBytes);

      // Store time values in case we are in one step mode
      mDacRunTime = (*mDacObjVecIter).mMSIP_runtime;
      mDacTimeTag = (*mDacObjVecIter).mTime_tag;
   }

   return numBytes;

} // End Receive()


//
// Private
//

int GenLoggerFileI::Init(const char* aFile)
{
   // A sucess until proven otherwise
   mStatus = 1;

   mFin.open(aFile);
   if (!mFin.good()) // prefer mFin.is_open()
   {
      auto out = ut::log::error() << "GenLoggerFileI: Cannot open file.";
      out.AddNote() << "File: " << aFile;
      mStatus = -1;
   }
   else
   {
      // Buffer used to read LoggerDataFileInfo
      mBuf = new GenBufIManaged(GenBuf::BigEndian, 131072);
      SetGenBufXferI(mBuf);

      // Read information at the beginning of the logfile
      mDataFileInfoPtr = new LoggerDataFileInfo(mFin, *this, mLogVersion);

      // ----------------------------------------------------------------------
      // Find DAC data in log file and read them
      // ----------------------------------------------------------------------
      // Find position in the logfile where the DAC data starts and go
      // to it.
      mFin.seekg(static_cast<long>(mDataFileInfoPtr->mDAC_offset), std::ios::beg);

      // Create a humogous buffer to hold the DAC
      mDacBuf = new GenBufIManaged(GenBuf::BigEndian, static_cast<unsigned long>(mDataFileInfoPtr->mDAC_size));
      // Read all DAC data

      mFin.read(reinterpret_cast<char*>(mDacBuf->GetBuffer()), static_cast<std::streamsize>(mDataFileInfoPtr->mDAC_size));

      // Set how much DAC data that were read
      mDacBuf->SetNumValidBytes(static_cast<unsigned long>(mDataFileInfoPtr->mDAC_size));
   }

   return mStatus;

} // End Init(fileName)

GenLoggerFileI::LoggerDataSetInfo::LoggerDataSetInfo() {}

GenLoggerFileI::LoggerDataSetInfo::~LoggerDataSetInfo() {}

GenLoggerFileI::LoggerDataFileInfo::LoggerDataFileInfo() {}

GenLoggerFileI::LoggerDataFileInfo::LoggerDataFileInfo(std::ifstream& aFin, GenLoggerFileI& aGenI, unsigned int& aVersion)
{
   GenBufIManaged* buf      = aGenI.GetGenBufIManaged();
   unsigned int    numBytes = 0;

   aFin.read(reinterpret_cast<char*>(buf->GetBuffer()), sizeof(mLog_filename));
   numBytes += sizeof(mLog_filename);

   aFin.read(reinterpret_cast<char*>(buf->GetBuffer() + numBytes), sizeof(mFrame_time));
   numBytes += sizeof(mFrame_time);

   aFin.read(reinterpret_cast<char*>(buf->GetBuffer() + numBytes), sizeof(mNum_set_ids));
   numBytes += sizeof(mNum_set_ids);

   buf->SetNumValidBytes(numBytes);

   aGenI.GetArray(mLog_filename, 128);
   aGenI.Get(mFrame_time);
   aGenI.Get(mNum_set_ids);

   // Compute size of LoggerDataSetInfo, because orginal code writes whole
   // structure.
   // Writing whole structures is a bad thing, because it is not transportable
   // between machines.  We will do this goofy stuff below because SGI rounds
   // up to double word boundary.  Logger should only write defined types.
   unsigned int dataSetSize = sizeof(LoggerDataSetInfo);
   unsigned int tmp         = (dataSetSize + 7) / 8;
   dataSetSize              = tmp * 8 * mNum_set_ids;

   // Might need to make this smarter someday if number of data sets really
   // change alot.
   unsigned int dataToRead =
      dataSetSize + sizeof(mDAC_offset) + sizeof(mDAC_size) + sizeof(mNumsets) + sizeof(mVersion_number);
   if (buf->GetNumUnusedBytes() < dataToRead)
   {
      { // RAII block
         auto out = ut::log::fatal() << "GenLoggerFileI::LoggerDataFileInfo: Internal buffer overflow.";
         out.AddNote() << "Need bigger buffer.";
      }
      exit(1);
   }

   aFin.read(reinterpret_cast<char*>(buf->GetBuffer() + numBytes), dataSetSize);
   numBytes += dataSetSize;

   aFin.read(reinterpret_cast<char*>(buf->GetBuffer() + numBytes), sizeof(mDAC_offset));
   numBytes += sizeof(mDAC_offset);

   aFin.read(reinterpret_cast<char*>(buf->GetBuffer() + numBytes), sizeof(mDAC_size));
   numBytes += sizeof(mDAC_size);

   aFin.read(reinterpret_cast<char*>(buf->GetBuffer() + numBytes), sizeof(mNumsets));
   numBytes += sizeof(mNumsets);

   // Dana -- Jake
   // Are we using version 4 now ???????
   aFin.read(reinterpret_cast<char*>(buf->GetBuffer() + numBytes), sizeof(mVersion_number));
   numBytes += sizeof(mVersion_number);

   buf->SetNumValidBytes(numBytes);

   // Since we are not using any of the data in LoggerDataSetInfo,
   // just skip over it for now.
   aGenI.SetGetOffset(static_cast<long>(dataSetSize), GenBuf::FromCur);

   // Somedeay may need to do something like this...
   // Allocate the space and set the correct size then read the structs
   // mSetVec.resize(mNum_set_ids);
   // for(int j = 0 ; j < mNum_set_ids ; ++j)
   //{
   //   mSetVec[j].Get(aGenI);
   //}

   aGenI.Get(mDAC_offset);
   aGenI.Get(mDAC_size);
   aGenI.Get(mNumsets);
   aGenI.Get(mVersion_number);

   aVersion = static_cast<unsigned int>(mVersion_number * 10.0);

} // End LoggerDataFileInfo c'tor

GenLoggerFileI::LoggerDataFileInfo::~LoggerDataFileInfo() {}

// Overloaded constructor
GenLoggerFileI::LoggerDAC::LoggerDAC() {}

void GenLoggerFileI::LoggerDAC::Get(GenI& aGenI)
{
   aGenI.Get(mSet_ID);
   aGenI.Get(mData_type);
   aGenI.Get(mMSIP_runtime);
   aGenI.Get(mTime_tag);
   aGenI.Get(mBody_number);
   aGenI.Get(mData_size);
   aGenI.Get(mOffset);
}

GenLoggerFileI::LoggerDAC::~LoggerDAC() {}
