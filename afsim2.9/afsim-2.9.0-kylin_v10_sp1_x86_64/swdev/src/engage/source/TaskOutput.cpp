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
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "TaskOutput.hpp"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <queue>

#include "OutputConfig.hpp"
#include "RunConfig.hpp"
#include "Simulation.hpp"
#include "Task.hpp"
#include "UtLength.hpp"
#include "UtPath.hpp"
#include "UtSpeed.hpp"
#include "WsfScenario.hpp"

namespace engage
{

namespace
{
//! Mutex for accessing the map of primary output files.
std::recursive_mutex sPrimaryFilesMutex;

//! Maps the primary output files to their associated stream pointers.
TaskOutput::OpenFileMap sPrimaryFiles;

//! Vector of summary output stream pointers.
std::vector<std::ostream*> sSummaryFilePtrs;

//! Summary data for runs and sites.
static std::vector<RunSummary> sRunSummary;

//! Mutex for accessing the summary output data.
static std::recursive_mutex sRunSummaryMutex;

//! Mutex for accessing the output queue.
std::recursive_mutex sOutputQueueMutex;

//! The output queue.
std::priority_queue<int, std::vector<int>, std::greater<int>> sOutputQueue;

typedef std::list<std::string>  FileList;
typedef std::map<int, FileList> OutputQueueFiles;

//! A map of intermediate files that need to be copied to the primary file.
OutputQueueFiles sOutputQueueFiles;

// The task index of the next task whose output must be copied to the primary files.
int sNextOutputTaskIndex(0);

// =================================================================================================
// PkTable Vars
// Note that for simplicity, all stored variables are in Util standard units
// m for length, m/s for velocity, etc.
// A conversion utility will be provided on output of tables to user set values

//! A struct object containing a double representing a Pk Value, and
//! a integer value representing how many Pk Values have been avereged into
//! the resulting Pk Value
struct PkEntry
{
   double mPkVal;
   int    mNumVals;
   PkEntry()
      : mPkVal(0.0)
      , mNumVals(0)
   {
   }
};

//! A three-dimensional vector representing Pk values mapping to indexes based
//! on Altitude, Crossrange, and Downrange values
typedef std::vector<std::vector<std::vector<PkEntry>>> PkVector;

//! Add another dimension for multiple runs. It is possible to have output for both
//! at the same time
typedef std::vector<PkVector> RunsPkVector;
RunsPkVector                  sPkVector;

//! Static vectors to hold expected floating point values for each run
//! These are held so we don't have to recompute these values every time
//! indexing occurs. Encountered values should equate exactly or very nearly to these
static std::vector<std::vector<double>> sCrVals;
static std::vector<std::vector<double>> sDrVals;
static std::vector<std::vector<double>> sAltitudeVals;

//! A vector that increments every time a Pk value is entered associated with a
//! specific altitude value. Allows for constant time lookup to determine when
//! it is appropriate to write a Pk table to disc without having to constantly
//! loop through the stored Pk values
static std::vector<std::vector<int>> sFilledPks;

//! The target type, indexed by run
static std::vector<std::string> sPkTargetType;

//! The site type, indexed by run
static std::vector<std::string> sPkSiteType;

//! The target velocity/speed, indexed by run
static std::vector<double> sPkTargetVelocity;

//! The top level directory path for table output, indexed by run
static std::vector<std::string> sPkOutputPath;

//! The number of tasks for each run
static std::vector<int> sPkNumRuns;

//! Mutex for accessing the Pk Vector
static std::recursive_mutex sRunPkVectorMutex;

//! Mutex for accessing the vector containing number of Pks entered
static std::recursive_mutex sFilledPksMutex;
} // namespace

// =================================================================================================
TaskOutput::TaskOutput(int aThreadId, const Task& aTask, Simulation& aSimulation)
   : mThreadId(aThreadId)
   , mSimulation(aSimulation)
   , mTask(aTask)
   , mOpenFileMap()
   , mFilePtrs()
   , mSummaryFilePtrs()
   , mTaskSummary()
   , mLastOutputTime()
   , mLastEventOutputTime()
   , mEvents()
{
}

// =================================================================================================
void TaskOutput::OpenFiles()
{
   const RunConfig::OutputList& outputList(mTask.GetRunConfig().GetOutputList());
   size_t                       outputCount(outputList.size());

   // Open the primary output file(s) if they are not yet open.

   // Loop through the list of output slots and open the necessary files.
   mFilePtrs.resize(outputCount, nullptr);
   mLastOutputTime.resize(outputCount, -1.0E6);
   mLastEventOutputTime.resize(outputCount, -1.0E6);
   size_t outputIndex = 0;
   for (RunConfig::OutputList::const_iterator oli = outputList.begin(); oli != outputList.end(); ++oli)
   {
      const OutputConfig& output(*oli);
      std::string         fileName(output.GetFileName());
      mFilePtrs[outputIndex] = nullptr;
      if (!fileName.empty())
      {
         // Open the primary output file if it is not already open.
         sPrimaryFilesMutex.lock();
         std::ostream* filePtr = FindOpenFile(sPrimaryFiles, fileName, fileName);
         if (output.DoSummaryOutput() &&
             (std::find(sSummaryFilePtrs.begin(), sSummaryFilePtrs.end(), filePtr) == sSummaryFilePtrs.end()))
         {
            sSummaryFilePtrs.push_back(filePtr);
         }
         sPrimaryFilesMutex.unlock();

         // If running multi-threaded, open the intermediate file if it is not already open.
         if (mThreadId > 0)
         {
            std::string intFileName(MakeIntermediateFileName(fileName, mTask.GetTaskIndex()));
            filePtr = FindOpenFile(mOpenFileMap, fileName, intFileName);
         }
         mFilePtrs[outputIndex] = filePtr;

         if (output.DoSummaryOutput() &&
             (std::find(mSummaryFilePtrs.begin(), mSummaryFilePtrs.end(), filePtr) == mSummaryFilePtrs.end()))
         {
            mSummaryFilePtrs.push_back(filePtr);
         }
      }
      ++outputIndex;
   }
}

// =================================================================================================
void TaskOutput::CloseFiles()
{
   // 'mOpenFileMap' is non-empty only when running multi-threaded.
   if (!mOpenFileMap.empty())
   {
      // Build the list of files that need copying.
      FileList fileList;
      for (OpenFileMap::const_iterator ofmi = mOpenFileMap.begin(); ofmi != mOpenFileMap.end(); ++ofmi)
      {
         fileList.push_back((*ofmi).first);
      }
      CloseAllFiles(mOpenFileMap);

      // Add an output queue entry for the task in the output queue.
      int taskIndex = mTask.GetTaskIndex();
      sOutputQueueMutex.lock();
      sOutputQueue.push(taskIndex);
      sOutputQueueFiles[taskIndex] = fileList;
      sOutputQueueMutex.unlock();

      // Process any queued intermediate output that can be processed at this time.
      ProcessQueuedOutput();
   }
}

// =================================================================================================
//! Write the variables to the files that have requested variable output.
void TaskOutput::Write(double aSimTime)
{
   const RunConfig::OutputList& outputList(mTask.GetRunConfig().GetOutputList());

   size_t outputIndex(0);
   for (RunConfig::OutputList::const_iterator oli = outputList.begin(); oli != outputList.end(); ++oli)
   {
      const OutputConfig&           output(*oli);
      const OutputConfig::ItemList& itemList(output.GetOutputItems());
      if ((!itemList.empty()) && output.DoPeriodicOutput(mSimulation.GetPhase(), aSimTime, mLastOutputTime[outputIndex]))
      {
         std::ostream& os(*mFilePtrs[outputIndex]);
         for (OutputConfig::ItemList::const_iterator ili = itemList.begin(); ili != itemList.end(); ++ili)
         {
            (*ili).DoOutput(os, mSimulation);
         }
         os << '\n';
         mLastOutputTime[outputIndex] = aSimTime; // Save when output last done for this output block
      }
      ++outputIndex;
   }
}

// =================================================================================================
//! Write the events to the files that have requested event output.
void TaskOutput::WriteEvent(double aSimTime)
{
   const RunConfig::OutputList& outputList(mTask.GetRunConfig().GetOutputList());

   size_t outputIndex(0);
   for (RunConfig::OutputList::const_iterator oli = outputList.begin(); oli != outputList.end(); ++oli)
   {
      const OutputConfig&             output(*oli);
      const OutputConfig::EventItems& eventItems(output.GetEventItems());
      if (output.DoEventOutput() && (! eventItems.empty())/* &&
          output.DoPeriodicOutput(mSimulation.GetPhase(), aSimTime, mLastEventOutputTime[outputIndex])*/)
      {
         std::ostream& os(*mFilePtrs[outputIndex]);
         // os << "Task " << mTask.GetTaskIndex() << ": T=" << aSimTime << " Event Output" << endl;
         Events::EventList& eventList = mEvents.GetEventList();
         for (Events::EventList::const_iterator eli = eventList.begin(); eli != eventList.end(); ++eli)
         {
            for (OutputConfig::EventItems::const_iterator eii = eventItems.begin(); eii != eventItems.end(); ++eii)
            {
               if ((((*eii).mEventType == "all") && (*eii).mEnabled) ||
                   (((*eii).mEventType == (*eli)->mEventType) && (*eii).mEnabled))
               {
                  os << "Task " << mTask.GetTaskIndex() + 1 << ":";
                  (*eli)->Write(os, mSimulation);
                  os << '\n';
               }
            }
         }
         mLastEventOutputTime[outputIndex] = aSimTime; // Save when output last done for this output block
      }
      ++outputIndex;
   }
   mEvents.ClearAllEvents(); // Clear all the events as they have been written to all files now
}

// =================================================================================================
//! Write the summary to the files that have requested summary output.
void TaskOutput::WriteTaskSummary()
{
   for (size_t i = 0; i < mSummaryFilePtrs.size(); ++i)
   {
      std::ostream& os = *(mSummaryFilePtrs[i]);
      os << "Task " << mTask.GetTaskIndex() << ": SUMMARY_OUTPUT:";
      os << " ShotCount=" << mTaskSummary.mShotCount;
      os << " NonZeroPkShotCount=" << mTaskSummary.mNonZeroPkShotCount;
      os << " NonZeroPkSum=" << mTaskSummary.mNonZeroPkSum;
      if (mTaskSummary.mNonZeroPkShotCount > 0)
      {
         os << " NonZeroPkavg=" << mTaskSummary.mNonZeroPkSum / mTaskSummary.mNonZeroPkShotCount;
      }
      else
      {
         os << " NonZeroPkavg=" << 0;
      }
      os << std::endl;
   }
}

// =================================================================================================
// static
void TaskOutput::ResetSummary(const InputConfig& aInputConfig)
{
   const InputConfig::RunList& runList(aInputConfig.GetRunList());

   sRunSummary.resize(runList.size());

   size_t runIndex = 0;
   for (InputConfig::RunList::const_iterator rli = runList.begin(); rli != runList.end(); ++rli)
   {
      const SiteConfig& siteConfig((*rli).GetSiteConfig());
      sRunSummary[runIndex].mSiteSummary.resize(siteConfig.GetPointCount());
      ++runIndex;
   }
}

// =================================================================================================
// static
SummaryData& TaskOutput::GetSummaryData(int aRunIndex, int aSiteIndex)
{
   sRunSummaryMutex.lock();
   return sRunSummary[aRunIndex].mSiteSummary[aSiteIndex];
}

// =================================================================================================
// static
void TaskOutput::ReleaseRunSummary()
{
   sRunSummaryMutex.unlock();
}

// =================================================================================================
// static
void TaskOutput::WriteSummary()
{
   for (size_t i = 0; i < sSummaryFilePtrs.size(); ++i)
   {
      std::ostream& os = *sSummaryFilePtrs[i];
      for (size_t i = 0; i < sRunSummary.size(); ++i)
      {
         RunSummary runSum = sRunSummary[i];
         for (size_t j = 0; j < runSum.mSiteSummary.size(); ++j)
         {
            RunSummary::SiteSummary siteSum = runSum.mSiteSummary[j];
            os << "SUMMARY_OUTPUT:";
            os << " RunIndex: " << i;
            os << " SiteIndex=" << j;
            os << " ShotCount=" << siteSum.mShotCount;
            os << " NonZeroPkShotCount=" << siteSum.mNonZeroPkShotCount;
            os << " NonZeroPkSum=" << siteSum.mNonZeroPkSum;
            if (siteSum.mNonZeroPkShotCount > 0)
            {
               os << " NonZeroPkavg=" << siteSum.mNonZeroPkSum / siteSum.mNonZeroPkShotCount;
            }
            else
            {
               os << " NonZeroPkavg=" << 0;
            }
            os << std::endl;
         }
      }
   }

   std::cout << "\nSUMMARY_OUTPUT:" << std::endl;
   for (size_t i = 0; i < sRunSummary.size(); ++i)
   {
      std::cout << "  RunIndex: " << i << std::endl;
      RunSummary runSum = sRunSummary[i];
      for (size_t j = 0; j < runSum.mSiteSummary.size(); ++j)
      {
         RunSummary::SiteSummary siteSum = runSum.mSiteSummary[j];
         std::cout << "    SiteIndex=" << j;
         std::cout << " ShotCount=" << siteSum.mShotCount;
         std::cout << " NonZeroPkShotCount=" << siteSum.mNonZeroPkShotCount;
         std::cout << " NonZeroPkSum=" << siteSum.mNonZeroPkSum;
         if (siteSum.mNonZeroPkShotCount > 0)
         {
            std::cout << " NonZeroPkavg=" << siteSum.mNonZeroPkSum / siteSum.mNonZeroPkShotCount;
         }
         else
         {
            std::cout << " NonZeroPkavg=" << 0;
         }
         std::cout << std::endl;
      }
   }
}

// =================================================================================================
// static
void TaskOutput::CloseAllFiles()
{
   CloseAllFiles(sPrimaryFiles);
}

// =================================================================================================
//! Take the output from a task-specific intermediate file and append it to the assoicated primary file.
//! @param aPrimaryFileName [input] The primary file name.
//! @param aTaskIndex       [input] The task index whose intermediate output is to appended.

// private static
void TaskOutput::AppendIntermediateFile(const std::string& aPrimaryFileName, int aTaskIndex)
{
   std::string   intFileName(MakeIntermediateFileName(aPrimaryFileName, aTaskIndex));
   std::ifstream ifs(intFileName.c_str());
   if (!ifs.is_open())
   {
      std::cout << "***** ERROR: Unable to open intermediate file " << intFileName << " for input" << std::endl;
      return;
   }
   std::ostream* osPtr = FindOpenFile(sPrimaryFiles, aPrimaryFileName, aPrimaryFileName);
   if (osPtr == nullptr)
   {
      std::cout << "***** ERROR: Unable to open primary output file " << aPrimaryFileName << std::endl;
      return;
   }
   // cout << "----- appending " << intFileName << " to " << aPrimaryFileName << endl;
   std::string line;
   while (std::getline(ifs, line))
   {
      (*osPtr) << line << '\n';
   }
   ifs.close();
   if (std::remove(intFileName.c_str()) != 0)
   {
      std::cout << "***** WARNING: Unable to remove intermediate file " << intFileName << std::endl;
   }
}

// =================================================================================================
//! Close all files in the specified open file map.
//! @param aOpenFileMap [input] The files to be closed.
// private static
void TaskOutput::CloseAllFiles(OpenFileMap& aOpenFileMap)
{
   for (OpenFileMap::iterator ofmi = aOpenFileMap.begin(); ofmi != aOpenFileMap.end(); ++ofmi)
   {
      std::ostream* osPtr((*ofmi).second);
      delete osPtr;
   }
   aOpenFileMap.clear();
}

// =================================================================================================
//! Open a primary or intermediate file.
//!
//! This routine will determine the file 'aActualFileName' is already open. If so, it will return
//! the stream pointer to the file that is already open. If it is not open, it will open the file
//! and return the stream pointer.
//!
//! @param aOutputFileMap [modified] The 'open file' map which contains the list of 'primary' file
//!                                  names and associated stream pointers.
//! @param aPrimaryFileName [input]  They name of the primary file used as the key for the file map.
//! @param aActualFileName  [input]  The name of the actual file to be opened if the requested key
//!                                  does not exist in the map.
//! @returns A pointer to the stream associated with aActualFileName.
//!
//! @note aPrimaryFileName and aActualFileName are the same when opening a primary file. They are
//! different when opening an intermediate file. The names of the actual and primary files are
//! related as defined by the routine MakeIntermediateFileName().
// private static
std::ostream* TaskOutput::FindOpenFile(OpenFileMap&       aOpenFileMap,
                                       const std::string& aPrimaryFileName,
                                       const std::string& aActualFileName)
{
   OpenFileMap::iterator ofmi  = aOpenFileMap.find(aPrimaryFileName);
   std::ostream*         osPtr = nullptr;
   if (ofmi == aOpenFileMap.end())
   {
      // File is not open, so open it.
      std::ofstream* ofsPtr = new std::ofstream(aActualFileName.c_str());
      if ((ofsPtr != nullptr) && (ofsPtr->is_open()))
      {
         osPtr = ofsPtr;
         aOpenFileMap.insert(std::pair<std::string, std::ostream*>(aPrimaryFileName, osPtr));
      }
      else
      {
         std::cout << "***** ERROR: Unable to open output file " << aActualFileName << std::endl;
         delete ofsPtr;
         osPtr = nullptr;
      }
   }
   else
   {
      // File is open. Simply return the pointer to the existing stream.
      osPtr = (*ofmi).second;
   }
   return osPtr;
}

// =================================================================================================
//! Given the primary file name and task index, create the associated intermediate output file name.
//! This is used when multi-threading is employed in order to create the task-specific file name
//! in which the output of the task will be stored until it can be copied to the primary output file.
// private static
std::string TaskOutput::MakeIntermediateFileName(const std::string& aPrimaryFileName, int aTaskIndex)
{
   std::ostringstream oss;
   oss << aPrimaryFileName << ".t" << aTaskIndex;
   return oss.str();
}

// =================================================================================================
//! Process any queued intermediate output that is eligible to be copied to the primary output.
//! This is called when the simulation has completed output processing. It will determine if any
//! of the currently queued intermediate output should be copied to the primary output.
// private static
void TaskOutput::ProcessQueuedOutput()
{
   bool endLoop(false);
   while (!endLoop)
   {
      FileList fileList;

      // Examine the top entry of the output queue to determine if it is the one that should be processed at
      // this time. Exit the loop if their is no output queued or if it is one that should be processed later.
      sOutputQueueMutex.lock();
      int taskIndex = (sOutputQueue.empty() ? -999 : sOutputQueue.top());
      if (taskIndex == sNextOutputTaskIndex)
      {
         sOutputQueue.pop();
         OutputQueueFiles::iterator oqfi = sOutputQueueFiles.find(taskIndex);
         if (oqfi != sOutputQueueFiles.end())
         {
            fileList = (*oqfi).second;
            sOutputQueueFiles.erase(oqfi);
         }
      }
      else
      {
         endLoop = true;
      }
      sOutputQueueMutex.unlock();

      if (!endLoop)
      {
         // cout << "----- Writing output for task " << taskIndex << endl;
         for (FileList::iterator fli = fileList.begin(); fli != fileList.end(); ++fli)
         {
            AppendIntermediateFile(*fli, taskIndex);
         }

         sOutputQueueMutex.lock();
         ++sNextOutputTaskIndex;
         sOutputQueueMutex.unlock();
      }
   }
}

// =================================================================================================
// static
//! In the case of user requested output of Pk Table data, this method must be called prior to usage
//! of the other Pk Table generation methods to initialize the static containers and set variables
//! for defined behavior. The InputConfig::RunList is used to populate this data here. This method
//! should not be called unless Pk data is required, as the time complexity of this method is
//! prohibitive and could needlessly delay initialization unless required
void TaskOutput::InitializePkOutput(const InputConfig::RunList* aRunList, int aNumPkRuns)
{
   // This should be called prior to thread spawning, so a mutex is generally unnecessary
   // However, sometimes people do crazy things
   std::lock_guard<std::recursive_mutex> lock(sRunPkVectorMutex);

   // Define the size of the vectors used for indexing, and fill them with the appropriate values
   sAltitudeVals.resize(aNumPkRuns);
   sCrVals.resize(aNumPkRuns);
   sDrVals.resize(aNumPkRuns);
   // Define the size of the vector used for storing the completed run count
   sFilledPks.resize(aNumPkRuns);
   // Store the velocity/speed values being used for each run, for ease of output
   sPkTargetVelocity.resize(aNumPkRuns);
   // Store the target and site name type used for each run, for ease of output
   sPkTargetType.resize(aNumPkRuns);
   sPkSiteType.resize(aNumPkRuns);
   // Define the size of the number of runs vector
   sPkNumRuns.resize(aNumPkRuns);


   int runNumber = 0;
   for (auto it = aRunList->begin(); it != aRunList->end(); ++it)
   {
      if (it->IsPkTableGen())
      {
         TargetConfig tempTargetConfig = it->GetTargetConfig();
         SiteConfig   tempSiteConfig   = it->GetSiteConfig();

         // Get altitude values
         sAltitudeVals[runNumber].resize(tempTargetConfig.GetGridAltCount(), 0.0);
         for (unsigned int i = 0; i < sAltitudeVals[runNumber].size(); ++i)
         {
            sAltitudeVals[runNumber][i] = tempTargetConfig.GetGridAltMin() + tempTargetConfig.GetGridAltStep() * i;
         }

         // Set the number of differing altitude values in incrementing count vector
         sFilledPks[runNumber].resize(tempTargetConfig.GetGridAltCount());

         // Get crossrange values
         sCrVals[runNumber].resize(tempSiteConfig.GetGridCrCount(), 0.0);
         for (unsigned int i = 0; i < sCrVals[runNumber].size(); ++i)
         {
            sCrVals[runNumber][i] = tempSiteConfig.GetGridCrMin() + tempSiteConfig.GetGridCrStep() * i;
         }

         // Get downrange values
         sDrVals[runNumber].resize(tempSiteConfig.GetGridDrCount(), 0.0);
         for (unsigned int i = 0; i < sDrVals[runNumber].size(); ++i)
         {
            sDrVals[runNumber][i] = tempSiteConfig.GetGridDrMin() + tempSiteConfig.GetGridDrStep() * i;
         }

         // Set velocity/speed values
         double curSpeed = it->GetTargetConfig().GetGridSpeed();
         // double curMach = it->GetTargetConfig().GetGridMach();     //Not currently set in the runlist
         sPkTargetVelocity[runNumber] = curSpeed;
         // sTargetVelocity[runNumber] = curMach;

         // Set target and site type
         sPkTargetType[runNumber] = it->GetPkTargetType();
         sPkSiteType[runNumber]   = it->GetPkSiteType();

         // Set number of tasks for this particular run
         sPkNumRuns[runNumber] = it->GetTaskCount();

         runNumber++;
      }
   }

   // Allocate the size of the PkTable values vector
   // 1st index -> run
   // 2nd index -> altitude
   // 3rd index -> crossrange
   // 4th index -> downrange

   // Resize the 1st index
   sPkVector.resize(aNumPkRuns);

   // For the following dimensions of the PkTable, use the indexing vector sizes
   // Set altitude sizes (i loop)
   for (size_t i = 0; i < sPkVector.size(); ++i)
   {
      size_t altitudeSize = sAltitudeVals[i].size();
      sPkVector[i].resize(altitudeSize);

      // Set crossrange sizes (j loop)
      for (size_t j = 0; j < sPkVector[i].size(); ++j)
      {
         size_t crSize = sCrVals[i].size();
         sPkVector[i][j].resize(crSize);

         // Set downrange sizes (k loop)
         for (size_t k = 0; k < sPkVector[i][j].size(); ++k)
         {
            size_t drSize = sDrVals[i].size();
            sPkVector[i][j][k].resize(drSize);
         }
      }
   }

   // Set the pathing variables for output
}

// =================================================================================================
// static
//! Private method for finding the correct index into our Pk Table data container, keeping in mind
//! possible variances in floating point accuracy
void TaskOutput::GetPkIndex(int&          aAltIndex,
                            int&          aCrIndex,
                            int&          aDrIndex,
                            const double& aAltitude,
                            const double& aCr,
                            const double& aDr,
                            int           aRun)
{
   aAltIndex = -1;
   aCrIndex  = -1;
   aDrIndex  = -1;

   // Find the values, using an epsilon to avoid direct matching of floating point values
   for (size_t i = 0; i < sAltitudeVals[aRun].size(); ++i)
   {
      if (UtMath::NearlyEqual(sAltitudeVals[aRun][i], aAltitude))
      {
         aAltIndex = static_cast<int>(i);
         break;
      }
   }

   for (size_t i = 0; i < sCrVals[aRun].size(); ++i)
   {
      if (UtMath::NearlyEqual(sCrVals[aRun][i], aCr))
      {
         aCrIndex = static_cast<int>(i);
         break;
      }
   }

   for (size_t i = 0; i < sDrVals[aRun].size(); ++i)
   {
      if (UtMath::NearlyEqual(sDrVals[aRun][i], aDr))
      {
         aDrIndex = static_cast<int>(i);
         break;
      }
   }

   assert(aAltIndex != -1);
   assert(aCrIndex != -1);
   assert(aDrIndex != -1);
}

// =================================================================================================
// static
//! Public method for insertion of Pk Tables into the data container for storage and processing.
//! Pk values are inserted based on data derived from the run configuration, and the current
//! run and task number of the task handling the Pk generation (typically, via a thread).
//! If periodic Pk table flushing is selected, this method will also check for the completion of
//! the necessary data for a individual Pk table file, and write the file to disc. If periodic
//! flushing is not selected, all Pk data will be flushed when all runs are completed.
void TaskOutput::InsertPk(double aPk, const RunConfig& aRunConfig, int aTaskNum, int aTargetTaskNum)
{
   // We need specific values pulled from the runconfig
   // Specifically, we need initial target altitude, site
   // crossrange and downrange, the repetition, and run number
   int runNum = aRunConfig.GetRunIndex();
   int repNum = aRunConfig.GetRepetitionCount();

   double crossrange, downrange, altitude, dummy;

   // It this is a run beyond the first, the value of the task number needs
   // to be modified to ensure proper indexing. Do so if required
   int workingTaskNumber = aTaskNum;
   if (runNum > 0)
   {
      int previousTasks = 0;
      for (int i = 0; i < runNum; ++i)
      {
         previousTasks += sPkNumRuns[i];
      }
      workingTaskNumber = aTaskNum - previousTasks;
   }

   // If the repetition count is greater than one, ENGAGE is currently
   // configured to run through all repetitions before moving to a different
   // site location. In such a case, modify the task number to index the
   // correct site location
   if (repNum > 1)
   {
      int repTask = workingTaskNumber / repNum;
      aRunConfig.GetSiteConfig().GetRunPosition(repTask, downrange, crossrange);
   }
   else
   {
      aRunConfig.GetSiteConfig().GetRunPosition(workingTaskNumber, downrange, crossrange);
   }
   aRunConfig.GetTargetConfig().GetRunPosition(aTargetTaskNum, dummy, dummy, altitude);

   // Find where we should be inserting this Pk Value into the data container
   int altIndex, crIndex, drIndex;
   GetPkIndex(altIndex, crIndex, drIndex, altitude, crossrange, downrange, runNum);

   // Determine the number of values in the current index location. If there exists value(s)
   // already, we average. Otherwise, just insert
   sRunPkVectorMutex.lock();
   PkEntry curPkVal = sPkVector[runNum][altIndex][crIndex][drIndex];
   sRunPkVectorMutex.unlock();
   if (curPkVal.mNumVals == 0)
   {
      curPkVal.mNumVals = 1;
      curPkVal.mPkVal   = aPk;
   }
   else
   {
      curPkVal.mPkVal = (aPk + curPkVal.mPkVal * curPkVal.mNumVals) / (curPkVal.mNumVals + 1);
      curPkVal.mNumVals++;
   }

   // Lock access and write Pk value
   sRunPkVectorMutex.lock();
   sPkVector[runNum][altIndex][crIndex][drIndex] = curPkVal;
   sRunPkVectorMutex.unlock();

   // Lock access and increment count
   sFilledPksMutex.lock();
   sFilledPks[runNum][altIndex]++;
   sFilledPksMutex.unlock();

   // If we need to check to flush, do it here
   int numPkVals = aRunConfig.GetSiteConfig().GetGridCrCount() * aRunConfig.GetSiteConfig().GetGridDrCount() * repNum;

   sFilledPksMutex.lock();
   int aTempPkValNum = sFilledPks[runNum][altIndex];
   sFilledPksMutex.unlock();

   if (aRunConfig.IsPkPeriodicFlush() && aTempPkValNum == numPkVals)
   {
      FlushPk(aRunConfig, runNum, altIndex);
   }
}

// =================================================================================================
// static
//! Writes the Pk table hierarchy to disc using the passed output path.
//! If values are provided for the optional run index and altitude index, this
//! method will only write out the single associated Pk table. Otherwise, the
//! entire table is output to disc
//! NOTE: This method is naive, in that it doesn't check if a table is ready
//! for writing out. Carelessness may lead to undefined behavior.
void TaskOutput::FlushPk(const RunConfig& aRunConfig,
                         int              aRunIndex /* = cENTIRE_TABLE*/,
                         int              aAltitudeIndex /* = cENTIRE_TABLE*/)
{
   // This is a partial flush
   if (aRunIndex > cENTIRE_TABLE && aAltitudeIndex > cENTIRE_TABLE)
   {
      UtPath outputPath(aRunConfig.GetPkPath());
      outputPath.MakeFullPath();
      if (!outputPath.Exists())
      {
         std::cout << "***** ERROR: Unable to flush Pk table: invalid path" << std::endl;
      }
      outputPath = outputPath.GetNormalizedPath() + "/" + sPkSiteType[aRunIndex] + "/" + sPkTargetType[aRunIndex];

      // Get the full directory structure to the file/CR-DR-PK table
      std::stringstream ss;
      UtUnitType*       unitPtr  = UtUnits::FindUnitType("LENGTH");
      int               unit     = unitPtr->ReadUnit(aRunConfig.GetPkLengthUnit());
      double            newValue = unitPtr->ConvertFromStandard(sAltitudeVals[aRunIndex][aAltitudeIndex], unit);
      ss << newValue;

      std::string lengthUnitModified = aRunConfig.GetPkLengthUnit();
      lengthUnitModified.erase(std::remove(lengthUnitModified.begin(), lengthUnitModified.end(), '/'),
                               lengthUnitModified.end());
      ss << lengthUnitModified;

      outputPath = outputPath.GetNormalizedPath() + "/" + ss.str();
      ;

      // Creates all directories on the way to the file location, if they don't exist
      outputPath.Mkdir();

      ss.str("");
      ss.clear();

      UtUnitType* speedPtr = UtUnits::FindUnitType("SPEED");
      unit                 = speedPtr->ReadUnit(aRunConfig.GetPkVelocityUnit());
      newValue             = speedPtr->ConvertFromStandard(sPkTargetVelocity[aRunIndex], unit);
      ss << newValue;

      std::string speedUnitModified = aRunConfig.GetPkVelocityUnit();
      speedUnitModified.erase(std::remove(speedUnitModified.begin(), speedUnitModified.end(), '/'),
                              speedUnitModified.end());
      ss << speedUnitModified;

      std::string crdrFilename = outputPath.GetNormalizedPath() + "/" + ss.str();
      ss.str("");
      ss.clear();

      // A header is attached to every file. The IDE gui plugin is reliant on this data, so it may change as development progresses
      ss << CreatePkHeader(aRunConfig.GetPkLengthUnit(), aRunConfig.GetPkVelocityUnit(), aRunIndex, aAltitudeIndex);
      ss << CreatePkStream(aRunConfig.GetPkLengthUnit(), aRunIndex, aAltitudeIndex);

      std::ofstream outputFile(crdrFilename.c_str());
      if (!outputFile.good())
      {
         std::cout << "***** ERROR: Unable to flush Pk table: unable to open table file location" << std::endl;
      }

      outputFile << ss.str();
      outputFile.flush();
      outputFile.close();
   }
   else if (aRunIndex == cENTIRE_TABLE && aAltitudeIndex == cENTIRE_TABLE)
   {
      // Output all table data at the end of the simulation
      // Simply iterate here through all values, and recurse back through this method
      // to create individual tables

      // Lock this, in case it isn't being called serially, due to PkVector access while
      // possible write ops
      std::lock_guard<std::recursive_mutex> lock(sRunPkVectorMutex);
      for (unsigned int i = 0; i < sPkVector.size(); i++)
      {
         for (unsigned int j = 0; j < sPkVector[i].size(); j++)
         {
            FlushPk(aRunConfig, i, j);
         }
      }
   }
   else
   {
      std::cout << "***** ERROR: Unable to flush Pk table: invalid flush arguments" << std::endl;
   }
}

// =================================================================================================
// static
//! Given a location indexed into the Pk grid data container at the run and altitude levels,
//! create a stream holding the Pk table data.
std::string TaskOutput::CreatePkStream(const std::string& aLengthUnit, int aRunIndex, int aAltitudeIndex)
{
   // The table format is two-dimensional, ESAMS-like style
   // The table is indexed by the first row, and the first column. For example:
   //       CR1   CR2   CR3   CR4
   // DR1   PK    PK    PK    PK
   // DR2   PK    PK    PK    PK
   // DR3   PK    PK    PK    PK
   // DR4   PK    PK    PK    PK

   std::stringstream ss;

   UtUnitType* unitPtr = UtUnits::FindUnitType("LENGTH");
   int         unit    = unitPtr->ReadUnit(aLengthUnit);

   // Get all of the CR values for the first row. Add some space initially
   // for increased human readability of the data, aligning the columns
   ss << FormatElement("");
   for (unsigned int i = 0; i < sCrVals[aRunIndex].size(); ++i)
   {
      double tempVal = unitPtr->ConvertFromStandard(sCrVals[aRunIndex][i], unit);
      ss << FormatElement(tempVal);
   }
   ss << std::endl;

   // iterate through each possible downrange value
   for (unsigned int i = 0; i < sDrVals[aRunIndex].size(); ++i)
   {
      // for each crossrange value possible in this downrange row, output the value
      for (unsigned int j = 0; j < sCrVals[aRunIndex].size(); ++j)
      {
         // if this is the first iteration, first output the downrange value
         if (j == 0)
         {
            double tempVal = unitPtr->ConvertFromStandard(sDrVals[aRunIndex][i], unit);
            ss << FormatElement(tempVal);
         }
         sRunPkVectorMutex.lock();
         ss << FormatElement(sPkVector[aRunIndex][aAltitudeIndex][j][i].mPkVal);
         sRunPkVectorMutex.unlock();
      }
      ss << std::endl;
   }
   return ss.str();
}

// =================================================================================================
// static
//! Creates the header for the Pk Table output, and returns it as a stream.
std::string TaskOutput::CreatePkHeader(const std::string& aLengthUnit,
                                       const std::string& aSpeedUnit,
                                       int                aRunIndex,
                                       int                aAltitudeIndex)
{
   std::stringstream ss;

   UtUnitType* lengthPtr  = UtUnits::FindUnitType("LENGTH");
   int         lengthUnit = lengthPtr->ReadUnit(aLengthUnit);
   UtUnitType* speedPtr   = UtUnits::FindUnitType("SPEED");
   int         speedUnit  = speedPtr->ReadUnit(aSpeedUnit);

   ss << "Site Platform Type: " << sPkSiteType[aRunIndex] << std::endl;
   ss << "Target Platform Type: " << sPkTargetType[aRunIndex] << std::endl;
   ss << "Length Units: " << aLengthUnit << std::endl;
   ss << "Speed Units: " << aSpeedUnit << std::endl;
   ss << "Altitude: " << lengthPtr->ConvertFromStandard(sAltitudeVals[aRunIndex][aAltitudeIndex], lengthUnit) << std::endl;
   ss << "Speed: " << speedPtr->ConvertFromStandard(sPkTargetVelocity[aRunIndex], speedUnit) << std::endl;

   return ss.str();
}

} // namespace engage
