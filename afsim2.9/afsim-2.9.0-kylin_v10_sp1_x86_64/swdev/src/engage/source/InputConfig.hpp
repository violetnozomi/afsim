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

#ifndef INPUTCONFIG_HPP
#define INPUTCONFIG_HPP

#include <list>

#include "OutputConfig.hpp"
#include "OutputRate.hpp"
#include "RunConfig.hpp"
#include "Simulation.hpp"
#include "Variables.hpp"
#include "WsfScenarioExtension.hpp"

namespace engage
{

class Task;

class InputConfig : public WsfScenarioExtension
{
public:
   InputConfig();
   ~InputConfig() override;

   static InputConfig& FindExtension(WsfScenario& aScenario);

   //! @name Implementation of WsfScenarioExtension base class methods.
   //@{
   bool ProcessInput(UtInput& aInput) override;
   bool Complete() override;
   void SimulationCreated(WsfSimulation& aSimulation) override;
   //@}

   const OutputRate* GetOutputRate(const std::string& aTableName) const;

   int GetThreadCount() const { return mThreadCount; }
   int GetTaskCount() const { return mTaskCount; }

   Task MakeTask(int aTaskIndex) const;

   typedef std::list<RunConfig> RunList;
   //! Return a reference to the list of runs to be performed.
   const RunList& GetRunList() const { return mRunList; }

private:
   typedef std::map<std::string, OutputConfig> OutputTemplates;
   typedef std::map<std::string, OutputRate>   OutputRates;

   int mThreadCount;
   int mTaskCount;

   Variables       mVariables;
   OutputTemplates mOutputTemplates;
   OutputRates     mOutputRates;
   RunList         mRunList;
};

} // namespace engage

#endif
