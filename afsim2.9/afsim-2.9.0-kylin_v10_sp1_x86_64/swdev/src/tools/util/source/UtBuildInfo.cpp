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

#include "UtBuildInfo.hpp"

#include "UtBuildInfoMacros.hpp"
#include "UtLog.hpp"
#include "UtPath.hpp"

namespace ut
{
namespace BuildInfo
{
namespace
{
enum class CMAKE_Path
{
   SHARED_LIB_PATH,
   EXE_PATH
};

std::string GetRelativeSharedLibraryPath()
{
   const UtPath lib_directory{ut::BuildInfo::cINSTALL_LIB_PATH};
   const UtPath exe_directory{ut::BuildInfo::cINSTALL_EXE_PATH};

   return exe_directory.GetRelativePath(lib_directory).GetNormalizedPath();
}

std::string CreateSharedLibraryFilePath(const std::string& aSharedLibraryBaseName, CMAKE_Path aDirectory)
{
   // Path to executable
   UtPath path = UtPath::GetExePath().Up();

   // In Linux GCC builds, buildPlusPlatform will look like "_ln4m64" in a release
   // build (where "ln4" refers to GCC's major version number, 4 in this case,
   // and "m64" refers to 64-bit vs. 32-bit) and "_d_ln4m64" in a debug build.
   // In Windows builds, this is "" in a release build and "_d" in a debug build.
#ifndef NDEBUG
   std::string buildPlusPlatform = cSWDEV_LIB_DEBUG_POSTFIX;
#else
   std::string          buildPlusPlatform = cSWDEV_LIB_RELEASE_POSTFIX;
#endif

#ifdef _WIN32
   constexpr const char prefix[]    = "";
   constexpr const char extension[] = ".dll";
#else
   constexpr const char prefix[]          = "lib";
   constexpr const char extension[]       = ".so";
#endif
   if (aDirectory == CMAKE_Path::SHARED_LIB_PATH)
   {
      path += GetRelativeSharedLibraryPath();
   }
   path += prefix + aSharedLibraryBaseName + buildPlusPlatform + extension;

   return path.GetSystemPath();
}
} // namespace


std::string UT_EXPORT GetSharedLibraryFilePath(const std::string& aSharedLibraryBaseName)
{
   std::vector<UtPath> search_locations;
   UtPath              candidatePath =
      ut::BuildInfo::CreateSharedLibraryFilePath(aSharedLibraryBaseName, ut::BuildInfo::CMAKE_Path::SHARED_LIB_PATH);
   search_locations.push_back(candidatePath);

   if (candidatePath.IsFile())
   {
      return candidatePath.GetSystemPath();
   }
   else
   {
      // Development builds might not use the INSTALL_LIB_PATH (defined in the
      // CMAKE files) for shared library placement. This attempts to find the
      // shared library where the .exe is located if it failed to locate the
      // library in INSTALL_LIB_PATH
      candidatePath =
         ut::BuildInfo::CreateSharedLibraryFilePath(aSharedLibraryBaseName, ut::BuildInfo::CMAKE_Path::EXE_PATH);
      search_locations.push_back(candidatePath);

      if (candidatePath.IsFile())
      {
         return candidatePath.GetSystemPath();
      }
      else
      {
         auto errorMsg = ut::log::error() << "Could not locate " << aSharedLibraryBaseName;
         errorMsg.AddNote() << "Search locations:";
         for (UtPath& path : search_locations)
         {
            errorMsg.AddNote() << " " << path.Up().GetSystemPath();
         }

         return {};
      }
   }
}

} // namespace BuildInfo

} // namespace ut
