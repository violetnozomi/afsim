// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UT_BUILDINFO_HPP
#define UT_BUILDINFO_HPP

/// @brief File contains a utility function to build file paths of shared libraries
///    consistent with the convention used in cmake/Modules/swdev_project.cmake.
///    The purpose of this function is to avoid hardcoding of file paths in code
///    which may lead inconsistent naming conventions when switching build architectures.

#include "ut_export.h"

#include <string>

namespace ut
{
namespace BuildInfo
{
/// @brief Given a shared library base name (unqualified by path, prefix, or suffix),
///    generate a path determined by build environment and tests if the library exists.
///
/// @param aSharedLibraryBaseName Name of the shared library,
///    unqualified with prefixes, suffixes, or extensions.
///
/// @returns Path of shared library if it exists; otherwise, it will return an empty
///    string.
std::string UT_EXPORT GetSharedLibraryFilePath(const std::string& aSharedLibraryBaseName);

} // namespace BuildInfo

} // namespace ut

#endif // UT_BUILDINFO_HPP
