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

#ifndef WSFCOVERAGEMEASUREOUTPUT_HPP
#define WSFCOVERAGEMEASUREOUTPUT_HPP

#include <string>

#include "WsfObject.hpp"

class UtInput;

namespace wsf
{
namespace coverage
{

class Coverage;
class Measure;

//! A class that performs output of the measured data.
class MeasureOutput : public WsfObject
{
public:
   MeasureOutput()                            = default;
   MeasureOutput(const MeasureOutput& aOther) = default;
   ~MeasureOutput() override                  = default;

   // WsfObject overrides
   MeasureOutput* Clone() const override = 0;
   bool           ProcessInput(UtInput& aInput) override;

   // MeasureOutput interface

   //! Write the measured data out.
   //!
   //! The subclass is responsible for implementing this in the way
   //! that produces the output required for that subclass.
   //!
   //! \param aCoverage - The coverage object defining the measure.
   //! \param aMeasure  - The measure to be output.
   virtual void Write(const Coverage& aCoverage, const Measure& aMeasure) = 0;

   //! Set the filename to a default value that is reasonably unique.
   //!
   //! To avoid the user having to manually set the name of every file,
   //! MeasureOutput provides an automatic naming feature. Subclasses
   //! should implement this so that the default names have a reasonable
   //! chance of being unique.
   //!
   //! \param aCoverage - The coverage object defining the measure.
   //! \param aMeasure  - The measure to be output.
   virtual void SetFileNameToDefault(const Coverage& aCoverage, const Measure& aMeasure) = 0;

   //! Return a string identifying the kind of output.
   //!
   //! This will match the identifier used in the input file for this
   //! output type.
   //!
   //! \returns - A string identifier for this output type.
   virtual std::string GetOutputType() const = 0;

   //! Return the file name into which output will be written.
   const std::string& GetFileName() const { return mFileName; }

   //! Return the directory into which any files will be generated.
   const std::string& GetOutputDir() const { return mOutputDir; }

   //! Set the file name into which output will be written.
   void SetFileName(const std::string& aFileName) { mFileName = aFileName; }

   //! Set the directory into which any files will be generated.
   void SetOutputDir(const std::string& aOutputDir) { mOutputDir = aOutputDir; }

private:
   std::string mOutputDir{};
   std::string mFileName{};
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGEMEASUREOUTPUT_HPP
