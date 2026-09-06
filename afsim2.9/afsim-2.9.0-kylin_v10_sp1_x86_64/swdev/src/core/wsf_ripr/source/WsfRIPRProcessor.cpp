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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "WsfRIPRProcessor.hpp"

#include <limits>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfComm.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommandChain.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMessage.hpp"
#include "WsfPlatform.hpp"
#include "WsfProcessor.hpp"
#include "WsfRIPRJob.hpp"
#include "WsfRIPRJobBoard.hpp"
#include "WsfRIPRManager.hpp"
#include "WsfRIPRObserver.hpp"
#include "WsfSimulation.hpp"
#include "WsfThreatProcessor.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackMessage.hpp"
#include "WsfUplinkProcessor.hpp"
#include "WsfWeapon.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponObserver.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptProcessor.hpp"

WsfRIPRProcessor::WsfRIPRProcessor(WsfScenario& aScenario, const char* aPretendName, bool aJobPassThrough)
   : WsfScriptProcessor(aScenario, aPretendName, "PROCESSOR")
   , mTargetPlatformIndex(0)
   , mManagerPtr(nullptr)
   , mJobChannelIds(0)
   , mJobPassThrough(aJobPassThrough)
   , mDesiredJobChannels(1)
   , mBiddingHijacked(false)
   , mJobBoardCommPtr(nullptr)
   , mJobBoardCommInput(nullptr)
{
   mJobBoardPtr = new WsfRIPRJobBoard(this, false);

   mContextPtr->DeclareVariable("WsfRIPRJob", "JOB");
}

WsfRIPRProcessor::WsfRIPRProcessor(WsfScenario& aScenario, bool aJobPassThrough)
   : WsfScriptProcessor(aScenario, "WsfRIPRProcessor", "PROCESSOR")
   , mTargetPlatformIndex(0)
   , mManagerPtr(nullptr)
   , mJobChannelIds(0)
   , mJobPassThrough(aJobPassThrough)
   , mDesiredJobChannels(1)
   , mBiddingHijacked(false)
   , mJobBoardCommPtr(nullptr)
   , mJobBoardCommInput(nullptr)
{
   mJobBoardPtr = new WsfRIPRJobBoard(this, false);

   mContextPtr->DeclareVariable("WsfRIPRJob", "JOB");
}


WsfRIPRProcessor::WsfRIPRProcessor(WsfScenario&       aScenario,
                                   const std::string& aPartClass,
                                   const std::string& aPartVarName,
                                   bool               aJobPassThrough)
   : WsfScriptProcessor(aScenario, aPartClass, aPartVarName)
   , mTargetPlatformIndex(0)
   , mManagerPtr(nullptr)
   , mJobChannelIds(0)
   , mJobPassThrough(aJobPassThrough)
   , mDesiredJobChannels(1)
   , mBiddingHijacked(false)
   , mJobBoardCommPtr(nullptr)
   , mJobBoardCommInput(nullptr)
{
   mJobBoardPtr = new WsfRIPRJobBoard(this);

   mContextPtr->DeclareVariable("WsfRIPRJob", "JOB");
}


// virtual
WsfRIPRProcessor::~WsfRIPRProcessor()
{
   for (unsigned int i = 0; i < this->NumJobChannels(); ++i)
   {
      mManagerPtr->RemoveRiprProc(this->GetId(i)); // remove this proc from the manager's records
   }

   mBufferedJobs.clear();

   delete mJobBoardPtr;
}


//! Copy constructor (for Clone())
// protected
WsfRIPRProcessor::WsfRIPRProcessor(const WsfRIPRProcessor& aSrc)
   : WsfScriptProcessor(aSrc)
   , mTargetPlatformIndex(aSrc.mTargetPlatformIndex)
   , mManagerPtr(nullptr)
   , mJobChannelIds(0)
   , mJobTypeChannels(aSrc.mJobTypeChannels)
   , mJobPassThrough(aSrc.mJobPassThrough)
   , mBiddingBlocks(aSrc.mBiddingBlocks)
   , mDesiredJobChannels(aSrc.mDesiredJobChannels)
   , mBiddingHijacked(false)
   , // don't clone this state.
   mJobBoardCommPtr(nullptr)
   , mJobBoardCommInput(aSrc.mJobBoardCommInput)
   , mBufferedJobs(aSrc.mBufferedJobs)
{
   mJobBoardPtr = new WsfRIPRJobBoard(*(aSrc.mJobBoardPtr));
   mJobBoardPtr->SetOwner(this);
   mContextPtr->DeclareVariable("WsfRIPRJob", "JOB");

   assert(mQueryBidTypeMap.size() == 0);
}


// virtual
WsfProcessor* WsfRIPRProcessor::Clone() const
{
   return new WsfRIPRProcessor(*this);
}

// virtual
bool WsfRIPRProcessor::Initialize2(double aSimTime)
{
   bool ok = WsfScriptProcessor::Initialize2(aSimTime);
   return ok;
}


// virtual
bool WsfRIPRProcessor::Initialize(double aSimTime)
{
   mManagerPtr = static_cast<WsfRIPRManager*>(GetSimulation()->FindExtension("wsf_ripr"));
   bool ok     = true;

   WsfPlatform* platformPtr = GetPlatform();
   if (platformPtr != nullptr)
   {
      // Ensure the weapon uplink specifications are correct.
      if (WsfUplinkProcessor::HasUplinkProcessor(*platformPtr) &&
          !WsfUplinkProcessor::GetUplinkProcessor(*platformPtr)->VerifyUplinks())
      {
         return false;
      }

      // Save off the processor id in aux data
      WsfStringId ThisProcessorID = this->GetNameId();
      platformPtr->GetAuxData().Assign("RIPRProcessor", ThisProcessorID.GetString());
      ok &= (platformPtr->GetAuxDataConst().GetString("RIPRProcessor") == ThisProcessorID);

      // find the WsfComm object we're supposed to be using, save a reference, and set up an internal link
      // from the comm to myself
      wsf::comm::Comm* comm = platformPtr->GetComponent<wsf::comm::Comm>(this->mJobBoardCommInput);
      SetJobBoardComm(comm);
      if (comm != nullptr)
      {
         comm->GetInternalLinks().AddRecipient(this);
      }
   }

   // Subscribe to simulation events of interest.
   mCallbacks.Add(WsfObserver::WeaponFired(GetSimulation()).Connect(&WsfRIPRProcessor::WeaponFired, this));
   mCallbacks.Add(WsfObserver::WeaponTerminated(GetSimulation()).Connect(&WsfRIPRProcessor::WeaponTerminated, this));
   mCallbacks.Add(WsfObserver::WeaponHit(GetSimulation()).Connect(&WsfRIPRProcessor::WeaponHit, this));
   mCallbacks.Add(WsfObserver::WeaponMissed(GetSimulation()).Connect(&WsfRIPRProcessor::WeaponMissed, this));

   // Add ourself to WsfRIPRManager list of RIPR procs
   mJobChannelIds.push_back(mManagerPtr->GetUniqueRiprId(this)); // always give the new object it's own id

   // check if "mDesiredJobChannels" should be increased
   JobTypeIter it = mJobTypeChannels.begin();
   for (; it != mJobTypeChannels.end(); ++it)
   {
      if (it->second >= mDesiredJobChannels)
      {
         mDesiredJobChannels = it->second + 1;
      }
   }
   SetNumJobChannels(mDesiredJobChannels);

   ok &= WsfScriptProcessor::Initialize(aSimTime);

   // we can register the script bidding blocks now, the script context has been initialized
   // give all the bidding block script pointers to the processor's map
   std::vector<std::pair<std::string, UtScript*>>::iterator iter = mBiddingBlocks.begin();
   for (; iter != mBiddingBlocks.end(); ++iter)
   {
      std::string jobType                = iter->first;
      UtScript*   bidBlockPtr            = iter->second;
      bool        biddingBlockRegistered = RegisterScriptBiddingBlock(jobType, mContextPtr, bidBlockPtr);
      if (!biddingBlockRegistered)
      {
         auto out = ut::log::error() << "Processor initialization failed. Duplicate bidding block.";
         out.AddNote() << "Processor: " << GetName();
         out.AddNote() << "Duplicate: " << jobType;
      }
      ok &= biddingBlockRegistered;
   }

   return ok;
}


