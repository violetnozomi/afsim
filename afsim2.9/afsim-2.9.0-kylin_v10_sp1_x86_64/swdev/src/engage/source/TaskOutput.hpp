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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef TASKOUTPUT_HPP
#define TASKOUTPUT_HPP

#include <iomanip>
#include <iosfwd>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "Events.hpp"

class WsfScenario;
#include "InputConfig.hpp"
#include "SummaryData.hpp"
#include "UtLength.hpp"
#include "UtSpeed.hpp"

namespace engage
{

class RunConfig;
class Simulation;
class Task;

//! This class is the run-time output controller for the tasks.
class TaskOutput
{
public:
   static const int cENTIRE_TABLE = -1;
   // Might want to make these user defined in the future for more
   // user control of Pk Table output formatting
   static const int cPADDING_WIDTH = 15; // This should be enough padding for values
   // Set precision could be used otherwise
   static const char cDELIMITER = ' ';

   typedef std::map<std::string, std::ostream*> OpenFileMap;

   TaskOutput(int aThreadId, const Task& aTask, Simulation& aSimulation);

   void OpenFiles();
   void CloseFiles();
   void Write(double aSimTime);
   void WriteEvent(double aSimTime);
   void WriteTaskSummary();

   Events&      GetEvents() { return mEvents; }
   SummaryData& GetTaskSummary() { return mTaskSummary; }

   static void WriteSummary();
   static void CloseAllFiles();

   static void         ResetSummary(const InputConfig& aInputConfig);
   static SummaryData& GetSummaryData(int aRunIndex, int aSiteIndex);
   static void         ReleaseRunSummary();

   //! Pk Table Methods
   static void InitializePkOutput(const InputConfig::RunList* aRunList, int aNumPkRuns);
   static void InsertPk(double aPk, const RunConfig& aRunConfig, int aTaskNum, int aTargetTaskNum);

   static void FlushPk(const RunConfig& aRunConfig, int aRunIndex = cENTIRE_TABLE, int aAltitudeIndex = cENTIRE_TABLE);

private:
   static void AppendIntermediateFile(const std::string& aPrimaryFileName, int aTaskIndex);
   static void CloseAllFiles(OpenFileMap& aOpenFileMap);

   static std::string CreatePkStream(const std::string& aLengthUnit, int aRunIndex, int aAltitudeIndex);

   static std::string
   CreatePkHeader(const std::string& aLengthUnit, const std::string& aSpeedUnit, int aRunIndex, int aAltitudeIndex);

   static std::ostream* FindOpenFile(OpenFileMap&       aOpenFileMap,
                                     const std::string& aPrimaryFileName,
                                     const std::string& aActualFileName);

   static void GetPkIndex(int&          aAltIndex,
                          int&          aCrIndex,
                          int&          aDrIndex,
                          const double& aAltitude,
                          const double& aCr,
                          const double& aDr,
                          int           aRun);

   //! Template for sane looking output of Pk tables
   template<typename T>
   static std::string FormatElement(T t)
   {
      std::stringstream ss;
      ss << std::left << std::setw(cPADDING_WIDTH) << std::setfill(cDELIMITER) << t;
      return ss.str();
   }

   static std::string MakeIntermediateFileName(const std::string& aPrimaryFileName, int aTaskIndex);
   static void        ProcessQueuedOutput();

   //! The ID of the thread executing the task. 0 if single threaded.
   int mThreadId;

   Simulation& mSimulation;

   const Task& mTask;

   //! Pointers to the task-specific 'intermediate' output files for multi-threading.
   OpenFileMap mOpenFileMap;

   //! Pointers to output files, index by output slot number.
   //! @note More than one slot may contain the same file pointer as multiple slots may use the same file.
   std::vector<std::ostream*> mFilePtrs;

   //! Files to received event output
   // std::vector<std::ostream*>       mEventFilePtrs;

   //! Files to receive summary output
   std::vector<std::ostream*> mSummaryFilePtrs;

   //! Task summary data for output
   SummaryData mTaskSummary;

   //! Time when slot last produced output.
   std::vector<double> mLastOutputTime;

   //! Time when slot last produced event output.
   std::vector<double> mLastEventOutputTime;

   //! Events class to handle event output.
   Events mEvents;
};

} // namespace engage

#endif
