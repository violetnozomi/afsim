// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCoverageMeasure.hpp"

#include <set>
#include <string>
#include <utility>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "WsfCoverage.hpp"
#include "WsfCoverageGrid.hpp"
#include "WsfCoverageMeasureOutputTypes.hpp"

namespace wsf
{
namespace coverage
{

bool Measure::ProcessInput(UtInput& aInput)
{
   bool        myCommand{true};
   std::string command;
   aInput.GetCommand(command);
   if (command == "output")
   {
      UtInputBlock block{aInput};
      std::string  outputTypeName;
      if (!block.ReadCommand(outputTypeName))
      {
         throw UtInput::BadValue{aInput, "Expected output type."};
      }
      auto outputTypePtr = MeasureOutputTypes::Get(mScenario).Find(outputTypeName);
      if (!outputTypePtr)
      {
         throw UtInput::BadValue{aInput, "Unrecognized output type."};
      }
      auto outputPtr = ut::clone(outputTypePtr);
      block.ProcessInput(outputPtr.get());
      mOutputs.emplace_back(std::move(outputPtr));
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

//! Initialize this Measure.
//!
//! In addition to propagating the output directory from @p aCoverage to
//! this objects output, this will assure that the output will produce files
//! with unique names among the outputs for this Measure. Finally, this calls
//! into the subclasses intialization method, in which it is expected that the
//! subclass will connect to the interval events on @p aCoverage.
//!
//! \param aCoverage - The coverage owning this.
//! \return          - true if initialization is successful; false otherwise.
bool Measure::Initialize(Coverage& aCoverage)
{
   // Set output directories and default file names as needed.
   for (auto& output : mOutputs)
   {
      output->SetOutputDir(aCoverage.GetOutputDir());
      output->SetFileNameToDefault(aCoverage, *this);
   }

   bool                  retval{true};
   std::set<std::string> filenames{};
   for (auto& output : mOutputs)
   {
      auto iter = filenames.find(output->GetFileName());
      if (iter != filenames.end())
      {
         auto err = ut::log::error() << "Output file name used multiple times; some output would be lost.";
         err.AddNote() << "MOE output of type '" << output->GetOutputType() << "'";
         err.AddNote() << "On measure with name '" << GetName() << "'";
         err.AddNote() << "On coverage with name '" << aCoverage.GetName() << "'";
         retval = false;
         break;
      }
      else
      {
         filenames.emplace(output->GetFileName());
      }
   }

   if (retval)
   {
      retval = InitializeP(aCoverage);
   }
   return retval;
}

//! Tack any action needed at the end of a coverage collection interval.
//!
//! In addition to allowing the subclass to perform any subclass-specific
//! tasks, this will cause all the output objects to produce their output.
//!
//! \param aCoverage - The coverage owning this object.
//! \param aSimTime  - The sim time of the end of collection.
void Measure::CollectionCompleting(Coverage& aCoverage, double aSimTime)
{
   CollectionCompletingP(aCoverage, aSimTime);

   for (auto& output : mOutputs)
   {
      output->Write(aCoverage, *this);
   }
}

} // namespace coverage
} // namespace wsf