//! Update the state of the processor.
//! This method should perform any actions that are necessary so that the system
//! reflects its condition at the supplied time.
//! @param aSimTime The current simulation time.
// virtual
void WsfRIPRProcessor::Update(double aSimTime)
{
   WsfScriptProcessor::Update(aSimTime);
   // don't call here, we'll update the job board and jobs when they're read from
   // mJobBoard->Update(aSimTime);
}

void WsfRIPRProcessor::BiddingHijacked(bool aValue)
{
   mBiddingHijacked = aValue;
   if (mManagerPtr == nullptr)
   {
      return;
   }
   mManagerPtr->GetObserver().BidControlChanged(GetSimulation()->GetSimTime(),
                                                GetPlatform()->GetIndex(),
                                                GetNameId(),
                                                BiddingHijacked());
}

// virtual
WsfRIPRJobBoard* WsfRIPRProcessor::JobBoard()
{
   // if this agent is a pass through for jobs, return it's commander's job board, if possible
   if (mJobPassThrough)
   {
      WsfRIPRProcessor* cmdrPtr = CommanderProcessor();
      if (cmdrPtr != nullptr)
      {
         return cmdrPtr->JobBoard();
      }
   }
   // otherwise just return it's own [empty] job board
   return mJobBoardPtr;
}


void WsfRIPRProcessor::SetJobPassThrough(bool aJobPassThrough)
{
   mJobPassThrough = aJobPassThrough;
   // if this agent is now a pass through for jobs, clear its job board
   if (mJobPassThrough)
   {
      mJobBoardPtr->RemoveAllJobs();
   }
}

bool WsfRIPRProcessor::JobWindowOpen(double aSimTime)
{
   return mJobBoardPtr->IsJobWindowOpen(aSimTime);
}

bool WsfRIPRProcessor::BidWindowOpen(double aSimTime)
{
   return mJobBoardPtr->IsBidWindowOpen(aSimTime);
}

void WsfRIPRProcessor::SetJobWindowOpen(bool aOpen)
{
   mJobBoardPtr->SetJobWindowOpen(aOpen);
}

void WsfRIPRProcessor::SetBidWindowOpen(bool aOpen)
{
   mJobBoardPtr->SetBidWindowOpen(aOpen);
}

void WsfRIPRProcessor::SetJobStickiness(double aStickiness)
{
   mJobBoardPtr->SetJobStickiness(aStickiness);
}

double WsfRIPRProcessor::QueryBid(WsfRIPRJob& aJob)
{
   double bid = -std::numeric_limits<double>::max();

   double           tempDouble = 0.0;
   UtScriptData     scriptRetVal(tempDouble);
   UtScriptDataList scriptArgs;

   // now check for a specifically named query_bid_type script block for a bid
   std::string               jobType = aJob.GetName();
   QueryBidMapType::iterator it      = mQueryBidTypeMap.find(jobType);
   if (it != mQueryBidTypeMap.end())
   {
      WsfScriptContext* biddingContextPtr = it->second.first;
      UtScript*         biddingScriptPtr  = it->second.second;
      mContextPtr->GetContext().Var("JOB").GetPointer()->SetAppObject(&aJob, aJob.GetReferenceCount());
      biddingContextPtr->ExecuteScript(biddingScriptPtr, scriptRetVal, scriptArgs);
      bid = scriptRetVal.GetDouble();
   }

   return bid;
}


double WsfRIPRProcessor::WallClock()
{
   return mRiprWallClock.GetClock();
}


bool WsfRIPRProcessor::RegisterScriptBiddingBlock(std::string aJobType, WsfScriptContext* aContextPtr, UtScript* aScriptPtr)
{
   if (mQueryBidTypeMap.find(aJobType) != mQueryBidTypeMap.end())
   {
      // bidding block already exists for this job type
      return false;
   }
   else
   {
      mQueryBidTypeMap[aJobType] = std::make_pair(aContextPtr, aScriptPtr);
      return true;
   }
}


bool WsfRIPRProcessor::UnregisterScriptBiddingBlock(std::string aJobType)
{
   QueryBidMapType::iterator it;
   it = mQueryBidTypeMap.find(aJobType);
   if (it == mQueryBidTypeMap.end())
   {
      // bidding block not found, not registered, so can not remove it
      return false;
   }
   mQueryBidTypeMap.erase(it);
   return true;
}


// virtual
bool WsfRIPRProcessor::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "job_pass_through")
   {
      this->SetJobPassThrough(true);
   }
   else if (command == "num_job_channels")
   {
      // int numChannels;
      // aInput.ReadValue(numChannels);
      // SetNumJobChannels(numChannels);
      aInput.ReadValue(mDesiredJobChannels);
   }
   else if (command == "channel_job_type")
   {
      unsigned int channelIndex;
      std::string  jobType;
      aInput.ReadValue(channelIndex);
      aInput.ReadValue(jobType);
      mJobTypeChannels.insert(std::pair<std::string, unsigned int>(jobType, channelIndex));
   }
   else if (command == "query_bid_type")
   {
      std::string jobType;
      aInput.ReadValue(jobType);
      UtScript* scriptPtr = mContextPtr->Compile("query_bid_type", "double", aInput, "end_query_bid_type", jobType);
      mBiddingBlocks.emplace_back(jobType, scriptPtr);
   }
   else if (command == "auto_exclusive_mode")
   {
      // don't use JobBoard() because that can return commander's JB if we're a passthrough
      bool temp;
      aInput.ReadValue(temp);
      mJobBoardPtr->SetAutoExclusiveMode(temp);
   }
   else if (command == "job_window_open_length")
   {
      // don't use JobBoard() because that can return commander's JB if we're a passthrough
      double temp;
      aInput.ReadValueOfType(temp, UtInput::cTIME);
      mJobBoardPtr->SetJobWindowOpenLength(temp);
   }
   else if (command == "bid_window_open_length")
   {
      // don't use JobBoard() because that can return commander's JB if we're a passthrough
      double temp;
      aInput.ReadValueOfType(temp, UtInput::cTIME);
      mJobBoardPtr->SetBidWindowOpenLength(temp);
   }
   else if (command == "job_stickiness")
   {
      double temp;
      aInput.ReadValue(temp);
      mJobBoardPtr->SetJobStickiness(temp);
   }
   else if (command == "job_board_comm")
   {
      std::string commName;
      aInput.ReadValue(commName);
      WsfStringId commNameId = commName;

      mJobBoardCommInput = commNameId;
   }
   else if (command == "weapon_uplink_path")
   {
      std::string sensorName;
      std::string commName;
      aInput.ReadValue(sensorName);
      aInput.ReadValue(commName);

      WsfPlatform* platformPtr = GetPlatform();
      if (platformPtr != nullptr && WsfUplinkProcessor::GetUplinkProcessor(*platformPtr))
      {
         WsfUplinkProcessor::GetUplinkProcessor(*platformPtr)->AddUplinkPath(sensorName, commName);
      }
   }
   else if (command == "max_weapon_uplinks")
   {
      int temp;
      aInput.ReadValue(temp);
      WsfPlatform* platformPtr = GetPlatform();
      if (platformPtr != nullptr)
      {
         WsfUplinkProcessor::GetUplinkProcessor(*platformPtr)->SetMaxUplinkCount((unsigned int)temp);
      }
   }
   else if (WsfScriptProcessor::ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}


void WsfRIPRProcessor::SetNumJobChannels(unsigned int aNumChannels)
{
   assert(mManagerPtr != nullptr);
   if (aNumChannels < 1)
   {
      aNumChannels = 1;
   }
   while (mJobChannelIds.size() > aNumChannels)
   {
      mJobChannelIds.pop_back();
   }
   while (mJobChannelIds.size() < aNumChannels)
   {
      int newChannel = mManagerPtr->GetUniqueRiprId(this);
      mJobChannelIds.push_back(newChannel);
      SetBufferedJobForChannel(newChannel, -1);
   }

   // notify XIO subscribers that the channel ID list has changed.
   WsfPlatform* myPlatformPtr = GetPlatform();
   assert(myPlatformPtr != nullptr);
   mManagerPtr->GetObserver().ChannelIdListChanged(GetSimulation()->GetSimTime(),
                                                   myPlatformPtr->GetIndex(),
                                                   GetNameId(),
                                                   mJobChannelIds);
}


