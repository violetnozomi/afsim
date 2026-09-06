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

#include "WsfDE_FileManager.hpp"

#include "UtPath.hpp"
#include "WsfSimulation.hpp"

//! Provide static access to the extension for the given @p aSimulationPtr.
WsfDE_FileManager& WsfDE_FileManager::Get(const WsfSimulation* aSimulationPtr)
{
   return static_cast<WsfDE_FileManager&>(aSimulationPtr->GetExtension(cEXTENSION_NAME));
}

//! Get a handle to a JPL Development Ephemeris File
//!
//! This method will return a new handle to the DE file with the given name. If there is
//! a problem loading the file, this will throw a WsfDE_File::Error with information about
//! the error. If a file with the given @p aFileName is already loaded, this will not
//! reload the file, but merely return a new handle to that file.
//!
//! \param aFileName - The name of a file for which a handle will be returned.
//! \returns         - A handle to the named file.
std::unique_ptr<WsfDE_FileHandle> WsfDE_FileManager::GetOrLoadFile(const std::string& aFileName)
{
   UtPath path(aFileName);
   path.MakeFullPath();
   auto iter = mFiles.find(path.GetNormalizedPath());
   if (iter == mFiles.end())
   {
      // No such file loaded, so create one and add it to the list.
      auto file   = ut::make_unique<WsfDE_File>(path.GetNormalizedPath());
      auto result = mFiles.emplace(path.GetNormalizedPath(), std::move(file));
      if (result.second)
      {
         iter = result.first;
      }
   }
   return ut::make_unique<WsfDE_FileHandle>(*iter->second);
}
