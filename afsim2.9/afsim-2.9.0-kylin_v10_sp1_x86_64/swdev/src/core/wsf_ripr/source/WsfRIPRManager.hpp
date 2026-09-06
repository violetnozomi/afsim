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
//*   WsfRIPRManager.hpp
//*
//* PURPOSE:
//*   The WsfRIPRManager class holds all the RIPR processor IDs.
//*   Only one instance of this class is needed at a time, so it is
//*   implemented as a singleton for the time being.
//*
//* NOTES:
//*   None
//***************************************************************************

#ifndef WSFRIPRMANAGER_HPP
#define WSFRIPRMANAGER_HPP

#include <map>
#include <vector>

#include "SafeRIPRProc.hpp"
#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
#include "WsfRIPR_Export.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfSimulationExtension.hpp"

class UtInput;
class WsfRIPRProcessor;
class WsfXIO_RIPRExtension;
class WsfEventOutput;
struct WsfRIPRObserver;

class RIPR_EXPORT WsfRIPRManagerData
{
public:
   WsfRIPRManagerData(WsfScenario& aScenario);
   ~WsfRIPRManagerData();
   bool         ProcessRIPR_Input(UtInput& aInput);
   WsfScenario* mScenarioPtr;
};


class RIPR_EXPORT WsfRIPR_ModuleSetup : public WsfScenarioExtension, public WsfRIPRManagerData
{
public:
   WsfRIPR_ModuleSetup(WsfScenario& aScenario);
   ~WsfRIPR_ModuleSetup() override;
   bool ProcessInput(UtInput& aInput) override;

   void SimulationCreated(WsfSimulation& aSimulation) override;

protected:
};


class RIPR_EXPORT WsfRIPRManager : public WsfSimulationExtension
{
public:
   WsfRIPRManager(const WsfRIPRManagerData& aData);

   ~WsfRIPRManager() override;

   //! Returns true if the RIPR kernel is instantiated, false otherwise
   bool IsActive();

   //! Initializes the RIPR XIO callbacks.
   //! Returns true if the type described by the string represents a type
   //! declared in WSF script as being a RIPR processor.
   bool IsRiprType(WsfStringId aType) const;


   unsigned int      GetUniqueRiprId(WsfRIPRProcessor* aProcPtr = nullptr);
   WsfRIPRProcessor* GetRiprProcById(unsigned int aId);
   void              RemoveRiprProc(unsigned int aId);

   //! Returns a copy of the RIPR unique ID to processor ID map.
   std::map<unsigned int, SafeRIPRProc> GetProcIdMap() const { return mIdToProcMap; }

   WsfRIPRObserver& GetObserver() const { return *mObserverPtr; }

private:
   WsfRIPRObserver*                     mObserverPtr;
   unsigned int                         mNextUniqueId;
   const WsfRIPRManagerData*            mShare;
   WsfXIO_RIPRExtension*                mRIPR_XIOPtr;
   UtCallbackHolder                     mCallbacks;
   WsfEventOutput*                      mEventOutputPtr;
   std::map<unsigned int, SafeRIPRProc> mIdToProcMap;

   std::vector<WsfStringId> mRiprTypeList;
};

#endif