int WsfRIPRProcessor::JobChannelByProcId(unsigned int aRIPRProcId)
{
   int                              chan = 0;
   ripr::JobChannelIdList::iterator it   = mJobChannelIds.begin();
   for (; it != mJobChannelIds.end(); ++it)
   {
      if (*it == aRIPRProcId)
      {
         break;
      }
      ++chan;
   }

   if (it != mJobChannelIds.end())
   {
      return chan;
   }
   return -1;
}


std::vector<unsigned int> WsfRIPRProcessor::JobTypeChannels(std::string aJobType)
{
   std::vector<unsigned int>           ret;
   std::pair<JobTypeIter, JobTypeIter> range;
   range = mJobTypeChannels.equal_range(aJobType);
   for (JobTypeIter it = range.first; it != range.second; ++it)
   {
      ret.push_back(it->second);
   }
   return ret;
}


std::vector<std::string> WsfRIPRProcessor::ChannelJobTypes(unsigned int aChannelIndex)
{
   std::vector<std::string> ret;
   JobTypeIter              it = mJobTypeChannels.begin();
   for (; it != mJobTypeChannels.end(); ++it)
   {
      if (it->second == aChannelIndex)
      {
         ret.push_back(it->first);
      }
   }
   return ret;
}


bool WsfRIPRProcessor::ValidJobTypeChannel(std::string aJobType, unsigned int aChannelIndex)
{
   if (mJobTypeChannels.empty())
   {
      return true; // everything passes if no job type channels were specified
   }
   JobTypeIter beg = mJobTypeChannels.lower_bound(aJobType);
   JobTypeIter end = mJobTypeChannels.upper_bound(aJobType);
   while (beg != end)
   {
      if (beg->second == aChannelIndex)
      {
         return true;
      }
      ++beg;
   }
   return false;
}


WsfPlatform* WsfRIPRProcessor::CommanderPlatform(const std::string& aCommandChainName)
{
   WsfPlatform* hostPlatformPtr = GetPlatform();
   if (hostPlatformPtr == nullptr)
   {
      return nullptr;
   }
   WsfStringId      CommandChainID  = aCommandChainName;
   WsfCommandChain* commandChainPtr = hostPlatformPtr->GetComponent<WsfCommandChain>(CommandChainID);
   if (commandChainPtr != nullptr)
   {
      WsfPlatform* commanderPlatformPtr = commandChainPtr->GetCommander();
      if (commanderPlatformPtr != nullptr)
      {
         // verify the platform is not it's own commander
         if (hostPlatformPtr->GetUniqueId() == commanderPlatformPtr->GetUniqueId())
         {
            return nullptr;
         }
         if (commanderPlatformPtr->GetAuxDataConst().AttributeExists("RIPRProcessor"))
         {
            // verify the platform is the ripr commander
            WsfStringId       RiprProcessorID = commanderPlatformPtr->GetAuxDataConst().GetString("RIPRProcessor");
            WsfRIPRProcessor* riprProcessorPtr =
               (WsfRIPRProcessor*)commanderPlatformPtr->GetComponent<WsfProcessor>(RiprProcessorID);
            if (riprProcessorPtr != nullptr)
            {
               // return the platform pointer
               return commanderPlatformPtr;
            }
         }
      }
   }
   return nullptr;
}

WsfPlatform* WsfRIPRProcessor::CommanderPlatform()
{
   return this->CommanderPlatform(WsfCommandChain::GetDefaultName());
}

WsfRIPRProcessor* WsfRIPRProcessor::CommanderProcessor(const std::string& aCommandChainName)
{
   WsfPlatform* hostPlatformPtr = GetPlatform();
   if (hostPlatformPtr != nullptr)
   {
      WsfStringId      CommandChainID  = aCommandChainName;
      WsfCommandChain* commandChainPtr = hostPlatformPtr->GetComponent<WsfCommandChain>(CommandChainID);
      if (commandChainPtr != nullptr)
      {
         WsfPlatform* commanderPlatformPtr = commandChainPtr->GetCommander();
         if (commanderPlatformPtr != nullptr)
         {
            // verify the platform is not it's own commander
            if (hostPlatformPtr->GetUniqueId() == commanderPlatformPtr->GetUniqueId())
            {
               return nullptr;
            }
            if (commanderPlatformPtr->GetAuxDataConst().AttributeExists("RIPRProcessor"))
            {
               WsfStringId       RiprProcessorID = commanderPlatformPtr->GetAuxDataConst().GetString("RIPRProcessor");
               WsfRIPRProcessor* riprProcessorPtr =
                  (WsfRIPRProcessor*)commanderPlatformPtr->GetComponent<WsfProcessor>(RiprProcessorID);
               if (riprProcessorPtr != nullptr)
               {
                  return riprProcessorPtr;
               }
            }
         }
      }
   }
   return nullptr;
}

ripr::RIPRCommanderList WsfRIPRProcessor::Commanders()
{
   ripr::RIPRCommanderList infoList;

   for (WsfComponentList::RoleIterator<WsfCommandChain> iter(*GetPlatform()); !iter.AtEnd(); ++iter)
   {
      const WsfStringId chainName  = iter->GetNameId();
      WsfRIPRProcessor* cmdProcPtr = CommanderProcessor(chainName.GetString());
      if (cmdProcPtr != nullptr)
      {
         ripr::RIPRCommanderInfo info;
         info.mCommandChainName  = chainName;
         info.mRIPRProcessorName = cmdProcPtr->GetNameId();
         info.mPlatformName      = iter->GetCommander()->GetNameId();
         info.mPlatformIndex     = iter->GetCommander()->GetIndex();
         infoList.push_back(info);
      }
   }
   return infoList;
}


// returns the first RIPR processor found on any of the platforms commanders
WsfRIPRProcessor* WsfRIPRProcessor::CommanderProcessor()
{
   std::string name = WsfCommandChain::GetDefaultName();
   return this->CommanderProcessor(name);
}


std::vector<WsfPlatform*> WsfRIPRProcessor::SubordinatePlatforms(const std::string& aCommandChainName)
{
   std::vector<WsfPlatform*> SubordinatePlatforms;
   WsfPlatform*              hostPlatformPtr = GetPlatform();
   if (hostPlatformPtr == nullptr)
   {
      return SubordinatePlatforms;
   }
   WsfStringId      CommandChainID  = aCommandChainName;
   WsfCommandChain* commandChainPtr = hostPlatformPtr->GetComponent<WsfCommandChain>(CommandChainID);
   if (commandChainPtr == nullptr)
   {
      return SubordinatePlatforms;
   }
   std::vector<WsfPlatform*> tempSubs = commandChainPtr->GetSubordinates();

   std::vector<WsfPlatform*>::iterator it;
   for (it = tempSubs.begin(); it != tempSubs.end(); ++it)
   {
      WsfPlatform* platPtr = *it;
      if (platPtr != nullptr)
      {
         // verify the platform is not it's own subordinate
         if (hostPlatformPtr->GetUniqueId() == platPtr->GetUniqueId())
         {
            continue;
         }
         if (platPtr->GetAuxDataConst().AttributeExists("RIPRProcessor"))
         {
            // verify the subordinate platform is the ripr platform (has a valid ripr processor)
            // this step may be unnecessary, we might be able to add the subordinate to the list right here
            WsfStringId       RiprProcessorID  = platPtr->GetAuxDataConst().GetString("RIPRProcessor");
            WsfRIPRProcessor* riprProcessorPtr = (WsfRIPRProcessor*)platPtr->GetComponent<WsfProcessor>(RiprProcessorID);
            if (riprProcessorPtr != nullptr)
            {
               // add the subordinate platform pointer to the subordinate list to output
               SubordinatePlatforms.push_back(platPtr);
            }
         }
      }
   }
   return SubordinatePlatforms;
}


std::vector<WsfPlatform*> WsfRIPRProcessor::SubordinatePlatforms()
{
   return this->SubordinatePlatforms(WsfCommandChain::GetDefaultName());
}


