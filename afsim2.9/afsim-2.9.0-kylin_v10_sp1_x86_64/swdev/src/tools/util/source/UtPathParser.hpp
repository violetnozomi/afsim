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

#ifndef UTPATHPARSER_HPP
#define UTPATHPARSER_HPP

#include "ut_export.h"

//----------------------------------------------------------------
// The UtPathParser provides a set of static helper methods that
// are convenient for parsing file paths.
//----------------------------------------------------------------

#include <string>
#include <vector>

namespace UtPathParser
{
// Returns the full path up to, but not including the /filename.
// Additionally, if a path is passed to GetPath one level will
//  be stripped off.
// Returns an empty string if no path exists.
// aFile is the full path with or without the filename.
UT_EXPORT std::string GetPath(const std::string& aFile);

// Returns the filename without the path and file extension.
// Returns an empty string if no filename exists.
// aFile is the full path with filename.
UT_EXPORT std::string GetName(const std::string& aFile);

// Returns the file's extension.
// Returns an empty string if no extension exists.
// aFile is the full path with filename.
UT_EXPORT std::string GetExtension(const std::string& aFile);

// Returns the filename with extension.
// Returns an empty string if no filename and extension exist.
// aFile is the full path with filename.
UT_EXPORT std::string GetNameAndExtension(const std::string& aFile);

// Return true if the file exists, false otherwise.
UT_EXPORT bool FileExists(const std::string& aFile);

//! Packs a vector of pathnames into a single concatenated list.
//! @param aPaths - the container of paths to pack into a single string.
//! @param separator - the separator that will delimit each path.
//! @return the concatenated string of paths.
UT_EXPORT std::string PackPaths(const std::vector<std::string>& aPaths, const std::string& aSeparator);
}; // namespace UtPathParser

#endif
