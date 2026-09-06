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
//*   SafeRIPRProc.cpp
//*
//* PURPOSE:
//*    SafeRIPRProc provides safe pointers to WsfRIPRProcessor objects
//*
//* NOTES:
//*   None
//***************************************************************************

#include "SafeRIPRProc.hpp"

#include "WsfPlatform.hpp"
#include "WsfRIPRProcessor.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"

//! Constructor that uses a WsfRIPRProcessor to build the safe pointer
//! @param aProc the WsfRIPRProcessor to point to
SafeRIPRProc::SafeRIPRProc(WsfRIPRProcessor* aProc)
{
   mPlatformIndex        = 0;
   WsfPlatform* ownerPtr = aProc->GetPlatform();
   if (ownerPtr)
   {
      mPlatformIndex = ownerPtr->GetIndex();
   }
   mProcessorIndex = aProc->GetNameId();
   mSimulationPtr  = aProc->GetSimulation();
   assert(mSimulationPtr);
}

//! Constructor that uses a WsfRIPRProcessor to build the safe pointer
//! @param aProc the WsfRIPRProcessor to point to
SafeRIPRProc::SafeRIPRProc(const WsfRIPRProcessor& aProc)
{
   mPlatformIndex        = 0;
   WsfPlatform* ownerPtr = aProc.GetPlatform();
   if (ownerPtr)
   {
      mPlatformIndex = ownerPtr->GetIndex();
   }
   mProcessorIndex = aProc.GetNameId();
   mSimulationPtr  = aProc.GetSimulation();
   assert(mSimulationPtr);
}

//! Constructor that creates a null safe pointer
SafeRIPRProc::SafeRIPRProc()
{
   mPlatformIndex  = 0;
   mProcessorIndex = nullptr;
   mSimulationPtr  = nullptr;
}

//! Returns a WsfRIPRProcessor* if the pointee exists, 0 otherwise
WsfRIPRProcessor* SafeRIPRProc::get() const
{
   if (mPlatformIndex == 0)
   {
      return nullptr;
   }
   WsfRIPRProcessor* procPtr = nullptr;
   WsfPlatform*      platPtr = mSimulationPtr->GetPlatformByIndex(mPlatformIndex);
   if (platPtr)
   {
      procPtr = dynamic_cast<WsfRIPRProcessor*>(platPtr->GetComponent<WsfProcessor>(mProcessorIndex));
   }
   return procPtr;
}

//! Returns the name of the pointee if it exists, 0 otherwise
std::string SafeRIPRProc::GetName() const
{
   std::string procName;
   if (mPlatformIndex == 0)
   {
      return procName;
   }
   WsfPlatform* platPtr = mSimulationPtr->GetPlatformByIndex(mPlatformIndex);
   if (platPtr)
   {
      WsfRIPRProcessor* procPtr = dynamic_cast<WsfRIPRProcessor*>(platPtr->GetComponent<WsfProcessor>(mProcessorIndex));
      if (procPtr)
      {
         procName = procPtr->GetName();
      }
   }
   return procName;
}

//! Compare safe pointers
//! @return true iff two SafeRIPRProcs point to the same platform index and processor index
bool SafeRIPRProc::operator==(const SafeRIPRProc& other) const
{
   return (!((mPlatformIndex != other.mPlatformIndex) || (mProcessorIndex != other.mProcessorIndex)));
}

//! Compare safe pointers
//! @return !operator==
bool SafeRIPRProc::operator!=(const SafeRIPRProc& other) const
{
   return !(*this == other);
}

//! Produces a consistant ordering of SafeRIPRProc objects
bool SafeRIPRProc::operator<(const SafeRIPRProc& other) const
{
   return ((mPlatformIndex < other.mPlatformIndex) ||
           (mPlatformIndex == other.mPlatformIndex && mProcessorIndex < other.mProcessorIndex));
}