std::vector<WsfRIPRProcessor*> WsfRIPRProcessor::SubordinateProcessors(const std::string& aCommandChainName)
{
   std::vector<WsfRIPRProcessor*> SubordinateProcessors;
   WsfPlatform*                   hostPlatformPtr = GetPlatform();
   if (hostPlatformPtr == nullptr)
   {
      return SubordinateProcessors;
   }
   WsfStringId      CommandChainID  = aCommandChainName;
   WsfCommandChain* commandChainPtr = hostPlatformPtr->GetComponent<WsfCommandChain>(CommandChainID);
   if (commandChainPtr == nullptr)
   {
      return SubordinateProcessors;
   }
   std::vector<WsfPlatform*> tempSubs = commandChainPtr->GetSubordinates();

   std::vector<WsfPlatform*>::iterator it;
   for (it = tempSubs.begin(); it != tempSubs.end(); ++it)
   {
      WsfPlatform* platPtr = *it;
      if (platPtr != nullptr)
      {
         // verify the platform is not it's own subordinate
         if (hostPlatformPtr->GetUniqueId() == platPtr->GetUniqueId())
         {
            continue;
         }
         if (platPtr->GetAuxDataConst().AttributeExists("RIPRProcessor"))
         {
            // verify the subordinate platform is the ripr platform (has a valid ripr processor)
            // this step may be unnecessary, we might be able to add the subordinate to the list right here
            WsfStringId       RiprProcessorID  = platPtr->GetAuxDataConst().GetString("RIPRProcessor");
            WsfRIPRProcessor* riprProcessorPtr = (WsfRIPRProcessor*)platPtr->GetComponent<WsfProcessor>(RiprProcessorID);
            if (riprProcessorPtr != nullptr)
            {
               // add the subordinate platform pointer to the subordinate list to output
               SubordinateProcessors.push_back(riprProcessorPtr);
            }
         }
      }
   }
   return SubordinateProcessors;
}


std::vector<WsfRIPRProcessor*> WsfRIPRProcessor::SubordinateProcessors()
{
   return this->SubordinateProcessors(WsfCommandChain::GetDefaultName());
}


double WsfRIPRProcessor::QuerySubordinatesMaxBid(WsfRIPRJob& aJob, bool expandPassThroughAgents)
{
   double                                   curBid = -std::numeric_limits<double>::max();
   double                                   maxBid = -std::numeric_limits<double>::max();
   std::vector<WsfRIPRProcessor*>           subs   = this->SubordinateProcessors();
   std::vector<WsfRIPRProcessor*>::iterator it;
   for (it = subs.begin(); it != subs.end(); ++it)
   {
      WsfRIPRProcessor* procPtr = *it;
      // if this ripr processor is a pass through, he won't ever bid himself
      if (procPtr->JobPassThrough())
      {
         if (expandPassThroughAgents)
         {
            // add this pass through agent's subordinates to the list of subordinates
            curBid = procPtr->QuerySubordinatesMaxBid(aJob, expandPassThroughAgents);
            maxBid = std::max(maxBid, curBid);
         }
      }
      else
      {
         // get this ripr processor's bid on this job
         curBid = procPtr->QueryBid(aJob);
         maxBid = std::max(maxBid, curBid);
      }
   }
   return maxBid;
}


//! A simulation observer to handle weapon firing events.
//! @param aSimTime        The current simulation time.
//! @param aEngagementPtr  A pointer to the weapon engagement data.
//! @param aTargetTrackPtr A pointer to the target track (may be 0).
void WsfRIPRProcessor::WeaponFired(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTargetTrackPtr)
{
   const WsfTrackId& targetTrackId = aEngagementPtr->GetTargetTrackId();
   if (targetTrackId.IsNull())
   {
      return;
   }
   const WsfWeapon* weaponPtr = aEngagementPtr->GetWeaponSystem();
   if (weaponPtr == nullptr)
   {
      return;
   }

   size_t      assigneeIndex    = aEngagementPtr->GetFiringPlatformIndex();
   WsfStringId weaponSystemName = aEngagementPtr->GetWeaponSystemName();
   size_t      targetIndex      = aEngagementPtr->GetTargetPlatformIndex();
   // size_t weaponPlatIndex  = aEngagementPtr->GetWeaponPlatformIndex();

   if (assigneeIndex != GetPlatform()->GetIndex())
   {
      bool                      found = false;
      std::vector<WsfPlatform*> subs  = Subordinates();
      for (size_t i = 0; i < subs.size(); ++i)
      {
         found |= (subs[i]->GetIndex() == assigneeIndex);
      }
      // we don't care about this weapon, because we didn't fire it (or our subordinates)
      if (!found)
      {
         return;
      }
      else
      {
         found = true;
      }
   }

   WeaponStatusList::iterator sli = FindWeaponStatus(targetTrackId, assigneeIndex, weaponSystemName);
   if (sli == mWeaponStatusList.end())
   {
      WeaponStatus status(targetTrackId, assigneeIndex, weaponSystemName);
      mWeaponStatusList.push_front(status);
      sli = mWeaponStatusList.begin();
   }
   (*sli).mTotalSalvosFired += 1;
   (*sli).mTotalRoundsFired += weaponPtr->GetActiveRequestCount();
   (*sli).mRoundsPending += weaponPtr->GetActiveRequestCount();
   (*sli).mTimeWeaponLastFired = aSimTime;
   (*sli).mTargetIndex         = targetIndex;

   // If this weapon launch is associated to an assignment from this platform then update the weapon data.
   //
   // NOTE: The combination of target track ID, assignee index and weapon system name should be unique
   //       across assigners as the target track ID is local to the assigner.

   sli = FindWeaponStatus(targetTrackId, assigneeIndex, weaponSystemName);
   if (sli != mWeaponStatusList.end())
   {
      WeaponStatus& status = *sli;
      if (status.mRoundsPending > 0)
      {
         status.mRoundsPending -= weaponPtr->GetActiveRequestCount();
         assert(status.mRoundsPending >= 0);
         status.mRoundsActive += weaponPtr->GetActiveRequestCount();
         status.mTimeWeaponLastFired = aSimTime;
      }
   }

   ActiveWeapon activeWeapon;
   activeWeapon.mTaskTrackId      = targetTrackId;
   activeWeapon.mTargetIndex      = targetIndex;
   activeWeapon.mWeaponSystemName = weaponSystemName;
   activeWeapon.mPlatformIndex    = aEngagementPtr->GetWeaponPlatformIndex();
   activeWeapon.mSerialNumber     = aEngagementPtr->GetSerialNumber();
   mActiveWeaponList.push_back(activeWeapon);

   // update any uplinks, for this weapon

   StartUplinking(activeWeapon.mPlatformIndex, activeWeapon.mTargetIndex);

   // for (UplinkPathList::const_iterator upli = mUplinkPathList.begin(); upli != mUplinkPathList.end(); ++upli)
   //{
   //    const UplinkPath& uplinkPath = *upli;
   //    WsfComm* commPtr = GetPlatform()->GetComponent<WsfComm>(uplinkPath.mCommName);
   //    if (commPtr != 0)
   //    {
   //       WsfPlatform* platformPtr = GetSimulation()->GetPlatformByIndex(activeWeapon.mPlatformIndex);
   //       if (platformPtr != 0)
   //       {
   //          TrackUplink trackUplink;
   //          //trackUplink.mSensorTrackId = activeWeapon.mTaskTrackId;
   //          trackUplink.mSensorName    = uplinkPath.mSensorName;
   //          trackUplink.mTargetId      = activeWeapon.mTargetIndex;
   //          trackUplink.mPlatformPtr   = platformPtr;
   //          trackUplink.mCommPtr       = commPtr;
   //          mTrackUplinkList.push_back(trackUplink);
   //       }
   //    }
   // }
}


