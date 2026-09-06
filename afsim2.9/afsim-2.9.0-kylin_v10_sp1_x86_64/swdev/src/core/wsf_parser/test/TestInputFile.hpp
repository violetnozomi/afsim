// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef TEST_INPUT_FILE_HPP
#define TEST_INPUT_FILE_HPP

#include <string>

#include "UtPath.hpp"

namespace wsf
{
namespace parser
{
namespace test
{

//! RAII object representing an input file
//! @par details
//!   On construction, this creates a file on the filesystem.
//!   On destruction, this removes the file from the filesystem.
//! @note This does not store the contents of the file
class InputFile final
{
public:
   //! Contains data representing an InputFile
   //! @note If you need to create multiple InputFiles at once, store multiple Data in a container
   struct Data final
   {
      //! The file path prefix
      UtPath mPathPrefix;
      //! The file name
      std::string mName;
      //! The file contents
      std::string mContents;
   }; // struct Data

   //! Construct an InputFile
   //! @param aPath is the file path prefix
   //! @param aName is the file name
   //! @param aContents is the file contents
   //! @pre a file at mPath containing aContents does not exist
   //! @post a file at mPath containing aContents does exist
   InputFile(const UtPath& aPathPrefix, const std::string& aName, const std::string& aContents);

   //! Prevent copy-construction of InputFiles
   InputFile(const InputFile& aSrc) = delete;
   //! Prevent move-construction of InputFiles
   InputFile(InputFile&& aSrc) = delete;

   //! Destructs an InputFile
   //! @pre a file at mPath does exist
   //! @post a file at mPath does not exist
   ~InputFile();

   //! Prevent copy-assignment of InputFiles
   InputFile& operator=(const InputFile& aSrc) = delete;
   //! Prevent move-assignment of InputFiles
   InputFile& operator=(InputFile&& aSrc) = delete;

   //! Get the path prefix
   //! @return the path prefix
   UtPath GetPathPrefix() const noexcept;
   //! Get the file name
   //! @return the file name
   std::string GetName() const noexcept;

   //! Get the file path
   //! @return the file path
   //! @note file path = path prefix + file name
   UtPath GetPath() const noexcept;
   //! Get the file path as a string
   //! @return the file path as a string
   //! @note file path = path prefix + file name
   std::string GetPathString() const noexcept;

private:
   //! Creates the file in the current working directory
   //! @param aContents is the file contents
   void WriteSource(const std::string& aContents);

   //! Removes the file from the current working directory
   void RemoveSource();

   //! The file path prefix
   UtPath mPathPrefix;
   //! The file name
   std::string mName;
}; // class InputFile

} // namespace test
} // namespace parser
} // namespace wsf

#endif // TEST_INPUT_FILE_HPP
