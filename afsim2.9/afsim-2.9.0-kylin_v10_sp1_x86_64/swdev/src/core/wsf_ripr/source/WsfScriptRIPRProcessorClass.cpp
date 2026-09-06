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

//***************************************************************************
//* FILE NAME:
//*   WsfScriptRIPRProcessorClass.cpp
//*
//* PURPOSE:
//*   The WsfScriptRIPRProcessorClass class inherits from
//*   WsfScriptProcessorClass and exposes the RIPR API to the script input
//*   files.
//*
//* NOTES:
//*   None
//***************************************************************************

#include "WsfScriptRIPRProcessorClass.hpp"

// NOTE: This code will probably fail on a 64 bit machine if the size of a
//       pointer and an int are not the same.  We pass pointers back to
//       script ints and thus there is a possibility of truncation.

#if defined(unix)
#include <stdint.h>
#elif defined(_WIN32)
#include <cstddef>
#endif

#include "UtMemory.hpp"
#include "UtScriptRef.hpp"
#include "WsfPlatform.hpp"
#include "WsfProcessor.hpp"
#include "WsfRIPRJob.hpp"
#include "WsfRIPRJobBoard.hpp"
#include "WsfRIPRManager.hpp"
#include "WsfRIPRProcessor.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptProcessorClass.hpp"


WsfScriptRIPRProcessorClass::WsfScriptRIPRProcessorClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptProcessorClass(aClassName, aScriptTypesPtr)
{
   AddClassName("WsfRIPRProcessor");

   AddMethod(ut::make_unique<CommanderProcessor_1>("CommanderProcessor")); // WsfRIPRProcessor CommanderProcessor()
   AddMethod(ut::make_unique<CommanderProcessor_1>(
      "GetRIPRCommanderProcessor")); // NO_DOC | DEPRECATED - no "get"  //WsfRIPRProcessor GetRIPRCommanderProcessor()
   AddMethod(ut::make_unique<CommanderProcessor_2>("CommanderProcessor")); // WsfRIPRProcessor CommanderProcessor(string)
   AddMethod(ut::make_unique<CommanderProcessor_2>(
      "GetRIPRCommanderProcessor")); // NO_DOC | DEPRECATED - no "get"  //WsfRIPRProcessor GetRIPRCommanderProcessor(string)

   AddMethod(ut::make_unique<CommanderPlatform_1>("CommanderPlatform")); // WsfPlatform CommanderPlatform()
   AddMethod(ut::make_unique<CommanderPlatform_1>(
      "GetRIPRCommanderPlatform")); // NO_DOC | DEPRECATED - no "get"  //WsfPlatform GetRIPRCommanderPlatform()
   AddMethod(ut::make_unique<CommanderPlatform_2>("CommanderPlatform")); // WsfPlatform CommanderPlatform(string)
   AddMethod(ut::make_unique<CommanderPlatform_2>(
      "GetRIPRCommanderPlatform")); // NO_DOC | DEPRECATED - no "get"  //WsfPlatform GetRIPRCommanderPlatform(string)

   AddMethod(ut::make_unique<SubordinateProcessors_1>(
      "SubordinateProcessors")); // Array<WsfRIPRProcessor> SubordinateProcessor()
   AddMethod(ut::make_unique<SubordinateProcessors_1>("GetRIPRSubordinateProcessors")); // NO_DOC | DEPRECATED - no "get"
                                                                                        // //Array<WsfRIPRProcessor>
                                                                                        // GetRIPRSubordinateProcessors()
   AddMethod(ut::make_unique<SubordinateProcessors_2>("SubordinateProcessors")); // Array<WsfRIPRProcessor>
                                                                                 // SubordinateProcessor(string)
   AddMethod(ut::make_unique<SubordinateProcessors_2>(
      "GetRIPRSubordinateProcessors")); // NO_DOC | DEPRECATED - no "get"  //Array<WsfRIPRProcessor>
                                        // GetRIPRSubordinateProcessors(string)

   AddMethod(ut::make_unique<SubordinatePlatforms_1>("SubordinatePlatforms")); // Array<WsfRIPRProcessor> SubordinatePlatform()
   AddMethod(ut::make_unique<SubordinatePlatforms_1>("GetRIPRSubordinatePlatforms")); // NO_DOC | DEPRECATED - no "get"
                                                                                      // //Array<WsfRIPRProcessor>
                                                                                      // GetRIPRSubordinatePlatforms()
   AddMethod(ut::make_unique<SubordinatePlatforms_2>(
      "SubordinatePlatforms")); // Array<WsfRIPRProcessor> SubordinatePlatform(string)
   AddMethod(ut::make_unique<SubordinatePlatforms_2>(
      "GetRIPRSubordinatePlatforms")); // NO_DOC | DEPRECATED - no "get"  //Array<WsfRIPRProcessor>
                                       // GetRIPRSubordinatePlatforms(string)

   AddMethod(ut::make_unique<QuerySubordinatesMaxBid_1>(
      "QuerySubordinatesMaxBid")); // double QuerySubordinatesMaxBid(WsfRIPRJob)
   AddMethod(ut::make_unique<QuerySubordinatesMaxBid_2>(
      "QuerySubordinatesMaxBid")); // double QuerySubordinatesMaxBid(WsfRIPRJob, bool)

   AddMethod(ut::make_unique<AddJob>());    // AddJob(WsfRIPRJob)
   AddMethod(ut::make_unique<RemoveJob>()); // RemoveJob(WsfRIPRJob)
   AddMethod(ut::make_unique<RemoveAllJobs>());
   AddMethod(ut::make_unique<DeleteCompletedJobs>());
   AddMethod(ut::make_unique<PurgeDeadTakers>());
   AddMethod(ut::make_unique<Jobs>());                // Array<WsfRIPRJob> Jobs()
   AddMethod(ut::make_unique<Jobs>("GetJobs"));       // NO_DOC | DEPRECATED - no "get"  //Array<WsfRIPRJob> GetJobs()
   AddMethod(ut::make_unique<JobFor_1>("JobFor"));    // WsfRIPRJob JobFor(double,WsfRIPRProcessor)
   AddMethod(ut::make_unique<JobFor_2>("JobFor"));    // WsfRIPRJob JobFor(double,WsfRIPRProcessor,int)
   AddMethod(ut::make_unique<JobFor_3>("JobFor"));    // WsfRIPRJob JobFor(WsfRIPRProcessor)
   AddMethod(ut::make_unique<JobFor_4>("JobFor"));    // WsfRIPRJob JobFor(WsfRIPRProcessor,int)
   AddMethod(ut::make_unique<JobFor_1>("GetJobFor")); // NO_DOC | DEPRECATED - no "get"  //WsfRIPRJob
                                                      // GetJobFor(double,WsfRIPRProcessor)
   AddMethod(ut::make_unique<JobFor_2>("GetJobFor")); // NO_DOC | DEPRECATED - no "get"  //WsfRIPRJob
                                                      // GetJobFor(double,WsfRIPRProcessor,int)
   AddMethod(ut::make_unique<JobFor_3>("GetJobFor")); // NO_DOC | DEPRECATED - no "get"  //WsfRIPRJob GetJobFor(WsfRIPRProcessor)
   AddMethod(ut::make_unique<JobFor_4>(
      "GetJobFor")); // NO_DOC | DEPRECATED - no "get"  //WsfRIPRJob GetJobFor(WsfRIPRProcessor,int)
   AddMethod(ut::make_unique<ClearBidsFor_1>("ClearBidsFor"));            // ClearBidsFor(WsfRIPRProcessor)
   AddMethod(ut::make_unique<ClearBidsFor_2>("ClearBidsFor"));            // ClearBidsFor(WsfRIPRProcessor,int)
   AddMethod(ut::make_unique<GetJobByData>());                            // WsfRIPRJob GetJobByData(string,WsfObject)
   AddMethod(ut::make_unique<GetJobById>());                              // WsfRIPRJob GetJobById(int)
   AddMethod(ut::make_unique<JobAllocationMode>());                       // double JobAllocationMode()
   AddMethod(ut::make_unique<JobAllocationMode>("GetJobAllocationMode")); // NO_DOC | DEPRECATED - should not have the
                                                                          // 'get'   //double GetJobAllocationMode()
   AddMethod(ut::make_unique<SetJobAllocationMode>());                    // SetJobAllocationMode(int)
   AddMethod(ut::make_unique<IsJobWindowOpen>());
   AddMethod(ut::make_unique<IsBidWindowOpen>());
   AddMethod(ut::make_unique<SetJobWindowOpen>()); // SetJobWindowOpen(bool)
   AddMethod(ut::make_unique<SetBidWindowOpen>()); // SetBidWindowOpen(bool)
   AddMethod(ut::make_unique<SetJobStickiness>()); // SetJobStickiness(double)
   AddMethod(ut::make_unique<PurgeTouchTime>()); // NO_DOC | PENDING - feature could be removed    //double PurgeTouchTime()
   AddMethod(ut::make_unique<SetPurgeTouchTime>()); // NO_DOC | PENDING - feature could be removed    //void SetPurgeTouchTime(double)
   AddMethod(ut::make_unique<PurgeUntouchedJobs_1>("PurgeUntouchedJobs")); // NO_DOC | PENDING - feature could be
                                                                           // removed    //int PurgeUntouchedJobs()
   AddMethod(ut::make_unique<PurgeUntouchedJobs_2>("PurgeUntouchedJobs")); // NO_DOC | PENDING - feature could be removed
                                                                           // //int PurgeUntouchedJobs(double)

   AddMethod(ut::make_unique<JobPassThrough>());
   AddMethod(ut::make_unique<JobPassThrough>("GetJobPassThrough")); // NO_DOC | DEPRECATED - should not have the 'get'
                                                                    // //bool JobPassThrough()
   AddMethod(ut::make_unique<SetJobPassThrough>());                 // SetJobPassThrough(bool)

   AddMethod(ut::make_unique<QueryBid>());                          // double QueryBid(WsfRIPRJob)
   AddMethod(ut::make_unique<NumJobChannels>());                    // int NumJobChannels()
   AddMethod(ut::make_unique<NumJobChannels>("GetNumJobChannels")); // NO_DOC | DEPRECATED - no "get"  //int GetNumJobChannels()
   AddMethod(ut::make_unique<SetNumJobChannels>());                 // SetNumJobChannels(int)
   AddMethod(ut::make_unique<JobTypeChannels>());                   // Array<unsigned int> JobTypeChannels(string)
   AddMethod(ut::make_unique<ChannelJobTypes>());                   // Array<string> ChannelJobType(unsigned int)

   AddMethod(ut::make_unique<WallClockTime>());                   // double WallClockTime()
   AddMethod(ut::make_unique<WallClockTime>("GetWallClockTime")); // NO_DOC | DEPRECATED - no "get"  //double GetWallClockTime()

   AddMethod(ut::make_unique<WeaponsActive_1>("WeaponsActive")); // int WeaponsActive()
   AddMethod(ut::make_unique<WeaponsActive_2>("WeaponsActive")); // int WeaponsActive(WsfTrack)
   AddMethod(ut::make_unique<WeaponsActiveOfType>());            // int WeaponsActiveOfType(int)
   AddMethod(ut::make_unique<ActiveWeaponPlatform>());           // WsfPlatform ActiveWeaponPlatform(int)

   AddMethod(ut::make_unique<PeersWeaponsActive>());                     // int PeersWeaponsActive(WsfTrack)
   AddMethod(ut::make_unique<PeersTargeting>());                         // int PeersTargeting(WsfTrack)
   AddMethod(ut::make_unique<SubsTargeting_1>("SubsTargeting"));         // int SubsTargeting(WsfTrack)
   AddMethod(ut::make_unique<SubsTargeting_2>("SubsTargeting"));         // int SubsTargeting(WsfTrack, WsfPlatform)
   AddMethod(ut::make_unique<SubsWeaponsActive_1>("SubsWeaponsActive")); // int SubsWeaponsActive(WsfTrack)
   AddMethod(ut::make_unique<SubsWeaponsActive_2>("SubsWeaponsActive")); // int SubsWeaponsActive(WsfTrack, WsfPlatform)
   AddMethod(ut::make_unique<WeaponsIncoming>());                        // int WeaponsIncoming(Array<WsfPlatform>

   AddMethod(ut::make_unique<Target>());            // WsfTrack Target() - same as TargetTrack(), legacy
   AddMethod(ut::make_unique<Target>("GetTarget")); // NO_DOC | DEPRECATED - no "get"  //WsfTrack GetTarget() - same as
                                                    // TargetTrack(), legacy
   AddMethod(ut::make_unique<TargetTrack>());       // WsfTrack TargetTrack()
   AddMethod(ut::make_unique<TargetTrack>("GetTargetTrack")); // NO_DOC | DEPRECATED - no "get"  //WsfTrack GetTargetTrack()
   AddMethod(ut::make_unique<TargetPlatform>());              // WsfPlatform TargetPlatform()
   AddMethod(ut::make_unique<TargetPlatform>("GetTargetPlatform")); // NO_DOC | DEPRECATED - no "get"  //WsfPlatform
                                                                    // GetTargetPlatform()
   AddMethod(ut::make_unique<TargetName>());                        // string TargetName()
   AddMethod(ut::make_unique<TargetName>("GetTargetName")); // NO_DOC | DEPRECATED - no "get"  //string GetTargetName()
   AddMethod(ut::make_unique<SetTarget_1>("SetTarget"));    // bool SetTarget(string)
   AddMethod(ut::make_unique<SetTarget_2>("SetTarget"));    // bool SetTarget(WsfPlatform)
   AddMethod(ut::make_unique<SetTarget_3>("SetTarget"));    // bool SetTarget(WsfTrack)
   AddMethod(ut::make_unique<ClearTarget>());               // bool ClearTarget()

   AddMethod(ut::make_unique<StartUplinking_1>("StartUplinking")); // bool StartUplinking( WsfPlatform )
   AddMethod(ut::make_unique<StartUplinking_2>("StartUplinking")); // bool StartUplinking( WsfPlatform, WsfTrack )
   AddMethod(ut::make_unique<StopUplinking>());                    // bool StopUplinking( WsfPlatform )
   AddMethod(ut::make_unique<IsUplinkingTo>());                    // bool IsUplinkingTo( WsfPlatform )
   AddMethod(ut::make_unique<UplinkCount>());                      // int UplinkCount( )
   AddMethod(ut::make_unique<UplinkPlatformEntry>());              // WsfPlatform UplinkPlatformEntry( int )
   AddMethod(ut::make_unique<UplinkCapable>());                    // bool UplinkCapable( )
}