//! A simulation observer to handle weapon termination events.
//! @param aSimTime        The current simulation time.
//! @param aEngagementPtr  A pointer to the weapon engagement data.
void WsfRIPRProcessor::WeaponTerminated(double aSimTime, const WsfWeaponEngagement* aEngagementPtr)
{
   const WsfTrackId& targetTrackId = aEngagementPtr->GetTargetTrackId();
   if (targetTrackId.IsNull())
   {
      return;
   }

   size_t      assigneeIndex    = aEngagementPtr->GetFiringPlatformIndex();
   WsfStringId weaponSystemName = aEngagementPtr->GetWeaponSystemName();

   // Update the weapon status in the assigning platform.
   WeaponStatusList::iterator sli = FindWeaponStatus(targetTrackId, assigneeIndex, weaponSystemName);
   if (sli != mWeaponStatusList.end())
   {
      WeaponStatus& status = *sli;
      if (status.mRoundsActive > 0)
      {
         --status.mRoundsActive;
         status.mTimeWeaponLastTerminated = aSimTime;
      }
   }

   // Remove any associated 'active weapon' entry
   for (ActiveWeaponList::iterator awli = mActiveWeaponList.begin(); awli != mActiveWeaponList.end(); ++awli)
   {
      ActiveWeapon& activeWeapon = *awli;
      if (activeWeapon.mSerialNumber == aEngagementPtr->GetSerialNumber())
      {
         mActiveWeaponList.erase(awli);
         break;
      }
   }

   // remove any weapon uplinks for this weapon
   StopUplinking(aEngagementPtr->GetWeaponPlatformIndex());

   ////for (TrackUplinkList::const_iterator tuli = mTrackUplinkList.begin(); tuli != mTrackUplinkList.end(); ++tuli)
   // for (TrackUplinkList::iterator tuli = mTrackUplinkList.begin(); tuli != mTrackUplinkList.end(); ++tuli)
   //{
   //    if (aEngagementPtr->GetWeaponPlatformIndex() == tuli->mPlatformPtr->GetIndex())
   //    {
   //       mTrackUplinkList.erase(tuli);
   //       break;
   //    }
   // }
}


//! A simulation observer to handle weapon hit events.
//! @param aSimTime           The current simulation time.
//! @param aEngagementPtr     A pointer to the weapon engagement data.
//! @param aTargetPlatformPtr A pointer to the targeted platform
void WsfRIPRProcessor::WeaponHit(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, WsfPlatform* aTargetPlatformPtr)
{
   const WsfTrackId& targetTrackId = aEngagementPtr->GetTargetTrackId();
   if (targetTrackId.IsNull())
   {
      return;
   }

   size_t      assigneeIndex    = aEngagementPtr->GetFiringPlatformIndex();
   WsfStringId weaponSystemName = aEngagementPtr->GetWeaponSystemName();

   // Update the weapon status in the assigning platform.

   WeaponStatusList::iterator sli = FindWeaponStatus(targetTrackId, assigneeIndex, weaponSystemName);
   if (sli != mWeaponStatusList.end())
   {
      WeaponStatus& status = *sli;
      status.mRoundsHit++;
      status.mTimeWeaponLastHit = aSimTime;
   }
}


//! A simulation observer to handle weapon miss events.
//! @param aSimTime           The current simulation time.
//! @param aEngagementPtr     A pointer to the weapon engagement data.
//! @param aTargetPlatformPtr A pointer to the targeted platform
void WsfRIPRProcessor::WeaponMissed(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, WsfPlatform* aTargetPlatformPtr)
{
   const WsfTrackId& targetTrackId = aEngagementPtr->GetTargetTrackId();
   if (targetTrackId.IsNull())
   {
      return;
   }

   size_t      assigneeIndex    = aEngagementPtr->GetFiringPlatformIndex();
   WsfStringId weaponSystemName = aEngagementPtr->GetWeaponSystemName();

   // Update the weapon status in the assigning platform.

   WeaponStatusList::iterator sli = FindWeaponStatus(targetTrackId, assigneeIndex, weaponSystemName);
   if (sli != mWeaponStatusList.end())
   {
      WeaponStatus& status = *sli;
      status.mRoundsMissed++;
      status.mTimeWeaponLastMissed = aSimTime;
   }
}


bool WsfRIPRProcessor::UplinkCapable()
{
   WsfPlatform* platformPtr = GetPlatform();
   if (platformPtr == nullptr || WsfUplinkProcessor::HasUplinkProcessor(*platformPtr) == false ||
       WsfUplinkProcessor::GetUplinkProcessor(*platformPtr)->IsUplinkPathListEmpty())
   {
      return false;
   }
   return true;
}


bool WsfRIPRProcessor::StartUplinking(size_t receiverIndex, size_t targetIndex)
{
   WsfPlatform* platformPtr = GetPlatform();
   if (platformPtr != nullptr && WsfUplinkProcessor::HasUplinkProcessor(*platformPtr))
   {
      bool uplinkStarted =
         WsfUplinkProcessor::GetUplinkProcessor(*platformPtr)->StartUplinking(receiverIndex, targetIndex);
      if (uplinkStarted)
      {
         WsfPlatform* weaponPlatformPtr = GetSimulation()->GetPlatformByIndex(receiverIndex);
         if (weaponPlatformPtr != nullptr)
         {
            WsfTrack*    trackPtr    = nullptr;
            WsfPlatform* tempPlatPtr = GetSimulation()->GetPlatformByIndex(targetIndex);
            if (tempPlatPtr != nullptr)
            {
               WsfTrack* tempTrackPtr = new WsfTrack;
               tempTrackPtr->SetTargetIndex(tempPlatPtr->GetIndex());
               tempTrackPtr->SetTargetName(tempPlatPtr->GetNameId()); // needed???
               trackPtr = tempTrackPtr;
               if (trackPtr == nullptr)
               {
                  trackPtr = (WsfTrack*)weaponPlatformPtr->GetTrackManager().GetCurrentTarget();
               }
               bool found = false;
               for (ActiveWeaponList::iterator awli = mActiveWeaponList.begin(); awli != mActiveWeaponList.end(); ++awli)
               {
                  if (awli->mPlatformIndex == weaponPlatformPtr->GetIndex() &&
                      awli->mTargetIndex == trackPtr->GetTargetIndex())
                  {
                     found = true;
                  }
               }
               if (!found)
               {
                  WsfWeaponEngagement* engagementPtr = WsfWeaponEngagement::Find(*weaponPlatformPtr);
                  ActiveWeapon         activeWeapon;
                  activeWeapon.mTaskTrackId = trackPtr->GetTrackId();
                  activeWeapon.mTargetIndex = trackPtr->GetTargetIndex();
                  activeWeapon.mWeaponSystemName =
                     (engagementPtr != nullptr) ? (engagementPtr->GetWeaponSystemName()) : WsfStringId();
                  activeWeapon.mPlatformIndex = weaponPlatformPtr->GetIndex();
                  activeWeapon.mSerialNumber  = (engagementPtr != nullptr) ? (engagementPtr->GetSerialNumber()) : (0);
                  mActiveWeaponList.push_back(activeWeapon);
               }
            }
         }
      }
      return uplinkStarted;
   }

   return false;
}

bool WsfRIPRProcessor::StartUplinking(WsfPlatform* receiverPtr, WsfTrack* targetPtr)
{
   if (receiverPtr == nullptr)
   {
      return false;
   }

   if (targetPtr == nullptr)
   {
      // find the target of the weapon
      targetPtr = (WsfTrack*)receiverPtr->GetTrackManager().GetCurrentTarget();
      if (targetPtr == nullptr)
      {
         return false;
      }
   }

   if (WeaponUplinkCount() >= WsfUplinkProcessor::GetUplinkProcessor(*GetPlatform())->GetMaxSupportedUplinks())
   {
      // don't create any new uplinks, only allow this uplink if the weapon is already being uplinked to
      //(perhaps this new uplink is for an additional track)
      if (!IsUplinkingTo(receiverPtr))
      {
         return false;
      }
   }

   // make sure we have an entry in our "active weapon" list for this weapon as well
   bool found = false;
   for (ActiveWeaponList::iterator awli = mActiveWeaponList.begin(); awli != mActiveWeaponList.end(); ++awli)
   {
      if (awli->mPlatformIndex == receiverPtr->GetIndex() && awli->mTargetIndex == targetPtr->GetTargetIndex())
      {
         found = true;
      }
   }
   if (!found)
   {
      WsfWeaponEngagement* engagementPtr = WsfWeaponEngagement::Find(*receiverPtr);
      ActiveWeapon         activeWeapon;
      activeWeapon.mTaskTrackId      = targetPtr->GetTrackId();
      activeWeapon.mTargetIndex      = targetPtr->GetTargetIndex();
      activeWeapon.mWeaponSystemName = (engagementPtr != nullptr) ? (engagementPtr->GetWeaponSystemName()) : WsfStringId();
      activeWeapon.mPlatformIndex    = receiverPtr->GetIndex();
      activeWeapon.mSerialNumber     = (engagementPtr != nullptr) ? (engagementPtr->GetSerialNumber()) : (0);
      mActiveWeaponList.push_back(activeWeapon);
   }

   WsfPlatform* platformPtr = GetPlatform();
   if (platformPtr != nullptr)
   {
      WsfUplinkProcessor::GetUplinkProcessor(*platformPtr)->StartUplinking(receiverPtr, targetPtr);
   }
   return true;
}

