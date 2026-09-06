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
//*   WsfScriptRIPRProcessorClass.hpp
//*
//* PURPOSE:
//*   The WsfScriptRIPRProcessorClass class inherits from
//*   WsfScriptProcessorClass and exposes the RIPR API to the script input
//*   files.
//*
//* NOTES:
//*   None
//***************************************************************************

#ifndef WSFSCRIPTRIPRPROCESSORCLASS_HPP
#define WSFSCRIPTRIPRPROCESSORCLASS_HPP

#include "script/WsfScriptProcessorClass.hpp"

//! Define script methods for WsfRIPRProcessor.
class WsfScriptRIPRProcessorClass : public WsfScriptProcessorClass
{
public:
   WsfScriptRIPRProcessorClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   // See the base class for a description of Create, Clone, and Destroy.

   void*            Create(const UtScriptContext& aContext) override;
   void*            Clone(void* aObjectPtr) override;
   void             Destroy(void* aObjectPtr) override;
   UtScriptContext* GetContext(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(NumJobChannels);
   UT_DECLARE_SCRIPT_METHOD(SetNumJobChannels);
   UT_DECLARE_SCRIPT_METHOD(JobTypeChannels);
   UT_DECLARE_SCRIPT_METHOD(ChannelJobTypes);
   UT_DECLARE_SCRIPT_METHOD(CommanderProcessor_1);
   UT_DECLARE_SCRIPT_METHOD(CommanderProcessor_2);
   UT_DECLARE_SCRIPT_METHOD(CommanderPlatform_1);
   UT_DECLARE_SCRIPT_METHOD(CommanderPlatform_2);
   UT_DECLARE_SCRIPT_METHOD(SubordinateProcessors_1);
   UT_DECLARE_SCRIPT_METHOD(SubordinateProcessors_2);
   UT_DECLARE_SCRIPT_METHOD(SubordinatePlatforms_1);
   UT_DECLARE_SCRIPT_METHOD(SubordinatePlatforms_2);
   UT_DECLARE_SCRIPT_METHOD(QuerySubordinatesMaxBid_1);
   UT_DECLARE_SCRIPT_METHOD(QuerySubordinatesMaxBid_2);
   UT_DECLARE_SCRIPT_METHOD(AddJob);
   UT_DECLARE_SCRIPT_METHOD(RemoveJob);
   UT_DECLARE_SCRIPT_METHOD(RemoveAllJobs);
   UT_DECLARE_SCRIPT_METHOD(DeleteCompletedJobs);
   UT_DECLARE_SCRIPT_METHOD(PurgeDeadTakers);
   UT_DECLARE_SCRIPT_METHOD(Jobs);
   UT_DECLARE_SCRIPT_METHOD(JobFor_1); // JobFor(double,WsfRIPRProcessor)
   UT_DECLARE_SCRIPT_METHOD(JobFor_2); // JobFor(double,WsfRIPRProcessor,int)
   UT_DECLARE_SCRIPT_METHOD(JobFor_3); // JobFor(WsfRIPRProcessor)
   UT_DECLARE_SCRIPT_METHOD(JobFor_4); // JobFor(WsfRIPRProcessor,int)
   UT_DECLARE_SCRIPT_METHOD(ClearBidsFor_1);
   UT_DECLARE_SCRIPT_METHOD(ClearBidsFor_2);
   UT_DECLARE_SCRIPT_METHOD(GetJobByData);
   UT_DECLARE_SCRIPT_METHOD(GetJobById);
   UT_DECLARE_SCRIPT_METHOD(JobAllocationMode);
   UT_DECLARE_SCRIPT_METHOD(SetJobAllocationMode);
   UT_DECLARE_SCRIPT_METHOD(IsJobWindowOpen);
   UT_DECLARE_SCRIPT_METHOD(IsBidWindowOpen);
   UT_DECLARE_SCRIPT_METHOD(SetJobWindowOpen);
   UT_DECLARE_SCRIPT_METHOD(SetBidWindowOpen);
   UT_DECLARE_SCRIPT_METHOD(SetJobStickiness);
   UT_DECLARE_SCRIPT_METHOD(PurgeTouchTime);
   UT_DECLARE_SCRIPT_METHOD(SetPurgeTouchTime);
   UT_DECLARE_SCRIPT_METHOD(PurgeUntouchedJobs_1);
   UT_DECLARE_SCRIPT_METHOD(PurgeUntouchedJobs_2);

   UT_DECLARE_SCRIPT_METHOD(JobPassThrough);
   UT_DECLARE_SCRIPT_METHOD(SetJobPassThrough);
   UT_DECLARE_SCRIPT_METHOD(QueryBid);
   UT_DECLARE_SCRIPT_METHOD(WallClockTime);

   // weapon/targeting accessors
   UT_DECLARE_SCRIPT_METHOD(WeaponsActive_1);
   UT_DECLARE_SCRIPT_METHOD(WeaponsActive_2);
   UT_DECLARE_SCRIPT_METHOD(WeaponsActiveOfType);
   UT_DECLARE_SCRIPT_METHOD(ActiveWeaponPlatform);
   UT_DECLARE_SCRIPT_METHOD(PeersTargeting);
   UT_DECLARE_SCRIPT_METHOD(PeersWeaponsActive);
   UT_DECLARE_SCRIPT_METHOD(SubsTargeting_1);
   UT_DECLARE_SCRIPT_METHOD(SubsTargeting_2);
   UT_DECLARE_SCRIPT_METHOD(SubsWeaponsActive_1);
   UT_DECLARE_SCRIPT_METHOD(SubsWeaponsActive_2);
   UT_DECLARE_SCRIPT_METHOD(WeaponsIncoming);
   UT_DECLARE_SCRIPT_METHOD(Target); // same as TargetTrack(), legacy
   UT_DECLARE_SCRIPT_METHOD(TargetTrack);
   UT_DECLARE_SCRIPT_METHOD(TargetPlatform);
   UT_DECLARE_SCRIPT_METHOD(TargetName);
   UT_DECLARE_SCRIPT_METHOD(SetTarget_1);
   UT_DECLARE_SCRIPT_METHOD(SetTarget_2);
   UT_DECLARE_SCRIPT_METHOD(SetTarget_3);
   UT_DECLARE_SCRIPT_METHOD(ClearTarget);
   UT_DECLARE_SCRIPT_METHOD(StartUplinking_1);
   UT_DECLARE_SCRIPT_METHOD(StartUplinking_2);
   UT_DECLARE_SCRIPT_METHOD(StopUplinking);
   UT_DECLARE_SCRIPT_METHOD(IsUplinkingTo);
   UT_DECLARE_SCRIPT_METHOD(UplinkCapable);
   UT_DECLARE_SCRIPT_METHOD(UplinkCount);
   UT_DECLARE_SCRIPT_METHOD(UplinkPlatformEntry);

   // these don't go here, put them into platforms/processors that inherit
   UT_DECLARE_SCRIPT_METHOD(WeaponsInFlight);
};

#endif