// virtual
void* WsfScriptRIPRProcessorClass::Create(const UtScriptContext& aContext)
{
   return static_cast<void*>(new WsfRIPRProcessor(*WsfScriptContext::GetSCENARIO(aContext), false));
}

// virtual
void* WsfScriptRIPRProcessorClass::Clone(void* aObjectPtr)
{
   WsfRIPRProcessor* objPtr = static_cast<WsfRIPRProcessor*>(aObjectPtr);
   assert(objPtr != nullptr);
   return objPtr->Clone();
}

// virtual
void WsfScriptRIPRProcessorClass::Destroy(void* aObjectPtr)
{
   WsfRIPRProcessor* objPtr = static_cast<WsfRIPRProcessor*>(aObjectPtr);
   delete objPtr;
}

UtScriptContext* WsfScriptRIPRProcessorClass::GetContext(void* aObjectPtr)
{
   UtScriptContext*  contextPtr   = nullptr;
   WsfRIPRProcessor* processorPtr = (WsfRIPRProcessor*)aObjectPtr;
   if (processorPtr != nullptr)
   {
      contextPtr = &(processorPtr->GetScriptContext().GetContext());
   }
   return contextPtr;
}

#include "script/WsfScriptDefs.hpp"

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, WallClockTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->WallClock());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, NumJobChannels, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->NumJobChannels());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, SetNumJobChannels, 1, "void", "int")
{
   int          temp        = aVarArgs[0].GetInt();
   unsigned int numChannels = (temp < 1) ? (1) : ((unsigned int)temp);
   aObjectPtr->SetNumJobChannels(numChannels);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, JobTypeChannels, 1, "Array<int>", "string")
{
   std::string                         jobType  = aVarArgs[0].GetString();
   std::vector<unsigned int>           channels = aObjectPtr->JobTypeChannels(jobType);
   std::vector<unsigned int>::iterator it;
   std::vector<UtScriptData>*          tempVecPtr = new std::vector<UtScriptData>();
   for (it = channels.begin(); it != channels.end(); ++it)
   {
      tempVecPtr->push_back(UtScriptData((int)(*it)));
   }
   aReturnVal.SetPointer(new UtScriptRef(tempVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, ChannelJobTypes, 1, "Array<string>", "int")
{
   int                                channel  = aVarArgs[0].GetInt();
   std::vector<std::string>           jobTypes = aObjectPtr->ChannelJobTypes(channel);
   std::vector<std::string>::iterator it;
   std::vector<UtScriptData>*         tempVecPtr = new std::vector<UtScriptData>();
   for (it = jobTypes.begin(); it != jobTypes.end(); ++it)
   {
      tempVecPtr->push_back(UtScriptData(*it));
   }
   aReturnVal.SetPointer(new UtScriptRef(tempVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, CommanderProcessor_1, 0, "WsfRIPRProcessor", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->CommanderProcessor(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, CommanderProcessor_2, 1, "WsfRIPRProcessor", "string")
{
   std::string CommandChainName = aVarArgs[0].GetString();
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->CommanderProcessor(CommandChainName), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, CommanderPlatform_1, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->CommanderPlatform(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, CommanderPlatform_2, 1, "WsfPlatform", "string")
{
   std::string CommandChainName = aVarArgs[0].GetString();
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->CommanderPlatform(CommandChainName), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, SubordinateProcessors_1, 0, "Array<WsfRIPRProcessor>", "")
{
   std::vector<WsfRIPRProcessor*>                 procs = aObjectPtr->SubordinateProcessors();
   std::vector<WsfRIPRProcessor*>::const_iterator it;
   std::vector<UtScriptData>*                     tempVecPtr = new std::vector<UtScriptData>();

   UtScriptClass* dataClassPtr = aContext.GetTypes()->GetClass(aReturnClassPtr->GetContainerDataTypeId());
   for (it = procs.begin(); it != procs.end(); ++it)
   {
      tempVecPtr->push_back(UtScriptData(new UtScriptRef((*it), dataClassPtr, UtScriptRef::cDONT_MANAGE)));
   }
   aReturnVal.SetPointer(new UtScriptRef(tempVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, SubordinateProcessors_2, 1, "Array<WsfRIPRProcessor>", "string")
{
   std::string                                    commandChainName = aVarArgs[0].GetString();
   std::vector<WsfRIPRProcessor*>                 procs = aObjectPtr->SubordinateProcessors(commandChainName);
   std::vector<WsfRIPRProcessor*>::const_iterator it;
   std::vector<UtScriptData>*                     tempVecPtr = new std::vector<UtScriptData>();

   UtScriptClass* dataClassPtr = aContext.GetTypes()->GetClass(aReturnClassPtr->GetContainerDataTypeId());
   for (it = procs.begin(); it != procs.end(); ++it)
   {
      tempVecPtr->push_back(UtScriptData(new UtScriptRef((*it), dataClassPtr, UtScriptRef::cDONT_MANAGE)));
   }
   aReturnVal.SetPointer(new UtScriptRef(tempVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, SubordinatePlatforms_1, 0, "Array<WsfPlatform>", "")
{
   std::vector<WsfPlatform*>                 plats = aObjectPtr->SubordinatePlatforms();
   std::vector<WsfPlatform*>::const_iterator it;
   std::vector<UtScriptData>*                tempVecPtr = new std::vector<UtScriptData>();

   UtScriptClass* dataClassPtr = aContext.GetTypes()->GetClass(aReturnClassPtr->GetContainerDataTypeId());
   for (it = plats.begin(); it != plats.end(); ++it)
   {
      tempVecPtr->push_back(UtScriptData(new UtScriptRef((*it), dataClassPtr, UtScriptRef::cDONT_MANAGE)));
   }
   aReturnVal.SetPointer(new UtScriptRef(tempVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, SubordinatePlatforms_2, 1, "Array<WsfPlatform>", "string")
{
   std::string                               commandChainName = aVarArgs[0].GetString();
   std::vector<WsfPlatform*>                 plats            = aObjectPtr->SubordinatePlatforms(commandChainName);
   std::vector<WsfPlatform*>::const_iterator it;
   std::vector<UtScriptData>*                tempVecPtr = new std::vector<UtScriptData>();

   UtScriptClass* dataClassPtr = aContext.GetTypes()->GetClass(aReturnClassPtr->GetContainerDataTypeId());
   for (it = plats.begin(); it != plats.end(); ++it)
   {
      tempVecPtr->push_back(UtScriptData(new UtScriptRef((*it), dataClassPtr, UtScriptRef::cDONT_MANAGE)));
   }
   aReturnVal.SetPointer(new UtScriptRef(tempVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, QuerySubordinatesMaxBid_1, 1, "double", "WsfRIPRJob")
{
   WsfRIPRJob* aJobPtr = (WsfRIPRJob*)aVarArgs[0].GetPointer()->GetAppObject();
   double      maxBid  = aObjectPtr->QuerySubordinatesMaxBid(*aJobPtr); // expands pass-through processors by default
   aReturnVal.SetDouble(maxBid);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, QuerySubordinatesMaxBid_2, 2, "double", "WsfRIPRJob, bool")
{
   WsfRIPRJob* aJobPtr            = (WsfRIPRJob*)aVarArgs[0].GetPointer()->GetAppObject();
   bool        expandPassThroughs = aVarArgs[1].GetBool();
   double      maxBid             = aObjectPtr->QuerySubordinatesMaxBid(*aJobPtr, expandPassThroughs);
   aReturnVal.SetDouble(maxBid);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, AddJob, 1, "void", "WsfRIPRJob")
{
   WsfRIPRJob* tempPtr = (WsfRIPRJob*)aVarArgs[0].GetPointer()->GetAppObject();
   aObjectPtr->JobBoard()->AddJob(tempPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, RemoveJob, 1, "void", "WsfRIPRJob")
{
   WsfRIPRJob* tempPtr = (WsfRIPRJob*)aVarArgs[0].GetPointer()->GetAppObject();
   aObjectPtr->JobBoard()->RemoveJob(tempPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, RemoveAllJobs, 0, "void", "")
{
   aObjectPtr->JobBoard()->RemoveAllJobs();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, DeleteCompletedJobs, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->JobBoard()->DeleteCompletedJobs(TIME_NOW));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, PurgeDeadTakers, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->JobBoard()->PurgeDeadBidders(TIME_NOW));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, Jobs, 0, "Array<WsfRIPRJob>", "")
{
   std::list<WsfRIPRJob*>                 jobList = aObjectPtr->JobBoard()->GetJobs();
   std::list<WsfRIPRJob*>::const_iterator i;
   std::vector<UtScriptData>*             tempVecPtr = new std::vector<UtScriptData>();

   UtScriptClass* dataClassPtr = aContext.GetTypes()->GetClass(aReturnClassPtr->GetContainerDataTypeId());
   for (i = jobList.begin(); i != jobList.end(); ++i)
   {
      tempVecPtr->push_back(UtScriptData(new UtScriptRef((*i), dataClassPtr, UtScriptRef::cDONT_MANAGE)));
   }
   aReturnVal.SetPointer(new UtScriptRef(tempVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, JobFor_1, 2, "WsfRIPRJob", "double, WsfRIPRProcessor")
{
   double            simTime = aVarArgs[0].GetDouble();
   WsfRIPRProcessor* procPtr = (WsfRIPRProcessor*)aVarArgs[1].GetPointer()->GetAppObject();

   WsfRIPRJob* returnJobPtr = aObjectPtr->JobFor(simTime, procPtr, 0);

   aReturnVal.SetPointer(new UtScriptRef(returnJobPtr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, JobFor_2, 3, "WsfRIPRJob", "double, WsfRIPRProcessor, int")
{
   double simTime = aVarArgs[0].GetDouble();

   // proc is the platform that wants to know what it's job is, probably aObjectPtr's subordinate
   WsfRIPRProcessor* procPtr = (WsfRIPRProcessor*)aVarArgs[1].GetPointer()->GetAppObject();

   // channel is the channel on procPtr that wants to know it's job
   unsigned int channel = (unsigned int)aVarArgs[2].GetInt();

   WsfRIPRJob* returnJobPtr = aObjectPtr->JobFor(simTime, procPtr, channel);

   aReturnVal.SetPointer(new UtScriptRef(returnJobPtr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, JobFor_3, 1, "WsfRIPRJob", "WsfRIPRProcessor")
{
   WsfRIPRProcessor* procPtr = (WsfRIPRProcessor*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfRIPRJob*       jobPtr  = aObjectPtr->JobBoard()->JobFor(TIME_NOW, procPtr->GetId());

   aReturnVal.SetPointer(new UtScriptRef(jobPtr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, JobFor_4, 2, "WsfRIPRJob", "WsfRIPRProcessor,int")
{
   WsfRIPRProcessor* procPtr = (WsfRIPRProcessor*)aVarArgs[0].GetPointer()->GetAppObject();
   unsigned int      channel = (unsigned int)aVarArgs[1].GetInt();
   WsfRIPRJob*       jobPtr  = aObjectPtr->JobBoard()->JobFor(TIME_NOW, procPtr->GetId(channel));

   aReturnVal.SetPointer(new UtScriptRef(jobPtr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, GetJobById, 1, "WsfRIPRJob", "int")
{
   int         jobId  = aVarArgs[0].GetInt();
   WsfRIPRJob* jobPtr = aObjectPtr->JobBoard()->GetJobById(jobId);
   aReturnVal.SetPointer(new UtScriptRef(jobPtr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, GetJobByData, 2, "WsfRIPRJob", "string, Object")
{
   std::string dataName = aVarArgs[0].GetString();

   WsfRIPRJob* jobPtr = aObjectPtr->JobBoard()->GetJobByData(dataName, aVarArgs[1]);

   aReturnVal.SetPointer(new UtScriptRef(jobPtr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, ClearBidsFor_1, 1, "void", "WsfRIPRProcessor")
{
   WsfRIPRProcessor* procPtr = (WsfRIPRProcessor*)aVarArgs[0].GetPointer()->GetAppObject();

   aObjectPtr->JobBoard()->ClearBidsFor(TIME_NOW, procPtr->GetId());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, ClearBidsFor_2, 2, "void", "WsfRIPRProcessor, int")
{
   WsfRIPRProcessor* procPtr = (WsfRIPRProcessor*)aVarArgs[0].GetPointer()->GetAppObject();
   unsigned int      channel = (unsigned int)aVarArgs[1].GetInt();

   aObjectPtr->JobBoard()->ClearBidsFor(TIME_NOW, procPtr->GetId(channel));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, JobAllocationMode, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->JobBoard()->GetAllocationMode());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, SetJobAllocationMode, 1, "void", "int")
{
   aObjectPtr->JobBoard()->SetAllocationMode((WsfRIPRJobBoard::AllocationMode)aVarArgs[0].GetInt());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, IsJobWindowOpen, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->JobWindowOpen(TIME_NOW));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, IsBidWindowOpen, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->BidWindowOpen(TIME_NOW));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, SetJobWindowOpen, 1, "void", "bool")
{
   bool open = (bool)aVarArgs[0].GetBool();
   aObjectPtr->SetJobWindowOpen(open);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, SetBidWindowOpen, 1, "void", "bool")
{
   bool open = (bool)aVarArgs[0].GetBool();
   aObjectPtr->SetBidWindowOpen(open);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, SetJobStickiness, 1, "void", "double")
{
   double stickiness = (double)aVarArgs[0].GetDouble();
   aObjectPtr->SetJobStickiness(stickiness);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, PurgeTouchTime, 0, "double", "") // NO_DOC | PENDING
{
   aReturnVal.SetDouble(aObjectPtr->JobBoard()->PurgeTouchTime());
}
UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, SetPurgeTouchTime, 1, "void", "double") // NO_DOC | PENDING
{
   double ptt = (double)aVarArgs[0].GetDouble();
   aObjectPtr->JobBoard()->SetPurgeTouchTime(ptt);
}
UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, PurgeUntouchedJobs_1, 0, "int", "") // NO_DOC | PENDING
{
   aReturnVal.SetInt(aObjectPtr->JobBoard()->PurgeUntouchedJobs());
}
UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, PurgeUntouchedJobs_2, 1, "int", "double") // NO_DOC | PENDING
{
   double st = (double)aVarArgs[0].GetDouble();
   aReturnVal.SetInt(aObjectPtr->JobBoard()->PurgeUntouchedJobs(st));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, JobPassThrough, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->JobPassThrough());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, SetJobPassThrough, 1, "void", "bool")
{
   aObjectPtr->SetJobPassThrough(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, QueryBid, 1, "double", "WsfRIPRJob")
{
   WsfRIPRJob* tempPtr = (WsfRIPRJob*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetDouble(aObjectPtr->QueryBid(*tempPtr));
}

//! Returns the total number of weapons in flight against aTarget
//! Includes all AIAI agents on same flight and any lead connected to the same GCI commander
//! aTarget == the track to check for weapons in flight against
UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, PeersWeaponsActive, 1, "int", "WsfTrack")
{
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetInt(aObjectPtr->PeersWeaponsActive((const WsfTrack&)(*trackPtr)));
}

//! Returns the total number of peer agents targeting the track
//! Includes all AIAI agents on same flight and any lead connected to the same GCI commander
//! aTarget == the track to check for targeting
UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, PeersTargeting, 1, "int", "WsfTrack")
{
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetInt(aObjectPtr->PeersTargeting((const WsfTrack&)(*trackPtr)));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, SubsTargeting_1, 1, "int", "WsfTrack")
{
   WsfTrack* targetPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetInt(aObjectPtr->SubsTargeting(targetPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, SubsTargeting_2, 2, "int", "WsfTrack, WsfPlatform")
{
   WsfTrack*    targetPtr  = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfPlatform* excludePtr = (WsfPlatform*)aVarArgs[1].GetPointer()->GetAppObject();
   aReturnVal.SetInt(aObjectPtr->SubsTargeting(targetPtr, excludePtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, SubsWeaponsActive_1, 2, "int", "WsfTrack, WsfPlatform")
{
   WsfTrack*    targetPtr  = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfPlatform* excludePtr = (WsfPlatform*)aVarArgs[1].GetPointer()->GetAppObject();
   aReturnVal.SetInt(aObjectPtr->SubsWeaponsActive(targetPtr, excludePtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, SubsWeaponsActive_2, 1, "int", "WsfTrack")
{
   WsfTrack* targetPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetInt(aObjectPtr->SubsWeaponsActive(targetPtr));
}

//! Returns the total number of weapons in flight against aTarget
//! aTarget == the track to check for weapons in flight against
UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, WeaponsActive_1, 1, "int", "WsfTrack")
{
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();

   aReturnVal.SetInt(aObjectPtr->WeaponsActive((const WsfTrack&)(*trackPtr)));
}

//! Returns the total number of weapons in flight
UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, WeaponsActive_2, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->WeaponsActive());
}

//! Returns the total number of weapons of given type in flight
//! argument 1 == the weapon type to check for number in flight
UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, WeaponsActiveOfType, 1, "int", "WsfWeapon")
{
   WsfWeapon* weaponPtr = (WsfWeapon*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetInt(aObjectPtr->WeaponsActiveOfType((const WsfWeapon&)(*weaponPtr)));
}

//! Returns the weapon platform of active weapon at the given index
//! argument 1 == the index of the active weapon (as known by the processor)
UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, ActiveWeaponPlatform, 1, "WsfPlatform", "int")
{
   unsigned int index = (unsigned int)aVarArgs[0].GetInt();
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->ActiveWeaponPlatform(index), aReturnClassPtr));
}

//! Returns a the number of weapons incoming on aPlatform and a list of those platforms (as a parameter)
UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, WeaponsIncoming, 1, "int", "Array<WsfPlatform>")
{
   std::vector<UtScriptData>* platListPtr =
      static_cast<std::vector<UtScriptData>*>(aVarArgs[0].GetPointer()->GetAppObject());

   WsfStringId    platClassTypeId = aContext.GetTypes()->GetTypeId("WsfPlatform");
   UtScriptClass* platClassPtr    = aContext.GetTypes()->GetClass(platClassTypeId);

   std::vector<WsfPlatform*> incomingList = aObjectPtr->WeaponsIncoming();

   for (unsigned int i = 0; i < incomingList.size(); ++i)
   {
      platListPtr->push_back(UtScriptData(new UtScriptRef(incomingList[i], platClassPtr)));
   }

   aReturnVal.SetInt(static_cast<int>(platListPtr->size()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, Target, 0, "WsfTrack", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->TargetTrack(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, TargetTrack, 0, "WsfTrack", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->TargetTrack(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, TargetPlatform, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->TargetPlatform(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, TargetName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->TargetName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, SetTarget_1, 1, "bool", "string")
{
   std::string targetName = aVarArgs[0].GetString();
   aReturnVal.SetBool(aObjectPtr->SetTargetByName(targetName));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, SetTarget_2, 1, "bool", "WsfPlatform")
{
   WsfPlatform* targetPlatformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetBool(aObjectPtr->SetTargetByPlatform(targetPlatformPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, SetTarget_3, 1, "bool", "WsfTrack")
{
   WsfTrack* targetTrackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetBool(aObjectPtr->SetTargetByTrack(targetTrackPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, ClearTarget, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->ClearTarget());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, StartUplinking_1, 1, "bool", "WsfPlatform")
{
   WsfPlatform* weaponPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetBool(aObjectPtr->StartUplinking(weaponPtr, nullptr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, StartUplinking_2, 2, "bool", "WsfPlatform, WsfTrack")
{
   WsfPlatform* weaponPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfTrack*    targetPtr = (WsfTrack*)aVarArgs[1].GetPointer()->GetAppObject();
   aReturnVal.SetBool(aObjectPtr->StartUplinking(weaponPtr, targetPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, StopUplinking, 1, "bool", "WsfPlatform")
{
   WsfPlatform* weaponPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetBool(aObjectPtr->StopUplinking(weaponPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, IsUplinkingTo, 1, "bool", "WsfPlatform")
{
   WsfPlatform* weaponPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetBool(aObjectPtr->IsUplinkingTo(weaponPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, UplinkCapable, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->UplinkCapable());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, UplinkCount, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->WeaponUplinkCount());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRIPRProcessorClass, WsfRIPRProcessor, UplinkPlatformEntry, 1, "WsfPlatform", "int")
{
   unsigned int index = (unsigned int)aVarArgs[0].GetInt();
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->UplinkWeaponPlatform(index), aReturnClassPtr));
}
