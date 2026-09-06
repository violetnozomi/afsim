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

#include "WsfCoverageTextOutputWriter.hpp"

#include "UtLog.hpp"
#include "UtPath.hpp"

namespace wsf
{
namespace coverage
{

TextOutputWriter::TextOutputWriter(const TextOutputWriter& aOther)
   : mPrecision{aOther.mPrecision}
   , mWidth{aOther.mWidth}
   , mRightJustify{aOther.mRightJustify}
   , mFixedFormat{aOther.mFixedFormat}
   , mStream{}
{
}

//! Open the file stream to write to the given @p aFileName.
bool TextOutputWriter::OpenStream(const std::string& aFileName)
{
   mStream.open(aFileName);
   return mStream.good();
}

//! Close the file stream.
void TextOutputWriter::CloseStream()
{
   mStream.close();
}

//! Create the path up to the containing folder.
//!
//! This will ensure that all folders exist in the given @p aPath.
//! This method returns true if the folder is successfully created,
//! or if the folder already exists.
//!
//! \param aPath - The folder whose existence is to be ensured.
//! \returns     - true is successful; false otherwise.
bool TextOutputWriter::CreateContainingFolder(const std::string& aPath) const
{
   UtPath path{aPath};
   path.MakeFullPath();
   bool retval = path.Mkdir();
   if (!retval)
   {
      auto err = ut::log::error() << "Unable to create output directory.";
      err.AddNote() << "Attempting to create directory: " << path.GetSystemPath();
   }
   return retval;
}

//! Open the file.
//!
//! This is a template pattern method that calls into the appropriate
//! members of the interface in the correct order.
bool TextOutputWriter::OpenFile(const std::string& aOutputDir, const std::string& aFileName)
{
   UtPath path{aOutputDir};
   path += aFileName;
   bool retval = OpenStream(path.GetNormalizedPath());
   if (!retval)
   {
      auto err = ut::log::error() << "Unable to open output file.";
      err.AddNote() << "File: " << path.GetSystemPath();
   }
   return retval;
}

//! Set the configured justification on the stream.
void TextOutputWriter::SetStreamJustify()
{
   if (mRightJustify)
   {
      GetStream() << std::right;
   }
   else
   {
      GetStream() << std::left;
   }
}

//! Set the configured format on the stream.
void TextOutputWriter::SetStreamFormat()
{
   if (mFixedFormat)
   {
      GetStream() << std::fixed;
   }
   else
   {
      GetStream() << std::scientific;
   }
}

} // namespace coverage
} // namespace wsf
