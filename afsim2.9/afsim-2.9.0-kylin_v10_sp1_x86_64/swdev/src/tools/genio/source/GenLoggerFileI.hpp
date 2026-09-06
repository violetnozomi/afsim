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
//
// GenLoggerFileI - A derived class of GenI that does buffered input on a
// logger file.  Physical reads are performed only when required.
//
// There are two ways to construct a GenLoggerFileI object.  The first method
// is with a logfile name, and the second is with a logfile name and a
// dataset Id.
//
// 1.  GenLoggerFile(const char* aFile)
//     a. Open logfile and read all DAC's at the bottom of the file.
//     b. To retrieve the needed data, the user will call
//        SetDataSet(int aDataSet).  This may be called several times to
//        get the desired data.  For example: SetDataSet(4) for DIS type
//        data and SetDataSet(5001) for VTrack type.
//
// 2.  GenLoggerFile(const char* aFile, int aDataSetId)
//     a. Open logfile and read all DAC's at the bottom of the file.
//     b. Read all data that has DataSetId = aDataSetId
//     c. To get data with a different dataSetId, the user has to
//        instantiate another GenLoggerFile.
//
//     First method                     Second method
//     ------------                     -------------
//     GenLoggerFile genLog(sep31.log)  GenLoggerFile genLog1(sep31.log, 4)
//      genLog.SetDataSet(4)            GenLoggerFile genLog2(sep31.log, 5001)
//      genLog.SetDataSet(5001)         GenLoggerFile genLog3(sep31.log, 5002)
//      genLog.SetDataSet(5002)
//
// Method 1 is a prefered method since all DAC's at the end of the logfile
// is read only once.  Method 2 is implemented to accomodate the current
// users who have not change their codes to use method 1.
//

#ifndef GEN_LOGGER_FILE_I_HPP
#define GEN_LOGGER_FILE_I_HPP

#include "genio_export.h"

#include <fstream>
#include <string>
#include <vector>

#include "GenI.hpp"

class GenBufIManaged;

class GENIO_EXPORT GenLoggerFileI : public GenI
{
public:
   // Open the in a read only mode file.
   //   aFile    : Logger file to read.
   explicit GenLoggerFileI(const char* aFile);

   // Overload construtor with logfile and dataset.  This method will
   // open logfile, read all DAC's, and retreive the DAC's of the
   // requested dataset.
   GenLoggerFileI(const char* aFile, int aDataSet);
   ~GenLoggerFileI() override;

   // Return value: >= 0, number of bytes in the input buffer.
   //                < 0, error (such as EOF)
   int Receive(int aWaitTimeInMicroSec) override;

   // Returns 1 if all is good.
   int GetStatus() const;

   inline int GetDacSize() const;

   // Returns the times in the current DAC structure
   float GetDacRuntime() const;
   float GetDacTimeTag() const;

   // SetDataSet is called after the user has instantiated a
   // GenLoggerFileI with a logfile.
   // For a given dataset Id, get all the DAC's and save into a vector,
   // mDacObjVec.  The user may call this method more than once, each time
   // with a dataset Id of interest.
   // For example: SetDataSet(4)    retreives all DAC's of DIS type
   //              SetDataSet(5003) retreives all DAC's of VTrack type data
   // Return < 0 if error
   int SetDataSet(int aDataSet);

   // Set to non zero for the Receive method to read multiple DACs per
   // call (default)
   // Set to zero for the Receive method to read one DAC per call
   void SetMultipleReceiveDac(unsigned int aFlag);

private:
   // Disallow the user to make an intantiation of this class
   // without giving it a filename.
   GenLoggerFileI();

   GenBufIManaged* GetGenBufIManaged();
   GenBufIManaged* GetDacGenBufIManaged();

   // Method to read the DAC portion at the end of the log file and save
   // it in a buffer, mDacBuf
   int Init(const char* aFile);

   struct LoggerDataSetInfo
   {
      LoggerDataSetInfo();
      ~LoggerDataSetInfo();
      int          mID;
      char         mName[64];
      int          mDefault_port;
      int          mColor;
      GENIO_UINT64 mNumber_of_sets;
      float        mMSIP_first_time_tag;
      float        mUser_first_time_tag;
      float        mMSIP_most_recent_time_tag;
      float        mUser_most_recent_time_tag;
   };

   struct LoggerDataFileInfo
   {
      // Default constructor
      LoggerDataFileInfo();

      LoggerDataFileInfo(std::ifstream& aIny, GenLoggerFileI& aGenI, unsigned int& aVersion);

      ~LoggerDataFileInfo();
      char                           mLog_filename[128];
      float                          mFrame_time;
      int                            mNum_set_ids;
      std::vector<LoggerDataSetInfo> mSetVec;
      GENIO_UINT64                   mDAC_offset;
      GENIO_UINT64                   mDAC_size;
      GENIO_UINT64                   mNumsets;
      float                          mVersion_number;
   };

   struct LoggerDAC
   {
      LoggerDAC();
      ~LoggerDAC();

      // Method to get the DAC's data members from the DAC buffer
      // (mDacBuf)
      void Get(GenI& aGenI);

      int          mSet_ID;
      int          mData_type;
      float        mMSIP_runtime;
      float        mTime_tag;
      int          mBody_number;
      int          mData_size;
      GENIO_UINT64 mOffset;
   };

   std::ifstream mFin;

   int          mStatus;
   int          mDataSet;
   unsigned int mMultiplePass;
   float        mDacRunTime;
   float        mDacTimeTag;

   // Logfile name used for printing error
   std::string mFileName;

   // This buffer is used for two different purposes:
   // 1.  To read LoggerDataFileInfo
   // 2.  To store desired data of a given dataSet Id
   // Before using it to store the user request data, the buffer that
   // containes the LoggerDataFileInfo will be deleted.
   GenBufIManaged* mBuf;

   // Buffer contains all DAC data
   GenBufIManaged* mDacBuf;

   // A list containing all the DAC's of a given dataset Id such as
   // DIS (dataset Id = 4) or VTrack (dataset Id = 5003)
   std::vector<LoggerDAC>           mDacObjVec;
   std::vector<LoggerDAC>::iterator mDacObjVecIter;

   // This member is saved to retreive the number of sets in the log file.
   // We could just save the number of sets only.  But... save the whole
   // struct just in case other info is needed.
   LoggerDataFileInfo* mDataFileInfoPtr;

   // LoggerDataFileInfo's mVersion_number*10.0
   unsigned int mLogVersion;

   friend struct LoggerDataSetInfo;
   friend struct LoggerDataFileInfo;
};

inline int GenLoggerFileI::GetStatus() const
{
   return mStatus;
}

inline int GenLoggerFileI::GetDacSize() const
{
   return static_cast<int>(mDacObjVec.size());
}

inline GenBufIManaged* GenLoggerFileI::GetGenBufIManaged()
{
   return mBuf;
}

inline GenBufIManaged* GenLoggerFileI::GetDacGenBufIManaged()
{
   return mDacBuf;
}

inline float GenLoggerFileI::GetDacRuntime() const
{
   return mDacRunTime;
}

inline float GenLoggerFileI::GetDacTimeTag() const
{
   return mDacTimeTag;
}

inline void GenLoggerFileI::SetMultipleReceiveDac(unsigned int aFlag)
{
   mMultiplePass = aFlag;
}

#endif