bool WsfRIPRProcessor::StopUplinking(size_t receiverIndex)
{
   // remove any weapon uplinks for this weapon
   WsfPlatform* platformPtr = GetPlatform();
   if (platformPtr != nullptr && WsfUplinkProcessor::HasUplinkProcessor(*platformPtr))
   {
      WsfUplinkProcessor::GetUplinkProcessor(*platformPtr)->StopUplinking(receiverIndex);
   }

   // if this platform didn't fire the weapon, and isn't supporting it anymore, remove it from the active weapons list
   WsfPlatform*         receiverPtr   = GetSimulation()->GetPlatformByIndex(receiverIndex);
   WsfWeaponEngagement* engagementPtr = (receiverPtr != nullptr) ? WsfWeaponEngagement::Find(*receiverPtr) : nullptr;
   size_t firingPlatformIndex         = (engagementPtr != nullptr) ? (engagementPtr->GetFiringPlatformIndex()) : (0);
   bool   drop                        = (GetPlatform()->GetIndex() != firingPlatformIndex);
   if (drop)
   {
      // Remove any associated 'active weapon' entry
      for (ActiveWeaponList::iterator awli = mActiveWeaponList.begin(); awli != mActiveWeaponList.end();)
      {
         if (awli->mPlatformIndex == receiverIndex)
         {
            awli = mActiveWeaponList.erase(awli);
         }
         else
         {
            ++awli;
         }
      }
   }

   return true;
}

bool WsfRIPRProcessor::StopUplinking(WsfPlatform* receiverPtr)
{
   if (receiverPtr == nullptr)
   {
      return false;
   }
   return StopUplinking((int)receiverPtr->GetIndex());
}

bool WsfRIPRProcessor::IsUplinkingTo(int receiverIndex)
{
   WsfPlatform* receiverPtr = GetSimulation()->GetPlatformByIndex(receiverIndex);
   return IsUplinkingTo(receiverPtr);
}

bool WsfRIPRProcessor::IsUplinkingTo(WsfPlatform* receiverPtr)
{
   if (receiverPtr == nullptr)
   {
      return false;
   }
   WsfPlatform* platformPtr = GetPlatform();
   if (platformPtr != nullptr && WsfUplinkProcessor::HasUplinkProcessor(*platformPtr))
   {
      return WsfUplinkProcessor::GetUplinkProcessor(*platformPtr)->IsUplinkingTo(receiverPtr);
   }
   return false;
}

unsigned int WsfRIPRProcessor::WeaponUplinkCount()
{
   WsfPlatform* platformPtr = GetPlatform();
   if (platformPtr != nullptr && WsfUplinkProcessor::HasUplinkProcessor(*platformPtr))
   {
      return WsfUplinkProcessor::GetUplinkProcessor(*platformPtr)->WeaponUplinkCount();
   }
   return 0;
}

//! Return the weapon platform at aIndex in the list of the weapons currently being uplinked to
WsfPlatform* WsfRIPRProcessor::UplinkWeaponPlatform(unsigned int aIndex)
{
   WsfPlatform* platformPtr = GetPlatform();
   if (platformPtr != nullptr && WsfUplinkProcessor::HasUplinkProcessor(*platformPtr))
   {
      return WsfUplinkProcessor::GetUplinkProcessor(*platformPtr)->UplinkWeaponPlatform(aIndex);
   }
   return nullptr;
}

//! Return the count of the number of weapons that are active.
int WsfRIPRProcessor::WeaponsActive()
{
   return static_cast<int>(mActiveWeaponList.size());
   // int weaponCount = 0;
   // for (WeaponStatusList::const_iterator sli = mWeaponStatusList.begin(); sli != mWeaponStatusList.end(); ++sli)
   //{
   //    weaponCount += max((*sli).mRoundsPending, 0) + max((*sli).mRoundsActive, 0);
   // }
   // return weaponCount;
}

//! Return the count of the number of weapons that are active against the specified track.
//! @param aTrack The track of interest.
int WsfRIPRProcessor::WeaponsActive(const WsfTrack& aTrack)
{
   size_t targetIndex = aTrack.GetTargetIndex();
   int    weaponCount = 0;
   // for (WeaponStatusList::const_iterator sli = mWeaponStatusList.begin(); sli != mWeaponStatusList.end(); ++sli)
   //{
   //    if (targetIndex == (*sli).mTargetIndex)
   //    {
   //       weaponCount += max((*sli).mRoundsPending, 0) + max((*sli).mRoundsActive, 0);
   //    }
   // }
   for (ActiveWeaponList::iterator awli = mActiveWeaponList.begin(); awli != mActiveWeaponList.end(); ++awli)
   {
      if (targetIndex == awli->mTargetIndex)
      {
         ++weaponCount;
      }
   }
   return weaponCount;
}

//! Return the count of the number of weapons of type aWeapon that are active.
//! @param aWeapon The weapon type of interest.
int WsfRIPRProcessor::WeaponsActiveOfType(const WsfWeapon& aWeapon)
{
   int                              weaponCount = 0;
   WeaponStatusList::const_iterator sli;
   for (sli = mWeaponStatusList.begin(); sli != mWeaponStatusList.end(); ++sli)
   {
      if ((*sli).mWeaponSystemName == aWeapon.GetNameId())
      {
         weaponCount += std::max((*sli).mRoundsPending, 0) + std::max((*sli).mRoundsActive, 0);
      }
   }
   return weaponCount;
}

//! Return the platform of the active weapon at the given index.
WsfPlatform* WsfRIPRProcessor::ActiveWeaponPlatform(unsigned int aIndex)
{
   if (aIndex < mActiveWeaponList.size())
   {
      unsigned int count = 0;
      for (ActiveWeaponList::iterator awli = mActiveWeaponList.begin(); awli != mActiveWeaponList.end(); ++awli)
      {
         if (count == aIndex)
         {
            return GetSimulation()->GetPlatformByIndex(awli->mPlatformIndex);
         }
         ++count;
      }
   }
   return nullptr;
}

int WsfRIPRProcessor::Targeting(const WsfTrack* aTrackPtr)
{
   if (aTrackPtr != nullptr && aTrackPtr->GetTargetIndex() == this->TargetPlatformId())
   {
      return 1;
   }
   return 0;
}

std::vector<WsfPlatform*> WsfRIPRProcessor::Subordinates()
{
   WsfPlatform* platPtr = GetPlatform();
   if (platPtr != nullptr)
   {
      WsfCommandChain* chainPtr = platPtr->GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId());
      if (chainPtr != nullptr)
      {
         return chainPtr->GetSubordinates();
      }
   }
   return std::vector<WsfPlatform*>();
}

int WsfRIPRProcessor::SubsTargeting(const WsfTrack* aTrackPtr, const WsfPlatform* aExcludePlatformPtr)
{
   int targetingCount = 0;
   if (aTrackPtr == nullptr)
   {
      return -1;
   }
   std::vector<WsfPlatform*> subs = this->Subordinates();
   for (unsigned int i = 0; i < subs.size(); ++i)
   {
      WsfPlatform* curPlatPtr = subs[i];
      if (curPlatPtr == aExcludePlatformPtr)
      {
         continue;
      }
      if (curPlatPtr->GetAuxDataConst().AttributeExists("RIPRProcessor"))
      {
         // this guy is a ripr agent, and might have weapons or subordinates with weapons
         WsfStringId       RiprProcessorID  = curPlatPtr->GetAuxDataConst().GetString("RIPRProcessor");
         WsfRIPRProcessor* riprProcessorPtr = (WsfRIPRProcessor*)curPlatPtr->GetComponent<WsfProcessor>(RiprProcessorID);
         if (riprProcessorPtr != nullptr)
         {
            targetingCount += riprProcessorPtr->Targeting(aTrackPtr);
            targetingCount += riprProcessorPtr->SubsTargeting(aTrackPtr, aExcludePlatformPtr);
         }
      }
   }
   return targetingCount;
}

