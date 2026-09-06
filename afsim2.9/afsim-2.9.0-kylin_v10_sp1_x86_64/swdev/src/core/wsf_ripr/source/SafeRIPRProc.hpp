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
//*   SafeRIPRProc.hpp
//*
//* PURPOSE:
//*    SafeRIPRProc provides safe pointers to WsfRIPRProcessor objects
//*
//* NOTES:
//*   None
//***************************************************************************

#ifndef WSF_RIPR_SAFE_RIPR_PROC_HPP
#define WSF_RIPR_SAFE_RIPR_PROC_HPP

#include <string>

#include "WsfStringId.hpp"
class WsfRIPRProcessor;
class WsfSimulation;
//! SafeRIPRProc provides safe pointers to WsfRIPRProcessor objects
class SafeRIPRProc
{
public:
   SafeRIPRProc();
   SafeRIPRProc(WsfRIPRProcessor* aProc);
   SafeRIPRProc(const WsfRIPRProcessor& aProc);
   WsfRIPRProcessor* get() const;
   std::string       GetName() const;
   bool              operator==(const SafeRIPRProc& other) const;
   bool              operator!=(const SafeRIPRProc& other) const;
   bool              operator<(const SafeRIPRProc& other) const;

private:
   WsfSimulation* mSimulationPtr;
   //! The platform index of the WsfRIPRProcessor that this points to
   size_t mPlatformIndex;

   //! The NameId of the WsfRIPRProcessor that this points to
   WsfStringId mProcessorIndex;
};

#endif
