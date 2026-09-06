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

#include "WsfCoverageMeasureTextOutput.hpp"

#include <fstream>
#include <iomanip>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtPath.hpp"
#include "WsfCoverage.hpp"
#include "WsfCoverageGrid.hpp"
#include "WsfCoverageMeasure.hpp"

namespace wsf
{
namespace coverage
{

MeasureTextOutput::MeasureTextOutput()
   : MeasureOutput()
   , mWriterPtr{ut::make_unique<TextOutputWriter>()}
{
}

MeasureTextOutput::MeasureTextOutput(std::unique_ptr<TextOutputWriter> aWriterPtr)
   : MeasureOutput()
   , mWriterPtr{std::move(aWriterPtr)}
{
}

bool MeasureTextOutput::ProcessInput(UtInput& aInput)
{
   bool        myCommand{true};
   std::string command;
   aInput.GetCommand(command);
   if (command == "precision")
   {
      int precision;
      aInput.ReadValue(precision);
      aInput.ValueGreaterOrEqual(precision, TextOutputWriter::cMIN_PRECISION);
      aInput.ValueLessOrEqual(precision, TextOutputWriter::cMAX_PRECISION);
      mWriterPtr->SetPrecision(precision);
   }
   else if (command == "width")
   {
      int width;
      aInput.ReadValue(width);
      aInput.ValueGreaterOrEqual(width, TextOutputWriter::cMIN_WIDTH);
      aInput.ValueLessOrEqual(width, TextOutputWriter::cMAX_WIDTH);
      mWriterPtr->SetWidth(width);
   }
   else if (command == "justify")
   {
      std::string leftright;
      aInput.ReadCommand(leftright);
      if (leftright == "left")
      {
         mWriterPtr->SetRightJustified(false);
      }
      else if (leftright == "right")
      {
         mWriterPtr->SetRightJustified(true);
      }
      else
      {
         throw UtInput::BadValue{aInput, "Unknown text field justification."};
      }
   }
   else if (command == "format")
   {
      std::string fixsci;
      aInput.ReadCommand(fixsci);
      if (fixsci == "fixed")
      {
         mWriterPtr->SetFixedFormat(true);
      }
      else if (fixsci == "scientific")
      {
         mWriterPtr->SetFixedFormat(false);
      }
      else
      {
         throw UtInput::BadValue{aInput, "Unknown text field format."};
      }
   }
   else
   {
      myCommand = MeasureOutput::ProcessInput(aInput);
   }
   return myCommand;
}

//! Write the measured data to a text file.
//!
//! This method uses the template pattern, calling into operations on the owned
//! TextOutputWriter and on the subclass.
//!
//! \param aCoverage - The coverage defining the given @p aMeasure.
//! \param aMeasure  - The measured data to be written to file.
void MeasureTextOutput::Write(const Coverage& aCoverage, const Measure& aMeasure)
{
   if (mWriterPtr->CreateContainingFolder(GetOutputDir()))
   {
      if (mWriterPtr->OpenFile(GetOutputDir(), GetFileName()))
      {
         mWriterPtr->SetStreamJustify();
         mWriterPtr->SetStreamFormat();
         WriteData(aCoverage, aMeasure);
         mWriterPtr->CloseStream();
      }
   }
}

void MeasureTextOutput::SetFileNameToDefault(const Coverage& aCoverage, const Measure& aMeasure)
{
   // If no name for the file has been set, the name is set based on the
   // grid's name, the measure's name and the specific suffix for the subclass.
   if (GetFileName().empty())
   {
      SetFileName(aCoverage.GetName() + "_" + aMeasure.GetName() + "_" + DefaultFileSuffix());
   }
}

} // namespace coverage
} // namespace wsf