int WsfRIPRProcessor::SubsWeaponsActive(const WsfTrack* aTrackPtr, const WsfPlatform* aExcludePlatformPtr)
{
   int activeCount = 0;
   if (aTrackPtr == nullptr)
   {
      return -1;
   }
   std::vector<WsfPlatform*> subs = this->Subordinates();
   for (unsigned int i = 0; i < subs.size(); ++i)
   {
      WsfPlatform* curPlatPtr = subs[i];
      if (curPlatPtr == aExcludePlatformPtr)
      {
         continue;
      }
      if (curPlatPtr->GetAuxDataConst().AttributeExists("RIPRProcessor"))
      {
         // this guy is a ripr agent, and might have weapons or subordinates with weapons
         WsfStringId       RiprProcessorID  = curPlatPtr->GetAuxDataConst().GetString("RIPRProcessor");
         WsfRIPRProcessor* riprProcessorPtr = (WsfRIPRProcessor*)curPlatPtr->GetComponent<WsfProcessor>(RiprProcessorID);
         if (riprProcessorPtr != nullptr)
         {
            activeCount += riprProcessorPtr->WeaponsActive(*aTrackPtr);
            activeCount += riprProcessorPtr->SubsWeaponsActive(aTrackPtr, aExcludePlatformPtr);
         }
      }
   }
   return activeCount;
}

//! Return the count of the number of weapons that are active against the specified track.
//! Includes all AIAI agents on same flight and any lead connected to the same GCI commander
//! @param aTrack The track of interest.
int WsfRIPRProcessor::PeersWeaponsActive(const WsfTrack& aTrack)
{
   int               numActive        = 0;
   WsfRIPRProcessor* riprCommanderPtr = nullptr;
   WsfRIPRProcessor* tempProcPtr      = CommanderProcessor();
   // climb as high up the ripr command chain as possible
   while (tempProcPtr != nullptr)
   {
      riprCommanderPtr = tempProcPtr;
      tempProcPtr      = riprCommanderPtr->CommanderProcessor();
   }
   // add the number of all weapons fired against the track by subordinate ripr agents
   if (riprCommanderPtr != nullptr)
   {
      numActive += riprCommanderPtr->SubsWeaponsActive(&aTrack);
   }
   numActive += WeaponsActive(aTrack);
   return numActive;
}

//! Return the count of the number of peers that are targeting the specified track.
//! Includes all AIAI agents on same flight and any lead connected to the same GCI commander
//! @param aTrack The track of interest.
int WsfRIPRProcessor::PeersTargeting(const WsfTrack& aTrack)
{
   int               numTargeting     = 0;
   WsfRIPRProcessor* riprCommanderPtr = nullptr;
   WsfRIPRProcessor* tempProcPtr      = this->CommanderProcessor();
   // climb as high up the ripr command chain as possible
   while (tempProcPtr != nullptr)
   {
      riprCommanderPtr = tempProcPtr;
      tempProcPtr      = riprCommanderPtr->CommanderProcessor();
   }
   // add the number of all weapons fired against the track by subordinate ripr agents
   if (riprCommanderPtr != nullptr)
   {
      numTargeting += riprCommanderPtr->SubsTargeting(&aTrack);
   }
   numTargeting += this->Targeting(&aTrack);
   return numTargeting;
}

//! Return a list of weapons that are active against the specified platform
//! This uses truth rather than sensor data, so it should be fixed sometime
std::vector<WsfPlatform*> WsfRIPRProcessor::WeaponsIncoming()
{
   std::vector<WsfPlatform*> incomingPlatforms;
   WsfPlatform*              myPlatPtr = GetPlatform();
   // unsigned int platformIndex = myPlatPtr->GetIndex();
   WsfThreatProcessor* tp(nullptr);

   // get the first WSF_THREAT_PROCESSOR on the platform
   for (WsfComponentList::RoleIterator<WsfProcessor> iter(*myPlatPtr); !iter.AtEnd(); ++iter)
   {
      tp = dynamic_cast<WsfThreatProcessor*>(*iter);
      if (tp != nullptr)
      {
         break;
      }
   }

   if (tp == nullptr)
   {
      return incomingPlatforms;
   }

   WsfPlatform*                   tempPlatPtr   = nullptr;
   WsfPlatform*                   addPlatPtr    = nullptr;
   std::set<WsfTrackId>*          threatListPtr = tp->GetThreatList();
   std::set<WsfTrackId>::iterator iter;

   for (iter = threatListPtr->begin(); iter != threatListPtr->end(); ++iter)
   {
      const WsfTrackId* tidPtr = &(*iter);
      tempPlatPtr              = GetSimulation()->GetPlatformByName(tidPtr->GetOwningPlatformId());
      if (tempPlatPtr != nullptr)
      {
         // addPlatPtr = GetSimulation()->GetPlatformByIndex(tempPlatPtr->GetMasterTrackList().FindTrack((const
         // WsfTrackId &)(*tidPtr))->GetTargetIndex());
         WsfTrack* tempTrackPtr = tempPlatPtr->GetMasterTrackList().FindTrack((const WsfTrackId&)(*tidPtr));
         if (tempTrackPtr != nullptr)
         {
            addPlatPtr = GetSimulation()->GetPlatformByIndex(tempTrackPtr->GetTargetIndex());
            incomingPlatforms.push_back(addPlatPtr);
         }
      }
   }

   /* This commented out portion of code uses truth from observers rather than
   sensor data. It will not work for dis entities and has been taken out in
   favor of the new WsfThreatProcessor.

   for (ActiveWeaponList::iterator awli = mActiveWeaponList.begin(); awli != mActiveWeaponList.end(); ++awli)
   {
      ActiveWeapon& activeWeapon = *awli;
      if (platformIndex == activeWeapon.mTargetIndex)
      {
         incomingPlatforms.push_back(GetSimulation()->GetPlatformByIndex(activeWeapon.mPlatformIndex));
      }
   }
   */
   return incomingPlatforms;
}

std::string WsfRIPRProcessor::TargetName()
{
   WsfPlatform* pPtr = this->TargetPlatform();
   if (pPtr != nullptr)
   {
      return pPtr->GetName();
   }
   return std::string();
}

WsfPlatform* WsfRIPRProcessor::TargetPlatform()
{
   return GetSimulation()->GetPlatformByIndex(TargetPlatformId());
}

size_t WsfRIPRProcessor::TargetPlatformId()
{
   return mTargetPlatformIndex;
}

WsfTrack* WsfRIPRProcessor::TargetTrack()
{
   unsigned int N = GetPlatform()->GetMasterTrackList().GetTrackCount();
   for (unsigned int i = 0; i < N; ++i)
   {
      WsfTrack* tPtr = GetPlatform()->GetMasterTrackList().GetTrackEntry(i);
      if (tPtr->GetTargetIndex() == TargetPlatformId())
      {
         return tPtr;
      }
   }
   return nullptr;
}

bool WsfRIPRProcessor::SetTargetByName(std::string aTargetName)
{
   WsfPlatform* pPtr = GetSimulation()->GetPlatformByName(aTargetName);
   return this->SetTargetByPlatform(pPtr);
}

bool WsfRIPRProcessor::SetTargetByPlatform(WsfPlatform* aPlatformPtr)
{
   this->ClearTarget();
   if (aPlatformPtr != nullptr)
   {
      this->SetTargetByPlatformId(aPlatformPtr->GetIndex());
      return true;
   }
   return false;
}

void WsfRIPRProcessor::SetTargetByPlatformId(size_t aTargetId)
{
   mTargetPlatformIndex = aTargetId;
}

bool WsfRIPRProcessor::SetTargetByTrack(WsfTrack* aTargetTrackPtr)
{
   this->ClearTarget();
   if (aTargetTrackPtr != nullptr)
   {
      this->SetTargetByPlatformId(aTargetTrackPtr->GetTargetIndex());
      return true;
   }
   return false;
}

bool WsfRIPRProcessor::ClearTarget()
{
   mTargetPlatformIndex = 0;
   return true;
}

