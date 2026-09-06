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

#ifndef SOSM_TESTCOMMANDS_HPP
#define SOSM_TESTCOMMANDS_HPP

class UtInput;
class SOSM_Manager;
//! Commands that can be used to test the model.
class SOSM_TestCommands
{
public:
   SOSM_TestCommands(SOSM_Manager* aManagerPtr);
   bool ProcessInput(UtInput& aInput);

private:
   void Test_BlackBodyPlot();
   void Test_SimpleTargetPlot();
   void Test_QueryAtmosphere(UtInput& aInput);
   void Test_QueryTarget(UtInput& aInput);
   void Test_QueryDetect(UtInput& aInput);

   SOSM_Manager* mManagerPtr;
};

#endif