WsfRIPRProcessor::WeaponStatusList::iterator WsfRIPRProcessor::FindWeaponStatus(const WsfTrackId& aTrackId,
                                                                                size_t            aAssigneeIndex,
                                                                                WsfStringId       aWeaponSystemName)
{
   for (WeaponStatusList::iterator sli = mWeaponStatusList.begin(); sli != mWeaponStatusList.end(); ++sli)
   {
      WeaponStatus& status = *sli;
      if ((status.mAssigneeIndex == aAssigneeIndex) && (status.mWeaponSystemName == aWeaponSystemName) &&
          (status.mTrackId == aTrackId))
      {
         return sli;
      }
   }
   return mWeaponStatusList.end();
}

WsfRIPRJob* WsfRIPRProcessor::JobFor(double aSimTime, WsfRIPRProcessor* aProcPtr, int aChannelIndex)
{
   WsfRIPRJob* returnJobPtr = nullptr;

   // now we check aObjectPtr's job board for proc's-channel's job assignment
   WsfRIPRJob* jobPtr = JobBoard()->JobFor(aSimTime, aProcPtr->GetId(aChannelIndex));

   wsf::comm::Comm* jbCommPtr = JobBoardComm();

   // if we have comm defined
   if (jbCommPtr != nullptr)
   {
      int newJobId = ((jobPtr != nullptr) ? (jobPtr->GetId()) : (-1));
      int oldJobId = aProcPtr->BufferedJobForChannel(aChannelIndex);
      // if the new job is different than the old job, send it via comm
      if (oldJobId != newJobId)
      {
         SendMessageJobAward(jbCommPtr, aProcPtr, aChannelIndex, newJobId);
      }

      // return the job we've got buffered
      returnJobPtr = JobBoard()->GetJobById(aProcPtr->BufferedJobForChannel(aChannelIndex));
   }
   else
   {
      returnJobPtr = jobPtr;
   }

   return returnJobPtr;
}

void WsfRIPRProcessor::SendMessageJobAward(wsf::comm::Comm*  aCommPtr,
                                           WsfRIPRProcessor* aDestProcPtr,
                                           int               aDestChannelIndex,
                                           int               aJobId)
{
   WsfPlatform* destPlatPtr = aDestProcPtr->GetPlatform();

   for (WsfComponentList::RoleIterator<wsf::comm::Comm> it(*destPlatPtr); !it.AtEnd(); ++it)
   {
      auto commRcvrPtr = *it;
      if (commRcvrPtr->IsTurnedOn() && commRcvrPtr->CanReceive())
      {
         auto networkManagerPtr = GetSimulation()->GetCommNetworkManager();
         if (networkManagerPtr->PathExists(aCommPtr->GetAddress(), commRcvrPtr->GetAddress()))
         {
            auto jbmPtr = ut::make_unique<WsfMessage>(UtStringIdLiteral("JOB_BOARD_MESSAGE"),
                                                      UtStringIdLiteral("JOB_AWARD"),
                                                      GetPlatform());
            //   jbmPtr->SetDestination(destPlatPtr->GetNameId());
            WsfAttributeContainer& aux = jbmPtr->GetAuxData();
            aux.Assign("target_processor_name", aDestProcPtr->GetNameId().GetString());
            aux.Assign("job_id", aJobId);
            aux.Assign("bidder_proc_id", (int)aDestProcPtr->GetId(aDestChannelIndex));

            aCommPtr->Send(GetSimulation()->GetSimTime(), std::move(jbmPtr), commRcvrPtr->GetAddress());
            break;
         }
      }
   }
}

// NOTE: Even though this method derives from WsfScriptProcessor, it does NOT invoke its ProcessMethod to
//       give it a shot at the message. This is left to the final derived class (typically WsfRIPRProcessor)
//       to call and/or accept/reject the fact that the callee has indicated that it processed the message.
// virtual
bool WsfRIPRProcessor::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   bool processed = false;

   WsfStringId messageType    = aMessage.GetType();
   WsfStringId messageSubtype = aMessage.GetSubType();
   if (messageType == WsfTrackMessage::GetTypeId())
   {
      // check for uplinks
      WsfPlatform* platformPtr = GetPlatform();
      if (platformPtr != nullptr && WsfUplinkProcessor::HasUplinkProcessor(*platformPtr))
      {
         WsfUplinkProcessor::GetUplinkProcessor(*platformPtr)->ProcessMessage(aSimTime, aMessage);
      }
   }
   else if (messageSubtype == WsfStringId("SET_PROGRESS"))
   {
      int         job_id = aMessage.GetAuxDataConst().GetInt("job_id");
      WsfRIPRJob* jobPtr = JobBoard()->GetJobById(job_id);
      if (jobPtr == nullptr)
      {
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "SET_PROGRESS message for nonexistent job.";
            out.AddNote() << "job_id: " << job_id;
         }
      }
      else
      {
         const auto& auxData = aMessage.GetAuxDataConst();
         jobPtr->SetProgress(auxData.GetInt("bidder_id"), auxData.GetInt("progress"));
      }
      processed = true;
   }
   else if (messageSubtype == WsfStringId("BID_JOB"))
   {
      const WsfAttributeContainer& aux            = aMessage.GetAuxDataConst();
      double                       bid            = aux.GetDouble("bid");
      double                       progress       = aux.GetDouble("progress");
      int                          job_id         = aux.GetInt("job_id");
      int                          bidder_proc_id = aux.GetInt("bidder_proc_id");

      WsfRIPRJob* jobPtr = JobBoard()->GetJobById(job_id);
      if (jobPtr == nullptr)
      {
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "BID_JOB message for bidder_proc_id: Nonexistent job.";
            out.AddNote() << "bidder_proc_id: " << bidder_proc_id;
            out.AddNote() << "job_id: " << job_id;
         }
      }
      else
      {
         jobPtr->BidJob(bidder_proc_id, bid, progress);
      }
      processed = true;
   }
   else if (messageSubtype == WsfStringId("UNBID_JOB"))
   {
      const WsfAttributeContainer& aux            = aMessage.GetAuxDataConst();
      int                          job_id         = aux.GetInt("job_id");
      int                          bidder_proc_id = aux.GetInt("bidder_proc_id");
      WsfRIPRJob*                  jobPtr         = JobBoard()->GetJobById(job_id);
      if (jobPtr == nullptr)
      {
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "UNBID_JOB message for bidder_proc_id: Nonexistent job.";
            out.AddNote() << "bidder_proc_id: " << bidder_proc_id;
            out.AddNote() << "job_id: " << job_id;
         }
      }
      else
      {
         jobPtr->UnbidJob(bidder_proc_id);
      }
      processed = true;
   }
   else if (messageSubtype == WsfStringId("JOB_AWARD"))
   {
      const WsfAttributeContainer& aux            = aMessage.GetAuxDataConst();
      int                          job_id         = aux.GetInt("job_id");
      int                          bidder_proc_id = aux.GetInt("bidder_proc_id");

      int channel = this->JobChannelByProcId(bidder_proc_id);

      this->SetBufferedJobForChannel(channel, job_id);

      processed = true;
   }

   if (!processed)
   {
      processed = WsfScriptProcessor::ProcessMessage(aSimTime, aMessage);
   }
   return processed;
}

// virtual
bool WsfRIPRProcessor::ReceiveMessage(double aSimTime, const WsfMessage& aMessage)
{
   // If the message is a job board message then make sure the message is intended for me.
   // (If sent through a broadcast medium then all parties will receive the message).
   if (aMessage.GetType() == "JOB_BOARD_MESSAGE")
   {
      auto* destComm = GetSimulation()->GetCommNetworkManager()->GetComm(aMessage.GetDstAddr());
      if (GetPlatform()->GetNameId() != destComm->GetPlatform()->GetNameId())
      {
         return false;
      }
      // If it is also directed to a specific processor then make sure this is the correct processor.
      WsfStringId target = aMessage.GetAuxDataConst().GetString("target_processor_name");
      if ((!target.IsNull()) && (target != GetNameId()))
      {
         return false;
      }
   }
   return ProcessMessage(aSimTime, aMessage);
}

const char* WsfRIPRProcessor::GetScriptClassName() const
{
   return "WsfRIPRProcessor";
}
